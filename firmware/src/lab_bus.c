// SPDX-License-Identifier: Apache-2.0
#include "lab_bus.h"
#include <string.h>
static lab_request_t queue[LAB_QUEUE];
static uint8_t traces[LAB_TRACE][64], usb[7];
static unsigned qhead,qcount,thead,tcount;
static uint8_t flags;
static uint32_t generation,sequence,dropped,usb_sequence,usb_time;
static bool usb_valid;
static void u16(uint8_t *p,uint16_t v){p[0]=v;p[1]=v>>8;}
static void u32(uint8_t *p,uint32_t v){for(unsigned i=0;i<4;i++)p[i]=v>>(8*i);}
static void header(uint8_t p[64],uint8_t kind,uint16_t id,uint32_t now){
    memset(p,0,64);memcpy(p,"MDV2",4);p[4]=2;p[5]=kind;u16(p+6,id);
    u32(p+8,++sequence);u32(p+12,now);p[19]=0x80; // ACK wire is not connected/measured.
}
static void push(const uint8_t p[64]){
    if(tcount==LAB_TRACE){thead=(thead+1)%LAB_TRACE;--tcount;++dropped;}
    memcpy(traces[(thead+tcount)%LAB_TRACE],p,64);++tcount;
}
static void status(uint8_t p[64],uint16_t id,uint8_t code,uint32_t now){
    header(p,2,id,now);p[18]=code;p[19]|=LAB_CONTROL;
    u32(p+22,dropped);p[26]=qcount;p[27]=flags;u32(p+28,generation);
}
void lab_init(void){qhead=qcount=thead=tcount=0;flags=7;generation=sequence=dropped=0;usb_valid=false;}
unsigned lab_queued(void){return qcount;}
uint8_t lab_flags(void){return flags;}
uint32_t lab_generation(void){return generation;}
bool lab_enqueue(const lab_request_t *r){
    if(qcount==LAB_QUEUE)return false;
    queue[(qhead+qcount)%LAB_QUEUE]=*r;++qcount;return true;
}
bool lab_take(lab_request_t *r){
    if(!qcount)return false;
    *r=queue[qhead];qhead=(qhead+1)%LAB_QUEUE;--qcount;return true;
}
void lab_complete(const lab_request_t *r,const uint8_t *rx,unsigned len,uint8_t code,uint32_t start,uint32_t end){
    if(!(flags&4) && r->origin!=LAB_MANUAL)return;
    uint8_t p[64];header(p,1,r->id,start);
    uint32_t elapsed=end-start;u16(p+16,elapsed>65535?65535:elapsed);
    p[18]=code;p[19]|=r->origin;p[20]=r->length;p[21]=len>21?21:len;
    memcpy(p+22,r->tx,r->length);if(rx)memcpy(p+43,rx,p[21]);push(p);
}
static bool known(uint8_t cmd){
    return cmd==0x40||cmd==0x41||cmd==0x42||cmd==0x43||cmd==0x44||cmd==0x45||cmd==0x46||cmd==0x47||cmd==0x4C||cmd==0x4D||cmd==0x4F;
}
void lab_output(const uint8_t *p,unsigned n,uint32_t now){
    uint8_t out[64];uint16_t id=n>=8?(uint16_t)(p[6]|p[7]<<8):0;
    if(n!=64||memcmp(p,"MDC2",4)||p[4]!=2){status(out,id,LAB_BAD,now);push(out);return;}
    if(p[5]==2){
        if(p[8]&~7u){status(out,id,LAB_BAD,now);push(out);return;}
        flags=p[8];++generation;
        lab_request_t r;
        while(lab_take(&r))lab_complete(&r,NULL,0,LAB_CANCEL,now,now);
        status(out,id,LAB_OK,now);push(out);return;
    }
    if(p[5]==3){
        header(out,3,id,now);memcpy(out+22,usb,7);u32(out+29,usb_sequence);u32(out+33,usb_time);out[37]=usb_valid;push(out);return;
    }
    if(p[5]!=1||p[8]<3||p[8]>21||p[9]<2||p[9]>50||p[11]!=1||(!known(p[12])&&!(p[10]&1))){
        status(out,id,LAB_BAD,now);push(out);return;
    }
    lab_request_t r={.id=id,.origin=LAB_MANUAL,.length=p[8],.timeout_ms=p[9]};
    memcpy(r.tx,p+11,r.length);
    if(!lab_enqueue(&r)){status(out,id,LAB_FULL,now);push(out);}
}
void lab_feature(uint8_t p[64],uint32_t now){
    if(tcount){memcpy(p,traces[thead],64);thead=(thead+1)%LAB_TRACE;--tcount;}
    else{status(p,0,LAB_OK,now);p[5]=0;}
}
void lab_usb_sent(const uint8_t p[7],uint32_t seq,uint32_t now){
    memcpy(usb,p,7);usb_sequence=seq;usb_time=now;usb_valid=true;
}
