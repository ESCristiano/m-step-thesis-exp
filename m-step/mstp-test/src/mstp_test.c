#include "mstp_test.h"

#define NS      0
#define S_TFM   1
#define S_BARE  2

extern mstp_conf_t mstp_conf[2];

void config_mstp(mstp_conf_t *mstp_conf_test, uint8_t world) {
    if(mstp_conf_test == NULL){
        return;
    }
    switch (world){
        case NS:
            mstp_conf[0] = *mstp_conf_test;
            break;
        case S_BARE:
            mstp_conf[1] = *mstp_conf_test;
            break;
        case S_TFM:
            mstp_conf[1] = *mstp_conf_test;
            break;
        default:
            break;
    }
}

void run_no_trace(  mstp_test_conf_t test_conf, 
                    uint8_t conf_id, 
                    uint16_t test_id, 
                    uint8_t print_test_info,
                    uint8_t world){ 
    test_conf.board_config[conf_id](); 
    if(print_test_info){
        printf("---------------------------------------\r\n");
        printf("Board Config: %s\r\n", test_conf.config_name[conf_id]);
        printf("Victim %d: %s\r\n", test_id, test_conf.name[test_id]);
        printf("---------------------------------------\r\n");
    }
    config_mstp(test_conf.mstp_conf[test_id], world);
    test_conf.test[test_id](); 
}

void run_trace( mstp_test_conf_t test_conf,
                uint8_t conf_id, 
                uint16_t test_id, 
                uint8_t print_test_info,
                uint8_t world){ 
    test_conf.board_config[conf_id](); 
    if(print_test_info){
        printf("---------------------------------------\r\n");
        printf("Board Config: %s\r\n", test_conf.config_name[conf_id]);
        printf("Victim %d: %s\r\n", test_id, test_conf.name[test_id]);
        printf("---------------------------------------\r\n");
    }
    config_mstp(test_conf.mstp_conf[test_id], world);
    trace(test_conf.test[test_id]);
}
