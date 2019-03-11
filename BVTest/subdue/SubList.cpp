//
//  SubList.cpp
//  Subdue_v1
//
//  Created by Macbook on 18/02/2019.
//  Copyright © 2019 Macbook. All rights reserved.
//

#include "stdafx.h"
#include "SubList.hpp"
#include "Substructure.hpp"
#include "Increment.hpp"
#include "GraphWriter.hpp"
#include "RefInstanceList.hpp"
#include "RefInstanceListNode.hpp"









//---------------------------------------------------------------------------
// NAME: FreeSubList
//
// INPUTS: (SubList *subList) - pointer to beginning of list to be freed
//
// RETURN:  void
//
// PURPOSE: Free memory in subList, including substructures pointed to.
//---------------------------------------------------------------------------

void SubList::FreeSubList()
{
    SubListNode *subListNode1 = NULL;
    SubListNode *subListNode2 = NULL;
    
    subListNode1 = head;
    while (subListNode1 != NULL)
    {
        subListNode2 = subListNode1;
        subListNode1 = (SubListNode *)subListNode1->getNext();
        // ((Substructure *) subListNode2->getInstance())->FreeSub();
        // free(subListNode2);
    }
}

//---------------------------------------------------------------------------
// NAME:  WriteResultsToFile
//
// INPUTS: (SubList *subList) - pointer to list of substructures
//         (FILE *subsFile) - file holding substructures
//         (Increment *increment) - current increment
//         (Parameters *parameters) - system parameters
//
// RETURN: (void)
//
// PURPOSE: Write results from increment to file.
//---------------------------------------------------------------------------

void SubList::WriteResultsToFile( FILE *subsFile,
                        Increment *increment, Params *parameters)
{
    SubListNode *subListNode = NULL;
    ULONG subSize;
    ULONG incSize;
    GraphWriter ww;
    incSize = increment->getNumPosVertices() + increment->getNumPosEdges() +
    increment->getNumNegVertices() + increment->getNumNegEdges();
 
        subListNode = head;
        if(subListNode != NULL)
        {
            subSize = subListNode->getInstance()->getDefinition()->getNumVertices() +
            subListNode->getInstance()->getDefinition()->getNumEdges();
            
            ww.WriteGraphToFile(subsFile, subListNode->getInstance()->getDefinition(),
                             parameters->labelList, 0, 0,
                             subListNode->getInstance()->getDefinition()->getNumVertices(), TRUE);
        }
    
}

//---------------------------------------------------------------------------
// NAME: MemberOfSubList
//
// INPUTS: (Substructure *sub)
//         (SubList *subList)
//         (LabelList *labelList)
//
// RETURN: (BOOLEAN)
//
// PURPOSE: Check if the given substructure's definition graph exactly
// matches a definition of a substructure on the subList.
//---------------------------------------------------------------------------

BOOLEAN SubList::MemberOfSubList(Substructure *sub, LabelList *labelList)
{
    SubListNode *subListNode;
    BOOLEAN found = FALSE;
    
        subListNode = head;
        while ((subListNode != NULL) && (! found))
        {
            if (sub->getDefinition()->GraphMatch(  subListNode->getInstance()->getDefinition(),
                           labelList, 0.0, NULL, NULL))
                found = TRUE;
            subListNode = (SubListNode *) subListNode->getNext();
        }
    return found;
}

