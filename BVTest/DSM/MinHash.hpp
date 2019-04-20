



//
//  MinHash.hpp
//  MinHash
//
//  Created by Macbook on 29/03/2019.
//  Copyright © 2019 Macbook. All rights reserved.
//

#include <iostream>
#include <vector>
#include <map>

#include <numeric>


/* srand example */

#include "..\listAdjacence.h"
#include "..\DirectedGraph.h"
#ifndef MinHash_hpp
#define MinHash_hpp
//#define MX_INT 2147483647
//#define MX_INT 6700417
#define MX_INT 2147483647; // = 2^31 - 1 !

using namespace std;


#include <iostream>
#include <algorithm>    // std::shuffle
#include <vector>       // std::vector
#include <random>       // std::default_random_engine
#include <chrono>       // std::chrono::system_clock
#include <unordered_map>


#define MAX_INT 2147483647;

class MinHash {
private:
	std::vector<std::vector<unsigned int>> hash;

	std::vector<std::vector<unsigned int>> signature;
	int numhash;

public:

	MinHash(int numHash, int N) :numhash(numHash) {
		hash.resize(numhash);
		vector<unsigned int > tmp;
		for (int j = 0; j < N; j++)		tmp.push_back(j);

		unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
		std::default_random_engine e(seed);

		for (int i = 0; i < numHash; i++)
		{
			std::shuffle(tmp.begin(), tmp.end(), e);
			for (int j = 0; j<N; j++) 
				hash[i].push_back(tmp[j] );
		}
		//printHash(hash, N);
		init_signature(N);
		//printHash(signature, N);
		
	}

	void init_signature(int N) {
		for (int i = 0; i < numhash; i++)
		{
			vector<unsigned int> c(N, 2147483647);
			signature.push_back(c);
		}
	}

	
	
	void  CalcHash(DirectedGraph G) {
		

		/*
		* traverse the edges of the graph 
		* to calculate the fingerprints of the hi
		*/
		for (TNGraph::TEdgeI EI = G.BegEI(); EI < G.EndEI(); EI++) {
			for (int k = 0; k < numhash; k++)
				if (signature[k][EI.GetSrcNId()] > hash[k][EI.GetDstNId()]) 
					signature[k][EI.GetSrcNId()] = hash[k][EI.GetDstNId()];
		}
		
		//printHash(signature, G.getNumNodes());
	
	}

	/*
	* Function that groups node with similar 
	* adjacency lists based on the signature 
	* matrix (suposed to be already set)
	* Inputs :
	*		- N : node numbers .
	*/

	map<unsigned int, std::vector<unsigned int>> getClusters(int N){
		
		/*
		* 1.0 Sort (descending order) the signature matrix based on number
		*	  of nodes that has same value hi ???
		*/



		map<unsigned int, std::vector<unsigned int>> list_of_clusters;
		/*
		 * 1.1 Get nodes with same h1
		 */
		for (int i = 0; i < N; i++) {
			if (list_of_clusters.find(signature[0][i]) == list_of_clusters.end()) {
				//a new cluster to add to the list of clusters
				vector<unsigned int> value(1, i);
				list_of_clusters.insert({signature[0][i],value});
			}
			else {
				// we append the current node to the cluster
				list_of_clusters.at(signature[0][i]).push_back(i);
			}
		}


		//	You can uncomment this section 
		//	if you want to visualize the clusters
		//  that were formed in the first stage 

		//for (std::map<unsigned int, std::vector<unsigned int>>::iterator it = list_of_clusters.begin(); it != list_of_clusters.end(); it++) {
		//	cout << it->first << ":	";
		//	for (int i = 0; i < it->second.size(); i++) cout<<it->second[i]<<"	";
		//	cout << endl;
		//}

		/*
		 * 1.2 refine clusters according to the rest values of hi , i=2 ... numhash
		 */

		vector<unsigned int> cluster_rest;

		for (std::map<unsigned int, std::vector<unsigned int>>::iterator it = list_of_clusters.begin(); it != list_of_clusters.end(); it++) {
			// foreach cluster we refine it
			vector<unsigned int> cluster = it->second;

			for (int i = 1; i < numhash & cluster.size() > 3; i++) {
				//Start filtering the nodes if they don't match the hi of the cluster
				int hi_cluster;

				//	1.2.1 find hi(cluster) = value of most element = most frequent value in hi
				hi_cluster = mostFrequent(cluster,i);
				//cout << "hash value of the cluster " << it->first << " is :"<< hi_cluster <<endl;

				//	1.2.2 remove nodes that does not match
				for (int j = 0; j < cluster.size() & cluster.size() > 2; j++) {
					if (hi_cluster != signature[i][j]) {
						it->second.erase(std::remove(it->second.begin(), it->second.end(), j), it->second.end());
						//TO DO :
						//	  - push it in an other cluster that contains all values where fingerprints are different
						//	  - verify first it does not exist ??? is it mandatory or can not happen
						if (std::find(cluster_rest.begin(), cluster_rest.end(), j) == cluster_rest.end()) {
							cluster_rest.push_back(j);
						}
					}
				}
			}

			if (cluster.size() == 1) {
				// should not be considered a cluster on it's own
				// we append it to the last cluster that contains 
				// rest of nodes (nodes that does not belong to any 
				// cluster)
				if (std::find(cluster_rest.begin(), cluster_rest.end(), it->second[0]) == cluster_rest.end()) {
					//check if it was not already in the cluster  ??? Can this ever happen ??
					cluster_rest.push_back(it->second[0]);
				}
				list_of_clusters.erase(it);
			}
			
			
		}

		// Append the rest of nodes to the list of clusters
		// We use MAX_INT as a key to avoid duplicate keys 
		if(cluster_rest.size()>1) list_of_clusters.insert({ 2147483647,cluster_rest });

		
		//	You can uncomment this section 
		//	if you want to visualize the final list 
		//  of clusters that minhash function return 

		cout <<" the list of clusters is  :: "<< endl;
		for (std::map<unsigned int, std::vector<unsigned int>>::iterator it = list_of_clusters.begin(); it != list_of_clusters.end(); it++) {
			cout << it->first << ":	";
			for (int i = 0; i < it->second.size(); i++) cout<<it->second[i]<<"	";
			cout << endl;
		}

		return list_of_clusters;

	}

	void printHash(std::vector<std::vector<unsigned int>> hash,int N) {
		for (int i = 0; i < numhash; i++)
		{
			for (int j = 0; j<N; j++) 
				cout << hash[i][j]<<"	";
			cout << endl;
		}
	}

	std::vector<std::vector<unsigned int>> getHash() {
		return hash;
	}



	//---------------------------------------------------------------------------
	// NAME:    mostfrequent
	//
	// INPUTS:  vector<UINt> arr  
	//
	// RETURN:  int
	//
	// PURPOSE: Compute the most frequent element in 
	// an array of integers
	// 
	//---------------------------------------------------------------------------

	 int mostFrequent(vector<unsigned int> arr,int hi)
	{
		// Insert all elements in hash. 
		unordered_map<int, int> hash;
		for (int i = 0; i < arr.size(); i++)
			hash[ signature[hi][arr[i]]  ]++;

		// find the max frequency 
		int max_count = 0, res = -1;
		for (auto i : hash) {
			if (max_count < i.second) {
				res = i.first;
				max_count = i.second;
			}
		}

		return res;
	}


};




#endif /* MinHash_hpp */
