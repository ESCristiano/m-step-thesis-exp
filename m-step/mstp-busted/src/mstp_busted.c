#include "mstp_busted.h"

#define BUSTED_DISABLED     0
#define BUSTED_SRAM1        1
#define BUSTED_FLASH        2
#define BUSTED_AHB2         3
#define BUSTED_APB1         4
#define BUSTED_FLASH_BASE   5


uint8_t monitor_memory = BUSTED_DISABLED;

// These are only used for evaluation purposes (Mstp-Debug)
volatile uint8_t test_target_memory = BUSTED_DISABLED;
extern uint32_t *test_print_trace;

extern uint32_t *tim2_CR1 ,
                *tim2_SR  ,
                *tim2_DIER,
                *tim2_EGR ,
                *tim2_CNT ,
                *tim2_ARR ,
                *tim2_CCR1;

extern uint32_t *tim3_CR1 ,
                *tim3_SR  ,
                *tim3_DIER,
                *tim3_EGR ,
                *tim3_CNT ,
                *tim3_ARR ,
                *tim3_CCR1;

extern uint32_t *tim7_CR1 ,
                *tim7_SR  ,
                *tim7_DIER,
                *tim7_EGR ,
                *tim7_CNT ,
                *tim7_ARR ,
                *tim7_CCR1;

uint32_t cont_counter = 0;

//------------------------------------------------------------------------------
// BUSted Gagdet DMA 
//------------------------------------------------------------------------------
uint32_t *dma1_ch3_ccr      = (uint32_t *) 0x40020030;
uint32_t *dma1_ch3_cpar_dst = (uint32_t *) 0x40020038;
uint32_t *dma1_ch3_cm0ar_src= (uint32_t *) 0x4002003C;

//------------------------------------------------------------------------------
// Target Memories to Create Contetion
//------------------------------------------------------------------------------
// placed in code memory, i.e., flash
const uint32_t Gadget_start = 0x80;
uint32_t *gpio_ptr          = (uint32_t *) 0x42020008; 
uint32_t *dma_ptr           = (uint32_t *) 0x40020018; 
extern uint32_t src;
extern uint32_t dst;  
uint32_t *src_ptr = &src;
uint32_t *dst_ptr = &dst;
//------------------------------------------------------------------------------

void mstp_busted_init(){
    cont_counter = 0;
}   

