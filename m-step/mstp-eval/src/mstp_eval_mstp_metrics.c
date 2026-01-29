#include "mstp_eval_mstp_metrics.h"

#include "psa/client.h"
#include "psa_manifest/sid.h"

//------------------------------------------------------------------------------
// Victim function pointers
//------------------------------------------------------------------------------
void test_baseline_s_tfm();
void test_ATO_inst_s_tfm();
void test_ICI_inst_s_tfm();
void test_IRI_inst_s_tfm();
void test_MIX_inst_s_tfm();
void test_printf_s_tfm();

void (*tests[N_MSTP_METRICS_VICTIMS])() = {
    test_baseline_s_tfm,
    test_ATO_inst_s_tfm,
    test_ICI_inst_s_tfm,
    test_IRI_inst_s_tfm,
    test_MIX_inst_s_tfm,
    test_printf_s_tfm
};

char *tests_names[N_MSTP_METRICS_VICTIMS] = {
    "test_baseline_s_tfm",
    "test_ATO_inst_s_tfm",
    "test_ICI_inst_s_tfm",
    "test_IRI_inst_s_tfm",
    "test_MIX_inst_s_tfm",
    "test_printf_s_tfm",
};

//------------------------------------------------------------------------------
// Victim Test Functions
//------------------------------------------------------------------------------
void test_baseline_s_tfm() {
    psa_status_t status;
    status = psa_call(MSTP_EVAL_EMPTY_HANDLE, PSA_IPC_CALL, NULL, 0, NULL, 0);
    if (status != PSA_SUCCESS){
        printf("psa_call has failed!\r\n");
        return;
    }
}

void test_ATO_inst_s_tfm() {
    psa_status_t status;
    status = psa_call(MSTP_EVAL_NORMAL_INST_HANDLE, PSA_IPC_CALL, NULL, 0, NULL, 0);
    if (status != PSA_SUCCESS){
        printf("psa_call has failed!\r\n");
        return;
    }
}

void test_ICI_inst_s_tfm() {
    psa_status_t status;
    status = psa_call(MSTP_EVAL_ICI_INST_HANDLE, PSA_IPC_CALL, NULL, 0, NULL, 0);
    if (status != PSA_SUCCESS){
        printf("psa_call has failed!\r\n");
        return;
    }
}

void test_IRI_inst_s_tfm() {
    psa_status_t status;
    status = psa_call(MSTP_EVAL_IRI_INST_HANDLE, PSA_IPC_CALL, NULL, 0, NULL, 0);
    if (status != PSA_SUCCESS){
        printf("psa_call has failed!\r\n");
        return;
    }
}

void test_MIX_inst_s_tfm() {
    psa_status_t status;
    status = psa_call(MSTP_EVAL_IRI_ATOMIC_INST_HANDLE, PSA_IPC_CALL, NULL, 0, NULL, 0);
    if (status != PSA_SUCCESS){
        printf("psa_call has failed!\r\n");
        return;
    }
}

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
mstp_test_conf_t test_conf_mstp_metrics = {
    .board_config   = board_conf,
    .config_name    = board_conf_names,
    .test           = tests,
    .name           = tests_names,
    .mstp_conf      = (mstp_conf_t *[])
                    {
                        NULL, // TEST_BASELINE 
                        NULL, // TEST_ATO_INST
                        NULL, // TEST_ICI_INST 
                        NULL, // TEST_IRI_INST 
                        NULL, // TEST_MIX_INST 
                        NULL  // TEST_PRINTF
                    },
    .n_experiments  = N_MSTP_METRICS_VICTIMS
};

//------------------------------------------------------------------------------
// Test#1: Mstp Metrics Test Implementation
//------------------------------------------------------------------------------
extern mstp_metrics_t mstp_metrics;
mstp_metrics_t metrics_tmp = {0};
mstp_metrics_t metrics_eval[N_MSTP_METRICS_VICTIMS] = {0};

