#!/usr/bin/env bash
ROOT="$(realpath "$(dirname "$0")")"
S_DIR=${ROOT}/../m-step-poc-s
TFM=${S_DIR}/trusted-firmware-m
NSPE_MSTP_APP=${ROOT}/../m-step-poc-ns
MSTP=${ROOT}/../m-step

# Default values
TARGET="STM32L5"
PROFILE="bare"

#-------------------------------------------------------------------------------
# Parse arguments
#-------------------------------------------------------------------------------
set -e

DEPLOY=false  # default
RAW_TRACE=""  # default

while [[ $# -gt 0 ]]; do
    case "$1" in
        -b|--build)
            BUILD_TYPE="$2"
            if [[   "$BUILD_TYPE" != "s"        && 
                    "$BUILD_TYPE" != "ns" ]]; then
                echo "Error: Invalid build type '$BUILD_TYPE'. Supported values are 's', 'ns'."
                exit 1
            fi
            shift 2
            ;;
        -c|--config)
            CONFIG_TYPE="$2"
            if [[ "$CONFIG_TYPE" != "s" && "$CONFIG_TYPE" != "ns" ]]; then
                echo "Error: Invalid config type '$CONFIG_TYPE'. Supported values are 's' or 'ns'."
                exit 1
            fi
            shift 2
            ;;
        -p|--profile)
            PROFILE="$2"
            if [[   "$PROFILE" != "mstp" 
                ]]; then
                echo "Error: Invalid profile '$PROFILE'."
                exit 1
            fi
            shift 2
            ;;
        -t|--target)
            TARGET="$2"
            shift 2
            ;;
        -d|--deploy)
            DEPLOY=true
            shift
            ;;
        -m|--monitor)
            RAW_TRACE="$2"
            if [[ -z "$RAW_TRACE" ]]; then
                echo "Error: -m|--monitor requires an output file path."
                exit 1
            fi
            shift 2
            ;;
        *)
            echo "Unknown argument: $1"
            echo "Usage: $0 [-b|--build <s|ns>] [-c|--config <s|ns>] [-t|--target <BoardName>] [-d|--deploy] [-p|--profile <bare|crypto|mstp>] [-m|--monitor <output_file>]"
            exit 1
            ;;
    esac
done

#-------------------------------------------------------------------------------
# Decode target 
#-------------------------------------------------------------------------------
if [[ "${TARGET}" == *"L5"* ]]; then
        PLATFORM="nucleo_l552ze_q"
elif [[ "${TARGET}" == *"U5"* ]]; then
        PLATFORM="b_u585i_iot02a"
else
        echo "Unknown target: ${TARGET}"
        exit 1
fi

CONFIG="${ROOT}/build/${PLATFORM}_${PROFILE}_${CONFIG_TYPE}"

#-------------------------------------------------------------------------------
# Configure SPE build system 
#-------------------------------------------------------------------------------
if [[ "${CONFIG_TYPE}" == "s" ]]; then

        cmake   -S ${TFM}                                               \
                -B ${CONFIG}                                            \
                -C ${ROOT}/tfm-configs/${PLATFORM}-${PROFILE}-config.cmake

fi

#-------------------------------------------------------------------------------
# Configure NS build system
#-------------------------------------------------------------------------------
if [[ "${CONFIG_TYPE}" == "ns" ]]; then
    cmake   -G "Eclipse CDT4 - Unix Makefiles" ${NSPE_MSTP_APP}/src \
            -B ${NSPE_MSTP_APP}/build/
fi

BUILD="${ROOT}/build/${PLATFORM}_${PROFILE}_${BUILD_TYPE}"

#-------------------------------------------------------------------------------
# Build SPE build system 
#-------------------------------------------------------------------------------
if [[ "${BUILD_TYPE}" == "s" ]]; then
        cmake --build ${BUILD} -- install

        # This file is missing from the SPE installation, maybe a TFM bug when
        # building Mbed TLS out of tree. 
        if [[ ! -f ${BUILD}/api_ns/interface/include/mbedtls/mbedtls_config.h ]]; then
            cp ${S_DIR}/mbedtls/include/mbedtls/mbedtls_config.h \
               ${BUILD}/api_ns/interface/include/mbedtls
        fi
fi

#-------------------------------------------------------------------------------
# Build NS (Baremetal)
#-------------------------------------------------------------------------------
if [[ "${BUILD_TYPE}" == "ns" ]]; then
        BUILD_S="${ROOT}/build/${PLATFORM}_${PROFILE}_s"
            
        cmake --build ${NSPE_MSTP_APP}/build

        arm-none-eabi-objcopy -O binary \
        ${NSPE_MSTP_APP}/src/NonSecure/build/tfm_ns.elf \
        ${NSPE_MSTP_APP}/src/NonSecure/build/tfm_ns.bin

        imgtool sign \
          -k ${BUILD_S}/api_ns/image_signing/keys/image_ns_signing_private_key.pem \
          --public-key-format full \
          --align 1 \
          -v 0.0.0 \
          -s 1 \
          -H 1024 \
          --pad-header \
          -S 0x10000 \
          --pad \
          --boot-record boot \
          ${NSPE_MSTP_APP}/src/NonSecure/build/tfm_ns.bin \
          ${BUILD_S}/api_ns/image_signing/scripts/tfm_ns_signed.bin
fi

#-------------------------------------------------------------------------------
# Flash TFM
#-------------------------------------------------------------------------------
if $DEPLOY; then        
        BUILD_S="${ROOT}/build/${PLATFORM}_${PROFILE}_s"
        TFM_SPE="$(realpath ${BUILD_S})"

        echo "Deploying for target ${TARGET} with profile ${TFM_SPE}"
        
        ${TFM_SPE}/api_ns/postbuild.sh

        #-----------------------------------------------------------------------
        # ATTENTION!
        #-----------------------------------------------------------------------
        # The script below only needs to run once to set up the option bytes.
        # If this is your first time running it, please uncomment the line below.
        # Otherwise, you likely won't be able to run the code on your board.
        # You can leave it uncommented, but it will add unnecessary time to each 
        # run. Once the board's option bytes are configured, they persist across 
        # power cycles, so you can leave this line commented out after the 
        # initial setup.
        #
        # ${TFM_SPE}/api_ns/regression.sh
        #-----------------------------------------------------------------------

        ${TFM_SPE}/api_ns/TFM_UPDATE.sh      
fi

#-------------------------------------------------------------------------------
# Log results
#-------------------------------------------------------------------------------
if [[ -n "${RAW_TRACE}" ]]; then
        python3 ${MSTP}/mstp-monitor/monitor.py \
                -o ${RAW_TRACE}
fi
