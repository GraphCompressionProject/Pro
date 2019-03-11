//
// Created by Macbook on 10/02/2019.
//
#include "stdafx.h"
#include "RefInstanceList.hpp"
#include "Substructure.hpp"
#include "Instance.hpp"
#include "RefInstanceListNode.hpp"

//----------------------------------------------------------------------------
// NAME: RemoveFromList
//
// INPUT:  (RefInstanceList *refInstanceList)
//         (RefInstanceListNode **currentNode)
//
// RETURN:  void
//
// PURPOSE:  Remove a node from the ref instance list and updates the pointers.
//----------------------------------------------------------------------------

void RefInstanceList::RemoveFromList(RefInstanceListNode **currentNode)
{
    RefInstanceListNode *cNode;
    
    cNode = *currentNode;
    
    head = (RefInstanceListNode *)cNode->getNext();
    cNode->FreeRefInstanceListNode();
    *currentNode = head;
}

//-----------------------------------------------------------------------------
// NAME:  ProcessInstancesForSub
//
// INPUT:  (Substructure *bestSub)-  a sub from the top-n list
//         (RefInstanceList *refInstanceList) - initial list of seed instances
//                                              for the sub
//         (InstanceList *finalInstanceList)  - final set of complete instances
//                                              for the sub
//         (Parameters *parameters) - system parms
//
// RETURN: (void)
//
// PURPOSE: Grow the seed instances into instances of a top-n substructure.
//-----------------------------------------------------------------------------

void RefInstanceList::ProcessInstancesForSub(Substructure *bestSub,
                            InstanceList *finalInstanceList,
                            Params *parameters)
{
    RefInstanceListNode *refInstanceListNode = NULL;
    ReferenceGraph *refGraph = NULL;
    Instance *gInstance;
    InstanceList *bestSubInstanceList;
    InstanceListNode *instanceListNode;
    InstanceList *remainingInstances = NULL;
    Instance *instance;
    Substructure *newSub;
    BOOLEAN listEmpty;
    BOOLEAN instanceAdded;
    BOOLEAN empty;
    BOOLEAN doPrune;
    ULONG index = 1;
    ULONG index2;
    ULONG kept;
    ULONG maxVertexCount;
    
    // Now we go through the process of trying to grow the seed instances into
    // instances of the top-n substructures.
    // Any instance returned on the instance list of a top-n sub must be a
    // subset of a top-n sub.  We now must check all of the instances and see if
    // they are an exact match to the top-n sub.
    refInstanceListNode = head;
    
    // Each refInstanceList has multiple refInstanceListNodes, each containing
    // an instance list and a refGraph
    while (refInstanceListNode != NULL)
    {
        empty = FALSE;
        while (!empty)
        {
            bestSubInstanceList = refInstanceListNode->getInstance();
            refGraph = refInstanceListNode->getRefGraph();
            remainingInstances = NULL;
            if (bestSubInstanceList != NULL)
            {
                instanceListNode = bestSubInstanceList->getHead();
            }
            else
                instanceListNode = NULL;
            
            listEmpty = TRUE;
            remainingInstances = NULL;
            
            // check every instance to see if it is an exact match to the sub
            index2 = 0;
            kept = 0;
            doPrune = FALSE;
            maxVertexCount = 0;
            
            while (instanceListNode != NULL)
            {
                index2++;
                instance = instanceListNode->getInstance();
                
                // This match check is a little convoluted at the moment.
                // It could be simplified by converting the ref instance directly
                // to a graph.
                
                // Create a copy of the instance and reference the full graph.
                // We have to do that to turn it into a sub that we can use for a
                // match check.
                gInstance = instance->CreateGraphRefInstance( refGraph);
                newSub = gInstance->CreateSubFromInstance( parameters->posGraph);
                instanceAdded = FALSE;
                if (parameters->outputLevel > 2)
                {
                    printf("\ninstance num: %lu\n", index2);
                    gInstance->PrintInstance( 0, parameters->posGraph, parameters->labelList);
                }
                
                // Check to see if the instance is an exact match to the bestSub
                if (newSub->getDefinition()->GraphMatch( bestSub->getDefinition(),
                               parameters->labelList, 0.0, NULL, NULL))
                {
                    // Update the increment data structure to reflect new instance.
                    // The instance will not be added if there are vertices that
                    // overlap with previously-added instances, from this or
                    // previous increments.
                    instanceAdded = gInstance->AddInstanceToIncrement(newSub , parameters);
                    
                    // keeping a final list of instances is technically unnecessary
                    if (instanceAdded)
                    {
                        finalInstanceList->InstanceListInsert(gInstance, FALSE);
                        doPrune = TRUE;
                    }
                }
                
                // If not then keep the instance around for further extension
                else
                {
                    if (listEmpty)
                    {
                        remainingInstances = InstanceList::AllocateInstanceList();
                        listEmpty = FALSE;
                    }
                    kept++;
                    remainingInstances->InstanceListInsert(instance, TRUE);
                    gInstance->FreeInstance();
                    if (instance->getNumVertices() > maxVertexCount)
                        maxVertexCount = instance->getNumVertices();
                }
                newSub->FreeSub();
                instanceListNode = (InstanceListNode *) instanceListNode->getNext();
            }
            
            refInstanceListNode->getInstance()->FreeInstanceList();
            refInstanceListNode->setInstance(NULL) ;
            if (parameters->outputLevel > 2)
            {
                printf("Examined %lu candidate instances, kept %lu for extension\n",
                       index2, kept);
                printf("maxVertexCount = %lu\n", maxVertexCount);
            }
            
            // If we found an instance and added it to the increment, then we can
            // prune any remaining instances that overlap, since we never
            // add duplicate instances
            if (doPrune && (remainingInstances != NULL))
                remainingInstances =
                 remainingInstances->PruneCandidateList(bestSub, refInstanceListNode->getRefGraph(),
                                   parameters);
            
            // Store the remaining instances for further extension.
            // The list may be empty.
            if (remainingInstances != NULL)
            {
                if (parameters->outputLevel > 2)
                    printf("Extend next set: %lu\n", index);
                refInstanceListNode->setInstance(remainingInstances) ;
                
                // At this point we have removed any instance that was an exact
                // match to a top-n sub.
                
                // Extend the ref graphs for the next iteration.
                refInstanceListNode->CreateExtendedGraphs( bestSub,
                                     parameters->posGraph, parameters);
                
                // For each instance on the list, returns the next extension of
                // the instance that is either a subset or equal to an existing
                // top-n sub.  If no instance could be extended then we are done.
                empty =  refInstanceListNode->ExtendBoundaryInstances( bestSub,
                                                parameters);
            }
            else
            {
                empty = TRUE;
                //printf("Empty: %lu\n", index);
            }
            index++;
        }
        RemoveFromList( &refInstanceListNode);
    }
}
