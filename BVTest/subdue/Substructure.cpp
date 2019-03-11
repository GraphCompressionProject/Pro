//
// Created by Macbook on 10/02/2019.
//
#include "stdafx.h"
#include "Substructure.hpp"
#include "Params.hpp"
#include "SubList.hpp"
#include "Increment.hpp"
#include "Compressor.hpp"
#include "IncrementList.hpp"
#include "IncrementListNode.hpp"
#include "AvlTableNode.hpp"
#include "AvlTreeList.hpp"
#include "InstanceVertexList.hpp"


//---------------------------------------------------------------------------
// NAME:  CheckVertexForOverlap
//
// INPUTS:  (ULONG vertex)
//          (Substructure *sub)
//          (Parameters *parameters)
//
// RETURN:  BOOLEAN
//
// PURPOSE: Check for overlapping vertex
//---------------------------------------------------------------------------

BOOLEAN Substructure::CheckVertexForOverlap(ULONG vertex,
                              Params *parameters)
{
    AvlTable *avlTable;
    ULONG *result;
    
    avlTable = GetSubTree( parameters);
    if (avlTable == NULL)
        return FALSE;
    
    result = (ULONG *)avlTable->avl_find( &vertex);
    if ((result != NULL) && (*result == vertex))
        return TRUE;
    else return FALSE;
}

//---------------------------------------------------------------------------
// NAME: ExtendSub
//
// INPUTS: (Substructure *sub) - substructure to be extended
//         (Parameters *parameters)
//
// RETURN: (SubList *) - list of extended substructures
//
// PURPOSE: Return list of substructures representing extensions to
// the given substructure.  Extensions are constructed by adding an
// edge (or edge and new vertex) to each positive instance of the
// given substructure in all possible ways according to the graph.
// Matching extended instances are collected into new extended
// substructures, and all such extended substructures are returned.
// If the negative graph is present, then instances of the
// substructure in the negative graph are also collected.
//---------------------------------------------------------------------------

SubList *Substructure::ExtendSub( Params *parameters)
{
    InstanceList *negInstanceList;
    InstanceList *newInstanceList;
    InstanceListNode *newInstanceListNode;
    Instance *newInstance;
    Substructure *newSub;
    SubList *extendedSubs;
    SubListNode *newSubListNode = NULL;
    ULONG newInstanceListIndex = 0;
    
    // parameters used
    LabeledGraph *posGraph = parameters->posGraph;
    LabeledGraph *negGraph = parameters->negGraph;
    LabelList *labelList = parameters->labelList;
    extendedSubs = SubList::AllocateSubList();
    newInstanceList = instances->ExtendInstances(posGraph);


    negInstanceList = NULL;
    if (negGraph != NULL)
        negInstanceList = negInstances->ExtendInstances( negGraph);
    newInstanceListNode = newInstanceList->getHead();
    while (newInstanceListNode != NULL)
    {
        newInstance = newInstanceListNode->getInstance();
        if (newInstance->getMinMatchCost() != 0.0)
        {
            // minMatchCost=0.0 means the instance is an exact match to a
            // previously-generated sub, so a sub created from this instance
            // would be a duplicate of one already on the extendedSubs list
            newSub = newInstance->CreateSubFromInstance( posGraph);

            if (! extendedSubs->MemberOfSubList(newSub, labelList))
            {
               newSub->AddPosInstancesToSub( newInstance, newInstanceList,
                                    parameters,newInstanceListIndex);

               if (negInstanceList != NULL)
                   newSub->AddNegInstancesToSub( newInstance, negInstanceList, parameters);
                // add newSub to head of extendedSubs list
                newSubListNode = SubListNode::AllocateSubListNode(newSub);
                newSubListNode->setNext(extendedSubs->getHead());
                extendedSubs->setHead(newSubListNode);
            } else delete newSub;
        }
       
        newInstanceListNode = (InstanceListNode *)newInstanceListNode->getNext();
        newInstanceListIndex++;
    }

    if(negInstanceList != NULL) negInstanceList->FreeInstanceList();
    if(newInstanceList != NULL) newInstanceList->FreeInstanceList();
    
    return extendedSubs;
}



//---------------------------------------------------------------------------
//
// NAME: AddPosInstancesToSub
//
// INPUTS: (Substructure *sub) - substructure to collect instances
//         (Instance *subInstance) - instance of substructure
//         (InstanceList *instanceList) - instances to collect from in
//                                        positive graph
//         (Parameters *parameters)
//         (ULONG index) - index of substructure into instance list
//
// RETURN: (void)
//
// PURPOSE: Add instance from instanceList to sub's positive
// instances if the instance matches sub's definition.  If
// allowInstanceOverlap=FALSE, then instances added only if they do
// not overlap with existing instances.
//---------------------------------------------------------------------------
void Substructure::AddPosInstancesToSub( Instance *subInstance, InstanceList *instanceList, Params *parameters, ULONG index)
{
    InstanceListNode *instanceListNode;
    Instance *instance;
    LabeledGraph *instanceGraph;
    double thresholdLimit;
    double matchCost;
    ULONG counter = 0;
    
    // parameters used
    LabeledGraph *posGraph              = parameters->posGraph;
    LabelList *labelList         = parameters->labelList;
    BOOLEAN allowInstanceOverlap = parameters->allowInstanceOverlap;
    double threshold             = parameters->threshold;
    
    // collect positive instances of substructure
    if (instanceList != NULL)
    {
        instances = InstanceList::AllocateInstanceList();
        //
        // Go ahead an insert the subInstance onto the list of instances for the
        // substructure, as it is obviously an instance.
        //
        subInstance->setUsed(TRUE) ;
        instances->InstanceListInsert(subInstance, FALSE);
        numInstances++;
        instanceListNode = instanceList->getHead();
        while (instanceListNode != NULL)
        {
            if (instanceListNode->getInstance() != NULL)
            {
                instance = instanceListNode->getInstance();
                if (allowInstanceOverlap ||
                    (! instances->InstanceListOverlap(instance)))
                {
                    thresholdLimit = threshold *
                    (instance->getNumVertices() + instance->getNumEdges());
                    instanceGraph = instance->InstanceToGraph( posGraph);
                    //
                    // First, if the threshold is 0.0, see if we can match on
                    // just the new edge that was added.
                    //
                    if (threshold == 0.0)
                    {
                        //
                        // To avoid processing duplicates, skip all entries
                        // before this instance (because they have been checked
                        // before in a previous call), and skip itself (because
                        // there is no point in comparing it to itself).
                        //
                        // Also, skip processing this instance if it is already
                        // matched with another substructure.
                        //
                        if ((counter > index) && (!instance->isUsed()))
                        {
                            if (definition->NewEdgeMatch ( subInstance, instanceGraph,
                                              instance, parameters, thresholdLimit,
                                              & matchCost))
                            {
                                if (matchCost < instance->getMinMatchCost())
                                    instance->setMinMatchCost(matchCost);
                                instance->setUsed(TRUE) ;
                                instances->InstanceListInsert(instance, FALSE);
                                numInstances++;
                            }
                        }
                    }
                    else
                    {
                        if (definition->GraphMatch( instanceGraph, labelList,
                                       thresholdLimit, & matchCost, NULL))
                        {
                            if (matchCost < instance->getMinMatchCost())
                                instance->setMinMatchCost(matchCost);
                            instances->InstanceListInsert(instance, FALSE);
                            numInstances++;
                        }
                    }
                    instanceGraph->FreeGraph();
                }
                counter++;
            }
            instanceListNode = (InstanceListNode *) instanceListNode->getNext();
        }
    }
}


