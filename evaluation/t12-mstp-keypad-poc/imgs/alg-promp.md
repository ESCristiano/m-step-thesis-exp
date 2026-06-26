I will give you a trace where there is a long stream of numbers like this (...01 02 02 01 01 01 01 01 01 01 01 01 01 01 02 02 01 01 01 01 01 01 01 01 01 01...)

I have three patterns that I want to find there. 

for-press      02 02 01 01 01 02 02 01 01 02 01 01 02 02 01 01 02 01 02 02 01 02 02 01 02 02 01
for-no-press   02 01 01 02 02 01 01 01 02 02 01 02 01 02 02 01 01 02 01 02 02 01 02 02 01 02 02 01
while          01 01 01 02 01 02 01 01 02 01 01 02 01 01 01 01 02 01 02 01 02 01 02 02 01
 
1 - First start by finding the "while" pattern to split the trace in small traces (trace-1.png) should be translated to (trace-2.png). The number of small traces depends on the number of matches on the "while" pattern

2 - Then search for all "for-no-press" on each small trace and make it arranged in one pattern per line (trace-3.png -> trace-4.png). The number of "for-no-press" patterns may vary, but typically is 15. 

3 - Next search for all "for-press" on each small trace and make it the pattern to be one line (trace-5.png -> trace-6.png). There should be only one "for-press" per small trace.

4 - Prune the remaining data samples that do don't belong to anything I mentioned above, and aline the traces per the size of the pattern per line (trace-7.png -> trace-8.png )

5 - Align the matrices, filling it with zeros (final example: trace-9.png)

Note: The traces images have the: for-press, for-no-press, and while patterns, but just for reference. The actual trace won't have it. It would like 0_raw_trace.txt. 

To process the 0_raw_trace.txt you should strip all extra prints and only get the trace.

Input (0_raw_trace.txt): 

============================================================
Test run at 2026-06-23 07:15:34
============================================================
Hello NS World!!
####################################################################
####################################################################
TRACE WILL BE HERE (....02 02 01 01 01 02 02 01 01 0 .....)
End NS World!!

What matters (0_raw_trace.txt): 

TRACE WILL BE HERE (....02 02 01 01 01 02 02 01 01 0 .....)

TODO: 
- Write a python script/program (to experiments/m-step/evaluation/t12-mstp-keypad-poc) to extract the traces to create the final trace like (trace-9.png)