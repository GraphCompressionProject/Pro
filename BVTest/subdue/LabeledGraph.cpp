//
// Created by Macbook on 10/02/2019.
//
#include "stdafx.h"
#include "LabeledGraph.hpp"
#include "utility.hpp"
#include "InstanceList.hpp"
#include "Params.hpp"
#include "LabeledGraph.hpp"
#include "Increment.hpp"
#include "Compressor.hpp"
#include "VertexMap.hpp"
#include "MatchHeap.hpp"
#include "ReferenceGraph.hpp"

void LabeledGraph::PrintGraph(LabelList *labelList){
     ULONG v;
   ULONG e;


      printf("  Graph(%luv,%lue):\n",numVertices, numEdges);
      // print vertices
      for (v = 0; v < numVertices; v++) 
      {
         printf("    ");
         PrintVertex( v, 0, labelList);
      }
      // print edges
      for (e = 0; e < numEdges; e++) 
      {
         printf("    ");
         PrintEdge( e, 0, labelList);
      }
   
}
               
//---------------------------------------------------------------------------
// NAME:    FreeGraph
//
// INPUTS:  (Graph *graph) - graph to be freed
//
// RETURN:  void
//
// PURPOSE: Free memory used by given graph, including the vertices array
// and the edges array for each vertex.
//---------------------------------------------------------------------------

void LabeledGraph::FreeGraph()
{
    unsigned long v;
    
    for (v = 0; v < numVertices; v++)
        delete vertices[v].getEdges();
    delete edges;
    delete vertices;

    
}
//---------------------------------------------------------------------------
// NAME: PrintVertex
//
// INPUTS: (Graph *graph) - graph containing vertex
//         (unsigned long vertexIndex) - index of vertex to print
//         (unsigned long vertexOffset) - offset for vertex index in the
//           case that this is one of many examples
//         (LabelList *labelList) - labels in graph
//
// RETURN: (void)
//
// PURPOSE: Print a vertex. Note that a graph containing many examples
// stores all vertices consecutively. Therefore, the first vertex of a
// later example will not have an index of 0, but an index of how many
// vertices came before in the number of examples. So, the vertexOffset
// is used to allow these vertices to be printed with indices corresponding
// to their order within the individual example, and not the entire graph
// of examples.
//---------------------------------------------------------------------------

void LabeledGraph::PrintVertex( unsigned long vertexIndex, unsigned long vertexOffset, LabelList *labelList)
{
   printf("v %lu ", vertexIndex - vertexOffset + 1);
    labelList->getHead()[vertices[vertexIndex].getLabel()].PrintLabel();
   printf("\n");
}


//---------------------------------------------------------------------------
// NAME: PrintEdge
//
// INPUTS: (Graph *graph) - graph containing edge
//         (unsigned long edgeIndex) - index of edge to print
//         (unsigned long vertexOffset) - offset for vertex index in the
//           case that this is one of many examples
//         (LabelList *labelList) - labels in graph
//
// RETURN: (void)
//
// PURPOSE: Print an edge. See note above on PrintVertex regarding the
// meaning of vertexOffset.
//---------------------------------------------------------------------------

void LabeledGraph::PrintEdge(unsigned long edgeIndex, unsigned long vertexOffset, LabelList *labelList)
{
   LabeledEdge edge =  edges[edgeIndex];
   if (edge.isDirected())
      printf("d");
   else 
      printf("u");
   printf(" %lu %lu ", edge.getE().first - vertexOffset + 1, edge.getE().second - vertexOffset + 1);
   labelList->getHead()[edge.getLabel()].PrintLabel( );
   printf("\n");
}


//---------------------------------------------------------------------------
// NAME:    CopyGraph
//
// INPUTS:  (Graph *g) - graph to be copied
//
// RETURN:  (Graph *) - pointer to copy of graph
//
// PURPOSE: Create and return a copy of the given graph.
//---------------------------------------------------------------------------

LabeledGraph *LabeledGraph::CopyGraph()
{
    LabeledGraph *gCopy;
    ULONG nv;
    ULONG ne;
    ULONG v;
    ULONG f;
    ULONG numEdges_;
    
    nv = numVertices;
    ne = numEdges;
    
    // allocate graph
    gCopy = LabeledGraph::AllocateGraph(nv, ne);
    
    // copy vertices; allocate and copy vertex edge arrays
    for (v = 0; v < nv; v++)
    {
        gCopy->getVertices()[v].setLabel(vertices[v].getLabel());
        gCopy->vertices[v].setMap(vertices[v].getMap());
        gCopy->vertices[v].setUsed(vertices[v].isUsed()) ;
        numEdges_ = vertices[v].getNumEdges();
        gCopy->vertices[v].setNumEdges(numEdges_) ;
        gCopy->getVertices()[v].setEdges(NULL) ;
       
        if (numEdges_ > 0)
        {
            gCopy->getVertices()[v].setEdges((ULONG *) malloc(numEdges_ * sizeof(ULONG)));
            if (gCopy->getVertices()[v].getEdges() == NULL)
                utility::OutOfMemoryError("CopyGraph:edges");
            for (f = 0; f < numEdges_; f++)
                gCopy->getVertices()[v].getEdges()[f] = vertices[v].getEdges()[f];
        }
    }
    
    // copy edges
    for (f = 0; f < ne; f++)
    {
        gCopy->getEdges()[f].getE().first = edges[f].getE().first;
        gCopy->getEdges()[f].getE().second = edges[f].getE().second;
        gCopy->getEdges()[f].setLabel(edges[f].getLabel()) ;
        gCopy->getEdges()[f].setDirected(edges[f].isDirected()) ;
        gCopy->getEdges()[f].setUsed(edges[f].isUsed()) ;
    }
    
    return gCopy;
}



//---------------------------------------------------------------------------
// NAME: AddEdge
//
// INPUTS: (Graph *graph) - graph to add edge to
//         (ULONG sourceVertexIndex) - index of edge's source vertex
//         (ULONG targetVertexIndex) - index of edge's target vertex
//         (BOOLEAN directed) - TRUE is edge is directed
//         (ULONG labelIndex) - index of edge's label in label list
//         (ULONG spansIncrement)
//
// RETURN: (void)
//
// PURPOSE: Add edge information to graph. AddEdge also changes the
// size of the currently-allocated edge array, which increases by
// LIST_SIZE_INC (instead of just 1) when exceeded.
//---------------------------------------------------------------------------

void LabeledGraph::AddEdge(ULONG sourceVertexIndex, ULONG targetVertexIndex,
             BOOLEAN directed, ULONG labelIndex, BOOLEAN spansIncrement)
{
    LabeledEdge *newEdgeList;
    ULONG edgeListSize = this->edgeListSize;
    
    // make sure there is enough room for another edge in the graph
    if (edgeListSize == this->numEdges)
    {
        edgeListSize += LIST_SIZE_INC;
        newEdgeList = (LabeledEdge *) realloc(this->edges, (sizeof(LabeledEdge) * (edgeListSize)));
        if (newEdgeList == NULL)
            utility::OutOfMemoryError("AddEdge:newEdgeList");
        this->edges = newEdgeList;
        this->edgeListSize = edgeListSize;
    }
    
    // add edge to graph
    this->edges[numEdges].getE().first  = sourceVertexIndex;
    this->edges[numEdges].getE().second = targetVertexIndex;
    this->edges[numEdges].setLabel(labelIndex);
    this->edges[numEdges].setDirected(directed);
    this->edges[numEdges].setUsed(FALSE);
    this->edges[numEdges].setSpanIncrement(spansIncrement);
    this->edges[numEdges].setValidPath(TRUE);
    
    // add index to edge in edge index array of both vertices
    AddEdgeToVertices(numEdges);
    
    numEdges++;
}


//---------------------------------------------------------------------------
// NAME: MDL
//
// INPUTS: (Graph *graph) - graph whose description length to be computed
//         (ULONG numLabels) - number of labels from which to choose for
//                             the labels in graph
//         (Parameters *parameters)
//
// RETURN: (double) - description length of graph in bits
//
// PURPOSE: Computes a minimal encoding of the graph in terms of bits
// as follows:
//
//   MDL (graph) = vertexBits + rowBits + edgeBits
//
//   vertexBits = lg(V) + V * lg(L)
//
//   rowBits = (V+1) * lg(B+1) + sum(i=0,V) lg C(V,k_i)
//
//   edgeBits = E * (1 + lg(L)) + (K+1) * lg(M)
//
// V is the number of vertices in the graph, E is the number of edges
// in the graph, and L is the number of unique labels (numLabels) in
// the graph.  If we assume an adjacency matrix representation of the
// graph, such that A[i,j] = 1 if there are one or more edges between
// vertex i and j, then k_i is the number of 1s in the ith row of A,
// then B = max(k_i) and K = sum(i=0,V) k_i.  Finally, M is the
// maximum number of edges between any two vertices in the graph.
//
// While the encoding is not provably minimal, a lot of work has been
// done to make it minimal.  See the Subdue papers for details on the
// encoding.
//---------------------------------------------------------------------------



double LabeledGraph::MDL( ULONG numLabels, Params *parameters)
{
    double vertexBits;
    double rowBits;
    double edgeBits;
    double totalBits;
    ULONG v1;
    ULONG V;  // number of vertices
    ULONG E;  // number of edges
    ULONG L;  // number of unique labels
    ULONG ki; // number of 1s in row i of adjacency matrix
    ULONG B;  // maximum number of 1s in a row of the adjacency matrix
    ULONG K;  // number of 1s in adjacency matrix
    ULONG M;  // maximum number of edges between any two vertices
    ULONG tmpM;
    
    V = this->numVertices;
    E = this->numEdges;
    L = numLabels;


    vertexBits = utility::Log2(V) + (V * utility::Log2(L));


    rowBits = V * utility::Log2Factorial(V, parameters);

    edgeBits = E * (1 + utility::Log2(L));
    B = 0;
    K = 0;
    M = 0;
    for (v1 = 0; v1 < V; v1++)
    {
        ki = NumUniqueEdges(v1);
        rowBits -= (utility::Log2Factorial(ki, parameters) +
                    utility::Log2Factorial((V - ki), parameters));
        if (ki > B)
        {
            B = ki;
        }
        K += ki;
        tmpM = MaxEdgesToSingleVertex(v1);
        if (tmpM > M)
        {
            M = tmpM;
        }
    }
    rowBits += ((V + 1) * utility::Log2(B + 1));
    edgeBits += ((K + 1) * utility::Log2(M));
    totalBits = vertexBits + rowBits + edgeBits;

    return totalBits;
}


