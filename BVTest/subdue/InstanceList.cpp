//
//  InstanceList.cpp
//  Subdue_v1
//
//  Created by Macbook on 18/02/2019.
//  Copyright © 2019 Macbook. All rights reserved.
//
#include "stdafx.h"
#include "InstanceList.hpp"
#include "LabeledGraph.hpp"
#include "Substructure.hpp"
#include "ReferenceGraph.hpp"

#include "Instance.hpp"

//---------------------------------------------------------------------------
// NAME: InstanceListOverlap
//
// INPUTS: (Instance *instance) - instance to check for overlap
//         (InstanceList *instanceList) - instances to check for overlap with
//                                        instance
//
// RETURN: (BOOLEAN)
//
// PURPOSE: Check if given instance overlaps at all with any instance
// in the given instance list.
//---------------------------------------------------------------------------

bool InstanceList::InstanceListOverlap(Instance *instance)
{
    InstanceListNode *instanceListNode;
    BOOLEAN overlap = FALSE;
    
    instanceListNode = getHead();
    while ((instanceListNode != NULL) && (! overlap))
    {
        if (instanceListNode->getInstance() != NULL)
            if (instance->InstanceOverlap(instanceListNode->getInstance()))
                overlap = TRUE;
        instanceListNode = (InstanceListNode *)instanceListNode->getNext();
    }
    return overlap;
}


//---------------------------------------------------------------------------
// NAME: InstanceListInsert
//
// INPUTS: (Instance *instance) - instance to insert
//         (InstanceList *instanceList) - list to insert into
//         (BOOLEAN unique) - if TRUE, then instance inserted only if
//                            unique, and if not unique, deallocated
//
// RETURN: (void)
//
// PURPOSE: Insert given instance on to given instance list.  If
// unique=TRUE, then instance must not already exist on list, and if
// so, it is deallocated.  If unique=FALSE, then instance is merely
// inserted at the head of the instance list.
//---------------------------------------------------------------------------

void InstanceList::InstanceListInsert(Instance *instance, bool unique)
{
    InstanceListNode *instanceListNode;
    
    if ((! unique) ||
        (unique && (! MemberOfInstanceList(instance))))
    {
        instanceListNode = InstanceListNode::AllocateInstanceListNode(instance);
        instanceListNode->setNext(this->head) ;
        head = instanceListNode;
    }
    else
        instance->FreeInstance();
}

bool InstanceList::MemberOfInstanceList(Instance *instance){
    InstanceListNode *instanceListNode;
    BOOLEAN found = FALSE;
    
   
        instanceListNode = head;
        while ((instanceListNode != NULL) && (! found))
        {
            if (instance->InstanceMatch( instanceListNode->getInstance()))
                found = TRUE;
            instanceListNode = (InstanceListNode *)instanceListNode->getNext();
        }
 
    return found;
}

//---------------------------------------------------------------------------
// NAME: ExamplesCovered
//
// INPUTS: (InstanceList *instanceList) - instances of substructure
//         (Graph *graph) - graph containing instances and examples
//         (ULONG numEgs) - number of examples to consider
//         (ULONG *egsVertexIndices) - vertex indices of each examples
//           starting vertex
//         (ULONG start) - start vertex for current increment
//
// RETURN: (ULONG) - number of examples covered by instances
//
// PURPOSE: Return the number of examples, whose starting vertices are
// stored in egsVertexIndices, are covered by an instance in
// instanceList.  Note that one example may contain more than one
// instance.
//---------------------------------------------------------------------------

