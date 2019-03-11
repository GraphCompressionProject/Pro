//
// Created by Macbook on 10/02/2019.
//
#include "stdafx.h"
#include "MatchHeap.hpp"
#include "utility.hpp"
#include <iostream>
using namespace std;

//---------------------------------------------------------------------------
// NAME: MatchHeapEmpty
//
// INPUTS: (MatchHeap *heap) - heap to check if empty
//
// RETURN: (BOOLEAN) - TRUE if heap is empty; else FALSE
//
// PURPOSE: Determine if given match node heap is empty.
//---------------------------------------------------------------------------

BOOLEAN MatchHeap::MatchHeapEmpty()
{
    if (numNodes == 0)
        return TRUE;
    return FALSE;
}

//---------------------------------------------------------------------------
// NAME: InsertMatchHeapNode
//
// INPUTS: (MatchHeapNode *node) - node to insert
//         (MatchHeap *heap) - heap to insert into
//
// RETURN:  (void)
//
// PURPOSE: Insert given node into given heap, maintaining increasing
// order by cost, and for nodes with the same cost, in decreasing order
// by depth.
//---------------------------------------------------------------------------

void MatchHeap::InsertMatchHeapNode(MatchHeapNode *node)
{
    ULONG i;
    ULONG parent;
    MatchHeapNode *node2;
    BOOLEAN done;
    
    numNodes++;
    // add more memory to heap if necessary
    if (numNodes > size)
    {
        size = 2 * size;
        nodes = (MatchHeapNode *) realloc(nodes, size * sizeof(MatchHeapNode));
        if (nodes == NULL)
            utility::OutOfMemoryError("InsertMatchHeapNode:heap->nodes");
    }
    // find right place for new node
    i = numNodes - 1;
    done = FALSE;
    while ((i > 0) && (! done))
    {
        parent = HeapParent(i);
        node2 = & nodes[parent];
        if ((node->getCost() < node2->getCost()) ||
            ((node->getCost() == node2->getCost()) && (node->getDepth() > node2->getDepth())))
        {
            nodes[i].setCost(nodes[parent].getCost());
            nodes[i].setDepth(nodes[parent].getDepth()) ;
            nodes[i].setMapping(nodes[parent].getMapping());
            i = parent;
        }
        else
            done = TRUE;
    }
    // store new node
    nodes[i].setCost(node->getCost());
    nodes[i].setDepth(node->getDepth());
    nodes[i].setMapping(node->getMapping());
}



//---------------------------------------------------------------------------
// NAME: ClearMatchHeap
//
// INPUTS: (MatchHeap *heap) - match node heap to clear
//
// RETURN: (void)
//
// PURPOSE: Free mappings of match nodes still in heap and reset heap to
// have zero nodes.
//---------------------------------------------------------------------------

void MatchHeap::ClearMatchHeap()
{
    ULONG i;
    
    for (i = 0; i < numNodes; i++)
        free(nodes[i].getMapping());
    numNodes = 0;
}


//---------------------------------------------------------------------------
// NAME: FreeMatchHeap
//
// INPUTS: (MatchHeap *heap) - match node heap to free
//
// RETURN: (void)
//
// PURPOSE: Free memory in given match node heap.
//---------------------------------------------------------------------------

void MatchHeap::FreeMatchHeap()
{
    ClearMatchHeap();
    free(this->getNodes());
    free(this);
}




//---------------------------------------------------------------------------
// NAME: ExtractMatchHeapNode
//
// INPUTS: (MatchHeap *heap) - heap to be removed from
//         (MatchHeapNode *node) - node into which to copy information
//                                 from best node
//
// RETURN: (void)
//
// PURPOSE: Return, in the given node, the best node in the heap and
// maintain ordering.
//---------------------------------------------------------------------------

void MatchHeap::ExtractMatchHeapNode( MatchHeapNode *node)
{
    ULONG i;
    
    // copy best node to input storage node
    node->setCost(nodes[0].getCost()) ;
    node->setDepth(nodes[0].getDepth());
    node->setMapping(nodes[0].getMapping())  ;
    
    // copy last node in heap array to first
    i = numNodes - 1;
    nodes[0].setCost(nodes[i].getCost()) ;
    nodes[0].setDepth(nodes[i].getDepth()) ;
    nodes[0].setMapping(nodes[i].getMapping());
    numNodes--;
    
    HeapifyMatchHeap();
}


//---------------------------------------------------------------------------
// NAME: HeapifyMatchHeap
//
// INPUTS: (MatchHeap *heap) - heap to heapify from root
//
// RETURN:  (void)
//
// PURPOSE: Restores the heap property of the heap starting at the root
// node.  The heap property is that parent nodes have less cost than their
// children, and if the cost is the same, then parents have greater or
// equal depth than their children.
//---------------------------------------------------------------------------

