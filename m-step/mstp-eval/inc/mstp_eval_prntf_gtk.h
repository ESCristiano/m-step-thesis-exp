#ifndef INC_EVAL_PRNTF_GTK_H
#define INC_EVAL_PRNTF_GTK_H

#include "mstp.h"
#include "mstp_test.h"
#include "mstp_metrics.h"
#include "mstp_board_setup.h"

extern mstp_test_conf_t test_conf_prntf_gtk;

typedef enum {
    TEST_PRTNF_GTK,
    N_PRTNF_GTK_VICTIMS
} test_prntf_gtk_t;

void test7_prntf_gtk_eval(uint16_t n_runs, uint8_t print, uint8_t world);

#endif 