//---------------------------------------------------------------------------
// NAME: SubListInsert
//
// INPUTS: (Substructure *sub) - substructure to be inserted
//         (SubList *subList) - list to be inserted in to
//         (ULONG max) - maximum number of substructures or different
//                       substructure values allowed on list;
//                       max = 0 means max = infinity
//         (BOOLEAN valueBased) - TRUE if list limited by different
//                                values; otherwise, limited by
//                                different substructures
//         (LabelList *labelList) - needed for checking sub equality
//
// RETURN: (void)
//
// PURPOSE: Inserts sub into subList, if not already there.  List is
// kept in decreasing order by substructure value.  If valueBased =
// TRUE, then max represents the maximum number of different valued
// substructures on the list; otherwise, max represents the maximum
// number of substructures on the list.  If sub is not inserted, then
// it is destroyed.  SubListInsert assumes given subList already
// conforms to maximums.
//---------------------------------------------------------------------------
//
void SubList::SubListInsert(Substructure *sub, ULONG max, BOOLEAN valueBased, LabelList *labelList)
{
    SubListNode *subIndex = NULL;
    SubListNode *subIndexPrevious = NULL;
    SubListNode *newSubListNode = NULL;
    ULONG numSubs = 0;
    ULONG numDiffVals = 0;
    BOOLEAN inserted = FALSE;

    newSubListNode = SubListNode::AllocateSubListNode(sub);

    // if subList empty, insert new sub and exit (no need to check maximums)
    if (head == NULL)
    {
        head = newSubListNode;
        return;
    }

    // if sub already on subList, destroy and exit
    subIndex = head;
    while ((subIndex != NULL) && (subIndex->getInstance()->getValue() >= sub->getValue()))
    {
        if (subIndex->getInstance()->getValue() == sub->getValue())
        {
            if (subIndex->getInstance()->getDefinition()->GraphMatch( sub->getDefinition(),
                           labelList, 0.0, NULL, NULL))
            {
                newSubListNode->FreeSubListNode();
                return;
            }
        }
        subIndex = (SubListNode *)subIndex->getNext();
    }

   
    // sub is unique, so insert in appropriate place and check maximums
    subIndex = head;
    while (subIndex != NULL)
    {
        //  cout <<"In the while"<<endl;
        if (! inserted)
        {
            if (subIndex->getInstance()->getValue() < sub->getValue())
            {
                newSubListNode->setNext(subIndex) ;
                if (subIndexPrevious != NULL)
                    subIndexPrevious->setNext(newSubListNode) ;
                else
                    head = newSubListNode;
                subIndex = newSubListNode;
                inserted = TRUE;
            }
            else if (subIndex->getNext() == NULL)
            {
                // Special case where the potential spot is the end of the
                // list, so go ahead and put it there, but may get removed
                // next time through the loop if boundaries are exceeded.
                subIndex->setNext(newSubListNode) ;
                inserted = TRUE;
            }
        }
        //  cout <<"after the if"<<endl;
        // update counters on number of substructures and different values
        numSubs++;
        if (subIndexPrevious == NULL)
            numDiffVals = 1;
        else if (subIndexPrevious->getInstance()->getValue() != subIndex->getInstance()->getValue())
            numDiffVals++;
        
        //  cout <<"Check if max exceeded"<<endl;
        // check if maximum exceeded
        if ((max > 0) &&
            (((valueBased) && (numDiffVals > max)) ||
             ((! valueBased) && (numSubs > max))) )
        {
            // max exceeded, so delete rest of subList from subIndex on
            if (subIndexPrevious != NULL)
                subIndexPrevious->setNext(NULL) ;
            while (subIndex != NULL)
            {
                //  cout <<"max exceeded inside while"<<endl;
                subIndexPrevious = subIndex;
                subIndex = (SubListNode *)subIndex->getNext();
                subIndexPrevious->FreeSubListNode();
                //  cout <<"out of the while"<<endl;
            }
        }
        else
        {
            subIndexPrevious = subIndex;
            subIndex = (SubListNode *)subIndex->getNext();
        }
    }

    if (! inserted)
        newSubListNode->FreeSubListNode();
}




//---------------------------------------------------------------------------
// NAME: PrintSubList
//
// INPUTS: (SubList *subList) - list of substructures to print
//         (Parameters *parameters)
//
// RETURN:  void
//
// PURPOSE: Print given list of substructures.
//---------------------------------------------------------------------------