//---------------------------------------------------------------------------
//
// NAME: AddNegInstancesToSub
//
// INPUTS: (Substructure *sub) - substructure to collect instances
//         (Instance *subInstance) - instance of substructure
//         (InstanceList *instanceList) - instances to collect from in
//                                        negative graph
//         (Parameters *parameters)
//
// RETURN: (void)
//
// PURPOSE: Add instance from instanceList to sub's negative
// instances if the instance matches sub's definition.  If
// allowInstanceOverlap=FALSE, then instances added only if they do
// not overlap with existing instances.
//---------------------------------------------------------------------------
void Substructure::AddNegInstancesToSub( Instance *subInstance,
                          InstanceList *instanceList, Params *parameters)
{
    InstanceListNode *instanceListNode;
    Instance *instance;
    LabeledGraph *instanceGraph;
    double thresholdLimit;
    double matchCost;
    
    // parameters used
    LabeledGraph *negGraph              = parameters->negGraph;
    LabelList *labelList         = parameters->labelList;
    BOOLEAN allowInstanceOverlap = parameters->allowInstanceOverlap;
    double threshold             = parameters->threshold;
    
    // collect negative instances of substructure
    if (instanceList != NULL)
    {
        negInstances = (InstanceList *) InstanceList::AllocateInstanceList();
        instanceListNode = instanceList->getHead();
        while (instanceListNode != NULL)
        {
            if (instanceListNode->getInstance() != NULL)
            {
                instance = instanceListNode->getInstance();
                if (allowInstanceOverlap ||
                    (! negInstances->InstanceListOverlap(instance)))
                {
                    thresholdLimit = threshold *
                    (instance->getNumVertices() + instance->getNumEdges());
                    instanceGraph = instance->InstanceToGraph( negGraph);
                    //
                    // First, if the threshold is 0.0, see if we can match on
                    // just the new edge that was added.
                    //
                    if (threshold == 0.0)
                    {
                        // If instance has already been added to another substructure's
                        // list of instances, we can skip it
                        if (!instance->isUsed())
                        {
                            if (definition->NewEdgeMatch ( subInstance, instanceGraph,
                                              instance, parameters, thresholdLimit,
                                              & matchCost))
                            {
                                if (matchCost < instance->getMinMatchCost())
                                    instance->setMinMatchCost(matchCost) ;
                                instance->setUsed(TRUE) ;
                                negInstances->InstanceListInsert(instance, FALSE);
                                numNegInstances++;
                            }
                        }
                    }
                    else
                    {
                        if (definition->GraphMatch( instanceGraph, labelList,
                                       thresholdLimit, & matchCost, NULL))
                        {
                            if (matchCost < instance->getMinMatchCost())
                                instance->setMinMatchCost(matchCost) ;
                            negInstances->InstanceListInsert(instance, FALSE);
                            numNegInstances++;
                        }
                    }
                    instanceGraph->FreeGraph();
                }
            }
            instanceListNode = (InstanceListNode *)instanceListNode->getNext();
        }
    }
}






////---------------------------------------------------------------------------
//// NAME: PrintSub
////
//// INPUTS: (Substructure *sub) - substructure to print
////         (Parameters *parameters) - parameters
////
//// RETURN: void
////
//// PURPOSE: Print given substructure's value, number of instances,
//// definition, and possibly the instances.
////---------------------------------------------------------------------------

void Substructure::PrintSub( Params *parameters)
{
    // parameters used
    LabelList *labelList = parameters->labelList;
    unsigned long outputLevel = parameters->outputLevel;


        printf("Substructure: value = %.*g", NUMERIC_OUTPUT_PRECISION, value);
        // print instance/example statistics if output level high enough
        if (outputLevel > 2)
        {
            printf("\n                  pos instances = %lu",numInstances);
            if (parameters->incremental)
                printf(", pos examples = %lu\n",(unsigned long) posIncrementValue);
            else
                printf(", pos examples = %lu\n",numExamples);

            printf("                  neg instances = %lu",numNegInstances);
            if (parameters->incremental)
                printf(", neg examples = %lu\n",(unsigned long) negIncrementValue);
            else
                printf(", neg examples = %lu\n",numNegExamples);
        }
        else
        {
            if ((parameters->incremental) && (parameters->evalMethod == EVAL_SETCOVER))
                printf(", pos examples = %lu, neg examples = %lu\n",(unsigned long) posIncrementValue, (unsigned long) negIncrementValue);
            else
                printf(", pos instances = %lu, neg instances = %lu\n",numInstances, numNegInstances);
        }
        // print subgraph
        if (definition != NULL)
        {
            definition->PrintGraph(labelList);
            // instances->PrintInstanceList(definition,labelList);
        }
        if (recursive)
        {
            printf("    re ");
            labelList->getHead()[recursiveEdgeLabel].PrintLabel();
            printf("\n");
        }
        // print instances if output level high enough
        if (outputLevel > 2)
        {
            printf("\n  Positive instances:\n");
            PrintPosInstanceList( parameters);
            if (numNegInstances > 0)
            {
                printf("\n  Negative instances:\n");
                PrintNegInstanceList(parameters);
            }
        }
}

