#include "stdafx.h"
#include "UndirectedGraph.h"



UndirectedGraph::UndirectedGraph(const char* fileName)
{
	TFIn FIn(fileName);
	PUNGraph G = TUNGraph::Load(FIn);
	graph = G;
}

UndirectedGraph::UndirectedGraph(string fileName)
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


			//cout << line << std::endl;
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

				if ( results.at(0) <= results.at(1) ) matrice[results.at(0)][results.at(1)] = true;
				else matrice[results.at(1)][results.at(0)] = true;
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




UndirectedGraph::UndirectedGraph(PUNGraph& G)
{
	graph = G;
}

int UndirectedGraph::getNumNodes()
{
	return graph->GetNodes();
}
int UndirectedGraph::getNumEdges()
{
	return graph->GetEdges();
}



int UndirectedGraph::getNodes()
{
	return nodes;
}
vector<boost::dynamic_bitset<>> UndirectedGraph::getMatrix()
{
	return matrice;
}

vector<listAdjacence> UndirectedGraph::getAdjList()
{
	return liste;
}

TUNGraph::TNodeI UndirectedGraph::getNodeIteratorBegin()
{
	return graph->BegNI();
}
TUNGraph::TNodeI UndirectedGraph::getNodeIteratorEnd()
{
	return graph->EndNI();
}
TUNGraph::TEdgeI UndirectedGraph::getEdgeIteratorBegin() {
	return graph->BegEI();
}
TUNGraph::TEdgeI UndirectedGraph::getEdgeIteratorEnd() {
	return graph->EndEI();
}

bool UndirectedGraph::edgeBetween(int node1, int node2)
{
	return graph->IsEdge(node1, node2);
}
bool UndirectedGraph::emptyGraph()
{
	return graph->Empty();
}
void UndirectedGraph::sortAdjencyLists()
{
	graph->SortNodeAdjV();
}



int UndirectedGraph::getNodeID(TUNGraph::TNode myNode)
{
	return myNode.GetId();
}
int UndirectedGraph::getDegree(TUNGraph::TNode myNode)
{
	return myNode.GetDeg();
}
int UndirectedGraph::getSuccessorIdAt(TUNGraph::TNode myNode, int index)
{
	return myNode.GetNbrNId(index);
}
bool UndirectedGraph::isSuccessor(TUNGraph::TNode myNode, int index)
{
	return myNode.IsNbrNId(index);
}

UndirectedGraph::~UndirectedGraph()
{

}
