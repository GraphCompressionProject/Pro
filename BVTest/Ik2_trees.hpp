#ifndef K2_TREES_IMPLEMENTATION_IK2_TREES_HPP
#define K2_TREES_IMPLEMENTATION_IK2_TREES_HPP
#include <boost/dynamic_bitset.hpp>
#include <ctgmath>
#include <iostream>
#include "helpfunctions.h"

using std::vector;

class Ik2_Trees{
    private:
        std::vector<boost::dynamic_bitset<> > T;
        boost::dynamic_bitset<> _T;
        boost::dynamic_bitset<> _L;
        int nbT;
        int k ;
        int prof ;

    public:

        Ik2_Trees(int k,int nbT, int n,bool diff, vector<vector<boost::dynamic_bitset<>>> A);

        void CalcDiff(vector<vector<boost::dynamic_bitset<>>> A , vector<vector<boost::dynamic_bitset<>>> B);
        // function that creates a T list for each level
        boost::dynamic_bitset<> build_from_matrix(int n,int l,int p,int q, vector<vector<boost::dynamic_bitset<>>> A );
        // Concatenate the T lists in 2 lists T and L
        void BuildTree();
       
        bool all_null_C(std::vector<boost::dynamic_bitset<>> Ctmp);

        boost::dynamic_bitset<> get_T();
        boost::dynamic_bitset<> get_L();
        
};




#endif //K2_TREES_IMPLEMENTATION_IK2_TREES_HPP