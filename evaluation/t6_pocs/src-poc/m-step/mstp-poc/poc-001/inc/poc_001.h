#ifndef INC_POC_001_H_
#define INC_POC_001_H_

typedef enum {
    POC_001,
    POC_002,
    POC_001_MAX
} poc_001_conf_t;

#define N_POC_001 POC_001_MAX

//------------------------------------------------------------------------------
// TF-M: Victims running on a TA on TF-M
//------------------------------------------------------------------------------
void poc_001();
void poc_002();

extern void (*poc[N_POC_001])();
extern char *poc_names[N_POC_001] ;

#endif 
