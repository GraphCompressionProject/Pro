#pragma once
#include <time.h>
#include <boost/program_options.hpp>
#include <iostream>
#include <string>
#include "DirectedGraph.h"
#include"Order.h"
#include "Snap.h"
#include "properties.hpp"
#include "Module.h"
#include "webgraph.hpp"


using namespace std;
using namespace webgraph;
namespace bvg = webgraph::bv_graph;

class motorpar {


public:
	motorpar() {}
	~motorpar() {}

	static void CompressGraph(string graphName,string data,int window_size,int max_ref_count,int min_interval_length, int zeta_k)
	{


		int flags = 1, order = 0;



		

		string destinationName="CPR";
		
		if (data == "Initial") order = 0;
		if (data == "BFS") order = 1;
		if (data == "DFS") order = 2;
		if (data == "GRAY") order = 3;
		if (data == "LEXICAL") order = 4;
		if (data == "RANDOM") order = 5;

		
		if (max_ref_count == -1) max_ref_count = bvg::graph::DEFAULT_MAX_REF_COUNT;
		

		cout << graphName << " " << destinationName << " " << order << " " << window_size << " " << max_ref_count << " " << min_interval_length << " " << zeta_k << endl;
		cout << "here" << endl;
		compressAllOrders(graphName, destinationName, window_size, max_ref_count, min_interval_length, zeta_k, flags, order);

	}

	static void orderGraph(string graphName)
	{
		clock_t tStart;
		double time;

		DirectedGraph G = DirectedGraph(graphName.c_str(), 1);

		G.saveGraph();

		Order o = Order(G);
		webgraph::properties props;

		tStart = clock();
		o.RandomOrder(G);
		time = (double)(clock() - tStart) / CLOCKS_PER_SEC;
		props.set_property("RandomOrder_time", utils::to_string(time));

		tStart = clock();
		o.GrayOrder(G);
		time = (double)(clock() - tStart) / CLOCKS_PER_SEC;
		props.set_property("GrayOrder_time", utils::to_string(time));

		tStart = clock();
		o.lexicographicalOrder(G);
		time = (double)(clock() - tStart) / CLOCKS_PER_SEC;
		props.set_property("LexicographicalOrder_time", utils::to_string(time));

		tStart = clock();
		o.DfsOrder(G);
		time = (double)(clock() - tStart) / CLOCKS_PER_SEC;
		props.set_property("DfsOrder_time", utils::to_string(time));

		tStart = clock();
		o.BfsOrder(G);
		time = (double)(clock() - tStart) / CLOCKS_PER_SEC;
		props.set_property("BfsOrder_time", utils::to_string(time));

		ofstream property_file(("..\\BVTest\\data\\" + graphName + "\\" + "OrderLog_" + graphName + ".properties").c_str());
		props.store(property_file, "Order properties");

		property_file.close();

	}

	static void compressAllOrders(const string basename, const string prefix_compress, int window_size, int max_ref_count, int min_interval_length, int zeta_k, int flags, int ordre)
	{
		switch (ordre)
		{
		case 0: {
			compressGraphByOrder(basename, prefix_compress, "Saved_", window_size, max_ref_count, min_interval_length, zeta_k, flags);
		} break;

		case 1:
		{
			compressGraphByOrder(basename, prefix_compress, "BfsOrder_", window_size, max_ref_count, min_interval_length, zeta_k, flags);
		} break;

		case 2:
		{
			compressGraphByOrder(basename, prefix_compress, "DfsOrder_", window_size, max_ref_count, min_interval_length, zeta_k, flags);
		}
		break;

		case 3:
		{
			compressGraphByOrder(basename, prefix_compress, "GrayOrder_", window_size, max_ref_count, min_interval_length, zeta_k, flags);
		}
		break;

		case 4:
		{
			compressGraphByOrder(basename, prefix_compress, "lexicographicalOrder_", window_size, max_ref_count, min_interval_length, zeta_k, flags);
		}
		break;

		case 5:
		{
			compressGraphByOrder(basename, prefix_compress, "RandomOrder_", window_size, max_ref_count, min_interval_length, zeta_k, flags);
		}
		break;

		}
	}

	static void compressGraphByOrder(const string basename, const string prefix_compress, string prefix_order, int window_size, int max_ref_count, int min_interval_length, int zeta_k, int flags) {

		double fileSize;
		namespace ag = webgraph::ascii_graph;
		ag::offline_graph graph = ag::offline_graph::load(prefix_order, basename, fileSize);

		namespace bvg = webgraph::bv_graph;
		ostream* log = &cerr;
		if (basename != "") {
			bvg::graph::store_offline_graph(graph, fileSize, prefix_compress, prefix_order, basename, window_size, max_ref_count, min_interval_length, zeta_k, flags, log);
		}
	}

};