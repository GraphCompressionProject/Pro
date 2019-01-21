
#include "stdafx.h"
#include "Ik2_Trees.hpp"

using namespace std ;

Ik2_Trees::Ik2_Trees(int k,int nbT, int n,bool diff, vector<vector<boost::dynamic_bitset<>>> A )
    :k(k),nbT(nbT)
{
    prof = helpfunctions::logk(k,n);
    if(!diff) build_from_matrix(n,1,0,0,A);
    else {
        //Building the matrix of differences
		vector<vector<boost::dynamic_bitset<>>> B ;
        CalcDiff(A,B);
        // Building the Ik2Tree
        build_from_matrix(n,1,0,0,B);
    }
	
     BuildTree();
     T.clear();
     T.shrink_to_fit();
}

void Ik2_Trees::CalcDiff(vector<vector<boost::dynamic_bitset<>>> A , vector<vector<boost::dynamic_bitset<>>> B){
    for(int i=0;i<10;i++){
        for(int j=0;j<10;j++){
            for(int m=0;m<10;m++){
                if((A[m][i][j]!=A[m-1][i][j]) || (m==0)) B[m][i][j] = 1;
                else B[m][i][j] =0;
            }
        }
    }
}

boost::dynamic_bitset<> Ik2_Trees::build_from_matrix(int n,int l,int p,int q, vector<vector<boost::dynamic_bitset<>>> A ){
  
    std::vector<boost::dynamic_bitset<>> C{0};
    boost::dynamic_bitset<> C3{0};
	boost::dynamic_bitset<> C2{ nbT,0 };
    // iterate over the matrix to build the
    // T list
    for(int i=0;i<k;i++){
        for(int j=0;j<k;j++){
            if (l == prof) { //leaf node
                for(int m=0;m<nbT;m++){
                    if(C.size()<m+1) C.push_back(C3);
                    if(A[m][p+i][q+j] == 0) C[m].push_back(false);
                    else C[m].push_back(true);
                }
            }else{
                    boost::dynamic_bitset<> C4{0};
                    C4 = build_from_matrix(n/k,l+1,p+i*(n/k),q+j*(n/k),A)  ;
                    for(boost::dynamic_bitset<>::size_type m=0;m<C4.size();m++){
                        if(C.size()<m+1) C.push_back(C3);
                        C[m].push_back(C4[m]);
						C2[m] = C2[m] || C4[m];
                    }
            }
        }
    }
    
    if (all_null_C(C)) return C2 ;

    // Verify if the current level Exist
    if(T.size()<prof-l+1)	 T.push_back(C3);

    for(int j=0;j<k*k;j++){
        for (boost::dynamic_bitset<>::size_type i = 0; i < C.size(); i++){
            if((helpfunctions::all_null_C(C[i]) == false) || (l==1)){
               T[prof-l].push_back(C[i][j]);
            }
        }
    }   

    return C2;
}

bool Ik2_Trees::all_null_C(std::vector<boost::dynamic_bitset<>> Ctmp){
    for(int i=0;i<Ctmp.size();i++){
        if(!helpfunctions::all_null_C(Ctmp[i])) return false;
    }
    return true;
}

void Ik2_Trees::BuildTree(){
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




boost::dynamic_bitset<> Ik2_Trees::get_L(){
    return _L;
}

boost::dynamic_bitset<> Ik2_Trees::get_T(){
    return _T;
}
