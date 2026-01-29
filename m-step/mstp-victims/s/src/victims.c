//------------------------------------------------------------------------------
// For some reason, the TF-M build system is ignoring the .s files
// so I'm putting the assembly code here in inline C
//------------------------------------------------------------------------------

#include "victims.h"
#include <stdio.h>
#include <stdint.h>
#include "test_eval_config.h"

#ifdef TEST1_ENABLE
  // Metrics
  #define EXPERIMENT_1
#endif
#ifdef TEST2_ENABLE
  // DIV diff
  #define EXPERIMENT_2
#endif
#ifdef TEST3_ENABLE
  // Inst diff
  #define EXPERIMENT_3
#endif
#ifdef TEST4_ENABLE
  // Cache
  #define EXPERIMENT_4
#endif
#ifdef TEST5_ENABLE
  // Contention
  #define EXPERIMENT_5
#endif

////////////////////////////////////////////////////////////////////////////////
///////////////////////////////// Test #1 Victims //////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------------
// Experiment #1: Victim functions with streak of atomic instructions
//------------------------------------------------------------------------------
#define ATOMIC_INSTR() do { \
  __asm volatile("mov r0,r0"); \
  __asm volatile("ldr r2, [sp]"); \
} while(0)

#ifdef EXPERIMENT_1

#define TEN_ATOMIC_INSTR()do {\
  ATOMIC_INSTR();\
  ATOMIC_INSTR();\
  ATOMIC_INSTR();\
  ATOMIC_INSTR();\
  ATOMIC_INSTR();\
} while(0)

#define HUNDRED_ATOMIC_INSTR()do {\
  TEN_ATOMIC_INSTR();\
  TEN_ATOMIC_INSTR();\
  TEN_ATOMIC_INSTR();\
  TEN_ATOMIC_INSTR();\
  TEN_ATOMIC_INSTR();\
  TEN_ATOMIC_INSTR();\
  TEN_ATOMIC_INSTR();\
  TEN_ATOMIC_INSTR();\
  TEN_ATOMIC_INSTR();\
  TEN_ATOMIC_INSTR();\
} while(0)

#define THOUSAND_ATOMIC_INSTR()do {\
  HUNDRED_ATOMIC_INSTR();\
  HUNDRED_ATOMIC_INSTR();\
  HUNDRED_ATOMIC_INSTR();\
  HUNDRED_ATOMIC_INSTR();\
  HUNDRED_ATOMIC_INSTR();\
  HUNDRED_ATOMIC_INSTR();\
  HUNDRED_ATOMIC_INSTR();\
  HUNDRED_ATOMIC_INSTR();\
  HUNDRED_ATOMIC_INSTR();\
  HUNDRED_ATOMIC_INSTR();\
} while(0)

//------------------------------------------------------------------------------
// Experiment #2: Victim functions with streak of ICI instructions
//------------------------------------------------------------------------------
#define ICI_INSTR() do { \
  __asm volatile("push {r0, r1, r2}"); \
  __asm volatile("pop {r0, r1, r2}"); \
} while(0)

#define TEN_ICI_INSTR()do {\
  ICI_INSTR();\
  ICI_INSTR();\
  ICI_INSTR();\
  ICI_INSTR();\
  ICI_INSTR();\
} while(0)

#define HUNDRED_ICI_INSTR()do {\
  TEN_ICI_INSTR();\
  TEN_ICI_INSTR();\
  TEN_ICI_INSTR();\
  TEN_ICI_INSTR();\
  TEN_ICI_INSTR();\
  TEN_ICI_INSTR();\
  TEN_ICI_INSTR();\
  TEN_ICI_INSTR();\
  TEN_ICI_INSTR();\
  TEN_ICI_INSTR();\
} while(0)

#define THOUSAND_ICI_INSTR()do {\
  HUNDRED_ICI_INSTR();\
  HUNDRED_ICI_INSTR();\
  HUNDRED_ICI_INSTR();\
  HUNDRED_ICI_INSTR();\
  HUNDRED_ICI_INSTR();\
  HUNDRED_ICI_INSTR();\
  HUNDRED_ICI_INSTR();\
  HUNDRED_ICI_INSTR();\
  HUNDRED_ICI_INSTR();\
  HUNDRED_ICI_INSTR();\
} while(0)

//------------------------------------------------------------------------------
// Experiment #3: Victim functions with streak of IRI instructions
//------------------------------------------------------------------------------
#define SETUP_CODE_IRI()do { \
  __asm volatile("mov r1, #0x1000"); \
  __asm volatile("mov r2, #0x7"); \
} while(0)

#define IRI_INSTR() do { \
  __asm volatile("udiv r0, r1, r2"); \
} while(0)

#define TEN_IRI_INSTR()do {\
  IRI_INSTR();\
  IRI_INSTR();\
  IRI_INSTR();\
  IRI_INSTR();\
  IRI_INSTR();\
  IRI_INSTR();\
  IRI_INSTR();\
  IRI_INSTR();\
  IRI_INSTR();\
  IRI_INSTR();\
} while(0)

#define HUNDRED_IRI_INSTR()do {\
  TEN_IRI_INSTR();\
  TEN_IRI_INSTR();\
  TEN_IRI_INSTR();\
  TEN_IRI_INSTR();\
  TEN_IRI_INSTR();\
  TEN_IRI_INSTR();\
  TEN_IRI_INSTR();\
  TEN_IRI_INSTR();\
  TEN_IRI_INSTR();\
  TEN_IRI_INSTR();\
} while(0)

