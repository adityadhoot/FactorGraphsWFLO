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

int allocatePopulation(Population *population, long N, int n){
}

int freePopulation(Population *population){
}

int evaluatePopulation(Population *population){
}

int computeUnivariateFrequencies(Population *population, float *p1){
}

int copyIndividual(Population *population, long where, char *x, float f){
}

int swapIndividuals(Population *population, long first, long second){
}

int printIndividual(FILE *out, char *x, int n){
}

