// ################################################################################
//
// name:          bayesian.cc      
//
// author:        Martin Pelikan
//
// purpose:       functions for construction and use of bayesian networks 
//                (not including the metric related functions and some of more
//                specific functions defined elsewhere)
//
// last modified: August 2000
//
// ################################################################################

#include <string.h>

#include "bayesian.h"
#include "boa.h"
#include "population.h"
#include "graph.h"
#include "memalloc.h"
#include "utils.h"
#include "random.h"
#include "operator.h"
#include "mymath.h"

#define FIXED_THRESHOLD 0.000
// ================================================================================
//
// name:          constructTheFactorGraphNetwork
//
// function:      constructs a factor graph given the population (including the decision
//                graphs containing needed frequencies and the network itself
//                needed in fact only for topological ordering of the variables. 
//								Simply converts the Bayesian Network into a Factor Graph
//
// parameters:    G........the Bayesian Network (input)
//                T........the decision graphs for each variable (input)
//                P........the population
//                params...parameters of the BOA
//								F........the resulting Factor Graph (output)
//
// returns:       (int) 0
//
// ================================================================================

/*int constructTheFactorGraph(AcyclicOrientedGraph *G,
			FrequencyDecisionGraph **T,
			Population *P,
			BoaParams *params)
{


}*/
// ================================================================================
//
// name:          constructTheNetwork
//
// function:      constructs a network given the population (including the decision
//                graphs containing needed frequencies and the network itself
//                needed in fact only for topological ordering of the variables
//
// parameters:    G........the resulting network (output)
//                T........the decision graphs for each variable (output)
//                P........the population
//                params...parameters of the BOA
//
// returns:       (int) 0
//
// ================================================================================

int constructTheNetwork(AcyclicOrientedGraph *G,
			FrequencyDecisionGraph **T,
			Population *P,
			BoaParams *params)
{
  int    i,j;
  int    n;
  long   N;
  int    maxIncoming;
  MergeOperator **merge;
  int           *numMerges;

  char **canAdd;

  long     operatorsLeft;
  Operator bestOperator;
  Operator *bestNodeOperator;

  // no operators applied

  ///!  numAppliedSplits=numAppliedMerges=0;
  ///!  maxDepth=0;

  // set the helper variables

  N           = P->N;
  n           = P->n;
  maxIncoming = params->maxIncoming;

  // allocate all needed parameters

  canAdd = (char**) Calloc(n,sizeof(char*));
  for (i=0; i<n; i++)
    canAdd[i] = (char*) malloc(n);

  // allocate the operators for each node

  bestNodeOperator = (Operator*) Calloc(n,sizeof(Operator));
  for (i=0; i<n; i++)
    resetOperator(&(bestNodeOperator[i]));

  // we'll allocate some memory for the info on merge operators

  numMerges = (int*) Calloc(n,sizeof(int));
  merge = (MergeOperator**) Calloc(n,sizeof(MergeOperator*));
  for (i=0; i<n; i++)
    {
      merge[i]=NULL;
      numMerges[i]=0;
    };

  // remove all edges (just in case...)

  G->removeAllEdges();

  // ---- reset all needed parameters

  // all edges can be added (unless we are UMDA which we are not)
  
  for (i=0; i<n; i++)
    for (j=0; j<n; j++)
      canAdd[i][j]=YES;

  // no best operator at the moment

  resetOperator(&bestOperator);

  // allocate and recompute the gains (not to worry about freeing them, it is done automatically)

  for (i=0; i<n; i++)
    {
      T[i]->getLeaves()->x->dArrayTmp=(double*) Calloc(n,sizeof(double));
      T[i]->getLeaves()->x->rightValue0Array = (double*) Calloc(G->size(),sizeof(double));
      T[i]->getLeaves()->x->rightValue1Array = (double*) Calloc(G->size(),sizeof(double));
      T[i]->getLeaves()->x->leftValue0Array = (double*) Calloc(G->size(),sizeof(double));
      T[i]->getLeaves()->x->leftValue1Array = (double*) Calloc(G->size(),sizeof(double));
      recomputeDecisionGraphSplitGains(T[i],T[i]->getLeaves()->x,G,maxIncoming,i,n,N);
      updateBestNodeOperator(&(bestNodeOperator[i]),T[i],merge[i],numMerges[i],i,n);
      updateBestOperator(&bestOperator,&(bestNodeOperator[i]));
    };

  // all operators are left now, we can split in each node once on any variable

  operatorsLeft = n*n;

  // while we gained and can do some more do splits

  while ((bestOperator.gain>0)&&(operatorsLeft>0))
    {

      if (operatorApplicable(&bestOperator,G))
	{
	  // apply the operator

	  applyOperator(&bestOperator,G);
	 
	  // update the gains
 
	  updateGainsAfterOperator(&bestOperator,G,maxIncoming,N);

	  // merge will be processed separately (if used at all)

	  if (params->allowMerge)
	    recomputeDecisionGraphMergeGains(T[bestOperator.where],
					     merge,
					     &(numMerges[bestOperator.where]),
					     bestOperator.where,
					     N);
	}
      else
	// delete the operator (we can't apply it for some reason)

	deleteOperator(&bestOperator);

      // update the best operator in the node we've touched

      updateBestNodeOperator(&(bestNodeOperator[bestOperator.where]),
			     T[bestOperator.where],
			     merge[bestOperator.where],
			     numMerges[bestOperator.where],
			     bestOperator.where,
			     n);

      // everything alright, now we must get the best gain again
      
      resetOperator(&bestOperator);
      
      for (i=0; i<n; i++)
	updateBestOperator(&bestOperator,&(bestNodeOperator[i]));
    };

  // free it all

  for (i=0; i<n; i++)
    free(canAdd[i]);
  free(canAdd);

  free(bestNodeOperator);

  free(numMerges);
  for (i=0; i<n; i++)
    if (merge[i]!=NULL)
      free(merge[i]);
  free(merge);

  // get back

  return 0;
}

