#pragma once
#include "Snap.h"
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <boost/dynamic_bitset.hpp>
#include "DirectedGraph.h"



#include <iostream> 
#include <iterator> 
#include <map> 

using namespace std;

class DynamicGraph
{
public:
	map<unsigned int, DirectedGraph> graph;
	string fileName;
	vector<vector<boost::dynamic_bitset<>>> matrice;
	int edges;
	int nodes;
	int Timstemps;
	vector<long> idT;


public:
	DynamicGraph() {

	};
	DynamicGraph(const char* fileName);
	
	void init(string filename);

	DynamicGraph(string filename);

	void printGraph();

	DirectedGraph getGraphAt(unsigned int timestemp) {
		return graph[timestemp];
	}

	string getFileName();

	map<unsigned int, DirectedGraph> getGraph() {
		return graph;
	}

	int getTimstemps();
	int getNodes();
	vector<vector<boost::dynamic_bitset<>>> getMatrix();

	~DynamicGraph();

	void creatMatrice() {
		//cout << nodes << endl;
		//cout << Timstemps << endl;
		for (int j = 0; j < Timstemps; j++)
		{
			vector<boost::dynamic_bitset<>> vect;
			for (int i = 0; i < nodes; i++)
			{
				boost::dynamic_bitset<> c{ nodes,0 };
				vect.push_back(c);
			}
			matrice.push_back(vect);
		}
		//cout << "matrice created " << endl;
	}
};

