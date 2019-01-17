#pragma once
#include "Module.h"
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>
#include <set>
#include "UndirectedGraph.h"
#include <stack>
#include <windows.h>
#include <direct.h>
#include <boost/lexical_cast.hpp>

using namespace std;



class SuperGraph
{
public:

	TPt<TNodeNetModule<TStr> > graph;
	string fileName;
	double fileSize;

	struct classcomp {
		bool operator() (const pair<int, int> &edge, const pair<int, int> &edge2) const
		{
			if (edge.first == edge2.first && edge.second == edge2.second) return false;
			else if (edge.first == edge2.second && edge.second == edge2.first) return false;
			else if (edge.first == edge2.first)
			{
				return edge.second < edge2.second;

			}
			else return edge.first < edge2.first;

		}
	};

	typedef pair<int, int> edgeCouple;
	typedef set<edgeCouple, classcomp> edgeSetCouple;
	typedef set<edgeCouple, classcomp>::iterator setiterator;

	

public:

	SuperGraph() { graph = TNodeNetModule<TStr>::New(); }
	SuperGraph(string fileName);
	SuperGraph(bool empty, string fileName) {

		graph = TNodeNetModule<TStr>::New();
		this->fileName = fileName;

		string line;
		fstream myfile;

		
		myfile.open(("..\\BVTest\\data2\\" + fileName + ".txt").c_str(), ios::in);
		//myfile.open("file")

		//std::istringstream iss;
		//std::vector<int> results;
		if (myfile.is_open()) {
			myfile.seekg(0, myfile.end);
			fileSize = myfile.tellg();
			myfile.seekg(0, myfile.beg);

			while (getline(myfile, line))
			{

				vector<string> x = split(line);

				graph->AddEdge2(atoi(x[0].c_str()), atoi(x[1].c_str()));

			}
			myfile.close();
			std::cout << "File Loaded" << std::endl;
		}
		else
		{
			cerr << "File not opened" << endl;
		}


	}
	//TPt<TNodeNetModule<TStr>> Supergr(const char* fileName, bool var);
	//SuperGraph(const char* fileName);

	//SuperGraph(const vector < TNodeNetModule<TStr>::TNodeI>);
	vector<string> split(string line);
	void plotGraph();
	TNodeNetModule<TStr>::TNodeI getNodeIteratorBegin();
	TNodeNetModule<TStr>::TNodeI getNodeIteratorEnd();
	TNodeNetModule<TStr>::TNodeI getNode(int node);
	TStr GetNDat(int node);

	char GetType(TNodeNetModule<TStr>::TNodeI node) { return node.GetType(); }
	int GetNodes() { return graph->GetNodes(); }
	int GetEdges() { return graph->GetEdges(); }
	int  GetID(TNodeNetModule<TStr>::TNodeI node) { return node.GetId(); }
	TStr GetLabel(TNodeNetModule<TStr>::TNodeI node) { return node.GetDat(); }
	TPt<TNodeNetModule<TStr>> GetContent(TNodeNetModule<TStr>::TNodeI node) { return node.GetModuleContent(); }
	bool isNode(int node) { return graph->IsNode(node); }


	

	string GetModuleContent(TNodeNetModule<TStr>::TNodeI node);
	

	void copy(TNodeNetModule<TStr>::TNodeI &node1, TNodeNetModule<TStr>::TNodeI &node2)
	{
		node1.GetDat() = node2.GetDat();
		//à revoir si le pointeur ne se supprime pas en supprimant les noeuds du graphe initial
		node1.GetModuleContent() = node2.GetModuleContent();
	}


	//affiche le graphe et ses modules sous frme de string 	 
	static ostringstream ModuleToString(TNodeNetModule<TStr>::TNodeI myNode)
	{
	
			vector <    pair<  pair<TNodeNetModule<TStr>::TNodeI, int>,bool   >   > order;
			
			TIntSet visited;

			TPt<TNodeNetModule<TStr>> fils;
			TNodeNetModule<TStr>::TNodeI it_fils;
			TNodeNetModule<TStr>::TNodeI  node;

		
		
					stack<TNodeNetModule<TStr>::TNodeI> stack;
					stack.push( myNode);

					while (!stack.empty())
					{
						node = stack.top();
						stack.pop();

						if(!visited.IsKey(node.GetId()))
				
						{
							order.push_back(make_pair(make_pair(node, node.GetModuleContent()->GetNodes()), false));
							visited.AddKey(node.GetId());
						}
						fils = node.GetModuleContent();

						for (it_fils = fils->BegNI(); it_fils != fils->EndNI(); it_fils++)
								if (!visited.IsKey(it_fils.GetId()))
							{
								stack.push(it_fils);
							}
					}
	
					return afficherVecteur(order);
	
	}


