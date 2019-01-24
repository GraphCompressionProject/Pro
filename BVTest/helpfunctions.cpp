#include "stdafx.h"
#include "helpfunctions.h"
#include <iostream>
#include <string>

/*
* Extention of the adjacency matrix in the case
* where its size is not a power of k
* Inputs :
*      - A: Original Matrix
*      - n : the resulted size
*/
using namespace std;
 void helpfunctions::extend_matrix(std::vector<boost::dynamic_bitset<>> *A, int n1)
{
	boost::dynamic_bitset<> C{ n1,0 };
	for (boost::dynamic_bitset<>::size_type i = 0; i < n1; i++) {
		if (i<(*A).size()) (*A)[i].resize(n1);
		else (*A).push_back(C);
	}
}
 bool helpfunctions::all_null_C(boost::dynamic_bitset<> C) {
	for (boost::dynamic_bitset<>::size_type i = 0; i < C.size(); i++)
		if (C[i] == true) return false;
	return true;
}

 unsigned int helpfunctions::logk(int k, int m) {
	return log(m) / log(k);
}
 boost::dynamic_bitset<> helpfunctions::toBinary(int n,int m)
 {
	 boost::dynamic_bitset<> r{0};
	 unsigned int v = n;
	 bool c;
	 while ((v != 0)|| (r.size()<m)) {
		 if (v == 0) {
			 r.push_back(false);
		 }
		 else {
			 c = (v % 2 == 0) ? false : true;
			 r.push_back(c);
			 v = v / 2;
		 }
	 }
	 return r;
 }

 int helpfunctions::rank(boost::dynamic_bitset<> _T, int n)
 {
	 int cpt = 0;
	 for (int i = 0; i <= n; i++)
		 if (_T[i] == 1) cpt++;
	 return cpt;
 }

 helpfunctions::helpfunctions()
{
}


helpfunctions::~helpfunctions()
{
}
