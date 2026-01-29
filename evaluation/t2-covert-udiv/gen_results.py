import argparse
import re
from collections import defaultdict, Counter

def parse_input_file(input_path):
    """Parse the input file and extract experiment data."""
    experiments = {}
    current_label = None
    latencies = []
    
    with open(input_path, 'r') as f:
        for line in f:
            line = line.strip()
            
            # Check if line is an experiment label (0xYYYYYYYY/0xXXXXXXXX)
            if re.match(r'^0x[0-9A-Fa-f]{8}/0x[0-9A-Fa-f]{8}$', line):
                # Save previous experiment if it had latencies
                if current_label and latencies:
                    # Use mode (most common value) to handle outliers
                    experiments[current_label] = Counter(latencies).most_common(1)[0][0]
                
                # Start new experiment
                current_label = line
                latencies = []
            
            # Check if line contains IRI Lat value
            elif line.startswith('IRI Lat:'):
                match = re.search(r'IRI Lat:\s*(\d+)', line)
                if match:
                    latencies.append(int(match.group(1)))
        
        # Save last experiment if it had latencies
        if current_label and latencies:
            # Use mode (most common value) to handle outliers
            experiments[current_label] = Counter(latencies).most_common(1)[0][0]
    
    return experiments

def organize_matrix(experiments):
    """Organize experiments into a matrix structure."""
    # Extract unique Y and X values
    y_values = set()
    x_values = set()
    
    for label in experiments.keys():
        y_hex, x_hex = label.split('/')
        y_values.add(y_hex.upper())
        x_values.add(x_hex.upper())
    
    # Sort values by their numeric value
    y_sorted = sorted(y_values, key=lambda x: int(x, 16))
    x_sorted = sorted(x_values, key=lambda x: int(x, 16))
    
    # Create matrix
    matrix = defaultdict(dict)
    for label, avg_lat in experiments.items():
        y_hex, x_hex = label.split('/')
        y_hex = y_hex.upper()
        x_hex = x_hex.upper()
        matrix[y_hex][x_hex] = avg_lat
    
    return matrix, y_sorted, x_sorted

def write_output(output_path, matrix, y_sorted, x_sorted):
    """Write the matrix to the output file."""
    with open(output_path, 'w') as f:
        # Write Y axis header
        f.write('Y: ' + ' '.join(y_sorted) + '\n')
        
        # Write X axis header
        f.write('X: ' + ' '.join(x_sorted) + '\n')
        
        # Write empty line
        f.write('\n')
        
        # Write matrix rows (from highest Y to lowest Y)
        for y in reversed(y_sorted):
            row_values = []
            for x in x_sorted:
                if x in matrix[y]:
                    # Format as 2-digit integer
                    row_values.append(f"{int(round(matrix[y][x])):02d}")
                else:
                    row_values.append('--')
            f.write(' '.join(row_values) + '\n')

def main():
    parser = argparse.ArgumentParser(description='Process IRI Latency data and generate matrix output.')
    parser.add_argument('-i', '--input', required=True, help='Input file path')
    parser.add_argument('-o', '--output', required=True, help='Output file path')
    
    args = parser.parse_args()
    
    # Parse input file
    experiments = parse_input_file(args.input)
    
    # Organize into matrix
    matrix, y_sorted, x_sorted = organize_matrix(experiments)
    
    # Write output
    write_output(args.output, matrix, y_sorted, x_sorted)
    
    print(f"Processing complete. Matrix written to {args.output}")

if __name__ == '__main__':
    main()
