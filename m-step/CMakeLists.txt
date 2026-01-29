cmake_minimum_required(VERSION 3.22)

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

enable_language(C ASM)

set (MSTP_DRIVERS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Drivers)

# add_subdirectory(mstp-victims/ns)

#-------------------------------------------------------------------------------
# TFM NS API Libraries
#-------------------------------------------------------------------------------
set(TFM_BUILD "${CMAKE_SOURCE_DIR}/../../../copilot/build/nucleo_l552ze_q_mstp_s")
set(CONFIG_SPE_PATH "${TFM_BUILD}/api_ns")

# Interface files exported from TF-M secure build
set(SPE_INSTALL_INTERFACE_INC    ${CONFIG_SPE_PATH}/interface/include)
set(SPE_INSTALL_INTERFACE_SRC    ${CONFIG_SPE_PATH}/interface/src)

# Include SPE configuration
list(APPEND CMAKE_MODULE_PATH ${CONFIG_SPE_PATH}/cmake)
include(${CONFIG_SPE_PATH}/cmake/spe_config.cmake)
include(spe_config)
include(spe_export)

# Use existing TFM API libraries if they exist, otherwise create them
if(NOT TARGET tfm_api_ns)
    add_library(tfm_api_ns STATIC)

    target_sources(tfm_api_ns
        PUBLIC
            $<$<BOOL:${TFM_PARTITION_PLATFORM}>:${SPE_INSTALL_INTERFACE_SRC}/tfm_platform_api.c>
            $<$<BOOL:${TFM_PARTITION_PROTECTED_STORAGE}>:${SPE_INSTALL_INTERFACE_SRC}/tfm_ps_api.c>
            $<$<BOOL:${TFM_PARTITION_INTERNAL_TRUSTED_STORAGE}>:${SPE_INSTALL_INTERFACE_SRC}/tfm_its_api.c>
            $<$<BOOL:${TFM_PARTITION_CRYPTO}>:${SPE_INSTALL_INTERFACE_SRC}/tfm_crypto_api.c>
            $<$<BOOL:${TFM_PARTITION_INITIAL_ATTESTATION}>:${SPE_INSTALL_INTERFACE_SRC}/tfm_attest_api.c>
            $<$<BOOL:${TFM_PARTITION_FIRMWARE_UPDATE}>:${SPE_INSTALL_INTERFACE_SRC}/tfm_fwu_api.c>
    )

    target_include_directories(tfm_api_ns
        PUBLIC
            ${SPE_INSTALL_INTERFACE_INC}
            ${SPE_INSTALL_INTERFACE_INC}/crypto_keys
    )

    if (TFM_PARTITION_CRYPTO)
        target_link_libraries(tfm_api_ns
            PUBLIC
                psa_crypto_config
        )
    endif()
endif()

if(NOT TARGET tfm_api_ns_tz AND CONFIG_TFM_USE_TRUSTZONE)
    add_library(tfm_api_ns_tz INTERFACE)

    target_sources(tfm_api_ns_tz
        INTERFACE
            ${SPE_INSTALL_INTERFACE_SRC}/tfm_tz_psa_ns_api.c
    )

    target_link_libraries(tfm_api_ns_tz
        INTERFACE
            ${CONFIG_SPE_PATH}/interface/lib/s_veneers.o
    )
endif()


#-------------------------------------------------------------------------------
# M-Step Platform library
#-------------------------------------------------------------------------------
add_library(mstp_platform 
    STATIC
        ${MSTP_DRIVERS_DIR}/stm32l5xx-hal-driver/Src/stm32l5xx_hal.c
        ${MSTP_DRIVERS_DIR}/stm32l5xx-hal-driver/Src/stm32l5xx_hal_cortex.c
        ${MSTP_DRIVERS_DIR}/stm32l5xx-hal-driver/Src/stm32l5xx_hal_dma.c
        ${MSTP_DRIVERS_DIR}/stm32l5xx-hal-driver/Src/stm32l5xx_hal_dma_ex.c
        ${MSTP_DRIVERS_DIR}/stm32l5xx-hal-driver/Src/stm32l5xx_hal_exti.c
        ${MSTP_DRIVERS_DIR}/stm32l5xx-hal-driver/Src/stm32l5xx_hal_flash.c
        ${MSTP_DRIVERS_DIR}/stm32l5xx-hal-driver/Src/stm32l5xx_hal_flash_ex.c
        ${MSTP_DRIVERS_DIR}/stm32l5xx-hal-driver/Src/stm32l5xx_hal_flash_ramfunc.c
        ${MSTP_DRIVERS_DIR}/stm32l5xx-hal-driver/Src/stm32l5xx_hal_gpio.c
        ${MSTP_DRIVERS_DIR}/stm32l5xx-hal-driver/Src/stm32l5xx_hal_gtzc.c
        ${MSTP_DRIVERS_DIR}/stm32l5xx-hal-driver/Src/stm32l5xx_hal_i2c.c
        ${MSTP_DRIVERS_DIR}/stm32l5xx-hal-driver/Src/stm32l5xx_hal_i2c_ex.c
        ${MSTP_DRIVERS_DIR}/stm32l5xx-hal-driver/Src/stm32l5xx_hal_icache.c
        ${MSTP_DRIVERS_DIR}/stm32l5xx-hal-driver/Src/stm32l5xx_hal_pwr.c
        ${MSTP_DRIVERS_DIR}/stm32l5xx-hal-driver/Src/stm32l5xx_hal_pwr_ex.c
        ${MSTP_DRIVERS_DIR}/stm32l5xx-hal-driver/Src/stm32l5xx_hal_rcc.c
        ${MSTP_DRIVERS_DIR}/stm32l5xx-hal-driver/Src/stm32l5xx_hal_rcc_ex.c
        ${MSTP_DRIVERS_DIR}/stm32l5xx-hal-driver/Src/stm32l5xx_hal_tim.c
        ${MSTP_DRIVERS_DIR}/stm32l5xx-hal-driver/Src/stm32l5xx_hal_tim_ex.c
        ${MSTP_DRIVERS_DIR}/stm32l5xx-hal-driver/Src/stm32l5xx_hal_uart.c
        ${MSTP_DRIVERS_DIR}/stm32l5xx-hal-driver/Src/stm32l5xx_hal_uart_ex.c 
) 

