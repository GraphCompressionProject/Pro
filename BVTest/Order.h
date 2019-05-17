#pragma once

#include "DirectedGraph.h"
#include "stdafx.h"
#include <iostream>
#include <algorithm>
#include <vector>
#include <functional>
#include <map>
#include <windows.h>
#include <string>
#include <stack>
#include <direct.h>


#include <fstream>



using namespace std;
using namespace std::placeholders;
class Order {

public:
	
    TNGraph graph; 


public:
	Order(DirectedGraph& G) { graph = *(G.graph); }
	~Order() {}


	vector<int> RandomOrder(DirectedGraph & G)
	{
		vector<int> a;
		for (TNGraph::TNodeI NI = G.getNodeIteratorBegin(); NI< G.getNodeIteratorEnd(); NI++)
		{
			a.push_back(NI.GetId());
		}

		//cout << "Random Order of the graph in process ..." << endl;
		random_shuffle(std::begin(a), std::end(a));
		saveOrderedGraph(G, a, "RandomOrder_");
		//cout << "Random Order accomplished." << endl;
		return a;

	}		
	vector<int> lexicographicalOrder(DirectedGraph & G)
	{
		vector<int> a;
		for (TNGraph::TNodeI NI = G.getNodeIteratorBegin(); NI< G.getNodeIteratorEnd(); NI++)
		{
			a.push_back(NI.GetId());
		}

		
		//cout << "Lexicographical Order of the graph in process ..." << endl;
		std::sort(a.begin(), a.end(),   bind(&Order::lexicographicalComparator, this, _1, _2));
		saveOrderedGraph(G, a, "lexicographicalOrder_");
		//cout << "Lexicographical Order accomplished" << endl;	
		return a;
	}	
	vector<int> GrayOrder(DirectedGraph & G)
	{
		vector<int> a;
		for (TNGraph::TNodeI NI = G.getNodeIteratorBegin(); NI< G.getNodeIteratorEnd(); NI++)
		{
			a.push_back(NI.GetId());
		}

		//cout << "Gray Order of the graph in process ..." << endl;
		std::sort(a.begin(), a.end(), bind(&Order::GrayComparator, this, _1, _2));
		saveOrderedGraph(G, a, "GrayOrder_");
		//cout << "Gray Order accomplished" << endl;
		return a;
		

	}
	vector <int> DfsOrder(DirectedGraph & G)
	{   
		vector <int> order;
		map<int, bool> a; 
		//initilaisation du vecteur des noeuds et visité à faux 
		for (TNGraph::TNodeI NI = G.getNodeIteratorBegin(); NI< G.getNodeIteratorEnd(); NI++)
		{
			a[NI.GetId()] = false;
		}

		
		map<int, bool>::iterator it; 
		vector<int> node_succ;
		vector<int>::iterator it_succ;
		int node; 
		
		for ( it= a.begin(); it != a.end(); ++it)
		{
			if (it->second == false) { 
				stack<int> stack;
				stack.push(it->first);
				while (!stack.empty())
				{
					node = stack.top();
					stack.pop();

					if (a[node]==false)
					{
						order.push_back(node);
						a[node] = true;
					}

					node_succ = DirectedGraph::getOutNeighborsVector(graph.GetNI(node));
					for (it_succ = node_succ.begin(); it_succ != node_succ.end(); ++it_succ)
						if (a[*it_succ] == false)
						{
							stack.push(*it_succ);
						}
				}
			
			}
		}
		
		//cout << "DFS Order of the graph in process ..." << endl;
		saveOrderedGraph(G, order, "DfsOrder_");
		//cout << "DFS Order accomplished" << endl;
		return order;
	}
	vector <int> BfsOrder(DirectedGraph & G) {

		
		vector <int> order;

		map<int, bool> a;
		for (TNGraph::TNodeI NI = G.getNodeIteratorBegin(); NI< G.getNodeIteratorEnd(); NI++)
		{
			a[NI.GetId()] = false;

		}

		int node; 
		list<int> queue;
		vector<int> node_succ;
		vector<int>::iterator it_succ; 
		map<int, bool>::iterator map_it;

		for (map_it = a.begin(); map_it != a.end() ; ++map_it)
		{
			if (map_it->second == false) {
				
				node = map_it->first;
				map_it->second = true;
				queue.push_back(node); 

				while (!queue.empty())
				{
					node = queue.front();
					order.push_back(node);
					queue.pop_front();

					//récupérer les successeur du noeud défilé 
					node_succ = DirectedGraph::getOutNeighborsVector(graph.GetNI(node));

					// si un noeud adjacent n'a pas été visité alors le marquer et l'enfiler 
					for (it_succ = node_succ.begin(); it_succ != node_succ.end(); ++it_succ)
					{
						if (a[*it_succ] == false)
						{
							a[*it_succ] = true;
							queue.push_back(*it_succ);
						}
					}
				}
			}

		}


		//cout << "BFS Order of the graph in process ..." << endl;
		saveOrderedGraph(G, order, "BfsOrder_");
		//cout << "BFS Order accomplished" << endl;
		return order;
	}
	

private: 

