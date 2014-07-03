// ################################################################################
//
// name:          fitness.cc
//
// author:        Martin Pelikan
//
// purpose:       the definition of fitness functions; in order to add a fitness  
//                one has to add it here (plus the definition in the header file
//                fitness.h); see documentation or the instructions below
//
// last modified: August 2000
//
// ################################################################################


// ================================================================================
// ================================================================================
//
// Instructions for adding a new fitness function: 
// ------------------------------------------------
//
// 1. create a function with the same input parameters as other fitness functions 
//    defined in this file (e.g., onemax) that returns the value of the fitness
//    given a binary chromosome of a particular length (sent as input parameters
//    to the fitness)
//
// 2. put the function definition in the fitness.h header file (look at onemax
//    as an example)
//
// 3. increase the counter numFitness and add a structure to the array of the
//    fitness descriptions fitnessDesc below. For compatibility of recent input
//    files, put it at the end of this array, in order not to change the numbers 
//    of already defined and used functions. The structure has the following items
//    (in this order):
//      a) a string description of the function (informative in output data files)
//      b) a pointer to the function (simple "&" followed by the name of a function)
//      c) a pointer to the function that returns true if an input solution is
//         globally optimal and false if this is not the case. If such function is
//         not available, just use NULL instead. The algorithm will understand...
//      d) a pointer to the function for initialization of the particular fitness
//         function (not used in any of these and probably not necessary for most
//         of the functions, but in case reading input file would be necessary
//         or so, it might be used in this way). Use NULL if there is no such 
//         function
//      e) a pointer to the "done" function, called when the fitness is not to be
//         used anymore, in case some memory is allocated in its initialization;
//         here it can be freed. Use NULL if there is no need for such function
//
//  4. the function will be assigned a number equal to its ordering number in the
//     array of function descriptions fitnessDesc minus 1 (the functions are
//     assigned numbers consequently starting at 0); so its number will be equal
//     to the number of fitness definitions minus 1 at the time it is added. Its
//     description in output files will be the same as the description string
//     (see 3a)
//
// ================================================================================
// ================================================================================

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include "armadillo"

#include "fitness.h"
#include "boa.h"
#include "WindFarmLayout.h"

#define numFitness 7

static Fitness fitnessDesc[numFitness] = {
  {"ONEMAX",&onemax,&areAllGenesOne,NULL,NULL},
  {"Quadratic 0.9 0 0 1",&quadratic,&areAllGenesOne,NULL,NULL},
  {"Fitness-3 DECEPTIVE",&f3deceptive,&areAllGenesOne,NULL,NULL},
  {"5-ORDER TRAP (Illinois Report No. 95008)",&trap5,&areAllGenesOne,NULL,NULL},
  {"3deceptive Bipolar",&f3deceptiveBipolar,&areBlocks6ZeroOrOne,NULL,NULL},
  {"3deceptive (OVERLAPPING in 1bit)",&f3deceptiveOverlapping,&areAllGenesOne,NULL,NULL},
  {"Wind Farm Layout",&wflofitness,NULL,NULL,NULL},
};

// ------------------
// the fitness in use
// ------------------

Fitness *fitness;

// ---------------------------
// the number of fitness calls
// ---------------------------

long   fitnessCalls_;


std::string wl_file = "input/init_layout.out";
std::string turbine_file = "input/turbine_coordinates.in";
std::string wrf_file = "input/wrf_test.rsf";
std::string fcoordinates = "input/coordinates";
std::string fwindDirections = "input/windDirections";
std::string fwindSpeeds = "input/windSpeeds";
std::string fwindProb = "input/windProbabilities";
std::string fZ = "input/turbineHeights";
std::string fCt = "input/CtTable";
std::string fDRot = "input/rotorDiameters";
std::string fZ0 = "input/roughnessLength";
std::string fpowerTable = "input/powerTable";

WindFarmLayout wind_farm_layout (wl_file.c_str(), turbine_file.c_str(), wrf_file.c_str(), fcoordinates,
																			fwindDirections, fwindSpeeds, fwindProb, fZ,
																			fCt, fDRot, fZ0, fpowerTable);
// ================================================================================
//
// name:          onemax
//
// function:      computes the value of onemax (or bit-count) function
//
// parameters:    x............a binary string
//                n............the length of a string
//
// returns:       (float) the sum of input bits
//
// ================================================================================

float onemax(char *x, int n)
{
   int s;

   s=0;

   for (register int i=0; i<n; i++)
       s += x[i];

   return (float) s;
}

// ================================================================================
//
// name:          quadratic
//
// function:      computes the value of quadratic function
//
// parameters:    x............a binary string
//                n............the length of a string
//
// returns:       (float) sum_{i=0}^{n/2-1}{f2(x[2i],x[2i+1]}, where f2(0,0)=0.9,
//                f2(1,1)=1, and f(0,1)=f(1,0)=0
//
// ================================================================================

