AN EFFICIENT SCHEDULABILITY ANALYSIS OF SEMI-CLAIRVOYANT SPORADIC TASK SYSTEMS WITH GRACEFUL DEGRADATION

# How to Compile:
=================
	
1. Go to the path where the .c and .h files are stored.
2. (Optional) Type "make clean" in the terminal to get rid of all the object, executable and output files.
3. Either type "make" or "make all" in the terminal to compile.

# How to Execute:
=================

1. Compile the code as mentioned above.
2. The program expects two command line arguments "<executable name> <input method>".
   - Type "./bin/test f" in the terminal to execute the program by providing taskset inputs via file "input.txt".
   - Type "./bin/test t" in the terminal to execute the program by generating taskset inputs via the task parameter generator module included in the code.

# Structure of the program:
===========================

1. driver.c takes the required input in one of the following two ways (input method is passed as the second argument after executable name via command line):
   - If the second argument is 'f' --> The program accepts an input file "input.txt" containing task set parameters for all tasksets.
   - If the second argument is 't' --> The program generates the task set parameters for all tasksets.
2. The program may thus proceed in one of the following two ways:
	 - In case of input method 'f', the "fetch_task_parameter" function is called by the driver to read task parameters from the input file "input.txt".
	 - In case of input method 't', the "task_parameter_generator" function is called by the driver to generate taskset parameters using the method described in section 5.1 of our paper.
3. The schedulability test is applied to the generated / fetched tasksets using both the algorithms (presented in section 4 of the paper) to determine whether it is EDF schedulable.	
4. Based on the return values of the schedulability test functions, the final outputs are recorded.
5. The output of the code is printed out on the terminal screen and stored as comma separated values in the file "output.csv".

# Directory structure:
=====================

mcqpa-sim-expts
├── bin
│   └── test
├── inc
│   ├── auxiliary.h
│   ├── generator.h
│   ├── schedtest.h
│   └── tasks.h
├── input.txt
├── LICENSE
├── makefile
├── obj
│   ├── auxiliary.o
│   ├── driver.o
│   ├── generator.o
│   ├── iterative.o
│   └── mcqpa.o
├── README.txt
├── references.txt
└── src
    ├── auxiliary.c
    ├── driver.c
    ├── generator.c
    ├── iterative.c
    └── mcqpa.c

4 directories, 20 files

# List of Files:
================

Program files:
--------------

1. driver.c: Contains main. The driver module takes / generates the input tasksets and starts the simulation.
2. tasks.h: Contains task ADT definition and related macros.
3. generator.c: Contains task set parameter generator functions.
4. generator.h: Contains macros (defined for default parameter values/limits) and function declarations for the task set generator module.
5. iterative.c: Contains implementation of the exact EDF schedulability test for 3-parameter sporadic task systems [3] --> Listed as Algorithm 1 in the paper
6. mcqpa.c: Contains implementation of the proposed schedulability test, MC-QPA --> Listed as Algorithm 2 in the paper
7. schedtest.h: Contains macros (for return values), function declarations and ADT definitions for the schedulability test modules.
8. auxiliary.c: Contains auxiliary functions required by the schedulability test modules.
9. auxiliary.h: Contains function declarations for the auxiliary functions module.

Makefile:
---------

10. makefile: Used to compile the code.

Input file:
-----------

11. input.txt: Required when input menthod 'f' is used, i.e., taskset parameters are provided via file. The format of the file is as follows:
    <number of tasksets>
    <number of tasks in the i-th taskset>
    <period>	<deadline>	<criticality>	<wcet[LO]>	<wcet[HI]>    --.
    <period>	<deadline>	<criticality>	<wcet[LO]>	<wcet[HI]>	|
       .	    .		      .	    	    .       	    . 		|-- Task parameters 
       .	    .		      .	    	    .	    	    .		|   for i-th taskset
    <period>	<deadline>	<criticality>	<wcet[LO]>	<wcet[HI]>    --'
        

	--> NOTE THAT:
	    * Period and deadline must be positive integers.
	    * Criticality value '0' represents LO, '1' represents HI.

Output file:
------------

12. output.csv: Contains the following information in the form of comma seprated values
For input method 't': Taskset utilization, Weighted Schedulability, Number of interval lengths checked by Iterative test and MC-QPA, Number of (t,s) pair-points checked by Iterative test and MC-QPA
For input method 'f': Schedulability of each taskset>, Number of interval lengths checked by Iterative test and MC-QPA>, Number of (t,s) pair-points checked by Iterative test and MC-QPA

References:
-----------

13. references.txt: The sources used for some of the algorithms in our code are cited in this file.


