############################ Base CONFIGURATION ################################
# Base path: directory of this config file
set(CONFIG_BASE_PATH "${CMAKE_CURRENT_LIST_DIR}/../../m-step-poc-s/" CACHE INTERNAL "Base path for relative resolution")

set(CMAKE_BUILD_TYPE Debug CACHE STRING "")
set(TFM_PLATFORM stm/nucleo_l552ze_q CACHE STRING "")

set(TFM_TOOLCHAIN_FILE "${CONFIG_BASE_PATH}/trusted-firmware-m/toolchain_GNUARM.cmake" CACHE FILEPATH "")
set(CONFIG_TFM_SOURCE_PATH "${CONFIG_BASE_PATH}/trusted-firmware-m" CACHE PATH "")
set(MBEDCRYPTO_PATH "${CONFIG_BASE_PATH}/mbedtls" CACHE PATH "")

set(TFM_PROFILE profile_large CACHE STRING "")
# Level 2 is the only supported on this platform
set(TFM_ISOLATION_LEVEL 2 CACHE STRING "") 