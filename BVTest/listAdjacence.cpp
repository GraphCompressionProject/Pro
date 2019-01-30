#include "stdafx.h"
#include "listAdjacence.h"


listAdjacence::listAdjacence()
{
	index = 0;
}


listAdjacence::~listAdjacence()
{
}

unsigned int listAdjacence::getNext()
{
	return listAdj[index];
}

vector<unsigned int>  listAdjacence::getList()
{
	return listAdj;
}

unsigned int listAdjacence::getindex()
{
	return index;
}

void listAdjacence::addElement(unsigned int elm)
{
	listAdj.push_back(elm);
}

void listAdjacence::advance()
{
	if(index<listAdj.size()-1) index++;
}
