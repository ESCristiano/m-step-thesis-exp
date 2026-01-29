import argparse
import re
from collections import defaultdict


def parse_trace_file(filepath):
    """Parse the trace file and extract experiment data."""
    experiments = []
    current_experiment = None
    
    with open(filepath, 'r') as f:
        for line in f:
            line = line.strip()
            
            # Match experiment header
            exp_match = re.match(r'Experiment (\d+):', line)
            if exp_match:
                if current_experiment:
                    experiments.append(current_experiment)
                current_experiment = {
                    'number': int(exp_match.group(1)),
                    'values': {}
                }
                continue
            
            # Match value counts
            value_match = re.match(r"Value '(\w+)':\s+\d+\s+times\s+\(([\d.]+)\)", line)
            if value_match and current_experiment:
                value = value_match.group(1)
                frequency = float(value_match.group(2))
                current_experiment['values'][value] = frequency
        
        # Add last experiment
        if current_experiment:
            experiments.append(current_experiment)
    
    return experiments


def create_frequency_matrix(experiments):
    """Create a frequency matrix from experiments."""
    # Find min and max values across all experiments
    all_values = set()
    for exp in experiments:
        all_values.update(exp['values'].keys())
    
    if not all_values:
        return [], []
    
    # Sort values to get the range
    sorted_values = sorted(all_values)
    min_val = int(sorted_values[0], 16)
    max_val = int(sorted_values[-1], 16)
    
    # Create complete range
    value_range = [f'{i:02x}' for i in range(min_val, max_val + 1)]
    
    # Build matrix
    matrix = []
    for value in value_range:
        row = []
        for exp in experiments:
            frequency = exp['values'].get(value, 0.0)
            row.append(frequency)
        matrix.append(row)
    
    return value_range, matrix


def write_matrix_output(filepath, value_range, matrix, num_experiments):
    """Write the matrix to output file."""
    with open(filepath, 'w') as f:
        # Write header
        y_axis = ' '.join(str(i+1) for i in range(len(value_range)))
        x_axis = ' '.join(str(i+1) for i in range(num_experiments))
        
        # f.write(f'Y: {y_axis}\n')
        # f.write(f'X: {x_axis}\n\n')
        
        # Write matrix in reverse order
        for row in reversed(matrix):
            row_str = ' '.join(f'{val:.2f}' for val in row)
            f.write(f'{row_str}\n')


def main():
    parser = argparse.ArgumentParser(description='Generate frequency matrix from trace file')
    parser.add_argument('-i', '--input', required=True, help='Input trace file path')
    parser.add_argument('-o', '--output', required=True, help='Output matrix file path')
    
    args = parser.parse_args()
    
    # Parse trace file
    experiments = parse_trace_file(args.input)
    
    if not experiments:
        print("No experiments found in trace file")
        return
    
    # Create frequency matrix
    value_range, matrix = create_frequency_matrix(experiments)
    
    # Write output
    write_matrix_output(args.output, value_range, matrix, len(experiments))
    
    print(f"Matrix generated successfully: {args.output}")
    print(f"Experiments: {len(experiments)}")
    print(f"Value range: {value_range[0]} to {value_range[-1]}")


if __name__ == '__main__':
    main()
