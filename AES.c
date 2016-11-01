#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include "../include/justGarble.h"

int *final;

int main() {
srand(time(NULL));
	GarbledCircuit garbledCircuit;
	GarblingContext garblingContext;

	int roundLimit = 10;
	int n = 128 * (roundLimit + 1);
	int m = 128;
	int q = 50000; //Just an upper bound
	int r = 50000;

	int inp[n];
	countToN(inp, n);

	int addKeyInputs[n * (roundLimit + 1)];
	int addKeyOutputs[n];
	int subBytesOutputs[n];
	int shiftRowsOutputs[n];
	int mixColumnOutputs[n];
	int round;
	block labels[2 * n];
	block outputbs[m];
	OutputMap outputMap = outputbs;
	InputLabels inputLabels = labels;
	int i, j;

	createInputLabels(labels, n);
	createEmptyGarbledCircuit(&garbledCircuit, n, m, q, r, inputLabels);
	startBuilding(&garbledCircuit, &garblingContext);

	countToN(addKeyInputs, 256);

	for (round = 0; round < roundLimit; round++) {

		AddRoundKey(&garbledCircuit, &garblingContext, addKeyInputs,
				addKeyOutputs);

		for (i = 0; i < 16; i++) {
			SubBytes(&garbledCircuit, &garblingContext, addKeyOutputs + 8 * i,
					subBytesOutputs + 8 * i);
		}

		ShiftRows(&garbledCircuit, &garblingContext, subBytesOutputs,
				shiftRowsOutputs);

		for (i = 0; i < 4; i++) {
			if (round == roundLimit - 1)
				MixColumns(&garbledCircuit, &garblingContext,
						shiftRowsOutputs + i * 32, mixColumnOutputs + 32 * i);
		}
		for (i = 0; i < 128; i++) {
			addKeyInputs[i] = mixColumnOutputs[i];
			addKeyInputs[i + 128] = (round + 2) * 128 + i;
		}
	}

	final = mixColumnOutputs;
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

