#pragma once




#include <iostream> 
#include <iterator> 

#include <algorithm>

#include <map> 
#include <string>
#include<boost/lexical_cast.hpp>

using namespace std;

#include "..\DynamicGraph.h"
#include "..\DSM\dsm.hpp"
#include "temporalMinHash.h"
#include "..\Ik2_trees.hpp"

class DDSM
{
private:
	std::vector<unsigned int > X;
	boost::dynamic_bitset<> B;
	Ik2_Trees Error;
	DynamicGraph graph;
	map<unsigned int, pair<unsigned int,pair<vector<unsigned int>,vector<unsigned int>>>> compressedVersion;
	
public:
	DDSM()
	{
	};

	DDSM(string filename, unsigned int numhash)
	{
		// reading the initial graphs
		graph.init(filename);
		unsigned int nbsubs = 0;
		
		//compressing the graph using the minhash
		for ( auto& it : graph.getGraph()) {
			//cout << it.first << endl;
			//for (auto i = it.second.BegEI(); i != it.second.EndEI();i++) {
			//	cout << "(" << i.GetSrcNId() << "," << i.GetDstNId() << ")" << endl;
			//}

			dsm d(it.second, numhash,false);
			//d.printX_B();

			vector<pair<vector<unsigned int>, vector<unsigned int>>> S_C_list = d.getSubList();
			for (auto& it1 : S_C_list) {
				compressedVersion.insert({ nbsubs, {it.first,{it1.first,it1.second}} });
				nbsubs++;
			}
		}

		map<unsigned int, vector<unsigned int>> sets;

		for (auto& it : compressedVersion) {
			std::sort(it.second.second.first.begin(),it.second.second.first.end());
			std::sort(it.second.second.second.begin(), it.second.second.second.end());
			vector<unsigned int> tmp(it.second.second.first.size() + it.second.second.second.size());
			
			vector<unsigned int> tmp1 = it.second.second.first;
			vector<unsigned int> tmp2 = it.second.second.second;

			std::set_union(tmp1.begin(), tmp1.end(),tmp2.begin(), tmp2.end(),tmp.begin());

			//for (int i = 0; i < tmp.size(); i++) cout << tmp[i];
			//cout << endl;
			sets.insert({it.first,tmp});
		}
		
		// stitching the discovered graphs
		// for that we use the minhash to get 
		// similar sets from different timestemps

		map<unsigned int, std::vector<unsigned int>> list_of_clusters;


		// 1. generate random hash function or random permutations 
		temporalMinHash m(numhash, compressedVersion.size());

		// 2. fingerprints processing
		m.CalcHash(sets);

		// 3. group sets with same fingerprints into clusters
		list_of_clusters = m.getCluster(sets.size());
		
		
		for (std::map<unsigned int, std::vector<unsigned int>>::iterator it = list_of_clusters.begin(); it != list_of_clusters.end(); it++) {
			//cout << "Cluster Number " << it->first << endl;
			vector<unsigned int> stitchLabel;
			vector<unsigned int> S ;
			vector<unsigned int> C ;
			
			for (int j = 0; j < it->second.size(); j++) {
				unsigned int h = compressedVersion.find(it->second[j])->second.first;
				stitchLabel.push_back(h);
				vector<unsigned int> S_tmp = compressedVersion.find(it->second[j])->second.second.first;
				vector<unsigned int> C_tmp = compressedVersion.find(it->second[j])->second.second.second;
				
				std::set_union(S_tmp.begin(),S_tmp.end(), S.begin(), S.end(), std::back_inserter(S));
				std::set_union(S_tmp.begin(),S_tmp.end(), S.begin(), S.end(), std::back_inserter(C));
				
			}
			
			std::vector<unsigned int>::iterator it3;
			 std::sort(S.begin(), S.end());   
			it3 = std::unique(S.begin(), S.end());   
			S.resize(std::distance(S.begin(), it3));

			std::vector<unsigned int>::iterator it2;
			std::sort(C.begin(), C.end());  
			it2 = std::unique(C.begin(), C.end());  
			C.resize(std::distance(C.begin(), it2));


			append(S, C, stitchLabel);

			
		}

		Ik2_Trees arbre(2, graph.getNodes(), true, graph);
		Error = arbre;

		printX_B();
		
		saveX_B(graph.getFileName());
	};

	void printX_B() {
		cout << "X	:";
		for (auto elem : X) cout << elem << "	";

		cout << endl << "B	:";
		for (int i = 0; i < B.size(); i++) cout << B[i] << "	";
		cout << endl << endl;

		cout << X.size() + B.size() +Error.get_L().size()+Error.get_T().size() << endl;
	}

	void saveX_B(string filename)
	{
		ofstream outFile;
		string folder1 = "..\\..\\BVTest\\data\\";
		outFile.open(folder1 + "DDSM_out_" + filename + ".txt", 'w');
		for (int i = 0; i < X.size(); i++) outFile << X[i];
		outFile << endl << endl;
		for (int i = 0; i < B.size(); i++) outFile << B[i];
		
			outFile << endl << endl;

			outFile << "##The k2-tree representation of the error" << endl;
			outFile << endl << endl;
			for (int i = 0; i < Error.get_T().size(); i++) outFile << Error.get_T()[i];
			outFile << endl << endl;
			for (int i = 0; i < Error.get_L().size(); i++) outFile << Error.get_L()[i];

		



		outFile.close();
	}

