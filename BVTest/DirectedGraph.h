#pragma once
#include "Snap.h"
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <boost/dynamic_bitset.hpp>
#include "listAdjacence.h"

using namespace std;


class DirectedGraph
{
public: 
	PNGraph graph;
	string fileName;
	/*
	 * Adjacency Matrix of the graph
	 */
	vector<boost::dynamic_bitset<>> matrice; 
	/*
	 * Adjacency List of the graph
	 */
	vector<listAdjacence> liste;
	int nodes;
	
	
public:
	DirectedGraph(const char* fileName);
	DirectedGraph(string fileName);
	DirectedGraph(bool empty, int n, int m, string fileName);
	DirectedGraph(const char* fileName, int load);
	DirectedGraph(PNGraph& G);
	

	int getNumNodes();
	int getNumEdges();
	int getNodes();
	vector<boost::dynamic_bitset<>> getMatrix();
	vector<listAdjacence> getAdjList();
	string getFileName() { return fileName; }

	TNGraph::TNodeI getNodeIteratorBegin();
	TNGraph::TNodeI getNodeIteratorEnd();
	TNGraph::TNodeI getNodeIteratorOf(int nodeID);
	TNGraph::TEdgeI getEdgeIteratorBegin();
	TNGraph::TEdgeI getEdgeIteratorEnd();

	bool edgeBetween(int node1, int node2);
	bool emptyGraph();
	void sortAdjencyLists();
	void saveGraph();
	void saveAsSnapGraph() {

		string folder1 = "..\\..\\BVTest\\data\\";	
		TSnap::SaveEdgeList(graph, (folder1 + fileName).c_str(), "Save as tab-separated list of edges");
	}

	void saveAsSnapGraph(const char * myFileName) {
		TSnap::SaveEdgeList(graph, myFileName, "Save as tab-separated list of edges");
	}

	void creatMatrice(int n) {
		for (int i = 0; i < n; i++) {
			boost::dynamic_bitset<> c{ n,0 };
			matrice.push_back(c);
		}
	}

	void addNodes(int node1, int node2);

	static int getNodeID(TNGraph::TNode myNode);
	static int getInDegree(TNGraph::TNode myNode);
	static int getOutDegree(TNGraph::TNode myNode);
	static int getDegree(TNGraph::TNode myNode);
	static int getInNeighborAt(TNGraph::TNode myNode, int index);
	static int getOutNeighborAt(TNGraph::TNode myNode, int index);
	static bool isInNeighbor(TNGraph::TNode myNode, int nodeId);
	static bool isOutNeighbor(TNGraph::TNode myNode, int nodeId);
	static string getOutNeighbors(TNGraph::TNodeI myNode);
	static vector<int> getOutNeighborsVector(TNGraph::TNodeI myNode);

	//return : -1 > path error ; 0 > directory exists ; 1 > directory doesn't exist
	static int dirExists(const std::string& dirName_in)
	{
		DWORD ftyp = GetFileAttributesA(dirName_in.c_str());
		if (ftyp == INVALID_FILE_ATTRIBUTES)
			return 1;  //doesn't exist

		if (ftyp & FILE_ATTRIBUTE_DIRECTORY)
			return 0;   // this is a directory!

		return -1;    // this is not a directory!
	}

	void showNeigbors()
	{
		for (TNGraph::TNodeI NI = getNodeIteratorBegin(); NI < getNodeIteratorEnd(); NI++)
		{
			cout << NI.GetId() << ":" << getOutNeighbors(NI) << endl;
		}
	}


	void plotGraph();

	~DirectedGraph();
};

