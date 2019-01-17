#pragma once
#include <string>
#include "k2_Trees.hpp"
#include "DirectedGraph.h"
#include "Ik2_trees.hpp"
#include "DynamicGraph.h"

using namespace std;
class motorK2_trees
{


public:
	motorK2_trees();
	~motorK2_trees();

	void compressGraph(const char* graphName,int k, string type)
	{
		if (type == "K2") compressGraphK2Stat(graphName, k);
		if (type == "IK2") compressGraphInterK2(graphName, k);
		if (type == "DIK2") compressGraphDiffInterK2(graphName, k);

	};

	void compressGraphK2Stat(const char* graphName, int k) {
		DirectedGraph graph(graphName);
		k2_Trees tree(k, graph.getNodes(), graph.getMatrix());
		cout << "T = " << tree.get_T() << endl;
		cout << "L = " << tree.get_L() << endl;
	};

	void compressGraphInterK2(const char* graphName, int k) {
		DynamicGraph graph(graphName);
		Ik2_Trees tree(k, graph.getTimstemps(), graph.getNodes(), 0, graph.getMatrix());
		cout << "T = " << tree.get_T() << endl;
		cout<< "L = "  << tree.get_L() << endl;
	};

	void compressGraphDiffInterK2(const char* graphName, int k) {
		DynamicGraph graph(graphName);
		Ik2_Trees tree(k, graph.getTimstemps(), graph.getNodes(), 1, graph.getMatrix());
		cout << "T = " << tree.get_T() << endl;
		cout << "L = " << tree.get_L() << endl;
	};

	void saveGraph();
};

