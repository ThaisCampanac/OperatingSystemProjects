This document will explain how to compile and execute the program.

To compile the code then type

gcc -pthread project.c -o project

into your terminal and then type

./project x x 

to execute the program. The "x" should be replaced with a number with the first x relating to
the number of doctor threads and the second relating to the number of patient threads.

If you input more than 3 doctors then the code will not execute and if you input more than 30
patients then the code will not execute.

