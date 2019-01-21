#pragma once
#include <iostream>
#include <string>
#include "DirectedGraph.h"
#include "gcupm.h"
class motor_pattern_extraction
{
public:
	motor_pattern_extraction();
	~motor_pattern_extraction();

	void compressGraph(const char* graphName, string type, int patternSize, int modelType)
	{
		if (type == "GCUPM") compressGraphUPM(graphName, patternSize, modelType);
	};

	void compressGraphUPM(const char* graphName, int patternSize, int modelType) {
		DirectedGraph graph(graphName);

		gcupm compressor(graph.getMatrix(), patternSize, modelType);

		std::vector<boost::dynamic_bitset<> > Comp = compressor.get_Result();
		for (boost::dynamic_bitset<>::size_type i = 0; i < Comp.size(); i++) {
			for (boost::dynamic_bitset<>::size_type j = 0; j < Comp[i].size(); j++)
				cout << Comp[i][j] << " ";
			cout << " \n"<<endl;
		}
	};
};

