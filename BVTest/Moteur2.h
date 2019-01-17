#pragma once

#include <time.h>
#include <boost/program_options.hpp>
#include <iostream>
#include <string>
#include "SuperGraph.h"
#include "CompressAgregation.h"
#include "Snap.h"
#include "properties.hpp"
#include "Module.h"



class Moteur2 {


public:
	Moteur2() {}
	~Moteur2() {}

	static void CompressingGraph(string graphName,string data,string iteration) {
		string folder1 = "..\\BVTest\\data2\\";
		int agre = 0;
		int k = 0;
		string stag;
		

		string data2;
		
		if (data == "Neignberhod") { agre = 0; data2 = "Neignberhod"; }
		if (data == "Label") { agre = 1; data2 = "Label";
		}
		if (data == "Label+neiber") { agre = 2; data2 = "Label+neiber";
		}
		if (data == "Triangle") { agre = 3; data2 = "Triangle"; }
		if (data == "Triangle+label") { agre = 4; data2 = "Triangle+label"; }
		
		SuperGraph G(graphName+ "label");
		
		int nbnodeinit = G.GetNodes();
		int nblieninit = G.GetEdges();
		string folder2 = (G.fileName).substr(0, (G.fileName).find("."));
		double filesizeinit = G.fileSize;
	
		stringstream(iteration) >> k ;
		properties props;
		props.set_property("basename", graphName);
		int val = UndirectedGraph::dirExists(folder1 + folder2);
		if (val != 0) {
			if (val == -1) { cerr << "Path Error ..." << endl; exit(0); }
			else { mkdir((folder1 + folder2).c_str()); }
		}

		ofstream property_file((folder1+folder2+"\\"+graphName + data2+".properties").c_str());
		int nbNodesinit = G.GetNodes();
		//clock_t tStartg = clock();
		double executionTimeGlobale = 0;
		for (int i = 0; i < k; i++) {
			clock_t tStart = clock();

			CritereAgregation(agre, G);
			double executionTime = (double)(clock() - tStart) / CLOCKS_PER_SEC;
			executionTimeGlobale += executionTime;
			//props.set_property("iteration", utils::to_string(i));
			//cout << i << endl;
			
			props.set_property(utils::to_string(i) + "-" + "NbNodeInitial", utils::to_string(nbnodeinit));
			props.set_property(utils::to_string(i) + "-" + "NbLienInitial", utils::to_string(nblieninit));
			props.set_property(utils::to_string(i)+"-"+"executionTime_(s)", utils::to_string(executionTime));
			props.set_property(utils::to_string(i) + "-" + "nbrNode_", utils::to_string(G.GetNodes()));
			props.set_property(utils::to_string(i) + "-" + "nbrLink_", utils::to_string(G.GetEdges()));
			//props.set_property(utils::to_string(i) + "executionTime_(s)", utils::to_string(executionTime));
			double size= G.savedGraph(i,data2);
			props.set_property(utils::to_string(i) + "-" + "bitspernode", utils::to_string((double)(size *8)/ nbnodeinit));
			props.set_property(utils::to_string(i) + "-" + "bitsperedge", utils::to_string((double)(size * 8) / nblieninit));
			props.set_property( "taille initiale", utils::to_string((double)(filesizeinit ) ));
			props.set_property("taille finale", utils::to_string((double)(size )));
			double ratio = ((size) / filesizeinit) * 100;
			props.set_property(utils::to_string(i) + "-" + "compressionRatio_%", utils::to_string(ratio));
			//utils::to_string((double)(size) / (G.fileSize)) * 100);

			//cout << size << endl;
			if (G.GetNodes() == nbNodesinit) {
				props.set_property("the iteration stoped at", utils::to_string(i));
				 stag = utils::to_string(i);
				break;
			
			}
			else { nbNodesinit = G.GetNodes(); }

			
		}
		//double executionTimeglo = (double)(clock() - tStartg) / CLOCKS_PER_SEC;
		props.set_property( "executionTimeglobal_(s)", utils::to_string(executionTimeGlobale));
		props.store(property_file, "Agregation properties");
		property_file.close();
		//folder1 + folder2 + "\\" + graphName + data2 + ".properties"
		
	}

	static void CritereAgregation(int agre,SuperGraph &G) {
		CompresssAgregation O;
		switch (agre)
		{
		case 0: {
			//for (int i = 0; i < k; i++) {
				O.compressGraphByNeighberhood(G);
				
				//cout << k << endl;
			//}
		} break;

		case 1:
		{
			O.compressGraphByLabel(G);
		} break;

		case 2:
		{
			O.compressGraphByLabelNeighberhood(G);
		}
		break;

		case 3:
		{
			O.compressGraphByTriangle(G);
		}
		break;

		case 4:
		{
			O.compressGraphByTriangleLabel(G);
		}
		break;

		
		

		}
	}
	



};

