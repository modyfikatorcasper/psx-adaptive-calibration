// SPDX-License-Identifier: Apache-2.0
// Derived from Joypad OS psx_host.c, bf507bf87aee18e24ea82f84647cd592a7a2ddaf.
// Modified for Modi: no router, no feedback, cooperative config, DMA polling,
// external passive DAT pull-up, strict parser, timeout recovery, raw reports.
#include "psx_transport.h"
#include "lab_bus.h"
#include "psx_init_policy.h"
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/dma.h"
#include "hardware/clocks.h"
#include "tusb.h"
#include "psx.pio.h"

enum { pin_clk=10, pin_att=11, pin_cmd=12, pin_dat=13, PSX_HW_BYTES=21 };
static PIO psx_pio=pio0;
static uint psx_sm, program_offset;
static int psx_dma;
static uint32_t psx_rx[PSX_HW_BYTES];
static bool busy, was_connected;
static uint32_t started_us, next_poll_us, settle_us, next_config_us, finished_us;
static uint8_t config_step;
static psx_init_policy_t init_policy;
static lab_request_t active;
static uint32_t active_generation, observed_generation;
static bool due(uint32_t now,uint32_t target){return (int32_t)(now-target)>=0;}

// Joypad OS slow configuration timings. Service USB between bytes.
static uint8_t psx_bb_byte(uint8_t out) {
    uint8_t in=0;
    for(int i=0;i<8;i++) {
        gpio_put(pin_cmd,(out>>i)&1);
        busy_wait_us(8); gpio_put(pin_clk,0); busy_wait_us(14);
        if(gpio_get(pin_dat)) in|=(1u<<i);
        gpio_put(pin_clk,1); busy_wait_us(14);
    }
    gpio_put(pin_cmd,1); busy_wait_us(250); tud_task(); return in;
}
static void sio_pins(void) {
    gpio_put(pin_clk,1); gpio_put(pin_cmd,1); gpio_put(pin_att,1);
    gpio_set_dir(pin_clk,true); gpio_set_dir(pin_cmd,true); gpio_set_dir(pin_att,true);
    gpio_set_dir(pin_dat,false);
    gpio_set_function(pin_clk,GPIO_FUNC_SIO); gpio_set_function(pin_att,GPIO_FUNC_SIO);
    gpio_set_function(pin_cmd,GPIO_FUNC_SIO); gpio_set_function(pin_dat,GPIO_FUNC_SIO);
}
static void restore_pio_pins(void) {
    pio_gpio_init(psx_pio,pin_clk); pio_gpio_init(psx_pio,pin_att);
    pio_gpio_init(psx_pio,pin_cmd); pio_gpio_init(psx_pio,pin_dat);
    pio_sm_set_consecutive_pindirs(psx_pio,psx_sm,pin_clk,2,true);
    pio_sm_set_consecutive_pindirs(psx_pio,psx_sm,pin_cmd,1,true);
    pio_sm_set_consecutive_pindirs(psx_pio,psx_sm,pin_dat,1,false);
}
// One command per task, bounded retries. Unsupported commands cannot hang.
static void queue_config(void) {
    static const uint8_t enter[]={1,0x43,0,1,0};
    static const uint8_t type[]={1,0x45,0,0x5A,0x5A,0x5A,0x5A,0x5A,0x5A};
    static const uint8_t mode[]={1,0x44,0,1,0,0,0,0,0};
    static const uint8_t rumble[]={1,0x4D,0,0,1};
    static const uint8_t pressure[]={1,0x4F,0,0xFF,0xFF,3,0,0,0};
    static const uint8_t exit[]={1,0x43,0,0,0x5A,0x5A,0x5A,0x5A,0x5A};
    const uint8_t *commands[]={enter,type,mode,rumble,pressure,pressure,pressure,exit,exit};
    const uint8_t lengths[]={5,9,9,5,9,9,9,9,9};
    lab_request_t r={.origin=LAB_AUTO,.length=lengths[config_step-1],.timeout_ms=20};
    for(unsigned i=0;i<r.length;i++)r.tx[i]=commands[config_step-1][i];
    lab_enqueue(&r);
}
static uint8_t response_status(const uint8_t *f,unsigned n){
    return psx_response_header_valid(f,n)?LAB_OK:LAB_HEADER;
}
static void complete(psx_state_t *state,const uint8_t *rx,unsigned len,uint8_t status,uint32_t end){
    if(active_generation!=lab_generation())status=LAB_CANCEL;
    lab_complete(&active,rx,len,status,started_us,end);
    if(active.tx[1]==0x42 && status!=LAB_CANCEL)
        psx_state_feed(state,rx,len,end);
    if(active.origin==LAB_AUTO && status!=LAB_CANCEL){
        if(config_step==2 && status==LAB_OK)psx_init_policy_query(&init_policy,rx,len);
        if(config_step==4 && !psx_init_policy_pressure(&init_policy,state->sample.id))config_step=7;
        if(++config_step>9){config_step=0;next_config_us=end+100000;psx_init_policy_end(&init_policy,end);}
        else next_config_us=end+1000;
        next_poll_us=end+1500;
    }
    finished_us=end;
}
static void abort_dma(void){
    dma_channel_abort(psx_dma);pio_sm_set_enabled(psx_pio,psx_sm,false);
    pio_sm_clear_fifos(psx_pio,psx_sm);pio_sm_restart(psx_pio,psx_sm);
    sio_pins();busy_wait_us(50);restore_pio_pins();
    pio_sm_exec(psx_pio,psx_sm,pio_encode_jmp(program_offset));
    pio_sm_set_enabled(psx_pio,psx_sm,true);busy=false;
}
static void execute_bb(psx_state_t *state){
    uint8_t rx[21];unsigned length=0;uint8_t status=LAB_OK;
    sio_pins();busy_wait_us(100);gpio_put(pin_att,0);busy_wait_us(14);
    for(unsigned i=0;i<active.length;i++){
        if(lab_generation()!=active_generation){status=LAB_CANCEL;break;}
        if((uint32_t)(time_us_32()-started_us)>active.timeout_ms*1000u){status=LAB_TIMEOUT;break;}
        rx[length++]=psx_bb_byte(active.tx[i]);
    }
    gpio_put(pin_att,1);busy_wait_us(600);restore_pio_pins();
    if(status==LAB_OK)status=response_status(rx,length);
    complete(state,rx,length,status,time_us_32());
}
void psx_transport_init(void) {
    gpio_init(pin_clk); gpio_init(pin_att); gpio_init(pin_cmd); gpio_init(pin_dat);
    sio_pins(); gpio_pull_up(pin_dat);
    program_offset=pio_add_program(psx_pio,&psx_program);
    psx_sm=pio_claim_unused_sm(psx_pio,true);
    pio_sm_config c=psx_program_get_default_config(program_offset);
    sm_config_set_sideset_pins(&c,pin_clk);
    sm_config_set_set_pins(&c,pin_dat,1);
    sm_config_set_out_pins(&c,pin_cmd,1); sm_config_set_in_pins(&c,pin_dat);
    sm_config_set_out_shift(&c,true,false,32); sm_config_set_in_shift(&c,true,true,8);
    sm_config_set_clkdiv(&c,(float)clock_get_hz(clk_sys)/500000.0f);
    pio_sm_init(psx_pio,psx_sm,program_offset,&c); restore_pio_pins();
    pio_sm_set_enabled(psx_pio,psx_sm,true);
    psx_dma=dma_claim_unused_channel(true);
    dma_channel_config dc=dma_channel_get_default_config(psx_dma);
    channel_config_set_transfer_data_size(&dc,DMA_SIZE_32);
    channel_config_set_read_increment(&dc,false); channel_config_set_write_increment(&dc,true);
    channel_config_set_dreq(&dc,pio_get_dreq(psx_pio,psx_sm,false));
    dma_channel_configure(psx_dma,&dc,psx_rx,&psx_pio->rxf[psx_sm],PSX_HW_BYTES,false);
    next_poll_us=time_us_32();
}
void psx_transport_task(psx_state_t *state) {
    uint32_t now=time_us_32();
    if(observed_generation!=lab_generation()){
        observed_generation=lab_generation();config_step=0;psx_init_policy_reset(&init_policy);
        settle_us=next_config_us=now+50000;
        if(!(lab_flags()&1)){state->sample.capabilities&=(uint8_t)~CAP_VALID;++state->sequence;}
    }
    if(busy) {
        if(active_generation!=lab_generation()){
            abort_dma();complete(state,NULL,0,LAB_CANCEL,time_us_32());
        } else if(!dma_channel_is_busy(psx_dma)) {
            uint8_t frame[PSX_HW_BYTES];
            for(unsigned i=0;i<PSX_HW_BYTES;i++)frame[i]=(uint8_t)(psx_rx[i]>>24);
            complete(state,frame,sizeof frame,response_status(frame,sizeof frame),now);
            busy=false;
        } else if((uint32_t)(now-started_us)>active.timeout_ms*1000u) {
            // Bounded recovery if a FIFO/DMA stalls. Restore idle and restart PIO.
            abort_dma();complete(state,NULL,0,LAB_TIMEOUT,time_us_32());
            next_poll_us=now+PSX_POLL_US;
        }
    }
    if(state->connected!=was_connected) {
        was_connected=state->connected;psx_init_policy_reset(&init_policy);config_step=0;
        settle_us=now+300000;next_config_us=settle_us;
    }
    if(busy || (uint32_t)(now-finished_us)<40)return;
    psx_init_policy_observe(&init_policy,state);
    if(!lab_queued() && (lab_flags()&2)) {
        if(config_step) {
            if(due(now,next_config_us))queue_config();
        } else {
        uint8_t id=state->sample.id;
        if(state->connected && due(now,settle_us) && due(now,next_config_us) &&
           (id==0x41 || id==0x73 || id==0x53 || id==0xF3) && psx_init_policy_begin(&init_policy)) {
            config_step=1;queue_config();
        }
        // Recovery still exits F3 after setup, without forcing analog again.
        else if(state->connected && id==0xF3 && due(now,next_config_us)) {
            config_step=8;queue_config();
        }
        }
    }
    if(!lab_queued() && !config_step && (lab_flags()&1) && due(now,next_poll_us)) {
        lab_request_t r={.origin=LAB_POLL,.length=21,.timeout_ms=10,.tx={1,0x42}};
        lab_enqueue(&r);next_poll_us=now+PSX_POLL_US;
    }
    if(lab_take(&active)){
        active_generation=lab_generation();started_us=time_us_32();
        if(active.origin!=LAB_POLL){execute_bb(state);return;}
        busy=true;
        dma_channel_set_write_addr(psx_dma,psx_rx,false);
        dma_channel_set_trans_count(psx_dma,PSX_HW_BYTES,true);
        pio_sm_put(psx_pio,psx_sm,0x00004201u); // motors always zero
    }
}

