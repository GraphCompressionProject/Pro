//
//  InstanceList.hpp
//  Subdue_v1
//
//  Created by Macbook on 18/02/2019.
//  Copyright © 2019 Macbook. All rights reserved.
//

#ifndef InstanceList_hpp
#define InstanceList_hpp

#include <stdio.h>


#include "List.hpp"
#include "InstanceListNode.hpp"


    class InstanceList : public List<InstanceListNode>{
    

    public:

        InstanceListNode *getHead(){
            return List<InstanceListNode>::getHead();
        }

        void InstanceListInsert(Instance *instance, bool unique);
        bool MemberOfInstanceList(Instance *instance);
        ULONG ExamplesCovered( LabeledGraph *graph,ULONG numEgs, ULONG *egsVertexIndices, ULONG start);
        BOOLEAN InstancesOverlap();
        InstanceList *ExtendInstances(LabeledGraph *graph);
        bool InstanceListOverlap(Instance *instance);
        
        InstanceList *GetRecursiveInstances(LabeledGraph *graph, ULONG numInstances, ULONG recEdgeLabel);
        InstanceList *ExtendInstancesByEdge(LabeledGraph *g1, LabeledEdge *edge1, LabeledGraph *g2,
                                                          Params *parameters);
        InstanceList *FilterInstances(LabeledGraph *subGraph,
                                                    LabeledGraph *graph, Params *parameters);
        //---------------------------------------------------------------------------
        // NAME: FreeInstanceList
        //
        // INPUTS: (InstanceList *instanceList)
        //
        // RETURN: (void)
        //
        // PURPOSE: Deallocate memory of instance list.
        //---------------------------------------------------------------------------

            void FreeInstanceList()
            {
                
                InstanceListNode *instanceListNode;
                InstanceListNode *instanceListNode2;
                if (head != NULL)
                {
                    instanceListNode = head;
                    while (instanceListNode != NULL)
                    {
                        instanceListNode2 = instanceListNode;
                        
                        instanceListNode = (InstanceListNode *) instanceListNode->getNext();
                        instanceListNode2->FreeInstanceListNode();
                    }
                }

            }
        
        //---------------------------------------------------------------------------
        // NAME: AllocateInstanceList
        //
        // INPUTS: (void)
        //
        // RETURN: (InstanceList *) - newly-allocated empty instance list
        //
        // PURPOSE: Allocate and return an empty instance list.
        //---------------------------------------------------------------------------
        
        static InstanceList *AllocateInstanceList(void)
        {
            InstanceList *instanceList;
            
            instanceList = new InstanceList();
            if (instanceList == NULL)
                utility::OutOfMemoryError("AllocateInstanceList:instanceList");
            instanceList->setHead(NULL) ;
            return instanceList;
        }
        ULONG CountInstances();
        
        
        //---------------------------------------------------------------------------
        // NAME: CollectRecursiveInstances
        //
        // INPUTS: (Instance **instanceMap) - map of instances to new instances
        //              in which they reside
        //         (ULONG numInstances) - number of original instances, i.e., length
        //              of instanceMap
        //
        // RETURN: (InstanceList *) - list of new instances
        //
        // PURPOSE: Creates and returns a list of new instances, which are all the
        // unique instances stored in the instanceMap array.
        //---------------------------------------------------------------------------
        
        static InstanceList *CollectRecursiveInstances(Instance **instanceMap, ULONG numInstances)
        {
            InstanceList *recInstances;
            ULONG i, j;
            
            recInstances = AllocateInstanceList();
            for (i = 0; i < numInstances; i++)
                if (instanceMap[i] != NULL)
                {
                    recInstances->InstanceListInsert(instanceMap[i], FALSE);
                    
                    for (j = i + 1; j < numInstances; j++)
                        if (instanceMap[j] == instanceMap[i])
                            instanceMap[j] = NULL;
                    
                    instanceMap[i] = NULL;
                }
            return recInstances;
        }
        
        void PrintInstanceList( LabeledGraph *graph,
                                             LabelList *labelList);
        
        void ProcessExtendedInstances(
                                                    InstanceList *candidateInstances,
                                                    Substructure *bestSub,
                                                    ReferenceGraph *refGraph, LabeledGraph *fullGraph,
                                                    Params *parameters);
        
        BOOLEAN CheckForSeedInstanceOverlap(Instance *candidateInstance);
        
        InstanceList *PruneCandidateList(Substructure *bestSub,
                                                       ReferenceGraph *refGraph,
                                                       Params *parameters);
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
    };



#endif /* InstanceList_hpp */