ULONG InstanceList::ExamplesCovered( LabeledGraph *graph,ULONG numEgs, ULONG *egsVertexIndices, ULONG start)
{
    ULONG i;
    ULONG egStartVertexIndex;
    ULONG egEndVertexIndex;
    ULONG instanceVertexIndex;
    InstanceListNode *instanceListNode;
    BOOLEAN found;
    ULONG numEgsCovered;
    
    numEgsCovered = 0;
  
    // for each example, look for a covering instance
    for (i = 0; i < numEgs; i++)
    {
        // find example's starting and ending vertex indices
        egStartVertexIndex = egsVertexIndices[i];
        if (egStartVertexIndex >= start)
        {
            if (i < (numEgs - 1))
                egEndVertexIndex = egsVertexIndices[i + 1] - 1;
            else
                egEndVertexIndex = graph->getNumVertices() - 1;
            // look for an instance whose vertices are in range
            instanceListNode = getHead();
            found = FALSE;
            while ((instanceListNode != NULL) && (! found))
            {
                // can check any instance vertex, so use the first
                instanceVertexIndex = instanceListNode->getInstance()->getVertices()[0] ;
                if ((instanceVertexIndex >= egStartVertexIndex) &&
                    (instanceVertexIndex <= egEndVertexIndex))
                {
                    // found an instance covering this example
                    numEgsCovered++;
                    found = TRUE;
                }
                instanceListNode = (InstanceListNode *)instanceListNode->getNext();
            }
        }
    
    }
    return numEgsCovered;
}

//---------------------------------------------------------------------------
// NAME: InstancesOverlap
//
// INPUTS: (InstanceList *instanceList)
//
// RETURN: (BOOLEAN) - TRUE if any pair of instances overlap
//
// PURPOSE: Check if any two instances in the given list overlap.  If
// so, return TRUE, else return FALSE.
//---------------------------------------------------------------------------

BOOLEAN InstanceList::InstancesOverlap()
{
    InstanceListNode *instanceListNode1;
    InstanceListNode *instanceListNode2;
    BOOLEAN overlap = FALSE;
    
        instanceListNode1 = head;
        while ((instanceListNode1 != NULL) && (! overlap))
        {
            instanceListNode2 = (InstanceListNode *) instanceListNode1->getNext();
            while ((instanceListNode2 != NULL) && (! overlap))
            {
                if ((instanceListNode1->getInstance() != NULL) &&
                    (instanceListNode2->getInstance() != NULL) &&
                    (instanceListNode1->getInstance()->InstanceOverlap(instanceListNode2->getInstance())))
                    overlap = TRUE;
                instanceListNode2 = (InstanceListNode *) instanceListNode2->getNext();
            }
            instanceListNode1 = (InstanceListNode *) instanceListNode1->getNext();
        }
    return overlap;
}

//---------------------------------------------------------------------------
// NAME: ExtendInstances
//
// INPUTS: (InstanceList *instanceList) - instances to be extended
//         (Graph *graph) - graph containing substructure instances
//
// RETURN: (InstanceList *) - list of extended instances
//
// PURPOSE: Create and return a list of new instances by extending the
// given substructure's instances by one edge (or edge and new vertex)
// in all possible ways based on given graph.
//---------------------------------------------------------------------------

InstanceList *InstanceList::ExtendInstances(LabeledGraph *graph)
{
    InstanceList *newInstanceList;
    InstanceListNode *instanceListNode;
    Instance *instance;
    Instance *newInstance;
    ULONG v;
    ULONG e;
    LabeledVertex vertex;
    LabeledEdge edge;

    newInstanceList = AllocateInstanceList();
    instanceListNode = head;


    while (instanceListNode != NULL)
    {
        instance = (Instance *) instanceListNode->getInstance();        
        graph->MarkInstanceEdges(instance, TRUE);
        
        for (v = 0; v < instance->getNumVertices(); v++)
        {
            vertex = graph->getVertices()[instance->getVertices()[v]];
            for (e = 0; e < vertex.getNumEdges(); e++)
            {
                edge = graph->getEdges()[vertex.getEdges()[e]];
                if (! edge.isUsed())
                {
                    // add new instance to list
                    newInstance = instance->CreateExtendedInstance( instance->getVertices()[v], vertex.getEdges()[e], graph);
                    newInstanceList->InstanceListInsert(newInstance, TRUE);
                }
            }
        }
        graph->MarkInstanceEdges(instance, FALSE);
        instanceListNode = (InstanceListNode *) instanceListNode->getNext();
    }
    return newInstanceList;

}


