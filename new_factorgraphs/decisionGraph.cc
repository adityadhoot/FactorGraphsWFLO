// ################################################################################
//
// name:          decisionGraph.cc
//
// author:        Martin Pelikan
//
// purpose:       decisionGraph class which allows creating a decision graph with
//                some information that can be stored in leaves.
//
// last modified: August 2000
//
// ################################################################################

#include <stdlib.h>
#include <string.h>

#include "decisionGraph.h"
#include "memalloc.h"
#include "random.h"

#define SHIFT_STEP 4

// ================================================================================
//
// name:          DecisionGraph::DecisionGraph
//
// function:      constructor
//
// parameters:    (none)
//
// returns:       void
//
// ================================================================================

DecisionGraph::DecisionGraph()
{
  // create the root node

  root = new LabeledTreeNode(LEAF);
  
  // one leaf at the moment (the root)

  numLeaves = 1;

  // add the root to the list of leaf nodes

  leaves           = new NodeListItem;
  leaves->x        = root;
  leaves->previous = leaves->next = NULL;

  // add pointer to the root's item in the leaf list to the root

  root->leavesItem = leaves;

  // reset the iterators

  iteratorGoToRoot();
  resetLeafIterator();
}

// ================================================================================
//
// name:          DecisionGraph::~DecisionGraph
//
// function:      destructor
//
// parameters:    (none)
//
// returns:       void
//
// ================================================================================

DecisionGraph::~DecisionGraph()
{
  deleteSubtree(root);
  deleteNodeList(leaves);
}

// ================================================================================
//
// name:          DecisionGraph::deleteSubtree
//
// function:      deletes the subtree from memory
//
// parameters:    x......the pointer to the root of the subtree (in fact,
//                       subgraph to delete
//
// returns:       (int) 0
//
// ================================================================================

int DecisionGraph::deleteSubtree(LabeledTreeNode *x)
{
  int i;

  // is the tree NULL?

  if (x==NULL)
    return 0;

  // delete the subtrees (careful about identical children)

  if (x->left!=x->right)
    deleteSubtree(x->left);
  deleteSubtree(x->right);

  // cut the node from the parents (not to be deleted multiple times)

  for (i=0; i<x->numParents; i++)
    {
      if (x->parent[i]->left==x)
	x->parent[i]->left=NULL;
      if (x->parent[i]->right==x)
	x->parent[i]->right=NULL;
    }

  if (x->parent)
    free(x->parent);

  // delete the node itself

  delete x;

  // get back

  return 0;
}

// ================================================================================
//
// name:          DecisionGraph::deleteNodeList
//
// function:      deletes the list of nodes
//
// parameters:    x......the pointer to the first node in the node list to delete
//
// returns:       (int) 0
//
// ================================================================================

int DecisionGraph::deleteNodeList(NodeListItem *x)
{
  if (x==NULL)
    return 0;

  deleteNodeList(x->next);
  delete x;

  return 0;
}

// ================================================================================
//
// name:          DecisionGraph::getRoot
//
// function:      returns the pointer to the root of the decision graph
//
// parameters:    x......the pointer to the first node in the node list to delete
//
// returns:       (LabeledTreeNode*) the pointer to the root
//
// ================================================================================

LabeledTreeNode *DecisionGraph::getRoot()
{
  // return the root

  return root;
}

// ================================================================================
//
// name:          DecisionGraph::setValue
//
// function:      sets a particular value assigned to a node
//
// parameters:    x.......a pointer to the node
//                value...the value to which to set the value of the node x
//                which...which of the values to set (there may be more than one)
//
// returns:       (double) the resulting value
//
// ================================================================================

double DecisionGraph::setValue(LabeledTreeNode *x, double value, char which)
{
  // set the value of a node and returns it

  return x->value[which]=value;
}

// ================================================================================
//
// name:          DecisionGraph::getValue
//
// function:      return a particular value assigned to the node
//
// parameters:    x.......a pointer to the node
//                which...which of the values to get (there may be more than one)
//
// returns:       (double) the requested value
//
// ================================================================================

