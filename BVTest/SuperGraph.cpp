#include "stdafx.h"
#include "SuperGraph.h"
#include <boost/lexical_cast.hpp>


//constructeur qui construit un graphe à partir d'un fichier 

SuperGraph::SuperGraph(string fileName)

{
	this->fileName = fileName;
	graph = TNodeNetModule<TStr>::New();
	string line;
	fstream myfile;

	//("..\\BVTest\\data\\" + this->fileName + ".txt").c_str();
	myfile.open(("..\\BVTest\\data2\\"+ fileName+".txt").c_str(), ios::in);

	//std::istringstream iss;
	//std::vector<int> results;
	if (myfile.is_open()) {
		myfile.seekg(0, myfile.end);
		fileSize = myfile.tellg();
		myfile.seekg(0, myfile.beg);

		while (getline(myfile, line))
		{
			vector<string> x = split(line);
			if (x[0] == "v") { // New node
				
				graph->AddNode(atoi(x[1].c_str()));
				graph->SetNDat(atoi(x[1].c_str()), x[2].c_str());
				//graph->GetNI(atoi(x[1].c_str())).GetType() = 'X';7
				graph->GetNI(atoi(x[1].c_str())).GetType() = 'X';
			}
			if (x[0] == "e") { // New edge
				graph->AddEdge(atoi(x[1].c_str()), atoi(x[2].c_str()));
			}
		}
		myfile.close();
		std::cout << "File Loaded" << std::endl;
	}
	else
	{
		cerr << "File not opened" << endl;
	}
}





vector<string> SuperGraph::split(string line)
{
	vector<string> elems;
	string word;
	for (int i = 0; i < line.length(); i++) {
		if (line[i] != ' ' && line[i] != '\t') { // Define word separator by ' ' or '\t'
			word += line[i];
		}
		else {
			elems.push_back(word);
			word.clear();
		}
	}
	elems.push_back(word);
	return elems;
}



TNodeNetModule<TStr>::TNodeI SuperGraph::getNodeIteratorBegin() {

	return graph->BegNI();
}
TNodeNetModule<TStr>::TNodeI SuperGraph::getNodeIteratorEnd() {
	return graph->EndNI();
}
TNodeNetModule<TStr>::TNodeI SuperGraph::getNode(int node) {
	return graph->GetNI(node);
}
TStr SuperGraph::GetNDat(int node) {
	return graph->GetNDat(node);
}

//fonction qui convertit un module et l'ecrit en string
string SuperGraph::GetModuleContent(TNodeNetModule<TStr>::TNodeI node)
{
	TIntV NIdV;
	node.GetModuleContent()->GetNIdV(NIdV);
	string nodes = "( ";
	for (int i = 0; i < NIdV.Len(); i++)
	{
		nodes += to_string(NIdV[i]);
		if (i != NIdV.Len() - 1) nodes += " , ";
	}
	nodes += ")";
	return nodes;

}



void SuperGraph::plotGraph()
{
	TIntStrH ID;

	for (TNodeNetModule<TStr>::TNodeI NI = getNodeIteratorBegin(); NI < getNodeIteratorEnd(); NI++)
	{
		ID.AddDat(NI.GetId()) = boost::lexical_cast<string>(NI.GetId()).c_str();
	}

	TSnap::DrawGViz <TPt<TNodeNetModule<TStr> >>(graph, gvlDot, (fileName + ".png").c_str(), "", ID);
}

SuperGraph::~SuperGraph()
{
}