//---------------------------------------------------------------------------
// NAME: PrintPosInstanceList
//
// INPUTS: (Substructure *sub) - substructure containing positive instances
//         (Parameters *parameters)
//
// RETURN: (void)
//
// PURPOSE: Print array of sub's positive instances.
//---------------------------------------------------------------------------

void Substructure::PrintPosInstanceList(Params *parameters)
{
   unsigned long i;
   unsigned long posEgNo;
   unsigned long vertexOffset = 0;
   InstanceListNode *instanceListNode;

   // parameters used
   LabeledGraph *posGraph = parameters->posGraph;
   unsigned long numPosEgs = parameters->numPosEgs;
   unsigned long *posEgsVertexIndices = parameters->posEgsVertexIndices;
   LabelList *labelList = parameters->labelList;

   if (instances != NULL) 
   {
      instanceListNode = instances->getHead();
      i = 1;
      while (instanceListNode != NULL) 
      {
         printf("\n  Instance %lu", i);
         if (numPosEgs > 1) 
         {
            posEgNo = instanceListNode->getInstance()->InstanceExampleNumber(posEgsVertexIndices, numPosEgs);
            vertexOffset = posEgsVertexIndices[posEgNo - 1];
            printf(" in positive example %lu:\n", posEgNo);
         } 
         else 
            printf(":\n");
         instanceListNode->getInstance()->PrintInstance( vertexOffset, posGraph, labelList);
         instanceListNode = (InstanceListNode *)instanceListNode->getNext();
         i++;
      }
   }
}


//---------------------------------------------------------------------------
// NAME: PrintNegInstanceList
//
// INPUTS: (Substructure *sub) - substructure containing negative instances
//         (Parameters *parameters)
//
// RETURN: (void)
//
// PURPOSE: Print array of sub's negative instances.
//---------------------------------------------------------------------------

void Substructure::PrintNegInstanceList( Params *parameters)
{
   unsigned long i;
   unsigned long negEgNo;
   unsigned long vertexOffset = 0;
   InstanceListNode *instanceListNode;

   // parameters used
   LabeledGraph *negGraph = parameters->negGraph;
   unsigned long numNegEgs = parameters->numNegEgs;
   unsigned long *negEgsVertexIndices = parameters->negEgsVertexIndices;
   LabelList *labelList = parameters->labelList;

   if (negInstances != NULL) 
   {
      instanceListNode = negInstances->getHead();
      i = 1;
      while (instanceListNode != NULL) 
      {
         printf("\n  Instance %lu", i);
         if (numNegEgs > 1) 
         {
            negEgNo = instanceListNode->getInstance()->InstanceExampleNumber(negEgsVertexIndices, numNegEgs);
            vertexOffset = negEgsVertexIndices[negEgNo - 1];
            printf(" in negative example %lu:\n", negEgNo);
         } 
         else 
            printf(":\n");
         instanceListNode->getInstance()->PrintInstance(vertexOffset, negGraph, labelList);
         instanceListNode = (InstanceListNode *)instanceListNode->getNext();
         i++;
      }
   }
}


//---------------------------------------------------------------------------
// NAME: PrintNewBestSub
//
// INPUTS: (Substructure *sub) - possibly new best substructure
//         (SubList *subList) - list of best substructures
//         (Parameters *parameters)
//
// RETURN: (void)
//
// PURPOSE: If sub is better than the best substructure on subList, then
// print it.  This should be called only if outputLevel > 3.
//---------------------------------------------------------------------------

void Substructure::PrintNewBestSub( SubList *subList, Params *parameters)
{
    unsigned long outputLevel = parameters->outputLevel;

    if ((subList->getHead() == NULL) || (value > subList->getHead()->getInstance()->getValue()))
    {
        parameters->outputLevel = 1; // turn off instance printing
        printf("\nNew best ");
        PrintSub(parameters);
        printf("\n");
        parameters->outputLevel = outputLevel;
    }
}


//---------------------------------------------------------------------------
// NAME: EvaluateSub
//
// INPUTS: (Substructure *sub) - substructure to evaluate
//         (Parameters *parameters)
//
// RETURN: (void)
//
// PURPOSE: Set the value of the substructure.  The value of a
// substructure S in a graph G is computed as
//
//   size(G) / (size(S) + size(G|S))
//
// The value of size() depends on whether we are using the EVAL_MDL or
// EVAL_SIZE evaluation method.  If EVAL_MDL, then size(g) computes
// the description length in bits of g.  If EVAL_SIZE, then size(g) is
// simply vertices(g)+edges(g).  The size(g|s) is the size of graph g
// after compressing it with substructure s.  Compression involves
// replacing each instance of s in g with a new single vertex (e.g.,
// Sub_1) and reconnecting edges external to the instance to point to
// the new vertex.  For EVAL_SIZE, the size(g|s) can be computed
// without actually performing the compression.
//
// If negative graphs Gn are present, then the evaluation becomes
//
//                    size(Gp) + size(Gn)
//        --------------------------------------------
//        size(S) + size(Gp|S) + size(Gn) - size(Gn|S)
//
// If the evaluation method is EVAL_SETCOVER, then the evaluation of
// substructure S becomes
//
//   (num pos egs containing S) + (num neg egs not containing S)
//   -----------------------------------------------------------
//               (num pos egs) + (num neg egs)
//
//---------------------------------------------------------------------------

