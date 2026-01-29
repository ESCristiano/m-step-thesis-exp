#ifndef INC_BOARD_SETUP_H_
#define INC_BOARD_SETUP_H_

typedef enum {
    BOARD_DEFAULT,
    BOARD_NO_CACHE,
    BOARD_FPU,
    BOARD_FPU_LAZY,
    N_BOARD_CONF
} poc_board_conf_t;

extern void (*board_conf[N_BOARD_CONF])();
extern char *board_conf_names[N_BOARD_CONF];

#endif 