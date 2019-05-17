#include "stdafx.h"
#include "dsm.hpp"
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <iostream>
#include <algorithm>
#include "Snap.h"
#include "..\tree_util.hh"

using namespace kptree;

using namespace std;
using namespace boost;
using namespace boost::multi_index;


#define threadshold 0

struct key_tag {};
struct value_tag {};

typedef std::pair< unsigned int, unsigned int > element_t;

typedef multi_index_container<
	element_t,
	indexed_by<
	ordered_unique< member< element_t, unsigned int, &element_t::first > >,
	ordered_non_unique<  member< element_t, unsigned int, &element_t::second > > > >  dataset_t;

typedef dataset_t::nth_index<0>::type  key_index_t;
typedef dataset_t::nth_index<1>::type value_index_t;



void dsm::discoverSubs(DirectedGraph g, int numHash)
{
	map<unsigned int, std::vector<unsigned int>> list_of_clusters;

	vector<	pair<	vector<unsigned int>, vector<unsigned int>	>	>	list_of_subs;

	// 1. generate random hash function or random permutations 
	MinHash m(numHash, g.getNumNodes());
	/*
	* We add the edges (u,u) to the graph in order to detect
	* not only cliques but all types of dense subgraphs
	*/
	DirectedGraph G = g;
	for (int i = 0; i < G.getNumNodes(); i++)  
		if (!G.edgeBetween(i, i)) {
			G.addNodes(i, i);
			_hasEdgeNN[i] = FALSE;
		}

	// 2. fingerprints processing
	m.CalcHash(G);

	// 3. group nodes with same fingerprints into clusters
	list_of_clusters = m.getClusters(G.getNumNodes());


	// 4. foreach cluster build the tree 
	//    and retreive the S and C sets
	
	for (std::map<unsigned int, std::vector<unsigned int>>::iterator it = list_of_clusters.begin(); it != list_of_clusters.end(); it++) {
		
		// 4.1 for each node in the cluster we order its adjacency list based on frequency
		std::vector<unsigned int> cluster = it->second;
		map<unsigned int, vector<unsigned int>> adjacList;
		//	multimap	<node_id,frequency>

		if (cluster.size() > 1) {
			dataset_t nodeFreqDescOrder;
			// 4.1 build the tree
			// new PNGRaphto represent the tree that we want to add

			for (int i = 0; i < cluster.size(); i++) {
				// foreach node we search for its adjacency list

				std::vector<unsigned int> outneighbor = G.getOutNeighborsVector(cluster[i]);
				adjacList[cluster[i]] = outneighbor;

				// foreach node in the adjacency list we add
				// the current node to its label list
				// and evaluate the frequency of that node

				for (int j = 0; j < outneighbor.size(); j++) {
					auto& bykey = nodeFreqDescOrder.get<0>();
					auto x = bykey.find(outneighbor[j]);

					if (x == bykey.end()) nodeFreqDescOrder.insert(element_t(outneighbor[j], 1));
					else {
						// Increment of the frequency of the node 
						//updating the frequency of the out node
						nodeFreqDescOrder.modify(x, [](auto& p) {p.second = p.second + 1; });
					}
				}


			}

			//	auto it2 = frequency.begin();
			//	cout << "Number of element is :" << frequency.size()<<endl;
			//	for (auto it = nodeFreqDescOrder.get<1>().begin(); it != nodeFreqDescOrder.get<1>().end(); ++it)
			//	{
			//		std::cout << it->first << " : "<<it->second <<endl;
			//		//std::cout << it2->first << endl;
			//		//if (it2 != frequency.end()) it2++;
			//	}

			auto& table = nodeFreqDescOrder.get<1>();
			// sorting the adjacency List of the cluster' node
			unsigned int n1;
			for (map<unsigned int, vector<unsigned int>>::iterator i = adjacList.begin(); i != adjacList.end(); i++) {
				n1 = 0;

				for (auto it = table.rbegin(); it != table.rend() & i->second.size() > 1; it++) {

					auto it_tmp = std::find(i->second.begin(), i->second.end(), it->first);
					if (it_tmp != i->second.end()) {
						// sawping between element @ it_tmp  and element @ n1
						if (it->second <= 1) i->second.erase(it_tmp); // droping element with frequency 1
						else {
							int swap = i->second[n1];
							i->second[n1] = *it_tmp;
							*it_tmp = swap;
							n1++;
						}

					}
				}
			}

			//for (map<unsigned int, vector<unsigned int>>::iterator i = adjacList.begin(); i != adjacList.end(); i++) {
			//	cout << i->first << " :	";
			//	for (int k = 0; k < i->second.size(); k++) cout << i->second[k] << "		";
			//	cout << endl;
			//}

			//Building the tree
			tree<pair<unsigned int, vector<unsigned int>>> T;
			tree<pair<unsigned int, vector<unsigned int>>>::iterator root = T.insert(T.begin(), { adjacList.begin()->second[0],{ adjacList.begin()->first } });
			tree<pair<unsigned int, vector<unsigned int>>>::iterator prec = root;

			map<unsigned int, vector<unsigned int>>::iterator i;
			for ( i = adjacList.begin(); i != adjacList.end(); i++) {
				vector<unsigned int> v = i->second;
				
				//get @ of v[0] in the tree :)
				tree<pair<unsigned int, vector<unsigned int>>>::iterator current = find_element(T, root, v[0]);

				if (current == root.end()) break;
				for (int k = 1; k < v.size(); k++) {

					pair<bool, tree<pair<unsigned int, vector<unsigned int>>>::iterator> tmp = isChild(v[k], current);
					if (!tmp.first) { //Is not a child
						//we insert
						current = T.append_child(current, { v[k],{ i->first } });
					}
					else { //else we only add it->first to its label_List 
						tmp.second->second.push_back(i->first);
						current = tmp.second;
					}

				}

				//kptree::print_subtree_bracketed(T, root);
				//cout << endl << endl;
			}

			if (i != adjacList.end()) continue;
			//	Affichage de l'arbre en sortie
			// kptree::print_subtree_bracketed(T,root);

			// 4.2 retreive all S and C
			vector<pair<vector<unsigned int>, vector<unsigned int>>> S_C = retreiveS_C(T);

			// 4.3 Evaluate the patterns that were found and choose the best
			// saving = (P.freq - 1) (P.size - 1) -1
			// saving = (|S| - 1)(|C| - 1) -1
			vector<pair<vector<unsigned int>, vector<unsigned int>>> bestS_C ;
			dataset_t  savings;
			for (int h = 0; h < S_C.size(); h++)
				savings.insert(element_t( h, ( S_C[h].first.size() - 1)*(S_C[h].second.size() - 1) - 1 ));

			for (auto it = savings.get<1>().rbegin(); it != savings.get<1>().rend(); it++) {
				if (it->second >= threadshold) { 
					bestS_C.push_back(S_C[it->first]); 
				}
				else break;
			}

			// 4.4 append the best S and C to subslist
			append(bestS_C,G);
			
		}
		}
		

		if (CompressError) {
			k2_Trees arbre(2, G.getNumNodes(), G);
			ErrorCompressed = arbre;
		}
		
		//printX_B();

}

