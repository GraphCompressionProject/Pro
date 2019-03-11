//
// Created by Macbook on 10/02/2019.
//
#include "stdafx.h"
#include "ReferenceGraph.hpp"
#include "Instance.hpp"
#include "LabeledGraph.hpp"
#include "Substructure.hpp"


//---------------------------------------------------------------------------
// NAME: MarkRefGraphInstanceEdges
//
// INPUT: (Instance *instance) - the instance to be used for marking
//        (ReferenceGraph *refGraph) - the graph to mark
//        (BOOLEAN value) - the value to mark
//
// RETURN:  void
//
// PURPOSE:  Mark the edges in the ref graph corresponding to the edges in the
//           instance as used (true/false).
//---------------------------------------------------------------------------

void ReferenceGraph::MarkRefGraphInstanceEdges(Instance *instance,
                               bool value)
{
    ULONG e;
    
    for (e = 0; e < instance->getNumEdges(); e++)
        edges[instance->getEdges()[e]].setUsed(value);
}


//---------------------------------------------------------------------------
// NAME:    CopyReferenceGraph
//
// INPUTS:  (Graph *g) - graph to be copied
//
// RETURN:  (ReferenceGraph *) - pointer to copy of graph
//
// PURPOSE:  Create and return a copy of the given ref graph.
//---------------------------------------------------------------------------

ReferenceGraph *ReferenceGraph::CopyReferenceGraph()
{
    ReferenceGraph *gCopy;
    ULONG nv;
    ULONG ne;
    ULONG v;
    ULONG e;
    ULONG numEdges;
    
    nv = this->numVertices;
    ne = this->numEdges;
    
    // allocate graph
    gCopy = AllocateReferenceGraph(nv, ne);
    
    // copy vertices; allocate and copy vertex edge arrays
    for (v = 0; v < nv; v++)
    {
        gCopy->getVertices()[v].setLabel(vertices[v].getLabel()) ;
        gCopy->getVertices()[v].setMap (vertices[v].getMap());
        gCopy->getVertices()[v].setUsed(vertices[v].isUsed()) ;
        gCopy->getVertices()[v].setVertexValid(vertices[v].isVertexValid());
        numEdges = vertices[v].getNumEdges();
        gCopy->getVertices()[v].setNumEdges(numEdges) ;
        gCopy->getVertices()[v].setEdges(NULL);
        if (numEdges > 0)
        {
            gCopy->getVertices()[v].setEdges( (ULONG *) malloc (numEdges * sizeof (ULONG)));
            if (gCopy->getVertices()[v].getEdges() == NULL)
                utility::OutOfMemoryError ("CopyGraph:edges");
            for (e = 0; e < numEdges; e++)
                gCopy->getVertices()[v].getEdges()[e] = vertices[v].getEdges()[e];
        }
    }
    
    // copy edges
    for (e = 0; e < ne; e++)
    {
        gCopy->getEdges()[e].setVertex1(edges[e].getVertex1()) ;
        gCopy->getEdges()[e].setVertex2(edges[e].getVertex2()) ;
        gCopy->getEdges()[e].setLabel(edges[e].getLabel()) ;
        gCopy->getEdges()[e].setDirected(edges[e].isDirected()) ;
        gCopy->getEdges()[e].setUsed(edges[e].isUsed()) ;
        gCopy->getEdges()[e].setMap(edges[e].getMap()) ;
        gCopy->getEdges()[e].setFailed( edges[e].isFailed());
    }
    return gCopy;
}

//---------------------------------------------------------------------------
// NAME: AddReferenceVertex
//
// INPUT: (ReferenceGraph *graph) - ref graph
//        (ULONG labelIndex) - the vertex's label index
//
// RETURN:  void
//
// PURPOSE:  Adds a vertex to a ref graph.
//---------------------------------------------------------------------------

void ReferenceGraph::AddReferenceVertex ( ULONG labelIndex)
{
    ReferenceVertex *newVertexList;
    ULONG numVertices = this->numVertices;
    ULONG vertexListSize = this->vertexListSize;
    
    // make sure there is enough room for another vertex
    if (vertexListSize == this->numVertices)
    {
        vertexListSize += LIST_SIZE_INC;
        newVertexList = (ReferenceVertex *) realloc
        (this->vertices, (sizeof (ReferenceVertex) * vertexListSize));
        if (newVertexList == NULL)
            utility::OutOfMemoryError("vertex list");
        this->vertices = newVertexList;
        this->vertexListSize = vertexListSize;
    }
    
    // store information in vertex
    this->vertices[numVertices].setLabel(labelIndex) ;
    this->vertices[numVertices].setNumEdges(0);
    this->vertices[numVertices].setEdges(NULL) ;
    this->vertices[numVertices].setMap(VERTEX_UNMAPPED) ;
    this->vertices[numVertices].setUsed(FALSE) ;
    this->vertices[numVertices].setVertexValid(TRUE) ;
    this->numVertices++;
}


