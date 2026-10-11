// SPDX-License-Identifier: Apache-2.0
#include "tusb.h"
#include "pico/unique_id.h"
#include <string.h>
// No Sony identifiers. pid.codes 1209:0001 is PRIVATE IN-HOUSE TEST ONLY.
#ifndef MODI_USB_VID
#error MODI_USB_VID must be configured
#endif
const tusb_desc_device_t desc_device={
 .bLength=sizeof(tusb_desc_device_t),.bDescriptorType=TUSB_DESC_DEVICE,.bcdUSB=0x0200,
 .bDeviceClass=0,.bDeviceSubClass=0,.bDeviceProtocol=0,.bMaxPacketSize0=64,
 .idVendor=MODI_USB_VID,.idProduct=MODI_USB_PID,.bcdDevice=0x0201,
 .iManufacturer=1,.iProduct=2,.iSerialNumber=3,.bNumConfigurations=1
};
// X,Y,Z,Rx give contiguous axes 0..3 in Chromium Windows RawInput.
// 12 action buttons + 4 D-pad buttons; hat remains available for native clients.
const uint8_t desc_gamepad[]={
 0x05,0x01,0x09,0x05,0xA1,0x01,
 0x15,0x00,0x26,0xFF,0x00,0x75,0x08,0x95,0x04,
 0x09,0x30,0x09,0x31,0x09,0x32,0x09,0x33,0x81,0x02,
 0x09,0x39,0x15,0x00,0x25,0x07,0x35,0x00,0x46,0x3B,0x01,
 0x65,0x14,0x75,0x04,0x95,0x01,0x81,0x42,
 0x65,0x00,0x75,0x04,0x95,0x01,0x81,0x03,
 0x05,0x09,0x19,0x01,0x29,0x10,0x15,0x00,0x25,0x01,
 0x75,0x01,0x95,0x10,0x81,0x02,0xC0
};
// WebHID can request this vendor collection; no report ID prefix, 64 bytes.
const uint8_t desc_raw[]={
 0x06,0x00,0xFF,0x09,0x01,0xA1,0x01,0x09,0x02,
 0x15,0x00,0x26,0xFF,0x00,0x75,0x08,0x95,0x40,0x81,0x02,
 0x09,0x03,0x91,0x02,0x09,0x04,0xB1,0x02,0xC0
};
#define CONFIG_LEN (TUD_CONFIG_DESC_LEN+2*TUD_HID_DESC_LEN)
const uint8_t desc_config[]={
 TUD_CONFIG_DESCRIPTOR(1,2,0,CONFIG_LEN,0,100),
 TUD_HID_DESCRIPTOR(0,4,HID_ITF_PROTOCOL_NONE,sizeof desc_gamepad,0x81,64,1),
 TUD_HID_DESCRIPTOR(1,5,HID_ITF_PROTOCOL_NONE,sizeof desc_raw,0x82,64,1)
};
const uint8_t *tud_descriptor_device_cb(void){return (const uint8_t*)&desc_device;}
const uint8_t *tud_descriptor_configuration_cb(uint8_t index){(void)index;return desc_config;}
const uint8_t *tud_hid_descriptor_report_cb(uint8_t instance){return instance==0?desc_gamepad:desc_raw;}
const uint16_t *tud_descriptor_string_cb(uint8_t index,uint16_t langid) {
 (void)langid;static uint16_t result[64];static char serial[2*PICO_UNIQUE_BOARD_ID_SIZE_BYTES+1];
 if(index==0){result[0]=0x0304;result[1]=0x0409;return result;}
 pico_get_unique_board_id_string(serial,sizeof serial);
 const char *strings[]={"","Modibox","Modi PSX Adapter",serial,"Modi HID Gamepad","Modi Raw Diagnostic HID"};
 if(index>=sizeof strings/sizeof strings[0])return NULL;
 size_t n=strlen(strings[index]);if(n>63)n=63;
 result[0]=(uint16_t)(0x0300|((n+1)*2));
 for(size_t i=0;i<n;i++)result[i+1]=(uint8_t)strings[index][i];
 return result;
}