void MatchHeap::HeapifyMatchHeap()
{
    ULONG parent;
    ULONG best;
    ULONG leftChild;
    ULONG rightChild;
    MatchHeapNode *parentNode;
    MatchHeapNode *bestNode;
    MatchHeapNode *leftNode;
    MatchHeapNode *rightNode;
    ULONG tmpDepth;
    double tmpCost;
    VertexMap *tmpMapping;
    
    parent = 0;
    best = 1;
    while (parent != best)
    {
        parentNode = & nodes[parent];
        leftChild = HeapLeftChild(parent);
        rightChild = HeapRightChild(parent);
        best = parent;
        bestNode = parentNode;
        // check if left child better than parent
        if (leftChild < numNodes)
        {
            leftNode = & nodes[leftChild];
            if ((leftNode->getCost() < parentNode->getCost()) ||
                ((leftNode->getCost() == parentNode->getCost()) &&
                 (leftNode->getDepth() > parentNode->getDepth())))
            {
                best = leftChild;
                bestNode = leftNode;
            }
        }
        // check if right child better than best so far
        if (rightChild < numNodes)
        {
            rightNode = & nodes[rightChild];
            if ((rightNode->getCost() < bestNode->getCost()) ||
                ((rightNode->getCost() == bestNode->getCost()) &&
                 (rightNode->getDepth() > bestNode->getDepth())))
            {
                best = rightChild;
                bestNode = rightNode;
            }
        }
        // if child better than parent, then swap
        if (parent != best)
        {
            tmpCost = parentNode->getCost();
            tmpDepth = parentNode->getDepth();
            tmpMapping = parentNode->getMapping();
            parentNode->setCost(bestNode->getCost());
            parentNode->setDepth(bestNode->getDepth());
            parentNode->setMapping(bestNode->getMapping()) ;
            bestNode->setCost(tmpCost)  ;
            bestNode->setDepth(tmpDepth) ;
            bestNode->setMapping(tmpMapping) ;
            parent = best;
            best = 0; // something other than parent so while loop continues
        }
    }
}


//---------------------------------------------------------------------------
// NAME: CompressMatchHeap
//
// INPUTS: (MatchHeap *heap) - match node heap to compress
//         (ULONG n) - limit used for bounding match node heap
//
// RETURN: (void)
//
// PURPOSE: Compress match node heap for the beginning of greedy
// search within the InexactGraphMatch function.  The first n nodes
// are left on the heap.  If there are more nodes on the heap, then
// the nodes with unique costs remain on the heap, and the rest are
// freed.  Note that the heap is assumed to already be in increasing
// order by cost, and for nodes having the same cost, in decreasing
// order by depth.
//---------------------------------------------------------------------------

void MatchHeap::CompressMatchHeap(ULONG n)
{
    MatchHeap *newHeap;
    MatchHeapNode node1;
    MatchHeapNode node2;
    
    newHeap = AllocateMatchHeap(n);
    
    // keep best n nodes
    while ((n > 0) && (! MatchHeapEmpty()))
    {
        ExtractMatchHeapNode( & node1);
        newHeap->InsertMatchHeapNode(& node1);
        n--;
    }
    
    // keep remaining nodes with unique costs
    while (! MatchHeapEmpty())
    {
        ExtractMatchHeapNode(& node2);
        if (node1.getCost() == node2.getCost())
            free(node2.getMapping());
        else
        {
            newHeap->InsertMatchHeapNode(& node2);
            node1.setCost(node2.getCost()) ;
        }
    }
    
    // free old heap and install new one in its place
    free(nodes);
    nodes = newHeap->getNodes();
    size = newHeap->getSize();
    numNodes = newHeap->getNumNodes();
    free(newHeap);
}

//---------------------------------------------------------------------------
// NAME: MergeMatchHeaps
//
// INPUTS: (MatchHeap *heap1) - match node list to be inserted from
//         (MatchHeap *heap2) - match node list to be inserted into
//
// RETURN:  (void)
//
// PURPOSE: Insert the match nodes of heap1 into heap2 while maintaining
// the order of heap2.  Ordering is by increasing cost, or if costs are
// equal, by decreasing depth.  The nodes of heap2 are removed, but the
// memory remains allocated.
//---------------------------------------------------------------------------

void MatchHeap::MergeMatchHeaps(MatchHeap *heap1)
{
    ULONG i;
    MatchHeapNode *node;
    
    for (i = 0; i < heap1->getNumNodes(); i++)
    {
        node = & heap1->getNodes()[i];
        InsertMatchHeapNode(node);
        node->setMapping(NULL) ;
    }
    heap1->numNodes = 0;
}

//---------------------------------------------------------------------------
// NAME: PrintMatchHeap
//
// INPUTS: (MatchHeap *heap) - match node heap to print
//
// RETURN: (void)
//
// PURPOSE: Print match node list.
//---------------------------------------------------------------------------

void MatchHeap::PrintMatchHeap()
{
    ULONG i;
    MatchHeapNode *node;
    
    printf("MatchHeap:\n");
    for (i = 0; i < numNodes; i++)
    {
        node = & nodes[i];
        printf("(%lu) ", i);
        node->PrintMatchHeapNode();
    }
}


























