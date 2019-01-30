#pragma once
using namespace std;
class listAdjacence
{
	vector<unsigned int> listAdj;
	unsigned int index;
public:
	listAdjacence();
	~listAdjacence();
	unsigned int getNext();
	vector<unsigned int>  getList();
	unsigned int getindex();
	void addElement(unsigned int elm);
	void advance();
};