// ================================================================================
//
// name:          generateNewInstances
//
// function:      generates new instances given the network and the decision graphs
//
// parameters:    G........the network
//                T........the decision graphs for each variable
//                P........the population to store the new instances in
//                params...parameters of the BOA
//
// returns:       (int) 0
//
// ================================================================================

int generateNewInstances(AcyclicOrientedGraph *G,
			 FrequencyDecisionGraph **T,
			 Population *P,
			BoaParams *params)
{
  long N;
  int  n;
  int *index;
  
  // assign the helper variables
  
  N = P->N;
  n = P->n;

  // allocate index for topological ordering

  index = (int*) Calloc(n,sizeof(int));

  // order the vertices topologically

  topologicalOrdering(G,index);

  // generate the new stuff

  for (long i=0; i<N; i++)
    generateInstance(P->x[i],G,index,T);

  // free memory

  free(index);

  // get back

  return 0;
}

// ================================================================================
//
// name:          generateNewInstance
//
// function:      generates a new instance given the network and the decision 
//                graphs
//
// parameters:    x........the instance (output)
//                G........the network
//                index....index containing the topological ordering of variables
//                T........the decision graphs for each variable
//
// returns:       (int) 0
//
// ================================================================================

int generateInstance(char *x,
		     AcyclicOrientedGraph *G,
		     int *index,
		     FrequencyDecisionGraph **T)
{
    int i;
  int n;
  int position;
  int value;
  double p0,p1;

  // assign the helper variables

  n = G->size();

  // generate the stuff

  for (i=0; i<n; i++)
    {
      position = index[i];
      T[position]->iteratorFollowInstanceFromRoot(x);
      
      p0 = T[position]->getIterator()->value[0];
      p1 = T[position]->getIterator()->value[1];
      p0 = p0/(p0+p1);
      p1 = 1-p0;

      value=(drand()<p1)? 1:0;

      x[position]=value;
    }

  // get back

  return 0;
}

// ================================================================================
//
// name:          topologicalOrdering
//
// function:      orders the variables topologically (so that we get all the 
//                stuff right before we generate the new instances)
//
// parameters:    G........the network
//                index....index containing the topological ordering of variables
//                         (output)
//
// returns:       (int) 0
//
// ================================================================================

int topologicalOrdering(AcyclicOrientedGraph *G,
			int *index)
{
  int i,j;
  int n;
  int numAdded;
  char *added;
  int numParents;
  int  *parentList;
  char canAdd;

  // assign the helper variables

  n = G->size();

  // allocate memory for the added array

  added = (char*) malloc(n);
  for (i=0; i<n; i++)
    added[i]=0;

  // order

  numAdded=0;
  while (numAdded<n)
    for (i=0; i<n; i++)
      if (!added[i])
	{
	  canAdd=1;
	  parentList=G->getParentList(i);
	  numParents=G->getNumIn(i);
	  for (j=0; (j<numParents)&&(canAdd); j++)
	    {
	      if (!added[parentList[j]])
		canAdd=0;
	    };
	  
	  if (canAdd)
	    {
	      index[numAdded]=i;
	      numAdded++;
	      added[i]=1;
	    };
	};
	
  // free memory

  free(added);

  // get back

  return 0;
}

