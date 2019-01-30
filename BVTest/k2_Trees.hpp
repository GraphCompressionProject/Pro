//
// Created by Macbook on 29/12/2018.
//

#ifndef K2_TREES_IMPLEMENTATION_K2_TREES_HPP
#define K2_TREES_IMPLEMENTATION_K2_TREES_HPP
#include <boost/dynamic_bitset.hpp>
#include <ctgmath>
#include <iostream>
#include "listAdjacence.h"


class k2_Trees {

private:
    std::vector<boost::dynamic_bitset<> > T;
    boost::dynamic_bitset<> _T;
    boost::dynamic_bitset<> _L;

    int k ;
    unsigned int prof ;

	// function that creates a T list for each level from Adjacency List
	int build_from_matrix(int n, int l, int p, int q, std::vector<boost::dynamic_bitset<> > A);
	// function that creates a T List for each level from Adjacency List
	int build_from_List(int n, int l, int p, int q, std::vector<listAdjacence> *A);
	// Concatenate the T lists in 2 lists T and L
	void BuildTree();

public:
    // Constructor
    k2_Trees(int k, int n, std::vector<boost::dynamic_bitset<> > A);
	k2_Trees(int k, int n, std::vector<listAdjacence> A);

	//void extend_matrix(std::vector<boost::dynamic_bitset<> > *A, int n1);

	
   

   // bool all_null_C(boost::dynamic_bitset<> C);
    /*
     * Getter of the two Lists
     */
    boost::dynamic_bitset<> get_L();
    boost::dynamic_bitset<> get_T();


    void Direct(int n,int  p,int  q, int   x);
    void Reverse(int n,int  p,int  q,int  x);
    
};


#endif //K2_TREES_IMPLEMENTATION_K2_TREES_HPP