void mstp_busted(mstp_ctx_t *mstp_ctx, mstp_conf_t *mstp_conf){
    static int counter = 0;               
    uint8_t contention_test_start = ((*test_print_trace >> 3) & 0x1) && ((*test_print_trace) & 0x1);

    monitor_memory = test_target_memory;

    if(mstp_conf[0].mstp_busted_enabled || mstp_conf[1].mstp_busted_enabled){
        if(contention_test_start){
            // The base Contention Threshold values change across target memories
            // because of the different memory access latencies. The BUSted gadget
            // is reading from the target memory to create contention, so the
            // target memory access latency is reflected on the contention measurement.
            
            // We need to disable the streak.threshold because we are doing synthethic
            // tests and we have a artificial stream of instructions with a long streak
            // of base instruction latencies, which will be confused as zero-steps.
            mstp_conf[0].streak_threshold = -1;
            mstp_conf[1].streak_threshold = -1;
            
            switch (monitor_memory)
            {
                // The BUSted gadget to measure contention is also creating 
                // contention on the AHB1, APB1 and APB2 buses, they all share
                // the same physical bus/connection. As we're writing to TIM7
                // to measure contention, We've to discount that on all measuremnets
                // to not get a bias by using the gadget (contention from the
                // gadget itself). By combining the contention measurements from
                // the gadget and the instruction latency we can differentiate 
                // between the different memory regions and types of contention.
                // And filter out the contention created by the gadget itself.
                case BUSTED_SRAM1:
                    // Config Busted Gadget To Create Contention on SRAM1 
                    *dma1_ch3_ccr &= ~(1<<0); // Disable
                    *dma1_ch3_cm0ar_src = &src;
                    *dma1_ch3_cpar_dst  = tim7_CR1;
                    *dma1_ch3_ccr |= (1<<0); // Enable
                    src = 0x80; 
    
                    // Adjust Mstp Contention point
                    mstp_conf[0].base_clk_2_collide = BASE_CLK_CONTENTION_NS+1;
                    mstp_conf[1].base_clk_2_collide = BASE_CLK_CONTENTION_S+1;
    
                    if(*tim7_CNT > 49 && *tim7_CNT < 51 && mstp_ctx->inst_time < 4)
                        cont_counter++;
                    break;
                case BUSTED_FLASH:
                    // Config Busted Gadget To Create Contention on FLASH
                    *dma1_ch3_ccr &= ~(1<<0); // Disable
                    *dma1_ch3_cm0ar_src = &Gadget_start;
                    *dma1_ch3_cpar_dst  = tim7_CR1;
                    *dma1_ch3_ccr |= (1<<0); // Enable
    
                    // Adjust Mstp Contention point
                    mstp_conf[0].base_clk_2_collide = BASE_CLK_CONTENTION_NS+2;
                    mstp_conf[1].base_clk_2_collide = BASE_CLK_CONTENTION_S +2;
    
                    if((*tim7_CNT > 55) && mstp_ctx->inst_time != 4)
                        cont_counter++;
                    break;
                case BUSTED_AHB2:
                    // Config Busted Gadget To Create Contention on AHB2
                    *dma1_ch3_ccr &= ~(1<<0); // Disable
                    *dma1_ch3_cm0ar_src = gpio_ptr;
                    *dma1_ch3_cpar_dst  = tim7_CR1;
                    *dma1_ch3_ccr |= (1<<0); // Enable
                    *gpio_ptr = 0x80; // Gadget_start
                    
                    // Adjust Mstp Contention point
                    mstp_conf[0].base_clk_2_collide = BASE_CLK_CONTENTION_NS+1;
                    mstp_conf[1].base_clk_2_collide = BASE_CLK_CONTENTION_S+1;
                    
                    if(*tim7_CNT >= 51 && mstp_ctx->inst_time < 4)
                        cont_counter++;
                    break;
                case BUSTED_APB1:
                    // Config Busted Gadget To Create Contention on APB1
                    *dma1_ch3_ccr &= ~(1<<0); // Disable
                    *dma1_ch3_cm0ar_src = tim2_CCR1;
                    *dma1_ch3_cpar_dst  = tim7_CR1;
                    *dma1_ch3_ccr |= (1<<0); // Enable
                    *tim2_CCR1 = 0x80; // Gadget_start
                    
                    // Adjust Mstp Contention point
                    mstp_conf[0].base_clk_2_collide = BASE_CLK_CONTENTION_NS+1;
                    mstp_conf[1].base_clk_2_collide = BASE_CLK_CONTENTION_S+1;
                    
                    if(*tim7_CNT >= 54 && mstp_ctx->inst_time >= 4)
                        cont_counter++;
                break;
            default:
                break;
            }
    
            if(mstp_conf[0].trace_enable || mstp_conf[1].trace_enable){
                printf("It: %d | ", counter);
                printf("Con: %d | ", cont_counter);
                printf("Timer: %d | %d clk\r\n", *tim7_CNT , mstp_ctx->inst_time);
            }
            
            *tim7_CNT = 0; // Reset TIM7 counter
            counter++;
    
        } else {
            // Restore Base Mstp configs, Mstp-BUSted disabled
            monitor_memory = BUSTED_DISABLED;
            
            src_ptr = &src;
            dst_ptr = &dst;
    
            mstp_conf[0].base_clk_2_collide = BASE_CLK_CONTENTION_NS;
            mstp_conf[1].base_clk_2_collide = BASE_CLK_CONTENTION_S;
        
            mstp_conf[0].streak_threshold = STREAK_THRESHOLD;
            mstp_conf[1].streak_threshold = STREAK_THRESHOLD;
    
            if(counter){
                printf("It: %d | ", counter);
                printf("Con: %d \r\n", cont_counter);
                counter = 0;
            }
        }
    }
}