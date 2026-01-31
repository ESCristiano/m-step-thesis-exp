#!/usr/bin/env bash
ROOT="$(realpath "$(dirname "$0")")"

OUTPUTS=${ROOT}/outputs
GTKW_CONFS=${ROOT}/gtkw_confs

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

# Check required arguments
if [[ -z "$INPUT_FILE" ]]; then
    echo "Error: -i|--input is required"
    echo "Usage: $0 [-i|--input <file>]"
    exit 1
fi

#-------------------------------------------------------------------------------
# Show VCD Trace
#-------------------------------------------------------------------------------
INPUT_FILE="$(realpath "${INPUT_FILE}")"

# Check if input file exists
if [[ ! -f "${INPUT_FILE}" ]]; then
    echo "Error: Input file '${INPUT_FILE}' does not exist"
    exit 1
fi

# Generate GTKWave configuration with user absolute paths
sed "s|{INSERT_ROOT}|$ROOT|g" ${GTKW_CONFS}/gtkconf_template.gtkw > ${GTKW_CONFS}/local_session.gtkw

gtkwave ${INPUT_FILE} \
        ${GTKW_CONFS}/local_session.gtkw \
        --cpu 16