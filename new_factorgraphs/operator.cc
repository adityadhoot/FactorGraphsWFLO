// ################################################################################
//
// name:          operator.cc      
//
// author:        Martin Pelikan
//
// purpose:       functions for handling operators on Bayesian networks with
//                decision graphs
//
// last modified: August 2000
//
// ################################################################################

#include <stdlib.h>
#include <string.h>

#include "bayesian.h"
#include "operator.h"
#include "memalloc.h"

// ================================================================================
//
// name:          resetOperator
//
// function:      resets the operator (pretty much set everything to null and give
//                a negative gain)
//
// parameters:    x...the operator to reset
//                
// returns:       (int) 0
//
// ================================================================================

int resetOperator(Operator *x)
{
  // reset the operator (empty and stuff)

  x->gain  = -1;
  x->type  = OPERATOR_NONE;
  x->where = -1;
  x->label = -1;
  x->node  = NULL;
  x->node2 = NULL;

  // get back

  return 0;
}

// ================================================================================
//
// name:          updateBestOperator
//
// function:      updates the best operator (if the new operator is better, set
//                the best operator to it
//
// parameters:    best...the best operator
//                x......the operator to use for an update
//                
// returns:       (int) 0
//
// ================================================================================

int updateBestOperator(Operator *best, Operator *x)
{
  // take the one with a better gain

  if (x->gain>best->gain)
    memcpy(best,x,sizeof(Operator));

  // get back
  
  return 0;
}

// ================================================================================
//
// name:          updateBestNodeOperator
//
// function:      updates the best operator for a node (just go through all
//                operators we can do and then pick the best one)
//
// parameters:    x..........the best operator for this variable
//                t..........the corresponding frequency graph
//                merge......merge operators for this variable
//                numMerge...number of merge operators for the variable
//                
// returns:       (int) 0
//
// ================================================================================

int updateBestNodeOperator(Operator *x, FrequencyDecisionGraph *t, MergeOperator *merge, int numMerges, int node, int n)
{
  int i,j;
  int numLeaves;
  NodeListItem *leafItem;

  //  printf("Updating best gain for node %u (%u + %u)\n",node,t->getNumLeaves(),numMerges);

  // assign some helper variables

  numLeaves = t->getNumLeaves();

  // reset the operator

  resetOperator(x);

  // try all splits, and update the best oeprator for this node

  for (i=0, leafItem=t->getLeaves(); (i<numLeaves); i++, leafItem=leafItem->next)
    for (j=0; j<n; j++)
      if (x->gain<leafItem->x->dArrayTmp[j])
	{
	  x->gain  = leafItem->x->dArrayTmp[j];
	  x->type  = OPERATOR_SPLIT_NODE;
	  x->where = node;
	  x->label = j;
	  x->node  = leafItem->x;
	  x->node2 = NULL;
	  x->t     = t;
	};

  // try all merges

  for (i=0; i<numMerges; i++)
    if (merge[i].gain>x->gain)
      {
	x->gain  = merge[i].gain;
	x->type  = OPERATOR_MERGE_NODE;
	x->where = node;
	x->label = 0;
	x->node  = merge[i].a;
	x->node2 = merge[i].b;
	x->t     = t;
      };

  // get back
  
  return 0;
}

// ================================================================================
//
// name:          applyOperator
//
// function:      applies the operator to a Bayesian network with decision graphs
//
// parameters:    x..........the operator
//                G..........the network
//                
// returns:       (int) 0
//
// ================================================================================

int applyOperator(Operator *x, AcyclicOrientedGraph *G)
{
  // apply the operator

  switch (x->type) {
  case OPERATOR_SPLIT_NODE: 
    x->t->split(x->node,x->label);
    x->node->left->dArrayTmp = (double*) Calloc(G->size(),sizeof(double));
    x->node->left->rightValue0Array = (double*) Calloc(G->size(),sizeof(double));
    x->node->left->rightValue1Array = (double*) Calloc(G->size(),sizeof(double));
    x->node->left->leftValue0Array = (double*) Calloc(G->size(),sizeof(double));
    x->node->left->leftValue1Array = (double*) Calloc(G->size(),sizeof(double));
    x->node->right->dArrayTmp = (double*) Calloc(G->size(),sizeof(double));
    x->node->right->rightValue0Array = (double*) Calloc(G->size(),sizeof(double));
    x->node->right->rightValue1Array = (double*) Calloc(G->size(),sizeof(double));
    x->node->right->leftValue0Array = (double*) Calloc(G->size(),sizeof(double));
    x->node->right->leftValue1Array = (double*) Calloc(G->size(),sizeof(double));
    x->node->left->value[0]=x->node->leftValue0Array[x->label];
    x->node->left->value[1]=x->node->leftValue1Array[x->label];
    x->node->right->value[0]=x->node->rightValue0Array[x->label];
    x->node->right->value[1]=x->node->rightValue1Array[x->label];
    ///!    if (x->node->depth+1>getMaxDepth())
    ///!      setMaxDepth(x->node->depth+1);
    G->addEdge(x->label,x->where);
    ///!numAppliedSplits++;
    break;

  case OPERATOR_MERGE_NODE:
    x->t->merge(x->node,x->node2);
    ///!numAppliedMerges++;
    break;

  default:
    printf("ERROR: Operator not defined (applyOperator)! Exiting!\n");
  };

  // get back

  return 0;
}

// ================================================================================
//
// name:          updateGainsAfterOperator
//
// function:      updates gains after applying an operator to the network
//
// parameters:    x............the operator
//                G............the network
//                maxParents...the bound on the number of parents
//                N............population size
//                
// returns:       (int) 0
//
// ================================================================================

int updateGainsAfterOperator(Operator *x, AcyclicOrientedGraph *G, int maxParents, long N)
{
   switch (x->type) {
   case OPERATOR_SPLIT_NODE: 
     recomputeDecisionGraphSplitGains(x->t,x->node->left,G,maxParents,x->where,G->size(),N);
     recomputeDecisionGraphSplitGains(x->t,x->node->right,G,maxParents,x->where,G->size(),N);
     break;

   case OPERATOR_MERGE_NODE:
     recomputeDecisionGraphSplitGains(x->t,x->node,G,maxParents,x->where,G->size(),N);
     break;


  default:
    printf("ERROR: Operator not defined (updateGainsAfterOperator)! Exiting!\n");
  };

  // get back

  return 0;
}

// ================================================================================
//
// name:          operatorApplicable
//
// function:      returns true if a given operator is applicable to the network
//                with decision graphs (this ignores the bound on the number of
//                parents) - that's considered elsewhere
//
// parameters:    x............the operator
//                G............the network
//                
// returns:       (int) 0
//
// ================================================================================

int operatorApplicable(Operator *x, AcyclicOrientedGraph *G)
{
  // we can split only when we can add a parent or it's already there

  if (x->type==OPERATOR_SPLIT_NODE)
    return ((G->connected(x->label,x->where))||(G->canAddEdge(x->label,x->where)));
  else
    if (x->type==OPERATOR_MERGE_NODE)
      return 1;

  // get back

  return 0;
}

// ================================================================================
//
// name:          deleteOperator
//
// function:      deletes an operator (just sets its gain to -1) - but this works
//                not on merges, but splits
//
// parameters:    x............the operator
//                
// returns:       (int) 0
//
// ================================================================================

int deleteOperator(Operator *x)
{
  // well, if it's the split, set the corresponding gain to -1

  if (x->type==OPERATOR_SPLIT_NODE)
    x->node->dArrayTmp[x->label]=-1;

  // get back
  
  return 0;
}