double DecisionGraph::getValue(LabeledTreeNode *x, char which)
{
  // return the value of a node

  return x->value[which];
}

// ================================================================================
//
// name:          DecisionGraph::split
//
// function:      splits the node on a particular value (label)
//                (usually identifying a variable)
//
// parameters:    x.......a pointer to the node
//                label...which label (variable id or feature) to split on
//
// returns:       (int) 0
//
// ================================================================================

int DecisionGraph::split(LabeledTreeNode *x, int label)
{
  // assign the label and type

  x->label = label;
  x->which = SPLIT;

  // split the node

  x->left  = new LabeledTreeNode(LEAF);
  x->right = new LabeledTreeNode(LEAF);

  // allocate a parent for each of the children

  x->left->numParents=1;
  x->left->parent = (LabeledTreeNode**) malloc(sizeof(LabeledTreeNode*));

  x->right->numParents=1;
  x->right->parent = (LabeledTreeNode**) malloc(sizeof(LabeledTreeNode*));

  // assign the parent
  
  x->left->parent[0] = x->right->parent[0] = x;
  
  // assign the depths

  x->left->depth = x->right->depth = x->depth+1;
  
  // replace the split node by its children
  
  updateLeavesAfterSplit(x);

  // get back

  return 0;
}

// ================================================================================
//
// name:          DecisionGraph::merge
//
// function:      merges two leaves (not necessarily with the same parent)
//
// parameters:    x.......a pointer to the first node
//                y.......a pointer to the second node
//
// returns:       (int) 0
//
// ================================================================================

int DecisionGraph::merge(LabeledTreeNode *x, LabeledTreeNode *y)
{
  int i,j,k;
  int newParents;
  int found;

  // update the leaf list

  updateLeavesBeforeMerge(x,y);

  // redirect the y's parents to x instead of y

  for (i=0; i<y->numParents; i++)
    if (y->parent[i]->left==y)
      y->parent[i]->left=x;
    else
      y->parent[i]->right=x;
  
  // assign all parents from y to x (only those that are new to x)

  newParents=0;
  for (i=0; i<y->numParents; i++)
    {
      found=0;
      for (j=0; j<x->numParents; j++)
	if (x->parent[j]==y->parent[i])
	  found=1;

      if (!found)
	newParents++;
    };

  x->parent=(LabeledTreeNode**) realloc(x->parent,(x->numParents+newParents)*sizeof(LabeledTreeNode*));

  k=x->numParents;
  for (i=0; i<y->numParents; i++)
    {
      found=0;
      for (j=0; j<x->numParents; j++)
	if (x->parent[j]==y->parent[i])
	  found=1;

      if (!found)
	x->parent[k++] = y->parent[i];
    };

  x->numParents+=newParents;

//   printf("Mergin nodes x and y, adding %u new parents.\n",newParents);

  // get back

  return 0;
}

// ================================================================================
//
// name:          DecisionGraph::updateLeavesAfterSplit
//
// function:      update the leaves (and other stuff) after performing a split
//
// parameters:    x.......a pointer to the node the split was made on
//
// returns:       (int) 0
//
// ================================================================================

int DecisionGraph::updateLeavesAfterSplit(LabeledTreeNode *x)
{
  NodeListItem *item;
  NodeListItem *newItem;

  // where is the leaf located in the list?

  item = (NodeListItem*) x->leavesItem;

  // replace the original item for x by an item for its left child
  
  item->x = x->left;
  x->left->leavesItem = item;

  // create a new list item and insert it between item and item->next

  newItem = new NodeListItem;
 
  if (item->next)
    item->next->previous = newItem;     // redirect the right neighbour

  newItem->next     = item->next;       // redirect the new item
  newItem->previous = item;             // 
  item->next        = newItem;          // redirect the left neighbour

  // new item points to the right child

  newItem->x = x->right;
  x->right->leavesItem = newItem;

  // increase the number of leaves by 1 (we replaced one by two)

  numLeaves++;

  // x's got no items in the list anymore

  x->leavesItem = NULL;

  // get back

  return 0;
}

