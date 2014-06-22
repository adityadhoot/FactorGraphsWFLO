// ################################################################################
//
// name:          labeledTreeNode.cc
//
// author:        Martin Pelikan
//
// purpose:       class LabeledTreeNode to store and manipulate nodes of a labeled 
//                tree
//
// last modified: August 2000
//
// ################################################################################

#include <stdlib.h>

#include "labeledTreeNode.h"
#include "memalloc.h"

// ================================================================================
//
// name:          LabeledTreeNode::LabeledTreeNode
//
// function:      constructor
//
// parameters:    (none)
//
// returns:       void
//
// ================================================================================

LabeledTreeNode::LabeledTreeNode(int type)
{
  // assign the variables
  
  which=type; 
  
  left=NULL; 
  right=NULL; 
  
  label=-1; 
  value[0]=0; 
  value[1]=0;

  numParents=0;
  parent=NULL; 
  depth=0; 
  
  leavesItem=NULL;
  leafParentsItem=NULL;

  parentLabelCoincidenceVector=NULL;

  dArrayTmp = NULL;
  rightValue0Array = NULL;
  rightValue1Array = NULL;
  leftValue0Array  = NULL;
  leftValue1Array  = NULL;
}

// ================================================================================
//
// name:          LabeledTreeNode::~LabeledTreeNode
//
// function:      destructor
//
// parameters:    (none)
//
// returns:       void
//
// ================================================================================

LabeledTreeNode::~LabeledTreeNode()
{

  if (parentLabelCoincidenceVector)
    free(parentLabelCoincidenceVector);
  if (dArrayTmp)
    free(dArrayTmp);
  if (rightValue0Array)
    free(rightValue0Array);
  if (rightValue1Array)
    free(rightValue1Array);
  if (leftValue0Array)
    free(leftValue0Array);
  if (leftValue1Array)
    free(leftValue1Array);
}

// ================================================================================
//
// name:          LabeledTreeNode::~LabeledTreeNode
//
// function:      allocates a coincidence vector containing 1's for all ancestors
//                in a decision graph
//
// parameters:    n......the number of possible values (labels) of ancestors
//
// returns:       (int) 0
//
// ================================================================================

int LabeledTreeNode::allocateInstanceIndexEntries(int n)
{
  parentLabelCoincidenceVector = (char*) Calloc(n,sizeof(char));

  // get back

  return 0;
}
