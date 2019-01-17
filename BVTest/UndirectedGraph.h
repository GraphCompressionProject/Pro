#pragma once
#include "Snap.h"

using namespace std;

class UndirectedGraph
{
public: 
	PUNGraph graph;

public:
	UndirectedGraph(const char* fileName);
	UndirectedGraph(PUNGraph& G);

	int getNumNodes();
	int getNumEdges();
	
	TUNGraph::TNodeI getNodeIteratorBegin();
	TUNGraph::TNodeI getNodeIteratorEnd();
	TUNGraph::TEdgeI getEdgeIteratorBegin();
	TUNGraph::TEdgeI getEdgeIteratorEnd();

	bool edgeBetween(int node1, int node2);
	bool emptyGraph();

	void sortAdjencyLists();

	static int getNodeID(TUNGraph::TNode myNode);
	static int getDegree(TUNGraph::TNode myNode);
	static int getSuccessorIdAt(TUNGraph::TNode myNode, int index);
	bool isSuccessor(TUNGraph::TNode myNode, int index);
	static int dirExists(const std::string& dirName_in)
	{
		DWORD ftyp = GetFileAttributesA(dirName_in.c_str());
		if (ftyp == INVALID_FILE_ATTRIBUTES)
			return 1;  //doesn't exist

		if (ftyp & FILE_ATTRIBUTE_DIRECTORY)
			return 0;   // this is a directory!

		return -1;    // this is not a directory!
	}

	~UndirectedGraph();
};

