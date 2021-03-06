#include "stdafx.h"
#include "MoteurOrdre.h"
#include "SuperGraph.h"
#include "Moteur2.h"
#include "motorpar.h"
#include "CompressAgregation.h"
#include <map>
#include "TextTable.h"
#include <boost/thread.hpp>
#include "motorK2_trees.h"
#include "motor_pattern_extraction.h"
#include "DDSM/DDSM.h"

void testExtractPattern(string graphName, string choix, int argc, char * argv[])
{
	motor_pattern_extraction motor;
	motor.compressGraph(graphName.c_str(), choix, argc, argv);
}

void testk2(string graphName, string choix, int k ,bool isDirected)
{
	motorK2_trees motor;
	const char * c = graphName.c_str();
	motor.compressGraph(c, k, choix, isDirected);
}

void testk2WithOrder(string graphName, int k,int ordre)
{
	DirectedGraph  graph(graphName.c_str(), 1);
	
	Order o(graph);
	vector <int> order;
	
	switch (ordre)
	{
	case 0:
	{
		order = o.BfsOrder(graph);
	} break;

	case 1:
	{
		order = o.DfsOrder(graph);
	}
	break;

	case 2:
	{
		order = o.GrayOrder(graph);
	}
	break;

	case 3:
	{
		order = o.lexicographicalOrder(graph);
	}
	break;

	case 4:
	{
		order = o.RandomOrder(graph);
	}
	break;

	}

	

	k2_Trees tree(k, graph.getNumNodes(), graph,order);
	std::fixed;

	//cout << "T = " << tree.get_T() << endl;
	//cout << "L = " << tree.get_L() << endl;
	//cout << "Le ratio de Compression est de :" << (graph.getNumNodes()*graph.getNumNodes()) / (tree.get_T().size() + tree.get_L().size()) << endl;
	//cout << "Le gain d'espace :" << (double)100 * (1 - ((tree.get_T().size() + tree.get_L().size()) / (graph.getNodes()*graph.getNodes()))) << "%" << endl;
	//cout << "Le nombre de bit par noeuds :" << (double)(tree.get_T().size() + tree.get_L().size()) / graph.getNumNodes() << endl;
	//cout << "Le temps de Compression est de :" << (double)tree.get_Time() << endl;
	
	cout <<  (graph.getNumNodes()*graph.getNumNodes()) / (tree.get_T().size() + tree.get_L().size()) << endl;
	//cout << "Le gain d'espace :" << (double)100 * (1 - ((tree.get_T().size() + tree.get_L().size()) / (graph.getNodes()*graph.getNodes()))) << "%" << endl;
	cout << (double)(tree.get_T().size() + tree.get_L().size()) / graph.getNumNodes() << endl;
	cout << (double)tree.get_Time() << endl;

}

void test3(string graphName1, string data1, string iteration1) {

	Moteur2 motr;
	motr.CompressingGraph(graphName1, data1, iteration1);
}

void testbv (string graphName, string data, int window_size, int max_ref_count, int min_interval_length, int zeta_k)
{
	motorpar moteur;
	
	moteur.CompressGraph(graphName, data, window_size, max_ref_count, min_interval_length, zeta_k);
}

void testOrdre(string graphName)
{

	MoteurOrdre::orderGraph(graphName);
	
}


int main(int argc, char ** argv) {


	using namespace boost;
	string graphName1;
	string data1;
	string iteration1;




	int C;
	stringstream(argv[1]) >> C;

	switch (C)
	{
	case 0: {
	testbv(argv[2], argv[3], atoi(argv[4]), atoi(argv[5]), atoi(argv[6]), atoi(argv[7]));

	} break;

	case 1:
	{
	test3(argv[2], argv[3], argv[4]);
	} break;
	case 2:
	{
	testOrdre(argv[2]);
	} break;
	case 3:
	{
	testk2(argv[2], argv[3], atoi(argv[4]), atoi(argv[5]));
	}
	break;
	case 4:
	{
	testExtractPattern(argv[2], argv[3], argc-3, &(argv[3]));
	}
	break;
	case 5: {
	testk2WithOrder(argv[2], atoi(argv[4]), atoi(argv[5]));
	}break;
	case 6:
	{
		testExtractPattern(argv[2], argv[3], argc - 3, &(argv[3]));
	}break;
	case 7: {

		DDSM d(string(argv[2]),atoi(argv[4]));
	}break;
	}

	
	
	
	
	
	

	return 0;
}