	bool lexicographicalComparator(int x, int y)
	{
		vector<int> x_succ, y_succ;
		x_succ = DirectedGraph::getOutNeighborsVector(graph.GetNI(x));
		y_succ = DirectedGraph::getOutNeighborsVector(graph.GetNI(y));

		std::vector<int>::iterator x_it = x_succ.begin(), x_itend = x_succ.end();
		std::vector<int>::iterator y_it = y_succ.begin(), y_itend = y_succ.end();

		int a, b;
		for (;;)
		{

			if (x_it == x_itend && y_it == y_itend)  return false;
			if (x_it == x_itend) return true;
			if (y_it == y_itend) return false;

			a = *x_it;
			b = *y_it;

			if (a != b) return ((b - a) < 0 ? true : false);

			++x_it;
			++y_it;
		}

	}
	bool GrayComparator(int x, int y)
	{
		vector<int> x_succ, y_succ;
		x_succ = DirectedGraph::getOutNeighborsVector(graph.GetNI(x));
		y_succ = DirectedGraph::getOutNeighborsVector(graph.GetNI(y));

		std::vector<int>::iterator x_it = x_succ.begin(), x_itend = x_succ.end();
		std::vector<int>::iterator y_it = y_succ.begin(), y_itend = y_succ.end();

		int a, b;
		bool parity = false;
		for (;;)
		{

			if (x_it == x_itend && y_it == y_itend)  return false;
			if (x_it == x_itend) return parity ? false : true;
			if (y_it == y_itend) return parity ? true : false;

			a = *x_it;
			b = *y_it;
			if (a != b) return parity ^ (a < b) ? true : false;

			parity = !parity;
			++x_it;
			++y_it;
		}
	}
	
	void saveOrderedGraph(DirectedGraph & G, vector<int> order, string OrderName)
	{


		ofstream myfile;

		string folder1 = "..\\BVTest\\data\\";
		string folder2 = (G.fileName).substr(0, (G.fileName).find("."));

		int val = DirectedGraph::dirExists(folder1 + folder2);
		if (val != 0) {
			if (val == -1) { cerr << "Path Error ..." << endl; exit(0); }
			else { mkdir((folder1 + folder2).c_str()); }
		}
		



		myfile.open(folder1 + folder2 + "\\" + OrderName + G.fileName +".txt" , ios::out);
		string line;
		if (myfile.is_open())
		{
			ostringstream o;
			o << G.getNumNodes();
			myfile << o.str() + "\n";
			for (std::vector<int>::iterator it = order.begin(); it != order.end(); ++it)
			{
				myfile << to_string(*it) + " : " + DirectedGraph::getOutNeighbors(G.getNodeIteratorOf(*it)) + "\n";
			}
			myfile.close();
		}
		else {
			//cerr << "File not opened" << endl;
		}
	}
	


	

};

