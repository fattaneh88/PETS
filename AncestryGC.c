#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../include/justGarble.h"

// number of SNP markers 
#define markers    131072
// lenght of output
#define m     7
// length of inputs
#define n   2 * markers

int *final;

int main(int argc, char **argv) {
	srand(time(NULL));
	GarbledCircuit garbledCircuit;
	GarblingContext garblingContext;

	//Set up circuit parameters
	int q = 10000000;
	int r = 10000000;

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

	int inpXOR[n];
	countToN(inpXOR, n);
	int outputsXOR[n/2];

	XORCircuit(&garbledCircuit, &garblingContext, n, inpXOR, outputsXOR);
	
	int inp1[n/2];
	int i;
	for (i = 0; i < n/2; i++)
		inp1[i] = outputsXOR[i];
	int inp2[n/4];

	int number = n/2;
	while(number != 1)
	{
		ADDCircuit(&garbledCircuit, &garblingContext, number, &inp1[0], &inp2[0]);
		for (i = 0; i < number/2; i++)
			inp1[i] = inp2[i];
		number/=2;
	}

	final = &inp2[0];

	finishBuilding(&garbledCircuit, &garblingContext, outputMap, final);

	long int timeGarble[TIMES];
	long int timeEval[TIMES];
	double timeGarbleMedians[TIMES];
	double timeEvalMedians[TIMES];
	int j;
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

