#ifndef INC_EVAL_CCH_NEMESIS_H
#define INC_EVAL_CCH_NEMESIS_H

#include "mstp.h"
#include "mstp_test.h"
#include "mstp_metrics.h"
#include "mstp_board_setup.h"

extern mstp_test_conf_t test_conf_cch_nemesis;

typedef enum {
    CCH_NEMESIS,
    CCH_NEMESIS_0X0F,
    CCH_NEMESIS_0X55,
    CCH_NEMESIS_0XF0,
    CCH_NEMESIS_0XFF,
    N_CCH_NEMESIS_VICTIMS
} test_cch_nemesis_t;

void cch_nemesis_int_lat(uint16_t n_runs, uint8_t print, uint8_t world);

#endif 