# Garbage-Collector

Homework 4: a garbage collector for C

Homework Objectives

Get experience working with existing C code
Get experience working with operating system defined protocols/systems
Learn about garbage collection
Homework Repository

Click here to check out your repository with the starting code: https://classroom.github.com/assignment-invitations/a5e953e658625f40f28ffeab024335af

In this homework, we build a basic, conservative garbage collector for C programs. Starting from the set of root pointers present in the stack and global variables, we traverse the “object graph” (in the case of malloc, a chunk graph) to find all chunks reachable from the root. These are marked using the third lowest order bit in the size field of each chunk.

As of today we have not covered the concepts related to garbage collection in class; if you wish to get a head start on this homework, take a look at section 9.10, and 9.10.2 specifically.

The skeleton code in the public git repository provides supporting code for finding the limits of the global variable area in memory, as well as for the stack and heap areas. It also demonstrates very basic marking and sweeping. Your task is to complete the garbage collector, so that it frees all garbage, and leaves every other chunk intact. To compile the template, type “make”, and run it with ./hw4.

Performance requirements

For full points, your program should pass all tests in main.c - that is, it should free all garbage memory. At present, these are not coded as actual tests, they simply output the heap size and number of free and in-use chunks. Nevertheless, your program should produce the correct values.

Runtime performance is not a major goal for this homework. However, the program should finish well within 2 seconds.

As usual, you should not be copying significant portions of code from internet resources.  Please make sure you cite any sources.