// ================================================================================
//
// name:          recomputeDecisionGraphSplitGains
//
// function:      computes the gains of splits on a particular node of a decision
//                graph containing the frequencies we need
//
// parameters:    t............the decision graph where the node is located
//                x............the node we want to compute the gains for
//                G............the network
//                maxParents...maximum number of parents of a node
//                node.........which node this tree is for
//                n............number of variables
//                N............population size
//
// returns:       (int) 0
//
// ================================================================================

int recomputeDecisionGraphSplitGains(FrequencyDecisionGraph *t,
				     LabeledTreeNode *x, 
				     AcyclicOrientedGraph *G,
				     int maxParents,
				     int node,
				     int n,
				     long N)
{
  int    label;
  double scoreBefore;
  double scoreAfter;
  double gain;
  LabeledTreeNode *dummy;
  double *leftValue0,*leftValue1;
  double *rightValue0,*rightValue1;
  double leftContribution;
  double rightContribution;

  // if this bit fixed now, ignore it

  if ((x->value[0]<=FIXED_THRESHOLD)||(x->value[1]<=FIXED_THRESHOLD))
    {
      for (label=0; label<n; label++)
	x->dArrayTmp[label]=-1;
      return 0;
    };

  // depth is too big - we can't split

  if (x->depth>=maxParents)
    {
      for (label=0; label<n; label++)
	x->dArrayTmp[label]=-1;
      return 0;
    };

  // compute basic contribution of this node (before split)
	
	scoreBefore = nodeContribution(x,N);

  // allocate memory for the frequencies of the splits

  leftValue0  = (double*) Calloc(n,sizeof(double));
  leftValue1  = (double*) Calloc(n,sizeof(double));
  rightValue0 = (double*) Calloc(n,sizeof(double));
  rightValue1 = (double*) Calloc(n,sizeof(double));

  // try all splits we can do (must not be our parent, and must not
  // create a cycle)

  t->computeSplitFrequencies(x,leftValue0,leftValue1,rightValue0,rightValue1);

  // we'll need this dummy node

  dummy = new LabeledTreeNode(LEAF);

  for (label=0; label<n; label++)
    {
      if ((node!=label)&&
      	  (x->parentLabelCoincidenceVector[label]==0)&&
      	  ((G->connected(label,node))||
	   ((G->getNumIn(node)<maxParents)&&(G->canAddEdge(label,node)))))
      {
	// compute frequencies if we did split
	
	dummy->value[0] = leftValue0[label];
	dummy->value[1] = leftValue1[label];
	leftContribution = nodeContribution(dummy,N);

	dummy->value[0] = rightValue0[label];
	dummy->value[1] = rightValue1[label];
	rightContribution = nodeContribution(dummy,N);

	// new contribution
	
	scoreAfter = leftContribution+rightContribution;
	
	// compute the gain

	gain = scoreAfter-scoreBefore-log2(double(N))/2;

	// update the contribution

	x->dArrayTmp[label]=gain;

	// update the values we computed (for future reference)

	x->rightValue0Array[label]=rightValue0[label];
	x->rightValue1Array[label]=rightValue1[label];
	x->leftValue0Array[label]=leftValue0[label];
	x->leftValue1Array[label]=leftValue1[label];
  }
    else
			x->dArrayTmp[label]= -1;
  }
  
  // delete the dummy node
  delete dummy;

  // free the memory used by the frequencies of the splits
  free(leftValue0);
  free(leftValue1);
  free(rightValue0);
  free(rightValue1);
  
  // get back
  
  return 0;
}

// ================================================================================
//
// name:          recomputeDecisionGraphMergeGains
//
// function:      computes the gains of merges on the decision graph corresponding
//                to a given node
//
// parameters:    t............the decision graph we want to compute the merge
//                             gains for
//                G............the network
//                merge........how the merge gains and operators really look like
//                             (output)
//                numMerges....how many of those merge operators we did find
//                n............number of variables
//                N............population size
//
// returns:       (int) 0
//
// ================================================================================