//------------------------------------------------------------------------------
// NAME: AddReferenceEdge
//
// INPUT: (ReferenceGraph *graph) - ref graph
//        (ULONG sourceVertexIndex) - source of the edge
//        (ULONG targetVertexIndex) - target of the edge
//        (BOOLEAN directed)
//        (ULONG labelIndex) - edge's label index
//        (BOOLEAN spansIncrement) - true if edge spans the increment boundary
//
// RETURN:  void
//
// PURPOSE:  Add an edge to a ref graph.
//------------------------------------------------------------------------------

void ReferenceGraph::AddReferenceEdge(  ULONG sourceVertexIndex,
                      ULONG targetVertexIndex, BOOLEAN directed,
                      ULONG labelIndex, BOOLEAN spansIncrement)
{
    ReferenceEdge *newEdgeList;
    ULONG edgeListSize = this->edgeListSize;
    
    // make sure there is enough room for another edge in the graph
    if (edgeListSize == this->numEdges)
    {
        edgeListSize += LIST_SIZE_INC;
        newEdgeList =
        (ReferenceEdge *) realloc(this->edges, (sizeof (ReferenceEdge) * edgeListSize));
        if (newEdgeList == NULL)
            utility::OutOfMemoryError("AddEdge:newEdgeList");
        this->edges = newEdgeList;
        this->edgeListSize = edgeListSize;
    }
    
    // add edge to graph
    this->edges[this->numEdges].setVertex1(sourceVertexIndex) ;
    this->edges[this->numEdges].setVertex2(targetVertexIndex) ;
    this->edges[this->numEdges].setSpansIncrement(spansIncrement) ;
    this->edges[this->numEdges].setLabel(labelIndex) ;
    this->edges[this->numEdges].setDirected(directed) ;
    this->edges[this->numEdges].setUsed(FALSE) ;
    this->edges[this->numEdges].setFailed(FALSE) ;
    
    // add index to edge in edge index array of both vertices
    AddRefEdgeToRefVertices( this->numEdges);
    
    this->numEdges++;
}

//-----------------------------------------------------------------------------
// NAME: AddRefEdgeToRefVertices
//
// INPUT: (ReferenceGraph *graph) - ref graph
//        (ULONG edgeIndex) - the edge
//
// RETURN:  void
//
// PURPOSE:  For both vertices connected to an edge we add the edge to the
//           vertices' edge list.
//-----------------------------------------------------------------------------

void ReferenceGraph::AddRefEdgeToRefVertices(ULONG edgeIndex)
{
    ULONG v1, v2;
    ReferenceVertex *vertex;
    ULONG *edgeIndices;
    
    v1 = this->edges[edgeIndex].getVertex1();
    v2 = this->edges[edgeIndex].getVertex2();
    vertex = & this->vertices[v1];
    edgeIndices =
    (ULONG *) realloc(vertex->getEdges(), sizeof (ULONG) * (vertex->getNumEdges() + 1));
    if (edgeIndices == NULL)
        utility::OutOfMemoryError ("AddRefEdgeToRefVertices:edgeIndices1");
    edgeIndices[vertex->getNumEdges()] = edgeIndex;
    vertex->setEdges(edgeIndices) ;
    vertex->setVertexValid(TRUE);
    vertex->setNumEdges(vertex->getNumEdges()+1);
    
    if (v1 != v2)                        // do not add a self edge twice
    {
        vertex = & this->vertices[v2];
        edgeIndices = (ULONG *) realloc (vertex->getEdges(),
                                         sizeof (ULONG) * (vertex->getNumEdges() + 1));
        if (edgeIndices == NULL)
            utility::OutOfMemoryError ("AddRefEdgeToRefVertices:edgeIndices2");
        edgeIndices[vertex->getNumEdges()] = edgeIndex;
        vertex->setEdges(edgeIndices) ;
        vertex->setVertexValid(TRUE);
        vertex->setNumEdges(vertex->getNumEdges()+1);
    }
}


//---------------------------------------------------------------------------
// NAME:  ExtendRefGraph
//
// INPUTS:  (ReferenceGraph *refGraph) - reference graph
//          (Substructure *bestSub) - the currently best substructure
//          (Graph *fullGraph) - complete graph
//          (Parameters *parameters) - system parameters
//
// RETURN:  (ReferenceGraph *) - updated reference graph
//
// PURPOSE:  Extend the ref graph by an edge or an edge and vertex in
//           all possible ways.
//---------------------------------------------------------------------------

