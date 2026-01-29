# Evaluation Graphs

Scripts and data for generating the figures presented in the paper.

## Overview

This directory contains:

- **Pre-computed pattern data**: Results from experiments ready for visualization
- **Python plotting scripts**: Generate publication-quality figures
- **Shell automation**: Batch generation of all figures

## Directory Structure

```
graphs/
├── 1-draw-graphs.sh        # Generate all figures
├── README.md               # This file
├── patterns/               # Pre-computed experiment data
│   ├── 06-div.txt          # Division timing data (Fig. 6a)
│   ├── 06-inst_diff.txt    # Instruction timing data (Fig. 6b)
│   ├── 06-cache.txt        # Cache timing data (Fig. 6c)
│   ├── 06-contention.txt   # Contention timing data (Fig. 6d)
│   ├── 06-shift_template.txt   # Shift template data (Fig. 7a, 10)
│   └── 06-sub_template.txt     # Sub template data (Fig. 7b)
└── scripts/                # Python plotting scripts
    ├── gen_matrix.py       # Heatmap matrix generator
    └── gen_template_matrix.py  # Template attack matrix generator
```

## Quick Start

Generate all paper figures:

```bash
./1-draw-graphs.sh
```

Or with a custom output directory:

```bash
./1-draw-graphs.sh -o /path/to/output
```

## Paper Figure Mapping

| Figure | Data File | Script | Description |
|--------|-----------|--------|-------------|
| **Fig. 6a** | `patterns/06-div.txt` | `gen_matrix.py` | UDIV timing differences |
| **Fig. 6b** | `patterns/06-inst_diff.txt` | `gen_matrix.py` | Instruction timing differences |
| **Fig. 6c** | `patterns/06-cache.txt` | `gen_matrix.py` | Cache timing differences |
| **Fig. 6d** | `patterns/06-contention.txt` | `gen_matrix.py` | Bus contention timing |
| **Fig. 7a** | `patterns/06-shift_template.txt` | `gen_template_matrix.py` | Shift template attack |
| **Fig. 7b** | `patterns/06-sub_template.txt` | `gen_template_matrix.py` | Sub template attack |
| **Fig. 10** | `patterns/06-shift_template.txt` | `gen_template_matrix.py` | Pruned template matrix |

## Pattern File Format

Pattern files use a simple text format:

```
Color_type: Integer    # or "Probability"
Gradient: orange       # Color gradient name
# Optional comments
# X and Y axis labels can be specified
1 2 3 4 5 6 7 8       # Data row 1
2 3 4 5 6 7 8 9       # Data row 2
...
```

## Individual Script Usage

### `gen_matrix.py` - Heatmap Generator

Generates heatmap visualizations for timing data.

```bash
python3 scripts/gen_matrix.py -p patterns/06-div.txt -o ./output
```

**Options:**
- `-p, --pattern`: Input pattern file
- `-o, --output`: Output directory

### `gen_template_matrix.py` - Template Attack Matrix

Generates template attack correlation matrices.

```bash
# Full matrix
python3 scripts/gen_template_matrix.py -p patterns/06-shift_template.txt -o ./output

# Pruned matrix (for Fig. 10)
python3 scripts/gen_template_matrix.py \
    -p patterns/06-shift_template.txt \
    -o ./output \
    --pruned \
    --pruned-start 10 \
    --pruned-final 138
```

**Options:**
- `-p, --pattern`: Input pattern file
- `-o, --output`: Output directory
- `--pruned`: Generate pruned version
- `--pruned-start`: Start index for pruning
- `--pruned-final`: End index for pruning

## Output Files

Generated figures are saved as PNG files:

```
output/
├── 06-div.png
├── 06-inst_diff.png
├── 06-cache.png
├── 06-contention.png
├── 06-shift_template.png
├── 06-shift_template_pruned.png
└── 06-sub_template.png
```

## Regenerating Data

To regenerate the pattern data from scratch (requires hardware):

```bash
# Run the corresponding experiment
cd ../t2-covert-udiv
./1-run-test.sh

# The pattern file will be generated in logs/
cat logs/06-div.txt
```

## Customizing Figures

### Changing Colors

Edit the `Gradient` line in pattern files:

```
Gradient: orange    # Options: orange, blue, green, etc.
```

### Adjusting Labels

Add X/Y axis labels in pattern files:

```
#Y: 4b 8b 12b 16b 20b 24b 28b 32b
#X: 4b 8b 12b 16b 20b 24b 28b 32b
```

## Dependencies

Required Python packages (provided by Nix environment):

- `matplotlib`
- `numpy`

Ensure you're in the Nix development shell:

```bash
nix develop
```