float quadratic(char *x, int n)
{
  char a,b;
  float f;
  register int i;

  f = 0;

  for (i=0; i<n;)
    {
      a = x[i++];
      b = x[i++];
      
      if (a==0)
	if (b==0)
	  f += 0.9;
	else
	  f += 0;
      else
	if (b==0)
	  f += 0;
	else
	  f += 1;
    }

  return f;
}

// ================================================================================
//
// name:          f3deceptive
//
// function:      computes the value of 3-deceptive function
//
// parameters:    x............a binary string
//                n............the length of a string
//
// returns:       (float) sum_{i=0}^{n/3-1}{f3(x[3i],x[3i+1],x[3i+2]}, where 
//                f3(0,0,0)=0.9, f3(0,0,1)=f3(0,1,0)=f3(1,0,0)=0.8,
//                f3(0,1,1)=f3(1,0,1)=f3(1,1,0)=0, and f3(1,1,1)=1
//
// ================================================================================

float f3deceptive(char *x, int n)
{
  float f;
  char s;
  register int i;

  f = 0;

  for (i=0; i<n;)
    {
      s  = x[i++];
      s += x[i++];
      s += x[i++];

      if (s==0)
	f+=0.9;
      else
      if (s==1)
	f+=0.8;
      else
      if (s==3)
	f+=1;
    }

  return f;
}

// ================================================================================
//
// name:          trap5
//
// function:      computes the value of trap-5 function
//
// parameters:    x............a binary string
//                n............the length of a string
//
// returns:       (float) the value of a trap5 function
//
// ================================================================================

float trap5(char *x, int n)
{
  int s;
  char u;
  register int i;

  s = 0;

  for (i=0; i<n; )
    {
      u  = x[i++];
      u += x[i++];
      u += x[i++];
      u += x[i++];
      u += x[i++];
      
      if (u<5)
	s += 4-u;
      else
	s += 5;
    };

   return (float) s;
}

// ================================================================================
//
// name:          f3deceptiveBipolar
//
// function:      computes the value of 3-deceptive bipolar function
//
// parameters:    x............a binary string
//                n............the length of a string
//
// returns:       (float) the value of a 3-deceptive bipolar function
//
// ================================================================================

float f3deceptiveBipolar(char *x, int n)
{
  float f;
  char s;
  register int i;
  
  f = 0;
  
  for (i=0; i<n;)
    {
      s  = x[i++];
      s += x[i++];
      s += x[i++];
      s += x[i++];
      s += x[i++];
      s += x[i++];

      s = abs(s-3);

      if (s==0)
	f+=0.9;
      else
	if (s==1)
	  f+=0.8;
	else
	  if (s==3)
	    f+=1;
    }

  return f;
}

// ================================================================================
//
// name:          f3deceptiveOverlapping
//
// function:      computes the value of 3-deceptive function overlapped in 1 bit
//                in a chain-like fashion
//
// parameters:    x............a binary string
//                n............the length of a string
//
// returns:       (float) the value of a 3-deceptive function with overlapping
//
// ================================================================================

float f3deceptiveOverlapping(char *x, int n)
{
  float f;
  char s;
  register int i;

  f = 0;

  for (i=0; i<n-1;)
    {
      s  = x[i++];
      s += x[i++];
      s += x[i];

      if (s==0)
	f+=0.9;
      else
      if (s==1)
	f+=0.8;
      else
      if (s==3)
	f+=1;
    }

  return f;
}


float wflofitness (char *x, int n)
{
	float f;
	//register int i;
	
	/*std::string wl_file = "input/init_layout.out";
	std::string turbine_file = "input/turbine_coordinates.in";
	std::string wrf_file = "input/wrf_test.rsf";
	std::string fcoordinates = "input/coordinates";
	std::string fwindDirections = "input/windDirections";
	std::string fwindSpeeds = "input/windSpeeds";
	std::string fwindProb = "input/windProbabilities";
	std::string fZ = "input/turbineHeights";
	std::string fCt = "input/CtTable";
	std::string fDRot = "input/rotorDiameters";
	std::string fZ0 = "input/roughnessLength";
	std::string fpowerTable = "input/powerTable";

	WindFarmLayout wind_farm_layout (wl_file.c_str(), turbine_file.c_str(), wrf_file.c_str(), fcoordinates,
																			fwindDirections, fwindSpeeds, fwindProb, fZ,
																			fCt, fDRot, fZ0, fpowerTable);*/

	int turs = 30;	
	int count = 0;
	for (int i=0; i<n;i++){
		if (x[i] == 1){
			count = count + 1;
		}
	}
	//std::cout <<count <<std::endl;
	if (count != turs){
		//std::cout <<count <<std::endl;
		return (-1*((count-turs)*(count-turs)));
	}

	arma::vec turbines (turs*2);
	
	count = 0;
	for (int i=0; i<n;i++){
		if (x[i] == 1){
			int j = i;
			int x,y;
			
			if (i<10){
				x = 0;
				y = i;
			}
			else {
				y = i%10;
				x = (i-y)/10;
			}
			if (count < turs){
			turbines(count) = (x*300);
			int test = count + turs;;
			turbines (test) = (y*300);
		}
			count = count + 1;
		}
	}

	//std::cout <<turbines <<std::endl;
	f = (float)(wind_farm_layout.calculateFarmPower(turbines));
	
	if (f > 100){
		for (int i=0;i<10;i++){
			for (int j=0;j<10;j++){
				std::cout <<(int)(x[(10*i)+j])<<" ";
			}		
					std::cout <<std::endl;
		}
					std::cout <<std::endl;
	}	
	return f;
}

