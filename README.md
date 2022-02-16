For this project, you need to implement the data carving classifier and mapper as described on the projects page. Your program needs to take a single command line argument (which is the name of a data file containing JPG and HTML fragments) and create two files:
"classification", which has the classification file format, and contains the classification data for each cluster of the input file.
"map", which has the map file format and contains a map entry for each cluster of the input file.
You are given skeleton code for this project, which contains the following files:
common.h - a header file containing constants to be used by all code
classify.h - a header file containing function signatures for the classification functions
classify.c - a source file containing code that implements the classification functions
project3.c - a skeleton source file for the main application
Makefile - a make file for the project
A note on the classification functions: the has_X_body() functions (where X is jpg or html) may return false negatives. I.e. they return false, when indeed the cluster belongs to type X. This happens when it is the last cluster for a file, containing not much actual file data, but rather a large amount of zero bytes to pad the end of the cluster. Thus, you may further need to test a cluster if it contains a header or a footer of type X, in which case you may automatically assume that the file type is of type X.

For this project, only modify the project3.c file. You may not create any additional files. You may not change the Makefile. Your code must work with the Makefile provided.

For the mapper implementation, you may opt to implement your state diagram from Project 2. However, for this project, you may assume that the input file only contains well-formed input. I.e., only the valid cases described for Project 2 will be in the input file. This should simplify the mapper logic considerably.
