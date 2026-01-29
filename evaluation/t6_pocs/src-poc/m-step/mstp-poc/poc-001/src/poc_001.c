#include "poc_001.h"

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "psa/client.h"
#include "psa_manifest/sid.h"
#include "psa/crypto.h"

// Victim function pointers
void (*poc[N_POC_001])() = {
  poc_001,
  poc_002
};

char *poc_names[N_POC_001] = {
  "poc_001",
  "poc_002"
};

//------------------------------------------------------------------------------
// We are invoking the key generation functoin in both PoCs. In PoC#1 is the native
// implementation to request the generation of a Key Pair; On PoC#2 we are using
// it to get an entry point on the crypto service and to execute a rsa_decrypt.c
// like example. He had to adpapt the example mbedtls provide to run on TF-M.
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