void Substructure::EvaluateSub(Params *parameters)
{
    double sizeOfSub;
    double sizeOfPosGraph;
    double sizeOfNegGraph;
    double sizeOfCompressedPosGraph;
    double sizeOfCompressedNegGraph;
    double subValue = 0.0;
    double sizeOfPosIncrement;
    double sizeOfNegIncrement;
    LabeledGraph *compressedGraph;
    ULONG numLabels;
    ULONG posEgsCovered;
    ULONG negEgsCovered;
    LabeledGraph *oldDefinition = NULL;
    Compressor c;
    // parameters used
    LabeledGraph *posGraph              = parameters->posGraph;
    LabeledGraph *negGraph              = parameters->negGraph;
    double posGraphDL            = parameters->posGraphDL;
    double negGraphDL            = parameters->negGraphDL;
    ULONG numPosEgs              = parameters->numPosEgs;
    ULONG numNegEgs              = parameters->numNegEgs;
    LabelList *labelList         = parameters->labelList;
    BOOLEAN allowInstanceOverlap = parameters->allowInstanceOverlap;
    ULONG evalMethod             = parameters->evalMethod;
    
    // calculate number of examples covered by this substructure
    numExamples = PosExamplesCovered(parameters);

    numNegExamples = 0;
    if (negGraph != NULL)
    {
        numNegExamples = NegExamplesCovered( parameters);
    }
  
    
    // if recursive sub, then temporarily add recursive edge to sub's definition
    if (recursive)
    {
        oldDefinition = definition;
        definition = oldDefinition->CopyGraph();
        // recursive edge added as a directed self edge (***** hack?)
        definition->AddEdge( 0, 0, TRUE, recursiveEdgeLabel, FALSE);
    }
    switch(evalMethod)
    {
        case EVAL_MDL:
            numLabels = labelList->getNumLabels();
            sizeOfSub = definition->MDL( numLabels, parameters);
            sizeOfPosGraph = posGraphDL; // cached at beginning
            compressedGraph = c.CompressGraph(posGraph, instances, parameters);
            

            numLabels++; // add one for new "SUB" vertex label
            if ((allowInstanceOverlap) &&
                ((instances->InstancesOverlap()) ||
                 (negInstances->InstancesOverlap())))
                numLabels++; // add one for new "OVERLAP" edge label
            
            sizeOfCompressedPosGraph = compressedGraph->MDL(numLabels, parameters);
            // add extra bits to describe where external edges connect to instances
            sizeOfCompressedPosGraph +=
            compressedGraph->ExternalEdgeBits(definition,numInstances);
            subValue = sizeOfPosGraph / (sizeOfSub + sizeOfCompressedPosGraph);


            compressedGraph->FreeGraph();
            if (negGraph != NULL)
            {
                sizeOfNegGraph = negGraphDL; // cached at beginning
                compressedGraph = c.CompressGraph(negGraph, negInstances,
                                                parameters);
                sizeOfCompressedNegGraph = compressedGraph->MDL(numLabels, parameters);
                // add extra bits to describe where external edges connect to
                // instances
                sizeOfCompressedNegGraph +=
                compressedGraph->ExternalEdgeBits( definition,
                                 numNegInstances);
                subValue = (sizeOfPosGraph + sizeOfNegGraph) /
                (sizeOfSub + sizeOfCompressedPosGraph + sizeOfNegGraph -
                 sizeOfCompressedNegGraph);
                compressedGraph->FreeGraph();
            }
            break;

        case EVAL_SIZE:
            sizeOfSub = (double) definition->GraphSize();
            if (parameters->incremental)
            {
                sizeOfPosIncrement = parameters->GetIncrement(parameters->GetCurrentIncrementNum())->IncrementSize(POS);
                posIncrementValue = (double) posGraph->SizeOfCompressedGraph( instances, parameters, POS);
                
                subValue = sizeOfPosIncrement / (sizeOfSub + posIncrementValue);
                if (negGraph != NULL)
                {
                    sizeOfNegIncrement = parameters->GetIncrement(parameters->GetCurrentIncrementNum())->IncrementSize(NEG);
                    negIncrementValue = (double) negGraph->SizeOfCompressedGraph(negInstances, parameters, NEG);
                    
                    subValue = (sizeOfPosIncrement + sizeOfNegIncrement) /  (sizeOfSub + posIncrementValue + sizeOfNegIncrement - negIncrementValue);
                }
            }
            else
            {
                sizeOfPosGraph = (double) posGraph->GraphSize();
                sizeOfCompressedPosGraph = (double) posGraph->SizeOfCompressedGraph( instances,parameters, POS); // iSubdue
                
                subValue = sizeOfPosGraph / (sizeOfSub + sizeOfCompressedPosGraph);
                if (negGraph != NULL)
                {
                    sizeOfNegGraph = (double) negGraph->GraphSize ();
                    sizeOfCompressedNegGraph =(double) negGraph->SizeOfCompressedGraph(negInstances,parameters, NEG);
                    subValue = (sizeOfPosGraph + sizeOfNegGraph) / (sizeOfSub + sizeOfCompressedPosGraph + sizeOfNegGraph - sizeOfCompressedNegGraph);
                }
            }

            break;

        case EVAL_SETCOVER:
            if (parameters->incremental)
            {
                numPosEgs = parameters->GetIncrement(parameters->GetCurrentIncrementNum())->IncrementNumExamples( POS);
                numNegEgs = parameters->GetIncrement(parameters->GetCurrentIncrementNum())->IncrementNumExamples( NEG);
            }
            posEgsCovered = PosExamplesCovered( parameters);
            if (parameters->incremental)    posIncrementValue = posEgsCovered;
            negEgsCovered = 0;
            if (negGraph != NULL)
            {
                negEgsCovered = NegExamplesCovered(parameters);
                if (parameters->incremental)
                    negIncrementValue = negEgsCovered;
            }
            subValue = ((double) (posEgsCovered + (numNegEgs - negEgsCovered))) / ((double) (numPosEgs + numNegEgs));
            break;
    }

    this->value = subValue;
    // if recursive sub, then restore old definition
    if (recursive)
    {
        definition->FreeGraph();
        definition = oldDefinition;
    }
}



//---------------------------------------------------------------------------
// NAME: PosExamplesCovered
//
// INPUTS: (Substructure *sub) - substructure to look for in pos examples
//         (Parameters *parameters)
//
// RETURN: (ULONG) - number of positive examples covered by sub
//
// PURPOSE: Returns the number of positive examples that contain the
// substructure sub as a subgraph.
//---------------------------------------------------------------------------