int recomputeDecisionGraphMergeGains(FrequencyDecisionGraph *t,
				     MergeOperator **merge,
				     int *numMerges,
				     int node,
				     long N)
{
  double scoreBefore;
  double scoreAfter;
  double gain;
  LabeledTreeNode *dummy;
  NodeListItem *a, *b;
  int numLeaves;

  // allocate a dummy node 

  dummy = new LabeledTreeNode(LEAF);

  // assign helper variables

  numLeaves = t->getNumLeaves();

  // allocate the merge operator array

  if (merge[node]!=NULL)
    free(merge[node]);
  
  merge[node]   = (MergeOperator*) Calloc(numLeaves*(numLeaves+1)/2,sizeof(MergeOperator));
  *numMerges    = 0;

  // compute merge gains for all pair of nodes
 
  for (a=t->getLeaves(); a->next!=NULL; a=a->next)
    for (b=a->next; b!=NULL; b=b->next)
      {
	// compute the score before the merge

	scoreBefore = nodeContribution(a->x,N)+nodeContribution(b->x,N);

	// update the dummy node (its frequencies are equal the sum of the frequencies 
	//                        of the two merged nodes)

	dummy->value[0] = a->x->value[0]+b->x->value[0];
	dummy->value[1] = a->x->value[1]+b->x->value[1];

	// compute the score after the merge (without really having to do the merge)

	scoreAfter = nodeContribution(dummy,N);

	// compute the gain

	gain = scoreAfter-scoreBefore+log2(double(N))/2;

	if (gain>0)
	  {
	    // compute the score after the merge
	    
	    merge[node][*numMerges].gain = gain;
	    merge[node][*numMerges].a    = a->x;
	    merge[node][*numMerges].b    = b->x;
	    
	    (*numMerges)++;
	  }
      }

  // delete the dummy node

  delete dummy;

  // get back

  return 0;
}

// ================================================================================
//
// name:          logGamma
//
// function:      computes a log of the Gamma function (defined as a factorial
//                of the input decreased by 1)
//
// parameters:    x....input parameter
//
// returns:       (double) log of the Gamma(x)
//
// ================================================================================

double logGamma(long x)
{
  if (x==1)
    return 0;
  else
    return getPrecomputedCummulativeLog(1,x-1);
}

// ================================================================================
//
// name:          nodeContribution
//
// function:      computes a contribution of a particular node to the log 
//                likelihood of the overall network
//
// parameters:    x....the node
//                N....the population size
//
// returns:       (double) the requested contribution
//
// ================================================================================

double nodeContribution(LabeledTreeNode *x, long N)
{
  double p_x_0;
  double p_x_1;
  long   m_x_0;
  long   m_x_1;
  double p_p;
  long   m_p;
  double score;

  // compute the probability and frequency of the node x 

  p_x_0 = x->value[0];
  m_x_0 = (long) ((double)p_x_0*(double)N);
  p_x_1 = x->value[1];
  m_x_1 = (long) ((double)p_x_1*(double)N);
  
  // is this node reachable? if not get the hell out of here

  if ((p_x_0==0)&&(p_x_1==0))
    return -10E10;

  // alright, let's go for it, compute the probability and frequency of the parents

  p_p = p_x_0 + p_x_1;
  m_p = (long) ((double)p_p*(double)N);

  // compute basic contribution of this node 

  score = -logGamma(2+m_p) + logGamma(1+m_x_0) + logGamma(1+m_x_1);
  
  // return the score

  return score;
}

// ================================================================================
//
// name:          initializeMetric
//
// function:      just to initialize what we need before we call the network 
//                construction, in fact only the cummulative logs
//
// parameters:    params...parameters of the BOA
//
// returns:       (int) 0
//
// ================================================================================

int initializeMetric(BoaParams *params)
{
  // we need to precompute some logarithms

  precomputeCummulativeLogarithms(params->N+3);

  // get back

  return 0;
}

// ================================================================================
//
// name:          doneMetric
//
// function:      do whatever needed to free memory and stuff, whatever we did
//                in initializeMetric
//
// parameters:    (none)
//
// returns:       (int) 0
//
// ================================================================================

int doneMetric()
{
  // done with the logarithms

  freePrecomputedCummulativeLogarithms();

  // get back

  return 0;
}
