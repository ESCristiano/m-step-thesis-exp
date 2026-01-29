#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "psa/client.h"
#include "psa_manifest/sid.h"
#include "psa/crypto.h"
#include "mstp_eval_poc.h"
#include "mstp_board_setup.h"

//------------------------------------------------------------------------------
// Victim function pointers
//------------------------------------------------------------------------------
void poc_001();
void poc_002();

void (*poc[N_POCS])() = {
  poc_001,
  poc_002
};

char *poc_names[N_POCS] = {
  "poc_001",
  "poc_002"
};

//------------------------------------------------------------------------------
// Test specific configs
//------------------------------------------------------------------------------
mstp_conf_t mstp_conf_aux = 
{
  .base_ISR_time          = BASE_ISR_TIME_S,
  .base_clk               = BASE_CLK_S,
  .base_clk_2_collide     = BASE_CLK_CONTENTION_S,
  .streak_threshold       = STREAK_THRESHOLD,
  .ici_streak_threshold   = ICI_STREAK_THRESHOLD,
  .preemptions_threshold  = PREEMPTIONS_THRESHOLD,
  .base_inst_time         = BASE_INST_TIME,   
  .debug                  = DEBUG_ON,
  .trace_enable           = ENABLE_TRACE, 
  .print_iri_latency      = DISABLE_IRI_PRINT,
  .print_ici_latency      = DISABLE_ICI_PRINT,
  .mstp_cache_enabled     = DISABLE_MSTP_CACHE,
  .mstp_busted_enabled    = DISABLE_MSTP_BUSTED,
  .start_trace_window     = 0,
  .end_trace_window       = (uint64_t) -1 // MAX_UINT64
};

mstp_conf_t *mstp_conf_poc[N_POCS] = 
{
  &mstp_conf_aux,
  &mstp_conf_aux
};

mstp_test_conf_t test_conf_poc = {
    .board_config   = board_conf,
    .config_name    = board_conf_names,
    .test           = poc,
    .name           = poc_names,
    .mstp_conf      = mstp_conf_poc,
    .n_experiments  = N_POCS
};

//------------------------------------------------------------------------------
// We are invoking the key generation functoin in both PoCs. In PoC#1 is the native
// implementation to request the generation of a Key Pair; On PoC#2 we are using
// it to get an entry point on the crypto service and to execute a rsa_decrypt.c
// like example. He had to adapt the example mbedtls provide to run on TF-M.
//
// Conditions:
// - if usage_flags are PSA_KEY_USAGE_EXPORT, normal key generation is performed.
// - if usage_flags are PSA_KEY_USAGE_DECRYPT | PSA_KEY_USAGE_EXPORT, the code
//    will execute the code associated with PoC#2
//------------------------------------------------------------------------------
psa_status_t invoke_poc_code_tfm( uint8_t poc_id) {
  psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
  psa_key_id_t key_id = 0;
  size_t key_bits = 1024;

  if(poc_id == 2) {
    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_DECRYPT | PSA_KEY_USAGE_EXPORT);
  } else {
    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_EXPORT);
  }
  psa_set_key_algorithm(&attributes, PSA_ALG_RSA_PKCS1V15_CRYPT);
  psa_set_key_type(&attributes, PSA_KEY_TYPE_RSA_KEY_PAIR);
  psa_set_key_bits(&attributes, key_bits);
  
  psa_status_t status = psa_generate_key(&attributes, &key_id);
  psa_reset_key_attributes(&attributes);
  
  return status;
}

//------------------------------------------------------------------------------
// Request RSA key generaton
//------------------------------------------------------------------------------
void poc_001() {
  psa_status_t status;

  printf("PoC#1 Start.\r\n");
  
  status = psa_crypto_init();
  if (status != PSA_SUCCESS) {
    printf("PoC#1 Failed to init: %d\r\n", status);
    return;
  }
  
  invoke_poc_code_tfm(1);
  
  if (status != PSA_SUCCESS) {
    printf("PoC#1 Failed: %d\r\n", status);
    return;
  }

  printf("PoC#1 executed successfully.\r\n");

}

//------------------------------------------------------------------------------
// Invoke program emulating the Mbed TLS example program.
//------------------------------------------------------------------------------
void poc_002() {
  psa_status_t status;

  printf("PoC#2 Start.\r\n");
  
  status = psa_crypto_init();
  if (status != PSA_SUCCESS) {
    printf("PoC#2 Failed to init: %d\r\n", status);
    return;
  }

  invoke_poc_code_tfm(2);

  if (status != PSA_SUCCESS) {
    printf("PoC#2 Failed: %d\r\n", status);
    return;
  }

  printf("PoC#2 executed successfully.\r\n");

}