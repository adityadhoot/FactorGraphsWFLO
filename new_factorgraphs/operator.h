#ifndef _operator_h_
#define _operator_h_

#include "frequencyDecisionGraph.h"
#include "labeledTreeNode.h"
#include "graph.h"

#define OPERATOR_NONE      -1
#define OPERATOR_SPLIT_NODE 0
#define OPERATOR_MERGE_NODE 1

struct Operator {
  float gain;
  int   type;
  int   where;
  int   label;
  FrequencyDecisionGraph *t;
  LabeledTreeNode *node;
  LabeledTreeNode *node2;
};

struct MergeOperator {
  float gain;
  LabeledTreeNode *a;
  LabeledTreeNode *b;
};

int resetOperator(Operator *x);
int updateBestOperator(Operator *best, Operator *x);
int updateBestNodeOperator(Operator *x, FrequencyDecisionGraph *t, MergeOperator *merge, int numMerges, int node, int n);
int applyOperator(Operator *x, AcyclicOrientedGraph *G);
int updateGainsAfterOperator(Operator *x, AcyclicOrientedGraph *G, int maxParents, long N);
int operatorApplicable(Operator *x, AcyclicOrientedGraph *G);
int deleteOperator(Operator *x);

#endif
