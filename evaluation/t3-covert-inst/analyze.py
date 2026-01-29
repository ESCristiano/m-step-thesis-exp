#!/usr/bin/env python3
"""
Script to count occurrences of values in experiment traces.
Processes trace files with format: 0x<address> <value>
Groups data by experiments marked with "# marker_name" comments.
"""

import sys
import argparse
from collections import defaultdict, Counter
import re
import os

def parse_trace_file(filename):
    """
    Parse trace file and count value occurrences per experiment.
    
    Args:
        filename (str): Path to the trace file
        
    Returns:
        list: List of dictionaries containing counts for each experiment
    """
    experiments = []
    current_experiment = []
    experiment_num = 0
    current_marker_name = None
    
    try:
        with open(filename, 'r') as file:
            for line_num, line in enumerate(file, 1):
                line = line.strip()
                
                # Skip empty lines
                if not line:
                    continue
                
                # Check if this is a new experiment marker (format: # marker_name)
                if line.startswith('#'):
                    # If we have data from previous experiment, process it
                    if current_experiment:
                        # Filter out '02' values for victim_eval_inst_diff_udiv experiment
                        if current_marker_name == 'victim_eval_inst_diff_udiv':
                            current_experiment = [v for v in current_experiment if v != '02']
                        
                        experiments.append({
                            'experiment': experiment_num,
                            'marker_name': current_marker_name or f'Experiment {experiment_num}',
                            'counts': Counter(current_experiment),
                            'total_entries': len(current_experiment)
                        })
                    
                    # Start new experiment and capture marker name
                    experiment_num += 1
                    # Extract marker name (everything after "# ")
                    current_marker_name = line[1:].strip()
                    current_experiment = []
                    continue
                
                # Parse data lines (format: 0x<address> <value>)
                # Use regex to match hex address and value
                match = re.match(r'0x[0-9a-fA-F]+\s+([0-9a-fA-F]+)', line)
                if match:
                    value = match.group(1)
                    current_experiment.append(value)
                else:
                    print(f"Warning: Could not parse line {line_num}: {line}")
        
        # Don't forget the last experiment if file doesn't end with a marker
        if current_experiment:
            # Filter out '02' values for victim_eval_inst_diff_udiv experiment
            if current_marker_name == 'victim_eval_inst_diff_udiv':
                current_experiment = [v for v in current_experiment if v != '02']
            
            experiments.append({
                'experiment': experiment_num if experiment_num > 0 else 1,
                'marker_name': current_marker_name or f'Experiment {experiment_num if experiment_num > 0 else 1}',
                'counts': Counter(current_experiment),
                'total_entries': len(current_experiment)
            })
    
    except FileNotFoundError:
        print(f"Error: File '{filename}' not found.")
        return []
    except Exception as e:
        print(f"Error reading file: {e}")
        return []
    
    return experiments

def display_results(experiments, output_file=None):
    """Display the counting results in a formatted way."""
    if not experiments:
        message = "No experiments found or processed."
        if output_file:
            with open(output_file, 'w') as f:
                f.write(message + "\n")
        else:
            print(message)
        return
    
    lines = []
    lines.append("=" * 50)
    lines.append("EXPERIMENT VALUE COUNTING RESULTS")
    lines.append("=" * 50)
    
    for exp in experiments:
        lines.append(f"\nExperiment {exp['experiment']}: {exp['marker_name']}")
        
        # Filter out outliers (values with count < 5)
        filtered_counts = {value: count for value, count in exp['counts'].items() if count >= 5}
        filtered_total = sum(filtered_counts.values())
        
        if not filtered_counts:
            lines.append(f"Total entries: {exp['total_entries']}")
            lines.append("No values with >= 5 occurrences (all outliers)")
            continue
        
        lines.append(f"Total entries: {filtered_total}")
        lines.append("Value counts:")
        
        # Sort by value for consistent output
        for value in sorted(filtered_counts.keys()):
            count = filtered_counts[value]
            percentage = count / filtered_total
            lines.append(f"  Value '{value}': {count} times ({percentage:.2f})")
    
    output = "\n".join(lines)
    
    if output_file:
        with open(output_file, 'w') as f:
            f.write(output + "\n")
        print(f"Results written to: {output_file}")
    else:
        print(output)
    
def main():
    """Main function to run the script."""
    parser = argparse.ArgumentParser(
        description='Count occurrences of values in experiment traces',
        formatter_class=argparse.RawDescriptionHelpFormatter
    )
    parser.add_argument('-i', '--input-file', required=True, 
                        help='Input trace file path')
    parser.add_argument('-o', '--output-file', default=None,
                        help='Output file path (default: print to stdout)')
    
    args = parser.parse_args()
    
    # Set default output file in current directory if not specified
    output_file = args.output_file
    if output_file is None:
        # Generate default output filename based on input
        base_name = os.path.splitext(os.path.basename(args.input_file))[0]
        output_file = f"{base_name}_results.txt"
    
    print(f"Processing trace file: {args.input_file}")
    experiments = parse_trace_file(args.input_file)
    
    if experiments:
        display_results(experiments, output_file)
    else:
        error_msg = "No valid data found in the file."
        if output_file:
            with open(output_file, 'w') as f:
                f.write(error_msg + "\n")
        print(error_msg)

if __name__ == "__main__":
    main()