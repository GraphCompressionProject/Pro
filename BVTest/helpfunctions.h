#pragma once
#include <boost/dynamic_bitset.hpp>
class helpfunctions
{
public:
	static void extend_matrix(std::vector<boost::dynamic_bitset<> > *A, int n1);
	static bool all_null_C(boost::dynamic_bitset<> C);
	static unsigned  int logk(int k, int m);
	static  boost::dynamic_bitset<> toBinary(int n,int m);
	
	/*
	* Getters of the direct and reverse neighbors
	*/

	static int rank(boost::dynamic_bitset<> _T, int n);

	helpfunctions();
	~helpfunctions();
};

