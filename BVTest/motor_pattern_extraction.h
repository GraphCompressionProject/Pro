#pragma once
#include <iostream>
#include <string>
#include "DirectedGraph.h"
#include "gcupm.h"
#include "subdue\Subdue.hpp"
#include "DSM\dsm.hpp"

class motor_pattern_extraction
{
public:
	motor_pattern_extraction();
	~motor_pattern_extraction();

	void compressGraph(const char* graphName, string type, int argc, char * argv[])
	{
		if (type == "GCUPM") compressGraphUPM(graphName, atoi(argv[1]), atoi(argv[2]));
		if (type == "Subdue") {
			
			Subdue sbd;
			sbd.main(argc, argv);

		}
		if (type == "DSM") compressGraph(graphName, atoi(argv[1]));
	};

	void compressGraph(const char * graphName, int numHash) {
		DirectedGraph graph(graphName, 1);
		dsm d(graph,numHash);
	}

	void compressGraph(int argc, char * argv[])
	{
		Subdue sbd;

		sbd.main(argc, argv);
	};


	void compressGraphUPM(const char* graphName, int patternSize, int modelType) {
		DirectedGraph graph(graphName);
	
		gcupm compressor(graph.getMatrix(), patternSize, modelType);

		/*std::vector<boost::dynamic_bitset<> > Comp = compressor.get_Result();
		for (boost::dynamic_bitset<>::size_type i = 0; i < Comp.size(); i++) {
				cout << Comp[i] << endl;
		}*/

		cout <<endl<< "========================================================" << endl;
		std::fixed;

		cout << "Le ratio de Compression est de :" << (graph.getNodes()*graph.getNodes()) / compressor.get_size() << endl;
		cout << "Le nombre de bit par noeuds :" << (double)(compressor.get_size()) / graph.getNodes() << endl;
		cout << "Le temps de Compression est de :" << (double)compressor.get_Time() << endl << endl;
	};
};