	static ostringstream afficherVecteur(vector <    pair<  pair<TNodeNetModule<TStr>::TNodeI, int>, bool   >   > & order)
	{
		ostringstream o; 
		TIntSet myset;
		int i = 0; 
		while (!order.empty())
		{
			if (order[i].second == false)
			{
				order[i].second = true;
				
				o << "{ ";
				o << to_string(order[i].first.first.GetId()) + " ";
				string s(order[i].first.first.GetDat().CStr());
				o << s;
				o << " ";
				char t = order[i].first.first.GetType();
				o.put(t);
				o << " ";

				if(!order[i].first.first.GetModuleContent().Empty())
				o << order[i].first.first.GetModuleContent()->GetNodes();
				o << " ";
			}

			if (order[i].first.second == 0)
			{
				if (order[i].first.first.GetType() == 'N')
				{
					// afficher les liens 


					o << "# \n";
					o << "(\n";
		
					for (TNodeNetModule<TStr>::TNodeI NI = order[i].first.first.GetModuleContent()->BegNI(); NI < order[i].first.first.GetModuleContent()->EndNI(); NI++)
					{
						if (!SetAllNeighbors(NI, myset)) {
							o << getNeighbors(NI, myset);
							myset.AddKey(NI.GetId());
							o << "\n";
						}

					}
					myset.Clr();
					o << ")\n";

				}
				if (i != 0)
				{
					order[i - 1].first.second--;
					order.erase(order.begin() + i); 
					i--;
				}
				else {
					order.erase(order.begin() + i);
				}

				o << " } ";
			}
			else
			{
				i++;
			}

		}

		return o;
	}





	static ostringstream ModuleToString2(TNodeNetModule<TStr>::TNodeI myNode)
	{
		TIntSet myset;
		ostringstream o;
		o << "{ ";
		o << to_string(myNode.GetId()) + " ";
		string s(myNode.GetDat().CStr());
		o << s;
		o << " ";
		char t = myNode.GetType();
		//string t(myNode.GetType());
		//o << "type=";
		o.put(t);
		//o << "" + to_string(myNode.GetType()) + " ";
		o << " ";

		//if (myNode.GetType() != 'X')
		if (!myNode.GetModuleContent().Empty())
		{
			//o << "nb=";

			o << myNode.GetModuleContent()->GetNodes();

			//o << "\n";
			//o << "\n\t";
			//o << "\n";
			for (TNodeNetModule<TStr>::TNodeI NI = myNode.GetModuleContent()->BegNI(); NI < myNode.GetModuleContent()->EndNI(); NI++)
			{
				o << ModuleToString(NI).str();
				//o << " \t";
			}
		}
		if (myNode.GetType() == 'N') {
			o << "# \n";
			o << "(\n";
			//set.Clr;
			//	cout << "i'am here" << endl;
			for (TNodeNetModule<TStr>::TNodeI NI = myNode.GetModuleContent()->BegNI(); NI < myNode.GetModuleContent()->EndNI(); NI++)
			{
				/*
				if (!SetAllNeighbors(NI, set)) {
				o << getNeighbors(NI, set);
				set.AddKey(NI.GetId());
				o << "\n";
				}*/
				if (!SetAllNeighbors(NI, myset)) {
					o << getNeighbors(NI, myset);
					myset.AddKey(NI.GetId());
					o << "\n";
				}

			}
			myset.Clr();
			o << ")\n";

		}
		o << "}";

		return o;
	}

	


	 ostringstream GraphToString()
	{
		 TIntSet myset;
		ostringstream o;

		for (TNodeNetModule<TStr>::TNodeI NI = graph->BegNI(); NI < graph->EndNI(); NI++)
		{
			o << ModuleToString(NI).str();
			o << "\n";
		}
		o << "# \n";
		for (TNodeNetModule<TStr>::TNodeI NI = graph->BegNI(); NI < graph->EndNI(); NI++)
		{
			if (!SetAllNeighbors(NI, myset)) {
				o << getNeighbors(NI, myset);
				myset.AddKey(NI.GetId());
				o << "\n";
			}
		}
		o << "# \n";
		
		


		return o;
	}

	

	static bool SetAllNeighbors(TNodeNetModule<TStr>::TNodeI myNode, TIntSet mySet) {
		 int nb=0;
		 for (int i = 0; i < myNode.GetDeg(); i++) {
			 if (mySet.IsKey(myNode.GetNbrNId(i)))
			 {
				 nb++;
			 }
		 }
		 if (nb == myNode.GetDeg()) return true;
		 else return false;
	 }

