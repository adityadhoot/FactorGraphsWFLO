#include "population.h"

//========================================================================================
// name:			generatePopulation
//
// function:	generates all strings in a population at random with uniform distribution
//
// parameters: 
//
// returns:
//=========================================================================================

void generatePopulation(Population *population){

	for (long i=0; i<population->N; i++){
		for (int j=0; j<population->n; j++){
			if (drand()<0.5)
				population->x[i][j] = 0;
			else 
				population->x[i][j] = 1;
		}
	}

}

//========================================================================================
// name:			allocatePopulation
//
// function:	
//
// parameters: 
//
// returns:
//=========================================================================================
void allocatePopulation(Population *population, long N, int n){
	population->N = N;
	population->n = n;

	// memory allocation for double char string array
	population->x = new char* [N];
	for (int i=0; i<N; i++){
		population->x[i] = new char [n];
	}
	population->f = new float [N]; //fitness array
}

//========================================================================================
// name:			freePopulation
//
// function:	
//
// parameters: 
//
// returns:
//=========================================================================================
void freePopulation(Population *population){

	for (long i=0; i<population->N; i++){
		Free (population->x[i]);
	}

	Free (population->x);
	Free (population->f);	//free the memory used by the array of fitness values
}


//========================================================================================
// name:			evaluatePopulation
//
// function:	evaluates fitness for all strings in a population
//
// parameters: 
//
// returns:
//=========================================================================================

void evaluatePopulation(Population *population){
	for (long i=0; i<population->N; i++){
		population->f[i] = getFitnessValue(population->x[i], population->n);
	}

}

//========================================================================================
// name:			computeUnivariateFrequencies
//
// function:	computes the frequencies of 1's on all positions in a population; 
//
// parameters: 
//
// returns:
//=========================================================================================
void computeUnivariateFrequencies(Population *population, float *p1){

	count1 = new long [population->n]; //array for 1 counts
	//computing the actual univariate counts
	for (int k=0; k<population->n; k++){
		count1[k] = 0;
	}

	for (long i=0; i<population->N; i++){
		for (int k=0; k<populationp->n; k++){
			if (population->x[i][k] == 1)
				count1[k]++;
		}
	}

	//computing the actual frequencies
	for (int k=0; k<population->n; k++){
		p1[k] = (float) count1[k]/(float) population->N;
	}

	//Free memory
	Free (count1);
}

//========================================================================================
// name:			copyIndividual
//
// function:	copy a given string into a particular spot in a population 
//
// parameters: 
//
// returns:
//=========================================================================================
void copyIndividual(Population *population, long where, char *x, float f=0){

	memcpy (population->x[where],x,population->n); 	//copy the individual's chromosome
	population->f[where] = f; //copy the fitness value
}

//========================================================================================
// name:			swapIndividuals
//
// function:	swaps specified two strings in a population
//
// parameters: 
//
// returns:
//=========================================================================================
void swapIndividuals(Population *population, long first, long second){
	char *auxX;
	float *auxF;
	auxX = new char [population->n];
	
	//Swap the strings
	memcpy (auxX, population->x[first], population->n);
	memcpy (population->x[first], population->[second], population->n);
	memcpy (population->x[second], auxX, population->n);

	//Swapping the fitness values
	auxF = population->f[first];
	population->f[first] = population->f[second];
	population->f[second] = auxF;

	Free(auxX);	//free memory
}

//========================================================================================
// name:			printIndividual
//
// function:	prints out a specified string to a stream
//
// parameters: 
//
// returns:
//=========================================================================================
void printIndividual(FILE *out, char *x, int n){
	for (int k=0; k<n; k++){
		fprintf (out, "%u", x[k]);
	}
}

