// SPDX-License-Identifier: Apache-2.0
#ifndef MODI_PSX_PARSER_H
#define MODI_PSX_PARSER_H
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#define MODI_RAW_SIZE 64
#define PSX_POLL_US 4500
enum { FAMILY_UNKNOWN=0, FAMILY_DIGITAL=1, FAMILY_DUAL_ANALOG=2,
       FAMILY_DUALSHOCK=3, FAMILY_DS2=4 };
enum { CAP_BUTTONS=1, CAP_AXES=2, CAP_PRESSURE=4, CAP_CONFIG=8, CAP_VALID=16 };
typedef struct {
    uint8_t id, family, mode, capabilities, length;
    uint8_t buttons[2], axes[4], pressure[12], native[21];
} psx_sample_t;
typedef struct {
    psx_sample_t sample;
    uint32_t sequence, errors, last_valid_us;
    bool connected, seen;
} psx_state_t;
typedef struct __attribute__((packed)) {
    uint8_t lx, ly, rx, ry, hat;
    uint16_t buttons;
} modi_gamepad_t;
bool psx_parse(const uint8_t *frame, size_t length, psx_sample_t *sample);
bool psx_response_header_valid(const uint8_t *frame,size_t length);
void psx_state_init(psx_state_t *state);
bool psx_state_feed(psx_state_t *state, const uint8_t *frame, size_t length, uint32_t now_us);
void psx_raw_report(const psx_state_t *state, uint32_t now_us, uint8_t out[64]);
void psx_gamepad_report(const psx_state_t *state, modi_gamepad_t *out);
#endif