void run_test1_n_times( mstp_test_conf_t test_conf, 
                        uint8_t conf_id, 
                        uint16_t test_id, 
                        uint16_t runs, 
                        uint8_t world){ 
    
    metrics_tmp.zero_steps          = 0;
    metrics_tmp.partial_steps       = 0;
    metrics_tmp.single_steps        = 0;
    metrics_tmp.instruction_folding = 0;
    metrics_tmp.multi_steps         = 0;
    metrics_tmp.interrupts          = 0;

    for(uint16_t i = 0; i < runs; i++){
        if (!i){
            run_trace(test_conf, conf_id, test_id, PRINT, world);
            printf("R#%d\r\n", i);
        }
        else{
            printf("R#%d\r\n", i);
            run_trace(test_conf, conf_id, test_id, NO_PRINT, world);
        }   
        // Remove the zero-steps that were partial steps
        metrics_tmp.zero_steps            += (mstp_metrics.zero_steps-mstp_metrics.partial_steps) < 0 ? 0 : (mstp_metrics.zero_steps-mstp_metrics.partial_steps);
        metrics_tmp.partial_steps         += mstp_metrics.partial_steps;
        metrics_tmp.single_steps          += mstp_metrics.single_steps;
        metrics_tmp.instruction_folding   += mstp_metrics.instruction_folding;
        metrics_tmp.multi_steps           += mstp_metrics.multi_steps;
        metrics_tmp.interrupts            += mstp_metrics.interrupts;
        clear_metrics();
    }
    metrics_tmp.zero_steps          /= runs;
    metrics_tmp.partial_steps       /= runs;
    metrics_tmp.single_steps        /= runs;
    metrics_tmp.instruction_folding /= runs;
    metrics_tmp.multi_steps         /= runs;
    metrics_tmp.interrupts          /= runs;
}

void print_test1_results(uint16_t test_id, uint16_t runs) {
    printf("\r\n");
    int32_t zero_diff = metrics_eval[test_id].zero_steps - metrics_eval[TEST_BASELINE].zero_steps;
    int32_t partial_diff = metrics_eval[test_id].partial_steps - metrics_eval[TEST_BASELINE].partial_steps;
    int32_t single_diff = metrics_eval[test_id].single_steps - metrics_eval[TEST_BASELINE].single_steps;
    int32_t folding_diff = metrics_eval[test_id].instruction_folding - metrics_eval[TEST_BASELINE].instruction_folding;
    int32_t multi_diff = metrics_eval[test_id].multi_steps - metrics_eval[TEST_BASELINE].multi_steps;
    int32_t interrupts_diff = metrics_eval[test_id].interrupts - metrics_eval[TEST_BASELINE].interrupts;
    
    printf("Zero Steps    (Avg %d runs): %8d\t%8d\r\n", 
        runs, 
        metrics_eval[TEST_BASELINE].zero_steps,
        zero_diff < 0 ? 0 : zero_diff
    );
    printf("Partial Steps (Avg %d runs): %8d\t%8d\r\n", 
        runs, 
        metrics_eval[TEST_BASELINE].partial_steps,
        partial_diff < 0 ? 0 : partial_diff
    );
    printf("Single Steps  (Avg %d runs): %8d\t%8d\r\n", 
        runs, 
        metrics_eval[TEST_BASELINE].single_steps,
        single_diff < 0 ? 0 : single_diff
    );
    printf("Inst Folding  (Avg %d runs): %8d\t%8d\r\n", 
        runs, 
        metrics_eval[TEST_BASELINE].instruction_folding,
        folding_diff < 0 ? 0 : folding_diff
    );
    printf("Multi Steps   (Avg %d runs): %8d\t%8d\r\n", 
        runs, 
        metrics_eval[TEST_BASELINE].multi_steps,
        multi_diff < 0 ? 0 : multi_diff
    );
    printf("Interrupts    (Avg %d runs): %8d\t%8d\r\n", 
        runs, 
        metrics_eval[TEST_BASELINE].interrupts,
        interrupts_diff < 0 ? 0 : interrupts_diff
    );
}

uint32_t ratio_steps_interrupts(uint32_t interrupts, uint32_t steps) {
    if (interrupts == 0 || steps <= 0) {
        return 0;
    }
    return (steps * 1000) / interrupts;  // Returns ratio * 1000 for 3 decimal precision
}

