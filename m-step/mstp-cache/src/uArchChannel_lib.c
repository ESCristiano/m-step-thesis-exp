#include "mstp_cache.h"

//------------------------------------------------------------------------------
// "Import" Assembly Functions to access the ICache Buffers
//------------------------------------------------------------------------------
extern void prime_set_0_way_0();
extern void second_prime_set_0_way_0();

extern void probe_set_0_way_0();
extern void probe_set_0_way_1();
//------------------------------------------------------------------------------

// touch_s uses the same block of jumps of the prime 1 
__attribute__ ((noinline)) 
void touch_s(uint8_t secret){
    void (*func_ptr)(void)=(&prime_set_0_way_0+1+((I_N_CACHE_SETS-secret-1)*I_CACHE_LINE_SIZE));
    func_ptr();
}

// touch_ns uses the same block of jumps of the prime 2
__attribute__ ((noinline)) 
void touch_ns(uint8_t secret){
    void (*func_ptr)(void)=(&second_prime_set_0_way_0+1+((I_N_CACHE_SETS-secret-1)*I_CACHE_LINE_SIZE));
    func_ptr();
}

void prime(){
    prime_set_0_way_0();
}

void prime_2(){
    second_prime_set_0_way_0();
}

//------------------------------------------------------------------------------
// Non Secure (or S tf-m level 1), other priv levels we don't have access to the
// systick timer
//------------------------------------------------------------------------------
#ifdef SYSTICK_AVAILABLE
    __attribute__((optimize(0)))
    __attribute__ ((noinline)) 
    volatile uint32_t probe_all(){
        volatile uint32_t time1, time2, result;
        SysTick->CTRL = 0x0;
        SysTick->LOAD  = (uint32_t)(0xFFFFFF - 1); //max counter value
        SysTick->VAL = 0; 
        SysTick->CTRL = 0x05; // starts the systick
        time1 = SysTick->VAL;
        prime_set_0_way_0();
        time2 = SysTick->VAL;
        result = time1 - time2; // inverted subtraction because it is a down counter
        SysTick->CTRL = 0x0; // stops the systick
        SysTick->VAL = 0; 
        return result;
    }

    __attribute__((optimize(0))) 
    __attribute__ ((noinline)) 
    volatile uint32_t probe_line(uint8_t line, uint8_t way){
    volatile uint32_t time1, time2, result;
    void (*func_ptr)(void);
    SysTick->CTRL = 0x0;
    SysTick->LOAD  = (uint32_t)(0xFFFFFF - 1); //max counter value
    SysTick->VAL = 0; 
    if(!way)
        func_ptr = (&probe_set_0_way_0+1+((line)*I_CACHE_LINE_SIZE));
    else
        func_ptr = (&probe_set_0_way_1+1+((line)*I_CACHE_LINE_SIZE));
    SysTick->CTRL = 0x05; // starts the systick
    time1 = SysTick->VAL;
    func_ptr();
    time2 = SysTick->VAL;
    result = time1 - time2; // inverted subtraction because it is a down counter
    SysTick->CTRL = 0x0; // stops the systick
    SysTick->VAL = 0; 
    return result;
    }
#else
    //------------------------------------------------------------------------------
    // Wall-clocks definitions (Board Peripheral Timers)
    //------------------------------------------------------------------------------
    #ifdef STM32L552
        #define UP_COUNTER
        uint32_t *ptr_ctr = 0x50001000, 
            *ptr_cnt = 0x50001000+0x24, 
            *rcc_ctrl = 0x50021000+0x058;

        // Use these "functions" as a MACRO to reduce cache polution. These functions
        // will be called in a __attribute__((optimize(0))) function, so inline 
        // functions are not allowed
        #define config_timer()(*rcc_ctrl = *rcc_ctrl | (1<<4))
        #define enable_timer()(*ptr_ctr = (*ptr_ctr)|1<<0)
        #define disable_timer()(*ptr_ctr = (*ptr_ctr)&~(1<<0))
    #endif 
    //------------------------------------------------------------------------------

    __attribute__((optimize(0))) 
    __attribute__ ((noinline)) 
    volatile uint32_t probe_all_s(){
        volatile uint32_t time1, time2;
        config_timer();
        enable_timer();
        time1 = *ptr_cnt;
        prime_set_0_way_0();
        time2 = *ptr_cnt;
        disable_timer();
        #ifdef UP_COUNTER
            return (time2 - time1);
        #endif
        #ifdef DOWN_COUNTER
            return (time1 - time2);
        #endif
    }

    __attribute__((optimize(0)))
    __attribute__ ((noinline)) 
    volatile uint32_t probe_line_s(uint8_t line, uint8_t way){
        volatile uint32_t time1, time2;
        void (*func_ptr)(void);
        config_timer();
        if(!way)
            func_ptr = (&probe_set_0_way_0+1+((line)*I_CACHE_LINE_SIZE));
        else
            func_ptr = (&probe_set_0_way_1+1+((line)*I_CACHE_LINE_SIZE));
        enable_timer();
        time1 = *ptr_cnt;
        func_ptr();
        time2 = *ptr_cnt;
        disable_timer();
        #ifdef UP_COUNTER
            return (time2 - time1);
        #endif
        #ifdef DOWN_COUNTER
            return (time1 - time2);
        #endif
    }
#endif