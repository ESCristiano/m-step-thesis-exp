#include "mstp_eval_covert_ch.h"

#include "psa/client.h"
#include "psa_manifest/sid.h"

//------------------------------------------------------------------------------
// Victim function pointers
//------------------------------------------------------------------------------
void test2_eval_div_inst_s_tfm();
void test3_eval_inst_diff_s_tfm();
void test4_eval_cache_s_tfm();
void test5_eval_cont_s_tfm();

void (*tests[N_COVERT_CH_VICTIMS])() = {
    test2_eval_div_inst_s_tfm,
    test3_eval_inst_diff_s_tfm,
    test4_eval_cache_s_tfm,
    test5_eval_cont_s_tfm
};

char *tests_names[N_COVERT_CH_VICTIMS] = {
    "test2_eval_div_inst_s_tfm",
    "test3_eval_inst_diff_s_tfm",
    "test4_eval_cache_s_tfm",
    "test5_eval_cont_s_tfm",
};

//------------------------------------------------------------------------------
// Victim Test Functions
//------------------------------------------------------------------------------
void test2_eval_div_inst_s_tfm() {
  psa_status_t status;
  status = psa_call(MSTP_EVAL_DIV_INS_HANDLE, PSA_IPC_CALL, NULL, 0, NULL, 0);
  if (status != PSA_SUCCESS){
      printf("psa_call has failed!\r\n");
      return;
  }
}

void test3_eval_inst_diff_s_tfm() {
  psa_status_t status;
  status = psa_call(MSTP_EVAL_INST_DIFF_HANDLE, PSA_IPC_CALL, NULL, 0, NULL, 0);
  if (status != PSA_SUCCESS){
      printf("psa_call has failed!\r\n");
      return;
  }
}

void test4_eval_cache_s_tfm() {
  psa_status_t status;
  status = psa_call(MSTP_EVAL_CACHE_HANDLE, PSA_IPC_CALL, NULL, 0, NULL, 0);
  if (status != PSA_SUCCESS){
      printf("psa_call has failed!\r\n");
      return;
  }
}

void test5_eval_cont_s_tfm() {
  psa_status_t status;
  status = psa_call(MSTP_EVAL_CONTENTION_HANDLE, PSA_IPC_CALL, NULL, 0, NULL, 0);
  if (status != PSA_SUCCESS){
      printf("psa_call has failed!\r\n");
      return;
  }
}

//------------------------------------------------------------------------------
// Test specific configs
//------------------------------------------------------------------------------
mstp_conf_t mstp_conf_test[N_COVERT_CH_VICTIMS] = {
    // TEST2_DIV
    {
        .base_ISR_time          = BASE_ISR_TIME_S,
        .base_clk               = BASE_CLK_S,
        .base_clk_2_collide     = BASE_CLK_CONTENTION_S,
        .streak_threshold       = STREAK_THRESHOLD,
        .ici_streak_threshold   = ICI_STREAK_THRESHOLD,
        .preemptions_threshold  = PREEMPTIONS_THRESHOLD,
        .base_inst_time         = BASE_INST_TIME,   
        .debug                  = DEBUG_ON,
        .trace_enable           = DISABLE_TRACE, 
        .print_iri_latency      = ENABLE_IRI_PRINT,
        .print_ici_latency      = DISABLE_ICI_PRINT,
        .mstp_cache_enabled     = DISABLE_MSTP_CACHE,
        .mstp_busted_enabled    = DISABLE_MSTP_BUSTED,
        .start_trace_window     = 0,
        .end_trace_window       = (uint64_t) -1 // MAX_UINT64
    },
    // TEST3_INST_DIFF
    {
        .base_ISR_time          = BASE_ISR_TIME_S,
        .base_clk               = BASE_CLK_S,
        .base_clk_2_collide     = BASE_CLK_CONTENTION_S,
        .streak_threshold       = -1,
        .ici_streak_threshold   = ICI_STREAK_THRESHOLD,
        .preemptions_threshold  = PREEMPTIONS_THRESHOLD,
        .base_inst_time         = BASE_INST_TIME,   
        .debug                  = DEBUG_ON,
        .trace_enable           = ENABLE_TRACE, 
        .print_iri_latency      = DISABLE_IRI_PRINT,
        .print_ici_latency      = DISABLE_ICI_PRINT,
        .mstp_cache_enabled     = DISABLE_MSTP_CACHE,
        .mstp_busted_enabled    = DISABLE_MSTP_BUSTED,
        .start_trace_window     = (uint64_t) -1,
        .end_trace_window       = (uint64_t) -1 // MAX_UINT64
    },
    // TEST4_CACHE
    {
        .base_ISR_time          = BASE_ISR_TIME_S,
        .base_clk               = BASE_CLK_S,
        .base_clk_2_collide     = BASE_CLK_CONTENTION_S,
        .streak_threshold       = STREAK_THRESHOLD,
        .ici_streak_threshold   = ICI_STREAK_THRESHOLD,
        .preemptions_threshold  = PREEMPTIONS_THRESHOLD,
        .base_inst_time         = BASE_INST_TIME,   
        .debug                  = DEBUG_ON,
        .trace_enable           = DISABLE_TRACE, 
        .print_iri_latency      = DISABLE_IRI_PRINT,
        .print_ici_latency      = DISABLE_ICI_PRINT,
        .mstp_cache_enabled     = ENABLE_MSTP_CACHE,
        .mstp_busted_enabled    = DISABLE_MSTP_BUSTED,
        .start_trace_window     = 0,
        .end_trace_window       = (uint64_t) -1 // MAX_UINT64
    },
    // TEST5_CONT
    {
        .base_ISR_time          = BASE_ISR_TIME_S,
        .base_clk               = BASE_CLK_S,
        .base_clk_2_collide     = BASE_CLK_CONTENTION_S,
        .streak_threshold       = -1,
        .ici_streak_threshold   = ICI_STREAK_THRESHOLD,
        .preemptions_threshold  = PREEMPTIONS_THRESHOLD,
        .base_inst_time         = BASE_INST_TIME,   
        .debug                  = DEBUG_ON,
        .trace_enable           = DISABLE_TRACE, 
        .print_iri_latency      = DISABLE_IRI_PRINT,
        .print_ici_latency      = DISABLE_ICI_PRINT,
        .mstp_cache_enabled     = DISABLE_MSTP_CACHE,
        .mstp_busted_enabled    = ENABLE_MSTP_BUSTED,
        .start_trace_window     = (uint64_t) -1,
        .end_trace_window       = (uint64_t) -1 // MAX_UINT64
    },
};

