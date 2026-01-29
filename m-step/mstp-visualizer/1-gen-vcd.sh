#!/usr/bin/env bash
ROOT="$(realpath "$(dirname "$0")")"

PY_SCRIPTS=${ROOT}/scripts
INPUTS=${ROOT}/../traces/tfm-inv-mod-001
OUTPUTS=${ROOT}/outputs
TRACE=${INPUTS}/trace.txt  # Default trace file
ELF=${INPUTS}/tfm_s.elf

#-------------------------------------------------------------------------------
# Parse arguments
#-------------------------------------------------------------------------------
set -e

CLEAN=false

while [[ $# -gt 0 ]]; do
    case "$1" in
        -t|--trace)
            if [[ -z "$2" ]]; then
                echo "Error: --trace requires a file path"
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
                echo "Error: --output requires a directory path"
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
            echo "Usage: $0 [-t|--trace TRACE_FILE] [-elf ELF_FILE] [-o|--output OUTPUT_DIR] [-c|--clean]"
            echo ""
            echo "Options:"
            echo "  -t, --trace TRACE_FILE   Specify trace file (default: ${INPUTS}/trace.txt)"
            echo "  -elf ELF_FILE            Specify elf file (default: ${INPUTS}/tfm_s.elf)"
            echo "  -o, --output OUTPUT_DIR  Specify output directory (default: ${ROOT}/outputs)"
            echo "  -c, --clean              Clean output directory"
            echo "  -h, --help               Show this help message"
            echo ""
            echo "Examples:"
            echo "  $0                                    # Use default trace file"
            echo "  $0 -t /path/to/my_trace.txt         # Use custom trace file"
            echo "  $0 --trace ./custom_trace.txt       # Use custom trace file"
            echo "  $0 -o /path/to/output               # Use custom output directory"
            exit 0
            ;;
        *)
            echo "Unknown argument: $1"
            echo "Usage: $0 [-t|--trace TRACE_FILE] [-elf ELF_FILE] [-o|--output OUTPUT_DIR] [-c|--clean]"
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
    echo "Clean completed."
    exit 0
fi

#-------------------------------------------------------------------------------
# Generate VCD
#-------------------------------------------------------------------------------
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

echo "Using trace file: $TRACE"
echo "Using binary file: $ELF"
echo "Output directory: $OUTPUTS"
echo ""

python3 ${PY_SCRIPTS}/gen-vcd.py    \
        "${TRACE}"                   \
        "${ELF}"                     \
        --output-dir "${OUTPUTS}"    \
        --vcd-name trace.vcd