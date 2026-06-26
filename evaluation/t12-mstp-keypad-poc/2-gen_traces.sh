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

TEST_NAME="C04-toy_attack_nemesis"
MATRIX_FILE=${LOG_DIR}/${TEST_NAME}.txt
PATTERN=${LOG_DIR}/${TEST_NAME}_aux.txt

CHAPTER=06-mstp
PAPER_CHAPTER_IMG_DIR=${PAPER_CHAPTER_DIR}/${CHAPTER}/1-img/06-mstp_keypad_poc

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
# Draw graphs 
#-------------------------------------------------------------------------------
TEST_NAME_1=C04-key-trace-4
TEST_NAME_2=C04-key-trace-3
TEST_NAME_3=C04-key-trace-5
TEST_NAME_4=C04-key-trace-A

MATRIX_FILE=${LOG_DIR}/${TEST_NAME_1}.txt
python3 ${GRAPH_SCRIPT_DIR}/gen_template_matrix.py    \
        -p "${MATRIX_FILE}"                         \
        -o "${OUTPUTS}/"

MATRIX_FILE=${LOG_DIR}/${TEST_NAME_2}.txt
python3 ${GRAPH_SCRIPT_DIR}/gen_template_matrix.py    \
        -p "${MATRIX_FILE}"                         \
        -o "${OUTPUTS}/"

MATRIX_FILE=${LOG_DIR}/${TEST_NAME_3}.txt
python3 ${GRAPH_SCRIPT_DIR}/gen_template_matrix.py    \
        -p "${MATRIX_FILE}"                         \
        -o "${OUTPUTS}/"

MATRIX_FILE=${LOG_DIR}/${TEST_NAME_4}.txt
python3 ${GRAPH_SCRIPT_DIR}/gen_template_matrix.py    \
        -p "${MATRIX_FILE}"                         \
        -o "${OUTPUTS}/"

# I'm using this output instead 
cp  ${OUTPUTS}/${TEST_NAME_1}_matrix.png \
    ${OUTPUTS}/${TEST_NAME_2}_matrix.png \
    ${OUTPUTS}/${TEST_NAME_3}_matrix.png \
    ${OUTPUTS}/${TEST_NAME_4}_matrix.png \
    ${PAPER_CHAPTER_IMG_DIR} 