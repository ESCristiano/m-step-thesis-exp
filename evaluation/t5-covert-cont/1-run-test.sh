#!/usr/bin/env bash
ROOT="$(realpath "$(dirname "$0")")"

GRAPH_SCRIPT="${ROOT}/../graphs/scripts"
GRAPH_SCRIPT_DIR="$(realpath "${GRAPH_SCRIPT}")"

MSTP="${ROOT}/../../m-step"
MSTP_DIR="$(realpath "${MSTP}")"

NS="${ROOT}/../../ns-world-bare"
NS_DIR="$(realpath "${NS}")"

LOG_DIR=${ROOT}/logs
RAW_TRACE=${LOG_DIR}/0_raw_trace.txt
RESULTS=${LOG_DIR}/1_results.txt
MATRIX_FILE=${LOG_DIR}/06-contention.txt

# Static Configuration Values
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
# Clean up if requested
#-------------------------------------------------------------------------------
if [ "$CLEAN" = true ]; then
    echo "Cleaning logs directory..."
    rm -r ${LOG_DIR}/* || true 
    ${NS_DIR}/0-config.sh -c
    echo "Clean completed."
    exit 0
fi

#-------------------------------------------------------------------------------
# Configure target
#-------------------------------------------------------------------------------
${NS_DIR}/0-config.sh 

#-------------------------------------------------------------------------------
# Build target & Deploy
#-------------------------------------------------------------------------------
TEST_CONFIG="${MSTP_DIR}/mstp-eval/inc/test_eval_config.h"
TEST_CONFIG_S="${MSTP_DIR}/mstp-victims/s/inc/test_eval_config.h"

# Enable TEST for both configs
for config in "${TEST_CONFIG}" "${TEST_CONFIG_S}"; do
    rm -rf "${config}" || true
    cat > "${config}" << 'EOF'
#define TEST5_ENABLE

EOF
done

${NS_DIR}/1-compile.sh --test-mode
${NS_DIR}/2-deploy.sh

#-------------------------------------------------------------------------------
# Log results
#-------------------------------------------------------------------------------
${NS_DIR}/3-monitor.sh -o ${RAW_TRACE}

#-------------------------------------------------------------------------------
# Gen Results && Matrix
#-------------------------------------------------------------------------------
python3 ${ROOT}/gen_results.py      \
        -i ${RAW_TRACE}             \
        -o ${RESULTS}

#-------------------------------------------------------------------------------
# Create Matrix File:
# - First part is a static header defining the experiment;
# - Second part is the results generated previously, which are appended.
#-------------------------------------------------------------------------------
# Header
cat > ${MATRIX_FILE} << 'EOF'
Color_type: Probability #Integer
Gradient: orange
Min_number_of_colors: 100
Y: SRAM1 AHB2 APB1 FLASH
X: SRAM1 AHB2 APB1 FLASH

EOF

# Append results to matrix file
cat ${RESULTS} >> ${MATRIX_FILE}

echo "Matrix file created: ${MATRIX_FILE}"
#-------------------------------------------------------------------------------
# Generate the graphic and open it
#-------------------------------------------------------------------------------
python3 ${GRAPH_SCRIPT_DIR}/gen_matrix.py   \
    -p ${MATRIX_FILE}                       \
    -o ${LOG_DIR}

# xdg-open ${LOG_DIR}/*.png || true