void SubList::PrintSubList( Params *parameters)
{
    ULONG counter = 1;
    SubListNode *subListNode = NULL;
    
        subListNode = head;
        while (subListNode != NULL)
        {
            printf("(%lu) ", counter);
            counter++;
            subListNode->getInstance()->PrintSub( parameters);
            printf("\n");
            subListNode = (SubListNode *) subListNode->getNext();
        }

}



//---------------------------------------------------------------------------
// NAME: CountSubs
//
// INPUTS: (SubList *subList) - list of substructures
//
// RETURN: (ULONG) - number of substructures in subList.
//
// PURPOSE: Return number of substructures on subList.
//---------------------------------------------------------------------------

ULONG SubList::CountSubs()
{
    ULONG counter = 0;
    SubListNode *subListNode;

        subListNode = head;
        while (subListNode != NULL)
        {
            counter++;
            subListNode = (SubListNode *)subListNode->getNext();
        }
    
    return counter;
}


//---------------------------------------------------------------------------
// NAME: InsertSub
//
// INPUTS: (SubList *masterSubList)
//         (Substructure *sub)
//         (double subValue)
//         (ULONG numPosInstances)
//         (ULONG numNegInstances)
//
// RETURN: (void)
//
// PURPOSE: We traverse all of the increments and collect the set of
//          unique substructures to compute the overall value for the
//          graph.  This function inserts a new substructure on that
//          master list.  The list is maintained in descending order by value.
//---------------------------------------------------------------------------

void SubList::InsertSub( Substructure *sub, double subValue,
               ULONG numPosInstances, ULONG numNegInstances)
{
    SubListNode *subIndex;
    SubListNode *last;
    SubListNode *newSubListNode;
    Substructure *newSub;
    
    subIndex = this->head;
    newSub = sub->CopySub();
    newSub->setNumInstances(numPosInstances) ;
    newSub->setNumNegInstances(numNegInstances) ;
    newSub->setValue(subValue) ;
    newSubListNode = SubListNode::AllocateSubListNode(newSub);
    last = NULL;
    
    // insert in descending order
    if (subIndex == NULL)
        this->head = newSubListNode;
    else
    {
        while (subIndex != NULL)
        {
            // keep going until we find the place to insert
            if (subIndex->getInstance()->getValue() > newSub->getValue())
            {
                last = subIndex;
                subIndex = (SubListNode *)subIndex->getNext();
            }
            else // insert
            {
                newSubListNode->setNext(subIndex) ;
                if (last != NULL)
                    last->setNext(newSubListNode) ;
                else  //insert at beginning of list
                    this->head = newSubListNode;
                return;
            }
        }
        // the case where we insert at the end of the list
        last->setNext(newSubListNode) ;
    }
}


//---------------------------------------------------------------------------
// NAME: PrintStoredSubList
//
// INPUTS: (SubList *subList) - list of substructures to print
//         (Parameters *parameters) - system parameters
//
// RETURN:  void
//
// PURPOSE: Print given list of substructures.
//---------------------------------------------------------------------------

void SubList::PrintStoredSubList( Params *parameters)
{
    ULONG counter = 1;
    ULONG num = parameters->numBestSubs;
    SubListNode *subListNode = NULL;
    
 
        subListNode = head;
        while ((subListNode != NULL) && (counter <= num))
        {
            printf("(%lu) ", counter);
            counter++;
            subListNode->getInstance()->PrintStoredSub( parameters);
            printf("\n");
            subListNode = (SubListNode *)subListNode->getNext();
        }

}


//---------------------------------------------------------------------------
// NAME: EvaluateBoundaryInstances
//
// INPUT: (SubList *bestSubList) - globally best subs known to this point
//        (Parameters *parameters) - system parameters
//
// RETURN: (InstanceList *) - a list of all instances that span the increment
//                            boundary
//
// PURPOSE: This function is called after each local increment is processed.
//          It finds any instances of the top-n substructures that span from
//          the current increment back to previous increments.
//---------------------------------------------------------------------------

