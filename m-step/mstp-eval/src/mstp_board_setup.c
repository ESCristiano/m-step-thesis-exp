#include "main.h"
#include "mstp_board_setup.h"

//------------------------------------------------------------------------------
// Board setup function pointers
//------------------------------------------------------------------------------
void baord_setup_default();
void baord_setup_no_cache();
void baord_setup_fpu();
void baord_setup_fpu_lazy();

void (*board_conf[N_BOARD_CONF])() = {
    baord_setup_default,
    baord_setup_no_cache,
    baord_setup_fpu,
    baord_setup_fpu_lazy
};  

char *board_conf_names[N_BOARD_CONF] = {
    "baord_setup_default",
    "baord_setup_no_cache",
    "baord_setup_fpu",
    "baord_setup_fpu_lazy"
  };

extern void MX_TIM3_Init(void);
extern void MX_TIM2_Init(void);
extern void MX_TIM5_Init(void);
extern void MX_TIM7_Init(void);
extern void MX_ICACHE_Init(void);
extern void MX_ICACHE_DeInit(void);

void baord_setup_default(){
    setup_ns_mpu(); 

    MX_TIM3_Init();
    MX_TIM2_Init();
    MX_TIM5_Init();
    MX_TIM7_Init();
    MX_ICACHE_Init();
    // disable fpu -> Clear ASPEN bit to disable FPU
    FPU->FPCCR &= ~(0x1 << 31);     

    SET_BIT(ICACHE->CR, ICACHE_CR_HITMEN);
    SET_BIT(ICACHE->CR, ICACHE_CR_MISSMEN);
}

void baord_setup_no_cache(){
    MX_TIM3_Init();
    MX_TIM2_Init();
    MX_TIM5_Init();
    MX_TIM7_Init();
    MX_ICACHE_DeInit();
    // disable fpu -> Clear ASPEN bit to disable FPU
    FPU->FPCCR &= ~(0x1 << 31);     
}

void baord_setup_fpu(){
    setup_ns_mpu(); 

    MX_TIM3_Init();
    MX_TIM2_Init();
    MX_TIM5_Init();
    MX_TIM7_Init();
    MX_ICACHE_Init();
    // enable fpu -> Set ASPEN bit to disable FPU
    FPU->FPCCR |= (0x1 << 31); 
    // disable lazy stacking -> Clear the LSPEN bit to disable lazy stacking
    FPU->FPCCR &= ~(1 << 30);
}

void baord_setup_fpu_lazy(){
    setup_ns_mpu(); 
    
    MX_TIM3_Init();
    MX_TIM2_Init();
    MX_TIM5_Init();
    MX_TIM7_Init();
    MX_ICACHE_Init();
    // enable fpu -> Set ASPEN bit to disable FPU
    FPU->FPCCR |= (0x1 << 31); 
    // disable lazy stacking -> Set the LSPEN bit to enable lazy stacking
    FPU->FPCCR |= (1 << 30); 
}