//---------------------------------------------------------------------------
// NAME: GetRecursiveInstances
//
// INPUTS: (Graph *graph) - graph in which to look for instances
//         (InstanceList *instances) - instances in which to look for pairs
//                connected by an edge of the given label
//         (ULONG numInstances) - number of instances given
//         (ULONG recEdgeLabel) - label of edge connecting recursive instance
//                pairs
//
// RETURN: (InstanceList *) - list of recursive instances
//
// PURPOSE: Builds and returns a new instance list, where each new instance
// may contain one or more of the original instances connected by edges
// with the given edge label.  NOTE: assumes instances' edges have already
// been marked as used.
//
// ***** TODO: Ensure that connecting edge starts at the same vertex in
// each pair of instances.
//---------------------------------------------------------------------------

InstanceList *InstanceList::GetRecursiveInstances(LabeledGraph *graph,
                                    ULONG numInstances, ULONG recEdgeLabel)
{
    Instance **instanceMap;
    InstanceList *recInstances;
    InstanceListNode *instanceListNode1;
    InstanceListNode *instanceListNode2;
    Instance *instance1;
    Instance *instance2;
    LabeledVertex *vertex1;
    ULONG i, i1, i2;
    ULONG v1;
    ULONG v2Index;
    ULONG e;
    LabeledEdge *edge;
    
    // Allocate instance map, where instanceMap[i]=j implies the original
    // instance i is now part of possibly new instance j
    instanceMap = (Instance **) malloc(numInstances * sizeof(Instance *));
    if (instanceMap == NULL)
        utility::OutOfMemoryError("GetRecursiveInstances:instanceMap");
    i = 0;
    instanceListNode1 = head;
    while (instanceListNode1 != NULL)
    {
        instanceMap[i] = instanceListNode1->getInstance();
        instanceListNode1 = (InstanceListNode *)instanceListNode1->getNext();
        i++;
    }
    
    // search instance list for a connected pair
    i1 = 0;
    instanceListNode1 = head;
    while (instanceListNode1 != NULL)
    {
        instance1 = instanceListNode1->getInstance();
        for (v1 = 0; v1 < instance1->getNumVertices(); v1++)
        {
            vertex1 = & graph->getVertices()[instance1->getVertices()[v1]];
            for (e = 0; e < vertex1->getNumEdges(); e++)
            {
                edge = & graph->getEdges()[vertex1->getEdges()[e]];
                if ((! edge->isUsed()) && (edge->getLabel() == recEdgeLabel))
                {
                    // search instance list for another instance involving edge
                    v2Index = edge->getE().second;
                    if (edge->getE().second == instance1->getVertices()[v1])
                        v2Index = edge->getE().first;
                    i2 = i1 + 1;
                    instanceListNode2 = (InstanceListNode *)instanceListNode1->getNext();
                    while (instanceListNode2 != NULL)
                    {
                        instance2 = instanceListNode2->getInstance();
                        if (instance2->InstanceContainsVertex( v2Index))
                        {
                            // found pair, so update instance map and new instances
                            Instance::AddRecursiveInstancePair(i1, i2, instance1, instance2,
                                                     vertex1->getEdges()[e], edge,
                                                     numInstances, instanceMap);
                        }
                        i2++;
                        instanceListNode2 = (InstanceListNode *)instanceListNode2->getNext();
                    }
                }
            }
        }
        i1++;
        instanceListNode1 = (InstanceListNode *)instanceListNode1->getNext();
    }
    
    recInstances = CollectRecursiveInstances(instanceMap, numInstances);
    free(instanceMap);
    return recInstances;
}