ULONG Substructure::PosExamplesCovered( Params *parameters)
{
    ULONG start = 0;
    Increment *increment = NULL;
    
    if (parameters->incremental)
    {
        increment = parameters->GetCurrentIncrement();
        start = increment->getStartPosEdgeIndex();
    }
    
    return instances->ExamplesCovered(
                           parameters->posGraph,
                           parameters->numPosEgs,
                           parameters->posEgsVertexIndices, start);
}


//---------------------------------------------------------------------------
// NAME: NegExamplesCovered
//
// INPUTS: (Substructure *sub) - substructure to look for in neg examples
//         (Parameters *parameters)
//
// RETURN: (ULONG) - number of negative examples covered by sub
//
// PURPOSE: Returns the number of negative examples that contain the
// substructure sub as a subgraph.
//---------------------------------------------------------------------------

ULONG Substructure::NegExamplesCovered(Params *parameters)
{
    ULONG start = 0;
    Increment *increment = NULL;
    
    if (parameters->incremental)
    {
        increment = parameters->GetCurrentIncrement();
        start = increment->getStartNegEdgeIndex() ;
    }
    
    return negInstances->ExamplesCovered(
                           parameters->negGraph,
                           parameters->numNegEgs,
                           parameters->negEgsVertexIndices, start);
}


//---------------------------------------------------------------------------
// NAME: RecursifySub
//
// INPUTS: (Substructure *sub) - substructure to recursify
//         (Parameters *parameters)
//
// RETURN: (Substructure *) - recursified substructure or NULL
//
// PURPOSE: Attempts to turn given substructure into a recursive
// substructure.  This is possible if two or more instances are found to be
// connected by a same-labeled edge.  Only the best such substructure is
// returned, or NULL if no such edge exists.
//
// ***** TODO: Ensure that connecting edge starts at the same vertex in
// each pair of instances.
//---------------------------------------------------------------------------

Substructure *Substructure::RecursifySub( Params *parameters)
{
    ULONG i;
    Substructure *recursiveSub = NULL;
    Substructure *bestRecursiveSub = NULL;
    InstanceListNode *instanceListNode1;
    InstanceListNode *instanceListNode2;
    Instance *instance1;
    Instance *instance2;
    LabeledVertex *vertex1;
    ULONG v1;
    ULONG v2Index;
    ULONG e;
    LabeledEdge *edge;
    BOOLEAN foundPair;
    
    // parameters used
    LabeledGraph *graph = parameters->posGraph;
    LabelList *labelList = parameters->labelList;
    
    // reset labels' used flag
    for (i = 0; i < labelList->getNumLabels(); i++)
        labelList->getHead()[i].setused(FALSE) ;
    
    // mark all instance edges
    instanceListNode1 = instances->getHead();
    while (instanceListNode1 != NULL)
    {
        instance1 = instanceListNode1->getInstance();
        graph->MarkInstanceEdges(instance1, TRUE);
        instanceListNode1 =(InstanceListNode *) instanceListNode1->getNext();
    }
    
    // search instance list for a connected pair
    instanceListNode1 = instances->getHead();
    while (instanceListNode1 != NULL)
    {
        instance1 = instanceListNode1->getInstance();
        for (v1 = 0; v1 < instance1->getNumVertices(); v1++)
        {
            vertex1 = & graph->getVertices()[instance1->getVertices()[v1]];
            for (e = 0; e < vertex1->getNumEdges(); e++)
            {
                edge = & graph->getEdges()[vertex1->getEdges()[e]];
                if ((! edge->isUsed()) && (! labelList->getHead()[edge->getLabel()].is_used()))
                {
                    // search instance list for another instance involving edge
                    v2Index = edge->getE().second;
                    if (edge->getE().second == instance1->getVertices()[v1])
                        v2Index = edge->getE().first;
                    foundPair = FALSE;
                    instanceListNode2 = (InstanceListNode *)instanceListNode1->getNext();
                    while ((instanceListNode2 != NULL) && (! foundPair))
                    {
                        instance2 = instanceListNode2->getInstance();
                        if ((instance2 != instance1) &&
                            (instance2->InstanceContainsVertex( v2Index)))
                            foundPair = TRUE;
                        instanceListNode2 =(InstanceListNode *) instanceListNode2->getNext();
                    }
                    if (foundPair)
                    {
                        // connected pair of instances found; make recursive sub
                        labelList->getHead()[edge->getLabel()].setused(TRUE) ;
                        recursiveSub = MakeRecursiveSub( edge->getLabel(), parameters);
                        if (bestRecursiveSub == NULL)
                            bestRecursiveSub = recursiveSub;
                        else if (recursiveSub->getValue() > bestRecursiveSub->getValue())
                        {
                            bestRecursiveSub->FreeSub();
                            bestRecursiveSub = recursiveSub;
                        }
                        else recursiveSub->FreeSub();
                    }
                }
            }
        }
        instanceListNode1 = (InstanceListNode *) instanceListNode1->getNext();
    }
    
    // unmark all instance edges
    instanceListNode1 = instances->getHead();
    while (instanceListNode1 != NULL)
    {
        instance1 = instanceListNode1->getInstance();
        graph->MarkInstanceEdges(instance1, FALSE);
        instanceListNode1 = (InstanceListNode *)instanceListNode1->getNext();
    }
    
    return bestRecursiveSub;
}

//---------------------------------------------------------------------------
// NAME: MakeRecursiveSub
//
// INPUTS: (Substructure *sub) - substructure to recursify
//         (ULONG edgeLabel) - label on edge connecting instance pairs
//         (Parameters *parameters)
//
// RETURN: (Substructure *) - recursified substructure
//
// PURPOSE: Creates and returns a recursive substructure built by
// connecting chains of instances of the given substructure by the given
// edge label.
//
// ***** TODO: Ensure that connecting edge starts at the same vertex in
// each pair of instances.
//---------------------------------------------------------------------------

