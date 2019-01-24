#include "stdafx.h"
#include "gcupm.h"
#include "helpfunctions.h"

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
	for (int i = col * patternSize+limit; i < (col + 1)*patternSize ; i++) {
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
			int v = valueOf(Adj, i, j, 1);
			unsigned int pos = helpfunctions::logk(2, v);
			
			if ((Adj[i][j*patternSize] && v == pow(2, pos)) || (!Adj[i][j*patternSize] && v == 0)) {
				//We have a match with the first pattern
				mat[i].push_back(true);
				boost::dynamic_bitset<> chunk = helpfunctions::toBinary(v==0? 0:pos+1,helpfunctions::logk(2, patternSize));
				
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
		    unsigned int v = valueOf(Adj, i, j,0);
			unsigned int pos = helpfunctions::logk(2, v);
			if ( v == pow(2, pos)){
				//We have a match with the first pattern
				mat[i].push_back(true);
				boost::dynamic_bitset<> chunk = helpfunctions::toBinary(pos,helpfunctions::logk(2,patternSize));
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
			int v = valueOf(Adj, i, j, 1);
			unsigned int pos = helpfunctions::logk(2, v);

			if ((Adj[i][j*patternSize] && v == pow(2, pos)) || (!Adj[i][j*patternSize] && v == 0)) {
				//We have a match with the first pattern
				mat[i].push_back(true);
				boost::dynamic_bitset<> chunk = helpfunctions::toBinary(v == 0 ? 0 : pos + 1, helpfunctions::logk(2, patternSize)+1);

				for (int o = 0; o < chunk.size(); o++)
					mat[i].push_back(chunk[o]);
			}
			else {
				//cout << v/2 << endl;
				v = v*2 + Adj[i][j*patternSize] ;
				
				pos= helpfunctions::logk(2, v);
				if (v == pow(2, pos)) {
					//We have a match with the first pattern
					mat[i].push_back(true);
					boost::dynamic_bitset<> chunk = helpfunctions::toBinary(pos + patternSize, helpfunctions::logk(2, patternSize)+1);
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

std::vector<boost::dynamic_bitset<> > gcupm::get_Result() {
	return mat;
}

gcupm::~gcupm()
{
}