	string savela() {

		fstream myfile;
		TIntSet myset;
		ostringstream o;

		string folder1 = "..\\BVTest\\data2\\";
		//string folder2 = (fileName).substr(0, (fileName).find("."));
		int val = UndirectedGraph::dirExists(folder1 );
		if (val != 0) {
			if (val == -1) { cerr << "Path Error ..." << endl; exit(0); }
			else { mkdir((folder1 ).c_str()); }
		}

		//	myfile.open( fileName + ".txt", ios::out);
		string name = folder1  + "\\" + fileName + "label";
		myfile.open(name + ".txt", ios::out);
		if (myfile.is_open())
		{
			

			for (TNodeNetModule<TStr>::TNodeI NI = graph->BegNI(); NI < graph->EndNI(); NI++)
			{
				myfile << "v";
				myfile << " ";
				myfile << NI.GetId();
				myfile << " " ;
				myfile << NI.GetDat().CStr();
				myfile << "\n";
			}
			for (TNodeNetModule<TStr>::TEdgeI EI = graph->BegEI(); EI < graph->EndEI(); EI++)
			{
				myfile << "e";
				myfile << " ";
				myfile << EI.GetSrcNId();
				myfile << " ";
				myfile << EI.GetDstNId();
				myfile << "\n";
			}
		}
		myfile.close();
		return name;
	}

	 static string getNeighbors(TNodeNetModule<TStr>::TNodeI myNode, TIntSet mySet )
	 {
		 ostringstream o;
		 if (!mySet.IsKey(myNode.GetId()))
		 {
			 if (!SetAllNeighbors(myNode, mySet))
			 {
				 o << myNode.GetId();
				 o << " : ";
				 for (int i = 0; i < myNode.GetDeg(); i++) {
					 if (!mySet.IsKey(myNode.GetNbrNId(i)))
					 {
						 o << myNode.GetNbrNId(i);
					 }
					 if (i != myNode.GetDeg() - 1) o << " ";
				 }
			 }
			 
		 }
		 
		 return o.str();
	 }

	// successeur du noeud first 
	string Firstequal(edgeSetCouple &S, int first)
	{
		ostringstream  o;
		o << first  ;
		
		setiterator it = S.begin();
		while ( it != S.end())
		{
			if (it->first == first)
			{
				o << it->second  ;
				S.erase(it);
				it = S.begin();
			}
			else
			{
				it++;
			}

		}

		//o += "\n";
		return o.str();
	}


	double savedGraph(int k,string agre)
	{
		fstream myfile;


		string folder1 = "..\\BVTest\\data2\\";
		string folder2 = (fileName).substr(0, (fileName).find("."));
		int val = UndirectedGraph::dirExists(folder1 + folder2);
		if (val != 0) {
			if (val == -1) { cerr << "Path Error ..." << endl; exit(0); }
			else { mkdir((folder1 + folder2).c_str()); }
		}

	//	myfile.open( fileName + ".txt", ios::out);
		myfile.open(folder1 + folder2 + "\\" + std::to_string(k)+ fileName+agre+"compresse"+".txt" , ios::out);
		if (myfile.is_open())
		{
			
			//myfile << "NumNodes=";
			myfile << graph->GetNodes();
			myfile <<  "\n";
			myfile << GraphToString().str();
			
			myfile.seekg(0, myfile.end);
			fileSize = myfile.tellg();
			myfile.seekg(0, myfile.beg);
			

			myfile.close();
		}
		

		else {
			cerr << "File not opened" << endl;
		}

		return fileSize;
	}


	string savedGraphlabel()
	{
		fstream myfile;


		string folder1 = "..\\BVTest\\data2\\";
		string folder2 = (fileName).substr(0, (fileName).find("."));
		int val = UndirectedGraph::dirExists(folder1 + folder2);
		if (val != 0) {
			if (val == -1) { cerr << "Path Error ..." << endl; exit(0); }
			else { mkdir((folder1 + folder2).c_str()); }
		}

		//	myfile.open( fileName + ".txt", ios::out);
		string name = folder1 + folder2 + "\\" + fileName + "label" ;
		myfile.open(name+"txt", ios::out);
		if (myfile.is_open())
		{

			//myfile << "NumNodes=";
			myfile << graph->GetNodes();
			myfile << "\n";
			myfile << GraphToString().str();

			myfile.seekg(0, myfile.end);
			fileSize = myfile.tellg();
			myfile.seekg(0, myfile.beg);


			myfile.close();
		}


		else {
			cerr << "File not opened" << endl;
		}

		return name;
	}

	void save()
	{
	
	}

	void Save() {
		string folder1 = "..\\BVTest\\data2\\";
		TSnap::SaveEdgeList(graph, (folder1 + fileName).c_str(), "label");
	}




	~SuperGraph();
};

