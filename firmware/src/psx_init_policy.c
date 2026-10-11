// SPDX-License-Identifier: Apache-2.0
#include "psx_init_policy.h"
#include <string.h>
void psx_init_policy_reset(psx_init_policy_t *p){memset(p,0,sizeof *p);}
void psx_init_policy_query(psx_init_policy_t *p,const uint8_t *rx,unsigned n){
    // 45 reports a protocol model, not a physical SCPH or authenticity.
    if(rx && n>=9 && (rx[0]==0 || rx[0]==255) && rx[1]==0xF3 && rx[2]==0x5A &&
       (rx[3]==1 || rx[3]==3))p->model=rx[3];
}
void psx_init_policy_observe(psx_init_policy_t *p,const psx_state_t *s){
    if(!s->connected || !(s->sample.capabilities&CAP_VALID) ||
       (s->sample.capabilities&CAP_CONFIG))return;
    if(s->sample.id==0x79)p->complete=true;
    if(!p->awaiting_poll || (int32_t)(s->last_valid_us-p->cycle_finished)<=0)return;
    p->awaiting_poll=false;
    // A usable analog response is the end of DS1/unknown-model setup. Only a
    // pad declaring DS2 model 3 needs the repeated pressure upgrade attempts.
    if(s->sample.id==0x53 || (s->sample.id==0x73 && p->model!=3) ||
       p->attempts>=50 || (s->sample.id==0x41 && p->model!=3 && p->attempts>=3))
        p->complete=true;
}
bool psx_init_policy_begin(psx_init_policy_t *p){
    if(p->complete || p->awaiting_poll || p->attempts>=50)return false;
    ++p->attempts;return true;
}
void psx_init_policy_end(psx_init_policy_t *p,uint32_t now){
    p->awaiting_poll=true;p->cycle_finished=now;
}
bool psx_init_policy_pressure(const psx_init_policy_t *p,uint8_t id){
    return p->model!=1 && id!=0x53;
}
