#ifndef _labeledTreeNode_h_
#define _labeledTreeNode_h_

#include <stdio.h>

// -------------------------------------
// well must polish this at some point

class LabeledTreeNode {

 public:

  int       which;

  int       depth;
  int       label;
  double    value[2];
  
  void *leavesItem;
  void *leafParentsItem;

  char *parentLabelCoincidenceVector;

  double *dArrayTmp;
  double *rightValue0Array;
  double *rightValue1Array;
  double *leftValue0Array;
  double *leftValue1Array;

  int             numParents;
  LabeledTreeNode **parent;

  LabeledTreeNode *left;
  LabeledTreeNode *right;

  LabeledTreeNode(int type);
  ~LabeledTreeNode();

  int allocateInstanceIndexEntries(int n);
  int reallocateInstanceIndexEntries(int n);
};

#endif