//---------------------------------------------------------------------------
// NAME: CountInstances
//
// INPUTS: (InstanceList *instanceList) - list of instances
//
// RETURN: (ULONG) - number of instances in instanceList.
//
// PURPOSE: Return number of instances in instance list.
//---------------------------------------------------------------------------

ULONG InstanceList::CountInstances()
{
    ULONG i = 0;
    InstanceListNode *instanceListNode;

        instanceListNode = head;
        while (instanceListNode != NULL)
        {
            i++;
            instanceListNode = (InstanceListNode *)instanceListNode->getNext();
        }
    return i;
}

//---------------------------------------------------------------------------
// NAME: ExtendInstancesByEdge
//
// INPUTS: (InstanceList *instanceList) - instances to extend by one edge
//         (Graph *g1) - graph whose instances we are looking for
//         (Edge *edge1) - edge in g1 by which to extend each instance
//         (Graph *g2) - graph containing instances
//         (Parameters parameters)
//
// RETURN: (InstanceList *) - new instance list with extended instances
//
// PURPOSE: Attempts to extend each instance in instanceList by an
// edge from graph g2 that matches the attributes of the given edge in
// graph g1.  Returns a new (possibly empty) instance list containing
// the extended instances.  The given instance list is de-allocated.
//---------------------------------------------------------------------------

InstanceList *InstanceList::ExtendInstancesByEdge(LabeledGraph *g1, LabeledEdge *edge1, LabeledGraph *g2,
                                                  Params *parameters)
{
    InstanceList *newInstanceList;
    InstanceListNode *instanceListNode;
    Instance *instance;
    Instance *newInstance;
    ULONG v2;
    ULONG e2;
    LabeledEdge *edge2;
    LabeledVertex *vertex2;
    
    newInstanceList = AllocateInstanceList();
    // extend each instance
    instanceListNode = this->getHead();
    while (instanceListNode != NULL)
    {
        instance = instanceListNode->getInstance();
        g2->MarkInstanceEdges(instance, TRUE);
        // consider extending from each vertex in instance
        for (v2 = 0; v2 < instance->getNumVertices(); v2++)
        {
            vertex2 = & g2->getVertices()[instance->getVertices()[v2]];
            for (e2 = 0; e2 < vertex2->getNumEdges(); e2++)
            {
                edge2 = & g2->getEdges()[vertex2->getEdges()[e2]];
                if ((! edge2->isUsed()) &&
                    (LabeledGraph::EdgesMatch(g1, edge1, g2, edge2, parameters)))
                {
                    // add new instance to list
                    newInstance =
                    instance->CreateExtendedInstance( instance->getVertices()[v2],
                                           vertex2->getEdges()[e2], g2);
                    newInstanceList->InstanceListInsert(newInstance, TRUE);
                }
            }
        }
        g2->MarkInstanceEdges(instance, FALSE);
        instanceListNode = (InstanceListNode *)instanceListNode->getNext();
    }
    this->FreeInstanceList();
    return newInstanceList;
}


//---------------------------------------------------------------------------
// NAME: FilterInstances
//
// INPUTS: (Graph *subGraph) - graph that instances must match
//         (InstanceList *instanceList) - list of instances
//         (Graph *graph) - graph containing instances
//         (Parameters *parameters)
//
// RETURN: (InstanceList *) - filtered instance list
//
// PURPOSE: Creates and returns a new instance list containing only
// those instances matching subGraph.  If
// parameters->allowInstanceOverlap=FALSE, then remaining instances
// will not overlap.  The given instance list is de-allocated.
//---------------------------------------------------------------------------

