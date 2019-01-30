#pragma once
#include <string>
#include "k2_Trees.hpp"
#include "DirectedGraph.h"
#include "Ik2_trees.hpp"
#include "DynamicGraph.h"
#include <iostream>

using namespace std;
class motorK2_trees
{


public:
	motorK2_trees();
	~motorK2_trees();

	void compressGraph(const char* graphName,int k, string type)
	{
		if (type == "K2_Matrix") compressGraphK2Stat(graphName, k,0);
		if (type == "K2_List") compressGraphK2Stat(graphName, k,1);
		if (type == "IK2") compressGraphInterK2(graphName, k);
		if (type == "DIK2") compressGraphDiffInterK2(graphName, k);

	};

	void compressGraphK2Stat(const char* graphName, int k,int AdjOrMatrix) {
		switch (AdjOrMatrix) {
		case 0:
			compressGraphK2StatFromMatrix(graphName, k);
			break;
		case 1:
			compressGraphK2StatFromListe(graphName, k);
			break;
		}
	};

	void compressGraphK2StatFromMatrix(const char* graphName, int k) {
		DirectedGraph graph(graphName);
		k2_Trees tree(k, graph.getNodes(), graph.getMatrix());
		cout << "T = " << tree.get_T() << endl;
		cout << "L = " << tree.get_L() << endl;
	};

	void compressGraphK2StatFromListe(const char* graphName, int k) {
		string s = string(graphName);
		
		DirectedGraph graph(s);

		vector<listAdjacence> t = graph.getAdjList();
		
		k2_Trees tree(k, graph.getNodes(), t);
		cout << "T = " << tree.get_T() << endl;
		cout << "L = " << tree.get_L() << endl;
	};

	void compressGraphInterK2(const char* graphName, int k) {
		DynamicGraph graph(graphName);
		
	};

	void compressGraphDiffInterK2(const char* graphName, int k) {
		DynamicGraph graph(graphName);
		Ik2_Trees tree(k, graph.getTimstemps(), graph.getNodes(), 1, graph.getMatrix());
		cout << "T = " << tree.get_T() << endl;
		cout << "L = " << tree.get_L() << endl;
	};

	void saveGraph();
};

