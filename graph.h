#ifndef __GRAPH_H_INCLUDED__
#define __GRAPH_H_INCLUDED__

class OrientedGraph{
	private:
		int N; 		//the number of vertices
		int N2; 	// N square
		char **coincidence;			//the coincidence matrix
		char **path;	//the matrix for maintanance of path
		int *numIn;		//the number of incoming vertices
		int *numOut;	//the number of outgoing vertices
		int *mark;		//the array for vertex-marks
		int **parentList;	//the list of parents for each node
		
	public:
		OrientedGraph (int n);
		virtual ~OrientedGraph ();

		int size();
		int addEdge (int i, int j);
		int removeEdge (int i, int j);
		int reverseEdge (int i, int j);
		int removeAllEdges ();
		int setMark (int i, int val);
		int setAllMarks (int val);
		int removeMark (int i);
		int removeAllMarks ();

		int getNumberOfVertices ();
		int connected (int i, int j);
		int notConnected (int i, int j);
		int existsPath (int i, int j);
		int getNumIn (int i);
		int getNumOut (int i);
		int getMark (int i);
		int *getParentList (int i);
		int canAddEdge (int i, int j);
		int canReverseEdge (int i, int j);
		
		int printCoincidenceMatrix (FILE *out);
		int printPathMatrix (FILE *out);
		int printNumInArray (FILE *out);
		int printNumOutArray (FILE *out);
		
};

class AcyclicOrientedGraph: public OrientedGraph {

 public:
  AcyclicOrientedGraph(int n);
  ~AcyclicOrientedGraph();

  int addEdge(int i, int j);
  int reverseEdge(int i, int j);
  int canAddEdge(int i, int j);
  int canReverseEdge(int i, int j);
};


#endif