InstanceList *InstanceList::FilterInstances(LabeledGraph *subGraph,
                              LabeledGraph *graph, Params *parameters)
{
    InstanceListNode *instanceListNode;
    Instance *instance;
    InstanceList *newInstanceList;
    LabeledGraph *instanceGraph;
    double thresholdLimit;
    double matchCost;
    
    newInstanceList = InstanceList::AllocateInstanceList();
  
        instanceListNode = (InstanceListNode *) getHead();
        while (instanceListNode != NULL)
        {
            if (instanceListNode->getInstance() != NULL)
            {
                instance = instanceListNode->getInstance();
                if (parameters->allowInstanceOverlap ||
                    (! newInstanceList-InstanceListOverlap(instance)))
                {
                    thresholdLimit = parameters->threshold *
                    (instance->getNumVertices() + instance->getNumEdges());
                    instanceGraph = instance->InstanceToGraph( graph);
                    if (subGraph->GraphMatch( instanceGraph, parameters->labelList,
                                   thresholdLimit, & matchCost, NULL))
                    {
                        if (matchCost < instance->getMinMatchCost())
                            instance->setMinMatchCost(matchCost) ;
                        newInstanceList->InstanceListInsert(instance, FALSE);
                    }
                    instanceGraph->FreeGraph();
                }
            }
            instanceListNode =(InstanceListNode *) instanceListNode->getNext();
        }
    this->FreeInstanceList();
    return newInstanceList;
}

//---------------------------------------------------------------------------
// NAME: PrintInstanceList
//
// INPUTS: (InstanceList *instanceList) - list of instances
//         (Graph *graph) - graph containing instances
//         (LabelList *labelList) - labels used in input graph
//
// RETURN: (void)
//
// PURPOSE: Print array of instances.
//---------------------------------------------------------------------------

void InstanceList::PrintInstanceList( LabeledGraph *graph,
                       LabelList *labelList)
{
    ULONG i = 0;
    InstanceListNode *instanceListNode;
    

        instanceListNode = head;
        while (instanceListNode != NULL)
        {
            printf("\n  Instance %lu:\n", i + 1);
            instanceListNode->getInstance()->PrintInstance( 0, graph, labelList);
            instanceListNode = (InstanceListNode *)instanceListNode->getNext();
            i++;
        }
    
}


//----------------------------------------------------------------------------
// NAME:: ProcessExtendedInstances
//
// INPUT: (InstanceList *extendedInstances) - instances to extend
//        (InstanceList candidateInstances) - store sgiso matches here
//        (Substructure *bestSub) - the corresponding top-n sub
//        (ReferenceGraph *refGraph) - the corresponding ref graph
//        (Graph *fullGraph) - complete graph
//        (Parameters *parameters) - system parms
//
// RETURN:  void
//
// PURPOSE: Take an input set of instances that have been extended and check to
//          see if they are still potential matches to their corresponding
//          top-n sub.
//----------------------------------------------------------------------------