void dsm::append(vector<pair<vector<unsigned int>, vector<unsigned int>>> bestS_C,DirectedGraph G) {

	for (auto elem : bestS_C) {
		vector<unsigned int> L;
		vector<unsigned int> M;
		vector<unsigned int> R;


		std::sort(elem.first.begin(), elem.first.end());
		std::sort(elem.second.begin(), elem.second.end());

		S_C_list.push_back(elem);

		std::set_difference(elem.first.begin(), elem.first.end(), elem.second.begin(), elem.second.end(), std::inserter(L, L.begin()));
		std::set_intersection(elem.second.begin(), elem.second.end(), elem.first.begin(), elem.first.end(), std::inserter(M, M.begin()));
		std::set_difference(elem.second.begin(), elem.second.end(), elem.first.begin(), elem.first.end(), std::inserter(R, R.begin()));

		B.push_back(true);
		for (auto a : L) {
			X.push_back(a);
			B.push_back(false);
			// We update the Graph of errors
			if (CompressError) {
				for (auto b : M) {
					if (G.edgeBetween(a, b)) G.DelEdge(a, b);
					else G.addedge2(a, b);
				}
				for (auto b : R) {
					if (G.edgeBetween(a, b)) G.DelEdge(a, b);
					else G.addedge2(a, b);
				}

			}
		}

		B.push_back(true);
		for (auto a : M) {
			X.push_back(a);
			B.push_back(false);
			if (CompressError) {
				for (auto b : R) {
					if (G.edgeBetween(a, b)) G.DelEdge(a, b);
					else G.addedge2(a, b);
				}
			}
		}

		B.push_back(true);
		for (auto a : R) {
			X.push_back(a);
			B.push_back(false);
		}

	}
}


