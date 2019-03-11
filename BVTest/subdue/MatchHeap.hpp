//
// Created by Macbook on 10/02/2019.
//

#ifndef SUBDUE_MATCHHEAP_HPP
#define SUBDUE_MATCHHEAP_HPP


#include "MatchHeapNode.hpp"
#include "utility.hpp"

#define HeapParent(i) ((i - 1) / 2)
#define HeapLeftChild(i) ((2 * i) + 1)
#define HeapRightChild(i) ((2 * i) + 2)


    class MatchHeap {
        
    private:
        unsigned long size;      // number of nodes allocated in memory
        unsigned long numNodes;  // number of nodes in heap
        MatchHeapNode *nodes;
        
    public:
        unsigned long getSize() {
            return size;
        }
        
        void setSize(unsigned long size){
            MatchHeap::size =size;
        }
        
        unsigned long getNumNodes() {
            return numNodes;
        }
        
        void setNumNodes(unsigned long numNodes){
            MatchHeap::numNodes =numNodes ;
        }
        
        MatchHeapNode *getNodes() {
            return nodes ;
        }
        
        void setNodes(MatchHeapNode * nodes){
            MatchHeap::nodes =nodes;
        }
        
        
        void InsertMatchHeapNode(MatchHeapNode *node);
        BOOLEAN MatchHeapEmpty();
        void FreeMatchHeap();
        void ClearMatchHeap();
        void HeapifyMatchHeap();
        void ExtractMatchHeapNode( MatchHeapNode *node);
        void CompressMatchHeap( ULONG n);
        void MergeMatchHeaps(MatchHeap *heap1);
        void PrintMatchHeap();
        
        
        
        
        //---------------------------------------------------------------------------
        // NAME: AllocateMatchHeap
        //
        // INPUTS: (unsigned long size) - nodes of memory initially allocated to heap
        //
        // RETURN: (MatchHeap *) - pointer to newly-allocated empty match node heap
        //
        // PURPOSE: Allocate memory for a match node heap with enough room for size
        // nodes.
        //---------------------------------------------------------------------------
        
        static MatchHeap *AllocateMatchHeap(unsigned long size)
        {
            MatchHeap *heap;
            
            heap = (MatchHeap *) malloc(sizeof(MatchHeap));
            if (heap == NULL) utility::OutOfMemoryError("AllocateMatchHeap:MatchHeap");
            
            heap->setNumNodes(0);
            heap->setSize(size);
            heap->setNodes((MatchHeapNode *) malloc(size * sizeof(MatchHeapNode))) ;
            
            if (heap->getNodes() == NULL)
                utility::OutOfMemoryError("AllocateMatchHeap:heap->nodes");
            
            return heap;
        }
        
        
        
    };





#endif //SUBDUE_MATCHHEAP_HPP