Substructure *Substructure::MakeRecursiveSub( ULONG edgeLabel, Params *parameters)
{
    Substructure *recursiveSub;
    
    // parameters used
    LabeledGraph *posGraph = parameters->posGraph;
    LabeledGraph *negGraph = parameters->negGraph;
    
    recursiveSub = AllocateSub();
    recursiveSub->setDefinition(definition->CopyGraph()) ;
    recursiveSub->setRecursive(TRUE) ;
    recursiveSub->setRecursiveEdgeLabel(edgeLabel) ;
    recursiveSub->setInstances(instances->GetRecursiveInstances(posGraph,numInstances,edgeLabel)) ;
    
    recursiveSub->setNumInstances(recursiveSub->instances->CountInstances()) ;
    if (negGraph != NULL)
    {
        recursiveSub->negInstances = negInstances->GetRecursiveInstances(negGraph, numNegInstances, edgeLabel);
        recursiveSub->numNegInstances =
        recursiveSub->getNegInstances()->CountInstances();
    }
    recursiveSub->EvaluateSub( parameters);
    return recursiveSub;
}

//---------------------------------------------------------------------------
// NAME:  AdjustMetrics
//
// INPUTS:  (Substructure *sub) - Substructure whose value is being adjusted
//          (Parameters *parameters)
//
// RETURN:  (void)
//
// PURPOSE:  Adjust the value calculations for a known sub based on the
//           new instance on an increment boundary that has been added.
//           Because examples are assumed to not cross boundary borders for
//           concept learning, this calculation is not made for setcover
//           evaluation.
//---------------------------------------------------------------------------

void Substructure::AdjustMetrics( Params *parameters)
{
    double sizeOfPosIncrement = 0.0;
    double sizeOfNegIncrement = 0.0;
    double sizeOfSub = 0.0;
    Increment *increment;
    
    increment = parameters->GetCurrentIncrement();
    
    // The increment pos/neg value is stored when the increment is first read in
    // and so measures the size of the original graph
    sizeOfSub = (double) definition->GraphSize();
    sizeOfPosIncrement = (double) parameters->IncrementSize(
                                                parameters->GetCurrentIncrementNum(), POS);
    
    // Modified to only compress and measure the current increment
    this->posIncrementValue = (double) parameters->posGraph->SizeOfCompressedGraph(
                                                            this->instances, parameters, POS);
    this->value = sizeOfPosIncrement / (sizeOfSub + this->posIncrementValue);
    if (parameters->negGraph != NULL)
    {
        sizeOfNegIncrement = (double) parameters->IncrementSize(
                                                    parameters->GetCurrentIncrementNum(), NEG);
        
        // Modified to only compress and measure the current increment
        this->negIncrementValue =
        (double) parameters->negGraph->SizeOfCompressedGraph(
                                       this->instances, parameters, NEG);
        this->value = (sizeOfPosIncrement + sizeOfNegIncrement) /
        (sizeOfSub + this->negIncrementValue +
         sizeOfNegIncrement - this->negIncrementValue);
    }
}


//---------------------------------------------------------------------------
// NAME:  CopySub
//
// INPUTS:  (Substructure *sub) - input substructure
//
// RETURN:  (Substructure *sub) - copy of input substructure
//
// PURPOSE:  Make new copy of existing substructure.
//---------------------------------------------------------------------------

Substructure *Substructure::CopySub()
{
    Substructure *newSub;
    
    newSub = (Substructure *) malloc(sizeof(Substructure));
    
    newSub->definition = definition->CopyGraph();
    newSub->setPosIncrementValue(posIncrementValue) ;
    newSub->setNegIncrementValue(negIncrementValue) ;
    newSub->setValue(value) ;
    newSub->setNumInstances(numInstances) ;
    newSub->setInstances(NULL);
    newSub->setNumNegInstances(numNegInstances);
    newSub->setNegInstances(NULL) ;
    newSub->setRecursive(recursive) ;
    newSub->setRecursiveEdgeLabel(recursiveEdgeLabel) ;
    
    return(newSub);
}


//---------------------------------------------------------------------------
// NAME: ComputeValue
//
// INPUTS: (IncrementListNode *incNodePtr)
//         (Substructure *sub)
//         (LabelList *labelList)
//         (ULONG *numPosInstances)
//         (ULONG *numNegInstances)
//         (ULONG *graphPosSize)
//         (ULONG *graphNegSize)
//         (Parameters *parameters)
//
// RETURN: (double)
//
// PURPOSE: Given a sub and a set of increments, compute the value of the sub.
//        We only need to look from the current increment forward, since the
//        sub would have to have been newly introduced in this increment.
//---------------------------------------------------------------------------