//---------------------------------------------------------------------------
// NAME: NumUniqueEdges
//
// INPUTS: (Graph *graph) - graph containing vertex
//         (ULONG v1) - vertex to find number of unique edges
//
// RETURN: (ULONG) - number of unique edges of vertex v1
//
// PURPOSE: Compute the number of different vertices that vertex v1 has an
// edge to.  If edge is undirected, then it is included only if connected
// to a larger-numbered vertex (or itself).  This prevents double counting
// undirected edges.
//---------------------------------------------------------------------------

ULONG LabeledGraph::NumUniqueEdges(ULONG v1)
{
    ULONG e;
   ULONG v2;
   LabeledEdge *edge;
   ULONG numUniqueEdges;

   numUniqueEdges = 0;
   // look through all edges of vertex v1
   for (e = 0; e < this->vertices[v1].getNumEdges(); e++) 
   {

      edge = & this->edges[this->vertices[v1].getEdges()[e]];
      if (edge->getE().first == v1)
         v2 = edge->getE().second;
      else 
         v2 = edge->getE().first;

      // check if edge connected to unique vertex
      if (((edge->isDirected()) && (edge->getE().first == v1)) || // out-going edge
          ((! edge->isDirected()) && (v2 >= v1))) 
      {
         if (! this->vertices[v2].isUsed()) 
         {
            numUniqueEdges++;
            this->vertices[v2].setUsed(TRUE) ;
         }
      }
   }
   // reset vertex used flags
   for (e = 0; e < this->vertices[v1].getNumEdges(); e++) 
   {
      edge = & this->edges[this->vertices[v1].getEdges()[e]];
      this->vertices[edge->getE().first].setUsed(FALSE);
      this->vertices[edge->getE().second].setUsed(FALSE);
   }
   return numUniqueEdges;
}


//---------------------------------------------------------------------------
// NAME: MaxEdgesToSingleVertex
//
// INPUTS: (Graph *graph) - graph containing vertex
//         (ULONG v1) - vertex to look for maximum edges
//
// RETURN: (ULONG) - maximum number of edges from v1 to another vertex
//
// PURPOSE: Computes the maximum number of edges between v1 and
// another vertex (including v1 itself).
//---------------------------------------------------------------------------

ULONG LabeledGraph::MaxEdgesToSingleVertex( ULONG v1)
{
    ULONG maxEdges;
    ULONG numEdges;
    ULONG i, j;
    ULONG v2, v2j;
    LabeledEdge *edge1, *edge2;
    ULONG numEdgesToVertex2;
    
    maxEdges = 0;
    numEdges = vertices[v1].getNumEdges();
    for (i = 0; i < numEdges; i++)
    {
        edge1 = & edges[vertices[v1].getEdges()[i]];
        // get other vertex
        v2 = (edge1->getE().first == v1) ? edge1->getE().second : edge1->getE().first;
        
        if (((edge1->isDirected()) && (edge1->getE().first == v1)) || // outgoing edge
            ((! edge1->isDirected()) && (v2 >= v1)))
        {
            // count how many edges to v2
            numEdgesToVertex2 = 1;
            for (j = i + 1; j < numEdges; j++)
            { //start at i+1 since only want max
                edge2 = & edges[vertices[v1].getEdges()[j]];
                v2j = (edge2->getE().first == v1) ? edge2->getE().second : edge2->getE().first;
                
                if (v2j == v2)
                {
                    // outgoing edge
                    if (((edge2->isDirected()) && (edge2->getE().first == v1)) ||
                        ((! edge1->isDirected()) && (v2j >= v1)))
                    {
                        numEdgesToVertex2++;
                    }
                }
            }
            if (numEdgesToVertex2 > maxEdges)
                maxEdges = numEdgesToVertex2;
        }
    }
    return maxEdges;
}
//---------------------------------------------------------------------------
// NAME: CopyUnmarkedGraph
//
// INPUTS: (Graph *g1) - graph to copy unused structure from
//         (Graph *g2) - graph to copy to
//         (ULONG vertexIndex) - index into g2's vertex array where to
//           start copying unused vertices from g1
//         (Parameters *parameters)
//
// RETURN: (void)
//
// PURPOSE: Copy unused vertices and edges from g1 to g2, starting at
// vertexIndex of g2's vertex array.  Ensures that copied edges map to
// correct vertices in g2.
//---------------------------------------------------------------------------

void LabeledGraph::CopyUnmarkedGraph( LabeledGraph *g2, ULONG vertexIndex,
                                     Params *parameters)
{
    ULONG v, e;
    ULONG v1, v2;
    ULONG edgeIndex;
    ULONG vertexOffset;
    ULONG edgeOffset;
    Increment *increment = NULL;
    
    if (parameters->incremental)
    {
        increment = parameters->GetCurrentIncrement();
        vertexOffset = increment->getStartPosVertexIndex();
        
        // Copy the newly compressed increment vertices to the new graph
        for (v = 0; v < increment->getNumPosVertices(); v++)
            if (!vertices[vertexOffset + v].isUsed())
            {
                g2->getVertices()[vertexIndex].setLabel(vertices[vertexOffset + v].getLabel()) ;
                // We have to tell the data generation module how to
                // link to this data from the next increment
                g2->getVertices()[vertexIndex].setNumEdges(0);
                g2->getVertices()[vertexIndex].setEdges(NULL) ;
                g2->getVertices()[vertexIndex].setMap(VERTEX_UNMAPPED);
                g2->getVertices()[vertexIndex].setUsed(FALSE) ;
                vertices[vertexOffset + v].setMap(vertexIndex) ;
                vertexIndex++;
            }
        
        // Copy the newly compressed increment edges to the new graph
        edgeOffset = increment->getStartPosEdgeIndex();
        edgeIndex = 0;
        for (e = 0; e < increment->getNumPosEdges(); e++)
            if (!edges[edgeOffset + e].isUsed())
            {
                if (edges[edgeOffset + e].isSpanIncrement())
                    g2->setNumEdges(g2->getNumEdges() - 1) ;
                else
                {
                    v1 = vertices[edges[edgeOffset + e].getE().first].getMap();
                    v2 = vertices[edges[edgeOffset + e].getE().second].getMap();
                    LabeledEdge::StoreEdge(g2->getEdges(), edgeIndex, v1, v2,
                                           edges[edgeOffset + e].getLabel(),
                                           edges[edgeOffset + e].isDirected(),
                                           edges[edgeOffset + e].isSpanIncrement());
                    g2->AddEdgeToVertices(edgeIndex);
                    edgeIndex++;
                }
            }
    }
    else
    {
        // copy unused vertices to g2
        for (v = 0; v < this->numVertices; v++)
            if (! vertices[v].isUsed())
            {
                g2->getVertices()[vertexIndex].setLabel(vertices[v].getLabel()) ;
                g2->getVertices()[vertexIndex].setNumEdges(0) ;
                g2->getVertices()[vertexIndex].setEdges(NULL);
                g2->getVertices()[vertexIndex].setMap(VERTEX_UNMAPPED);
                g2->getVertices()[vertexIndex].setUsed(FALSE);
                this->vertices[v].setMap(vertexIndex)  ;
                vertexIndex++;
            }
        // copy unused edges to g2
        edgeIndex = 0;
        for (e = 0; e < this->numEdges; e++)
            if (! edges[e].isUsed())
            {
                v1 = vertices[edges[e].getE().first].getMap();
                v2 = vertices[edges[e].getE().second].getMap();
                LabeledEdge::StoreEdge(g2->getEdges(), edgeIndex, v1, v2, edges[e].getLabel(),
                                       edges[e].isDirected(), edges[e].isSpanIncrement());
                g2->AddEdgeToVertices(edgeIndex);
                edgeIndex++;
            }
        
    
    }
}


//---------------------------------------------------------------------------
// NAME: AddOverlapEdges
//
// INPUTS: (Graph *compressedGraph) - compressed graph to add edges to
//         (Graph *graph) - graph being compressed
//         (InstanceList *instanceList) - substructure instances used to
//                                        compress graph
//         (ULONG overlapLabelIndex) - index into label list of "OVERLAP"
//                                     label
//         (ULONG startVertex) - index of the first vertex in the increment
//         (ULONG startEdge) - index of the first edge in the increment
//
// RETURN: (void)
//
// PURPOSE: This procedure adds edges to the compressedGraph
// describing overlapping instances of the substructure in the given
// graph.  First, if two instances overlap at all, then an undirected
// "OVERLAP" edge is added between them.  Second, if an external edge
// points to a vertex shared between multiple instances, then
// duplicate edges are added to all instances sharing the vertex.
//
// This procedure makes the following assumptions about its inputs,
// which are satisfied by CompressGraph().
//
// 1. The "SUB" vertices for the n instances are the first n vertices
// in the compressed graph's vertex array, in order corresponding to
// the instance list's order.  I.e., the ith instance in the instance
// list corresponds to compressedGraph->vertices[i-1].
//
// 2. All vertices and edges in the instances are marked (used=TRUE)
// in the graph.  Instance vertices will be unmarked as processed.
//
// 3. The vertices in the given graph are all mapped to their appropriate
// vertices in the compressedGraph.
//
// 4. For external edges pointing to vertices shared by multiple
// instances, the compressed graph already contains one such edge
// pointing to the first "SUB" vertex corresponding to the instance in
// the substructure's instance list that contains the shared vertex.
//---------------------------------------------------------------------------