mstp_test_conf_t test_conf_covert_ch = {
    .board_config   = board_conf,
    .config_name    = board_conf_names,
    .test           = tests,
    .name           = tests_names,
    .mstp_conf      = (mstp_conf_t *[])
                        {
                            &mstp_conf_test[0], 
                            &mstp_conf_test[1],
                            &mstp_conf_test[2], 
                            &mstp_conf_test[3]
                        },
    .n_experiments  = N_COVERT_CH_VICTIMS
};

//------------------------------------------------------------------------------
// Test#2: Data-dependent Instructions
//------------------------------------------------------------------------------
void test2_div_instructions_eval(uint16_t n_runs, uint8_t print, uint8_t world) {   
    // the n_runs has no meaning here, the victim has N instructions
    run_trace(test_conf_covert_ch, BOARD_DEFAULT, TEST2_DIV, print, world);
}

//------------------------------------------------------------------------------
// Test#3: Instructions Differentiation
//------------------------------------------------------------------------------
void test3_instructions_diff(uint16_t n_runs, uint8_t print, uint8_t world) {   
    // the n_runs has no meaning here, the victim has N instructions
    run_trace(test_conf_covert_ch, BOARD_DEFAULT, TEST3_INST_DIFF, print, world);
}

//------------------------------------------------------------------------------
// Test#4: Cache Misses
//------------------------------------------------------------------------------
void test4_cache(uint16_t n_runs, uint8_t print, uint8_t world) {   
    // the n_runs has no meaning here, the victim has N instructions
    run_trace(test_conf_covert_ch, BOARD_DEFAULT, TEST4_CACHE, print, world);
}

//------------------------------------------------------------------------------
// Test#5: Memory Contention
//------------------------------------------------------------------------------
#define BUSTED_DISABLED     0
#define BUSTED_SRAM1        1
#define BUSTED_FLASH        2
#define BUSTED_AHB2         3
#define BUSTED_APB1         4

extern uint8_t test_target_memory;
extern void ICache_invalidation();

void test5_memory_contention(uint16_t n_runs, uint8_t print, uint8_t world) {  
    // the n_runs has no meaning here, the victim has N instructions
    test_target_memory = BUSTED_SRAM1;
    run_trace(test_conf_covert_ch, BOARD_DEFAULT, TEST5_CONT, print, world);

    // Invalidate to simulate a cold run.
    ICache_invalidation();
    test_target_memory = BUSTED_AHB2;
    run_trace(test_conf_covert_ch, BOARD_DEFAULT, TEST5_CONT, print, world);

    // Invalidate to simulate a cold run.
    ICache_invalidation();
    test_target_memory = BUSTED_APB1;
    run_trace(test_conf_covert_ch, BOARD_DEFAULT, TEST5_CONT, print, world);

    // Invalidate to simulate a cold run.
    ICache_invalidation();
    test_target_memory = BUSTED_FLASH;
    run_trace(test_conf_covert_ch, BOARD_DEFAULT, TEST5_CONT, print, world);
}
