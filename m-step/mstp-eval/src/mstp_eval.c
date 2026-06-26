#include "main.h"
#include "mstp.h"
#include "mstp_eval.h"
#include "mstp_eval_poc.h"
#include "mstp_board_setup.h"
#include "mstp_eval_prntf_gtk.h"
#include "mstp_eval_toy_attack.h"
#include "mstp_eval_mstp_metrics.h"
#include "mstp_eval_mstp_keypad_poc.h"
#include "test_eval_config.h"

void mstp_eval(void) {    
    printf("####################################################################\r\n");
    printf("####################################################################\r\n");
    //--------------------------------------------------------------------------
    // Evaluation - USENIX SECURITY'26 Paper
    //--------------------------------------------------------------------------
    #ifdef TEST1_ENABLE
        // test1_mstp_metrics_eval(10, NO_PRINT, S_TFM);
        test1_mstp_metrics_eval(1, NO_PRINT, S_TFM);
    #endif
    #ifdef TEST2_ENABLE
        test2_div_instructions_eval(1, NO_PRINT, S_TFM);
    #endif
    #ifdef TEST3_ENABLE
        test3_instructions_diff(1, NO_PRINT, S_TFM);
    #endif
    #ifdef TEST4_ENABLE
        test4_cache(1, NO_PRINT, S_TFM);
    #endif
    #ifdef TEST5_ENABLE
        test5_memory_contention(1, NO_PRINT, S_TFM);
    #endif
    #ifdef TEST7_ENABLE
        test7_prntf_gtk_eval(1, NO_PRINT, S_TFM);
    #endif

    //--------------------------------------------------------------------------
    // PoC Attacks - USENIX SECURITY'26 Paper
    //--------------------------------------------------------------------------
    // To run the PoCs without trace, uncomment the next lines
    // run_no_trace(test_conf_poc, BOARD_DEFAULT, POC_001, PRINT, S_TFM);
    // run_no_trace(test_conf_poc, BOARD_DEFAULT, POC_002, PRINT, S_TFM);
    
    // // To run the PoCs with trace, uncomment the next lines. 
    // run_trace(test_conf_poc, BOARD_DEFAULT, POC_001, PRINT, S_TFM);
    // run_trace(test_conf_poc, BOARD_DEFAULT, POC_002, PRINT, S_TFM);

    //--------------------------------------------------------------------------
    // Extra - PhD Thesis
    //--------------------------------------------------------------------------
    #ifdef TEST_TOY_ATTACK_ENABLE
        // else-path
        printf("TRACE ELSE\r\n");    
        toy_attack_int_lat(1, NO_PRINT, S_TFM);
        printf("\r\n");
        // if-path
        printf("TRACE IF\r\n");
        toy_attack_int_lat(1, NO_PRINT, S_TFM);
        printf("\r\n");
    #endif
    
    #ifdef TEST_CCH_NEMESIS_ENABLE
        printf("Nemesis Covert-Channel Test\r\n");    
        cch_nemesis_int_lat(1, NO_PRINT, S_TFM);
        printf("\r\n");
    #endif

    #ifdef TEST_MSTP_KEYPAD_POC_ENABLE
        mstp_keypad_poc(1, NO_PRINT, S_TFM);
        printf("\r\n");
    #endif
}