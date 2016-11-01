#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../include/justGarble.h"

// size of matrix 
#define N    4
// lenght of each element
#define l     32
// length of inputs
#define n   2 * N * N * l
//length of outputs
#define m   1

int *final;

int main(int argc, char **argv) {
	srand(time(NULL));
	GarbledCircuit garbledCircuit;
	GarblingContext garblingContext;

	//Set up circuit parameters
	int q = 1000000;
	int r = 1000000;

	//Set up input and output tokens/labels.
	block *labels = (block*) malloc(sizeof(block) * 2 * n);
	block *outputbs = (block*) malloc(sizeof(block) * m);

	OutputMap outputMap = outputbs;
	InputLabels inputLabels = labels;

	//Actually build a circuit. Alternatively, this circuit could be read
	//from a file.
	createInputLabels(labels, n);
	createEmptyGarbledCircuit(&garbledCircuit, n, m, q, r, inputLabels);
	startBuilding(&garbledCircuit, &garblingContext);

	int inp[n];
	countToN(inp, n);

	int out[n];

	int sum[2*l];
	int mul1[2*l], mul2[2*l];
	int add1[4*l];

	int i, j , k, f;
	for (i = 0 ; i < N; i++){
		for (j = 0; j < N; j++){
			for (f = 0 ; f <  2*l; f++)
				sum[f] = fixedZeroWire(&garbledCircuit, &garblingContext);
			
			for (k = 0; k < N; k++){
				for (f = 0; f < l; f++)
					mul1[f] = inp[i*l*N+k*l+f];
				for (f = 0; f < l; f++)
					mul1[f+l] = inp[N*N*l+k*l*N+j*l+f];
	
				MULCircuit(&garbledCircuit, &garblingContext, 2*l, mul1, mul2);

				for (f = 0; f < 2*l; f++)
					add1[f] = mul2[f];
				for (f = 0; f < 2*l; f++)
					add1[f+2*l] = sum[f];

				ADDCircuit(&garbledCircuit, &garblingContext, 4*l, add1, sum);					
			}
			
			for (f = 0; f < 2*l; f++)
				out[i*2*l*N+j*2*l+f] = sum[f];
		}
	}

	final = &out[0];

	finishBuilding(&garbledCircuit, &garblingContext, outputMap, final);

	long int timeGarble[TIMES];
	long int timeEval[TIMES];
	double timeGarbleMedians[TIMES];
	double timeEvalMedians[TIMES];
	for (j = 0; j < TIMES; j++) {
		for (i = 0; i < TIMES; i++) {
			timeGarble[i] = garbleCircuit(&garbledCircuit, inputLabels, outputMap);
			timeEval[i] = timedEval(&garbledCircuit, inputLabels);
		}
		timeGarbleMedians[j] = ((double) median(timeGarble, TIMES))/ garbledCircuit.q;
		timeEvalMedians[j] = ((double) median(timeEval, TIMES))/ garbledCircuit.q;
	}
	double garblingTime = doubleMean(timeGarbleMedians, TIMES);
	double evalTime = doubleMean(timeEvalMedians, TIMES);
	printf("%lf %lf\n", garblingTime, evalTime);
	printf("%d\n", garbledCircuit.q);
	printf("%d\n", garbledCircuit.r);
	return 0;

}

