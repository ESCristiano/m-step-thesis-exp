#!/usr/bin/env bash
ROOT="$(realpath "$(dirname "$0")")"

COPILOT="${ROOT}/../copilot/copilot.sh"

MSTP_DIR="$(realpath "${ROOT}/../m-step")"
TEST_CONFIG="${MSTP_DIR}/mstp-eval/inc/test_eval_config.h"

# Static Configuration Values
TARGET="STM32L5"
BUILD_TYPE="ns"
PROFILE="mstp"

#-------------------------------------------------------------------------------
# Parse arguments
#-------------------------------------------------------------------------------
set -e

BUILD_ALL=true
TEST_MODE=false

while [[ $# -gt 0 ]]; do
    case "$1" in
        -b|--build)
            BUILD_ALL=false
            WORLD="$2"
            if [[   "$WORLD" != "s"        && 
                    "$WORLD" != "ns"  ]]; then
                echo "Error: Invalid build type '$WORLD'. Supported values are 's', 'ns'."
                exit 1
            fi
            shift 2
            ;;
        -t|--test-mode)
            TEST_MODE=true
            shift
            ;;
        -h|--help)
            echo "Usage: $0 [-b|--build <s|ns>] [-t|--test-mode]"
            exit 0
            ;;
        *)
            echo "Unknown argument: $1"
            echo "Usage: $0 [-b|--build <s|ns>] [-t|--test-mode]"
            exit 1
            ;;
    esac
done

#-------------------------------------------------------------------------------
# Build S and NS 
#-------------------------------------------------------------------------------
if [[ "$BUILD_ALL" == "true" ]]; then
    echo "Building for target $TARGET with profile $PROFILE"

    if [[ "$TEST_MODE" == "false" ]]; then
        # Disable all test configurations and creating an empty one. This will not
        # make all tests disabled by default. If you wanna run tests, you should go
        # to the evaluation folder and run the specific scripts defined there or, 
        # provide your own test config file.
        rm -rf "${TEST_CONFIG}" || true
        touch "${TEST_CONFIG}" || true
    fi

    # Build S
    ${COPILOT} -b s -t ${TARGET} -p ${PROFILE}

    # Build NS
    ${COPILOT} -b ${BUILD_TYPE} -t ${TARGET} -p ${PROFILE}
fi

#-------------------------------------------------------------------------------
# Build only one image at the time
#-------------------------------------------------------------------------------
if [[ "$BUILD_ALL" == "false" ]]; then
    echo "Building for target $TARGET with profile $PROFILE" for $BUILD_TYPE world
    
    if [[ "$TEST_MODE" == "false" ]]; then
        # Disable all test configurations and creating an empty one. This will not
        # make all tests disabled by default. If you wanna run tests, you should go
        # to the evaluation folder and run the specific scripts defined there or, 
        # provide your own test config file.
        rm -rf "${TEST_CONFIG}" || true
        touch "${TEST_CONFIG}" || true
    fi

    if [[ "$WORLD" == "s" ]]; then
        # Build S
        ${COPILOT} -b s -t ${TARGET} -p ${PROFILE}
    elif [[ "$WORLD" == "ns" ]]; then
        # Build NS
        ${COPILOT} -b ${BUILD_TYPE} -t ${TARGET} -p ${PROFILE}
    fi
fi