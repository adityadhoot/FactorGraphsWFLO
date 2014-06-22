#ifndef _frequencyDecisionGraph_h_
#define _frequencyDecisionGraph_h_

#include "decisionGraph.h"
#include "population.h"

// -------------------------------------

class FrequencyDecisionGraph: public DecisionGraph
{
 private:

  Population *p;
  int         myPosition;

  double univariateFrequency(int k);
  double instanceFrequency(int *index, char *x, int n);

  int recursivePrint(FILE *out, LabeledTreeNode *x, int shift);
  
 public:
  FrequencyDecisionGraph(Population *p);
  FrequencyDecisionGraph(Population *p, int myPosition);
  
  int merge(LabeledTreeNode *x, LabeledTreeNode *y);
  int split(LabeledTreeNode *x, int label);

  int computeFrequencies();
  int computeSplitFrequencies(LabeledTreeNode *x, double *left0, double *left1, double *right0, double *right1);

  // print out the graph, starting from the root

  int print(FILE *out);
  int print(FILE *out, int shift);
};

#endif
