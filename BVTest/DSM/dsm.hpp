#pragma once
#include "MinHash.hpp"
#include "..\DirectedGraph.h"
#include <boost\dynamic_bitset.hpp>
#include "../tree.h"
#include "../k2_Trees.hpp"



using namespace std;

class dsm
{
private:
	double tExec;
	std::vector<unsigned int > X;
	boost::dynamic_bitset<> B;
	k2_Trees ErrorCompressed;
	bool CompressError;

	vector<pair<vector<unsigned int>, vector<unsigned int>>> S_C_list;

	void discoverSubs(DirectedGraph g, int numHash);
	boost::dynamic_bitset<> _hasEdgeNN;
	void append(vector<pair<vector<unsigned int>, vector<unsigned int>>> bestS_C,DirectedGraph G);

	template<class T>
	typename tree<T>::iterator find_element(const tree<T>& t, typename tree<T>::iterator iRoot, unsigned int n)
	{
		if (t.empty()) return NULL;
		if (iRoot->first == n) return iRoot;
		else {
			if (t.number_of_children(iRoot) == 0) {
				return NULL;
			}
			else {
				for (auto it = iRoot.begin(); it != iRoot.end(); it++) {
					auto i = find_element(t, it, n);
					if (i != NULL) return i;
				}
				return NULL;
			}
		}
	}

	pair<bool, tree<pair<unsigned int, vector<unsigned int>>>::iterator> isChild(int n, tree<pair<unsigned int, vector<unsigned int>>>::iterator current) {
		tree<pair<unsigned int, vector<unsigned int>>>::sibling_iterator  begin = current.begin();
		bool found = false;
		while (!found && begin != current.end()) {
			if (begin->first == n) found = true;
			else ++begin;
		}
		return { found,begin };
	}

	vector<pair<vector<unsigned int>, vector<unsigned int>>> retreiveS_C(tree<pair<unsigned int, vector<unsigned int>>> T);

public:
	dsm();
	dsm(DirectedGraph g,int numHash,bool compressError);
	
	void printX_B();

	pair<std::vector<unsigned int >, boost::dynamic_bitset<>> getX_B() {
		return pair<std::vector<unsigned int >, boost::dynamic_bitset<>>(X, B);
	}

	void saveX_B(string filename);

	vector<pair<vector<unsigned int>, vector<unsigned int>>> getSubList() {
		return S_C_list;
	}

	int getErrorSize() {
		if (CompressError) {
			return (ErrorCompressed.get_T().size() + ErrorCompressed.get_L().size());
		}
		else {
			return 0;
		}
	}

	void updateError(DirectedGraph G, pair<vector<unsigned int>, vector<unsigned int>> S_C);
	
	double get_Time() {
		return tExec;
	}

	~dsm();
};