ReferenceGraph *ReferenceGraph::ExtendRefGraph( Substructure *bestSub,
                               LabeledGraph *fullGraph, Params *parameters)
{
    ReferenceGraph *newGraph;
    LabeledVertex *vertex;
    LabeledEdge *edge;
    ULONG labelIndex;
    ULONG v2;
    ULONG secondVertex = 0;
    ULONG firstVertex;
    ULONG v;
    ULONG e;
    ULONG i;
    BOOLEAN found;
    BOOLEAN directed;
    
    newGraph = CopyReferenceGraph();
    // When the new graph is created, it sizes the dynamic lists to the exact
    // number of vertices & edges.  The original graph may have bigger lists
    // because of the way the list sizes are incremented.
    
    fullGraph->MarkGraphEdgesUsed(this, TRUE);
    // Mark the fullGraph edges invalid
    fullGraph->MarkGraphEdgesValid(this, TRUE);
    
    for (v = 0; v <  numVertices; v++)
    {
        // If vertex has at least one valid edge connecting it to the graph
        if (vertices[v].isVertexValid())
        {
            vertex = &fullGraph->getVertices()[vertices[v].getMap()];
            for (e = 0; e < vertex->getNumEdges(); e++)
            {
                edge = &fullGraph->getEdges()[vertex->getEdges()[e]];
                if (!edge->isUsed() && edge->isValidPath())
                {
                    edge->setUsed(TRUE) ;
                    // get edge's other vertex
                    if (edge->getE().first == vertices[v].getMap())
                        v2 = edge->getE().second;
                    else
                        v2 = edge->getE().first;
                    // If second vertex is not in bestSub or if the second vertex is
                    // already used in another full instance of bestSub, then do not
                    // add the edge
                    if (bestSub->getDefinition()->VertexInSub( &fullGraph->getVertices()[v2]) &&
                        !bestSub->CheckVertexForOverlap(v2, parameters))
                    {
                        // check if edge's other vertex already in new reference graph
                        found = FALSE;
                        for (i = 0; ((i < newGraph->numVertices) && (! found)); i++)
                        {
                            if (newGraph->getVertices()[i].getMap() == v2)
                            {
                                found = TRUE;
                                secondVertex = i;
                            }
                        }
                        if (!found)
                        {
                            labelIndex = fullGraph->getVertices()[v2].getLabel();
                            newGraph->AddReferenceVertex( labelIndex);
                            newGraph->getVertices()[newGraph->getNumVertices()-1].setMap(v2) ;
                            secondVertex = newGraph->getNumVertices()-1;
                        }
                        labelIndex = edge->getLabel();
                        directed = edge->isDirected();
                        
                        // Just to get the ordering right
                        if (v2 == edge->getE().second)
                            firstVertex = v;
                        else
                        {
                            firstVertex = secondVertex;
                            secondVertex = v;
                        }
                        newGraph->AddReferenceEdge( firstVertex, secondVertex,
                                         directed, labelIndex, FALSE);
                        newGraph->getEdges()[newGraph->getNumEdges()-1].setMap(vertex->getEdges()[e]) ;
                    }
                }
            }
        }
    }
    
    fullGraph->MarkGraphEdgesUsed(newGraph, FALSE);
    //reset the fullGraph edges
    fullGraph->MarkGraphEdgesValid(this, FALSE);
    
    return newGraph;
}


//----------------------------------------------------------------------------
// NAME: MarkVertices
//
// INPUT: (ReferenceGraph *refGraph) - corresponding ref graph
//        (ULONG v1) - first vertex of failed edge
//        (ULONG v2) - second vertex of failed edge
//
// RETURN:  void
// PURPOSE: If a vertex in the refGraph is only connected by failed edges, then
// we mark that vertex temporarily as invalid so that we do not consider
// it for expansion until it becomes reconnected with a good edge.
//----------------------------------------------------------------------------

void ReferenceGraph::MarkVertices( ULONG v1, ULONG v2)
{
    BOOLEAN vertexValid;
    ReferenceVertex *refVertex;
    ReferenceEdge *refEdge;
    ULONG e;
    
    refVertex = &vertices[v1];
    vertexValid = FALSE;
    
    //if all of the vertex's edges have failed, then mark the vertex
    //as invalid
    for (e=0; e<refVertex->getNumEdges(); e++)
    {
        refEdge = &edges[refVertex->getEdges()[e]];
        if (!refEdge->isFailed())
        {
            vertexValid = TRUE;
            break;
        }
    }
    refVertex->setVertexValid(vertexValid) ;
    
    if (v1 != v2)
    {
        refVertex = &vertices[v2];
        vertexValid = FALSE;
        
        // If all of the vertex's edges have failed, then mark the vertex
        // as invalid
        for (e=0; e<refVertex->getNumEdges(); e++)
        {
            refEdge = &edges[refVertex->getEdges()[e]];
            if (!refEdge->isFailed())
            {
                vertexValid = TRUE;
                break;
            }
        }
        refVertex->setVertexValid(vertexValid) ;
    }
}







































































