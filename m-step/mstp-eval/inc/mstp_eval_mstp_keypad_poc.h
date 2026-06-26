#ifndef INC_EVAL_KEYPAD_POC_H
#define INC_EVAL_KEYPAD_POC_H

#include "mstp.h"
#include "mstp_test.h"
#include "mstp_metrics.h"
#include "mstp_board_setup.h"

extern mstp_test_conf_t test_conf_mstp_keypad_poc;

typedef enum {
    MSTP_KEYPAD_POC_1,
    MSTP_KEYPAD_POC_2,
    N_MSTP_KEYPAD_POC_VICTIMS
} test_mstp_keypad_poc_t;

void mstp_keypad_poc(uint16_t n_runs, uint8_t print, uint8_t world);

#endif 