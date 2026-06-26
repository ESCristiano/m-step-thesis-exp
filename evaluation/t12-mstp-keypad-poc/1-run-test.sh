#!/usr/bin/env bash
ROOT="$(realpath "$(dirname "$0")")"

COPILOT_DIR="$(realpath "${ROOT}/../../copilot")"
GRAPH_SCRIPT_DIR="$(realpath "${ROOT}/../graphs/scripts")"
MSTP_DIR="$(realpath "${ROOT}/../../m-step")"
NS_DIR="$(realpath "${ROOT}/../../ns-world-bare")"
PAPER_CHAPTER_DIR="$(realpath "${ROOT}/../../../../src/Chapters")"

LOG_DIR=${ROOT}/logs
RAW_TRACE=${LOG_DIR}/0_raw_trace.txt
OUTPUTS=${ROOT}/outputs
PYTHON_BIN=/nix/store/03iibjzr6bmh3b0h1cw2vhcfpjcn9mww-python3-3.12.8-env/bin/python

TEST_NAME="C04-toy_attack_nemesis"
MATRIX_FILE=${LOG_DIR}/${TEST_NAME}.txt
PATTERN=${LOG_DIR}/${TEST_NAME}_aux.txt

CHAPTER=04-uArch
PAPER_CHAPTER_IMG_DIR=${PAPER_CHAPTER_DIR}/${CHAPTER}/1-imgs/C04-toy_nemesis

# Static Configuration Values
CLEAN=false
VISUALIZE_ONLY=false

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
        -vi|--visualize)
            VISUALIZE_ONLY=true
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
    rm -r ${OUTPUTS}/* || true
    ${NS_DIR}/0-config.sh -c
    echo "Clean completed."
    exit 0
fi

#-------------------------------------------------------------------------------
# Draw graphs only
#-------------------------------------------------------------------------------
if [ "$VISUALIZE_ONLY" = true ]; then
    "${PYTHON_BIN}" ${GRAPH_SCRIPT_DIR}/gen_matrix.py \
            -p "${MATRIX_FILE}"           \
            -o "${OUTPUTS}/"

    xdg-open ${OUTPUTS}/${TEST_NAME}_matrix.png
        exit 0
fi

#-------------------------------------------------------------------------------
# Configure target
#-------------------------------------------------------------------------------
# ${NS_DIR}/0-config.sh

#-------------------------------------------------------------------------------
# Build target & Deploy
#-------------------------------------------------------------------------------
TEST_CONFIG="${MSTP_DIR}/mstp-eval/inc/test_eval_config.h"
TEST_CONFIG_S="${MSTP_DIR}/mstp-victims/s/inc/test_eval_config.h"

# Enable TEST 
for config in "${TEST_CONFIG}" "${TEST_CONFIG_S}"; do
    rm -rf "${config}" || true
    cat > "${config}" << 'EOF'
#define TEST_MSTP_KEYPAD_POC_ENABLE

EOF
done

${NS_DIR}/1-compile.sh --test-mode
${NS_DIR}/2-deploy.sh

#-------------------------------------------------------------------------------
# Log results
#-------------------------------------------------------------------------------
rm ${RAW_TRACE} | true
${NS_DIR}/3-monitor.sh -o ${RAW_TRACE}

#-------------------------------------------------------------------------------
# Log results
#-------------------------------------------------------------------------------
PRUNED_TRACE_DIR=${LOG_DIR}/pruned_trace
rm -rf ${PRUNED_TRACE_DIR} | true
mkdir -p ${PRUNED_TRACE_DIR} | true
"${PYTHON_BIN}" ${ROOT}/3-extract-trace-matrix.py \
    -i "${RAW_TRACE}"           \
    -o "${PRUNED_TRACE_DIR}"

#-------------------------------------------------------------------------------
# Filter results
#-------------------------------------------------------------------------------
# Fist filter out the relevant lines (those with "TRACE")  
# Then from those lines, remove the "TRACE" prefix and keep only the content of 
# the trace 
# grep --no-group-separator -A 1 -E 'TRACE' ${RAW_TRACE} |\
# grep -v -E 'TRACE' > ${PATTERN}

#-------------------------------------------------------------------------------
# Create Matrix File:
# - First part is a static header defining the experiment;
# - Second part is the results generated previously, which are appended.
#-------------------------------------------------------------------------------
# Header
# cat > ${MATRIX_FILE} << 'EOF'
# Color_type: Integer #Integer or Probability
# Gradient: orange
# # Min_number_of_colors: 4
# Y: else if 
# # X: 

# EOF

# # Append results to matrix file
# cat ${PATTERN} >> ${MATRIX_FILE}

# echo "Matrix file created: ${MATRIX_FILE}"

#-------------------------------------------------------------------------------
# Draw graphs
#-------------------------------------------------------------------------------
# python3 ${GRAPH_SCRIPT_DIR}/gen_matrix.py \
#         -p "${MATRIX_FILE}"           \
#         -o "${OUTPUTS}/"

# xdg-open ${OUTPUTS}/${TEST_NAME}_matrix.png


#-------------------------------------------------------------------------------
# Build one matrix image per split trace generated in ${PRUNED_TRACE_DIR}.
# Each source file becomes its own labeled matrix and PNG.
#-------------------------------------------------------------------------------
trace_files=()
while IFS= read -r file; do
    trace_files+=("${file}")
done < <(find "${PRUNED_TRACE_DIR}" -maxdepth 1 -type f -name '0_trace_matrix_*.txt' | sort -V)

if [ ${#trace_files[@]} -eq 0 ]; then
    echo "Error: no trace files found in ${PRUNED_TRACE_DIR} (expected 0_trace_matrix_*.txt)" >&2
    exit 1
fi

for idx in "${!trace_files[@]}"; do
    trace_file="${trace_files[$idx]}"
    trace_name="$(basename "${trace_file}" .txt)"
    matrix_input="${LOG_DIR}/${trace_name}.txt"

    : > "${matrix_input}"

    i=0
    while IFS= read -r line || [ -n "${line}" ]; do
        if [[ ${i} -eq 0  ]]; then
            printf 'orange Loop %s\n' "${line}" >> "${matrix_input}"
            i=$((i + 1))
        else
            printf 'orange for-%s %s\n' "$((i-1))" "${line}" >> "${matrix_input}"
            i=$((i + 1))
        fi
    done < "${trace_file}"

    "${PYTHON_BIN}" ${GRAPH_SCRIPT_DIR}/gen_template_matrix.py    \
            -p "${matrix_input}"                       \
            -o "${OUTPUTS}/"
done

## I'm using this output instead 
# cp ${OUTPUTS}/${TEST_NAME}_matrix.png \
#    ${PAPER_CHAPTER_IMG_DIR} 