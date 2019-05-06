#pragma once




#include <iostream> 
#include <iterator> 
#include <map> 
#include <string>
#include<boost/lexical_cast.hpp>

using namespace std;

#include "..\DynamicGraph.h"
#include "..\DSM\dsm.hpp"
#include "temporalMinHash.h"


class DDSM
{
private:
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
			cout << it.first << endl;
			//for (auto i = it.second.BegEI(); i != it.second.EndEI();i++) {
			//	cout << "(" << i.GetSrcNId() << "," << i.GetDstNId() << ")" << endl;
			//}

			dsm d(it.second, numhash);
			d.printX_B();
			
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
		



	};

	/*
	function that give the appropriatelabel according to the 
	analysis of the timestemps where it appears

	Input :
		-	vector of timestemps 
		-	Number of timestemps in the hole graph

	Output :
		-	Temporal signture of the structure
	
	*/
	string getTmpLabel(vector<unsigned int> timestemps,int T) {
		int size = timestemps.size();
		if ( size == T) return string("c");
		else if (size == 1) return string("o" + timestemps[0]);
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
	}

	pair <bool, bool> rangedPeriodic(vector<unsigned int> timestemps) {
		bool ranged = true;
		bool periodic = true;
		unsigned int period = timestemps[0] - timestemps[1];
		for (int i = 0; i < timestemps.size() && (ranged || periodic); i++) {
			if (timestemps[i + 1] - timestemps[i] > 1) ranged = false;
			else if (timestemps[i + 1] - timestemps[i] != period) periodic = false;
		}
		return { ranged,periodic };
	}

	~DDSM()
	{
	};
};

