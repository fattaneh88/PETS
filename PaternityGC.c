#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../include/justGarble.h"

// number of STRs 
 #define N    26 
// length of STRs in binary form  
 #define l    9
// lenght of output
#define m     1
//number of inputs
#define n    4*l*N
int *final;

int main() 
{	
        srand(time(NULL));
        GarbledCircuit garbledCircuit;
        GarblingContext garblingContext;

        int q = n + (4 * (l - 1) + 3) * N + N - 1; //Just an upper bound
        int r = 2 * n + (l - 1) * N * 4 + 3 * N + N - 1;
	
	//Set up input and output tokens/labels.
        block *labels = (block*) malloc(sizeof(block) * 2 * n);
        block *outputbs = (block*) malloc(sizeof(block) * m);

        OutputMap outputMap = outputbs;
        InputLabels inputLabels = labels;
        
	//Actually build a circuit. Alternatively, this circuit could be read
        //from a file.
        createInputLabels(labels,n);
        createEmptyGarbledCircuit(&garbledCircuit, n, m, q, r, inputLabels);
        startBuilding(&garbledCircuit, &garblingContext);

        int inpf[N];
        int outputsf[1];

        int inpXOR[n];
        countToN(inpXOR, n);

        int outputsXOR[n];

        XOR1Circuit(&garbledCircuit, &garblingContext, n, inpXOR, outputsXOR);

        int i, j;
        int k = 0;
        for (i = 0; i < N; i++)
        {
                int outputsOR[4];
                for (j = 0; j < 4; j++)
                {
                        ORCircuit(&garbledCircuit, &garblingContext, l, &outputsXOR[k * l] , &outputsOR[j]);
                        k++;
                }
                ANDCircuit(&garbledCircuit, &garblingContext, 4, outputsOR, &inpf[i]);
        }

        ORCircuit(&garbledCircuit, &garblingContext, N, inpf, outputsf);

        final = &outputsf[0];
        finishBuilding(&garbledCircuit, &garblingContext, outputMap, final);

	long timeGarble[TIMES];
	long timeEval[TIMES];
	double timeGarbleMedians[TIMES];
	double timeEvalMedians[TIMES];

	for (j = 0; j < TIMES; j++) {
		for (i = 0; i < TIMES; i++) {
			timeGarble[i] = garbleCircuit(&garbledCircuit, inputLabels, outputMap);
			timeEval[i] = timedEval(&garbledCircuit, inputLabels);
		}
		timeGarbleMedians[j] = ((double) median(timeGarble, TIMES))
				/ garbledCircuit.q;
		timeEvalMedians[j] = ((double) median(timeEval, TIMES)) / garbledCircuit.q;
	}
	double garblingTime = doubleMean(timeGarbleMedians, TIMES);
	double evalTime = doubleMean(timeEvalMedians, TIMES);
	printf("%lf %lf\n", garblingTime, evalTime);
	printf("%d\n", garbledCircuit.q);
	printf("%d\n", garbledCircuit.r);
	return 0;
}
