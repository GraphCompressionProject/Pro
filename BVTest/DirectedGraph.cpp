#include "stdafx.h"
#include "DirectedGraph.h"
#include <boost/lexical_cast.hpp>
#include <string>
#include <direct.h>
#include "Order.h"



DirectedGraph::DirectedGraph(bool empty, int n, int m, string fileName)
{
	graph = TNGraph::New();
	this->fileName = fileName;
	if (!empty)
	{
		for (int i = 1; i<=n; ++i) graph->AddNode(i);
		for (int i = 1; i<=m; ++i)
		{
			while (true)
			{
				int j = TInt::GetRnd(n);
				int k = TInt::GetRnd(n);
				if ((j != k) && (!graph->IsEdge(j, k)) && (j>0 && k>0))
				{
					graph->AddEdge(j, k);
					break;
				}
			}
		}
	}
}
//charger un graphe depuis un fichier : si load 1 : le charger depuis un fichier Snap avec addnodes sinon chargement par défaut 
// 
//vérifier que le noeud source possede noeud destination sinon ajouter que source <--- à vérifier  (lors du chargement)
//
DirectedGraph::DirectedGraph(const char* fileName, int load)
{
	this->fileName =  fileName;
	if (load == 1)
	{
		graph = TNGraph::New();

		string line;
		fstream myfile; 
		myfile.open(("..\\..\\BVTest\\data\\" + this->fileName+".txt" ).c_str(), ios::in);
		
		myfile.seekg(0, myfile.end);
		double length = myfile.tellg();
		myfile.seekg(0, myfile.beg);

		//double partie = length / 10.0 ; 
		

		//double cumul = 0.0; 
		int pourcentage = 10;
		double pourcentage2; 
		double pourcen_cum=0; 
		int i = 0;

		cout << "file length " << (int) length << " bytes" << endl;

		std::istringstream iss;
		std::vector<int> results;
		if (myfile.is_open())
		{  
			std::cout << "File Loading ... " << std::endl;
			while (getline(myfile, line))
			{
				pourcen_cum += (((double)line.size()) / length)*100;
				if (pourcen_cum >= 10.0)
				{
					cout << " --- " << pourcentage << "%";
					pourcentage += 10;
					pourcen_cum -= 10;
					i++;
				}

				if (line[0] != '#')
				{
					iss.clear();
					iss.str(line);
					results.clear();
					for (int n = 0; n < 2; n++)
					{
						int val;
						iss >> val;
						results.push_back(val);
					}
				
					addNodes(results.at(0), results.at(1));
				}
			}

			while (i !=10)
			{
				cout << " --- " << pourcentage << "%";
				pourcentage += 10;
				i++;
			}
			cout << endl;


			myfile.close();
			std::cout << "File Loaded. " << std::endl;


		}
		else
		{
			cerr << "File not opened" << endl;
		}

	}
	else 
	{
		
		TFIn FIn((".\\data\\" + this->fileName).c_str());
		PNGraph G = TNGraph::Load(FIn);
		graph = G;
	}


}
DirectedGraph::DirectedGraph(PNGraph& G)
{
	graph = G;
}

DirectedGraph::DirectedGraph(const char* fileName)
{
		this->fileName = fileName;
		string line;
		fstream myfile;
		myfile.open(("..\\..\\BVTest\\data\\" + this->fileName + ".txt").c_str(), ios::in);
		
		myfile.seekg(0, myfile.end);
		double length = myfile.tellg();
		myfile.seekg(0, myfile.beg);
		//récupérer le nombre de noeuds 
		string nextline;
		getline(myfile, nextline);
		istringstream nl(nextline);
		nl >> nodes;

		cout << nodes << endl;
		//graphe non vide
		assert(nodes > 0);
		//double partie = length / 10.0 ; 
		cout << "file length " << (int)length << " bytes" << endl;

		creatMatrice(nodes);
		//double cumul = 0.0; 
		int pourcentage = 10;
		double pourcentage2;
		double pourcen_cum = 0;
		int i = 0;

		cout << "file length " << (int)length << " bytes" << endl;

		std::istringstream iss;
		std::vector<int> results;
		if (myfile.is_open())
		{
			std::cout << "File Loading ... " << std::endl;
			
			while (getline(myfile, line))
			{
			

				cout << line << std::endl;
				pourcen_cum += (((double)line.size()) / length) * 100;
				if (pourcen_cum >= 10.0)
				{

					cout << " --- " << pourcentage << "%";
					pourcentage += 10;
					pourcen_cum -= 10;
					i++;
				}

				if (line[0] != '#')
				{
					iss.clear();
					iss.str(line);
					results.clear();
					for (int n = 0; n < 2; n++)
					{
						int val;
						
						iss >> val;
						results.push_back(val);
					}
					
					matrice[results.at(0)][results.at(1)] = true;
				}
			}

			while (i != 10)
			{
				cout << " --- " << pourcentage << "%";
				pourcentage += 10;
				i++;
			}
			cout << endl;


			myfile.close();
			std::cout << "File Loaded. " << std::endl;


		}
		else
		{
			cerr << "File not opened" << endl;
		}

}


