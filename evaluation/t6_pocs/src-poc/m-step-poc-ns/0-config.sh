#!/usr/bin/env bash
ROOT="$(realpath "$(dirname "$0")")"

COPILOT="${ROOT}/../copilot/copilot.sh"
COPILOT_DIR="$(dirname "${COPILOT}")"

# Static Configuration Values
TARGET="STM32L5"
CONFIG=true
BUILD_TYPE="ns_costum"
PROFILE="mstp"
CLEAN=false

#-------------------------------------------------------------------------------
# Parse arguments
#-------------------------------------------------------------------------------
set -e

while [[ $# -gt 0 ]]; do
    case "$1" in
        -c|--clean)
            CLEAN=true
            shift 
            ;;
        -h|--help)
            echo "Usage: $0 [-c|--clean]"
            exit 0
            ;;
        *)
            echo "Unknown argument: $1"
            echo "Usage: $0 [-c|--clean]"
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

#-------------------------------------------------------------------------------
# Clean Build Directories
#-------------------------------------------------------------------------------
if [[ "$CLEAN" == "true" ]]; then
    echo "Cleaning NS and TF-M building directories."
    
    rm -rf ${ROOT}/build  \
           ${ROOT}/src/NonSecure/build/ \
           ${ROOT}/src/Secure/build/ \
           ${COPILOT_DIR}/build/${PLATFORM}_${PROFILE}_s

else
#-------------------------------------------------------------------------------
# Config S and NS
#-------------------------------------------------------------------------------
    ${COPILOT} -c s -t ${TARGET} -p ${PROFILE}

    # NS Config
    cmake   -G "Eclipse CDT4 - Unix Makefiles" ${ROOT}/src \
            -B ${ROOT}/build/
fi