//========================================================================================
// name:			initialize
//
// function:	initializes everything the Factor Graphs needs to run. 
//
// parameters: 
//
// returns:
//=========================================================================================

void factorgraphs::initialize (FactorGraphParams *fgParams){
	char filename[200];

  initializeFitness(fgParams);	// initialize fitness
  initializeMetric(fgParams);		// initialize metric
  resetFitnessCalls();   				// reset the counter for fitness calls
  setSeed(fgParams->randSeed);	// set random seed
  intializeBasicStatistics(&populationStatistics,fgParams);   // initialize statistics

  // open output files (if the base of the output file names specified)
  if (boaParams->outputFilename)
    {
      sprintf(filename,"%s.log",fgParams->outputFilename);
      logFile = fopen(filename,"w");

      sprintf(filename,"%s.fitness",fgParams->outputFilename);
      fitnessFile = fopen(filename,"w");

      sprintf(filename,"%s.model",fgParams->outputFilename);
      modelFile = fopen(filename,"w");
    }
  else
    logFile = fitnessFile = modelFile = NULL;
}

//========================================================================================
// name:			factorgraphs
//
// function:	runs the Factor Graph implementation for a given problem
//
// parameters: 
//
// returns:
//=========================================================================================
void factorgraphs::factorgraphs (FactorGraphParams *fgParams){
	//Variable Definition
	long N, numOffspring, numParents, t;
  int n;
  Population population, parents, offspring;
  int terminationReason;

	//Variable declarations
	N = fgParams->N;
	numOffspring = (long)((float)fgParams->N * fgParams->percentOffspring)/100;
	numParents = (long) fgParams->N;
	n = fgParams->n;

	//Population Allocation
	allocatePopulation (&population, N, n);
	allocatePopulation (&parents, numParents, n);
	allocatePopulation (&offspring, numOffspring, n);

	generatePopulation (&population); //first randomly generated population according to uniform distribution
	evaluatePopulation (&population); //evaluate the first population

	//Computing Statistics on the first initial population
	t = 0;
	computeBasicStatistics (&populationStatistics, t, &population, fgParams);
	generationStatistics (stdout, &populationStatistics); //output the statistics to console
	generationStatistics (logFile, &populationStatistics); //output the statistics to log
	fitnessStatistics (fitnessFile, &populationStatistics); //output fitness statistics to log

	pause(fgParams);	//lets pause here

	//Lets begin the iteration! Keep iterating until termination criteria is not met
	while (!(terminationReason = terminationCriteria(fgParams))) {
		selectTheBest (&population, &parents, fgParams); //perform truncation selection
		generateOffspring (t, &parents, &offspring, fgParams); //create offspring
		evaluatePopulation (&offspring); //evaluate the offspring for fitness
		replaceWorst (&population, &offspring); //replace the worst of the population with offspring
		t++; //iteration cycle update
		
		//Computing further statistics on the updated population set
		computeBasicStatistics (&populationStatistics, t, &population, fgParams);
		generationStatistics (stdout, &populationStatistics); //output the statistics to console
		generationStatistics (logFile, &populationStatistics); //output the statistics to log
		fitnessStatistics (fitnessFile, &populationStatistics); //output fitness statistics to log
	
		pause(fgParams);	//lets pause here	
	};

	//Computing statistics on the final population set at termination
	computeBasicStatistics (&populationStatistics, t, &population, fgParams);
	finalStatistics (stdout, terminationReasonDescription[terminationReason], &populationStatistics); //output the statistics to console
	finalStatistics(logFile,terminationReasonDescription[terminationReason],&populationStatistics);//output the statistics to log

	//Free up some memory used up here
	freePopulation (&population);
	freePopulation (&parents);
	freePopulation (&offspring);
}

