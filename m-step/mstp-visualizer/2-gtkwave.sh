#!/usr/bin/env bash
ROOT="$(realpath "$(dirname "$0")")"

OUTPUTS=${ROOT}/outputs
GTKW_CONFS=${ROOT}/gtkw_confs
INPUT_FILE="${OUTPUTS}/trace.vcd"

#-------------------------------------------------------------------------------
# Parse arguments
#-------------------------------------------------------------------------------
set -e

while [[ $# -gt 0 ]]; do
    case "$1" in
        -i|--input)
            INPUT_FILE="$2"
            shift 2
            ;;
        -h|--help)
            echo "Usage: $0 [-i|--input <file>]"
            exit 0
            ;;
        *)
            echo "Unknown argument: $1"
            echo "Usage: $0 [-i|--input <file>]"
            exit 1
            ;;
    esac
done

# Check if input file exists
if [[ ! -f "${INPUT_FILE}" ]]; then
    echo "Error: Input file '${INPUT_FILE}' does not exist"
    exit 1
fi

gtkwave ${INPUT_FILE} \
        ${GTKW_CONFS}/trace_visualization.gtkw \
        --cpu 16