double Substructure::ComputeValue(IncrementListNode *incNodePtr,
                    LabelList *labelList, ULONG *numPosInstances,
                    ULONG *numNegInstances, ULONG *graphPosSize,
                    ULONG *graphNegSize, Params *parameters)
{
    Increment *increment;
    IncrementListNode *incrementNode;
    SubListNode *subListNode;
    Substructure *incrementSub;
    BOOLEAN found = FALSE;
    ULONG totalPosInstances = 0;
    ULONG totalNegInstances = 0;
    ULONG totalPosEgs = 0;
    ULONG totalNegEgs = 0;
    ULONG startIncrement;
    double totalPosGraphSize = 0; // total size of uncompressed pos graph
    double totalNegGraphSize = 0; // total size of uncompressed neg graph
    double compressedPosGraphSize = 0;
    double compressedNegGraphSize = 0;
    double subValue;
    
    if (incNodePtr == NULL)
        printf("ComputeValue Error: incNodePtr is null\n");
    
    if (parameters->evalMethod != EVAL_SETCOVER)
    {
        // If the increment we are examining is not the first one, then we need
        // to collect the graph size from all previous increments in
        // which the sub did not occur.
        startIncrement = incNodePtr->getInstance()->getIncrementNum();
        
        if (startIncrement !=
            parameters->incrementList->getHead()->getInstance()->getIncrementNum())
        {
            incrementNode = parameters->incrementList->getHead();
            
            // traverse each increment
            while (incrementNode->getInstance()->getIncrementNum() < startIncrement)
            {
                increment = incrementNode->getInstance();
                totalPosGraphSize += increment->getNumPosVertices() +
                increment->getNumPosEdges();
                totalNegGraphSize += increment->getNumNegVertices() +
                increment->getNumNegEdges();
                incrementNode = (IncrementListNode *)incrementNode->getNext();
            }
            compressedPosGraphSize = totalPosGraphSize; // no compression yet
            compressedNegGraphSize = totalNegGraphSize;
        }
    }
    
    // Start with the increment in which the sub was found and then
    // traverse all subsequent increments looking for sub
    while (incNodePtr != NULL)
    {
        increment = (Increment *)incNodePtr->getInstance();
        
        if (parameters->evalMethod != EVAL_SETCOVER)
        {
            // Compute the total graph size from the size of each increment
            totalPosGraphSize += increment->getNumPosVertices() +
            increment->getNumPosEdges();
            totalNegGraphSize += increment->getNumNegVertices() +
            increment->getNumNegEdges();
        }
        subListNode = increment->getSubList()->getHead();
        found = FALSE;
        
        // Traverse all subs in this increment and find the sub in question
        // Note that only the local best substructures are searched for a match,
        // so the number of instances may not be perfectly accurately udpated
        while (subListNode != NULL)
        {
            incrementSub = subListNode->getInstance();
            if (incrementSub->getDefinition()->GraphMatch( definition, labelList,
                           0.0, NULL, NULL))
            {
                // Found it, now update the values
                if (parameters->evalMethod == EVAL_SETCOVER)
                {
                    totalPosEgs += (ULONG) incrementSub->getPosIncrementValue();
                    totalNegEgs += (ULONG) incrementSub->getNegIncrementValue();
                }
                else
                {
                    compressedPosGraphSize += incrementSub->getPosIncrementValue();
                    compressedNegGraphSize += incrementSub->getNegIncrementValue();
                }
                totalPosInstances += incrementSub->getNumInstances();
                totalNegInstances += incrementSub->getNumNegInstances();
                found = TRUE;
                break;  //if we found it we can stop
            }
            subListNode = (SubListNode *)subListNode->getNext();
        }
        
        // If this increment did not contain the substructure then we have to
        // add the full size of the increment into the compressed graph size
        if (!found && parameters->evalMethod != EVAL_SETCOVER)
        {
            compressedPosGraphSize += increment->getNumPosVertices() +
            increment->getNumPosEdges();
            compressedNegGraphSize += increment->getNumNegVertices() +
            increment->getNumNegEdges();
        }
        incNodePtr = (IncrementListNode *)incNodePtr->getNext();
    }
    
    *numPosInstances = totalPosInstances;
    *numNegInstances = totalNegInstances;
    *graphPosSize = totalPosGraphSize;
    *graphNegSize = totalNegGraphSize;
    
    if (parameters->evalMethod == EVAL_SETCOVER)
    {
        subValue = ((double) (totalPosEgs +
                              (parameters->numPosEgs - totalNegEgs))) /
        ((double) (parameters->numPosEgs + parameters->numNegEgs));
    }
    else
    {
        if (totalNegGraphSize == 0)
            subValue = totalPosGraphSize /
            ((double) definition->GraphSize() +
             compressedPosGraphSize);
        else
            subValue = (totalPosGraphSize + totalNegGraphSize) /
            ((double) definition->GraphSize() +
             compressedPosGraphSize + totalNegGraphSize -
             compressedNegGraphSize);
    }
    return(subValue);
}


//---------------------------------------------------------------------------
// NAME:  GetSubTree
//
// INPUTS:  (Substructure *sub) - current substructure
//          (Parameters *parameters) - system parameters
//
// RETURN:  (struct avl_table *) - pointer to avl tree
//
// PURPOSE:  Return the avl tree corersponding to the input substructure.
//---------------------------------------------------------------------------

AvlTable *Substructure::GetSubTree( Params *parameters)
{
    AvlTreeList *avlTreeList;
    AvlTableNode *avlTableNode;
    BOOLEAN found = FALSE;
    
    avlTreeList = parameters->vertexList->getHead();
    avlTableNode = avlTreeList->getHead();
    
    while ((avlTableNode != NULL) && !found)
    {
        if (definition->GraphMatch(avlTableNode->getSub()->getDefinition(),
                       parameters->labelList, 0.0, NULL, NULL))
            found = TRUE;
        else
            avlTableNode =(AvlTableNode *) avlTableNode->getNext();
    }
    
    if (found)
        return avlTableNode->getInstance();
    else
        return NULL;
}

//---------------------------------------------------------------------------
// NAME: PrintStoredSub
//
// INPUTS: (Substructure *sub) - substructure to print
//         (Parameters *parameters) - parameters
//
// RETURN: void
//
// PURPOSE: Print given substructure's value, number of instances,
//          definition, and possibly the instances.
//---------------------------------------------------------------------------

void Substructure::PrintStoredSub ( Params *parameters)
{
  
        printf("Substructure: value = %.*g, ", NUMERIC_OUTPUT_PRECISION,
               value);
        printf("pos instances = %lu, neg instances = %lu\n",
               numInstances, numNegInstances);
        if (definition != NULL)
            definition->PrintGraph( parameters->labelList);
    
}

//---------------------------------------------------------------------------
// NAME: CompressFinalGraphs
//
// INPUTS: (Substructure *sub) - substructure for compression
//         (Parameters *parameters) - contains graphs and label list
//         (ULONG iteration) - SUBDUE iteration (or predefined sub)
//           on which compressing
//         (BOOLEAN predefinedSub) - TRUE if compressing substructure is
//           from the predefined substructure file
//
// RETURN: void
//
// PURPOSE: Compresses the positive and negative (if defined) graphs
// with the given substructure.  Parameters->labelList is modified to
// remove any labels no longer referenced and to add "SUB_" and
// "OVERLAP_" labels, as needed.  Note that adding the "SUB_" and
// "OVERLAP_" labels must take place after the call to CompressGraph,
// because CompressGraph works under the assumption that they have not
// yet been added.  If predefinedSub=TRUE, then an addition prefix is
// added to the "SUB" and "OVERLAP" labels.
//---------------------------------------------------------------------------

