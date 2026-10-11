// SPDX-License-Identifier: Apache-2.0
#include "psx_parser.h"
#include <string.h>
static void le32(uint8_t *p, uint32_t v) {
    for (unsigned i=0; i<4; ++i) p[i]=(uint8_t)(v>>(8*i));
}
bool psx_response_header_valid(const uint8_t *f,size_t n){
    if(!f || n<3 || (f[0]!=0xFF && f[0]!=0x00))return false;
    if(f[2]==0x5A)return true;
    // PSX-SPX configuration commands, caution 3: pressing ANALOG after
    // configuration can replace the normal reply's 5A with 00. Preserve it.
    return f[2]==0 && (f[1]==0x41 || f[1]==0x53 || f[1]==0x73 || f[1]==0x79);
}
bool psx_parse(const uint8_t *f, size_t n, psx_sample_t *s) {
    if (!s || !psx_response_header_valid(f,n)) return false;
    psx_sample_t p={0};
    p.id=f[1];
    p.mode=255;
    p.capabilities=CAP_VALID;
    p.length=(uint8_t)(3+2*(p.id&15));
    if (p.length>21 || n<p.length) return false;
    memset(p.native, 0xFF, sizeof p.native);
    memcpy(p.native, f, p.length);
    p.buttons[0]=p.buttons[1]=0xFF;
    // Missing axes are zero with CAP_AXES unset, never invented center values.
    switch(p.id) {
    case 0x41: p.family=FAMILY_DIGITAL; p.mode=0; p.capabilities|=CAP_BUTTONS; break;
    case 0x53: p.family=FAMILY_DUAL_ANALOG; p.mode=1; p.capabilities|=CAP_BUTTONS|CAP_AXES; break;
    case 0x73: p.family=FAMILY_DUALSHOCK; p.mode=1; p.capabilities|=CAP_BUTTONS|CAP_AXES; break;
    case 0x79: p.family=FAMILY_DS2; p.mode=2; p.capabilities|=CAP_BUTTONS|CAP_AXES|CAP_PRESSURE; break;
    case 0xF3: p.mode=3; p.capabilities|=CAP_CONFIG; break;
    default: return false; // Optional special controllers are not in this MVP.
    }
    if (p.capabilities&CAP_BUTTONS) { p.buttons[0]=f[3]; p.buttons[1]=f[4]; }
    if (p.capabilities&CAP_AXES) memcpy(p.axes,f+5,4);
    if (p.capabilities&CAP_PRESSURE) memcpy(p.pressure,f+9,12);
    *s=p; return true;
}
void psx_state_init(psx_state_t *s) {
    memset(s,0,sizeof *s); s->sample.id=0xFF; s->sample.mode=255;
    s->sample.buttons[0]=s->sample.buttons[1]=0xFF;
    memset(s->sample.native,0xFF,21);
}
bool psx_state_feed(psx_state_t *s, const uint8_t *f, size_t n, uint32_t now) {
    ++s->sequence;
    psx_sample_t p;
    if (psx_parse(f,n,&p)) {
        // Family describes the observed protocol, not a guaranteed physical model.
        // Remember DS2 capability within this connection when it switches modes.
        if (s->connected && s->sample.family==FAMILY_DS2) p.family=FAMILY_DS2;
        else if ((p.capabilities&CAP_CONFIG) && s->connected) p.family=s->sample.family;
        s->sample=p; s->connected=true; s->seen=true; s->last_valid_us=now;
        return true;
    }
    bool absent = f && n && (f[0]==0xFF || f[0]==0x00);
    if (absent) for (size_t i=1;i<n;i++) if(f[i]!=f[0]) {absent=false;break;}
    if (!absent) ++s->errors;
    // No checksum exists in PSX. Invalid reports are not fresh input.
    s->sample.capabilities &= (uint8_t)~CAP_VALID;
    if (s->connected && (uint32_t)(now-s->last_valid_us)>=30000) {
        s->connected=false;
        memset(&s->sample,0,sizeof s->sample);
        s->sample.id=0xFF; s->sample.mode=255;
        s->sample.buttons[0]=s->sample.buttons[1]=0xFF;
        memset(s->sample.native,0xFF,21);
    }
    return false;
}
void psx_raw_report(const psx_state_t *s, uint32_t now, uint8_t o[64]) {
    memset(o,0,64); memcpy(o,"MPSX",4);
    const psx_sample_t *p=&s->sample;
    o[4]=1; o[5]=p->family; o[6]=p->id; o[7]=p->mode;
    o[8]=s->connected; o[9]=p->capabilities;
    memcpy(o+10,p->buttons,2); memcpy(o+12,p->axes,4); memcpy(o+16,p->pressure,12);
    le32(o+28,s->sequence); le32(o+32,s->errors);
    o[36]=PSX_POLL_US&255; o[37]=PSX_POLL_US>>8; le32(o+38,now);
    o[42]=p->length; memcpy(o+43,p->native,21);
}
void psx_gamepad_report(const psx_state_t *s, modi_gamepad_t *o) {
    memset(o,0,sizeof *o); o->lx=o->ly=o->rx=o->ry=128; o->hat=8;
    const psx_sample_t *p=&s->sample;
    if (!s->connected || !(p->capabilities&CAP_BUTTONS) || !(p->capabilities&CAP_VALID)) return;
    if(p->capabilities&CAP_AXES) {o->rx=p->axes[0];o->ry=p->axes[1];o->lx=p->axes[2];o->ly=p->axes[3];}
    uint16_t b=(uint16_t)~(p->buttons[0]|((uint16_t)p->buttons[1]<<8));
    // HID button order: Cross,Circle,Square,Triangle,L1,R1,L2,R2,Select,Start,L3,R3.
    static const uint8_t bits[12]={14,13,15,12,10,11,8,9,0,3,1,2};
    for(unsigned i=0;i<12;i++) if(b&(1u<<bits[i])) o->buttons|=(uint16_t)(1u<<i);
    static const uint8_t dpad[4]={4,6,7,5}; // Up,Down,Left,Right
    for(unsigned i=0;i<4;i++) if(b&(1u<<dpad[i])) o->buttons|=(uint16_t)(1u<<(12+i));
    int x=((b>>5)&1)-((b>>7)&1), y=((b>>6)&1)-((b>>4)&1);
    if(y<0) o->hat=x>0?1:x<0?7:0;
    else if(y>0) o->hat=x>0?3:x<0?5:4;
    else if(x) o->hat=x>0?2:6;
}