InstanceList *SubList::EvaluateBoundaryInstances(Params *parameters)
{
    InstanceListNode *instanceListNode;
    // final set of instances that match up with subs
    InstanceList *finalInstanceList;
    Instance *instance;
    SubListNode *subListNode;
    Substructure *bestSub;
    BOOLEAN empty;
    RefInstanceList *refInstanceList;
    ReferenceGraph *refGraph = NULL;
    RefInstanceListNode *refInstanceListNode = NULL;
    
    empty = FindInitialBoundaryInstances( parameters->posGraph,
                                         parameters->labelList,
                                         parameters->GetCurrentIncrement()->getStartPosVertexIndex(), parameters);
    if (empty)
        return NULL;
    
    // holds the final set of boundary instances
    finalInstanceList = InstanceList::AllocateInstanceList();
    
    // For each top-n sub, create a list of refGraphs that correspond to each
    // candidate instance
    subListNode = head;
    while (subListNode != NULL)
    {
        // List of RefInstanceListNodes for a particular top-n sub, one for each
        // initial seed instance
        refInstanceList = RefInstanceList::AllocateRefInstanceList();
        bestSub = subListNode->getInstance();
        if (parameters->outputLevel > 2)
        {
            printf("Processing Instances for Top-n Sub:\n");
            bestSub->PrintStoredSub( parameters);
        }
        if (bestSub->getInstances() != NULL)
        {
            instanceListNode = bestSub->getInstances()->getHead();
            while (instanceListNode != NULL)
            {
                // The RefInstanceListNode holds a reference graph and the list of
                // instances that use it.  We create one RefInstanceListNode for
                // each initial seed instance.
                refInstanceListNode = RefInstanceListNode::AllocateRefInstanceListNode();
                instance = instanceListNode->getInstance();
                 refInstanceListNode->getInstance()->InstanceListInsert(instance,
                                   FALSE);
                refGraph =
                instance->InstanceToRefGraph( parameters->posGraph, parameters);
                refInstanceListNode->setRefGraph(refGraph) ;
                refInstanceListNode->setFirstPass(TRUE);
                refInstanceListNode->setDoExtend(TRUE);
                
                // Insert at head of list
                refInstanceListNode->setNext(refInstanceList->getHead()) ;
                refInstanceList->setHead(refInstanceListNode) ;
                
                instanceListNode = (InstanceListNode *)instanceListNode->getNext();
            }
        }
        bestSub->getInstances()->FreeInstanceList();
        bestSub->setInstances(NULL) ;
        bestSub->setNumInstances(0);
        
        // For each top-n sub, store the set of refGraphs
        
        refInstanceList->ProcessInstancesForSub(bestSub, finalInstanceList, parameters);
        free(refInstanceList);
        subListNode = (SubListNode *)subListNode->getNext();
    }
    
    return finalInstanceList;
}


//------------------------------------------------------------------------------
// NAME: VertexInSubList
//
// INPUT: (SubList *subList) - list of top-n subs
//        (Vertex *vertex) - vertex from the full graph that we are comparing
//                           to the sub's vertices
//
// RETURN:  BOOLEAN - true if the vertex is consistent with one of the subs
//
// PURPOSE:  Check to see if this vertex is included in any of the subs.
//------------------------------------------------------------------------------

BOOLEAN SubList::VertexInSubList( LabeledVertex *vertex)
{
    SubListNode *subListNode;
    Substructure *sub;
    BOOLEAN found;
    
    if (head != NULL)
        subListNode = this->head;
    else
    {
        printf("Best SubList is null\n");
        exit(0);
    }
    
    // pass through each substructure
    while (subListNode != NULL)
    {
        sub = subListNode->getInstance();
        
        // Look at each vertex index in the instance and see if it matches our
        // vertex, if so then return TRUE
        found = sub->getDefinition()->VertexInSub( vertex);
        
        if (found)
            return TRUE;
        subListNode = (SubListNode *)subListNode->getNext();
    }
    
    return FALSE;
}