void LabeledGraph::AddOverlapEdges(LabeledGraph *compressedGraph,
                                   InstanceList *instanceList, ULONG overlapLabelIndex,
                                   ULONG startVertex, ULONG startEdge)
{
    InstanceListNode *instanceListNode1;
    InstanceListNode *instanceListNode2;
    ULONG instanceNo1;
    ULONG instanceNo2;
    Instance *instance1;
    Instance *instance2;
    ULONG v1;
    ULONG v2;
    ULONG e;
    LabeledVertex *vertex1;
    LabeledVertex *vertex2;
    LabeledEdge *edge1;
    LabeledEdge *overlapEdges;
    ULONG numOverlapEdges;
    ULONG totalEdges;
    ULONG edgeIndex;
    
    Compressor c;
    
    overlapEdges = NULL;
    numOverlapEdges = 0;
    // for each instance1 in substructure's instance
    instanceListNode1 = instanceList->getHead();
    instanceNo1 = 1;
    while (instanceListNode1 != NULL)
    {
        instance1 = instanceListNode1->getInstance();
        // for each vertex in instance1
        for (v1 = 0; v1 < instance1->getNumEdges(); v1++)
        {
            vertex1 = &vertices[instance1->getVertices()[v1]];
            if (vertex1->isUsed())
            {  // (used==TRUE) indicates unchecked for sharing
                // for each instance2 after instance1
                instanceListNode2 = (InstanceListNode *) instanceListNode1->getNext();
                instanceNo2 = instanceNo1 + 1;
                while (instanceListNode2 != NULL)
                {
                    instance2 = instanceListNode2->getInstance();
                    // for each vertex in instance2
                    for (v2 = 0; v2 < instance2->getNumVertices(); v2++)
                    {
                        vertex2 = &vertices[instance2->getVertices()[v2]];
                        if ((vertex1 == vertex2) &&
                            (instance1->getVertices()[v1] >= startVertex))
                        {  // point to same vertex, thus overlap
                            // add undirected "OVERLAP" edge between corresponding
                            // "SUB" vertices, if not already there
                            overlapEdges =
                            LabeledEdge::AddOverlapEdge(overlapEdges, &numOverlapEdges,
                                                        instanceNo1 - 1, instanceNo2 - 1,
                                                        overlapLabelIndex);
                            // for external edges involving vertex1,
                            // duplicate for vertex2
                            for (e = 0; e < vertex1->getNumEdges(); e++)
                            {
                                edge1 = &edges[vertex1->getEdges()[e]];
                                if ((!edge1->isUsed()) &&
                                    (vertex1->getEdges()[e] >= startEdge))
                                { // edge external to instance
                                    overlapEdges =c.AddDuplicateEdges(overlapEdges, &numOverlapEdges,
                                                      edge1, this, instanceNo1 - 1, instanceNo2 - 1);
                                }
                            }
                        }
                    }
                    instanceListNode2 = (InstanceListNode *)instanceListNode2->getNext();
                    instanceNo2++;
                }
                vertex1->setUsed(FALSE) ; // i.e., done processing vertex1 for overlap
            }
        }
        instanceListNode1 =(InstanceListNode *) instanceListNode1->getNext();
        instanceNo1++;
    }
    
    // add overlap edges to compressedGraph
    if (numOverlapEdges > 0)
    {
        totalEdges = compressedGraph->getNumEdges() + numOverlapEdges;
        compressedGraph->setEdges((LabeledEdge *) realloc(compressedGraph->getEdges(), (totalEdges * sizeof(LabeledEdge)))) ;
        if (compressedGraph->getEdges() == NULL)
            utility::OutOfMemoryError("AddOverlapEdges:compressedGraph->edges");
        edgeIndex = compressedGraph->numEdges;
        for (e = 0; e < numOverlapEdges; e++)
        {
            LabeledEdge::StoreEdge(compressedGraph->getEdges(), edgeIndex,
                                   overlapEdges[e].getE().first, overlapEdges[e].getE().second,
                                   overlapEdges[e].getLabel(), overlapEdges[e].isDirected(),
                                   overlapEdges[e].isSpanIncrement());
            compressedGraph->AddEdgeToVertices( edgeIndex);
            edgeIndex++;
        }
        compressedGraph->setNumEdges(compressedGraph->getNumEdges() + numOverlapEdges);
        free(overlapEdges);
    }
}


//---------------------------------------------------------------------------
// NAME: MarkInstanceVertices
//
// INPUTS: (Instance *instance) - instance whose vertices to set
//         (Graph *graph) - graph containing instance
//         (BOOLEAN value) - value to set edge's used flag
//
// RETURN: (void)
//
// PURPOSE: Set the used flag to the given value for each vertex in
// instance.
//---------------------------------------------------------------------------

void LabeledGraph::MarkInstanceVertices(Instance *instance,  BOOLEAN value)
{
    ULONG v;
    
    for (v = 0; v < instance->getNumVertices(); v++)
        vertices[instance->getVertices()[v]].setUsed(value);
}

//---------------------------------------------------------------------------
// NAME: MarkInstanceEdges
//
// INPUTS: (Instance *instance) - instance whose edges to set
//         (Graph *graph) - graph containing instance
//         (BOOLEAN value) - value to set edge's used flag
//
// RETURN: (void)
//
// PURPOSE: Set the used flag to the given value for each edge in
// instance.
//---------------------------------------------------------------------------

void LabeledGraph::MarkInstanceEdges(Instance *instance,  BOOLEAN value)
{
    ULONG e;
    
    for (e = 0; e < instance->getNumEdges(); e++)
        edges[instance->getEdges()[e]].setUsed(value);
}


//---------------------------------------------------------------------------
// NAME: ExternalEdgeBits
//
// INPUTS: (Graph *compressedGraph) - graph compressed with a substructure
//         (Graph *subGraph) - substructure's graph definition
//         (ULONG numInstances) - number of substructure instances
//
// RETURN: (double) - bits describing external edge connections
//
// PURPOSE: Computes and returns the number of bits necessary to
// describe which vertex in a substructure's instance does an external
// edge connect to.  This is computed as lg(vertices(S)), where S is
// the substructure used to compress the graph.  While this
// information is not retained in the compressed graph, it is
// important to include in the MDL computation so that the values are
// consistent with a quasi-lossless compression.
//
// Note that this procedure assumes that the "SUB" vertices in
// compressedGraph occupy the first numInstances vertices in the
// graph's vertex array.  This property is maintained by the
// CompressGraph() procedure.
//---------------------------------------------------------------------------

double LabeledGraph::ExternalEdgeBits( LabeledGraph *subGraph,
                        ULONG numInstances)
{
    ULONG v;
    ULONG e;
    LabeledVertex *vertex;
    LabeledEdge *edge;
    double log2SubVertices;
    double edgeBits;
    
    log2SubVertices = utility::Log2(subGraph->getNumVertices());
    edgeBits = 0.0;
    // add bits for each edge connected to "SUB" vertex
    for (v = 0; v < numInstances; v++)
    {
        vertex = & vertices[v];
        for (e = 0; e < vertex->getNumEdges(); e++)
        {
            edge = & edges[vertex->getEdges()[e]];
            edgeBits += log2SubVertices;
            if (edge->getE().first == edge->getE().second) // self-edge
                edgeBits += log2SubVertices;
        }
    }
    return edgeBits;
}



//---------------------------------------------------------------------------
// NAME: GraphSize
//
// INPUTS: (Graph *graph)
//
// RETURN: (ULONG) - size of graph
//
// PURPOSE: Return size of graph as vertices plus edges.
//---------------------------------------------------------------------------

ULONG LabeledGraph::GraphSize()
{
    return(numVertices + numEdges);
}


//---------------------------------------------------------------------------
// NAME: SizeOfCompressedGraph
//
// INPUTS: (Graph *graph) - graph to be compressed
//         (InstanceList *instanceList) - instances of compressing sub
//         (Parameters *parameters)
//         (ULONG graphType)
//
// RETURN: (ULONG) - size of graph compressed with substructure
//
// PURPOSE: Computes the size (vertices+edges) of the given graph as
// if compressed using the given instances.  The compression is not
// actually done, since the compressed graph size can be determined
// computationally by subtracting the unique structure in the
// substructure instances and adding one new vertex per instance.  If
// overlap is allowed, then new edges are included to describe the
// overlap between instances and to duplicate edges to and from
// overlapping vertices.
//---------------------------------------------------------------------------

ULONG LabeledGraph::SizeOfCompressedGraph( InstanceList *instanceList,Params *parameters, ULONG graphType)
{
    ULONG size;
    InstanceListNode *instanceListNode;
    Instance *instance;
    ULONG v, e;
    BOOLEAN allowInstanceOverlap = parameters->allowInstanceOverlap;
    
    if (parameters->incremental)
        size = parameters->GetIncrement(parameters->GetCurrentIncrementNum())->IncrementSize(graphType);
    else size = GraphSize();
    
    if (instanceList != NULL)
    {
        instanceListNode = instanceList->getHead();
        if (allowInstanceOverlap)
        {
            // reduce size by amount of unique structure, which is marked
            while (instanceListNode != NULL)
            {
                size++; // new "SUB" vertex of instance
                instance = instanceListNode->getInstance();
                // subtract unique vertices
                for (v = 0; v < instance->getNumVertices(); v++)
                    if (!vertices[instance->getVertices()[v]].isUsed())
                    {
                        size--;
                        vertices[instance->getVertices()[v]].setUsed(TRUE) ;
                    }
                for (e = 0; e < instance->getNumEdges(); e++)   // subtract unique edges
                    if (!edges[instance->getEdges()[e]].isUsed())
                    {
                        size--;
                        edges[instance->getEdges()[e]].setUsed(TRUE) ;
                    }
                instanceListNode =(InstanceListNode *) instanceListNode->getNext();
            }
            // increase size by number of overlap edges (assumes marked instances)
            size += NumOverlapEdges(instanceList);
            // reset used flag of instances' vertices and edges
            instanceListNode = instanceList->getHead();
            while (instanceListNode != NULL)
            {
                instance = instanceListNode->getInstance();
                MarkInstanceVertices(instance, FALSE);
                MarkInstanceEdges(instance, FALSE);
                instanceListNode =(InstanceListNode *) instanceListNode->getNext();
            }
        }
        else
        {
            // no overlap, so just subtract size of instances
            while (instanceListNode != NULL)
            {
                size++; // new "SUB" vertex of instance
                instance = instanceListNode->getInstance();
                size -= (instance->getNumVertices() + instance->getNumEdges());
                instanceListNode = (InstanceListNode *)instanceListNode->getNext();
            }
        }
    }
    return size;
}


//---------------------------------------------------------------------------
// NAME: NumOverlapEdges
//
// INPUTS: (Graph *graph) - graph being "compressed" by substructure
//         (InstanceList *instanceList) - substructure instances
//
// RETURN: (ULONG) - number of "OVERLAP" and duplicate edges needed
//
// PURPOSE: Computes the number of "OVERLAP" and duplicate edges that
// would be needed to compress the graph with the substructure
// instances.  If two instances overlap at all, then an undirected
// "OVERLAP" edge is added between them.  If an external edge points
// to a vertex shared between multiple instances, then duplicate edges
// are added to all instances sharing the vertex.
//
// This procedure assumes all vertices and edges in the instances are
// marked (used=TRUE) in the graph.  Instance vertices will be
// unmarked as processed.
//---------------------------------------------------------------------------