int DirectedGraph::getNumNodes()
{
	return graph->GetNodes();
}
int DirectedGraph::getNumEdges()
{
	return graph->GetEdges();
}

int DirectedGraph::getNodes()
{
	return nodes;
}
vector<boost::dynamic_bitset<>> DirectedGraph::getMatrix()
{
	return matrice;
}

TNGraph::TNodeI DirectedGraph::getNodeIteratorBegin()
{
	return graph->BegNI();
}
TNGraph::TNodeI DirectedGraph::getNodeIteratorEnd()
{
	return graph->EndNI();
}
TNGraph::TNodeI DirectedGraph::getNodeIteratorOf(int nodeID) {
	return graph->GetNI(nodeID);
}
TNGraph::TEdgeI DirectedGraph::getEdgeIteratorBegin() {
	return graph->BegEI();
}
TNGraph::TEdgeI DirectedGraph::getEdgeIteratorEnd() {
	return graph->EndEI();
}

bool DirectedGraph::edgeBetween(int node1, int node2)
{
	return graph->IsEdge(node1, node2);
}
bool DirectedGraph::emptyGraph()
{
	return graph->Empty();
}
void DirectedGraph::sortAdjencyLists()
{
	graph->SortNodeAdjV();
}

void DirectedGraph::saveGraph()
{
	ofstream myfile;
	string folder1 = "..\\..\\BVTest\\data\\";
	string folder2 = (this->fileName).substr(0, (this->fileName).find("."));
	
	int val = dirExists(folder1 + folder2);
	if (val != 0) {
		if (val == -1){ cerr << "Path Error ..." << endl; exit(0);}
		else { mkdir((folder1 + folder2).c_str()); }
	}

	myfile.open( folder1+folder2+"\\Saved_" + this->fileName+".txt", ios::out);
	string line;
	if (myfile.is_open())
	{
		ostringstream o;
		o << getNumNodes();
		myfile << o.str() + "\n";
		for (TNGraph::TNodeI NI = getNodeIteratorBegin(); NI < getNodeIteratorEnd(); NI++)
		{
			myfile << to_string(NI.GetId()) + " : " + getOutNeighbors(NI) + "\n";
		}

		myfile.close();
	}
	else {
		cerr << "File not opened" << endl;
	}

}





void DirectedGraph::addNodes(int node1, int node2)
{
	graph->AddEdge2(node1, node2);
}


int DirectedGraph::getNodeID(TNGraph::TNode myNode)
{
	return myNode.GetId();
}
int DirectedGraph::getInDegree(TNGraph::TNode myNode)
{
	return myNode.GetInDeg();
}
int DirectedGraph::getOutDegree(TNGraph::TNode myNode)
{
	return myNode.GetOutDeg();
}
int DirectedGraph::getDegree(TNGraph::TNode myNode)
{
	return myNode.GetDeg();
}
int DirectedGraph::getInNeighborAt(TNGraph::TNode myNode, int index)
{
	return myNode.GetInNId(index);
}
int DirectedGraph::getOutNeighborAt(TNGraph::TNode myNode, int index)
{
	return myNode.GetOutNId(index);
}
string DirectedGraph::getOutNeighbors(TNGraph::TNodeI myNode)
{
	ostringstream o;
	for (int i = 0; i < myNode.GetOutDeg(); i++) { 
		o << myNode.GetOutNId(i); 
		if (i != myNode.GetOutDeg() - 1) o << " ";
	}	
	return o.str();
}


vector<int> DirectedGraph::getOutNeighborsVector(TNGraph::TNodeI myNode)
{
	vector<int> Succ; 
	for (int i = 0; i < myNode.GetOutDeg(); i++) {
		Succ.push_back(myNode.GetOutNId(i));
	}
	return Succ;
}



bool DirectedGraph::isInNeighbor(TNGraph::TNode myNode, int nodeId)
{
	return myNode.IsInNId(nodeId);
}
bool DirectedGraph::isOutNeighbor(TNGraph::TNode myNode, int nodeId)
{
	return myNode.IsOutNId(nodeId);
}



void DirectedGraph::plotGraph()
{
	TIntStrH ID;
	for (TNGraph::TNodeI NI = getNodeIteratorBegin() ; NI < getNodeIteratorEnd(); NI++)
	{
		ID.AddDat(NI.GetId()) = boost::lexical_cast<string>(NI.GetId()).c_str();
	}
	
	TSnap::DrawGViz<PNGraph>(graph, gvlDot, (fileName + ".png").c_str(), "", ID);
}



DirectedGraph::~DirectedGraph()
{
}

