//
// Created by Macbook on 10/02/2019.
//

#include "stdafx.h"
#include "MatchHeapNode.hpp"


unsigned long MatchHeapNode::getDepth() {
    return depth;
}

void MatchHeapNode::setDepth(unsigned long depth){
    MatchHeapNode::depth =depth;
}

double MatchHeapNode::getCost() {
    return cost;
}

void MatchHeapNode::setCost(double cost){
    MatchHeapNode::cost =cost;
}

VertexMap *MatchHeapNode::getMapping() {
    return mapping;
}

void MatchHeapNode::setMapping(VertexMap *mapping){
    MatchHeapNode::mapping = mapping ;
}

//---------------------------------------------------------------------------
// NAME: PrintMatchHeapNode
//
// INPUTS: (MatchHeapNode *node) - match node to print
//
// RETURN: (void)
//
// PURPOSE: Print match node.
//---------------------------------------------------------------------------

void MatchHeapNode::PrintMatchHeapNode()
{
    ULONG i;
    printf("MatchHeapNode: depth = %lu, cost = %f, mapping =",
           depth, cost);
    if (depth > 0)
    {
        printf("\n");
        for (i = 0; i < depth; i++)
        {
            printf("            %lu -> ", mapping[i].getV1());
            if (mapping[i].getV2() == VERTEX_UNMAPPED)
                printf("unmapped\n");
            else if (mapping[i].getV2() == VERTEX_DELETED)
                printf("deleted\n");
            else printf("%lu\n", mapping[i].getV2());
        }
    }
    else
        printf(" NULL\n");
}


