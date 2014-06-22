// ================================================================================
// name:          initializeBasicStatistics
//
// function:      initializes basic statistics stuff
//
// parameters:    fgParams....the parameters sent to the fg
//
// returns:       (int) 0
// ================================================================================

void statistics::intializeBasicStatistics(BasicStatistics *statistics, fgParams *fgParams){
	//allocate memory for univariate frequencies
	statistics->p1 = new float [fgParams->n];
	return; 
}

// ================================================================================
// name:          doneBasicStatistics
//
// function:      done method?
//
// parameters:   
//
// returns:       
// ================================================================================
void statistics::doneBasicStatistics(BasicStatistics *statistics){
	//free up the memory used for univariate frequencies
	Free (statistics->p1);
	return;
}

// ================================================================================
// name:          computeBasicStatistics
//
// function:      computes some basic statistics (on fitness and so) and sets some
//                variables required for printing this information out in the
//                future
//
// parameters:    t............number of generation
//                population...a current population
//                boaParams....the parameters sent to the BOA
//
// returns:       (int) 0
// ================================================================================
void statistics::computeBasicStatistics(BasicStatistics *statistics, long t, Population *population, fgParams *fgParams){
	//some variable definition
	statistics->N = population->N;
	statistics->n = population->n;
	statistics->generation = t;
	statistics->guidanceThreshold = fgParams->guidanceThreshold;

	//computing the maximal, minimal, and average fitness in the population
	statistics->max = 0;
	statistics->minF = statistics->maxF = statistics->avgF = population->f[0];
	if (isBestDefined()){
		statistics->numOptimal = (isOptimal(population->x[0],statistics->n))? 1:0;
	}

	for (long i=1; i<statistics->N; i++){
		statistics->avgF + = population->f[i];

		//Checking to see if a better min or max is found
		if (population->f[i] < statistics->minF){
			statistics->minF = population->f[i];
		}
		else {
			 if (population->f[i]>statistics->maxF){
					statistics->maxF = population->f[i];
					statistics->max = i;
				}
		}

		//checking if the string is optimal.
		if (isBestDefined()){
			if (isOptimal(population->x[i], statistics->n)){
				statistics->numOptimal++;			
			}
		}	
	};

	statistics->avgF /= (double) statistics->N;

	computeUnivariateFrequencies(population, statistics->p1); //allocate memory and compute univariate frequencies
	
	if (isBestDefined()){
		statistics->bestX = population->x[statistics->max];
	}

	return;
}



// ================================================================================
// name:          generationStatistics
//
// function:      prints out various statistics
//
// parameters:   
//
// returns:       (int) 0
// ================================================================================
int statistics::generationStatistics(FILE *out, BasicStatistics *statistics){
  // output not null?

  if (out==NULL)
    return;

  // print it all out
  fprintf(out,"--------------------------------------------------------\n");
  fprintf(out,"Generation                   : %lu\n",statistics->generation);
  fprintf(out,"Fitness evaluations          : %lu\n",getFitnessCalls());
  fprintf(out,"Fitness (max/avg/min)        : (%5f %5f %5f)\n",statistics->maxF,statistics->avgF,statistics->minF);

  if (isBestDefined()){
    fprintf(out,"Percentage of optima in pop. : %1.2f\n",((float)statistics->numOptimal/(float)statistics->N)*100);
	}

  fprintf(out,"Population bias              : ");
  printGuidance(out,statistics->p1,statistics->n,statistics->guidanceThreshold);
  fprintf(out,"\n");
  fprintf(out,"Best solution in the pop.    : ");

  if (isBestDefined()){
    printIndividual(out,statistics->bestX,statistics->n);
 	}
  else {
    fprintf(out,"Statistic not available.");
	}

  fprintf(out,"\n");
  return;
}

// ================================================================================
//
// name:          fitnessStatistics
//
// function:      prints out the statistics on fitness
//
// parameters:    out..........output stream
//
// returns:       void
//
// ================================================================================

void statistics::fitnessStatistics(FILE *out, BasicStatistics *statistics)
{
  // output not null?
  if (out==NULL){
    return;
	}

  // print out the fitness statistics
  fprintf(out,"%3lu %7lu %10f %10f %10f\n",statistics->generation,getFitnessCalls(),statistics->maxF,statistics->avgF,statistics->minF);

  return;
}

// ================================================================================
//
// name:          finalStatistics
//
// function:      prints out statistics (called after the run is over)
//
// parameters:    out...........output stream
//                termination...the reason for terminating the algorithm (string)
//
// returns:       void
//
// ================================================================================

void statistics::finalStatistics(FILE *out, char *termination, BasicStatistics *statistics)
{
  // is output stream null?
  if (out==NULL){
    return;
	}

  // print out all the statistics to the file
  fprintf(out, "\n=================================================================\n");
  fprintf(out, "FINAL STATISTICS\n");
  fprintf(out, "Termination reason           : %s\n",termination);
  fprintf(out, "Generations performed        : %lu\n",statistics->generation);
  fprintf(out, "Fitness evaluations          : %lu\n",getFitnessCalls());
  fprintf(out, "Fitness (max/avg/min)        : (%5f %5f %5f)\n",statistics->maxF,statistics->avgF,statistics->minF);
  if (isBestDefined())
    fprintf(out, "Percentage of optima in pop. : %1.2f\n",((float)statistics->numOptimal/(float)statistics->N)*100);
  fprintf(out,"Population bias              : ");
  printGuidance(out,statistics->p1,statistics->n,statistics->guidanceThreshold);
  fprintf(out,"\n");
  fprintf(out,"Best solution in the pop.    : ");
  if (isBestDefined())
    printIndividual(out,statistics->bestX,statistics->n);
  else
    fprintf(out,"Statistic not available.");
  fprintf(out,"\n\nThe End.\n");

  return;
}

// ================================================================================
//
// name:          printModel
//
// function:      prints out the constructed model (called each generation)
//
// parameters:    out..........output stream
//                t............number of generation
//                G............the constructed network
//
// returns:       void
//
// ================================================================================

void statistics::printModel(FILE *out, long t, AcyclicOrientedGraph *G, FrequencyDecisionGraph **T)
{
  int i;

  // is output stream null?
  if (out==NULL)
    return;

  // print out the generation number

  fprintf(out,"--------------------------------------------------------\n");
  fprintf(out,"Generation: %3lu\n\n",t);

  // print out the model

  for (i=0; i<G->size(); i++)
    {
      fprintf(out,"%3u:\n",i);
      T[i]->print(out,5);
    };

  // get back

  return;
}

// ================================================================================
//
// name:          printGuidance
//
// function:      prints out a population bias (where each bit is biased) according
//                to a threshold ("0" if p1<treshold, "1" if p1>1-treshold, else 
//                ".")
//
// parameters:    out..........output stream
//                p1...........univariate frequencies for 1's
//                n............string length
//                threshold....threshold for bias (closer than that is biased)
//
// returns:       (int) 0
//
// ================================================================================

int printGuidance(FILE *out, float *p1, int n, float threshold)
{
  int k;
  float threshold1;
  
  // compute upper threshold

  threshold1 = 1-threshold;

  // print where the frequencies are biased

  for (k=0; k<n; k++)
    if (p1[k]<threshold)
      fprintf(out,"0");
    else
      if (p1[k]>threshold1)
	fprintf(out,"1");
      else
	fprintf(out,".");

  // get back

  return;
}
