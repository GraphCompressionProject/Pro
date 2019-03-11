//
// Created by Macbook on 12/02/2019.
//

#include "stdafx.h"
#include "Compressor.hpp"
#include "InstanceList.hpp"
#include "Params.hpp"
#include "LabeledGraph.hpp"
#include "Increment.hpp"
#include "LabelList.hpp"





//---------------------------------------------------------------------------
// NAME: CompressGraph
//
// INPUTS: (Graph *graph) - graph to be compressed
//         (InstanceList *instanceList) - substructure instances used to
//                                        compress graph
//         (Parameters *parameters)
//
// RETURN: (Graph *) - compressed graph
//
// PURPOSE: Returns a new graph, which is the given graph compressed
// with the given substructure instances.  "SUB" vertices replace each
// instance of the substructure, and "OVERLAP" edges are added between
// "SUB" vertices of overlapping instances.  Edges connecting to
// overlapping vertices are duplicated, one per each instance involved
// in the overlap.  Note that the "SUB" and "OVERLAP" labels are
// assumed to be the next two (respectively) labels beyond the label
// list given in the parameters, although they are not actually there
// until the graph is compressed for good at the end of the iteration.
//---------------------------------------------------------------------------

LabeledGraph *Compressor::CompressGraph(LabeledGraph *graph, InstanceList *instanceList,Params *parameters)
{
    InstanceListNode *instanceListNode;
    Instance *instance;
    ULONG instanceNo;
    ULONG numInstances;
    ULONG v, e;
    ULONG nv, ne;
    ULONG numInstanceVertices;
    ULONG numInstanceEdges;
    ULONG vertexIndex;
    ULONG subLabelIndex;
    ULONG overlapLabelIndex;
    LabeledGraph *compressedGraph;
    ULONG startVertex = 0;
    ULONG startEdge = 0;
    Increment *increment = NULL;
    
    // parameters used
    LabelList *labelList = parameters->labelList;
    BOOLEAN allowInstanceOverlap = parameters->allowInstanceOverlap;
    
    // assign "SUB" and "OVERLAP" labels an index of where they would be
    // in the label list if actually added
    subLabelIndex = labelList->getNumLabels();
    overlapLabelIndex = labelList->getNumLabels() + 1;
    
    if (parameters->incremental)
    {
        increment = parameters->GetCurrentIncrement();
        startVertex = increment->getStartPosVertexIndex();
        startEdge = increment->getStartPosEdgeIndex();
    }
    
    // mark and count unique vertices and edges in graph from instances
    numInstanceVertices = 0;
    numInstanceEdges = 0;
    instanceNo = 1;
    instanceListNode = instanceList->getHead();
    
    // Count number of vertices and edges that will be compressed by instances
    while (instanceListNode != NULL)
    {
        instance = instanceListNode->getInstance();
        for (v = 0; v < instance->getNumVertices(); v++)      // add in unique vertices
        {
            if ((!graph->getVertices()[instance->getVertices()[v]].isUsed()) &&
                ((!parameters->incremental) ||
                 (instance->getVertices()[v] >= startVertex)))
            {
                numInstanceVertices++;
                graph->getVertices()[instance->getVertices()[v]].setUsed(TRUE);
                // assign vertex to first instance it occurs in
                graph->getVertices()[instance->getVertices()[v]].setMap(instanceNo - 1);
            }
        }
        for (e = 0; e < instance->getNumEdges(); e++) // add in unique edges
            if ((!graph->getEdges()[instance->getEdges()[e]].isUsed()) &&
                ((!parameters->incremental) ||
                 (instance->getEdges()[e] >= startEdge)))
            {
                numInstanceEdges++;
                graph->getEdges()[instance->getEdges()[e]].setUsed(TRUE) ;
            }
        instanceNo++;
        instanceListNode = (InstanceListNode *) instanceListNode->getNext();
    }
    numInstances = instanceNo - 1;
    
    // allocate new graph with appropriate number of vertices and edges
    if (parameters->incremental)
    {
        nv = increment->getNumPosVertices() - numInstanceVertices + numInstances;
        ne = increment->getNumPosEdges() - numInstanceEdges;
    }
    else
    {
        nv = graph->getNumVertices() - numInstanceVertices + numInstances;
        ne = graph->getNumEdges() - numInstanceEdges;
    }
    compressedGraph = LabeledGraph::AllocateGraph(nv, ne);
    
    // insert SUB vertices for each instance
    vertexIndex = 0;
    for (v = 0; v < numInstances; v++)
    {
        compressedGraph->getVertices()[vertexIndex].setLabel(subLabelIndex) ;
        compressedGraph->getVertices()[vertexIndex].setNumEdges(0);
        compressedGraph->getVertices()[vertexIndex].setEdges (NULL);
        compressedGraph->getVertices()[vertexIndex].setMap(VERTEX_UNMAPPED);
        compressedGraph->getVertices()[vertexIndex].setUsed(FALSE) ;
        vertexIndex++;
    }
    
    // insert vertices and edges from non-compressed part of graph
    graph->CopyUnmarkedGraph( compressedGraph, vertexIndex, parameters);

   



    // add edges describing overlap, if appropriate (note: this will
    // unmark instance vertices)
    if (allowInstanceOverlap)
        graph->AddOverlapEdges(compressedGraph, instanceList, overlapLabelIndex,
                        startVertex, startEdge);
    
    // reset used flag of instances' vertices and edges
    instanceListNode = (InstanceListNode *) instanceList->getHead();
    while (instanceListNode != NULL)
    {
        instance = instanceListNode->getInstance();
        graph->MarkInstanceVertices(instance, FALSE);
        graph->MarkInstanceEdges(instance, FALSE);
        instanceListNode = (InstanceListNode *) instanceListNode->getNext();
    }
    
    return compressedGraph;
}


