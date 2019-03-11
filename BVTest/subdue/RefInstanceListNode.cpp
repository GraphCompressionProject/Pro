//
// Created by Macbook on 10/02/2019.
//
#include "stdafx.h"
#include "RefInstanceListNode.hpp"

//---------------------------------------------------------------------------
// NAME: ExtendBoundaryInstances
//
// INPUTS:  (Substructure *bestSubList) - set of top n subs
//          (RefInstanceListNode *refInstanceListSet) - array of
//             refInstanceLists, one for each top-n sub
//          (Parameters *parameters) - system parameters
//
// RETURN:  BOOLEAN
//
// PURPOSE:  For each top-n sub we have created a reference graph and a
//           corresponding set of instances that are subsets of the top-n sub,
//           which we are attempting to grow into an instance of a top-n sub.
//           This function moves through the list of top-n subs and tries to
//           grow the instances associated with each sub with respect to the
//           reference graph.  Returns true if no extensions were possible,
//           signifying that we are done.
//---------------------------------------------------------------------------

BOOLEAN RefInstanceListNode::ExtendBoundaryInstances(Substructure *bestSub,
                                Params *parameters)
{
    Instance *bestInstance;
    InstanceList *bestSubInstanceList;
    InstanceList *extendedInstances = NULL;
    InstanceList *candidateInstances = NULL;
    InstanceListNode *instanceListNode;
    BOOLEAN empty = FALSE;
    BOOLEAN firstPass;
    BOOLEAN foundInstances;
    ReferenceGraph *refGraph;
    LabeledGraph *fullGraph;
    
    fullGraph = parameters->posGraph;
    extendedInstances = NULL;
    if (instance != NULL)
    {
        firstPass = this->firstPass;
        candidateInstances = InstanceList::AllocateInstanceList();
        bestSubInstanceList = instance;
        refGraph = this->refGraph;
        foundInstances = FALSE;
        if (bestSubInstanceList != NULL)
        {
            instanceListNode =(InstanceListNode *) bestSubInstanceList->getHead();
            // process each instance on the candidate list
            while (instanceListNode != NULL)
            {
                bestInstance = instanceListNode->getInstance();
                // try to extend the instance into a new set of instances, with
                // respect to the reference graph
                extendedInstances = bestInstance->ExtendConstrainedInstance( bestSub,
                                                              refGraph, fullGraph, parameters);
                // if we found extended instances then process them
                if (extendedInstances != NULL)
                {
                    foundInstances = TRUE;
                    // check to see which of the extended instances are actually
                    // still viable in terms of their ability to grow into the top-n
                    // sub we are processing
                    extendedInstances->ProcessExtendedInstances( candidateInstances,
                                             bestSub, refGraph, fullGraph,
                                             parameters);
                    // at this point the candidate instances are on the
                    // candidateInstances list
                    extendedInstances->FreeInstanceList();
                }
                
                // Since we only extend the ref graph after we run out of
                // extendable instances, we need to keep failed instances around
                // for another iteration.  It is possible this instance is good but
                // we just could not extend it until the ref graph was extended.
                // FirstPass is set to true if this is the first attempt to extend
                // instances after the ref graph has been extended.
                else if (!firstPass &&
                         !candidateInstances->MemberOfInstanceList(bestInstance))
                    candidateInstances->InstanceListInsert(bestInstance, FALSE);
                instanceListNode = (InstanceListNode *)instanceListNode->getNext();
            }
        }
        
        // If we could not create any extended instances and this is not the first
        // attempt immediately after the ref graph has been extended, then it is
        // time to extend the ref graph again.
        if (!foundInstances && !firstPass)
            doExtend = TRUE;
        
        // If we did not find any extended instances for this ref Graph and it is
        // the first attempt to find them after the ref graph has been extended,
        // then set empty to true so that this list node will be freed upon
        // function return.
        // We only do this when no extensions were possible on the first attempt
        // after the ref Graph has been extended or if we do not have any
        // remaining candidate instances.
        // Keep in mind that we keep instances that we could not extend if it is
        // possible that they could not be extended because the refGraph needed to
        // be extended first, so the candidate instance list will only be empty if
        // we have exhausted all options for all instances.
        if ((!foundInstances && firstPass) || (candidateInstances->getHead() == NULL))
            empty = TRUE;
         firstPass = FALSE;
        
        // If we found some candidates then store them
        if (candidateInstances->getHead() != NULL)
        {
            empty = FALSE;
            bestSubInstanceList->FreeInstanceList();
            instance = candidateInstances;
        }
        else
            candidateInstances->FreeInstanceList();
    }
    else
        empty = TRUE;
    
    return empty;
}

//------------------------------------------------------------------------------
// NAME:: CreateExtendedGraphs
//
// INPUT: (RefInstanceListNode *refInstanceListNode) - array of refInstanceLists
//        (Substructure *bestSubList) - list of top-n subs
//        (Graph *graph) - the full graph
//        (Parameters *parameters) - system parameters
//
// RETURN:  void
//
// PURPOSE:  Calls functions to extend each ref graph by one edge and possibly
//           one vertex in all possible ways.  The extension is only performed
//           if the instance evaluation has completely exhausted the current
//           extension, recognized by the doExtend flag.
//------------------------------------------------------------------------------

void RefInstanceListNode::CreateExtendedGraphs(
                          Substructure *bestSub, LabeledGraph *graph,
                          Params *parameters)
{
    ReferenceGraph *newRefGraph;
    
    // Extend the graphs
   
        if (doExtend)
        {
            newRefGraph = refGraph->ExtendRefGraph( bestSub, graph, parameters);
            refGraph->FreeReferenceGraph();
            refGraph = newRefGraph;
            firstPass = TRUE;
            doExtend = FALSE;
        }
    
}






