#define THOUSAND_IRI_INSTR()do {\
  HUNDRED_IRI_INSTR();\
  HUNDRED_IRI_INSTR();\
  HUNDRED_IRI_INSTR();\
  HUNDRED_IRI_INSTR();\
  HUNDRED_IRI_INSTR();\
  HUNDRED_IRI_INSTR();\
  HUNDRED_IRI_INSTR();\
  HUNDRED_IRI_INSTR();\
  HUNDRED_IRI_INSTR();\
  HUNDRED_IRI_INSTR();\
} while(0)
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Experiment #4: Victim functions with streak of IRI+Atomic interleaved
//------------------------------------------------------------------------------
#define IRI_ATOMIC_INSTR() do { \
  __asm volatile("udiv r0, r1, r2"); \
  __asm volatile("ldr r3, [sp]"); \
} while(0)

#define TEN_IRI_ATOMIC_INSTR()do {\
  IRI_ATOMIC_INSTR();\
  IRI_ATOMIC_INSTR();\
  IRI_ATOMIC_INSTR();\
  IRI_ATOMIC_INSTR();\
  IRI_ATOMIC_INSTR();\
} while(0)

#define HUNDRED_IRI_ATOMIC_INSTR()do {\
  TEN_IRI_ATOMIC_INSTR();\
  TEN_IRI_ATOMIC_INSTR();\
  TEN_IRI_ATOMIC_INSTR();\
  TEN_IRI_ATOMIC_INSTR();\
  TEN_IRI_ATOMIC_INSTR();\
  TEN_IRI_ATOMIC_INSTR();\
  TEN_IRI_ATOMIC_INSTR();\
  TEN_IRI_ATOMIC_INSTR();\
  TEN_IRI_ATOMIC_INSTR();\
  TEN_IRI_ATOMIC_INSTR();\
} while(0)

#define THOUSAND_IRI_ATOMIC_INSTR()do {\
  HUNDRED_IRI_ATOMIC_INSTR();\
  HUNDRED_IRI_ATOMIC_INSTR();\
  HUNDRED_IRI_ATOMIC_INSTR();\
  HUNDRED_IRI_ATOMIC_INSTR();\
  HUNDRED_IRI_ATOMIC_INSTR();\
  HUNDRED_IRI_ATOMIC_INSTR();\
  HUNDRED_IRI_ATOMIC_INSTR();\
  HUNDRED_IRI_ATOMIC_INSTR();\
  HUNDRED_IRI_ATOMIC_INSTR();\
  HUNDRED_IRI_ATOMIC_INSTR();\
} while(0)

//------------------------------------------------------------------------------
#endif // EXPERIMENT_1

void victim_empty(void) {
}

void victim_normal_inst(void) {

  #ifdef EXPERIMENT_1
  THOUSAND_ATOMIC_INSTR();
  THOUSAND_ATOMIC_INSTR();
  #endif // EXPERIMENT_1

}

void victim_ICI_inst(void) {
  #ifdef EXPERIMENT_1
  THOUSAND_ICI_INSTR();
  THOUSAND_ICI_INSTR();
  #endif // EXPERIMENT_1
}

void victim_IRI_inst(void) {
  #ifdef EXPERIMENT_1
  SETUP_CODE_IRI();
  THOUSAND_IRI_INSTR();
  THOUSAND_IRI_INSTR();
  #endif // EXPERIMENT_1
}

void victim_IRI_ATOMIC_inst(void) {
  #ifdef EXPERIMENT_1
  SETUP_CODE_IRI();
  THOUSAND_IRI_ATOMIC_INSTR();
  THOUSAND_IRI_ATOMIC_INSTR();
  #endif // EXPERIMENT_1
}

void victim_printf(void) {
  printf("(USENIX SECURITY'26 Paper): Testing M-Step over a generic stdio.h lib printf.\n\r");
}

////////////////////////////////////////////////////////////////////////////////
///////////////////////////////// Test #2 Victims //////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#ifdef EXPERIMENT_2
//------------------------------------------------------------------------------
// Division Timing Analysis
//------------------------------------------------------------------------------
#define UDIV_INSTR() do { \
  __asm volatile("udiv r0, r1, r2"); \
} while(0)

#define EXECUTE_DIV_TEST_32(dividend_low, dividend_high, divisor_low, divisor_high) do { \
  __asm volatile("mov r1, #" #dividend_low "; movt r1, #" #dividend_high); \
  __asm volatile("mov r2, #" #divisor_low "; movt r2, #" #divisor_high); \
  UDIV_INSTR(); \
  ATOMIC_INSTR(); \
} while(0)

#define TEST(dividend, divisor) do { \
  EXECUTE_DIV_TEST_32((dividend) & 0xFFFF, ((dividend) >> 16) & 0xFFFF, (divisor) & 0xFFFF, ((divisor) >> 16) & 0xFFFF); \
  EXECUTE_DIV_TEST_32((dividend) & 0xFFFF, ((dividend) >> 16) & 0xFFFF, (divisor) & 0xFFFF, ((divisor) >> 16) & 0xFFFF); \
  EXECUTE_DIV_TEST_32((dividend) & 0xFFFF, ((dividend) >> 16) & 0xFFFF, (divisor) & 0xFFFF, ((divisor) >> 16) & 0xFFFF); \
  EXECUTE_DIV_TEST_32((dividend) & 0xFFFF, ((dividend) >> 16) & 0xFFFF, (divisor) & 0xFFFF, ((divisor) >> 16) & 0xFFFF); \
  EXECUTE_DIV_TEST_32((dividend) & 0xFFFF, ((dividend) >> 16) & 0xFFFF, (divisor) & 0xFFFF, ((divisor) >> 16) & 0xFFFF); \
  EXECUTE_DIV_TEST_32((dividend) & 0xFFFF, ((dividend) >> 16) & 0xFFFF, (divisor) & 0xFFFF, ((divisor) >> 16) & 0xFFFF); \
  EXECUTE_DIV_TEST_32((dividend) & 0xFFFF, ((dividend) >> 16) & 0xFFFF, (divisor) & 0xFFFF, ((divisor) >> 16) & 0xFFFF); \
  EXECUTE_DIV_TEST_32((dividend) & 0xFFFF, ((dividend) >> 16) & 0xFFFF, (divisor) & 0xFFFF, ((divisor) >> 16) & 0xFFFF); \
  EXECUTE_DIV_TEST_32((dividend) & 0xFFFF, ((dividend) >> 16) & 0xFFFF, (divisor) & 0xFFFF, ((divisor) >> 16) & 0xFFFF); \
  EXECUTE_DIV_TEST_32((dividend) & 0xFFFF, ((dividend) >> 16) & 0xFFFF, (divisor) & 0xFFFF, ((divisor) >> 16) & 0xFFFF); \
  printf("\n\r"); \
} while(0)

