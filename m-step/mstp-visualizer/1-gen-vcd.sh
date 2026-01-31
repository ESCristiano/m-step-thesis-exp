#!/usr/bin/env bash
ROOT="$(realpath "$(dirname "$0")")"

GEN_VCD=${ROOT}/scripts/gen-vcd.py
OUTPUTS=${ROOT}/outputs
GTKW_CONFS=${ROOT}/gtkw_confs

#-------------------------------------------------------------------------------
# Parse arguments
#-------------------------------------------------------------------------------
set -e

CLEAN=false

while [[ $# -gt 0 ]]; do
    case "$1" in
        -t|--trace)
            if [[ -z "$2" ]]; then
                echo "Error: -t|--trace requires a file path"
                exit 1
            fi
            TRACE="$2"
            shift 2
            ;;
        -elf)
            if [[ -z "$2" ]]; then
                echo "Error: -elf requires a elf path"
                exit 1
            fi
            ELF="$2"
            shift 2
            ;;
        -o|--output)
            if [[ -z "$2" ]]; then
                echo "Error: -o|--output requires a directory path"
                exit 1
            fi
            OUTPUTS="$2"
            shift 2
            ;;
        -c|--clean)
            CLEAN=true
            shift
            ;;
        -h|--help)
            echo "Usage: $0 [-t|--trace <file>] [-elf <file>] [-o|--output <dir>] [-c|--clean]"
            echo ""
            echo "Options:"
            echo "  -t, --trace TRACE_FILE   Specify trace file (required)"
            echo "  -elf ELF_FILE            Specify elf file (required)"
            echo "  -o, --output OUTPUT_DIR  Specify output directory (required)"
            echo "  -c, --clean              Clean output directory"
            echo "  -h, --help               Show this help message"
            exit 0
            ;;
        *)
            echo "Unknown argument: $1"
            echo "Usage: $0 [-t|--trace <file>] [-elf <file>] [-o|--output <dir>] [-c|--clean]"
            echo "Use -h or --help for more information"
            exit 1
            ;;
    esac
done

#-------------------------------------------------------------------------------
# Clean up if requested
#-------------------------------------------------------------------------------
if [ "$CLEAN" = true ]; then
    echo "Cleaning Experiment..."
    rm -r ${OUTPUTS}/* || true 
    rm -r ${GTKW_CONFS}/*.tbl || true 
    rm -r ${GTKW_CONFS}/local_session.gtkw || true 
    echo "Clean completed."
    exit 0
fi

#-------------------------------------------------------------------------------
# Generate VCD
#-------------------------------------------------------------------------------
TRACE="$(realpath "${TRACE}")"
ELF="$(realpath "${ELF}")"
OUTPUTS="$(realpath "${OUTPUTS}")"

# Validate trace file exists
if [[ ! -f "$TRACE" ]]; then
    echo "Error: Trace file '$TRACE' not found"
    exit 1
fi

# Validate ELF file exists
if [[ ! -f "$ELF" ]]; then
    echo "Error: ELF file '$ELF' not found"
    exit 1
fi

echo "##########################################################################"
echo "Using trace file: $TRACE"
echo "Using binary file: $ELF"
echo "Output directory: $OUTPUTS"
echo "##########################################################################"
echo ""

python3 ${GEN_VCD}                  \
        "${TRACE}"                  \
        "${ELF}"                    \
        --output-dir "${OUTPUTS}"   \
        --tables-dir "${GTKW_CONFS}"\
        --vcd-name trace.vcd