void print_final_results(uint16_t runs) {
    printf("\r\n");
    mstp_metrics_t metrics_relative[N_MSTP_METRICS_VICTIMS] = {0};
    uint32_t total_steps[N_MSTP_METRICS_VICTIMS] = {0};

    for(int i = 0; i < N_MSTP_METRICS_VICTIMS; i++) {
        if(i != TEST_BASELINE) {
            // Ensure no negative values by using max(0, difference)
            int32_t zero_diff = metrics_eval[i].zero_steps - metrics_eval[TEST_BASELINE].zero_steps;
            int32_t partial_diff = metrics_eval[i].partial_steps - metrics_eval[TEST_BASELINE].partial_steps;
            int32_t single_diff = metrics_eval[i].single_steps - metrics_eval[TEST_BASELINE].single_steps;
            int32_t folding_diff = metrics_eval[i].instruction_folding - metrics_eval[TEST_BASELINE].instruction_folding;
            int32_t multi_diff = metrics_eval[i].multi_steps - metrics_eval[TEST_BASELINE].multi_steps;
            int32_t interrupts_diff = metrics_eval[i].interrupts - metrics_eval[TEST_BASELINE].interrupts;
            
            metrics_relative[i].zero_steps          = zero_diff < 0 ? 0 : zero_diff;
            metrics_relative[i].partial_steps       = partial_diff < 0 ? 0 : partial_diff;
            metrics_relative[i].single_steps        = single_diff < 0 ? 0 : single_diff;
            metrics_relative[i].instruction_folding = folding_diff < 0 ? 0 : folding_diff;
            metrics_relative[i].multi_steps         = multi_diff < 0 ? 0 : multi_diff;
            metrics_relative[i].interrupts          = interrupts_diff < 0 ? 0 : interrupts_diff;
        }
        else
            metrics_relative[i] = metrics_eval[i];
        total_steps[i] = metrics_relative[i].partial_steps + metrics_relative[i].single_steps;
    }

    printf("                             : %8s\t%8s\t%8s\t%8s\t%8s\t%8s\r\n",
           "Base",
           "Atomic", 
           "ICI",
           "IRI",
           "IRI_ATOM",
           "Printf"
    );

    printf("Zero Steps    (Avg %4d runs): %8d\t%8d\t%8d\t%8d\t%8d\t%8d\r\n", 
        runs, 
        metrics_relative[TEST_BASELINE].zero_steps,
        metrics_relative[TEST_ATO_INST].zero_steps,
        metrics_relative[TEST_ICI_INST].zero_steps,
        metrics_relative[TEST_IRI_INST].zero_steps,
        metrics_relative[TEST_MIX_INST].zero_steps,
        metrics_relative[TEST_PRINTF].zero_steps
    );
    printf("Partial Steps (Avg %4d runs): %8d\t%8d\t%8d\t%8d\t%8d\t%8d\r\n", 
        runs, 
        metrics_relative[TEST_BASELINE].partial_steps,
        metrics_relative[TEST_ATO_INST].partial_steps,
        metrics_relative[TEST_ICI_INST].partial_steps,
        metrics_relative[TEST_IRI_INST].partial_steps,
        metrics_relative[TEST_MIX_INST].partial_steps,
        metrics_relative[TEST_PRINTF].partial_steps
    );
    printf("Single Steps  (Avg %4d runs): %8d\t%8d\t%8d\t%8d\t%8d\t%8d\r\n", 
        runs, 
        metrics_relative[TEST_BASELINE].single_steps,
        metrics_relative[TEST_ATO_INST].single_steps,
        metrics_relative[TEST_ICI_INST].single_steps,
        metrics_relative[TEST_IRI_INST].single_steps,
        metrics_relative[TEST_MIX_INST].single_steps,
        metrics_relative[TEST_PRINTF].single_steps
    );
    printf("Inst Folding  (Avg %4d runs): %8d\t%8d\t%8d\t%8d\t%8d\t%8d\r\n", 
        runs, 
        metrics_relative[TEST_BASELINE].instruction_folding,
        metrics_relative[TEST_ATO_INST].instruction_folding,
        metrics_relative[TEST_ICI_INST].instruction_folding,
        metrics_relative[TEST_IRI_INST].instruction_folding,
        metrics_relative[TEST_MIX_INST].instruction_folding,
        metrics_relative[TEST_PRINTF].instruction_folding
    );
    printf("Multi Steps   (Avg %4d runs): %8d\t%8d\t%8d\t%8d\t%8d\t%8d\r\n", 
        runs, 
        metrics_relative[TEST_BASELINE].multi_steps,
        metrics_relative[TEST_ATO_INST].multi_steps,
        metrics_relative[TEST_ICI_INST].multi_steps,
        metrics_relative[TEST_IRI_INST].multi_steps,
        metrics_relative[TEST_MIX_INST].multi_steps,
        metrics_relative[TEST_PRINTF].multi_steps
    );
    printf("Interrupts    (Avg %4d runs): %8d\t%8d\t%8d\t%8d\t%8d\t%8d\r\n", 
        runs, 
        metrics_relative[TEST_BASELINE].interrupts,
        metrics_relative[TEST_ATO_INST].interrupts,
        metrics_relative[TEST_ICI_INST].interrupts,
        metrics_relative[TEST_IRI_INST].interrupts,
        metrics_relative[TEST_MIX_INST].interrupts,
        metrics_relative[TEST_PRINTF].interrupts
    );
    
    printf("Ratio         (Avg %4d runs): %8d\t%8d\t%8d\t%8d\t%8d\t%8d\r\n", 
        runs,
        ratio_steps_interrupts( metrics_relative[TEST_BASELINE].interrupts, total_steps[TEST_BASELINE]),
        ratio_steps_interrupts(metrics_relative[TEST_ATO_INST].interrupts, total_steps[TEST_ATO_INST]),
        ratio_steps_interrupts(metrics_relative[TEST_ICI_INST].interrupts, total_steps[TEST_ICI_INST]),
        ratio_steps_interrupts(metrics_relative[TEST_IRI_INST].interrupts, total_steps[TEST_IRI_INST]),
        ratio_steps_interrupts(metrics_relative[TEST_MIX_INST].interrupts, total_steps[TEST_MIX_INST]),
        ratio_steps_interrupts(metrics_relative[TEST_PRINTF].interrupts, total_steps[TEST_PRINTF])
    );
}

