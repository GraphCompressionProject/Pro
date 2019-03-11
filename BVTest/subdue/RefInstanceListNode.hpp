//
// Created by Macbook on 10/02/2019.
//

#ifndef SUBDUE_REFINSTANCELISTNODE_HPP
#define SUBDUE_REFINSTANCELISTNODE_HPP


#include "ListNode.hpp"
#include "InstanceList.hpp"
#include "ReferenceGraph.hpp"

    class RefInstanceListNode :ListNode<InstanceList>{
    private:
        
        ReferenceGraph *refGraph;
        bool firstPass;
        bool doExtend;
        
        
    public:
        
        RefInstanceListNode *getNext(){
            return (RefInstanceListNode *)ListNode<InstanceList>::getNext();
        }
        
        InstanceList *getInstance() const {
            return instance;
        }
        
        void setInstance(InstanceList *instance) {
            this->instance = instance;
        }
        
        void setNext(RefInstanceListNode *next) {
            this->next = next;
        }
        
        ReferenceGraph *getRefGraph() {
            return refGraph;
        };
        
        void setRefGraph(ReferenceGraph *refGraph){
            this->refGraph=refGraph;
        };
        
        bool isFirstPass() {
            return firstPass;
        };
        
        void setFirstPass(bool firstPass){
            this->firstPass = firstPass;
        };
        
        bool isDoExtend() {
            return doExtend;
        };
        
        void setDoExtend(bool doExtend){
            this->doExtend = doExtend ;
        };
        
        //------------------------------------------------------------------------------
        // NAME: AllocateRefInstanceListNode
        //
        // RETURN:  (RefInstanceListNode *)
        //
        // PURPOSE:  Create a RefInstanceListNode.
        //------------------------------------------------------------------------------
        
        static RefInstanceListNode *AllocateRefInstanceListNode()
        {
            RefInstanceListNode *refInstanceListNode = (RefInstanceListNode *)malloc(sizeof(RefInstanceListNode));
            
            refInstanceListNode->setInstance(InstanceList::AllocateInstanceList());
            refInstanceListNode->setRefGraph(NULL);
            refInstanceListNode->setNext(NULL);
            
            return refInstanceListNode;
        }
        
        //------------------------------------------------------------------------------
        // NAME: FreeRefInstanceListNode
        //
        // INPUT:  (RefInstanceListNode *)
        //
        // RETURN:  void
        //
        // PURPOSE: Free the input node.
        //------------------------------------------------------------------------------
        
        void FreeRefInstanceListNode()
        {
            getInstance()->FreeInstanceList();
            refGraph->FreeReferenceGraph();
        }
        
        BOOLEAN ExtendBoundaryInstances(Substructure *bestSub,
                                                             Params *parameters);
        
        
        void CreateExtendedGraphs(
                                                       Substructure *bestSub, LabeledGraph *graph,
                                                       Params *parameters);
        
    };






#endif //SUBDUE_REFINSTANCELISTNODE_HPP
