#include "mstp_eval_prntf_gtk.h"

#include "psa/client.h"
#include "psa_manifest/sid.h"

//------------------------------------------------------------------------------
// Victim function pointers
//------------------------------------------------------------------------------
void test_printf_s_tfm();

void (*tests[N_PRTNF_GTK_VICTIMS])() = {
    test_printf_s_tfm
};

char *tests_names[N_PRTNF_GTK_VICTIMS] = {
    "test_printf_s_tfm"
};

//------------------------------------------------------------------------------
// Victim Test Functions
//------------------------------------------------------------------------------
void test_printf_s_tfm() {
    psa_status_t status;
    status = psa_call(MSTP_EVAL_PRINTF_HANDLE, PSA_IPC_CALL, NULL, 0, NULL, 0);
    if (status != PSA_SUCCESS){
        printf("psa_call has failed!\r\n");
        return;
    }
}

//------------------------------------------------------------------------------
// Test specific configs
//------------------------------------------------------------------------------
mstp_conf_t mstp_conf_test[N_PRTNF_GTK_VICTIMS] = {
    // TEST_PRTNF_GTK
    {
        .base_ISR_time          = BASE_ISR_TIME_S,
        .base_clk               = BASE_CLK_S,
        .base_clk_2_collide     = BASE_CLK_CONTENTION_S,
        .streak_threshold       = STREAK_THRESHOLD,
        .ici_streak_threshold   = ICI_STREAK_THRESHOLD,
        .preemptions_threshold  = PREEMPTIONS_THRESHOLD,
        .base_inst_time         = BASE_INST_TIME,   
        .debug                  = DEBUG_ON,
        .trace_enable           = ENABLE_TRACE, 
        .print_iri_latency      = DISABLE_IRI_PRINT,
        .print_ici_latency      = DISABLE_ICI_PRINT,
        .start_trace_window     = 0,
        .mstp_cache_enabled     = DISABLE_MSTP_CACHE,
        .mstp_busted_enabled    = DISABLE_MSTP_BUSTED,
        .end_trace_window       = (uint64_t) -1 // MAX_UINT64
    },
};

mstp_test_conf_t test_conf_prntf_gtk = {
    .board_config   = board_conf,
    .config_name    = board_conf_names,
    .test           = tests,
    .name           = tests_names,
    .mstp_conf      = (mstp_conf_t *[])
                    {
                        mstp_conf_test  // TEST_PRTNF_GTK
                    },
    .n_experiments  = N_PRTNF_GTK_VICTIMS
};

//------------------------------------------------------------------------------
// Test#7: Trace Printf to Output on GTKWave
//------------------------------------------------------------------------------
void test7_prntf_gtk_eval(uint16_t n_runs, uint8_t print, uint8_t world) {   
    // the n_runs has no meaning here, the victim has N instructions
    run_trace(test_conf_prntf_gtk, BOARD_DEFAULT, TEST_PRTNF_GTK, print, world);
}