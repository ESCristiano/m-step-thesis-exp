#ifndef INC_EVAL_MSTP_METRICS_H
#define INC_EVAL_MSTP_METRICS_H

#include "mstp.h"
#include "mstp_test.h"
#include "mstp_metrics.h"
#include "mstp_board_setup.h"

extern mstp_test_conf_t test_conf_mstp_metrics;

typedef enum {
    TEST_BASELINE,
    TEST_ATO_INST, 
    TEST_ICI_INST,     
    TEST_IRI_INST,
    TEST_MIX_INST,
    TEST_PRINTF,
    N_MSTP_METRICS_VICTIMS
} test_mstp_metrics_t;

void test1_mstp_metrics_eval(uint16_t n_runs, uint8_t print, uint8_t world);

#endif 