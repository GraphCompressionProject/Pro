

#ifndef K2_TREES_IMPLEMENTATION_IK2_TREES_HPP
#define K2_TREES_IMPLEMENTATION_IK2_TREES_HPP
#include <boost/dynamic_bitset.hpp>
#include <ctgmath>
#include <iostream>
#include <fstream>
#include "helpfunctions.h"

using namespace std;

using std::vector;

class Ik2_Trees {
private:
	std::vector<boost::dynamic_bitset<> > T;
	boost::dynamic_bitset<> _T;
	boost::dynamic_bitset<> _L;
	int nbT;
	int k;
	int prof;
	double tExec;
	bool isDirected;
public:


	Ik2_Trees(int k, int nbT, int n, bool diff, vector<vector<boost::dynamic_bitset<>>> A);

	vector<vector<boost::dynamic_bitset<>>>  CalcDiff(vector<vector<boost::dynamic_bitset<>>> A);
	// function that creates a T list for each level
	boost::dynamic_bitset<> build_from_matrix(int n, int l, int p, int q, vector<vector<boost::dynamic_bitset<>>> A);
	// Concatenate the T lists in 2 lists T and L
	void BuildTree();

	bool all_null_C(std::vector<boost::dynamic_bitset<>> Ctmp);

	boost::dynamic_bitset<> get_T();
	boost::dynamic_bitset<> get_L();
	double get_Time();


	void getDirect(int n, int  p, int  q, int  x);
	void getReverse(int n, int  p, int  q, int  x);

	void Direct(int n, int  p, int  q, int   x);
	void Reverse(int n, int  p, int  q, int  x);

	void saveT_L(string filename) {
		ofstream outFile;
		string folder1 = "..\\..\\BVTest\\data\\";
		outFile.open(folder1 + "IK2_out_" + filename + ".txt", 'w');
		for (int i = 0; i < _T.size(); i++) outFile << _T[i];
		outFile << endl << endl;
		for (int i = 0; i < _L.size(); i++) outFile << _L[i];
		outFile.close();
	}

};




#endif //K2_TREES_IMPLEMENTATION_IK2_TREES_HPP