//---------------------------------------------------------------------------
// NAME: AddDuplicateEdges
//
// INPUTS: (Edge *overlapEdges) - edge array realloc-ed to store new edge
//         (ULONG *numOverlapEdgesPtr) - pointer to variable holding number
//           of total overlapping edges; will be incremented by 1, 2 or 3
//         (Edge *edge) - edge to be duplicated
//         (Graph *graph) - uncompressed graph containing edge
//         (ULONG sub1VertexIndex) - "SUB" vertex index for first instance
//         (ULONG sub2VertexIndex) - "SUB" vertex index for second instance
//
// RETURN: (Edge *) - realloc-ed edge array
//
// PURPOSE: Add duplicate edges to Sub2 based on overlapping vertex between
// Sub1 and Sub2.  The logic is complicated, so the description is offered
// as pseudocode:
//
//    if edge connects S1 to external vertex
//    then add duplicate edge connecting external vertex to S2
//    else "edge connects S1 to another (or same) vertex in S1"
//         add duplicate edge connecting S1 to S2
//         if other vertex is unmarked (overlapping and already processed)
//         then add duplicate edge connecting S2 to S2
//         if edge connects S1 to the same vertex in S1 (self edge)
//         then add duplicate edge connecting S2 to S2
//              if edge directed
//              then add duplicate edge from S2 to S1
//---------------------------------------------------------------------------

LabeledEdge *Compressor::AddDuplicateEdges(LabeledEdge *overlapEdges, ULONG *numOverlapEdgesPtr,LabeledEdge *edge, LabeledGraph *graph,
                        ULONG sub1VertexIndex, ULONG sub2VertexIndex)
{
    ULONG numOverlapEdges;
    ULONG v1, v2;
    
    numOverlapEdges = *numOverlapEdgesPtr;
    overlapEdges = (LabeledEdge *) realloc(overlapEdges,
                                    ((numOverlapEdges + 1) * sizeof(LabeledEdge)));
    if (overlapEdges == NULL)
        utility::OutOfMemoryError("AddDuplicateEdges:overlapEdges1");
    
    if (graph->getVertices()[edge->getE().first].getMap() != sub1VertexIndex)
    {
        // duplicate edge from external vertex
        v1 = graph->getVertices()[edge->getE().first].getMap();
        v2 = sub2VertexIndex;
        LabeledEdge::StoreEdge(overlapEdges, numOverlapEdges, v1, v2, edge->getLabel(),
                  edge->isDirected(), edge->isSpanIncrement());
        numOverlapEdges++;
    }
    else if (graph->getVertices()[edge->getE().second].getMap() != sub1VertexIndex)
    {
        // duplicate edge to an external vertex
        v1 = sub2VertexIndex;
        v2 = graph->getVertices()[edge->getE().second].getMap();
        LabeledEdge::StoreEdge(overlapEdges, numOverlapEdges, v1, v2, edge->getLabel(),
                  edge->isDirected(), edge->isSpanIncrement());
        numOverlapEdges++;
    }
    else
    {  // edge connects Sub1 to another (or same) vertex in Sub1
        // duplicate edge connecting Sub1 to Sub2
        v1 = sub1VertexIndex;
        v2 = sub2VertexIndex;
        LabeledEdge::StoreEdge(overlapEdges, numOverlapEdges, v1, v2, edge->getLabel(),
                  edge->isDirected(), edge->isSpanIncrement());
        numOverlapEdges++;
        // if other vertex unmarked (i.e., overlapping and already processed)
        // then duplicate edge connecting Sub2 to Sub2
        if ((! graph->getVertices()[edge->getE().first].isUsed()) ||
            (! graph->getVertices()[edge->getE().second].isUsed()))
        {
            overlapEdges = (LabeledEdge *) realloc(overlapEdges,
                                            ((numOverlapEdges + 1) * sizeof(LabeledEdge)));
            if (overlapEdges == NULL)
                utility::OutOfMemoryError ("AddDuplicateEdges:overlapEdges2");
            LabeledEdge::StoreEdge(overlapEdges, numOverlapEdges, v2, v2, edge->getLabel(),
                      edge->isDirected(), edge->isSpanIncrement());
            numOverlapEdges++;
        }
        // if edge connects Sub1 to the same vertex in Sub1 (self edge)
        // then add duplicate edge connecting Sub2 to Sub2
        if (edge->getE().first == edge->getE().second)
        {
            overlapEdges = (LabeledEdge *) realloc(overlapEdges,
                                            ((numOverlapEdges + 1) * sizeof(LabeledEdge)));
            if (overlapEdges == NULL)
                utility::OutOfMemoryError("AddDuplicateEdges:overlapEdges3");
            LabeledEdge::StoreEdge(overlapEdges, numOverlapEdges, v2, v2, edge->getLabel(),
                      edge->isDirected(), edge->isSpanIncrement());
            numOverlapEdges++;
            // if edge directed
            // then add duplicate edge from S2 to S1
            if (edge->isDirected())
            {
                overlapEdges = (LabeledEdge *) realloc(overlapEdges,
                                                ((numOverlapEdges + 1) * sizeof(LabeledEdge)));
                if (overlapEdges == NULL)
                    utility::OutOfMemoryError("AddDuplicateEdges:overlapEdges4");
                LabeledEdge::StoreEdge(overlapEdges, numOverlapEdges, v2, v1, edge->getLabel(),
                          edge->isDirected(), edge->isSpanIncrement());
                numOverlapEdges++;
            }
        }
    }
    *numOverlapEdgesPtr = numOverlapEdges;
    return overlapEdges;
}