// ================================================================================
//
// name:          DecisionGraph::updateLeavesBeforeMerge
//
// function:      update the leaves (and other stuff) before performing a merge
//                (we must do it before, because after doing it we will not have
//                 enough information to do this really)
//
// parameters:    x.......a pointer to the first node we want to merge
//                y.......a pointer to the second node we want to merge
//
// returns:       (int) 0
//
// ================================================================================

int DecisionGraph::updateLeavesBeforeMerge(LabeledTreeNode *x, LabeledTreeNode *y)
{
  NodeListItem *xItem, *yItem;

  // assign x to the list item of the left child

  xItem = (NodeListItem*) x->leavesItem;
  yItem = (NodeListItem*) y->leavesItem;

  x->leavesItem = xItem;
  xItem->x = x;
  
  // delete the item for the right child

  if (yItem->previous)
    yItem->previous->next=yItem->next;

  if (yItem->next)
    yItem->next->previous=yItem->previous;

  if (yItem==leaves)
    leaves=yItem->next;

  Free(yItem);

  // decrement the number of leaves

  numLeaves--;

  // get back

  return 0;
}

// ================================================================================
//
// name:          DecisionGraph::getNumLeaves
//
// function:      returns the number of leaves in the decision graph
//
// parameters:    (none)
//
// returns:       (int) the number of leaves
//
// ================================================================================

int DecisionGraph::getNumLeaves()
{
  // return the number of leaves

  return numLeaves;
}

// ================================================================================
//
// name:          DecisionGraph::getLeaves
//
// function:      returns the pointer to the list of leaves
//
// parameters:    (none)
//
// returns:       (int) the requested pointer to the list of leaves
//
// ================================================================================

NodeListItem *DecisionGraph::getLeaves()
{
  // return the list of leaves

  return leaves;
}

// ================================================================================
//
// name:          DecisionGraph::getIterator
//
// function:      returns the iterator pointer to the node
//
// parameters:    (none)
//
// returns:       (int) the pointer to a node where the iterator points
//
// ================================================================================

LabeledTreeNode *DecisionGraph::getIterator()
{
  // returns the iterator

  return iterator;
}

// ================================================================================
//
// name:          DecisionGraph::getIterator
//
// function:      returns the value sitting in the node where the iterator points
//
// parameters:    which....which of the values to get (there might be more)
//
// returns:       (int) the requested value
//
// ================================================================================

double DecisionGraph::getIteratorValue(char which)
{
  // returns the iterator

  return iterator->value[which];
}

// ================================================================================
//
// name:          DecisionGraph::getIteratorLabel
//
// function:      returns the label sitting in the node where the iterator points
//                (good for the splits)
//
// parameters:    (none)
//
// returns:       (int) the requested label
//
// ================================================================================

int DecisionGraph::getIteratorLabel()
{
  // returns the iterator

  return iterator->label;
}

// ================================================================================
//
// name:          DecisionGraph::iteratorGoToRoot
//
// function:      moves the iterator to the root
//
// parameters:    (none)
//
// returns:       (int) 0
//
// ================================================================================

int DecisionGraph::iteratorGoToRoot()
{
  // move the iterator to the root

  iteratorGoTo(root);

  // get back

  return 0;
}

// ================================================================================
//
// name:          DecisionGraph::iteratorGoTo
//
// function:      moves the iterator to a particular node
//
// parameters:    (none)
//
// returns:       (int) 0
//
// ================================================================================

int DecisionGraph::iteratorGoTo(LabeledTreeNode *x)
{
  // move the iterator to the node x

  iterator = x;

  // get back

  return 0;
}

// ================================================================================
//
// name:          DecisionGraph::iteratorGoToLeft
//
// function:      moves the iterator to the left child (from where he currently is)
//
// parameters:    (none)
//
// returns:       (int) 0
//
// ================================================================================

int DecisionGraph::iteratorGoLeft()
{
  // move the iterator to the left child, if possible

  iterator = iterator->left;

  // get back

  return 0;
}

// ================================================================================
//
// name:          DecisionGraph::iteratorGoToRight
//
// function:      moves the iterator to the right child (from where he currently 
//                is)
//
// parameters:    (none)
//
// returns:       (int) 0
//
// ================================================================================

