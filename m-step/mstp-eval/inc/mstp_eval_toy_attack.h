#ifndef INC_EVAL_TOY_ATTACK_H
#define INC_EVAL_TOY_ATTACK_H

#include "mstp.h"
#include "mstp_test.h"
#include "mstp_metrics.h"
#include "mstp_board_setup.h"

extern mstp_test_conf_t test_conf_toy_attack;

typedef enum {
    TOY_ATTACK,
    N_TOY_ATTACK_VICTIMS
} test_toy_attack_t;

void toy_attack_int_lat(uint16_t n_runs, uint8_t print, uint8_t world);

#endif 