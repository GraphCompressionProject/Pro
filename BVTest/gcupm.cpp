#include "stdafx.h"
#include "gcupm.h"
#include "utils.h"

gcupm::gcupm(std::vector<boost::dynamic_bitset<> > Adj,int patternSize, int modelType)
	:modelType(modelType), patternSize(patternSize)
{	
	switch (modelType) {
	case 0:
		discoverPattern1(Adj);
		break;
	case 1 :
		discoverPattern2(Adj);
		break;
	case 2 :
		discoverPattern3(Adj);
		break;
	default:
		cout << "error in model type\n It could only be of type 0, 1 or 2." << endl;
		break;
	}

	
}

int gcupm::valueOf(std::vector<boost::dynamic_bitset<> > Adj,int lign,int col,int limit) {
	int n = 0;
	int puissance = 1;
	for (int i = col * patternSize; i < (col + 1)*patternSize -limit; i++) {
		n += puissance * Adj[lign][i];
		puissance*=2;
	}
	return n;
}

void gcupm::discoverPattern1(std::vector<boost::dynamic_bitset<> > Adj)
{
	boost::dynamic_bitset<>C{ 0 };
	for (int i = 0; i < Adj.size(); i++) {
		mat.push_back(C);
		for (int j = 0; j < Adj.size() / patternSize; j++) {
			int i = valueOf(Adj, i, j,1);
			int pos = log(i) / log(2);
			if ((Adj[i][(j+1)*patternSize-1] && i == pow(2, pos)) || (!Adj[i][(j+1)*patternSize] && i == 0)) {
				//We have a match with the first pattern
				mat[i].push_back(true);
				boost::dynamic_bitset<> chunk = utils::toBinary(pos);
				for (int o = 0; o < chunk.size(); o++)
					mat[i].push_back(chunk[o]);
			}
			else {
				mat[i].push_back(false);
				for (int k = 0; k<patternSize; k++)
					mat[i].push_back(Adj[i][j*patternSize + k]);
			}

		}
	}
		

}

void gcupm::discoverPattern2(std::vector<boost::dynamic_bitset<> > Adj)
{
	boost::dynamic_bitset<>C{ 0 };
	for (int i = 0; i < Adj.size(); i++) {
		mat.push_back(C);
		for (int j = 0; j < Adj.size() / patternSize; j++) {
			int i = valueOf(Adj, i, j,0);
			int pos = log(i) / log(2);
			if ( i == pow(2, pos)){
				//We have a match with the first pattern
				mat[i].push_back(true);
				boost::dynamic_bitset<> chunk = utils::toBinary(pos);
				for (int o = 0; o < chunk.size(); o++)
					mat[i].push_back(chunk[o]);
			}
			else {
				mat[i].push_back(false);
				for (int k = 0; k<patternSize; k++)
					mat[i].push_back(Adj[i][j*patternSize + k]);
			}

		}
	}
}

void gcupm::discoverPattern3(std::vector<boost::dynamic_bitset<>> Adj)
{
	boost::dynamic_bitset<>C{ 0 };
	for (int i = 0; i < Adj.size(); i++) {
		mat.push_back(C);
		for (int j = 0; j < Adj.size() / patternSize; j++) {
			int i = valueOf(Adj, i, j,1);
			int pos = log(i) / log(2);
			if ((Adj[i][(j + 1)*patternSize - 1] && i == pow(2, pos)) || (!Adj[i][(j + 1)*patternSize] && i == 0)) {
				//We have a match with the first pattern
				mat[i].push_back(true);
				boost::dynamic_bitset<> chunk = utils::toBinary(pos);
				for (int o = 0; o < chunk.size(); o++)
					mat[i].push_back(chunk[o]);

			}
			else {
				i += Adj[i][j*patternSize-1] * pow(2, patternSize -1);
				pos= log(i) / log(2);
				if (i == pow(2, pos)) {
					//We have a match with the second pattern
					mat[i].push_back(true);
					boost::dynamic_bitset<> chunk = utils::toBinary(pos + utils::logk(patternSize, 2));
					for (int o = 0; o < chunk.size(); o++)
						mat[i].push_back(chunk[o]);
				}
				else {
					mat[i].push_back(false);
					for (int k = 0; k<patternSize; k++)
						mat[i].push_back(Adj[i][j*patternSize + k]);
				}
			} 

		}
	}
}



gcupm::~gcupm()
{
}
