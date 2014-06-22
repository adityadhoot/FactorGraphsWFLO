// ################################################################################
//
// name:          frequencyDecisionGraph.cc
//
// author:        Martin Pelikan
//
// purpose:       frequencyDecisionGraph extends the decisionGraph by putting
//                the frequencies into the leaves. It allows two modes, one for
//                plain frequencies and one for frequencies related to some
//                particular value of a particular variable.
//
// last modified: August 2000
//
// ################################################################################

#include <stdlib.h>
#include <string.h>

#include "decisionGraph.h"
#include "frequencyDecisionGraph.h"

#define SHIFT_STEP 4

// ================================================================================
//
// name:          FrequencyDecisionGraph::FrequencyDecisionGraph
//
// function:      constructor
//
// parameters:    p....population to bind  this frequency graph to (we must have
//                     the data set in advance, it's just better i guess)
//
// returns:       void
//
// ================================================================================

FrequencyDecisionGraph::FrequencyDecisionGraph(Population *p):DecisionGraph()
{
  // set the active population and disable the position of an active bit

  this->p          = p;
  this->myPosition = -1;

  // allocate the instance index stuff (we don't do this in traditional decision trees)

  getRoot()->allocateInstanceIndexEntries(p->n);

  // the probability in the root is 1 (no position associated with the tree)

  setValue(getRoot(),1,0);
}

// ================================================================================
//
// name:          FrequencyDecisionGraph::FrequencyDecisionGraph
//
// function:      yet another constructor
//
// parameters:    p............population to bind  this frequency graph to
//                myPosition...the position (variable) we want to bind this graph 
//                             with (we can do this and will use this mostly in 
//                             fact)
//
// returns:       void
//
// ================================================================================

FrequencyDecisionGraph::FrequencyDecisionGraph(Population *p, int myPosition):DecisionGraph()
{
  double f;

  // set the active population and the active bit

  this->p          = p;
  this->myPosition = myPosition;

  // allocate the instance index stuff (we don't do this in traditional decision trees)

  getRoot()->allocateInstanceIndexEntries(p->n);

  // the probability in the root is the univariate frequency in the position

  f = univariateFrequency(myPosition);
  setValue(getRoot(),1-f,0);
  setValue(getRoot(),f,1);
}

// ================================================================================
//
// name:          FrequencyDecisionGraph::merge
//
// function:      merges two leaves (not necessarily with the same parent)
//                ...in fact we really do just little additional work to what we do
//                in decision graphs without frequencies....
//
// parameters:    x.......a pointer to the first node
//                y.......a pointer to the second node
//
// returns:       void
//
// ================================================================================

int FrequencyDecisionGraph::merge(LabeledTreeNode *x, LabeledTreeNode *y)
{
  // increase the value in x by the value in y

  x->value[0] += y->value[0];
  if (myPosition>=0)
    x->value[1] += y->value[1];

  // merge the children of x now
 
  DecisionGraph::merge(x,y);
  
  // get back
  
  return 0;
}

// ================================================================================
//
// name:          FrequencyDecisionGraph::split
//
// function:      splits the node on a particular value (label)
//                (usually identifying a variable)
//                NOTE: WE DO NOT RECOMPUTE THE FREQUENCIES HERE!!
//
// parameters:    x.......a pointer to the node
//                label...which label (variable id or feature) to split on
//
// returns:       void
//
// ================================================================================

int FrequencyDecisionGraph::split(LabeledTreeNode *x, int label)
{
  // split the node x now

  DecisionGraph::split(x,label);

  // allocate instance/frequency related stuff for both children

  x->left->allocateInstanceIndexEntries(p->n);
  x->right->allocateInstanceIndexEntries(p->n);
 
  // it is sufficient to compute frequencies for one of the two new instances,
  // because the second one can be computed by using the value in x (which is
  // the sum of the two), so that's what we're gonna do

  // copy the instance information from the parent to the new children (if any)
  
  if (x->depth>0)
    {
      memcpy(x->left->parentLabelCoincidenceVector,x->parentLabelCoincidenceVector,p->n);
      memcpy(x->right->parentLabelCoincidenceVector,x->parentLabelCoincidenceVector,p->n);
    };
  
  // add the new information to each child
  
  x->left->parentLabelCoincidenceVector[x->label]  = 1;
  x->right->parentLabelCoincidenceVector[x->label] = 1;
  
  // should recompute frequencies?!!! (well i take care of this elsewhere
  // its faster - but careful about this....
  
  // get back
  
  return 0;
}

// ================================================================================
//
// name:          FrequencyDecisionGraph::univariateFrequency
//
// function:      computes a frequency of 1 on position k in the population
//                associated with this frequency decision graph
//
// parameters:    k.... a position we want to compute the frequency at
//
// returns:       (double) the requested frequency
//
// ================================================================================

double FrequencyDecisionGraph::univariateFrequency(int k)
{
  long i;
  long N;
  long count;

  // assign helper variables

  N = p->N;

  // compute the frequency of x_k=1

  count=0;
  for (i=0; i<N; i++)
    count += p->x[i][k];

  // return the resulting frequency

  return (double)count/(double)N;
}

// ================================================================================
//
// name:          FrequencyDecisionGraph::computeFrequencies
//
// function:      compute all frequencies encoded by the tree, it's just as fast
//                as if we wanted to recompute the stuff for just one leaf, as it
//                is implemented. we pay for being a graph not a tree here...
//
// parameters:    (none)
//
// returns:       (int) 0
//
// ================================================================================

