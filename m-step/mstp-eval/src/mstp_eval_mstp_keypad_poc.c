#include "mstp_eval_mstp_keypad_poc.h"

//------------------------------------------------------------------------------
// Victim function pointers
//------------------------------------------------------------------------------
void wrap_victim_s_world();
void read_keypad_2();
void read_keypad_3();

void (*tests[N_MSTP_KEYPAD_POC_VICTIMS])() = {
    read_keypad_2,
    read_keypad_3
};

char *tests_names[N_MSTP_KEYPAD_POC_VICTIMS] = {
    "read_keypad_2",
    "read_keypad_3"

};

//------------------------------------------------------------------------------
// Victim Test Functions
//------------------------------------------------------------------------------
void wrap_victim_s_world() {
    // This is defined on tfm but as a direct secure call to a veneer, so no 
    // tfm noise.
    tfm_victim_mstp_keypad_poc();
}

void read_keypad_2() {
    // This is defined on tfm but as a direct secure call to a veneer, so no 
    // tfm noise.
    // tfm_read_keypad_2();
    // tfm_read_keypad();
    tfm_read_pin();
}

void read_keypad_3() {
    // This is defined on tfm but as a direct secure call to a veneer, so no 
    // tfm noise.
    tfm_read_keypad_3();
}

//------------------------------------------------------------------------------
// Test specific configs
//------------------------------------------------------------------------------
mstp_conf_t mstp_conf_test[N_MSTP_KEYPAD_POC_VICTIMS] = {
    // TEST_PRTNF_GTK
    {
        .base_ISR_time          = BASE_ISR_TIME_S,
        .base_clk               = BASE_CLK_S,
        .base_clk_2_collide     = BASE_CLK_CONTENTION_S,
        .streak_threshold       = STREAK_THRESHOLD,
        .ici_streak_threshold   = ICI_STREAK_THRESHOLD,
        .preemptions_threshold  = PREEMPTIONS_THRESHOLD,
        .base_inst_time         = BASE_INST_TIME,   
        .debug                  = DEBUG_OFF,
        .trace_enable           = ENABLE_TRACE, 
        .print_iri_latency      = DISABLE_IRI_PRINT,
        .print_ici_latency      = DISABLE_ICI_PRINT,
        .start_trace_window     = 0,
        .mstp_cache_enabled     = DISABLE_MSTP_CACHE,
        .mstp_busted_enabled    = DISABLE_MSTP_BUSTED,
        .end_trace_window       = (uint64_t) -1 // MAX_UINT64
    },
};

mstp_test_conf_t test_conf_mstp_keypad_poc = {
    .board_config   = board_conf,
    .config_name    = board_conf_names,
    .test           = tests,
    .name           = tests_names,
    .mstp_conf      = (mstp_conf_t *[])
                    {
                        &mstp_conf_test[0],
                        &mstp_conf_test[0],  
                    },
    .n_experiments  = N_MSTP_KEYPAD_POC_VICTIMS
};

//------------------------------------------------------------------------------
// Test#7: Trace Printf to Output on GTKWave
//------------------------------------------------------------------------------
extern void ICache_invalidation(); 
void mstp_keypad_poc(uint16_t n_runs, uint8_t print, uint8_t world) {  
    // Make sure the ICache is cold on each run for the test to run always under
    // the same conditions.
    // for(int i = 0; i < 4; i++){
    //     printf("TRACE %d\r\n", i);    
    //     ICache_invalidation();
    //     run_trace(test_conf_mstp_keypad_poc, BOARD_DEFAULT, MSTP_KEYPAD_POC_1, print, world);
    //     printf("\r\n");
    // }
    // ICache_invalidation();

    // Prime Cache
    read_keypad_2();

    run_trace(test_conf_mstp_keypad_poc, BOARD_DEFAULT, MSTP_KEYPAD_POC_1, print, world);
    printf("\r\n");
}