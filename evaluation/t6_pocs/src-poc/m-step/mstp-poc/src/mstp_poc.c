#include "main.h"
#include "mstp.h"
#include "poc_001.h"
#include "mstp_poc_setup.h"
#include "test_poc_config.h"

#include <inttypes.h>

#define N_VICTIMS N_POC_001
#define MSTP_DISABLE    0
#define MSTP_ENABLE     1

#define NO_TRACE        0
#define MSTP_TRACE      1

#define UINT32_MAX 0xFFFFFFFF

uint32_t volatile   *tim5_CR1   = (uint32_t *) 0x40000C00,
                    *tim5_SR    = (uint32_t *) 0x40000C10,
                    *tim5_DIER  = (uint32_t *) 0x40000C0C,
                    *tim5_EGR   = (uint32_t *) 0x40000C14,
                    *tim5_CNT   = (uint32_t *) 0x40000C24,
                    *tim5_ARR   = (uint32_t *) 0x40000C2C,
                    *tim5_CCR1  = (uint32_t *) 0x40000C34;

uint64_t elapsed_time = 0; // Initialize to a large value
uint32_t count = 0;


void print_poc_names(uint8_t index, uint8_t word){
    printf("%s\r\n", poc_names[index]);
}

void trace_poc(uint8_t index, uint8_t word){
    trace(poc[index]);
}

void run_poc(uint8_t index, uint8_t word){
    poc[index]();
}

void run_poc_trace(setup_conf_t setup_conf, uint8_t poc_id, uint8_t world){ 
    setup(setup_conf, world);
    printf("---------------------------------------\r\n");
    printf("Setup %d\r\n", setup_conf);
    print_poc_names(poc_id, world);
    printf("---------------------------------------\r\n");
    poc_config(POC_SETUP_DEFAULT, world);
    trace_poc(poc_id, world); 
}

void run_poc_no_trace(setup_conf_t setup_conf, uint8_t poc_id, uint8_t world){ 
    setup(setup_conf, world);
    printf("---------------------------------------\r\n");
    printf("Setup %d\r\n", setup_conf);
    print_poc_names(poc_id, world);
    printf("---------------------------------------\r\n");
    poc_config(POC_SETUP_DEFAULT, world);
    run_poc(poc_id , world); 
}

void TIM5_IRQHandler(void)
{
    if (count++ != 0)
        printf("count: %u\r\n", count-1);

    *tim5_SR  = 0;
}

void print_uint64(uint64_t value) {
    if (value == 0) {
        printf("0");
        return;
    }
    
    uint32_t billions = (uint32_t)(value / 1000000000ULL);
    uint32_t remainder = (uint32_t)(value % 1000000000ULL);
    
    if (billions > 0) {
        printf("%u", billions);
        printf("%09u", remainder);  // Pad with zeros
    } else {
        printf("%u", remainder);
    }
}

extern void MX_TIM5_Init(void);

void config_measurement_timer(void) {
    static first_time = 0;
    if(!first_time++)
        MX_TIM5_Init();
    *tim5_CR1 &= ~(1<<0);
    *tim5_CNT = 0;
    *tim5_SR = 0;
    *tim5_DIER = 1<<0;
    *tim5_ARR = -1;
}

void start_timer(void) {
    config_measurement_timer();
    *tim5_CR1 |= (1<<0);
}

// Fix 2: Calculate elapsed time correctly
void stop_timer() {
    *tim5_CR1 &= ~(1<<0);
    
    // Calculate total elapsed time
    // Ignore first interrupt it was due to initialization
    uint64_t total_time = ((uint64_t)(count-1) * UINT32_MAX) + *tim5_CNT;
    elapsed_time = total_time;
    
    print_time_metrics();
    elapsed_time = 0;
    count = 1; // Reset is 1 to ignore first interrupt due to initialization
}

void print_time_metrics() {
    // Convert to double for calculations
    double elapsed_time_d = (double)elapsed_time;
    
    // Convert to time units (clock is 110MHz
    double time_ms = elapsed_time_d / 110000.0;
    double time_seconds = elapsed_time_d / 110000000.0;
    double time_min = elapsed_time_d / 6600000000.0;
    
    printf("---------------------------------------\r\n");
    printf("Time: ");
    print_uint64(elapsed_time);
    printf(" clks\r\n");

    // Print as integers by casting
    // I'm multiplying by 5 because actual real time, is at lest 5x more, don't
    // getting why, are the freq really 110MHz?
    printf("Time: %u ms\r\n", (uint32_t)time_ms*5);
    printf("Time: %u seconds\r\n", (uint32_t)time_seconds*5);
    printf("Time: %u minutes\r\n", (uint32_t)time_min*5);
    printf("count: %u \r\n", count-1);
    
}

extern mstp_conf_t mstp_conf;


void PoC_1(uint8_t trace) {
    printf("PoC#1.\r\n");
    
    start_timer();
    if(trace)
        run_poc_trace(SETUP_DEFAULT, POC_001, S_TFM);
    else
        run_poc_no_trace(SETUP_DEFAULT, POC_001, S_TFM);
    stop_timer();
}

void PoC_2(uint8_t trace) {
    printf("PoC#2.\r\n");
    
    start_timer();
    if(trace)
        run_poc_trace(SETUP_DEFAULT, POC_002, S_TFM);
    else
        run_poc_no_trace(SETUP_DEFAULT, POC_002, S_TFM);
    stop_timer();
}

void mstp_poc(void) {    
    printf("####################################################################\r\n");
    
    // IF you want to run the PoCs without trace, uncomment the next lines
    #ifdef POC1_ENABLE
        // PoC_1(NO_TRACE);
    #endif

    #ifdef POC2_ENABLE
        // PoC_2(NO_TRACE);
    #endif

    // Conditionally run PoC 1 based on CMake variable
    #ifdef POC1_ENABLE
        PoC_1(MSTP_TRACE);
    #endif
    
    // Conditionally run PoC 2 based on CMake variable
    #ifdef POC2_ENABLE
        PoC_2(MSTP_TRACE);
    #endif
}