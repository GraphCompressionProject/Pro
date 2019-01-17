//
// Created by Macbook on 29/12/2018.
//
#include "stdafx.h"
#include "k2_Trees.hpp"

using namespace std ;
/*
    * The Constructor
    * Inputs :
    *      - k: Numbre of childs of the k2-trees
    *      - prof : the depth of the k2-tree
    *      - the Adjacency Matrix of the graph
    */
k2_Trees::k2_Trees(int k, int n, std::vector<boost::dynamic_bitset<> > A)
    :k(k)
{

    prof = logk(k,n);
    build_from_matrix(n,1,0,0,A);
    BuildTree();
    T.clear();
    T.shrink_to_fit();

}

/*
    * Recursive construction of the k2-tree
    * Inputs :
    *      n: is the submatrix size,
    *      l: the current level,
    *      p: the row offset of the current submatrix
    *      q: the column offset
    *      A: The matrix
    */

int k2_Trees::build_from_matrix(int n,int l,int p,int q, std::vector<boost::dynamic_bitset<> > A){
    boost::dynamic_bitset<> C{0};
    // iterate over the matrix to build the
    // T list
    for(int i=0;i<k;i++){
        for(int j=0;j<k;j++){
            if (l == prof) { //leaf node
                if(A[p+i][q+j] == 0) C.push_back(false);
                else C.push_back(true);
            }else{
                if(build_from_matrix(n/k,l+1,p+i*(n/k),q+j*(n/k),A) == 0) C.push_back(false);
                else C.push_back(true);
            }
        }
    }
    if (all_null_C(C)) return 0;
    // Verify if the current level Exist
    if(T.size()<prof-l+1)
    { // if not allocate space for Element
        boost::dynamic_bitset<> C2{0};
        T.push_back(C2);
    }

    //Concatenate C to the T list of level l
    for (boost::dynamic_bitset<>::size_type i = 0; i < C.size(); i++){
        T[prof-l].push_back(C[i]);
    }
    return 1;
}

// int k2_Trees::build_from_matrix(int n,int l,int p,int q, int A [10][10]){
//     boost::dynamic_bitset<> C{0};
//     // iterate over the matrix to build the
//     // T list
//     for(int i=0;i<k;i++){
//         for(int j=0;j<k;j++){
//             if (l == prof) { //leaf node
//                 if(A[p+i][q+j] == 0) C.push_back(false);
//                 else C.push_back(true);
//             }else{
//                 if(build_from_matrix(n/k,l+1,p+i*(n/k),q+j*(n/k),A) == 0) C.push_back(false);
//                 else C.push_back(true);
//             }
//         }
//     }
//     if (all_null_C(C)) return 0;
//     // Verify if the current level Exist
//     if(T.size()<prof-l+1)
//     { // if not allocate space for Element
//         boost::dynamic_bitset<> C2{0};
//         T.push_back(C2);
//     }
//     // Resize The list of the level
//     T[prof-l].resize(T[prof-l].size()+k*k);

//     //Concatenate C to the T list of level l
//     for (boost::dynamic_bitset<>::size_type i = 0; i < C.size(); i++){
//         T[prof-l]<<=1;
//         T[prof-l][0]=C[i];
//     }
//     return 1;
// }
/*
 * Build of L and T lists From T
 */
// void k2_Trees::BuildTree(){

//     int m = T.size();

//     for(int j=0;j<T[0].size();j++){
//         _L.push_back(T[0][j]);
//     }

//     for(int i=1;i<m;i++){
//         for(int j=0;j<T[i].size();j++){
//             _T.push_back(T[i][j]);
//         }
//     }
// }

void k2_Trees::BuildTree(){

    int m = T.size();

    for(int j=0;j<T[0].size();j++){
        _L.push_back(T[0][j]);
    }

    for(int i=m-1;i>=1;i--){
        for(int j=0;j<T[i].size();j++){
            _T.push_back(T[i][j]);
        }
    }
}

int k2_Trees::logk(int k, int m){
    return log(m)/log(k) ;
}

boost::dynamic_bitset<> k2_Trees::get_L(){
    return _L;
}

boost::dynamic_bitset<> k2_Trees::get_T(){
    return _T;
}

bool k2_Trees::all_null_C(boost::dynamic_bitset<> C){
    for (boost::dynamic_bitset<>::size_type i = 0; i < C.size(); i++)
        if(C[i] == true) return false ;
    return true ;
}

int k2_Trees::rank(boost::dynamic_bitset<> _T,int n){
    int cpt=0;
    for(int i=0;i<=n;i++)
        if (_T[i] == 1) cpt++;
    return cpt;
}

/* 
 * Function that retreives the direct neighbors
 * Inputs :
 *      - n : size of current submatrix
 *      - p : page p of interest
 *      - q : column offset of the current submatrix in the global matrix
 *      - x : the position in T : L of the node to process 
 *            (the initial −1 is an artifact because our 
 *            trees do not represent the root node)
 */

void k2_Trees::Direct(int n,int  p,int  q,int  x){
    int v = _T.size() ;
    if(v - x <= 0) //leaf
    {
        if (_L[x-v] == true) cout << q << " ";
    }
    else //internal Node
    {
        if ((x== -1)||(_T[x]==1)){
            unsigned int y=0;
            y= rank(_T,x)*k*k + k*(p/(n/k));
            for(int j=0;j<k;j++){
                Direct(n/k, p%(n/k), q + j*(n/k), y+j);
            }
        }
    }
}

/* 
 * Function that retreives the reverse neighbors
 * Inputs :
 *      - n : size of current submatrix
 *      - p : page p of interest
 *      - q : column offset of the current submatrix in the global matrix
 *      - x : the position in T : L of the node to process 
 *            (the initial −1 is an artifact because our 
 *            trees do not represent the root node)
 */
void k2_Trees::Reverse(int n,int  q,int  p,int  x){
    int size = _T.size();
    if(x>=size) //leaf
    {
        if (_L[x-size] == true) cout << p << " ";
    }
    else //internal Node
    {
        if ((x== -1)||(_T[x]==1)){
            unsigned int y= rank(_T,x)*k*k + q/(n/k);
            for(int j=0;j<k;j++){
                Reverse(n/k, q%(n/k), p + j*(n/k), y + j*k);
            }
        }
    }
}
