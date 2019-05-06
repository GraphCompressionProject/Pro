#pragma once
#include "../DSM/MinHash.hpp"


class temporalMinHash : MinHash
{
public:
	temporalMinHash(int numHash, int N) :MinHash(numHash, N) {
		/*
			
		*/
	}

	map<unsigned int, std::vector<unsigned int>> getCluster(int N) {
		return getClusters(N);
	}

	void  CalcHash(map<unsigned int, vector<unsigned int>> compressedVersion) {


		/*
		* traverse the sets
		* to calculate the fingerprints of the hi
		*/

		for (auto& it : compressedVersion) {

			for (unsigned int n : it.second) {
				for (int k = 0; k < numhash; k++)
					if (signature[k][it.first] > hash[k][n])
						signature[k][it.first] = hash[k][n];
			}
			
		}

		//printHash(signature, G.getNumNodes());

	}

	~temporalMinHash();
};