//========================================================================================
// name:			terminationCriteria
//
// function:	checks whether some of the termination criteria is not matched and returns teh number of
//						the	criterion that has been met or 0;
//
// parameters: 
//
// returns:
//=========================================================================================
int factorgraphs::terminationCriteria (FactorGraphParams *fgParams){
	int result;
	result = 0; // no reason to finish yet
	
	//cehck if the proportion of optima reached the required value, if yes terminate
	if ((!result)&&(fgParams->maxOptimal>=0)){
		result = (float(populationStatistics.numOptimal*100)/(float)populationStatistics.N>=boaParams->maxOptimal)? MAXOPTIMAL_TERMINATION:0;
	}

	//check if should terminate if optimum has been found and if this is the case if yes
	if ((!result)&&(fgParams->stopWhenFoundOptimum)){
		if (isBestDefined()){
			result = (isOptimal(populationStatistics.bestX,populationStatistics.n))? OPTIMUMFOUND_TERMINATION:0;
		}
	}

 // if there's no reason to finish yet and the epsilon threshold was set, check it

  if ((!result)&&(fgParams->epsilon>=0))
    {
      int   k;
      float epsilon1;

      // set epsilon1 to (1-boaParams->epsilon)

      epsilon1 = 1-fgParams->epsilon;

      // are all frequencies closer than epsilon to either 0 or 1?

      result=EPSILON_TERMINATION;
      for (k=0; k<populationStatistics.n; k++)
	if ((populationStatistics.p1[k]>=boaParams->epsilon)&&(populationStatistics.p1[k]<=epsilon1))
	  result=0;
    }

  // check if the number of generations wasn't exceeded

  if ((!result)&&(fgParams->maxGenerations>=0))
    result = (populationStatistics.generation>=fgParams->maxGenerations)? MAXGENERATIONS_TERMINATION:0;

  // get back

  return result;
}

//========================================================================================
// name:			generateOffspring
//
// function:	generates offspring in the Factor Graph and also constructs the network for the
//						selected strings, and then uses it to generate new strings.
//
// parameters: 
//
// returns:
//=========================================================================================
void factorgraphs::generateOffspring (long t, Population *parents, Population *offspring, FactorGraphParams *fgParams){

	AcyclicOrientedGraph *G;
	FrequencyDecisionGraph **T;

	G = new AcyclicOrientedGraph (parents->n); // initialize an empty network
	// allocate memory for frequency trees
	T = new FrequencyDecisionGraph* (parents->n);

	for (int k=0; k<parents->n; k++){
		T[k] = new FrequencyDecisionGraph (parents, k);
	}
	
	constructTheNetwork (G, T, parents, fgParams);	//constructing Bayesian network to model the selected set of parents
	
	printModel (modelFile, t, G, T); //print out the model for this generation
	generateNewInstances (G, T, offspring); //generate the new points
	
	//delete network and trees
	delete G;
	for (int k=0; k<parents->n; k++){
		delete T[k];
	}

	Free (T); //delete array with the trees
}

//========================================================================================
// name:			pause
//
// function:	waits for enter key if required
//
// parameters: 
//
// returns:
//=========================================================================================
void factorgraphs::pause (FactorGraphParams *fgParams){
	if (fgParams->pause){
		printf("Press Enter to continue.");
		getchar ();
	};

}

//========================================================================================
// name:			done
//
// function:	gets back to normal what was changed in initialization of the Factor Graphs
//
// parameters: 
//
// returns:
//=========================================================================================

void factorgraphs::done (FactorGraphParams *fgParams){
	doneMetric (); //get rid of the metric
	doneFitness (fgParams); //get rid of the fitness
	doneBasicStatistics (&populationStatistics); //statistics done
	
	if (logFile){
		fclose(logFile);
	}
	
	if (fitnessFile){
		fclose (fitnessFile);
	}

	if (modelFile){
		fclose (modelFile);
	}
}

// ================================================================================
//
// name:          getLogFile
//
// function:      returns a pointer to the log file strea
//
// parameters:    (none)
//
// returns:       (FILE*) a pointer to the log file stream
//
// ================================================================================

FILE *getLogFile()
{
  return logFile;
}

// ================================================================================
//
// name:          getModelFile
//
// function:      returns a pointer to the model file stream
//
// parameters:    (none)
//
// returns:       (FILE*) a pointer to the model file stream
//
// ================================================================================

FILE *getModelFile()
{
  return modelFile;
}

// ================================================================================
//
// name:          getFitnessFile
//
// function:      returns a pointer to the fitness file stream
//
// parameters:    (none)
//
// returns:       (FILE*) a pointer to the fitness file stream
//
// ================================================================================

FILE *getFitnessFile()
{
  return fitnessFile;
}


