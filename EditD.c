#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../include/justGarble.h"

// number of inputs
#define number    128
// length of each input
#define l     8
// length of inputs
#define n   2 * number * l
// length of output
#define m    7


int *final;

int main(int argc, char **argv) {
	srand(time(NULL));
	GarbledCircuit garbledCircuit;
	GarblingContext garblingContext;

	//Set up circuit parameters
	int q = 5000000;
	int r = 5000000;

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
	
	int dis[(number+1)*(number+1)*m];
	int i, j, a;
	for (i = 0; i < number+1; i++){
		j = m-1;
		a = i;
 		while (a > 0){
       			if (a%2 == 1)
           			dis[i*m+j] = fixedOneWire(&garbledCircuit, &garblingContext);
			else
				dis[i*m+j] = fixedZeroWire(&garbledCircuit, &garblingContext);
      			j=-1;
        		a = a/2 ;
		}
		if (a == 0 & j > -1)
			for (j; j > -1; j--)
				dis[i*m+j] = fixedZeroWire(&garbledCircuit, &garblingContext);				
 	}

	for (i = 0; i < number+1; i++){
		j = m-1;
		a = i;
 		while (a > 0){
       			if (a%2 == 1)
           			dis[i*m*(number+1)+j] = fixedOneWire(&garbledCircuit, &garblingContext);
			else
				dis[i*m*(number+1)+j] = fixedZeroWire(&garbledCircuit, &garblingContext);
      			j=-1;
        		a = a/2 ;
		}
		if (a == 0 & j > -1)
			for (j; j > -1; j--)
				dis[i*m*(number+1)+j] = fixedZeroWire(&garbledCircuit, &garblingContext);				
 	}

	int min[m], temp1[2*l], temp2[1], out[1], temp3[2*m], temp4[m], temp6[m], temp8[m];

	for (i = 1; i < number+1; i++)
		for (j = 1; j < number+1; j++)
		{
		  	for (a = 0; a < l; a++)
				temp1[a] = inp[(i-1)*l+a];

		  	for (a = 0; a < l; a++)
				temp1[a+l] = inp[number*l+(j-1)*l+a];

			EQUCircuit(&garbledCircuit, &garblingContext,  2*l, temp1, temp2);
			NOTCircuit(&garbledCircuit, &garblingContext, 1, temp2, out);
			for (a = 0; a < m; a++)
				temp3[a] = dis[(i-1)*(number+1)*m+(j-1)*m+a];
			for (a = 0; a < m-1; a++)
				temp3[a+m] = fixedZeroWire(&garbledCircuit, &garblingContext);
			temp3[a+m] = out[0];

			ADDCircuit(&garbledCircuit, &garblingContext,  2*m, temp3, temp8);
			

			for (a = 0; a < m; a++)
				temp3[a] = dis[(i-1)*(number+1)*m+j*m+a];
			for (a = 0; a < m-1; a++)
				temp3[a+m] = fixedZeroWire(&garbledCircuit, &garblingContext);
			temp3[a+m] = fixedOneWire(&garbledCircuit, &garblingContext);

			ADDCircuit(&garbledCircuit, &garblingContext,  2*m, temp3, temp4);

			for (a = 0; a < m; a++)
				temp3[a] = dis[(i)*(number+1)*m+(j-1)*m+a];
			for (a = 0; a < m-1; a++)
				temp3[a+m] = fixedZeroWire(&garbledCircuit, &garblingContext);
			temp3[a+m] = fixedOneWire(&garbledCircuit, &garblingContext);

			ADDCircuit(&garbledCircuit, &garblingContext,  2*m, temp3, temp6);

			for (a = 0; a < m; a++)
				temp3[a] = temp4[a];
			for (a = 0; a < m; a++)
				temp3[a+m] = temp6[a];

			MINCircuit(&garbledCircuit, &garblingContext,  2*m, temp3, min);

			for (a = 0; a < m; a++)
				temp3[a] = temp8[a];
			for (a = 0; a < m; a++)
				temp3[a+m] = min[a];

			MINCircuit(&garbledCircuit, &garblingContext,  2*m, temp3, min);

			for (a = 0; a < m; a++)
				dis[(i)*(number+1)*m+(j)*m+a] = min[a];		
	
		}

	final = &dis[(number)*(number+1)*m+(number)*m+a];

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

