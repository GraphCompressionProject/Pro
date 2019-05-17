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

	bool isEdge(int tmp, int node1, int node2) {
		return (tmp<graph.size()) && (graph[tmp].edgeBetween(graph[tmp].getNodeID(node1), graph[tmp].getNodeID(node2)));
	}

	void DelEdge(int tmp, int node1, int node2) {
		graph[tmp].DelEdge(graph[tmp].getNodeID(node1), graph[tmp].getNodeID(node2));
	}

	void addEdge2(int tmp, int node1, int node2) {
		graph[tmp].addedge2(node1, node2);
	}

	void setGraph(map<unsigned int, DirectedGraph> G) {
		for (auto & elem : G) {
			graph.insert({ elem.first,elem.second });
		}
	}

	

	void copyDiff(DynamicGraph *G) {

		map<unsigned int, DirectedGraph> g_tmp;
		
		int i = 0;
		
		map<unsigned int, DirectedGraph>::iterator prec;
		map<unsigned int, DirectedGraph>::iterator elem;
		
		for (elem = graph.begin(); elem != graph.end(); elem++) {
			if (i != 0) {
				
				DirectedGraph Gt_i_1 = prec->second;
				DirectedGraph Gt_i = elem->second;
				
				for (TNGraph::TEdgeI EI = Gt_i_1.BegEI(); EI < Gt_i_1.EndEI(); EI++) {
					if (Gt_i.edgeBetween(EI.GetSrcNId(), EI.GetDstNId())) {
						Gt_i.DelEdge(EI.GetSrcNId(), EI.GetDstNId());
					}
					else {
						Gt_i.addedge2(EI.GetSrcNId(), EI.GetDstNId());
					}
				}
				
				g_tmp.insert({ elem->first,Gt_i });

			}
			else {
				
				g_tmp.insert({ elem->first,elem->second });
			}
			
			prec = elem;
			i++;
		}

		G->setGraph(g_tmp);
		
	}

	unsigned int nbtmp() {
		return graph.size();
	}

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

