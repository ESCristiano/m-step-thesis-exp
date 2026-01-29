import argparse
import re


def parse_input_file(filepath):
    """Parse input file and extract It and Con values."""
    experiments = []
    
    with open(filepath, 'r') as f:
        content = f.read()
    
    # Find all victim_eval_cont blocks
    pattern = r'# victim_eval_cont\s+It:\s*(\d+)\s*\|\s*Con:\s*(\d+)\s+# victim_eval_cont end'
    matches = re.findall(pattern, content)
    
    for it, con in matches:
        experiments.append({'It': int(it), 'Con': int(con)})
    
    return experiments


def calculate_probabilities(experiments):
    """Calculate probability for each experiment."""
    probabilities = []
    n_experiments = len(experiments)
    
    for i, exp in enumerate(experiments):
        # Last experiment uses N = It, others use N = 1000
        N = exp['It'] if i == n_experiments - 1 else 1000
        con = exp['Con']
        
        # If Con > N, probability = 1.0
        prob = min(con / N, 1.0) if N > 0 else 1.0
        probabilities.append(prob)
    
    return probabilities


def create_anti_diagonal_matrix(probabilities):
    """Create matrix with probabilities on anti-diagonal."""
    n = len(probabilities)
    matrix = [[0.0 for _ in range(n)] for _ in range(n)]
    
    # Reverse probabilities so exp4, exp3, exp2, exp1 order
    reversed_probs = probabilities[::-1]
    
    # Place probabilities on anti-diagonal
    # Anti-diagonal: matrix[i][n-1-i]
    for i in range(n):
        matrix[i][n - 1 - i] = reversed_probs[i]
    
    return matrix


def save_matrix(matrix, filepath):
    """Save matrix to file."""
    with open(filepath, 'w') as f:
        for row in matrix:
            # Format each value to 2 decimal places
            formatted_row = ' '.join(f'{val:.2f}' for val in row)
            f.write(formatted_row + '\n')


def main():
    parser = argparse.ArgumentParser(description='Generate probability matrix from victim evaluation results.')
    parser.add_argument('-i', '--input', required=True, help='Input file path')
    parser.add_argument('-o', '--output', required=True, help='Output file path')
    
    args = parser.parse_args()
    
    # Parse input file
    experiments = parse_input_file(args.input)
    
    # Calculate probabilities
    probabilities = calculate_probabilities(experiments)
    
    # Create anti-diagonal matrix
    matrix = create_anti_diagonal_matrix(probabilities)
    
    # Save matrix
    save_matrix(matrix, args.output)
    
    print(f"Matrix generated successfully. Saved to {args.output}")


if __name__ == '__main__':
    main()