#endif // EXPERIMENT_2

void victim_eval_div_inst() {

  #ifdef EXPERIMENT_2
  printf("Matrix Division Timing Test\n\r");
  printf("---------------------------\n\r");
  
  // Row 0: 0x00000000 / all divisors
  // printf("0x00000000/0x0FFFFFFF\n\r"); TEST(0x00000000, 0x0FFFFFFF);
  // printf("0x00000000/0x00FFFFFF\n\r"); TEST(0x00000000, 0x00FFFFFF);
  // printf("0x00000000/0x000FFFFF\n\r"); TEST(0x00000000, 0x000FFFFF);
  // printf("0x00000000/0x0000FFFF\n\r"); TEST(0x00000000, 0x0000FFFF);
  // printf("0x00000000/0x00000FFF\n\r"); TEST(0x00000000, 0x00000FFF);
  // printf("0x00000000/0x000000FF\n\r"); TEST(0x00000000, 0x000000FF);
  // printf("0x00000000/0x0000000F\n\r"); TEST(0x00000000, 0x0000000F);
  // printf("0x00000000/0x00000000\n\r"); TEST(0x00000000, 0x00000000);
  
  // Row 1: 0x0000000F / all divisors
  printf("0x0000000F/0xFFFFFFFF\n\r"); TEST(0x0000000F, 0xFFFFFFFF);
  printf("0x0000000F/0x0FFFFFFF\n\r"); TEST(0x0000000F, 0x0FFFFFFF);
  printf("0x0000000F/0x00FFFFFF\n\r"); TEST(0x0000000F, 0x00FFFFFF);
  printf("0x0000000F/0x000FFFFF\n\r"); TEST(0x0000000F, 0x000FFFFF);
  printf("0x0000000F/0x0000FFFF\n\r"); TEST(0x0000000F, 0x0000FFFF);
  printf("0x0000000F/0x00000FFF\n\r"); TEST(0x0000000F, 0x00000FFF);
  printf("0x0000000F/0x000000FF\n\r"); TEST(0x0000000F, 0x000000FF);
  printf("0x0000000F/0x0000000F\n\r"); TEST(0x0000000F, 0x0000000F);
  // printf("0x0000000F/0x00000000\n\r"); TEST(0x0000000F, 0x00000000);
  
  // Row 2: 0x000000FF / all divisors
  printf("0x000000FF/0xFFFFFFFF\n\r"); TEST(0x000000FF, 0xFFFFFFFF);
  printf("0x000000FF/0x0FFFFFFF\n\r"); TEST(0x000000FF, 0x0FFFFFFF);
  printf("0x000000FF/0x00FFFFFF\n\r"); TEST(0x000000FF, 0x00FFFFFF);
  printf("0x000000FF/0x000FFFFF\n\r"); TEST(0x000000FF, 0x000FFFFF);
  printf("0x000000FF/0x0000FFFF\n\r"); TEST(0x000000FF, 0x0000FFFF);
  printf("0x000000FF/0x00000FFF\n\r"); TEST(0x000000FF, 0x00000FFF);
  printf("0x000000FF/0x000000FF\n\r"); TEST(0x000000FF, 0x000000FF);
  printf("0x000000FF/0x0000000F\n\r"); TEST(0x000000FF, 0x0000000F);
  // printf("0x000000FF/0x00000000\n\r"); TEST(0x000000FF, 0x00000000);
  
  // Row 3: 0x00000FFF / all divisors
  printf("0x00000FFF/0xFFFFFFFF\n\r"); TEST(0x00000FFF, 0xFFFFFFFF);
  printf("0x00000FFF/0x0FFFFFFF\n\r"); TEST(0x00000FFF, 0x0FFFFFFF);
  printf("0x00000FFF/0x00FFFFFF\n\r"); TEST(0x00000FFF, 0x00FFFFFF);
  printf("0x00000FFF/0x000FFFFF\n\r"); TEST(0x00000FFF, 0x000FFFFF);
  printf("0x00000FFF/0x0000FFFF\n\r"); TEST(0x00000FFF, 0x0000FFFF);
  printf("0x00000FFF/0x00000FFF\n\r"); TEST(0x00000FFF, 0x00000FFF);
  printf("0x00000FFF/0x000000FF\n\r"); TEST(0x00000FFF, 0x000000FF);
  printf("0x00000FFF/0x0000000F\n\r"); TEST(0x00000FFF, 0x0000000F);
  // printf("0x00000FFF/0x00000000\n\r"); TEST(0x00000FFF, 0x00000000);
  
  // Row 4: 0x0000FFFF / all divisors
  printf("0x0000FFFF/0xFFFFFFFF\n\r"); TEST(0x0000FFFF, 0xFFFFFFFF);
  printf("0x0000FFFF/0x0FFFFFFF\n\r"); TEST(0x0000FFFF, 0x0FFFFFFF);
  printf("0x0000FFFF/0x00FFFFFF\n\r"); TEST(0x0000FFFF, 0x00FFFFFF);
  printf("0x0000FFFF/0x000FFFFF\n\r"); TEST(0x0000FFFF, 0x000FFFFF);
  printf("0x0000FFFF/0x0000FFFF\n\r"); TEST(0x0000FFFF, 0x0000FFFF);
  printf("0x0000FFFF/0x00000FFF\n\r"); TEST(0x0000FFFF, 0x00000FFF);
  printf("0x0000FFFF/0x000000FF\n\r"); TEST(0x0000FFFF, 0x000000FF);
  printf("0x0000FFFF/0x0000000F\n\r"); TEST(0x0000FFFF, 0x0000000F);
  // printf("0x0000FFFF/0x00000000\n\r"); TEST(0x0000FFFF, 0x00000000);
  
  // Row 5: 0x000FFFFF / all divisors
  printf("0x000FFFFF/0xFFFFFFFF\n\r"); TEST(0x000FFFFF, 0xFFFFFFFF);
  printf("0x000FFFFF/0x0FFFFFFF\n\r"); TEST(0x000FFFFF, 0x0FFFFFFF);
  printf("0x000FFFFF/0x00FFFFFF\n\r"); TEST(0x000FFFFF, 0x00FFFFFF);
  printf("0x000FFFFF/0x000FFFFF\n\r"); TEST(0x000FFFFF, 0x000FFFFF);
  printf("0x000FFFFF/0x0000FFFF\n\r"); TEST(0x000FFFFF, 0x0000FFFF);
  printf("0x000FFFFF/0x00000FFF\n\r"); TEST(0x000FFFFF, 0x00000FFF);
  printf("0x000FFFFF/0x000000FF\n\r"); TEST(0x000FFFFF, 0x000000FF);
  printf("0x000FFFFF/0x0000000F\n\r"); TEST(0x000FFFFF, 0x0000000F);
  // printf("0x000FFFFF/0x00000000\n\r"); TEST(0x000FFFFF, 0x00000000);
  
  // Row 6: 0x00FFFFFF / all divisors
  printf("0x00FFFFFF/0xFFFFFFFF\n\r"); TEST(0x00FFFFFF, 0xFFFFFFFF);
  printf("0x00FFFFFF/0x0FFFFFFF\n\r"); TEST(0x00FFFFFF, 0x0FFFFFFF);
  printf("0x00FFFFFF/0x00FFFFFF\n\r"); TEST(0x00FFFFFF, 0x00FFFFFF);
  printf("0x00FFFFFF/0x000FFFFF\n\r"); TEST(0x00FFFFFF, 0x000FFFFF);
  printf("0x00FFFFFF/0x0000FFFF\n\r"); TEST(0x00FFFFFF, 0x0000FFFF);
  printf("0x00FFFFFF/0x00000FFF\n\r"); TEST(0x00FFFFFF, 0x00000FFF);
  printf("0x00FFFFFF/0x000000FF\n\r"); TEST(0x00FFFFFF, 0x000000FF);
  printf("0x00FFFFFF/0x0000000F\n\r"); TEST(0x00FFFFFF, 0x0000000F);
  // printf("0x00FFFFFF/0x00000000\n\r"); TEST(0x00FFFFFF, 0x00000000);
  
  // Row 7: 0x0FFFFFFF / all divisors
  printf("0x0FFFFFFF/0xFFFFFFFF\n\r"); TEST(0x0FFFFFFF, 0xFFFFFFFF);
  printf("0x0FFFFFFF/0x0FFFFFFF\n\r"); TEST(0x0FFFFFFF, 0x0FFFFFFF);
  printf("0x0FFFFFFF/0x00FFFFFF\n\r"); TEST(0x0FFFFFFF, 0x00FFFFFF);
  printf("0x0FFFFFFF/0x000FFFFF\n\r"); TEST(0x0FFFFFFF, 0x000FFFFF);
  printf("0x0FFFFFFF/0x0000FFFF\n\r"); TEST(0x0FFFFFFF, 0x0000FFFF);
  printf("0x0FFFFFFF/0x00000FFF\n\r"); TEST(0x0FFFFFFF, 0x00000FFF);
  printf("0x0FFFFFFF/0x000000FF\n\r"); TEST(0x0FFFFFFF, 0x000000FF);
  printf("0x0FFFFFFF/0x0000000F\n\r"); TEST(0x0FFFFFFF, 0x0000000F);
  // printf("0x0FFFFFFF/0x00000000\n\r"); TEST(0x0FFFFFFF, 0x00000000);
  
  // Row 8: 0xFFFFFFFF / all divisors
  printf("0xFFFFFFFF/0xFFFFFFFF\n\r"); TEST(0xFFFFFFFF, 0xFFFFFFFF);
  printf("0xFFFFFFFF/0x0FFFFFFF\n\r"); TEST(0xFFFFFFFF, 0x0FFFFFFF);
  printf("0xFFFFFFFF/0x00FFFFFF\n\r"); TEST(0xFFFFFFFF, 0x00FFFFFF);
  printf("0xFFFFFFFF/0x000FFFFF\n\r"); TEST(0xFFFFFFFF, 0x000FFFFF);
  printf("0xFFFFFFFF/0x0000FFFF\n\r"); TEST(0xFFFFFFFF, 0x0000FFFF);
  printf("0xFFFFFFFF/0x00000FFF\n\r"); TEST(0xFFFFFFFF, 0x00000FFF);
  printf("0xFFFFFFFF/0x000000FF\n\r"); TEST(0xFFFFFFFF, 0x000000FF);
  printf("0xFFFFFFFF/0x0000000F\n\r"); TEST(0xFFFFFFFF, 0x0000000F);
  // printf("0xFFFFFFFF/0x00000000\n\r"); TEST(0xFFFFFFFF, 0x00000000);
  #endif // EXPERIMENT_2
  
}

