#include "stdafx.h"
#include "utils.h"


/*
* Extention of the adjacency matrix in the case
* where its size is not a power of k
* Inputs :
*      - A: Original Matrix
*      - n : the resulted size
*/

 void utils::extend_matrix(std::vector<boost::dynamic_bitset<>> *A, int n1)
{
	boost::dynamic_bitset<> C{ n1,0 };
	for (boost::dynamic_bitset<>::size_type i = 0; i < n1; i++) {
		if (i<(*A).size()) (*A)[i].resize(n1);
		else (*A).push_back(C);
	}
}
 bool utils::all_null_C(boost::dynamic_bitset<> C) {
	for (boost::dynamic_bitset<>::size_type i = 0; i < C.size(); i++)
		if (C[i] == true) return false;
	return true;
}

 int utils::logk(int k, int m) {
	return log(m) / log(k);
}
 boost::dynamic_bitset<> toBinary(int n)
 {
	 boost::dynamic_bitset<> r;
	 while (n != 0) {
		 r.push_back(n % 2 == 0 ? false : true);
		 n /= 2;
	 }
	 return r;
 }

utils::utils()
{
}


utils::~utils()
{
}