int DecisionGraph::iteratorGoRight()
{
  // move the iterator to the right child, if possible

  iterator = iterator->right;

  // get back

  return 0;
}

// ================================================================================
//
// name:          DecisionGraph::iteratorFollowInstance
//
// function:      moves the iterator to the right leaf which is consistent with
//                a given instance (starting in the node the iterator points at
//                right now)
//
// parameters:    x.....the instance we want to traverse the graph with
//
// returns:       (int) 0
//
// ================================================================================

int DecisionGraph::iteratorFollowInstance(char *x)
{
  while (iterator->which!=LEAF)
    {
      if (x[iterator->label]==0)
	iteratorGoLeft();
      else
	iteratorGoRight();
    };

  return 0;
}

// ================================================================================
//
// name:          DecisionGraph::iteratorFollowInstanceFromRoot
//
// function:      moves the iterator to the right leaf which is consistent with
//                a given instance, starting in the root
//
// parameters:    x.....the instance we want to traverse the graph with
//
// returns:       (int) 0
//
// ================================================================================

int DecisionGraph::iteratorFollowInstanceFromRoot(char *x)
{
  iterator=root;

  while (iterator->which!=LEAF)
    {
      if (x[iterator->label]==0)
	iteratorGoLeft();
      else
	iteratorGoRight();
    };

  return 0;
}

// ================================================================================
//
// name:          DecisionGraph::getLeafIterator
//
// function:      returns the pointer to the node node where the leaf iterator 
//                points at
//
// parameters:    (none)
//
// returns:       (int) 0
//
// ================================================================================

NodeListItem *DecisionGraph::getLeafIterator()
{
  // returns the leaf iterator

  return leafIterator;
}

// ================================================================================
//
// name:          DecisionGraph::resetLeafIterator
//
// function:      directs the leaf iterator to the first leaf in the list
//                (note that the order is whatever comes out as we are performing
//                 various operators on the graph, it can be very strange)
//
// parameters:    (none)
//
// returns:       (int) 0
//
// ================================================================================

int DecisionGraph::resetLeafIterator()
{
  // set the leaf iterator to the first leaf

  leafIterator = leaves;

  // get back

  return 0;
}

// ================================================================================
//
// name:          DecisionGraph::leafIteratorNext
//
// function:      move the leaf iterator to the next leaf
//
// parameters:    (none)
//
// returns:       (int) 0
//
// ================================================================================

int DecisionGraph::leafIteratorNext()
{
  // can't do anything with NULL, error

  if (leafIterator==NULL)
    return -1;

  // move the leaf iterator to the next leaf

  leafIterator = leafIterator->next;
  
  // get back
  
  return 0;
}

// ================================================================================
//
// name:          DecisionGraph::leafIteratorPrevious
//
// function:      move the leaf iterator to the previous leaf
//
// parameters:    (none)
//
// returns:       (int) 0
//
// ================================================================================

int DecisionGraph::leafIteratorPrevious()
{
  // can't do anything with NULL, error

  if (leafIterator==NULL)
    return -1;

  // move the leaf iterator to the next leaf

  leafIterator = leafIterator->previous;
  
  // get back
  
  return 0;
}

// ================================================================================
//
// name:          DecisionGraph::getLeafIteratorValue
//
// function:      returns the value sitting on the leaf where the leaf iterator is
//                pointing at
//
// parameters:    which....which of the values we want (there may be more)
//
// returns:       (double) the requested value
//
// ================================================================================

double DecisionGraph::getLeafIteratorValue(char which)
{
  // return the value of the leaf corresponding to a current value of the leaf iterator

  return leafIterator->x->value[which];
}

// ================================================================================
//
// name:          DecisionGraph::getLeafIteratorNode
//
// function:      returns the (leaf) node where the leaf iterator is pointing at
//
// parameters:    (none)
//
// returns:       (double) the requested leaf node
//
// ================================================================================

LabeledTreeNode *DecisionGraph::getLeafIteratorNode()
{
  // return the leaf corresponding to a current value of the leaf iterator

  return leafIterator->x;
}
