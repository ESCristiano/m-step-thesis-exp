#ifndef INC_MSTP_TEST_H
#define INC_MSTP_TEST_H

#include "mstp.h"

#define NO_PRINT 0
#define PRINT    1

typedef struct {
    void (**board_config)();// Pointer to array of configurations
    char **config_name;     // Pointer to array of config names
    void (**test)();        // Pointer to array of tests
    char **name;            // Pointer to array of tests names
    mstp_conf_t **mstp_conf; // M-Step array of configuration per test
    uint32_t n_experiments; 
} mstp_test_conf_t;

void config_mstp(mstp_conf_t *, uint8_t);
void run_no_trace( mstp_test_conf_t, uint8_t, uint16_t, uint8_t, uint8_t);
void run_trace( mstp_test_conf_t, uint8_t, uint16_t, uint8_t, uint8_t);
    
#endif 