void test1_mstp_metrics_eval(uint16_t n_runs, uint8_t print, uint8_t world) {   
    // Define baseline configuration 
    run_test1_n_times(test_conf_mstp_metrics, BOARD_DEFAULT, TEST_BASELINE, 1, world);
    metrics_eval[TEST_BASELINE] = metrics_tmp;
    if(print)
        print_test1_results(TEST_BASELINE, n_runs);
    
    // Run the actual tests
    run_test1_n_times(test_conf_mstp_metrics, BOARD_DEFAULT, TEST_ATO_INST, n_runs, world);
    metrics_eval[TEST_ATO_INST] = metrics_tmp;
    if(print)
        print_test1_results(TEST_ATO_INST, n_runs);
    
    run_test1_n_times(test_conf_mstp_metrics, BOARD_DEFAULT, TEST_ICI_INST, n_runs, world);
    metrics_eval[TEST_ICI_INST] = metrics_tmp;
    if(print)
        print_test1_results(TEST_ICI_INST, n_runs);
    
    run_test1_n_times(test_conf_mstp_metrics, BOARD_DEFAULT, TEST_IRI_INST, n_runs, world);
    metrics_eval[TEST_IRI_INST] = metrics_tmp;
    if(print)
        print_test1_results(TEST_IRI_INST, n_runs);

    run_test1_n_times(test_conf_mstp_metrics, BOARD_DEFAULT, TEST_MIX_INST, n_runs, world);
    metrics_eval[TEST_MIX_INST] = metrics_tmp;
    if(print)
        print_test1_results(TEST_MIX_INST, n_runs);

    run_test1_n_times(test_conf_mstp_metrics, BOARD_DEFAULT, TEST_PRINTF, n_runs, world);
    metrics_eval[TEST_PRINTF] = metrics_tmp;
    if(print)
        print_test1_results(TEST_PRINTF, n_runs);

    print_final_results(n_runs);
}