	void append(vector<unsigned int> S, vector<unsigned int> C, vector<unsigned int> tmp) {
		vector<unsigned int> tmpDEscription = getTmpLabel(tmp, graph.getGraph().size());

		vector<unsigned int> L;
		vector<unsigned int> M;
		vector<unsigned int> R;

		std::set_difference(S.begin(),S.end(),C.begin(), C.end(), std::inserter(L, L.begin()));
		std::set_intersection(S.begin(), S.end(), C.begin(), C.end(), std::inserter(M, M.begin()));
		std::set_difference(C.begin(), C.end(), S.begin(),S.end(), std::inserter(R, R.begin()));

		//nodes with out edges only
		B.push_back(true);
		for (auto a : L) {
			X.push_back(a);
			B.push_back(false);
			// We update the Graph of errors
			
			for (auto b : tmp) {
				for (auto c : M) {
					if (graph.isEdge(b,a,c)) graph.DelEdge(b,a,c);
					else graph.addEdge2(b, a, c);
				}
				for (auto c : R) {
					if (graph.isEdge(b, a, c)) graph.DelEdge(b, a, c);
					else graph.addEdge2(b, a, c);
				}
			}
			

		}


		//nodes with in and out neighbors
		B.push_back(true);
		for (auto a : M) {
			X.push_back(a);
			B.push_back(false);
			for (auto b : tmp) {
				for (auto c : M) {
					if (graph.isEdge(b, a, c)) graph.DelEdge(b, a, c);
					else graph.addEdge2(b, a, c);
				}
				for (auto c : R) {
					if (graph.isEdge(b, a, c)) graph.DelEdge(b, a, c);
					else graph.addEdge2(b, a, c);
				}
			}
		}


		//nodes with in edges only
		B.push_back(true);
		for (auto a : R) {
			X.push_back(a);
			B.push_back(false);
		}

		//Saving the error
		B.push_back(true);
		for (auto a : tmpDEscription) {
			X.push_back(a);
			B.push_back(false);
		}
	}

	/*
	function that give the appropriatelabel according to the 
	analysis of the timestemps where it appears

	Input :
		-	vector of timestemps 
		-	Number of timestemps in the hole graph

	Output :
		-	Temporal signture of the structure
	
	
	string getTmpLabel(vector<unsigned int> timestemps,int T) {
		int size = timestemps.size();
		if ( size == T) return string("c");
		else if (size == 1) return string("o" + boost::lexical_cast<std::string>(timestemps[0]));
		else {
			pair<bool, bool> rangedOrperidic= rangedPeriodic(timestemps);
			if (rangedOrperidic.first) return (string("r")+ boost::lexical_cast<std::string>(timestemps[0]) + boost::lexical_cast<std::string>(timestemps[timestemps.size()-1]));
			else if (rangedOrperidic.second) return string("p"+timestemps[1]-timestemps[0]);
			else {

				string result;
				for (int i = 0; i < timestemps.size(); i++) {
					result += boost::lexical_cast<std::string>(timestemps[i]);
				}
				return string("f") + result;
			}
		}
	}*/

	vector<unsigned int> getTmpLabel(vector<unsigned int> timestemps, int T) {
		vector<unsigned int> tmpDescriptor;
		int size = timestemps.size();
		if (size == T) tmpDescriptor.push_back(0); //Constant
		else if (size == 1) {
			//onshot
			tmpDescriptor.push_back(1);
			tmpDescriptor.push_back(timestemps[0]);
		}
		else {
			pair<bool, bool> rangedOrperidic = rangedPeriodic(timestemps);
			if (rangedOrperidic.first) {
				//ranged
				tmpDescriptor.push_back(2);
				tmpDescriptor.push_back(timestemps[0]);
				tmpDescriptor.push_back(timestemps[timestemps.size() - 1]);

			}
			else if (rangedOrperidic.second) {
				//periodic
				tmpDescriptor.push_back(3);
				tmpDescriptor.push_back(timestemps[1] - timestemps[0]);
			} 
			else {
				//flickering
				tmpDescriptor.push_back(4);
				string result;
				for (int i = 0; i < timestemps.size(); i++) {
					tmpDescriptor.push_back(timestemps[i]);
				}
			}
		}

		return tmpDescriptor;
	}

	pair <bool, bool> rangedPeriodic(vector<unsigned int> timestemps) {
		bool ranged = true;
		bool periodic = true;
		unsigned int period = timestemps[0] - timestemps[1];
		for (int i = 0; i < timestemps.size()-1 && (ranged || periodic); i++) {
			if (timestemps[i]<timestemps[0] || timestemps[i] > timestemps[timestemps.size()-1]) ranged = false;
			else if (timestemps[i + 1] - timestemps[i] != period) periodic = false;
		}
		return { ranged,periodic };
	}

	~DDSM()
	{
	};
};

