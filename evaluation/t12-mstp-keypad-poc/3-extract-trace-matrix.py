#!/usr/bin/env python3
"""Extract and align keypad trace patterns from a raw NS-world log.

This script reads the raw monitor output, keeps only the numeric trace payload,
segments it by the `while` anchor pattern, extracts `for-no-press` and
`for-press` matches per segment, prunes unmatched samples, and writes a plain
numeric matrix where all rows are right-padded with zeros.
"""

from __future__ import annotations

import argparse
import re
from dataclasses import dataclass
from pathlib import Path


WHILE_PATTERN = [
    1,
    1,
    1,
    2,
    1,
    2,
    1,
    1,
    2,
    1,
    1,
    2,
    1,
    1,
    1,
    1,
    2,
    1,
    2,
    1,
    2,
    1,
    2,
    2,
    1,
]

FOR_NO_PRESS_PATTERN = [
    2,
    1,
    1,
    2,
    2,
    1,
    1,
    1,
    2,
    2,
    1,
    2,
    1,
    2,
    2,
    1,
    1,
    2,
    1,
    2,
    2,
    1,
    2,
    2,
    1,
    2,
    2,
    1,
]

FOR_PRESS_PATTERN = [
    2,
    2,
    1,
    1,
    1,
    2,
    2,
    1,
    1,
    2,
    1,
    1,
    2,
    2,
    1,
    1,
    2,
    1,
    2,
    2,
    1,
    2,
    2,
    1,
    2,
    2,
    1,
]

START_MARKER = "Hello NS World!!"
END_MARKER = "End NS World!!"
OUTPUT_BASENAME = "0_trace_matrix"


class TraceExtractionError(RuntimeError):
    """Raised when strict extraction invariants are violated."""


@dataclass
class SegmentResult:
    """Holds extracted rows and counters for one while-bounded segment."""

    index: int
    source_start: int
    source_end: int
    rows: list[list[int]]
    for_no_press_matches: int
    for_press_matches: int


def default_input_path() -> Path:
    return Path(__file__).resolve().parent / "logs" / "0_raw_trace.txt"


def default_output_dir() -> Path:
    return Path(__file__).resolve().parent / "logs"


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Extract while/for-* pattern matrix from raw keypad traces."
    )
    parser.add_argument(
        "-i",
        "--input",
        type=Path,
        default=default_input_path(),
        help="Path to the raw trace file (default: logs/0_raw_trace.txt)",
    )
    parser.add_argument(
        "-o",
        "--output-dir",
        "--output",
        dest="output_dir",
        type=Path,
        default=default_output_dir(),
        help="Directory where matrix files are written (default: logs)",
    )
    parser.add_argument(
        "-v",
        "--verbose",
        action="store_true",
        help="Print detailed per-segment extraction information.",
    )
    parser.add_argument(
        "--non-strict-press",
        action="store_true",
        help="Do not fail when a segment does not contain exactly one for-press match.",
    )
    return parser.parse_args()


def extract_trace_region(raw_text: str) -> str:
    """Keep only the textual region expected to contain numeric trace samples."""
    trace_text = raw_text

    start_idx = trace_text.find(START_MARKER)
    if start_idx != -1:
        trace_text = trace_text[start_idx + len(START_MARKER) :]

    end_idx = trace_text.find(END_MARKER)
    if end_idx != -1:
        trace_text = trace_text[:end_idx]

    return trace_text


def tokenize_samples(trace_region: str) -> list[int]:
    """Extract signed integer tokens from trace text in-order."""
    return [int(match.group(0)) for match in re.finditer(r"-?\d+", trace_region)]


def find_non_overlapping_matches(samples: list[int], pattern: list[int]) -> list[int]:
    """Find all non-overlapping exact matches of pattern within samples."""
    if not samples or not pattern or len(samples) < len(pattern):
        return []

    matches: list[int] = []
    idx = 0
    pattern_len = len(pattern)
    max_start = len(samples) - pattern_len

    while idx <= max_start:
        if samples[idx : idx + pattern_len] == pattern:
            matches.append(idx)
            idx += pattern_len
            continue
        idx += 1

    return matches


def split_segments_by_while(samples: list[int]) -> list[tuple[int, int, list[int]]]:
    """Split global trace into while-bounded segments."""
    while_matches = find_non_overlapping_matches(samples, WHILE_PATTERN)
    if not while_matches:
        raise TraceExtractionError("No while pattern match was found in the trace.")

    segments: list[tuple[int, int, list[int]]] = []
    for index, start in enumerate(while_matches):
        end = while_matches[index + 1] if index + 1 < len(while_matches) else len(samples)
        segment_samples = samples[start:end]
        segments.append((start, end, segment_samples))

    return segments


def format_sample(value: int) -> str:
    """Render samples as two-digit values when non-negative."""
    if value < 0:
        return str(value)
    return f"{value:02d}"


