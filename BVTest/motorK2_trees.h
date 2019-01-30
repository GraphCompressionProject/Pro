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
		/*cout << "T = " << tree.get_T() << endl;
		cout << "L = " << tree.get_L() << endl;*/
		cout << "========================================================" << endl;
		std::fixed;

		cout << "Le ratio de Compression est de :" << (graph.getNodes()*graph.getNodes()) / (tree.get_T().size() + tree.get_L().size()) << endl;
		//cout << "Le gain d'espace :" << (double)100 * (1 - ((tree.get_T().size() + tree.get_L().size()) / (graph.getNodes()*graph.getNodes()))) << "%" << endl;
		cout << "Le nombre de bit par noeuds :" << (double)(tree.get_T().size() + tree.get_L().size()) / graph.getNodes() << endl;
		cout << "Le temps de Compression est de :" << (double)tree.get_Time() << endl;
	};

	void compressGraphK2StatFromListe(const char* graphName, int k) {
		string s = string(graphName);
		
		DirectedGraph graph(s);

		vector<listAdjacence> t = graph.getAdjList();
		
		k2_Trees tree(k, graph.getNodes(), t);
		/*cout << "T = " << tree.get_T() << endl;
		cout << "L = " << tree.get_L() << endl;*/
		cout << "========================================================" << endl;
		std::fixed;

		cout << "Le ratio de Compression est de :" << (graph.getNodes()*graph.getNodes()) / (tree.get_T().size() + tree.get_L().size()) << endl;
		//cout << "Le gain d'espace :" << (double) 1 - ((tree.get_T().size() + tree.get_L().size()) / (graph.getNodes()*graph.getNodes())) << endl;
		cout << "Le nombre de bit par noeuds :" << (double) (tree.get_T().size() + tree.get_L().size()) / graph.getNodes() << endl;
		cout << "Le temps de Compression est de :" << (double)tree.get_Time() << endl;
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

	
};

