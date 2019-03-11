//
// Created by Macbook on 10/02/2019.
//

#ifndef SUBDUE_REFERENCEGRAPH_HPP
#define SUBDUE_REFERENCEGRAPH_HPP

#include "ReferenceEdge.hpp"
#include "ReferenceVertex.hpp"
#include "utility.hpp"

#include <iostream>
using namespace std;


class Instance;
    class ReferenceGraph {
    private:
        unsigned long  numVertices; // number of vertices in graph
        unsigned long  numEdges;    // number of edges in graph
        ReferenceVertex *vertices;   // array of graph vertices
        ReferenceEdge   *edges;      // array of graph edges
        unsigned long vertexListSize; // memory allocated to vertices array
        unsigned long edgeListSize;   // memory allocated to edges array
        
    public:
        unsigned long getNumVertices() {
            return numVertices;
        };
        
        void setNumVertices(unsigned long numVertices){
            this->numVertices =numVertices;
        };
        
        unsigned long getNumEdges() {
            return numEdges;
        };
        
        void setNumEdges(unsigned long numEdges){
            this->numEdges = numEdges;
        };
        
        ReferenceVertex *getVertices() {
            return vertices;
        };
        
        void setVertices(ReferenceVertex *vertices){
            this->vertices = vertices;
        };
        
        ReferenceEdge *getEdges() {
            return edges;
        };
        
        void setEdges(ReferenceEdge *edges){
            this->edges = edges;
        };
        
        unsigned long getVertexListSize() {
            return vertexListSize;
        }
        
        void setVertexListSize(unsigned long vertexListSize){
            this->vertexListSize = vertexListSize;
        };
        
        unsigned long getEdgeListSize() {
            return edgeListSize ;
        };
        
        void setEdgeListSize(unsigned long edgeListSize){
            this->edgeListSize = edgeListSize;
        };
        
        void MarkRefGraphInstanceEdges(Instance *instance,
                                                       bool value);
        
        void AddReferenceVertex ( ULONG labelIndex);
        
        void AddReferenceEdge(  ULONG sourceVertexIndex,
                                              ULONG targetVertexIndex, BOOLEAN directed,
                                              ULONG labelIndex, BOOLEAN spansIncrement);
        void AddRefEdgeToRefVertices(ULONG edgeIndex);
        void MarkVertices( ULONG v1, ULONG v2);
        ReferenceGraph *ExtendRefGraph( Substructure *bestSub,
                                                       LabeledGraph *fullGraph, Params *parameters);
        
        
        //-----------------------------------------------------------------------------
        // NAME: AllocateReferenceGraph
        //
        // INPUT: (ULONG v) - number of vertices
        //        (ULONG e) - number of edges
        //
        // RETURN: (ReferenceGraph *) -the new ref graph
        //
        // PURPOSE:  Create a new reference graph.
        //-----------------------------------------------------------------------------
        
        static ReferenceGraph *AllocateReferenceGraph (ULONG v, ULONG e)
        {
            ReferenceGraph *graph;
            graph = (ReferenceGraph *) malloc (sizeof (ReferenceGraph));
            
            if (graph == NULL)
                utility::OutOfMemoryError("AllocateReferenceGraph:graph");
            graph->setNumVertices(v) ;
            graph->setNumEdges(e);
            graph->setVertices(NULL) ;
            graph->setEdges(NULL) ;
            if (v > 0)
            {
                graph->setVertices((ReferenceVertex *) malloc (sizeof (ReferenceVertex) * v));
                
                if (graph->getVertices() == NULL)
                    utility::OutOfMemoryError ("AllocateReferenceGraph:graph->vertices");
            }
            graph->setVertexListSize(v) ;
            if (e > 0)
            {
                graph->setEdges((ReferenceEdge *) malloc (sizeof (ReferenceEdge) * e)) ;
                if (graph->getEdges() == NULL)
                    utility::OutOfMemoryError ("AllocateReferenceGraph:graph->edges");
            }
            graph->setEdgeListSize(e) ;
            return graph;
        }
        

        
        //-----------------------------------------------------------------------------
        // NAME: FreeReferenceGraph
        //
        // INPUT:  (ReferenceGraph *graph) - the ref graph
        //
        // RETURN:  void
        //
        // PURPOSE:  Free the ref graph.
        //-----------------------------------------------------------------------------
        
        void FreeReferenceGraph ()
        {
            ULONG v;
                for (v = 0; v < numVertices; v++)
                    free (vertices[v].getEdges());
                free (edges);
                free (vertices);
    
        }
        
        ReferenceGraph *CopyReferenceGraph();
        
        
        
        
        
        
    };





#endif //SUBDUE_REFERENCEGRAPH_HPP