//char areAllTurbinesOptimal(char *x, int n){
//}

//}
// ================================================================================
//
// name:          areAllGenesOne
//
// function:      checks whether input string is composed of one's only
//
// parameters:    x............a binary string
//                n............the length of a string
//
// returns:       (char) non-zero if x=(1,...,1), 0 otherwise
//
// ================================================================================

char areAllGenesOne(char *x, int n)
{
  register int i;

  for (i=0; (i<n)&&(x[i]==1); i++);
    
  return (i==n);
}

// ================================================================================
//
// name:          areAllBlocks6ZeroOrOne
//
// function:      checks whether all tight non-overlapping blocks in an input
//                string are 0...0 or 1...1
//
// parameters:    x............a binary string
//                n............the length of a string
//
// returns:       (char) non-zero if all blocks are 0...0 or 1...1, 0 otherwise
//
// ================================================================================

char areBlocks6ZeroOrOne(char *x, int n)
{
  register int i;
  int j;
  int ok;

  ok = 1;

  for (i=0; i<n;)
    {
      if (x[i++]==0)
	{
	  for (j=1; j<6; j++)
	    if (x[i++]!=0)
	      ok = 0;
	}
      else
	{
	  for (j=1; j<6; j++)
	    if (x[i++]!=1)
	      ok = 0;
	}
    }

  return ok;
}

// ================================================================================
// ================================================================================
// ================================================================================


// ================================================================================
//
// name:          setFitness
//
// function:      sets the tested function according to its ordering number
//
// parameters:    n............the number of a fitness to use
//
// returns:       (int) 0
//
// ================================================================================

int setFitness(int n)
{
  if ((n>=0) && (n<numFitness))
    fitness = &(fitnessDesc[n]);
  else
    {
      fprintf(stderr,"ERROR: Specified fitness function doesn't exist (%u)!",n);
      exit(-1);
    }
  
  return 0;
}

// ================================================================================
//
// name:          getFitnessDesc
//
// function:      gets a string description of a particular function
//
// parameters:    n............the number of a fitness to get the description of
//
// returns:       (char*) the string description of the function
//
// ================================================================================

char *getFitnessDesc(int n)
{
  return fitnessDesc[n].description;
}

// ================================================================================
//
// name:          initializeFitness
//
// function:      calls an init method of chosen fitness (if any)
//
// parameters:    boaParams....the parameters passed to the BOA
//
// returns:       (int) return value of the called method or 0
//
// ================================================================================

int initializeFitness(BoaParams *boaParams)
{
  if (fitness->init)
    return fitness->init(boaParams);

  return 0;
}

// ================================================================================
//
// name:          doneFitness
//
// function:      calls a done method of chosen fitness (if any)
//
// parameters:    boaParams....the parameters passed to the BOA
//
// returns:       (int) return value of the called method or 0
//
// ================================================================================

int doneFitness(BoaParams *boaParams)
{
  if (fitness->done)
    return fitness->done(boaParams);
  
  return 0;
}

// ================================================================================
//
// name:          getFitnessValue
//
// function:      evaluates the fitness for an input string
//
// parameters:    x............the string
//                n............the length of the string
//
// returns:       (float) the value of chosen fitness for the input string
//
// ================================================================================

float getFitnessValue(char *x, int n) 
{
  fitnessCalled();
  return fitness->fitness(x,n);
}

// ================================================================================
//
// name:          isBestDefined
//
// function:      checks whether the proposition identifying the optimal strings is
//                defined for a chosen function
//
// parameters:    (none)
//
// returns:       (int) non-zero if the proposition function defined, 0 otherwise
//
// ================================================================================

int isBestDefined()
{
  return (int) (fitness->isBest!=NULL);
}

// ================================================================================
//
// name:          isOptimal
//
// function:      checks whether the input string is optimal (assuming the 
//                proposition function is defined)
//
// parameters:    x............the string
//                n............the length of the string
//
// returns:       (int) the value of the proposition
//
// ================================================================================

int isOptimal(char *x, int n) 
{
 return fitness->isBest(x,n);
}

// ================================================================================
//
// name:          resetFitnessCalls
//
// function:      resets the number of fitness calls (sets it to 0)
//
// parameters:    (none)
//
// returns:       (int) 0
//
// ================================================================================

int resetFitnessCalls(void)
{
  return (int) (fitnessCalls_=0);
}

// =============================================================

long fitnessCalled(void)
{
  return fitnessCalls_++;
}

// =============================================================

long getFitnessCalls(void)
{
  return fitnessCalls_;
}