ULONG LabeledGraph::NumOverlapEdges( InstanceList *instanceList)
{
    InstanceListNode *instanceListNode1;
    InstanceListNode *instanceListNode2;
    ULONG instanceNo1;
    ULONG instanceNo2;
    Instance *instance1;
    Instance *instance2;
    ULONG v1;
    ULONG v2;
    ULONG e;
    LabeledVertex *vertex1;
    LabeledVertex *vertex2;
    LabeledEdge *edge1;
    LabeledEdge *overlapEdges;
    ULONG numOverlapEdges;
    ULONG overlapLabelIndex;
    Compressor c ;
    overlapLabelIndex = 0; // bogus value never used since graph not compressed
    overlapEdges = NULL;
    numOverlapEdges = 0;
    // for each instance1 in substructure's instance
    instanceListNode1 = (InstanceListNode *)instanceList->getHead();
    instanceNo1 = 1;
    while (instanceListNode1 != NULL)
    {
        instance1 = instanceListNode1->getInstance();
        // for each vertex in instance1
        for (v1 = 0; v1 < instance1->getNumVertices(); v1++)
        {
            vertex1 = & vertices[instance1->getVertices()[v1]];
            if (vertex1->isUsed())
            { // (used==TRUE) indicates unchecked for sharing
                // for each instance2 after instance1
                instanceListNode2 = (InstanceListNode *)instanceListNode1->getNext();
                instanceNo2 = instanceNo1 + 1;
                while (instanceListNode2 != NULL)
                {
                    instance2 = instanceListNode2->getInstance();
                    // for each vertex in instance2
                    for (v2 = 0; v2 < instance2->getNumVertices(); v2++)
                    {
                        vertex2 = & vertices[instance2->getVertices()[v2]];
                        if (vertex1 == vertex2)
                        { // point to same vertex, thus overlap
                            // add undirected "OVERLAP" edge between corresponding
                            // "SUB" vertices, if not already there
                            overlapEdges =
                            LabeledEdge::AddOverlapEdge(overlapEdges, & numOverlapEdges,
                                           instanceNo1 - 1, instanceNo2 - 1,
                                           overlapLabelIndex);
                            // for external edges involving vertex1,
                            // duplicate for vertex2
                            for (e = 0; e < vertex1->getNumEdges(); e++)
                            {
                                edge1 = & edges[vertex1->getEdges()[e]];
                                if (! edge1->isUsed())
                                { // edge external to instance
                                    overlapEdges = c.AddDuplicateEdges(overlapEdges, & numOverlapEdges,
                                                      edge1, this, instanceNo1 - 1, instanceNo2 - 1);
                                }
                            }
                        }
                    }
                    instanceListNode2 = (InstanceListNode *)instanceListNode2->getNext();
                    instanceNo2++;
                }
                vertex1->setUsed(FALSE); // i.e., done processing vertex1 for overlap
            }
        }
        instanceListNode1 = (InstanceListNode *)instanceListNode1->getNext();
        instanceNo1++;
    }
    free (overlapEdges);
    return numOverlapEdges;
}


//---------------------------------------------------------------------------
// NAME:    GraphMatch
//
// INPUTS:  (Graph *g1)
//          (Graph *g2) - graphs to be matched
//          (LabelList *labelList) - list of vertex and edge labels
//          double threshold - upper bound on match cost
//          double *matchCost - pointer to pass back actual cost of match;
//                              ignored if NULL
//          (VertexMap *mapping) - array to hold final vertex mapping;
//                                 ignored if NULL
//
// RETURN:  (BOOLEAN) - TRUE is graphs match with cost less than threshold
//
// PURPOSE: Returns TRUE if g1 and g2 match with cost less than the given
// threshold.  If so, side-effects are to store the match cost in the
// variable pointed to by matchCost and to store the mapping between g1 and
// g2 in the given mapping input if non-NULL.
//---------------------------------------------------------------------------

BOOLEAN LabeledGraph::GraphMatch( LabeledGraph *g2, LabelList *labelList,
                   double threshold, double *matchCost, VertexMap *mapping)
{
    double cost;
    
    // first, quick check for exact matches
    if ((threshold == 0.0) &&
        ((numVertices != g2->numVertices) ||
         (numEdges != g2->numEdges)))
        return FALSE;
    
    // call InexactGraphMatch with larger graph first
    if (numVertices < g2->numVertices)
        cost = g2->InexactGraphMatch( this, labelList, threshold, mapping);
    else
        cost = this->InexactGraphMatch( g2, labelList, threshold, mapping);
    
    // pass back actual match cost, if desired
    if (matchCost != NULL)
        *matchCost = cost;
    
    if (cost > threshold)
        return FALSE;
    
    return TRUE;
}



//---------------------------------------------------------------------------
// NAME:    InexactGraphMatch
//
// INPUTS:  Graph *g1
//          Graph *g2 - graphs to be matched
//          LabelList *labelList - list of vertex and edge labels
//          double threshold - upper bound on match cost
//          (VertexMap *mapping) - array to hold final vertex mapping;
//                                 if NULL, then ignored
//
// RETURN:  Cost of transforming g1 into an isomorphism of g2.  Will be
//          MAX_DOUBLE if cost exceeds threshold
//
// PURPOSE: Compute minimum-cost transformation of g1 to an
// isomorphism of g2, but any match cost exceeding the given threshold
// is not considerd.  Graph g1 should be the larger graph in terms of
// vertices.  A side-effect is to store the mapping between g1 and g2
// in the given mapping input if non-NULL.
//
// TODO: May want to input a partial mapping to influence mapping
// order of vertices in g1.
//---------------------------------------------------------------------------

