#include "stdafx.h"
#include "DynamicGraph.h"


DynamicGraph::DynamicGraph(PNGraph& G)
{
}


DynamicGraph::~DynamicGraph()
{
}


DynamicGraph::DynamicGraph(const char* fileName)
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

	//graphe non vide
	assert(nodes > 0);
	//double partie = length / 10.0 ; 
	cout << "file length " << (int)length << " bytes" << endl;
	//recuperer le nb des timestemps
	getline(myfile, nextline);
	istringstream nt(nextline);
	nt >> Timstemps;

	creatMatrice();
	//double cumul = 0.0; 
	int pourcentage = 10;
	double pourcentage2;
	double pourcen_cum = 0;
	int i = 0;

	cout << "file length " << (int)length << " bytes" << endl;

	std::istringstream iss;
	std::vector<int> results;
	long T=0;
	int iT=0;
	


	if (myfile.is_open())
	{
		std::cout << "File Loading ... " << std::endl;

		while (getline(myfile, line))
		{

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
				for (int n = 0; n < 3; n++)
				{
					int val;
					iss >> val;
					results.push_back(val);
				}
				if (T != results.at(2))
				{
					T = results.at(2);
					iT++;
					
					idT.push_back(T);
				}
				

				matrice[iT][results.at(0)][results.at(1)] = true;

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




	int DynamicGraph::getNodes()
	{
		return nodes;
	}
	int DynamicGraph::getTimstemps()
	{
		return Timstemps;
	}
	vector<vector<boost::dynamic_bitset<>>> DynamicGraph::getMatrix()
	{
		return matrice;
	}