def extract_rows_from_segment(
    segment_index: int,
    source_start: int,
    source_end: int,
    segment: list[int],
    strict_press: bool,
) -> SegmentResult:
    """Extract while + for-no-press + for-press rows from one segment."""
    while_len = len(WHILE_PATTERN)

    if len(segment) < while_len or segment[:while_len] != WHILE_PATTERN:
        raise TraceExtractionError(
            f"Segment {segment_index} [{source_start}:{source_end}] does not start with while pattern."
        )

    rows: list[list[int]] = [WHILE_PATTERN.copy()]
    for_no_press_matches = 0
    for_press_matches = 0

    idx = while_len
    while idx < len(segment):
        if segment[idx : idx + len(FOR_PRESS_PATTERN)] == FOR_PRESS_PATTERN:
            rows.append(FOR_PRESS_PATTERN.copy())
            for_press_matches += 1
            idx += len(FOR_PRESS_PATTERN)
            continue

        if segment[idx : idx + len(FOR_NO_PRESS_PATTERN)] == FOR_NO_PRESS_PATTERN:
            rows.append(FOR_NO_PRESS_PATTERN.copy())
            for_no_press_matches += 1
            idx += len(FOR_NO_PRESS_PATTERN)
            continue

        idx += 1

    if strict_press and for_press_matches != 1:
        preview = " ".join(format_sample(v) for v in segment[:96])
        raise TraceExtractionError(
            " ".join(
                [
                    f"Segment {segment_index} [{source_start}:{source_end}] has",
                    f"{for_press_matches} for-press matches; expected exactly 1.",
                    f"Segment prefix: {preview}",
                ]
            )
        )

    return SegmentResult(
        index=segment_index,
        source_start=source_start,
        source_end=source_end,
        rows=rows,
        for_no_press_matches=for_no_press_matches,
        for_press_matches=for_press_matches,
    )


def right_pad_rows(segment_results: list[SegmentResult]) -> tuple[int, list[list[list[int]]]]:
    """Right-pad all rows to equal width with zeros."""
    all_rows = [row for segment in segment_results for row in segment.rows]
    if not all_rows:
        return 0, []

    max_width = max(len(row) for row in all_rows)
    padded_segments: list[list[list[int]]] = []

    for segment in segment_results:
        padded_rows: list[list[int]] = []
        for row in segment.rows:
            padded_rows.append(row + [0] * (max_width - len(row)))
        padded_segments.append(padded_rows)

    return max_width, padded_segments


def write_matrix(output_path: Path, padded_segments: list[list[list[int]]]) -> None:
    """Write plain numeric matrix rows separated by blank lines per segment."""
    output_path.parent.mkdir(parents=True, exist_ok=True)

    with output_path.open("w", encoding="utf-8") as handle:
        for seg_idx, rows in enumerate(padded_segments):
            for row in rows:
                handle.write(" ".join(format_sample(value) for value in row))
                handle.write("\n")

            if seg_idx + 1 < len(padded_segments):
                handle.write("\n")


def write_segment_files(output_path: Path, padded_segments: list[list[list[int]]]) -> list[Path]:
    """Write one matrix file per segment as <stem>_<index><suffix>."""
    output_path.parent.mkdir(parents=True, exist_ok=True)

    generated_files: list[Path] = []
    stem = output_path.stem
    suffix = output_path.suffix or ".txt"

    for seg_idx, rows in enumerate(padded_segments, start=1):
        segment_path = output_path.with_name(f"{stem}_{seg_idx}{suffix}")
        with segment_path.open("w", encoding="utf-8") as handle:
            for row in rows:
                handle.write(" ".join(format_sample(value) for value in row))
                handle.write("\n")
        generated_files.append(segment_path)

    return generated_files


def resolve_output_paths(output_dir: Path) -> tuple[Path, Path]:
    """Return combined output file path and base stem path inside output_dir."""
    output_dir.mkdir(parents=True, exist_ok=True)
    combined_output = output_dir / f"{OUTPUT_BASENAME}.txt"
    stem_output = output_dir / f"{OUTPUT_BASENAME}.txt"
    return combined_output, stem_output


def main() -> int:
    args = parse_args()

    if not args.input.exists():
        raise TraceExtractionError(f"Input file not found: {args.input}")

    raw_text = args.input.read_text(encoding="utf-8")
    trace_region = extract_trace_region(raw_text)
    samples = tokenize_samples(trace_region)

    if not samples:
        raise TraceExtractionError("No numeric samples were found in the selected trace region.")

    segments = split_segments_by_while(samples)
    strict_press = not args.non_strict_press

    results: list[SegmentResult] = []
    for segment_index, (start, end, segment_samples) in enumerate(segments):
        result = extract_rows_from_segment(
            segment_index=segment_index,
            source_start=start,
            source_end=end,
            segment=segment_samples,
            strict_press=strict_press,
        )
        results.append(result)

    max_width, padded_segments = right_pad_rows(results)

    if args.output_dir.exists() and not args.output_dir.is_dir():
        raise TraceExtractionError(f"Output path is not a directory: {args.output_dir}")

    combined_output, stem_output = resolve_output_paths(args.output_dir)
    # write_matrix(combined_output, padded_segments)
    generated_segment_files = write_segment_files(stem_output, padded_segments)

    total_rows = sum(len(segment.rows) for segment in results)
    print(f"Input samples: {len(samples)}")
    print(f"While segments: {len(results)}")
    print(f"Output rows: {total_rows}")
    print(f"Aligned row width: {max_width}")
    print(f"Output directory: {args.output_dir}")
    # print(f"Output file: {combined_output}")
    for path in generated_segment_files:
        print(f"Segment output file: {path}")

    if args.verbose:
        for segment in results:
            print(
                " ".join(
                    [
                        f"segment={segment.index}",
                        f"range=[{segment.source_start}:{segment.source_end}]",
                        f"for-no-press={segment.for_no_press_matches}",
                        f"for-press={segment.for_press_matches}",
                        f"rows={len(segment.rows)}",
                    ]
                )
            )

    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except TraceExtractionError as error:
        print(f"Error: {error}")
        raise SystemExit(1)