int FrequencyDecisionGraph::computeFrequencies()
{
  long i;
  int  j;
  int numLeaves;
  long N;
  LabeledTreeNode *node;
  char *s;
  NodeListItem *leafItem;

  // set helper variables

  N         = p->N;
  numLeaves = getNumLeaves();

  // set the value in all leaves to 0

  leafItem = getLeaves();
  for (j=0; j<numLeaves; j++, leafItem=leafItem->next)
    leafItem->x->value[0]=leafItem->x->value[1]=0;
  
  // compute the new values

  for(i=0; i<N; i++)
    {
      s=p->x[i];
      node = getRoot();

      while (node->which!=LEAF)
	{
	  if (s[node->label]==0)
	    node = node->left;
	  else
	    node = node->right;
	};

      if (myPosition>=0)
	node->value[s[myPosition]]++;
      else
	node->value[0]++;
    };

  // divide the values in all leaves by N

  leafItem = getLeaves();
  if (myPosition>=0)
    for (j=0; j<numLeaves; j++, leafItem=leafItem->next)
      {
	leafItem->x->value[0]/=(double)N;
	leafItem->x->value[1]/=(double)N;
      }
  else
    for (j=0; j<numLeaves; j++, leafItem=leafItem->next)
      leafItem->x->value[0]/=(double)N;
    

  // get back
 
  return 0;
}

// ================================================================================
//
// name:          FrequencyDecisionGraph::computeSplitFrequencies
//
// function:      compute the frequencies we would get in the children of a given
//                node if we performed splits on all possible labels (features)
//
// parameters:    x....the active node
//                left0...value[0]'s for all left children, indexed by the split 
//                        label
//                left1...value[1]'s for all left children, indexed by the split 
//                        label
//                right0..value[0]'s for all left children, indexed by the split 
//                        label
//                right1..value[1]'s for all left children, indexed by the split 
//                        label
//                
// returns:       (int) 0
//
// ================================================================================

int FrequencyDecisionGraph::computeSplitFrequencies(LabeledTreeNode *x, double *left0, double *left1, double *right0, double *right1)
{
  long i;
  int  n;
  long N;
  LabeledTreeNode *node;
  char *s;
  int label;

  // set helper variables

  N         = p->N;
  n         = p->n;

  // reset the counts

  for (label=0; label<n; label++)
    left0[label] = left1[label] = right0[label] = right1[label] = 0;

  // compute the frequencies

  for(i=0; i<N; i++)
    {
      s=p->x[i];
      node = getRoot();

      while (node->which!=LEAF)
	{
	  if (s[node->label]==0)
	    node = node->left;
	  else
	    node = node->right;
	};

      if (node==x)
	if (myPosition>=0)
	  {
	    for (label=0; label<n; label++)
	      if (s[label]==0)
		if (s[myPosition])
		  left1[label]++;
		else
		  left0[label]++;
	      else
		if (s[myPosition])
		  right1[label]++;
		else
		  right0[label]++;
	  }
	else
	  {
	    for (label=0; label<n; label++)
	      if (s[label]==0)
		left0[label]++;
	      else
		right0[label]++;
	  };
    };

  // divide the values in all leaves by N

  if (myPosition>=0)
    for (label=0; label<n; label++)
      {
	left0[label]  /= (double)N;
	left1[label]  /= (double)N;
	right0[label] /= (double)N;
	right1[label] /= (double)N;
      }
  else
    for (label=0; label<n; label++)
      {
	left0[label]  /= (double)N;
	right0[label] /= (double)N;
      };

  // get back
 
  return 0;
}

// ================================================================================
//
// name:          FrequencyDecisionGraph::print
//
// function:      prints out the frequency decision graph into a file
//
// parameters:    out....the file
//                
// returns:       (int) 0
//
// ================================================================================

int FrequencyDecisionGraph::print(FILE *out)
{
  // call the recursive print function

  recursivePrint(out,getRoot(),0);

  // get back

  return 0;
}

// ================================================================================
//
// name:          FrequencyDecisionGraph::print
//
// function:      prints out the frequency decision graph into a file with a shift
//                to the right
//
// parameters:    out....the file
//                shift..initial shift
//                
// returns:       (int) 0
//
// ================================================================================

int FrequencyDecisionGraph::print(FILE *out, int shift)
{
  // call the recursive print function

  recursivePrint(out,getRoot(),shift);

  // get back

  return 0;
}

// ================================================================================
//
// name:          FrequencyDecisionGraph::recursivePrint
//
// function:      recursive version of the print
//
// parameters:    out....the file
//                x......root of the subtree we are to print
//                shift..initial shift
//                
// returns:       (int) 0
//
// ================================================================================

int FrequencyDecisionGraph::recursivePrint(FILE *out, LabeledTreeNode *x, int shift)
{
  int i;

  // if the tree is NULL, do nothing

  if (x==NULL)
    return 0;

  // print shift spaces

  for (i=0; i<shift; i++)
    fputc(' ',out);

  // for leaves, print the value, for splits, print the label

  if (x->which==LEAF)
    fprintf(out,"p(x%u=1|...)=%1.2f\n",myPosition,x->value[0]/(x->value[0]+x->value[1]));
  if (x->which==SPLIT)
    fprintf(out,"%i \n",x->label);

  // print the children sub-trees

  recursivePrint(out,x->left,shift+SHIFT_STEP);
  recursivePrint(out,x->right,shift+SHIFT_STEP);

  // get back

  return 0;
}
