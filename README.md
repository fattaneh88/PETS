# PETS
Here is all the implementations of the following publication:

M. Blanton, F. Bayatbabolghani. Efficient Server-Aided Secure Two-Party Function Evaluation with Applications to Genomic Computation, to The annual Privacy Enhancing Technologies Symposium, Darmstadt, Germany, July 2016

Our implementations were built in C/C++, and we used JustGarble and MIRACL libraries as external libraries. We updated some of the implementations of the JustGarble library and all are included in this project. The implementations have two separate parts: 
The first part is circuits implementations including the following source codes:
- AES.c
- AncestryGC.c
- circuit.c
- CompatabilityGC.c
- EditD.c
- eval.c
- garble.c
- hammingD.c
- MMultiplication.c
- PaternityGC.c
- util.c
- util.h

This part can be compiled by the JustGarble library Makefile. 


The second part of implementations is ZKPKs and OT and including two source codes:
- COT.c
- COT.h

To run each procedure of the COT.c, you need to write a simple program to call the procedure. As an example to call OT-extension you can use main.c. 

Note that JustGarble library is only used for the first part of implementations and MIRACL library is only used for the second part of implementations.