target_include_directories(mstp_platform 
    PUBLIC
        ${MSTP_DRIVERS_DIR}/CMSIS/Device/Include
        ${MSTP_DRIVERS_DIR}/CMSIS/Include
        ${MSTP_DRIVERS_DIR}/stm32l5xx-hal-driver/Inc
    PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}/mstp/inc
)

#-------------------------------------------------------------------------------
# M-Step library
#-------------------------------------------------------------------------------
add_library(mstp 
    STATIC
        ${CMAKE_CURRENT_SOURCE_DIR}/mstp/src/mstp.c
        ${CMAKE_CURRENT_SOURCE_DIR}/mstp/src/mstp.s
)

target_include_directories(mstp 
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/mstp/inc
        ${CMAKE_CURRENT_SOURCE_DIR}/mstp-eval/inc
        ${CMAKE_CURRENT_SOURCE_DIR}/mstp-busted/inc
        ${CMAKE_CURRENT_SOURCE_DIR}/mstp-cache/inc
        ${CMAKE_CURRENT_SOURCE_DIR}/mstp-metrics/inc
)

target_link_libraries(mstp 
    PRIVATE
        mstp_platform
        mstp_metrics
        mstp_busted
        mstp_cache
)

#-------------------------------------------------------------------------------
# M-Step Evaluation library
#-------------------------------------------------------------------------------
add_library(mstp_eval 
    STATIC
        ${CMAKE_CURRENT_SOURCE_DIR}/mstp-eval/src/mstp_eval.c
        ${CMAKE_CURRENT_SOURCE_DIR}/mstp-eval/src/mstp_eval_poc.c
        ${CMAKE_CURRENT_SOURCE_DIR}/mstp-eval/src/mstp_board_setup.c
        ${CMAKE_CURRENT_SOURCE_DIR}/mstp-eval/src/mstp_eval_mstp_metrics.c
        ${CMAKE_CURRENT_SOURCE_DIR}/mstp-eval/src/mstp_eval_covert_ch.c
)

target_include_directories(mstp_eval 
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/mstp-eval/inc
        ${SPE_INSTALL_INTERFACE_INC}
)

target_link_libraries(mstp_eval 
    PRIVATE
        mstp
        mstp_platform
        mstp_opdecoder
        mstp_test
    PUBLIC
        tfm_api_ns
        $<$<BOOL:${CONFIG_TFM_USE_TRUSTZONE}>:tfm_api_ns_tz>
)

#-------------------------------------------------------------------------------
# M-Step Test 
#-------------------------------------------------------------------------------
add_library(mstp_test
    STATIC
        ${CMAKE_CURRENT_SOURCE_DIR}/mstp-test/src/mstp_test.c
)
        
target_include_directories(mstp_test 
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/mstp-test/inc
)

target_link_libraries(mstp_test 
    PRIVATE
        mstp
        mstp_platform
)

#-------------------------------------------------------------------------------
# M-Step BUSted Plugin
#-------------------------------------------------------------------------------
add_library(mstp_busted
    STATIC
        ${CMAKE_CURRENT_SOURCE_DIR}/mstp-busted/src/mstp_busted.c
)
        
target_include_directories(mstp_busted 
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/mstp-busted/inc
)

target_link_libraries(mstp_busted 
    PRIVATE
        mstp
        mstp_platform
)

#-------------------------------------------------------------------------------
# M-Step Cache Plugin
#-------------------------------------------------------------------------------
add_library(mstp_cache
    STATIC
        ${CMAKE_CURRENT_SOURCE_DIR}/mstp-cache/src/mstp_cache.c
        ${CMAKE_CURRENT_SOURCE_DIR}/mstp-cache/src/uArchChannel_lib.c
        ${CMAKE_CURRENT_SOURCE_DIR}/mstp-cache/src/i_cache_buffer_ns.s
)
        
target_include_directories(mstp_cache 
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/mstp-cache/inc
)

target_link_libraries(mstp_cache
    PRIVATE
        mstp
        mstp_platform
)

#-------------------------------------------------------------------------------
# M-Step Metrics Plugin
#-------------------------------------------------------------------------------
add_library(mstp_metrics
    STATIC
        ${CMAKE_CURRENT_SOURCE_DIR}/mstp-metrics/src/mstp_metrics.c
)
        
target_include_directories(mstp_metrics 
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/mstp-metrics/inc
)

target_link_libraries(mstp_metrics
    PRIVATE
        mstp
        mstp_platform
        mstp_opdecoder
)

#-------------------------------------------------------------------------------
# M-Step Metrics Plugin
#-------------------------------------------------------------------------------
add_library(mstp_opdecoder
    STATIC
        ${CMAKE_CURRENT_SOURCE_DIR}/mstp-opdecoder/src/armv8_decop_lib.c
)
        
target_include_directories(mstp_opdecoder 
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/mstp-opdecoder/inc
)

target_link_libraries(mstp_opdecoder
    PRIVATE
        mstp
        mstp_platform
)