////////////////////////////////////////////////////////////////////////////////
///////////////////////////////// Test #3 Victims //////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#ifdef EXPERIMENT_3
//------------------------------------------------------------------------------
// Instructions Dependent Interrupt Latencies
//------------------------------------------------------------------------------
// MOV: Emulating a NOP instruction
#define MOV() do { \
  __asm volatile("mov r0, r0"); \
} while(0)

#define TEN_MOV()do {\
  MOV(); \
  MOV(); \
  MOV(); \
  MOV(); \
  MOV(); \
  MOV(); \
  MOV(); \
  MOV(); \
  MOV(); \
  MOV(); \
} while(0)

#define HUNDRED_MOV()do {\
  TEN_MOV(); \
  TEN_MOV(); \
  TEN_MOV(); \
  TEN_MOV(); \
  TEN_MOV(); \
  TEN_MOV(); \
  TEN_MOV(); \
  TEN_MOV(); \
  TEN_MOV(); \
  TEN_MOV(); \
} while(0)

#define THOUSAND_MOV()do {\
  HUNDRED_MOV(); \
  HUNDRED_MOV(); \
  HUNDRED_MOV(); \
  HUNDRED_MOV(); \
  HUNDRED_MOV(); \
  HUNDRED_MOV(); \
  HUNDRED_MOV(); \
  HUNDRED_MOV(); \
  HUNDRED_MOV(); \
  HUNDRED_MOV(); \
} while(0)

