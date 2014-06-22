// ################################################################################
//
// name:          graph.cc      
//
// author:        Martin Pelikan
//
// purpose:       the definition of classes OrientedGraph and AcyclicOrientedGraph
//                for manipulation with oriented graphs
//
// last modified: August 2000
//
// ################################################################################

#include <stdlib.h>
#include <stdio.h>

#include "graph.h"
#include "memalloc.h"
#include "stack.h"
#include "utils.h"

//==========================================================
//==========================================================
//==========================================================

OrientedGraph::OrientedGraph(int n)
{
  int i;

  // set the number of vertices

  N=n;
  N2=N*N;

  // allocate memory for coincidence, path, and parentList matrices

  coincidence = (char**) Calloc(N,sizeof(char*));
  path = (char**) Calloc(N,sizeof(char*));
  parentList = (int**) Calloc(N,sizeof(int*));

  for (i=0; i<N; i++)
    {
      coincidence[i] = (char*) Calloc(N,sizeof(char));
      path[i]        = (char*) Calloc(N,sizeof(char));
      parentList[i]  = (int*) Calloc(N,sizeof(int));
    }

  // allocate memory for the number of incoming and outcoming edges

  numIn = (int*) Calloc(N,sizeof(int));
  numOut = (int*) Calloc(N,sizeof(int));

  // allocate memory for vertex-marks

  mark = (int*) Calloc(n,sizeof(int));

  // set this graph to an empy graph

  removeAllEdges();

  // unmark all vertices

  removeAllMarks();
}

//==========================================================

OrientedGraph::~OrientedGraph()
{
  int i;

  // free the memory used by coincidence, path, and parentList matrices

  for (i=0; i<N; i++)
    {
      Free(coincidence[i]);
      Free(path[i]);
      Free(parentList[i]);
    }

  Free(coincidence);
  Free(path);
  Free(parentList);

  // free the memory used by numInt and numOut arrays

  Free(numIn);
  Free(numOut);

  // free the memory used by vertex-marks

  Free(mark);
}

//==========================================================

int OrientedGraph::size()
{
  return N;
}

//==========================================================

int OrientedGraph::addEdge(int i, int j)
{
  register int k,l;

  // is the edge already present?

  if (coincidence[i][j])
     return SUCCESS;

  // connect the vertices and update the list of parents

  coincidence[i][j]=CONNECTED;
  parentList[j][numIn[j]]=i;
  numOut[i]++;
  numIn[j]++;
  path[i][j]=CONNECTED;

  // update the path matrix

  for (k=0; k<N; k++)
  if (path[k][i])
     for (l=0; l<N; l++)
         if ((l!=k)&&(path[j][l]))
            path[k][l]=CONNECTED;

  return SUCCESS;
}

//==========================================================

int OrientedGraph::removeEdge(int i, int j)
{
  int k;
  register int l,m;
  char added;
  IntStack *from,*to,*from2,*to2;

  // is the edge present?

  if (!coincidence[i][j])
     return SUCCESS;

  // initialize stacks (needed later)

  from  = new IntStack((int)N2);
  to    = new IntStack((int)N2);
  from2 = new IntStack((int)N2);
  to2   = new IntStack((int)N2);

  // remove the edge

  coincidence[i][j]=NOT_CONNECTED;
  numOut[i]--;
  numIn[j]--;

  // update the list of parents

  for (k=0; k<=numIn[j]; k++)
    if (parentList[j][k]==i)
      if (k<numIn[j])
	parentList[j][k]=parentList[j][numIn[j]];

  // update the path matrix

  // a) remove all paths that could possibly be wrong

  for (k=0; k<N; k++)
    if (path[k][i])
      for (l=0; l<N; l++)
	if ((l!=k)&&(!coincidence[k][l])&&(path[j][l]))
	  {
	    path[k][l]=0;
	    from->push(k);
	    to->push(l);
	  }

  // b) add all paths that were potentially removed incorrectly

  do
    {
      added=0;

      while (from->notEmpty())
	{
	  k=from->pop();
	  l=to->pop();
	  
	  for (m=0; m<N; m++)
	    if ((path[k][m])&&(path[m][l]))
	      {
		path[k][l]=CONNECTED;
		added=1;
	      }

	  if (!path[k][l])
	    {
	      from2->push(k);
	      to2->push(l);
	    }
	}

      // swap stacks

      swapPointers((void**) &from, (void**) &from2);
      swapPointers((void**) &to, (void**) &to2);

    } while (added);

  // free used stacks

  delete from;
  delete to;
  delete from2;
  delete to2;

  // get back

  return SUCCESS;
}

//==========================================================

int OrientedGraph::reverseEdge(int i, int j)
{
  if (canReverseEdge(i,j))
    {
      removeEdge(i,j);
      return addEdge(j,i);
    }
  else
    return FAIL;
}

