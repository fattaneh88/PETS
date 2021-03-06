#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../include/justGarble.h"

// number of SNP markers 
#define markers    10
// lenght of output
#define m     1
// length of inputs
#define n   2 * markers

int *final;

int main(int argc, char **argv) {
	srand(time(NULL));
	GarbledCircuit garbledCircuit;
	GarblingContext garblingContext;

	//Set up circuit parameters
	int q = 2 * markers - 1;
	int r = 4 * markers - 1;

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

	int inpAND[n];
	countToN(inpAND, n);
	int outputsAND[n/2];

	AND1Circuit(&garbledCircuit, &garblingContext, n, inpAND, outputsAND);

	int outputs[1];

	ORCircuit(&garbledCircuit, &garblingContext, n/2, outputsAND, outputs);

	final = &outputs[0];

	finishBuilding(&garbledCircuit, &garblingContext, outputMap, final);

	long int timeGarble[TIMES];
	long int timeEval[TIMES];
	double timeGarbleMedians[TIMES];
	double timeEvalMedians[TIMES];
	int j, i;
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