//---------------------------------------------------------------------------
// NAME: CompressWithPredefinedSubs
//
// INPUTS: (Parameters *parameters)
//
// RETURN: (void)
//
// PURPOSE: Searches for predefined substructures in the positive and
// negative (if given) graphs.  If found, the graphs are compressed by
// the substructure.  This function de-allocates the
// parameters->preSubs array and the graphs it points to.
//---------------------------------------------------------------------------

void Compressor::CompressWithPredefinedSubs(Params *parameters)
{
    ULONG i;
    ULONG numPosInstances;
    ULONG numNegInstances;
    InstanceList *posInstanceList;
    InstanceList *negInstanceList;
    Substructure *sub;
    LabelList *newLabelList;
    
    // parameters used
    LabeledGraph *posGraph  = parameters->posGraph;
    LabeledGraph *negGraph  = parameters->negGraph;
    ULONG numPreSubs = parameters->numPreSubs;
    LabeledGraph **preSubs  = parameters->preSubs;
    
    for (i = 0; i < numPreSubs; i++)
    {
        posInstanceList = posGraph->FindInstances(preSubs[i], parameters);
        numPosInstances = posInstanceList->CountInstances();
        negInstanceList = NULL;
        numNegInstances = 0;
        if (negGraph != NULL)
        {
            negInstanceList = negGraph->FindInstances(preSubs[i], parameters);
            numNegInstances = negInstanceList->CountInstances();
        }
        // if found some instances, then report and compress
        if ((numPosInstances > 0) || (numNegInstances > 0))
        {
            sub = Substructure::AllocateSub();
            sub->setDefinition(preSubs[i]) ;
            printf ("Found %lu instances of predefined substructure %lu:\n",
                    (numPosInstances + numNegInstances), (i + 1));
            if (numPosInstances > 0)
            {
                printf("  %lu instances in positive graph\n", numPosInstances);
                sub->setInstances(posInstanceList) ;
                sub->setNumInstances(numPosInstances) ;
            }
            if (numNegInstances > 0)
            {
                printf("  %lu instances in negative graph\n", numNegInstances);
                sub->setNegInstances(negInstanceList) ;
                sub->setNumNegInstances(numNegInstances) ;
            }
            printf("  Compressing...\n");
            sub->CompressFinalGraphs( parameters, (i + 1), TRUE);
            posGraph = parameters->posGraph;
            negGraph = parameters->negGraph;
            sub->FreeSub();
        }
        else
        {
              cout << "I am here"<<endl;
            // no instances, so free the graph and the empty instance lists
            preSubs[i]->FreeGraph();
            if(posInstanceList!=NULL)    posInstanceList->FreeInstanceList();
            if (negInstanceList!=NULL)   negInstanceList->FreeInstanceList();
            
        }
    }
    free(parameters->preSubs);
    parameters->preSubs = NULL;
    
    // compress label list and recompute graphs' labels
    newLabelList = LabelList::AllocateLabelList();
    newLabelList->CompressLabelListWithGraph( posGraph, parameters);
    if (negGraph != NULL)
        newLabelList->CompressLabelListWithGraph( negGraph, parameters);
    parameters->labelList->FreeLabelList();
    parameters->labelList = newLabelList;
    
    // recompute graphs' MDL (necessary since label list has changed)
    if (parameters->evalMethod == EVAL_MDL)
    {
        parameters->posGraphDL =
        posGraph->MDL( newLabelList->getNumLabels(), parameters);
        if (parameters->negGraph != NULL)
            parameters->negGraphDL =
            negGraph->MDL(newLabelList->getNumLabels(), parameters);
    }
}




































