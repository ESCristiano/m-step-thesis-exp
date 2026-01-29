import argparse
import re


def parse_input_file(input_path):
    """Parse the input file and extract the data matrix."""
    with open(input_path, 'r') as f:
        lines = f.readlines()
    
    # Find header line (contains CL)
    header_idx = None
    for i, line in enumerate(lines):
        if 'CL' in line:
            header_idx = i
            break
    
    if header_idx is None:
        raise ValueError("No header line found in input file")
    
    # Extract column headers
    header_line = lines[header_idx]
    columns = [col.strip() for col in header_line.split('|') if col.strip() and 'CL' in col]
    num_cols = len(columns)
    
    # Extract data rows
    data_rows = []
    for line in lines[header_idx + 1:]:
        # Skip empty lines
        if not line.strip():
            continue
        
        # Split by '|' and extract cell values
        cells = [cell.strip() for cell in line.split('|') if cell.strip()]
        
        # Only process lines that have the expected number of cells
        if len(cells) == num_cols:
            data_rows.append(cells)
    
    return data_rows, num_cols


def convert_to_matrix(data_rows, num_cols):
    """Convert data rows to matrix format and reverse order."""
    matrix = []
    
    # Process each row
    for row in data_rows:
        matrix_row = []
        for i in range(num_cols):
            if i < len(row):
                if 'XX' in row[i] or 'X' in row[i]:
                    matrix_row.append('1.00')
                else:
                    matrix_row.append('0.00')
            else:
                matrix_row.append('0.00')
        matrix.append(matrix_row)
    
    # Reverse the order of rows
    matrix.reverse()
    
    return matrix


def write_output_file(output_path, matrix):
    """Write the matrix to output file."""
    with open(output_path, 'w') as f:
        for row in matrix:
            f.write(' '.join(row) + ' \n')


def main():
    parser = argparse.ArgumentParser(description='Convert input table to matrix format')
    parser.add_argument('-i', '--input', required=True, help='Input file path')
    parser.add_argument('-o', '--output', required=True, help='Output file path')
    
    args = parser.parse_args()
    
    # Parse input file
    data_rows, num_cols = parse_input_file(args.input)
    
    # Convert to matrix
    matrix = convert_to_matrix(data_rows, num_cols)
    
    # Write output
    write_output_file(args.output, matrix)
    
    print(f"Matrix successfully written to {args.output}")


if __name__ == '__main__':
    main()