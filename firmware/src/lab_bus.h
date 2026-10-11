// SPDX-License-Identifier: Apache-2.0
#pragma once
#include <stdint.h>
#include <stdbool.h>
#define LAB_BYTES 21
#define LAB_QUEUE 16
#define LAB_TRACE 256
enum { LAB_POLL=0, LAB_AUTO=1, LAB_MANUAL=2, LAB_CONTROL=3 };
enum { LAB_OK=0, LAB_TIMEOUT=1, LAB_HEADER=2, LAB_CANCEL=3, LAB_FULL=4, LAB_BAD=5 };
typedef struct { uint16_t id; uint8_t origin,length,timeout_ms,tx[LAB_BYTES]; } lab_request_t;
void lab_init(void);
bool lab_enqueue(const lab_request_t *request);
bool lab_take(lab_request_t *request);
unsigned lab_queued(void);
uint8_t lab_flags(void);
uint32_t lab_generation(void);
void lab_output(const uint8_t *packet, unsigned length, uint32_t now);
void lab_complete(const lab_request_t *request, const uint8_t *rx, unsigned length,
                  uint8_t status, uint32_t start, uint32_t end);
void lab_feature(uint8_t packet[64], uint32_t now);
void lab_usb_sent(const uint8_t gamepad[7], uint32_t raw_sequence, uint32_t now);