//------------------------------------------------------------------------------
// LDR SRAM1: Sp is pointing to the stack frame which is located on SRAM1
#define LDR_SRAM1() do { \
  __asm volatile("ldr r0, [sp]"); \
} while(0)

#define TEN_LDR_SRAM1()do {\
  LDR_SRAM1(); \
  LDR_SRAM1(); \
  LDR_SRAM1(); \
  LDR_SRAM1(); \
  LDR_SRAM1(); \
  LDR_SRAM1(); \
  LDR_SRAM1(); \
  LDR_SRAM1(); \
  LDR_SRAM1(); \
  LDR_SRAM1(); \
} while(0)

#define HUNDRED_LDR_SRAM1()do {\
  TEN_LDR_SRAM1(); \
  TEN_LDR_SRAM1(); \
  TEN_LDR_SRAM1(); \
  TEN_LDR_SRAM1(); \
  TEN_LDR_SRAM1(); \
  TEN_LDR_SRAM1(); \
  TEN_LDR_SRAM1(); \
  TEN_LDR_SRAM1(); \
  TEN_LDR_SRAM1(); \
  TEN_LDR_SRAM1(); \
} while(0)

#define THOUSAND_LDR_SRAM1()do {\
  HUNDRED_LDR_SRAM1(); \
  HUNDRED_LDR_SRAM1(); \
  HUNDRED_LDR_SRAM1(); \
  HUNDRED_LDR_SRAM1(); \
  HUNDRED_LDR_SRAM1(); \
  HUNDRED_LDR_SRAM1(); \
  HUNDRED_LDR_SRAM1(); \
  HUNDRED_LDR_SRAM1(); \
  HUNDRED_LDR_SRAM1(); \
  HUNDRED_LDR_SRAM1(); \
} while(0)

//------------------------------------------------------------------------------
// LDR *sp to store 
#define SETUP_CODE_STR_SRAM1() do {  \
  __asm volatile("ldr r0, [sp]");     \
} while(0)

// LDR SRAM1: Sp is pointing to the stack frame which is located on SRAM1
#define STR_SRAM1() do { \
  __asm volatile("str r0, [sp]"); \
} while(0)

#define TEN_STR_SRAM1()do {\
  STR_SRAM1(); \
  STR_SRAM1(); \
  STR_SRAM1(); \
  STR_SRAM1(); \
  STR_SRAM1(); \
  STR_SRAM1(); \
  STR_SRAM1(); \
  STR_SRAM1(); \
  STR_SRAM1(); \
  STR_SRAM1(); \
} while(0)

#define HUNDRED_STR_SRAM1()do {\
  TEN_STR_SRAM1(); \
  TEN_STR_SRAM1(); \
  TEN_STR_SRAM1(); \
  TEN_STR_SRAM1(); \
  TEN_STR_SRAM1(); \
  TEN_STR_SRAM1(); \
  TEN_STR_SRAM1(); \
  TEN_STR_SRAM1(); \
  TEN_STR_SRAM1(); \
  TEN_STR_SRAM1(); \
} while(0)

#define THOUSAND_STR_SRAM1()do {\
  HUNDRED_STR_SRAM1(); \
  HUNDRED_STR_SRAM1(); \
  HUNDRED_STR_SRAM1(); \
  HUNDRED_STR_SRAM1(); \
  HUNDRED_STR_SRAM1(); \
  HUNDRED_STR_SRAM1(); \
  HUNDRED_STR_SRAM1(); \
  HUNDRED_STR_SRAM1(); \
  HUNDRED_STR_SRAM1(); \
  HUNDRED_STR_SRAM1(); \
} while(0)

//------------------------------------------------------------------------------
#define SETUP_CODE_LDR_PERIPH() do { \
  __asm volatile("mov r1, #0x0000"); \
  __asm volatile("movt r1,#0x4000"); \
} while(0)

#define LDR_PERIPH() do { \
  __asm volatile("ldr r0, [r1]"); \
} while(0)

