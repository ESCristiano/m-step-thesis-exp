#ifndef INC_EVAL_COVERT_CH_H
#define INC_EVAL_COVERT_CH_H

#include "mstp.h"
#include "mstp_test.h"
#include "mstp_board_setup.h"

extern mstp_test_conf_t test_conf_covert_ch;

typedef enum {
    TEST2_DIV,
    TEST3_INST_DIFF,
    TEST4_CACHE,
    TEST5_CONT,
    N_COVERT_CH_VICTIMS
} test_covert_ch_t;

void test2_div_instructions_eval(uint16_t n_runs, uint8_t print, uint8_t world);
void test3_instructions_diff(uint16_t n_runs, uint8_t print, uint8_t world);
void test4_cache(uint16_t n_runs, uint8_t print, uint8_t world);
void test5_memory_contention(uint16_t n_runs, uint8_t print, uint8_t world);

#endif 