double LabeledGraph::InexactGraphMatch(LabeledGraph *g2, LabelList *labelList,
                         double threshold, VertexMap *mapping)
{
    ULONG i, v1, v2;
    ULONG nv1 = numVertices;
    ULONG nv2 = g2->numVertices;
    LabeledEdge *edge;
    MatchHeap *globalQueue = NULL;
    MatchHeap *localQueue = NULL;
    MatchHeapNode node;
    MatchHeapNode newNode;
    MatchHeapNode bestNode;
    double cost = 0.0;
    double newCost = 0.0;
    ULONG numNodes = 0;
    ULONG quickMatchThreshold = 0;
    BOOLEAN quickMatch = FALSE;
    BOOLEAN done = FALSE;
    ULONG *orderedVertices = NULL;
    ULONG *mapped1 = NULL; // mapping of vertices in g1 to vertices in g2
    ULONG *mapped2 = NULL; // mapping of vertices in g2 to vertices in g1


    
    // Compute threshold on mappings tried before changing from optimal
    // search to greedy search
    quickMatchThreshold = utility::MaximumNodes(nv1);
    
    // Order vertices of g1 by degree
    orderedVertices = (ULONG *) malloc(sizeof(ULONG) * nv1);
    if (orderedVertices == NULL)
        utility::OutOfMemoryError("orderedVertices");
    OrderVerticesByDegree( orderedVertices);
//
//    // Allocate arrays holding vertex maps from g1 to g2 and g2 to g1
    mapped1 = (ULONG *) malloc(sizeof(ULONG) * nv1);
    if (mapped1 == NULL)
        utility::OutOfMemoryError("mapped1");
    mapped2 = (ULONG *) malloc(sizeof(ULONG) * nv2);
    if (mapped2 == NULL)
        utility::OutOfMemoryError("mapped2");

    globalQueue = MatchHeap::AllocateMatchHeap(nv1 * nv1);
    localQueue = MatchHeap::AllocateMatchHeap(nv1);
    node.setDepth(0) ;
    node.setCost(0.0) ;
    node.setMapping(NULL) ;
    globalQueue->InsertMatchHeapNode(& node);
    bestNode.setDepth(0) ;
    bestNode.setCost(MAX_DOUBLE) ;
    bestNode.setMapping(NULL);

    while ((! globalQueue->MatchHeapEmpty()) && (! done))
    {
        globalQueue->ExtractMatchHeapNode( & node);
        if (node.getCost() < bestNode.getCost())
        {
            if (node.getDepth() == nv1)
            {   // complete mapping found
                free(bestNode.getMapping());
                bestNode.setCost(node.getCost()) ;
                bestNode.setDepth(node.getDepth()) ;
                bestNode.setMapping(node.getMapping());
                if (! quickMatch)
                    done = TRUE;
            }
            else
            { // expand node's partial mapping
                // compute mappings between graphs' vertices
                for (i = 0; i < nv1; i++)
                    mapped1[i] = VERTEX_UNMAPPED;
                for (i = 0; i < nv2; i++)
                    mapped2[i] = VERTEX_UNMAPPED;
                for (i = 0; i < node.getDepth(); i++)
                {
                    mapped1[node.getMapping()[i].getV1()] = node.getMapping()[i].getV2();
                    if (node.getMapping()[i].getV2() != VERTEX_DELETED)
                        mapped2[node.getMapping()[i].getV2()] = node.getMapping()[i].getV1();
                }
                v1 = orderedVertices[node.getDepth()];
                // first, try mapping v1 to nothing
                newCost = node.getCost() + DELETE_VERTEX_COST;
                if ((newCost <= threshold) && (newCost < bestNode.getCost()))
                {
                    for (i = 0; i < vertices[v1].getNumEdges(); i++)
                    {
                        edge = & edges[vertices[v1].getEdges()[i]];
                        if (v1 == edge->getE().first)
                            v2 = edge->getE().second;
                        else
                            v2 = edge->getE().first;
                        if ((mapped1[v2] != VERTEX_DELETED) || (v2 == v1))
                        {
                            newCost += DELETE_EDGE_WITH_VERTEX_COST;
                        }
                    }
                }
                // if complete mapping, add cost for any unmapped vertices and
                // edges in g2
                if ((newCost <= threshold) && (newCost < bestNode.getCost()))
                {
                    if (node.getDepth() == (nv1 - 1))
                    {
                        cost = g2->InsertedVerticesCost( mapped2);
                        newCost += cost;
                    }
                }
                if ((newCost <= threshold) && (newCost < bestNode.getCost()))
                {
                    // add new node to local queue
                    newNode.setDepth(node.getDepth() + 1) ;
                    newNode.setCost(newCost) ;
                    newNode.setMapping(VertexMap::AllocateNewMapping(node.getDepth() + 1, node.getMapping(), v1, VERTEX_DELETED));
                    
                    localQueue->InsertMatchHeapNode(& newNode);
                }
                // second, try mapping v1 to each unmapped vertex in g2
                for (v2 = 0; v2 < nv2; v2++)
                {
                    if (mapped2[v2] == VERTEX_UNMAPPED)
                    {
                        mapped1[v1] = v2;
                        mapped2[v2] = v1;
                        newCost = node.getCost();
                        newCost += SUBSTITUTE_VERTEX_LABEL_COST *
                        labelList->LabelMatchFactor(vertices[v1].getLabel(),
                                         g2->getVertices()[v2].getLabel());
                        if ((newCost <= threshold) && (newCost < bestNode.getCost()))
                        {
                            cost = DeletedEdgesCost(g2,v1,v2,mapped1,labelList);
                            newCost += cost;
                            cost = g2->InsertedEdgesCost( v2, mapped2);
                            newCost += cost;
                        }
                        // if complete mapping, add cost for any unmapped vertices
                        // and edges in g2
                        if ((newCost <= threshold) && (newCost < bestNode.getCost()))
                        {
                            if (node.getDepth() == (nv1 - 1))
                            {
                                cost = g2->InsertedVerticesCost( mapped2);
                                newCost += cost;
                            }
                        }
                        if ((newCost <= threshold) && (newCost < bestNode.getCost()))
                        {
                            // add new node to local queue
                            newNode.setDepth(node.getDepth() + 1) ;
                            newNode.setCost(newCost) ;
                            newNode.setMapping(VertexMap::AllocateNewMapping(node.getDepth() + 1,node.getMapping(),v1,v2)) ;
                            localQueue->InsertMatchHeapNode(& newNode);
                        }
                        mapped1[v1] = VERTEX_UNMAPPED;
                        mapped2[v2] = VERTEX_UNMAPPED;
                    }
                }
                free(node.getMapping());
                // Add nodes in localQueue to globalQueue
                if (quickMatch)
                {
                    if (! localQueue->MatchHeapEmpty())
                    {
                        localQueue->ExtractMatchHeapNode( & node);
                        globalQueue->InsertMatchHeapNode(& node);
                        localQueue->ClearMatchHeap();
                    }
                }
                else
                    globalQueue->MergeMatchHeaps(localQueue); // clears localQueue
            }
        }
        else
            free(node.getMapping());

        // check if maximum nodes exceeded, and if so, switch to greedy search
        numNodes++;
        if ((! quickMatch) && (numNodes > quickMatchThreshold))
        {
            globalQueue->CompressMatchHeap( nv1);
            quickMatch = TRUE;
        }
    } // end while

    // copy best mapping to input mapping array, if available
    if ((mapping != NULL) && (bestNode.getMapping() != NULL))
        for (i = 0; i < nv1; i++)
        {
            mapping[i].setV1(bestNode.getMapping()[i].getV1()) ;
            mapping[i].setV2(bestNode.getMapping()[i].getV2()) ;
        }

    // free memory
    free(bestNode.getMapping());
    localQueue->FreeMatchHeap();
    globalQueue->FreeMatchHeap();
    free(mapped2);
    free(mapped1);
    free(orderedVertices);
    
    return bestNode.getCost();
}


    //---------------------------------------------------------------------------
    // NAME:    NewEdgeMatch
    //
    // INPUTS:  (Graph *g1) - the graph of the substructure
    //          (instance *inst1) - the substructure instance
    //          (Graph *g2) - the graph of the instance
    //          (instance *inst2) - the instance to be matched
    //          (Parameters *parameters) - system parameters
    //          (double threshold) - the threshold to be used
    //          (double *cost) - cost to be returned
    //
    // RETURN:  (BOOLEAN) - TRUE if instances match on the
    //          newly added edge.
    //
    // PURPOSE: Returns TRUE if inst1 and inst2 match on the new
    //          edge (and vertices) that were added.  Initially, a simple
    //          check is done, then, if it matches, other attributes are
    //          checked to see if it is an exact match.  Finally, if all else
    //          fails, a call to GraphMatch is made.  This is done because it
    //          still could be an exact match if the graphs are just rotated.
    //
    // NOTE:    Currently, this function is only called if the threshold is 0.0.
    //          Future performance improvements will look into how to further reduce
    //          the calls to GraphMatch, which calls InexactGraphMatch, where most
    //          of the SUBDUE work is performed.
    //---------------------------------------------------------------------------
    
    bool LabeledGraph::NewEdgeMatch( Instance *inst1, LabeledGraph *g2,Instance *inst2, Params *parameters,
                         double threshold, double *cost)
    {
        VertexMap *subMapping;
        ULONG newEdge1 = inst1->getNewEdge();
        ULONG newEdge2 = inst2->getNewEdge();
        ULONG newVertex1 = inst1->getNewVertex();
        ULONG newVertex2 = inst2->getNewVertex();
        ULONG i,j;
        ULONG inst1Vertex1 = 0;
        ULONG inst1Vertex2 = 0;
        ULONG inst2Vertex1 = 0;
        ULONG inst2Vertex2 = 0;
        
        LabelList *labelList = parameters->labelList;
		
	

        //
        // First, see if the simple match is valid:
        //    - are the new edge labels the same?
        //    - are the directions of the edges the same?
        //    - are the new vertex labels (if new vertex added) the same?
        //
        if ((edges[newEdge1].getLabel() == g2->getEdges()[newEdge2].getLabel()) &&
            (edges[newEdge1].isDirected() == g2->getEdges()[newEdge2].isDirected()) &&
            (((newVertex1 == VERTEX_UNMAPPED) && (newVertex2 == VERTEX_UNMAPPED)) ||
             ((newVertex1 != VERTEX_UNMAPPED) && (newVertex2 != VERTEX_UNMAPPED) &&
              (vertices[newVertex1].getLabel() == g2->getVertices()[newVertex2].getLabel()))))
        {
		
            //
            // First, map the second instance to the parent of the first instance,
            // and then see if their mappings match.
            // (only if there is a new vertex)
            //
            if (newVertex2 != VERTEX_UNMAPPED)
            {
                inst1->UpdateMapping(inst2);
            }
		
            // get first instance's vertices for mapped index
			inst1Vertex1 = inst1->getMapping()[inst1->getMappingIndex1()].getV1();
			inst1Vertex2 = inst1->getMapping()[inst1->getMappingIndex2()].getV1();
			
            // get second instance's vertices for mapped index
            inst2Vertex1 = inst2->getMapping()[inst2->getMappingIndex1()].getV1();
            inst2Vertex2 = inst2->getMapping()[inst2->getMappingIndex2()].getV1();
            
            //
            // Now, see if the vertex mappings validate that they are the same
            //
            if (((edges[newEdge1].getE().first ==
                  g2->getEdges()[newEdge2].getE().first) &&
                 (edges[newEdge1].getE().second ==
                  g2->getEdges()[newEdge2].getE().second) &&
                 (inst1Vertex1 == inst2Vertex1) &&
                 (inst1Vertex2 == inst2Vertex2)) ||
                ((!g2->getEdges()[newEdge2].isDirected()) &&
                 (edges[newEdge1].getE().first ==
                  g2->edges[newEdge2].getE().first) &&
                 (edges[newEdge1].getE().second ==
                  g2->getEdges()[newEdge2].getE().first) &&
                 (inst1Vertex1 == inst2Vertex2) &&
                 (inst1Vertex2 == inst2Vertex1)))
            {
                *cost = 0.0;
                return TRUE;
            }
        }
        //
        // Last resort... may need to rotate the graph to get a match...
        // Will return mapping for possible later use by the instance.
        //
        // First, create temporary holding place for mapping returned from
        // GraphMatch
        //
        subMapping = (VertexMap *) malloc(sizeof(VertexMap) * inst2->getNumVertices());
        if (subMapping == NULL)
            utility::OutOfMemoryError("NewEdgeMatch: subMapping");
        
        if (GraphMatch( g2, labelList, threshold, NULL, subMapping))
        {
            // Declare some variables that were not needed until now
            ULONG value;
            ULONG index = 0;
            BOOLEAN *mapSet = NULL;
            VertexMap *tempMapping;
            ULONG *sortedMapping;
            //
            // Sort the mapping returned from GraphMatch
            //
            sortedMapping = (ULONG *) malloc(sizeof(ULONG) * inst2->getNumVertices());
            if (sortedMapping == NULL)
                utility::OutOfMemoryError("NewEdgeMatch: sortedMapping");
            for (i = 0; i < inst2->getNumVertices(); i++)
                sortedMapping[subMapping[i].getV1()] = subMapping[i].getV2();
            //
            // Then, temporarily sort the existing instance mapping
            // such that the values (v2) are in increasing order from 1..n
            // (the order returned from GraphMatch assumes that)
            //
            mapSet = (BOOLEAN *) malloc(sizeof(BOOLEAN) * inst2->getNumVertices());
            if (mapSet == NULL)
                utility::OutOfMemoryError("NewEdgeMatch: mapSet");
            
            tempMapping = (VertexMap *) malloc(sizeof(VertexMap) * inst2->getNumVertices());
            if (tempMapping == NULL)
                utility::OutOfMemoryError("NewEdgeMatch: tempMapping");
            
            for (i = 0; i < inst2->getNumVertices(); i++)
                mapSet[i] = FALSE;
            
            for (i = 0; i < inst2->getNumVertices(); i++)
            {
                // find what should be next mapping in the order...
                value = MAX_UNSIGNED_LONG;
                for (j = 0; j < inst2->getNumVertices(); j++)
                {
                    if ((inst2->getMapping()[j].getV2() < value) && (!mapSet[j]))
                    {
                        value = inst2->getMapping()[j].getV2();
                        index = j;
                    }
                }
                mapSet[index] = TRUE;
                tempMapping[i].setV1(i);
                tempMapping[i].setV2(value) ;
            }
          //  free(mapSet);
            //
            for (i=0;i<inst2->getNumVertices();i++)
            {
                inst2->getMapping()[i].setV2(tempMapping[sortedMapping[i]].getV2()) ;
            }
            //
          //  free(tempMapping);
           // free(subMapping);
            //free(sortedMapping);
            return TRUE;
        }
        
        free(subMapping);
        return FALSE;
    }


