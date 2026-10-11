// SPDX-License-Identifier: Apache-2.0
#include "pico/stdlib.h"
#include "tusb.h"
#include "psx_transport.h"
#include "lab_bus.h"
#include <string.h>
static psx_state_t state;
static uint8_t latest_raw[64];
static modi_gamepad_t latest_gamepad;
int main(void) {
    psx_state_init(&state); lab_init(); psx_transport_init(); tud_init(0);
    uint32_t last_sequence=UINT32_MAX;
    bool raw_pending=false, gamepad_pending=false;
    for(;;) {
        tud_task();
        psx_transport_task(&state);
        if(state.sequence!=last_sequence) {
            last_sequence=state.sequence;
            psx_raw_report(&state,time_us_32(),latest_raw);
            psx_gamepad_report(&state,&latest_gamepad);
            raw_pending=gamepad_pending=true;
        }
        if(tud_mounted() && !tud_suspended()) {
            if(gamepad_pending && tud_hid_n_ready(0))
                if(tud_hid_n_report(0,0,&latest_gamepad,sizeof latest_gamepad)) {
                    gamepad_pending=false;
                    lab_usb_sent((const uint8_t*)&latest_gamepad,last_sequence,time_us_32());
                }
            if(raw_pending && tud_hid_n_ready(1))
                if(tud_hid_n_report(1,0,latest_raw,sizeof latest_raw)) raw_pending=false;
        }
        tight_loop_contents();
    }
}
uint16_t tud_hid_get_report_cb(uint8_t instance,uint8_t report_id,hid_report_type_t type,
                             uint8_t *buffer,uint16_t reqlen) {
    if(report_id) return 0;
    if(instance==1 && type==HID_REPORT_TYPE_FEATURE && reqlen>=64) {
        lab_feature(buffer,time_us_32());return 64;
    }
    if(type!=HID_REPORT_TYPE_INPUT) return 0;
    const void *data=instance==1?(const void*)latest_raw:(const void*)&latest_gamepad;
    uint16_t size=instance==1?64:sizeof latest_gamepad;
    if(size>reqlen) size=reqlen;
    memcpy(buffer,data,size); return size;
}
void tud_hid_set_report_cb(uint8_t instance,uint8_t report_id,hid_report_type_t type,
                          const uint8_t *buffer,uint16_t size) {
    if(instance==1 && !report_id && type==HID_REPORT_TYPE_OUTPUT)
        lab_output(buffer,size,time_us_32());
}
