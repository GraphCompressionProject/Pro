#pragma once
#include <boost/dynamic_bitset.hpp>
class utils
{
public:
	static void extend_matrix(std::vector<boost::dynamic_bitset<> > *A, int n1);
	static bool all_null_C(boost::dynamic_bitset<> C);
	static  int logk(int k, int m);
	static  boost::dynamic_bitset<> toBinary(int n);

	utils();
	~utils();
};