//---------------------------------------------------------------------------
// NAME: InsertedVerticesCost
//
// INPUTS: (Graph *g2) - graph being transformed to that may contain vertices
//                       not mapped to
//         (ULONG *mapped2) - mapping of vertices from g2 to g1
//
// RETURN: (double) - cost of inserting vertices and edges not mapped in g2
//
// PURPOSE: When all vertices in g1 have been mapped, some of them may have
// been mapped to NODE_DELETED and some of g2's vertices may then be
// unmapped.  This function computes the cost of inserting the necessary
// vertices and edges to transform g1 into g2.  Care is taken not to
// inflate the cost for edges both of whose vertices are unmapped.
//---------------------------------------------------------------------------

double LabeledGraph::InsertedVerticesCost( ULONG *mapped2)
{
    ULONG v;
    ULONG v2;
    ULONG e;
    double cost = 0.0;
    
    for (v = 0; v < numVertices; v++)
        if (mapped2[v] == VERTEX_UNMAPPED)
        {
            cost += INSERT_VERTEX_COST;
            // add cost for edges to mapped vertices and self edges
            for (e = 0; e < vertices[v].getNumEdges(); e++)
            {
                if (v == edges[vertices[v].getEdges()[e]].getE().first)
                    v2 = edges[vertices[v].getEdges()[e]].getE().second;
                else
                    v2 = edges[vertices[v].getEdges()[e]].getE().first;
                if ((mapped2[v2] != VERTEX_UNMAPPED) || (v2 == v))
                    cost += INSERT_EDGE_WITH_VERTEX_COST;
            }
        }
    return cost;
}

//---------------------------------------------------------------------------
// NAME: DeletedEdgesCost
//
// INPUTS: (Graph *g1)
//         (Graph *g2) - graphs containing vertices being mapped
//         (ULONG v1) - vertex in g1 being mapped
//         (ULONG v2) - vertex in g2 being mapped
//         (ULONG *mapped1) - mapping of vertices in g1 to vertices in g2
//         (LabelList *labelList) - label list containing labels for g1 and g2
//
// RETURN: (double) - cost of match edges according to given mapping
//
// PURPOSE: Compute the cost of matching edges involved in the new
// mapping, which has just added v1 -> v2.  In the case of multiple
// edges between two vertices, do a greedy search to find a low-cost
// mapping of edges to edges.
//
// NOTE: Assumes InsertedEdgesCost() run right after this one.
//---------------------------------------------------------------------------

double LabeledGraph::DeletedEdgesCost( LabeledGraph *g2, ULONG v1, ULONG v2,
                        ULONG *mapped1, LabelList *labelList)
{
    ULONG e1, e2;
    LabeledEdge *edge1, *edge2;
    ULONG otherVertex1, otherVertex2;
    LabeledEdge *bestMatchEdge;
    double bestMatchCost;
    double matchCost;
    double totalCost = 0.0;
    
    // try to match each edge involving v1 to an edge involving v2
    for (e1 = 0; e1 < vertices[v1].getNumEdges(); e1++)
    {
        edge1 = & edges[vertices[v1].getEdges()[e1]];
        if (edge1->getE().first == v1)
            otherVertex1 = edge1->getE().second;
        else
            otherVertex1 = edge1->getE().first;
        if ((mapped1[otherVertex1] != VERTEX_UNMAPPED) &&
            (mapped1[otherVertex1] != VERTEX_DELETED))
        {
            // target vertex of edge also mapped
            bestMatchEdge = NULL;
            bestMatchCost = -1.0;
            otherVertex2 = mapped1[otherVertex1];
            for (e2 = 0; e2 < g2->getVertices()[v2].getNumEdges(); e2++)
            {
                edge2 = & g2->getEdges()[g2->getVertices()[v2].getEdges()[e2]];
                if ((! edge2->isUsed()) &&
                    (((edge2->getE().first == otherVertex2) && (edge2->getE().second == v2)) ||
                     ((edge2->getE().first == v2) && (edge2->getE().second == otherVertex2))))
                {
                    // compute cost of matching edge in g1 to edge in g2
                    matchCost = 0.0;
                    if (edge1->isDirected() != edge2->isDirected())
                        matchCost += SUBSTITUTE_EDGE_DIRECTION_COST;
                    if (edge1->isDirected() && edge2->isDirected() &&
                        (edge1->getE().first != edge1->getE().second) &&
                        (((edge1->getE().first == v1) &&
                          (edge2->getE().first  == otherVertex2)) ||
                         ((edge1->getE().first  == otherVertex1) &&
                          (edge2->getE().first  == v2))))
                        matchCost += REVERSE_EDGE_DIRECTION_COST;
                    matchCost += SUBSTITUTE_EDGE_LABEL_COST *
                    labelList->LabelMatchFactor(edge1->getLabel(), edge2->getLabel());
                    if ((matchCost < bestMatchCost) || (bestMatchCost < 0.0))
                    {
                        bestMatchCost = matchCost;
                        bestMatchEdge = edge2;
                    }
                }
            }
            // if matching edge found, then add cost of match and mark edge used;
            // else add cost of deleting edge from g1
            if (bestMatchEdge != NULL)
            {
                bestMatchEdge->setUsed(TRUE) ;
                totalCost += bestMatchCost;
            }
            else
                totalCost += DELETE_EDGE_COST;
        }
    }
    return totalCost;
}


//---------------------------------------------------------------------------
// NAME: InsertedEdgesCost
//
// INPUTS: (Graph *g2) - graph containing vertex being mapped to
//         (ULONG v2) - vertex in g2 being mapped to
//         (ULONG *mapped2) - array mapping vertices of g2 to vertices of g1
//
// RETURN: (double) - cost of inserting edges found in g2 between v2
//                    and another mapped vertex, but not matched to
//                    edges in g1
//
// PURPOSE: Compute the cost of adding edges to the transformation
// from g1 to g2 for edges in g2 that are between v2 and another mapped
// vertex, but are not matched to edges in g1 by DeleteEdgesCost().
//
// NOTE: Assumes DeletedEdgesCost() run before this one.
//---------------------------------------------------------------------------

double LabeledGraph::InsertedEdgesCost(ULONG v2, ULONG *mapped2)
{
    ULONG e2;
    LabeledEdge *edge2;
    double totalCost = 0.0;
    
    for (e2 = 0; e2 < vertices[v2].getNumEdges(); e2++)
    {
        edge2 = & edges[vertices[v2].getEdges()[e2]];
        
        if ((! edge2->isUsed()) &&
            (mapped2[edge2->getE().first] != VERTEX_UNMAPPED) &&
            (mapped2[edge2->getE().second] != VERTEX_UNMAPPED))
            totalCost += INSERT_EDGE_COST;
        edge2->setUsed(FALSE) ;
    }
    return totalCost;
}


//---------------------------------------------------------------------------
// NAME: OrderVerticesByDegree
//
// INPUTS: (Graph *g) - graph whose vertices are to be sorted by degree
//         (ULONG *orderedVertices) - array to hold vertex indices
//                                    sorted by degree
//
// RETURN: (void)
//
// PURPOSE: Compute an array of vertex indices ordered by degree.
// Several studies have shown that matching higher degree vertices
// first will speed up the match.
//---------------------------------------------------------------------------

void LabeledGraph::OrderVerticesByDegree( ULONG *orderedVertices)
{
    ULONG nv = numVertices;
    ULONG i, j;
    ULONG degree;
    ULONG *vertexDegree = NULL;
    
    vertexDegree = (ULONG *) malloc(sizeof(ULONG) * nv);
    if (vertexDegree == NULL)
        utility::OutOfMemoryError("vertexDegree");
    
    // insertion sort vertices by degree
    for (i = 0; i < nv; i++)
    {
        degree = vertices[i].getNumEdges();
        j = i;
        while ((j > 0) && (degree > vertexDegree[j-1]))
        {
            vertexDegree[j] = vertexDegree[j-1];
            orderedVertices[j] = orderedVertices[j-1];
            j--;
        }
        vertexDegree[j] = degree;
        orderedVertices[j] = i;
    }
    free(vertexDegree);
}

//---------------------------------------------------------------------------
// NAME:    ReadVertex
//
// INPUTS:  (Graph *graph) - pointer to graph being constructed
//          (FILE *fp) - pointer to graph file stream
//          (LabelList *labelList) - list of vertex and edge labels
//          (ULONG *pLineNo) - pointer to line counter in calling function
//          (ULONG vertexOffset) - offset to add to vertex numbers
//
// RETURN:  (void)
//
// PURPOSE: Read and check the vertex number and label, store label in
// given label list, and add vertex to graph.  A non-zero vertexOffset
// indicates this vertex is part of a graph beyond the first.
//--------------------------------------------------------------------------

void LabeledGraph::ReadVertex(FILE *fp, LabelList *labelList, ULONG *pLineNo, ULONG vertexOffset)
{
    ULONG vertexID;
    ULONG labelIndex;
    
    // read and check vertex number
    vertexID = utility::ReadInteger(fp, pLineNo) + vertexOffset;

    if (vertexID != (numVertices + 1))
    {
        fprintf(stderr, "Error: invalid vertex number at line %lu.\n",
                *pLineNo);
        exit(1);
    }
    // read label
    labelIndex = labelList->ReadLabel(fp, pLineNo);
    
    AddVertex( labelIndex);
}


//---------------------------------------------------------------------------
// NAME: AddVertex
//
// INPUTS: (Graph *graph) - graph to add vertex
//         (ULONG labelIndex) - index into label list of vertex's label
//
// RETURN: (void)
//
// PURPOSE: Add vertex information to graph. AddVertex also changes the
// size of the currently-allocated vertex array, which increases by
// LIST_SIZE_INC (instead of just 1) when exceeded.
//---------------------------------------------------------------------------

