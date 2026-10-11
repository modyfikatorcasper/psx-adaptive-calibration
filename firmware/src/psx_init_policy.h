// SPDX-License-Identifier: Apache-2.0
#pragma once
#include "psx_parser.h"
typedef struct {
    uint8_t model, attempts;
    bool complete, awaiting_poll;
    uint32_t cycle_finished;
} psx_init_policy_t;
void psx_init_policy_reset(psx_init_policy_t *policy);
void psx_init_policy_query(psx_init_policy_t *policy,const uint8_t *rx,unsigned length);
void psx_init_policy_observe(psx_init_policy_t *policy,const psx_state_t *state);
bool psx_init_policy_begin(psx_init_policy_t *policy);
void psx_init_policy_end(psx_init_policy_t *policy,uint32_t now);
bool psx_init_policy_pressure(const psx_init_policy_t *policy,uint8_t id);
