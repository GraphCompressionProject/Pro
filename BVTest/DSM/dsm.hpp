#pragma once
#include "MinHash.hpp"
#include "..\DirectedGraph.h"
#include <boost\dynamic_bitset.hpp>
#include "../tree.h"

using namespace std;

class dsm
{
private:
	std::vector<unsigned int > X;
	boost::dynamic_bitset<> B;

	void discoverSubs(DirectedGraph g, int numHash);
	boost::dynamic_bitset<> _hasEdgeNN;
	void append(vector<pair<vector<unsigned int>, vector<unsigned int>>> bestS_C);

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
	dsm(DirectedGraph g,int numHash);
	void printX_B();

	

	~dsm();
};

