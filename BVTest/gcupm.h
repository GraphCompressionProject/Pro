#pragma once
#include <boost/dynamic_bitset.hpp>
#include <time.h>
#include <iostream>
#include <fstream>
using namespace std;
class gcupm
{
private:
	/*
	 * Matrix to contain the compressed graph
	*/
	std::vector<boost::dynamic_bitset<> > mat;
	/*
	 * The type of the Pattern to be considered
	 * Values :
	 *	- 0 : (pattern with all 0's) or (1 at MSB and another bit to 1)
	 *	- 1 : pattern with only one 1
	 *	- 2 : the union of the two previous sets
	*/
	unsigned int modelType;
	/*
	 * The size of the patten in order to limit 
	 * the size of its indicator in the compressed 
	 * Graph (32bits, 64bits, ...)
	*/
	unsigned int patternSize;

	/*
	 * Execution time of the compressio process
	 */
	double tExecution;

	int valueOf(std::vector<boost::dynamic_bitset<> > Adj, int lign, int col,int limit);

	void discoverPattern1(std::vector<boost::dynamic_bitset<> > Adj);
	void discoverPattern2(std::vector<boost::dynamic_bitset<> > Adj);
	void discoverPattern3(std::vector<boost::dynamic_bitset<> > Adj);

public:
	gcupm(std::vector<boost::dynamic_bitset<> > Adj, int patternSize, int modelType);
	std::vector<boost::dynamic_bitset<> > get_Result();
	double get_Time();
	int get_size();
	virtual ~gcupm();

	void saveMat(string filename) {
		ofstream outFile;
		string folder1 = "..\\..\\BVTest\\data\\";
		outFile.open(folder1 + "GCUPM_out_" + filename + ".txt", 'w');

		for (int i = 0; i < mat.size(); i++) {
			for (int j = 0; j < mat[i].size(); j++)
				outFile << mat[i][j];
			outFile << endl;
		}
		outFile.close();
	}
};