#define TEN_LDR_PERIPH()do {\
  LDR_PERIPH(); \
  LDR_PERIPH(); \
  LDR_PERIPH(); \
  LDR_PERIPH(); \
  LDR_PERIPH(); \
  LDR_PERIPH(); \
  LDR_PERIPH(); \
  LDR_PERIPH(); \
  LDR_PERIPH(); \
  LDR_PERIPH(); \
} while(0)

#define HUNDRED_LDR_PERIPH()do {\
  TEN_LDR_PERIPH(); \
  TEN_LDR_PERIPH(); \
  TEN_LDR_PERIPH(); \
  TEN_LDR_PERIPH(); \
  TEN_LDR_PERIPH(); \
  TEN_LDR_PERIPH(); \
  TEN_LDR_PERIPH(); \
  TEN_LDR_PERIPH(); \
  TEN_LDR_PERIPH(); \
  TEN_LDR_PERIPH(); \
} while(0)

#define THOUSAND_LDR_PERIPH()do {\
  HUNDRED_LDR_PERIPH(); \
  HUNDRED_LDR_PERIPH(); \
  HUNDRED_LDR_PERIPH(); \
  HUNDRED_LDR_PERIPH(); \
  HUNDRED_LDR_PERIPH(); \
  HUNDRED_LDR_PERIPH(); \
  HUNDRED_LDR_PERIPH(); \
  HUNDRED_LDR_PERIPH(); \
  HUNDRED_LDR_PERIPH(); \
  HUNDRED_LDR_PERIPH(); \
} while(0)

//------------------------------------------------------------------------------  
#define STR_PERIPH() do { \
  __asm volatile("str r0, [r1]"); \
} while(0)

#define TEN_STR_PERIPH()do {\
  STR_PERIPH(); \
  STR_PERIPH(); \
  STR_PERIPH(); \
  STR_PERIPH(); \
  STR_PERIPH(); \
  STR_PERIPH(); \
  STR_PERIPH(); \
  STR_PERIPH(); \
  STR_PERIPH(); \
  STR_PERIPH(); \
} while(0)

#define HUNDRED_STR_PERIPH()do {\
  TEN_STR_PERIPH(); \
  TEN_STR_PERIPH(); \
  TEN_STR_PERIPH(); \
  TEN_STR_PERIPH(); \
  TEN_STR_PERIPH(); \
  TEN_STR_PERIPH(); \
  TEN_STR_PERIPH(); \
  TEN_STR_PERIPH(); \
  TEN_STR_PERIPH(); \
  TEN_STR_PERIPH(); \
} while(0)

#define THOUSAND_STR_PERIPH()do {\
  HUNDRED_STR_PERIPH(); \
  HUNDRED_STR_PERIPH(); \
  HUNDRED_STR_PERIPH(); \
  HUNDRED_STR_PERIPH(); \
  HUNDRED_STR_PERIPH(); \
  HUNDRED_STR_PERIPH(); \
  HUNDRED_STR_PERIPH(); \
  HUNDRED_STR_PERIPH(); \
  HUNDRED_STR_PERIPH(); \
  HUNDRED_STR_PERIPH(); \
} while(0)
//------------------------------------------------------------------------------
#define PUSH() do { \
  __asm volatile("push {r0,r1,lr}"); \
} while(0)

#define TEN_PUSH()do {\
  PUSH(); \
  PUSH(); \
  PUSH(); \
  PUSH(); \
  PUSH(); \
  PUSH(); \
  PUSH(); \
  PUSH(); \
  PUSH(); \
  PUSH(); \
} while(0)

#define HUNDRED_PUSH()do {\
  TEN_PUSH(); \
  TEN_PUSH(); \
  TEN_PUSH(); \
  TEN_PUSH(); \
  TEN_PUSH(); \
  TEN_PUSH(); \
  TEN_PUSH(); \
  TEN_PUSH(); \
  TEN_PUSH(); \
  TEN_PUSH(); \
} while(0)

#define THOUSAND_PUSH()do {\
  HUNDRED_PUSH(); \
  HUNDRED_PUSH(); \
  HUNDRED_PUSH(); \
  HUNDRED_PUSH(); \
  HUNDRED_PUSH(); \
  HUNDRED_PUSH(); \
  HUNDRED_PUSH(); \
  HUNDRED_PUSH(); \
  HUNDRED_PUSH(); \
  HUNDRED_PUSH(); \
} while(0)
//------------------------------------------------------------------------------
#define POP() do { \
  __asm volatile("pop {r0,r1,lr}"); \
} while(0)

#define TEN_POP()do {\
  POP(); \
  POP(); \
  POP(); \
  POP(); \
  POP(); \
  POP(); \
  POP(); \
  POP(); \
  POP(); \
  POP(); \
} while(0)

#define HUNDRED_POP()do {\
  TEN_POP(); \
  TEN_POP(); \
  TEN_POP(); \
  TEN_POP(); \
  TEN_POP(); \
  TEN_POP(); \
  TEN_POP(); \
  TEN_POP(); \
  TEN_POP(); \
  TEN_POP(); \
} while(0)

#define THOUSAND_POP()do {\
  HUNDRED_POP(); \
  HUNDRED_POP(); \
  HUNDRED_POP(); \
  HUNDRED_POP(); \
  HUNDRED_POP(); \
  HUNDRED_POP(); \
  HUNDRED_POP(); \
  HUNDRED_POP(); \
  HUNDRED_POP(); \
  HUNDRED_POP(); \
} while(0)
//------------------------------------------------------------------------------
// We have to put an udiv + a atomic instruction due to the M-Step limitation
// of not being able to trace multiple consecutive Restartable instructions.
// Then for the results I've to remove all measurements/samples corresponding to
// the atomic instruction. 
#define UDIV() do { \
  __asm volatile("udiv r0, r1, r1"); \
  __asm volatile("ldr r3, [sp]"); \
} while(0)

