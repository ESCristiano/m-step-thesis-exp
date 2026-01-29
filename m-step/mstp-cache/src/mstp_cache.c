#include "mstp_cache.h"

// 8 lines of the covert channel
uint8_t search_lines[8] = {10, 11, 12, 13, 14, 15, 16, 17};
uint8_t covert_channel_bits[8] = {0};
extern uint32_t *test_print_trace;

void mstp_cache_init(){
    // Reset covert channel bits to zero
    for(int i = 0; i < 8; i++)
        covert_channel_bits[i] = 0;
}   

void mstp_cache(mstp_ctx_t *mstp_ctx, mstp_conf_t mstp_conf){
    uint8_t mstp_cache_enabled = ((*test_print_trace >> 2) & 1);
    static uint8_t mstp_cache_enabled_prev = 0;
    
    if(mstp_conf.mstp_cache_enabled){
        // Skip this if we're out of the trace window
        if((mstp_ctx->interrupts_counter >= mstp_conf.start_trace_window && 
            mstp_ctx->interrupts_counter <= mstp_conf.end_trace_window)){
            if(mstp_ctx->lr_exc_ret.secure_stack == S_STACK && mstp_cache_enabled){
                // Force Zero-step to see the cache line touched, then next it single step 
                static uint8_t cache_toggle = 0;
                if (cache_toggle) {
                    mstp_cache_eval();
                }
                cache_toggle = !cache_toggle;
            }
            
            if(mstp_cache_enabled_prev && !mstp_cache_enabled ){
                mstp_cache_eval_results();
            }
            
            mstp_cache_enabled_prev = mstp_cache_enabled;
        }
    }
}

// This function prints all the cache lines that were touched per single-step
void mstp_cache_all_icache(){
    static uint8_t first_call = 1;
    char cache_miss[256] = {0};
    int probe_time;

    //Ignore 1st call, we just want to prime the cache on 1st access
    if(!first_call)
    {
        for (int line = 0; line < 256; line++)
        {
            probe_time = probe_line(line, 0);
            if(probe_time > 21)
                printf("%03d | ", line);
        }
        printf("\r\n");
    }

    first_call = 0;

    prime();
}

int mstp_cache_eval(){
    static uint8_t first_call = 1;
    char cache_miss[256] = {0};
    int probe_time;

    //Ignore 1st call, we just want to prime the cache on 1st access
    if(!first_call)
    {
        for (int line = 0; line < 256; line++)
        {
            probe_time = probe_line(line, 0);
            if(probe_time > 21)
            {               
                for(int i = 0; i < 8; i++) 
                {
                    if(line == search_lines[i]) 
                        covert_channel_bits[i] = line;
                }
            }
        }
    }

    first_call = 0;

    prime();
}

void mstp_cache_eval_results(){
    // Print Covert Channel Matrix Header
    for(int i = 0; i < 8; i++) 
        printf("CL%02d\t| ", search_lines[i]);
    printf("\r\n");

    // Print Covert Channel Matrix
    for (int i = 0; i < 8; i++){
        for (int j = 0; j < 8; j++)
        {
            if(search_lines[j] == covert_channel_bits[i]) {
                printf(" XX\t| ");
            } else
                printf(" --\t| ");
        }
        printf("\r\n"); 
    }
} 

void ICache_invalidation(){
    if (READ_BIT(ICACHE->SR, ICACHE_SR_BUSYF) != 0U) {
        return -1;
    }
    else{
        /* Make sure BSYENDF is reset before to start cache invalidation */
        CLEAR_BIT(ICACHE->FCR, ICACHE_FCR_CBSYENDF);

        /* Launch cache invalidation */
        SET_BIT(ICACHE->CR, ICACHE_CR_CACHEINV);

        while (READ_BIT(ICACHE->SR, ICACHE_SR_BSYENDF) == 0U);
    }
    /* Clear BSYENDF */
    WRITE_REG(ICACHE->FCR, ICACHE_FCR_CBSYENDF);
}