//---------------------------------------------------------------------------
// NAME: FindInitialBoundaryInstances
//
// INPUT:  (SubList *bestSubList) - top n subs
//         (Graph *graph) - the accumulated graph
//         (LabelList *labelList) - accumulated collection of labels
//         (ULONG startVertexIndex) - index of the first vertex
//                                    in the current increment
//         (Parameters *parameters) - system parameters
//
// RETURN:  BOOLEAN
//
// PURPOSE:  Called once by EvaluateBoundaryInstances.  Look at each vertex
//           that has a boundary edge and see if it could be a part of a top-n
//           substructure instance.  Add the instance to the instance list of
//           each top-n sub for which we find a match.  This has the effect of
//           creating a set of 2-vertex instances that we use as a starting
//           point for extension.
//---------------------------------------------------------------------------

BOOLEAN SubList::FindInitialBoundaryInstances( LabeledGraph *graph,
                                     LabelList *labelList,
                                     ULONG startVertexIndex,
                                     Params *parameters)
{
    LabeledVertex *vertices;
    LabeledVertex v;
    LabeledEdge *edges;
    LabeledEdge e;
    ULONG i, j;
    Instance *instance;
    Instance *extInstance;
    Instance *newInstance;
    Substructure *sub;
    BOOLEAN foundMatch;
    BOOLEAN empty = TRUE;
    SubListNode *subListNode;
    Substructure *bestSub;
    
    
    vertices = graph->getVertices();
    edges = graph->getEdges();
    
    // Look at each vertex in this increment
    for (i=startVertexIndex;i<graph->getNumVertices();i++)
    {
        v = vertices[i];
        // Look at each edge for this vertex
        for (j=0;j<v.getNumEdges();j++)
        {
            e = edges[v.getEdges()[j]];
            
            // Proceed only if the edge spans the increment and both vertices
            // could be part of a top-n sub
            if (e.isSpanIncrement() &&
                (VertexInSubList( &vertices[e.getE().first])) &&
                (VertexInSubList( &vertices[e.getE().second])))
            {
                // refCount set to 0
                instance = Instance::AllocateInstance (1, 0);
                instance->getVertices()[0] = i;
                instance->setMinMatchCost(0.0) ;
                extInstance = instance->CreateExtendedInstance( i, v.getEdges()[j], graph);
                instance->FreeInstance();
                
                // Run through each top-n sub and add the instance to the list
                // anywhere we find a sgiso match.
                // To use the existing code we have to turn it into a sub.
                sub = extInstance->CreateSubFromInstance( parameters->posGraph);
                subListNode = head;
                while(subListNode != NULL)
                {
                    bestSub = subListNode->getInstance();
                    foundMatch = sub->getDefinition()->CheckForSubgraph(
                                                  bestSub->getDefinition(), parameters);
                    
                    // If we found a sgiso match then make sure the vertices do not
                    // overlap with another instance already on the sub's list
                    // and make sure the vertices have not already been used for
                    // another instance in a previous increment.
                    if (foundMatch &&
                        !bestSub->getInstances()->CheckForSeedInstanceOverlap(extInstance) &&
                        !extInstance->CheckInstanceForOverlap( bestSub, parameters))
                    {
                        if (bestSub->getInstances() == NULL)
                            bestSub->setInstances(InstanceList::AllocateInstanceList()) ;
                        newInstance = extInstance->CopyInstance();
                        
                        // This insertion increments the instance's refCount
                        // If the instance is already on the list it will be freed
                        bestSub->getInstances()->InstanceListInsert(newInstance, TRUE);
                        empty = FALSE;
                    }
                    subListNode = (SubListNode *)subListNode->getNext();
                }
                sub->FreeSub();
                extInstance->FreeInstance();
            }
        }
    }
    return empty;
}




























