#ifndef _DECISIONGRAPH_h_
#define _DECISIONGraph_h_

#include <stdio.h>

#include "labeledTreeNode.h"

#define LEAF  0
#define SPLIT 1

// -------------------------------------

struct NodeListItem {
  LabeledTreeNode *x;

  NodeListItem *previous;
  NodeListItem *next;
};

// -------------------------------------

class DecisionGraph {

 private:
  LabeledTreeNode *root;

  int              numLeaves;
  NodeListItem    *leaves;

  LabeledTreeNode *iterator;
  NodeListItem    *leafIterator;

  int deleteSubtree(LabeledTreeNode *x);
  int deleteNodeList(NodeListItem *x);

  int updateLeavesAfterSplit(LabeledTreeNode *x);
  int updateLeavesBeforeMerge(LabeledTreeNode *x, LabeledTreeNode *y);

 protected:
  LabeledTreeNode *newNode();
  int deleteNode(LabeledTreeNode *x);

 public:
  DecisionGraph();
  virtual ~DecisionGraph();

  // get the root

  LabeledTreeNode *getRoot();
  double           setValue(LabeledTreeNode *x, double value, char which);
  double           getValue(LabeledTreeNode *x, char which);

  // node operations

  int split(LabeledTreeNode *x, int label);
  int merge(LabeledTreeNode *x, LabeledTreeNode *y);

  // functions on the leaves

  int           getNumLeaves();
  NodeListItem *getLeaves();

  // iterator functions
  LabeledTreeNode *getIterator();
  double getIteratorValue(char which);
  int getIteratorLabel();
  int iteratorGoToRoot();
  int iteratorGoTo(LabeledTreeNode *x);
  int iteratorGoLeft();
  int iteratorGoRight();
  int iteratorFollowInstance(char *x);
  int iteratorFollowInstanceFromRoot(char *x);

  // leaf iterator functions
  NodeListItem *getLeafIterator();
  double getLeafIteratorValue(char which);
  LabeledTreeNode *getLeafIteratorNode();
  int resetLeafIterator();
  int leafIteratorNext();
  int leafIteratorPrevious();
};
  
#endif