#define TEN_UDIV()do {\
  UDIV();\
  UDIV();\
  UDIV();\
  UDIV();\
  UDIV();\
  UDIV();\
  UDIV();\
  UDIV();\
  UDIV();\
  UDIV();\
} while(0)

#define HUNDRED_UDIV()do {\
  TEN_UDIV();\
  TEN_UDIV();\
  TEN_UDIV();\
  TEN_UDIV();\
  TEN_UDIV();\
  TEN_UDIV();\
  TEN_UDIV();\
  TEN_UDIV();\
  TEN_UDIV();\
  TEN_UDIV();\
} while(0)

#define THOUSAND_UDIV()do {\
  HUNDRED_UDIV();\
  HUNDRED_UDIV();\
  HUNDRED_UDIV();\
  HUNDRED_UDIV();\
  HUNDRED_UDIV();\
  HUNDRED_UDIV();\
  HUNDRED_UDIV();\
  HUNDRED_UDIV();\
  HUNDRED_UDIV();\
  HUNDRED_UDIV();\
} while(0)

// This is only used for evaluation purposes (Mstp-Debug)
#endif // EXPERIMENT_3
uint32_t *test_print_trace = (uint32_t *)  0x40000C24; // tim5_CNT

void victim_eval_inst_diff_mov(){
#ifdef EXPERIMENT_3
  printf("# victim_eval_inst_diff_mov\r\n");
  *test_print_trace = 0x2; // Start the trace and Disable the streak threshold
  THOUSAND_MOV();
  *test_print_trace = 0x0; // End the trace
  #endif // EXPERIMENT_3
}

void victim_eval_inst_diff_ldr_sram1(){
#ifdef EXPERIMENT_3
  printf("# victim_eval_inst_diff_ldr_sram1\r\n");
  *test_print_trace = 0x1; // Start the trace
  THOUSAND_LDR_SRAM1();
  *test_print_trace = 0x0; // End the trace
  #endif // EXPERIMENT_3
}

void victim_eval_inst_diff_str_sram1(){
  #ifdef EXPERIMENT_3
    printf("# victim_eval_inst_diff_str_sram1\r\n");
    *test_print_trace = 0x1; // Start the trace
    SETUP_CODE_STR_SRAM1();
    THOUSAND_STR_SRAM1();
    *test_print_trace = 0x0; // End the trace
    #endif // EXPERIMENT_3
  }

void victim_eval_inst_diff_ldr_periph(){
#ifdef EXPERIMENT_3
  printf("# victim_eval_inst_diff_ldr_periph\r\n");
  *test_print_trace = 0x1; // Start the trace
  SETUP_CODE_LDR_PERIPH();
  THOUSAND_LDR_PERIPH();
  *test_print_trace = 0x0; // End the trace
  #endif // EXPERIMENT_3
}

void victim_eval_inst_diff_str_periph(){
  #ifdef EXPERIMENT_3
    printf("# victim_eval_inst_diff_str_periph\r\n");
    *test_print_trace = 0x1; // Start the trace
    SETUP_CODE_LDR_PERIPH();
    THOUSAND_STR_PERIPH();
    *test_print_trace = 0x0; // End the trace
    #endif // EXPERIMENT_3
}

void victim_eval_inst_diff_push(){
  #ifdef EXPERIMENT_3
    printf("# victim_eval_inst_diff_push\r\n");
    for (uint8_t i = 0; i < 10; i++){
      *test_print_trace = 0x1; // Start the trace
      HUNDRED_PUSH();
      *test_print_trace = 0x0; // End the trace
      HUNDRED_POP(); 
    }
    #endif // EXPERIMENT_3
}

void victim_eval_inst_diff_pop(){
  #ifdef EXPERIMENT_3
    printf("# victim_eval_inst_diff_pop\r\n");
    for (uint8_t i = 0; i < 10; i++){
      HUNDRED_PUSH();
      *test_print_trace = 0x1; // Start the trace
      HUNDRED_POP(); 
      *test_print_trace = 0x0; // End the trace
    }
    #endif // EXPERIMENT_3
}

void victim_eval_inst_diff_udiv(){
  #ifdef EXPERIMENT_3
    printf("# victim_eval_inst_diff_udiv\r\n");
    *test_print_trace = 0x1; // Start the trace
    THOUSAND_UDIV();
    *test_print_trace = 0x0; // End the trace
    #endif // EXPERIMENT_3
}

////////////////////////////////////////////////////////////////////////////////
///////////////////////////////// Test #4 Victims //////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------------
// Cache Timing Analysis
//------------------------------------------------------------------------------

// I_CACHE_SIZE_BUFFER, this is a dummy function just to get an address which 
// maps to the CL_0  
__attribute__((optimize(0)))    
__attribute__((aligned(256*16)))
void cl_base() {                
  __asm volatile("mov r0, r0"); 
}                               

// Define the cache lines we want to access
#define DEFINE_CL(num) \
  const int cl_##num = (int)cl_base + (num)*16 ;

// Access CL by:
// - Load the address of the variable holding the address of the cache line
// - Load the address of the cache line
// - Load the content of that address to bring the cl into the cache
#define ACCESS_CL(x) do {             \
  __asm volatile("ldr r0, =cl_" #x);  \
  __asm volatile("ldr r0, [r0]");     \
  __asm volatile("ldr r0, [r0]");     \
} while(0)

