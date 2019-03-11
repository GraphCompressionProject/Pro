#include "stdafx.h"
#include "UndirectedGraph.h"



UndirectedGraph::UndirectedGraph(const char* fileName)
{
	TFIn FIn(fileName);
	PUNGraph G = TUNGraph::Load(FIn);
	graph = G;
}

UndirectedGraph::UndirectedGraph(string fileName)
{
	
}




UndirectedGraph::UndirectedGraph(PUNGraph& G)
{
	graph = G;
}

int UndirectedGraph::getNumNodes()
{
	return graph->GetNodes();
}
int UndirectedGraph::getNumEdges()
{
	return graph->GetEdges();
}

TUNGraph::TNodeI UndirectedGraph::getNodeIteratorBegin()
{
	return graph->BegNI();
}
TUNGraph::TNodeI UndirectedGraph::getNodeIteratorEnd()
{
	return graph->EndNI();
}
TUNGraph::TEdgeI UndirectedGraph::getEdgeIteratorBegin() {
	return graph->BegEI();
}
TUNGraph::TEdgeI UndirectedGraph::getEdgeIteratorEnd() {
	return graph->EndEI();
}

bool UndirectedGraph::edgeBetween(int node1, int node2)
{
	return graph->IsEdge(node1, node2);
}
bool UndirectedGraph::emptyGraph()
{
	return graph->Empty();
}
void UndirectedGraph::sortAdjencyLists()
{
	graph->SortNodeAdjV();
}



int UndirectedGraph::getNodeID(TUNGraph::TNode myNode)
{
	return myNode.GetId();
}
int UndirectedGraph::getDegree(TUNGraph::TNode myNode)
{
	return myNode.GetDeg();
}
int UndirectedGraph::getSuccessorIdAt(TUNGraph::TNode myNode, int index)
{
	return myNode.GetNbrNId(index);
}
bool UndirectedGraph::isSuccessor(TUNGraph::TNode myNode, int index)
{
	return myNode.IsNbrNId(index);
}

UndirectedGraph::~UndirectedGraph()
{

}