//==========================================================

int OrientedGraph::canReverseEdge(int i, int j)
{
  return connected(i,j);
}

//==========================================================

int OrientedGraph::removeAllEdges()
{
  for (int i=0; i<N; i++)
  {
    path[i][i]=CONNECTED;
    numIn[i]=0;
    numOut[i]=0;
    for (int j=0; j<i; j++)
      coincidence[i][j]=coincidence[j][i]=path[i][j]=path[j][i]=NOT_CONNECTED;
  }

  return SUCCESS;
}

//==========================================================

int OrientedGraph::setMark(int i, int val)
{
  mark[i]=val;
  return SUCCESS;
}

//==========================================================

int OrientedGraph::removeMark(int i)
{
  mark[i]=0;
  return SUCCESS;
}

//==========================================================

int OrientedGraph::removeAllMarks()
{
  register int i;

  for (i=0; i<N; i++)
    mark[i]=0;

  return SUCCESS;
}

//==========================================================

int OrientedGraph::setAllMarks(int val)
{
  register int i;
  
  for (i=0; i<N; i++)
    mark[i]=val;

  return SUCCESS;
}

//==========================================================

int OrientedGraph::connected(int i, int j)
{
  return (coincidence[i][j]);
}

//==========================================================

int OrientedGraph::notConnected(int i, int j)
{
  return (coincidence[i][j]);
}

//==========================================================

int OrientedGraph::existsPath(int i, int j)
{
  return (path[i][j]);
}

//==========================================================

int OrientedGraph::getNumIn(int i)
{
  return numIn[i];
}

//==========================================================

int OrientedGraph::getNumOut(int i)
{
  return numOut[i];
}

//==========================================================

int OrientedGraph::getMark(int i)
{
  return mark[i];
}

//==========================================================

int OrientedGraph::getNumberOfVertices()
{
  return N;
}

//==========================================================

int *OrientedGraph::getParentList(int i)
{
  return parentList[i];
}

//==========================================================

int  OrientedGraph::printCoincidenceMatrix(FILE *out)
{
  int i,j;

  fprintf(out,"coincidenceMatrix\n");
  for (i=0; i<N; i++)
    {
      for (j=0; j<N; j++)
	fprintf(out,"%u ",coincidence[i][j]);
      fprintf(out,"\n");
    }
  
  return SUCCESS;
}

//==========================================================

int  OrientedGraph::printPathMatrix(FILE *out)
{
  int i,j;

  fprintf(out,"pathMatrix\n");
  for (i=0; i<N; i++)
    {
      for (j=0; j<N; j++)
	fprintf(out,"%u ",path[i][j]);
      fprintf(out,"\n");
    }
  
  return SUCCESS;
}

//==========================================================

int  OrientedGraph::printNumInArray(FILE *out)
{
  int i,j;

  fprintf(out,"numIn\n");
  for (i=0; i<N; i++)
    {
      fprintf(out,"%u (",numIn[i]);
      for (j=0; j<numIn[i]; j++)
	fprintf(out,"%u ",parentList[i][j]);
      fprintf(out,")");
    }

  fprintf(out,"\n");

  return SUCCESS;
}

//==========================================================

int  OrientedGraph::printNumOutArray(FILE *out)
{
  int i;

  fprintf(out,"numOut\n");
  for (i=0; i<N; i++)
    fprintf(out,"%u ",numOut[i]);
  fprintf(out,"\n");

  return SUCCESS;
}

//==========================================================
//==========================================================
//==========================================================


AcyclicOrientedGraph::AcyclicOrientedGraph(int n):OrientedGraph(n)
{
}

//==========================================================

AcyclicOrientedGraph::~AcyclicOrientedGraph()
{
}

//==========================================================

int AcyclicOrientedGraph::addEdge(int i, int j)
{
  if (canAddEdge(i,j))
    return OrientedGraph::addEdge(i,j);
  else
    return FAIL;
}

//==========================================================

int AcyclicOrientedGraph::canAddEdge(int i, int j)
{
  return (!existsPath(j,i))&&(!connected(i,j));
}

//==========================================================

int AcyclicOrientedGraph::reverseEdge(int i, int j)
{
  if (connected(i,j))
  {
    removeEdge(i,j);

    if (addEdge(j,i))
      return SUCCESS;
    else
      {
        addEdge(i,j);
	return FAIL;
      }
  }
  else
    return FAIL;
}

//==========================================================

int AcyclicOrientedGraph::canReverseEdge(int i, int j)
{
  int result;

  if (connected(i,j))
    {
      removeEdge(i,j);
      result = (!existsPath(i,j));
      addEdge(i,j);

      return result;
    }
  else
    return FAIL;
}

//==========================================================