// Define our covert channel. The CL we picked up are arbitrary within the range
// of the cache lines which are available (i.e., are not touched by the M-Step
// context-switch). Ideally, M-Step should create no noise on the cache. However,
// we fine-tune it having the M-Step assembly logic cached (and only that) thus,
// we still have some noise on the cache. In principle, there is no practically 
// limitation on putting all M-Step non-cacheable. We just did not spend the time
// tunning it further. 
DEFINE_CL(10)
DEFINE_CL(11)
DEFINE_CL(12)
DEFINE_CL(13)
DEFINE_CL(14)
DEFINE_CL(15)
DEFINE_CL(16)
DEFINE_CL(17)

void victim_eval_cache_base() {
  #ifdef EXPERIMENT_4
  printf("# victim_eval_cache_base\r\n");
  *test_print_trace = 0b100; // Start the trace
  __asm volatile("mov r0, r0");
  *test_print_trace = 0x0; // End the trace
  #endif // EXPERIMENT_4
}

// We are transmiting 0xFF via cache lines 10-17, i.e., all cache lines accessed
// which means all 8 bits of the covert channel are 1s.
// But any 8-bit value can be transmitted, just uncomment the corresponding CL
// to transmit a 0. For example, to transmit 0xFE, just comment the ACCESS_CL(17) 
void victim_eval_cache_trojan() {
  #ifdef EXPERIMENT_4
  printf("# victim_eval_cache_trojan\r\n");
  *test_print_trace = 0b100; // Start the trace
  ACCESS_CL(10);
  ACCESS_CL(11);
  ACCESS_CL(12);
  ACCESS_CL(13);
  ACCESS_CL(14);
  ACCESS_CL(15);
  ACCESS_CL(16);
  ACCESS_CL(17);
  *test_print_trace = 0x0; // End the trace
  #endif // EXPERIMENT_4
}

////////////////////////////////////////////////////////////////////////////////
///////////////////////////////// Test #5 Victims //////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#ifdef EXPERIMENT_5
//------------------------------------------------------------------------------
// Memory contention Timing Analysis
//------------------------------------------------------------------------------
#define SETUP_CODE_LDR_SRAM1() do { \
  __asm volatile("mov r1, #0x0000"); \
  __asm volatile("movt r1,#0x2000"); \
} while(0)

#define SETUP_CODE_LDR_APB1() do { \
  __asm volatile("mov r1, #0x0000"); \
  __asm volatile("movt r1,#0x4000"); \
} while(0)

#define SETUP_CODE_LDR_AHB2() do { \
  __asm volatile("mov r1, #0x0000"); \
  __asm volatile("movt r1,#0x4202"); \
} while(0)

#define LDR() do { \
  __asm volatile("ldr r0, [r1]"); \
} while(0)

#define TEN_LDR()do {\
  LDR(); \
  LDR(); \
  LDR(); \
  LDR(); \
  LDR(); \
  LDR(); \
  LDR(); \
  LDR(); \
  LDR(); \
  LDR(); \
} while(0)

#define HUNDRED_LDR()do {\
  TEN_LDR(); \
  TEN_LDR(); \
  TEN_LDR(); \
  TEN_LDR(); \
  TEN_LDR(); \
  TEN_LDR(); \
  TEN_LDR(); \
  TEN_LDR(); \
  TEN_LDR(); \
  TEN_LDR(); \
} while(0)

#define THOUSAND_LDR()do {\
  HUNDRED_LDR(); \
  HUNDRED_LDR(); \
  HUNDRED_LDR(); \
  HUNDRED_LDR(); \
  HUNDRED_LDR(); \
  HUNDRED_LDR(); \
  HUNDRED_LDR(); \
  HUNDRED_LDR(); \
  HUNDRED_LDR(); \
  HUNDRED_LDR(); \
} while(0)

#define LDR_FLASH() do { \
  __asm volatile("ldr r0, [pc]"); \
} while(0)

#define TEN_LDR_FLASH()do {\
  LDR_FLASH(); \
  LDR_FLASH(); \
  LDR_FLASH(); \
  LDR_FLASH(); \
  LDR_FLASH(); \
  LDR_FLASH(); \
  LDR_FLASH(); \
  LDR_FLASH(); \
  LDR_FLASH(); \
  LDR_FLASH(); \
} while(0)

#define HUNDRED_LDR_FLASH()do {\
  TEN_LDR_FLASH(); \
  TEN_LDR_FLASH(); \
  TEN_LDR_FLASH(); \
  TEN_LDR_FLASH(); \
  TEN_LDR_FLASH(); \
  TEN_LDR_FLASH(); \
  TEN_LDR_FLASH(); \
  TEN_LDR_FLASH(); \
  TEN_LDR_FLASH(); \
  TEN_LDR_FLASH(); \
} while(0)

#define THOUSAND_LDR_FLASH()do {\
  HUNDRED_LDR_FLASH(); \
  HUNDRED_LDR_FLASH(); \
  HUNDRED_LDR_FLASH(); \
  HUNDRED_LDR_FLASH(); \
  HUNDRED_LDR_FLASH(); \
  HUNDRED_LDR_FLASH(); \
  HUNDRED_LDR_FLASH(); \
  HUNDRED_LDR_FLASH(); \
  HUNDRED_LDR_FLASH(); \
  HUNDRED_LDR_FLASH(); \
} while(0)


#endif // EXPERIMENT_5

void victim_eval_cont() {
  #ifdef EXPERIMENT_5
  printf("# victim_eval_cont\r\n");
  *test_print_trace = 0b1001; // Start the trace
  SETUP_CODE_LDR_SRAM1();
  THOUSAND_LDR();

  SETUP_CODE_LDR_AHB2();
  THOUSAND_LDR();

  SETUP_CODE_LDR_APB1();
  THOUSAND_LDR();

  THOUSAND_LDR_FLASH();
  
  *test_print_trace = 0x0; // End the trace
  printf("# victim_eval_cont end\r\n\r\n");
  #endif // EXPERIMENT_5
}