void LabeledGraph::AddVertex( ULONG labelIndex)
{
    LabeledVertex *newVertexList;
    ULONG numVertices = this->numVertices;
    ULONG vertexListSize = this->vertexListSize;
    
    // make sure there is enough room for another vertex
    if (vertexListSize == numVertices)
    {
        vertexListSize += LIST_SIZE_INC;
        newVertexList = (LabeledVertex *) realloc(this->vertices, (sizeof(LabeledVertex) * (vertexListSize)));
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
    this->numVertices++;
}


//---------------------------------------------------------------------------
// NAME:    ReadEdge
//
// INPUTS:  (Graph *graph) - pointer to graph being constructed
//          (FILE *fp) - pointer to graph file stream
//          (LabelList *labelList) - list of vertex and edge labels
//          (ULONG *pLineNo) - pointer to line counter in calling function
//          (BOOLEAN directed) - TRUE if edge is directed
//          (ULONG vertexOffset) - offset to add to vertex numbers
//
// RETURN:  (void)
//
// PURPOSE: Read and check the vertex numbers and label, store label in
// given label list, and add edge to graph.  A non-zero vertexOffset
// indicates the edge's vertices are part of a graph beyond the first.
//---------------------------------------------------------------------------

void LabeledGraph::ReadEdge( FILE *fp, LabelList *labelList, ULONG *pLineNo, BOOLEAN directed, ULONG vertexOffset)
{
    ULONG sourceVertexID;
    ULONG targetVertexID;
    ULONG sourceVertexIndex;
    ULONG targetVertexIndex;
    ULONG labelIndex;
    
    // read and check vertex numbers
    sourceVertexID = utility::ReadInteger(fp, pLineNo) + vertexOffset;
    if ((sourceVertexID < 1) || (sourceVertexID > numVertices))
    {
        fprintf(stderr,
                "Error: reference to undefined vertex number at line %lu.\n",
                *pLineNo);
        exit(1);
    }
    targetVertexID = utility::ReadInteger(fp, pLineNo) + vertexOffset;
    if ((targetVertexID < 1) || (targetVertexID > numVertices))
    {
        fprintf(stderr,
                "Error: reference to undefined vertex number at line %lu.\n",
                *pLineNo);
        exit(1);
    }
    sourceVertexIndex = sourceVertexID - 1;
    targetVertexIndex = targetVertexID - 1;
    
    // read and store label
    labelIndex = labelList->ReadLabel(fp, pLineNo);
    
    AddEdge( sourceVertexIndex, targetVertexIndex, directed, labelIndex, FALSE);
}


//--------------------------------------------------------------
// NAME:  ReadIncrementVertex
//
// INPUTS: (Graph *graph)
//         (FILE *fp)
//         (LabelList *labelList)
//         (ULONG *pLineNo)
//         (ULONG vertexOffset)
//
// RETURN:  void
//
// PURPOSE:  Read information for a single vertex in the current
//           increment file.
//--------------------------------------------------------------

void LabeledGraph::ReadIncrementVertex( FILE *fp, LabelList *labelList,
                         ULONG *pLineNo, ULONG vertexOffset,Params *parameters)
{
    ULONG vertexID;
    ULONG labelIndex;
    
    // read and check vertex number
    vertexID = utility::ReadInteger(fp, pLineNo) + vertexOffset;
    if (vertexID != (numVertices + 1))
    {
        fprintf(stderr, "Error: invalid vertex number at line %lu.\n", *pLineNo);
        exit(1);
    }
    // read label
    labelIndex = labelList->ReadLabel(fp, pLineNo);
    
    AddVertex( labelIndex);
    if (parameters->readingPositive)
        parameters->numIncrementPosVertices++;
    else
        parameters->numIncrementNegVertices++;
}


//--------------------------------------------------------------
// NAME:  ReadIncrementEdge
//
// INPUTS: (Graph *graph)
//         (FILE *fp)
//         (LabelList *labelList)
//         (ULONG *pLineNo)
//         (BOOLEAN directed)
//         (ULONG startVertexIndex)
//         (ULONG vertexOffset)
//
// RETURN:  void
//
// PURPOSE:  Read information for a single edge in the current
//           increment file.
//--------------------------------------------------------------

void LabeledGraph::ReadIncrementEdge( FILE *fp, LabelList *labelList,
                       ULONG *pLineNo, BOOLEAN directed,
                       ULONG startVertexIndex, ULONG vertexOffset,Params *parameters)
{
    ULONG sourceVertexID;
    ULONG targetVertexID;
    ULONG sourceVertexIndex;
    ULONG targetVertexIndex;
    ULONG labelIndex;
    BOOLEAN spansIncrement;
    
    // read and check vertex numbers
    sourceVertexID = utility::ReadInteger(fp, pLineNo) + vertexOffset;
    if (sourceVertexID >  numVertices)
    {
        fprintf(stderr,
                "Error: reference to undefined vertex number at line %lu.\n", *pLineNo);
        exit(1);
    }
    targetVertexID = utility::ReadInteger(fp, pLineNo) + vertexOffset;
    if (targetVertexID > numVertices)
    {
        fprintf(stderr,
                "Error: reference to undefined vertex number at line %lu.\n", *pLineNo);
        exit(1);
    }
    sourceVertexIndex = sourceVertexID - 1;
    targetVertexIndex = targetVertexID - 1;
    
    // read and store label
    labelIndex = labelList->ReadLabel(fp, pLineNo);
    
    if ((sourceVertexIndex < startVertexIndex) ||
        (targetVertexIndex < startVertexIndex))
        spansIncrement = TRUE;
    else
        spansIncrement = FALSE;
    
    AddEdge( sourceVertexIndex, targetVertexIndex, directed, labelIndex, spansIncrement);
    
    if (parameters->readingPositive)
        parameters->numIncrementPosEdges++;
    else
        parameters->numIncrementNegEdges++;
}


//---------------------------------------------------------------------------
// NAME: AddEdgeToVertices
//
// INPUTS: (Graph *graph) - graph containing edge and vertices
//         (ULONG edgeIndex) - edge's index into graph edge array
//
// RETURN: (void)
//
// PURPOSE: Add edge index to the edge array of each of the two
// vertices involved in the edge.  If a self-edge, then only add once.
//---------------------------------------------------------------------------

void LabeledGraph::AddEdgeToVertices( ULONG edgeIndex)
{
    ULONG v1, v2;
    LabeledVertex *vertex;
    ULONG *edgeIndices;
    
    v1 = edges[edgeIndex].getE().first;
    v2 = edges[edgeIndex].getE().second;
    vertex = & vertices[v1];
    edgeIndices = (ULONG *) realloc(vertex->getEdges(),
                                    sizeof(ULONG) * (vertex->getNumEdges() + 1));
    if (edgeIndices == NULL)
        utility::OutOfMemoryError("AddEdgeToVertices:edgeIndices1");
    edgeIndices[vertex->getNumEdges()] = edgeIndex;
    vertex->setEdges(edgeIndices) ;

    vertex->setNumEdges(vertex->getNumEdges()+1);
    
    if (v1 != v2)
    { // don't add a self edge twice
        vertex = & vertices[v2];
        edgeIndices = (ULONG *) realloc(vertex->getEdges(),
                                        sizeof(ULONG) * (vertex->getNumEdges() + 1));
        if (edgeIndices == NULL)
            utility::OutOfMemoryError("AddEdgeToVertices:edgeIndices2");
        edgeIndices[vertex->getNumEdges()] = edgeIndex;
        vertex->setEdges(edgeIndices) ;
        vertex->setNumEdges(vertex->getNumEdges()+1);
    }

}

//---------------------------------------------------------------------------
// NAME: FindInstances
//
// INPUTS: (Graph *g1) - graph to search for
//         (Graph *g2) - graph to search in
//         (Parameters *parameters)
//
// RETURN: (InstanceList *) - list of instances of g1 in g2, may be empty
//
// PURPOSE: Searches for subgraphs of g2 that match g1 and returns the
// list of such subgraphs as instances in g2.  Returns empty list if
// no matches exist.  This procedure mimics the DiscoverSubs loop by
// repeatedly expanding instances of subgraphs of g1 in g2 until
// matches are found.  The procedure is optimized toward g1 being a
// small graph and g2 being a large graph.
//
// Note: This procedure is equivalent to the NP-Hard subgraph
// isomorphism problem, and therefore can be quite slow for some
// graphs.
//---------------------------------------------------------------------------

InstanceList *LabeledGraph::FindInstances(LabeledGraph *g1, Params *parameters)
{
    ULONG v;
    ULONG v1;
    ULONG e1;
    LabeledVertex *vertex1;
    LabeledEdge *edge1;
    InstanceList *instanceList;
    BOOLEAN *reached;
    BOOLEAN noMatches;
    BOOLEAN found;
    
    reached = (BOOLEAN *) malloc(sizeof(BOOLEAN) * g1->getNumVertices());
    if (reached == NULL)
        utility::OutOfMemoryError("FindInstances:reached");
    for (v1 = 0; v1 < g1->numVertices; v1++)
        reached[v1] = FALSE;
    v1 = 0; // first vertex in g1
    reached[v1] = TRUE;
    vertex1 = & g1->getVertices()[v1];
    instanceList = FindSingleVertexInstances( vertex1, parameters);
    noMatches = FALSE;
    if (instanceList->getHead() == NULL) // no matches to vertex1 found in g2
        noMatches = TRUE;
    while ((vertex1 != NULL) && (! noMatches))
    {
        vertex1->setUsed(TRUE) ;
        // extend by each unmarked edge involving vertex v1
        for (e1 = 0; ((e1 < vertex1->getNumEdges()) && (! noMatches)); e1++)
        {
            edge1 = & g1->getEdges()[g1->getVertices()[v1].getEdges()[e1]];
            if (! edge1->isUsed())
            {
                reached[edge1->getE().first] = TRUE;
                reached[edge1->getE().second] = TRUE;
                instanceList =
                instanceList->ExtendInstancesByEdge( g1, edge1,this , parameters);
                if (instanceList->getHead() == NULL)
                    noMatches = TRUE;
                edge1->setUsed(TRUE) ;
            }
        }
        // find next un-used, reached vertex
        vertex1 = NULL;
        found = FALSE;
        for (v = 0; ((v < g1->getNumVertices()) && (! found)); v++)
        {
            if ((! g1->getVertices()[v].isUsed()) && (reached[v]))
            {
                v1 = v;
                vertex1 = & g1->getVertices()[v1];
                found = TRUE;
            }
        }
    }
    free(reached);
    
    // set used flags to FALSE
    for (v1 = 0; v1 < g1->getNumVertices(); v1++)
        g1->getVertices()[v1].setUsed(FALSE);
    for (e1 = 0; e1 < g1->getNumEdges(); e1++)
        g1->getEdges()[e1].setUsed(FALSE);
    
    // filter instances not matching g1
    // filter overlapping instances if appropriate
    instanceList = instanceList->FilterInstances(g1, this, parameters);
    
    return instanceList;
}


//---------------------------------------------------------------------------
// NAME: FindSingleVertexInstances
//
// INPUTS: (Graph *graph) - graph to look in for matching vertices
//         (Vertex *vertex) - vertex to look for
//         (Parameters *parameters)
//
// RETURN: (InstanceList *) - list of single-vertex instances matching
//                            vertex
//
// PURPOSE: Return a (possibly-empty) list of single-vertex instances, one
// for each vertex in the given graph that matches the given vertex.
//---------------------------------------------------------------------------

InstanceList *LabeledGraph::FindSingleVertexInstances( LabeledVertex *vertex, Params *parameters)
{
    ULONG v;
    InstanceList *instanceList;
    Instance *instance;
    
    instanceList = InstanceList::AllocateInstanceList();
    for (v = 0; v < numVertices; v++)
    {
        if (vertices[v].getLabel() == vertex->getLabel())
        {
            // ***** do inexact label matches here? (instance->minMatchCost too)
            instance = Instance::AllocateInstance(1, 0);
            instance->getVertices()[0] = v;
            instance->setMinMatchCost(0.0) ;
            instanceList->InstanceListInsert(instance, FALSE);
        }
    }
    return instanceList;
}

//---------------------------------------------------------------------------
// NAME: MarkGraphEdgesUsed
//
// INPUT: (ReferenceGraph *refGraph) - reference graph
//        (Graph *fullGraph) - complete input graph
//        (BOOLEAN value) - true or false value for the edge
//
// RETURN:  void
//
// PURPOSE:  Find each edge in the full graph that is present in the ref graph,
//           mark the "used" flag as true or false.
//---------------------------------------------------------------------------

void LabeledGraph::MarkGraphEdgesUsed(ReferenceGraph *refGraph,
                        BOOLEAN value)
{
    ULONG e;
    
    for (e = 0; e < refGraph->getNumEdges(); e++)
        edges[refGraph->getEdges()[e].getMap()].setUsed(value) ;
}

//---------------------------------------------------------------------------
// NAME: MarkGraphEdgesValid
//
// INPUT: (ReferenceGraph *refGraph) - the reference graph
//        (Graph *fullGraph) - the complete graph
//        (BOOLEAN mark) - if true then mark the full graph;
//                         if false then reset the full graph to true.
//
// RETURN:  void
//
// PURPOSE:  Mark the edges in fullGraph as valid/invalid based on their state
//           in the ref graph.  If the BOOLEAN parm "mark" is true then we are
//           marking the fullGraph edges based on the refGraph edges.
//           If false, then we are resetting the fullGraph edges to TRUE.
//---------------------------------------------------------------------------

void LabeledGraph::MarkGraphEdgesValid(ReferenceGraph *refGraph,
                                       BOOLEAN mark)
{
    ULONG e;
    
    for (e = 0; e < refGraph->getNumEdges(); e++)
    {
        if (mark)
        {
            if (refGraph->getEdges()[e].isFailed())
                edges[refGraph->getEdges()[e].getMap()].setValidPath(FALSE) ;
            else
                edges[refGraph->getEdges()[e].getMap()].setValidPath(TRUE) ;
        }
        else // unmark
            edges[refGraph->getEdges()[e].getMap()].setValidPath(TRUE) ;
    }
}


//---------------------------------------------------------------------------
// NAME: PositiveExample
//
// INPUTS: (Graph *graph) - graph to test if positive
//         (Graph **subGraphs) - disjunctive concept of a positive example
//         (ULONG numSubGraph) - number of subgraphs
//         (Parameters *parameters)
//
// RETURN: (BOOLEAN) - TRUE is example graph is positive
//
// PURPOSE:
//---------------------------------------------------------------------------

BOOLEAN LabeledGraph::PositiveExample( LabeledGraph **subGraphs,
                        ULONG numSubGraphs, Params *parameters)
{
    ULONG i = 0;
    BOOLEAN found = FALSE;
    InstanceList *instanceList = NULL;
    
    while ((i < numSubGraphs) && (! found))
    {
        instanceList = FindInstances(subGraphs[i], parameters);
        if (instanceList->getHead() != NULL)
            found = TRUE;
        instanceList->FreeInstanceList();
        i++;
    }
    return found;
}

//------------------------------------------------------------------------------
// NAME: VertexInSub
//
// INPUT: (Graph *subDef) - the sub against which we are comparing the vertex
//        (Vertex *vertex) - vertex from the full graph that we are comparing
//                           to the sub's vertices
//
// RETURN:  BOOLEAN - true if the vertex is consistent with the sub
//
// PURPOSE:  This is designed to compare the vertices in a substructure's graph
//           definition to a vertex from a full graph
//------------------------------------------------------------------------------

BOOLEAN LabeledGraph::VertexInSub( LabeledVertex *vertex)
{
    LabeledVertex *vertices;
    ULONG i;
    
    vertices = this->vertices;
    
    // Look at each vertex index in the instance and see if it matches our vertex
    // if so then return TRUE
    for (i=0;i<this->numVertices;i++)
    {
        // If label and degree are the same, we have a match.
        // The degree of the sub's vertex must be <= to the degree of the
        // graph's vertex.
        if ((vertices[i].getLabel() == vertex->getLabel()) &&
            (vertices[i].getNumEdges() <= vertex->getNumEdges()))
            return TRUE;
    }
    
    return FALSE;
}


//---------------------------------------------------------------------------
// NAME: CheckForMatch
//
// INPUTS:  (Graph *subGraph)
//          (InstanceList *instanceList)
//          (Graph *graph)
//          (Parameters *parameters)
//
// RETURN:  BOOLEAN
//
// PURPOSE:  A version of the function found in the sgiso code.
// Called by CheckForSubgraph.
//---------------------------------------------------------------------------

BOOLEAN LabeledGraph::CheckForMatch(InstanceList *instanceList, LabeledGraph *graph,
                      Params *parameters)
{
    InstanceListNode *instanceListNode;
    Instance *instance;
    LabeledGraph *instanceGraph;
    BOOLEAN foundMatch = FALSE;
    double matchCost;
    int i = 0;
    
    if (instanceList != NULL)
    {
        instanceListNode = (InstanceListNode *)instanceList->getHead();
        while (instanceListNode != NULL)
        {
            i++;
            if (instanceListNode->getInstance() != NULL)
            {
                instance = instanceListNode->getInstance();
                instanceGraph = instance->InstanceToGraph ( graph);
                if (GraphMatch ( instanceGraph, parameters->labelList,
                                0.0, &matchCost, NULL))
                    foundMatch = TRUE;
                instanceGraph->FreeGraph ();
                if(foundMatch)
                    break;
            }
            instanceListNode = (InstanceListNode *)instanceListNode->getNext();
        }
    }
    return foundMatch;
}

//---------------------------------------------------------------------------
// NAME: CheckForSubgraph
//
// INPUT: (Graph *g1) - graph 1
//        (Graph *g2) - graph 2
//        (Parameters *parameters) - System parms
//
// RETURN:  BOOLEAN
//
// PURPOSE:  This is a version of the sgiso that returns TRUE as soon as the
//           first subgraph is found instead of trying to find all subgraphs.
//           That is all we care about in the boundary evaluation.
//---------------------------------------------------------------------------

BOOLEAN LabeledGraph::CheckForSubgraph( LabeledGraph *g2, Params *parameters)
{
    ULONG v;
    ULONG v1;
    ULONG e1;
    LabeledVertex *vertex1;
    LabeledEdge *edge1;
    InstanceList *instanceList;
    BOOLEAN *reached;
    BOOLEAN noMatches;
    BOOLEAN found;
    BOOLEAN foundMatch = FALSE;
    
    reached = (BOOLEAN *) malloc (sizeof (BOOLEAN) * numVertices);
    if (reached == NULL)
        utility::OutOfMemoryError ("FindInstances:reached");
    for (v1 = 0; v1 < numVertices; v1++)
        reached[v1] = FALSE;
    
    v1 = 0; // first vertex in g1
    reached[v1] = TRUE;
    vertex1 = &vertices[v1];
    instanceList = g2->FindSingleVertexInstances ( vertex1, parameters);
    noMatches = FALSE;
    if (instanceList->getHead() == NULL) // no matches to vertex1 found in g2
        noMatches = TRUE;
    
    while ((vertex1 != NULL) && (! noMatches) && (!foundMatch))
    {
        vertex1->setUsed(TRUE) ;
        // extend by each unmarked edge involving vertex v1
        for (e1 = 0; ((e1 < vertex1->getNumEdges()) && (! noMatches) && (!foundMatch));
             e1++)
        {
            edge1 = &edges[vertices[v1].getEdges()[e1]];
            if (! edge1->isUsed())
            {
                reached[edge1->getE().first] = TRUE;
                reached[edge1->getE().second] = TRUE;
                instanceList =
                instanceList->ExtendInstancesByEdge( this, edge1, g2, parameters);
                
                // We have a set of instances from g2.  If one of them matches g1
                // then we are done.
                foundMatch = CheckForMatch( instanceList, g2, parameters);
                if (instanceList->getHead() == NULL)
                    noMatches = TRUE;
                edge1->setUsed(TRUE) ;
            }
        }
        if (foundMatch)
            break;
        
        // Find next un-used, reached vertex
        vertex1 = NULL;
        found = FALSE;
        for (v = 0; ((v < numVertices) && (! found)); v++)
        {
            if ((! vertices[v].isUsed()) && (reached[v]))
            {
                v1 = v;
                vertex1 = & vertices[v1];
                found = TRUE;
            }
        }
    }
    free (reached);
    
    // set used flags to FALSE
    for (v1 = 0; v1 < numVertices; v1++)
        vertices[v1].setUsed(FALSE);
    for (e1 = 0; e1 < numEdges; e1++)
        edges[e1].setUsed(FALSE);
    
    return foundMatch;
}


//---------------------------------------------------------------------------
// NAME: MarkExample
//
// INPUTS: (ULONG egStartVertexIndex) - starting vertex of example
//         (ULONG egEndVertexIndex) - ending vertex of example
//         (Graph *graph) - graph containing example
//         (BOOLEAN value) - value for used flag of example vertices/edges
//
// RETURN: (void)
//
// PURPOSE: Sets the used flag to value for all vertices and edges
// comprising the example whose range of vertices is given.
//---------------------------------------------------------------------------

void LabeledGraph::MarkExample(ULONG egStartVertexIndex, ULONG egEndVertexIndex,BOOLEAN value)
{
    ULONG v;
    ULONG e;
    LabeledVertex *vertex;
    
    for (v = egStartVertexIndex; v <= egEndVertexIndex; v++)
    {
        vertex = & vertices[v];
        vertex->setUsed(value) ;
        for (e = 0; e < vertex->getNumEdges(); e++)
            edges[vertex->getEdges()[e]].setUsed(value) ;
    }
}
























