void InstanceList::ProcessExtendedInstances(
                              InstanceList *candidateInstances,
                              Substructure *bestSub,
                              ReferenceGraph *refGraph, LabeledGraph *fullGraph,
                              Params *parameters)
{
    InstanceListNode *instanceListNode;
    Instance *gInstance;
    Instance *instance;
    Substructure *newSub;
    BOOLEAN foundMatch;
    ULONG edgeIndex;
    ULONG v1;
    ULONG v2;
    ULONG index;
    
    if (head == NULL)
    {
        fprintf(stderr, "extendedInstances NULL\n");
        exit(1);
    }
    instanceListNode = head;
    
    // Look at each instance extension and keep the ones that are still subsets
    // of the top-n sub
    index = 1;
    while (instanceListNode != NULL)
    {
        instance = (Instance *)instanceListNode->getInstance();
        
        // create a copy of the instance and make it reference the full graph
        gInstance = instance->CreateGraphRefInstance( refGraph);
        newSub = gInstance->CreateSubFromInstance( fullGraph);
        
        // check to see if this instance is a subset of one of the best subs
        // first try graph match, then try sgiso
        
        foundMatch =newSub->getDefinition()->CheckForSubgraph( bestSub->getDefinition(), parameters);
        // We found this instance within (subgraph) a top n sub, so it is a keeper
        if (foundMatch && (!candidateInstances->MemberOfInstanceList(instance)))
        {
            // candidateInstances is the list of instances that are candidates for
            // further expansion
            candidateInstances->InstanceListInsert(instance, FALSE);
        }
        // this instance does not match so mark the newly added refEdge as bad
        else if(!foundMatch)
        {
            edgeIndex = instance->getEdges()[instance->getNewEdge()];
            v1 = refGraph->getEdges()[edgeIndex].getVertex1();
            v2 = refGraph->getEdges()[edgeIndex].getVertex2();
            
            // we have to ensure that this instance truly failed (and not just a
            // duplicate) before we mark the vertices and edge as invalid
            if (!bestSub->getDefinition()->VertexInSub(
                             &fullGraph->getVertices()[refGraph->getVertices()[v1].getMap()]) ||
                !bestSub->getDefinition()->VertexInSub(
                             &fullGraph->getVertices()[refGraph->getVertices()[v2].getMap()]))
            {
                refGraph->getEdges()[edgeIndex].setFailed(TRUE) ;
                refGraph->MarkVertices( v1, v2);
            }
        }
        delete newSub;
        gInstance->FreeInstance();
        instanceListNode = (InstanceListNode *)instanceListNode->getNext();
        index++;
    }
    
    
    
    
    
}


//---------------------------------------------------------------------------
// NAME:  CheckForSeeInstanceOverlap
//
// INPUT:  (Instance *candidateInstance)
//         (InstanceList *instanceList)
//
// RETURN:  BOOLEAN
//
// PURPOSE: Called by FindInitialBoundaryInstances.  Make sure we do not add
//          seed instances with overlapping vertices.
//---------------------------------------------------------------------------

BOOLEAN InstanceList::CheckForSeedInstanceOverlap(Instance *candidateInstance)
{
    InstanceListNode *instanceListNode;
    Instance *subInstance;
    ULONG i, j;
    

        for (i=0;i<candidateInstance->getNumVertices();i++)
        {
            instanceListNode = head;
            while (instanceListNode != NULL)
            {
                subInstance = instanceListNode->getInstance();
                for (j=0;j<subInstance->getNumVertices();j++)
                {
                    if (candidateInstance->getVertices()[i] == subInstance->getVertices()[j])
                        return TRUE;
                }
                instanceListNode = (InstanceListNode *) instanceListNode->getNext();
            }
        }

    return FALSE;
}


//------------------------------------------------------------------------------
// NAME:  PruneCandidateList
//
// INPUTS:  (Substructure *bestSub)
//          (ReferenceGraph *refGraph)
//          (InstanceList *instanceList)
//          (Parameters *parameters)
//
// RETURN:  (InstanceList *)
//
// PURPOSE:  After an instance has been found and added to the increment
//           instance list, we can remove any other candidate instances that
//           have overlapping vertices.
//------------------------------------------------------------------------------

InstanceList *InstanceList::PruneCandidateList(Substructure *bestSub,
                                 ReferenceGraph *refGraph,
                                 Params *parameters)
{
    InstanceListNode *instanceListNode;
    InstanceList *keepList = AllocateInstanceList();
    BOOLEAN overlap;
    Instance *gInstance;
    
    instanceListNode = head;
    
    while (instanceListNode != NULL)
    {
        gInstance = instanceListNode->getInstance()->CreateGraphRefInstance( refGraph);
        overlap = gInstance->CheckInstanceForOverlap( bestSub, parameters);
        gInstance->FreeInstance();
        
        if (!overlap)
            keepList->InstanceListInsert(instanceListNode->getInstance(), FALSE);
        instanceListNode = (InstanceListNode *) instanceListNode->getNext();
    }
    FreeInstanceList();
    return keepList;
}










