void Substructure::CompressFinalGraphs( Params *parameters,
                         ULONG iteration, BOOLEAN predefinedSub)
{
    LabeledGraph *compressedPosGraph;
    LabeledGraph *compressedNegGraph;
    char subLabelString[TOKEN_LEN];
    char overlapLabelString[TOKEN_LEN];
    Label label;
    LabelList *newLabelList;
    Compressor c;
    // parameters used
    LabeledGraph *posGraph              = parameters->posGraph;
    LabeledGraph *negGraph              = parameters->negGraph;
    BOOLEAN allowInstanceOverlap = parameters->allowInstanceOverlap;
    LabelList *labelList         = parameters->labelList;
    
    compressedPosGraph = posGraph;
    compressedNegGraph = negGraph;
    
    if ( numInstances > 0)
        compressedPosGraph = c.CompressGraph(posGraph,  instances, parameters);
    if ( numNegInstances > 0)
        compressedNegGraph = c.CompressGraph(negGraph,  negInstances,
                                           parameters);
    
    // add "SUB" and "OVERLAP" (if used) labels to label list
    if (parameters->incremental)
    {
        if (predefinedSub)
            sprintf(subLabelString, "%s_%s_%lu_%lu", PREDEFINED_PREFIX,
                    SUB_LABEL_STRING, iteration,
                    parameters->GetCurrentIncrementNum());
        else
            sprintf(subLabelString, "%s_%lu_%lu", SUB_LABEL_STRING, iteration,
                    parameters->GetCurrentIncrementNum());
    }
    else
    {
        if (predefinedSub)
            sprintf(subLabelString, "%s_%s_%lu", PREDEFINED_PREFIX,
                    SUB_LABEL_STRING, iteration);
        else
            sprintf(subLabelString, "%s_%lu", SUB_LABEL_STRING, iteration);
    }
    
    label.setLabelType(STRING_LABEL) ;
    label.getLabelValue().setStringLabel(subLabelString) ;
    labelList->StoreLabel(& label );
    if (allowInstanceOverlap &&
        ((instances->InstancesOverlap()) ||
         (negInstances->InstancesOverlap())))
    {
        if (predefinedSub)
            sprintf(overlapLabelString, "%s_%s_%lu", PREDEFINED_PREFIX,
                    OVERLAP_LABEL_STRING, iteration);
        else
            sprintf(overlapLabelString, "%s_%lu", OVERLAP_LABEL_STRING,
                    iteration);
        label.getLabelValue().setStringLabel(overlapLabelString) ;
        labelList->StoreLabel(&label);
    }
    
    // reset graphs with compressed graphs
    if ( numInstances > 0)
    {
        parameters->posGraph->FreeGraph();
        parameters->posGraph = compressedPosGraph;
    }
    if ( numNegInstances > 0)
    {
        parameters->negGraph->FreeGraph();
        parameters->negGraph = compressedNegGraph;
    }
    
    // Recompute label list and MDL for positive and negative graphs.
    // This should not be done for compression using a predefined sub,
    // because compressing the label list may remove a label that a
    // later predefined sub refers to.  Since the label list may then
    // contain labels not in the positive or negative graphs, then the
    // MDL calculation will be wrong.  Recomputing the label list and
    // the graphs' MDL should be done in CompressWithPredefinedSubs
    // after all predefined subs have been processed.
    
    if (!predefinedSub)
    {
        // compress label list and recompute graphs' labels
        newLabelList = LabelList::AllocateLabelList();
        newLabelList->CompressLabelListWithGraph( compressedPosGraph, parameters);
        if (compressedNegGraph != NULL)
            newLabelList->CompressLabelListWithGraph( compressedNegGraph,
                                       parameters);
        parameters->labelList->FreeLabelList();
        parameters->labelList = newLabelList;
        
        if (parameters->evalMethod == EVAL_MDL)
        {
            // recompute graph MDL (always necessary since label list has changed)
            parameters->posGraphDL = parameters->posGraph->MDL(
                                         newLabelList->getNumLabels(), parameters);
            if (parameters->negGraph != NULL)
                parameters->negGraphDL = parameters->negGraph->MDL(
                                             newLabelList->getNumLabels(), parameters);
        }
    }
}

/*****************************************************************************
 *                          Getters and Setters
 *****************************************************************************/

LabeledGraph *Substructure::getDefinition() const {
    return definition;
}


void Substructure::setDefinition(LabeledGraph *definition) {
    Substructure::definition = definition;
}


unsigned long Substructure::getNumInstances() const {
    return numInstances;
}


void Substructure::setNumInstances(unsigned long numInstances) {
    Substructure::numInstances = numInstances;
}


unsigned long Substructure::getNumExamples() const {
    return numExamples;
}


void Substructure::setNumExamples(unsigned long numExamples) {
    Substructure::numExamples = numExamples;
}


InstanceList *Substructure::getInstances() const {
    return instances;
}


void Substructure::setInstances(InstanceList *instances) {
    Substructure::instances = instances;
}


unsigned long Substructure::getNumNegInstances() const {
    return numNegInstances;
}


void Substructure::setNumNegInstances(unsigned long numNegInstances) {
    Substructure::numNegInstances = numNegInstances;
}


unsigned long Substructure::getNumNegExamples() const {
    return numNegExamples;
}


void Substructure::setNumNegExamples(unsigned long numNegExamples) {
    Substructure::numNegExamples = numNegExamples;
}


InstanceList *Substructure::getNegInstances() const {
    return negInstances;
}


void Substructure::setNegInstances(InstanceList *negInstances) {
    Substructure::negInstances = negInstances;
}


double Substructure::getValue() const {
    return value;
}


void Substructure::setValue(double value) {
    Substructure::value = value;
}


bool Substructure::isRecursive() const {
    return recursive;
}


void Substructure::setRecursive(bool recursive) {
    Substructure::recursive = recursive;
}


unsigned long Substructure::getRecursiveEdgeLabel() const {
    return recursiveEdgeLabel;
}


void Substructure::setRecursiveEdgeLabel(unsigned long recursiveEdgeLabel) {
    Substructure::recursiveEdgeLabel = recursiveEdgeLabel;
}


double Substructure::getPosIncrementValue() const {
    return posIncrementValue;
}


void Substructure::setPosIncrementValue(double posIncrementValue) {
    Substructure::posIncrementValue = posIncrementValue;
}


double Substructure::getNegIncrementValue() const {
    return negIncrementValue;
}


void Substructure::setNegIncrementValue(double negIncrementValue) {
    Substructure::negIncrementValue = negIncrementValue;
}