vector<pair<vector<unsigned int>, vector<unsigned int>>> dsm::retreiveS_C(tree<pair<unsigned int, vector<unsigned int>>> T ) {
	tree<pair<unsigned int, vector<unsigned int>>>::leaf_iterator begin = T.begin_leaf();
	tree<pair<unsigned int, vector<unsigned int>>>::leaf_iterator end = T.end_leaf();
	vector<pair<vector<unsigned int>, vector<unsigned int>>> S_C;
	vector<unsigned int> S;
	vector<unsigned int> C;
	while (begin != end) {
		S = begin->second;
		//construction of C from the leaf back to the root
		tree<pair<unsigned int, vector<unsigned int>>>::iterator tmp = begin;
		while (T.is_valid(tmp)) {
			C.push_back(tmp->first);
			tmp = T.parent(tmp);
		}
		//cout << "S: ";
		//for (auto j : S) cout << j << "	";
		//cout << endl;
		//cout << "C: ";
		//for (auto j : C) cout << j << "	";
		//cout << endl << endl;

		S_C.push_back({ S,C });
		begin++;
		C.clear();
	}
	return S_C;
}
	
void dsm::printX_B() {
	cout << "X	:";
	for (auto elem : X) cout << elem << "	";

	cout << endl<< "B	:";
	for (int i = 0; i < B.size();i++) cout << B[i] << "	";
	cout << endl << endl;

	cout << X.size() + B.size() << endl;
}


dsm::dsm()
{
}

dsm::dsm(DirectedGraph g, int numHash,bool compressError):
	CompressError(compressError)
{
	clock_t tStart = clock();
	for (int i = 0; i < g.getNumNodes(); i++) _hasEdgeNN.push_back(TRUE);
	discoverSubs(g, numHash);
	tExec = (double)(clock() - tStart) / CLOCKS_PER_SEC;
	saveX_B(g.getFileName());
}


void dsm::saveX_B(string filename)
{
	ofstream outFile;
	string folder1 = "..\\..\\BVTest\\data\\";
	outFile.open(folder1+"DSM_out_"+filename+".txt",'w');
	for (int i = 0; i < X.size();i++) outFile<<X[i];
	outFile << endl <<endl;
	for (int i = 0; i < B.size(); i++) outFile << B[i];
	if (CompressError) {
		outFile << endl << endl;

		outFile << "##The k2-tree representation of the error" << endl;
		outFile << endl << endl;
		for (int i = 0; i < ErrorCompressed.get_T().size(); i++) outFile << ErrorCompressed.get_T()[i];
		outFile << endl << endl;
		for (int i = 0; i < ErrorCompressed.get_L().size(); i++) outFile << ErrorCompressed.get_L()[i];

	}



	outFile.close();
}

void dsm::updateError(DirectedGraph G, pair<vector<unsigned int>, vector<unsigned int>> S_C)
{


}

dsm::~dsm()
{

}
