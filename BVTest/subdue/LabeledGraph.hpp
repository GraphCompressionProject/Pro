//
// Created by Macbook on 10/02/2019.
//

#ifndef SUBDUE_LABELEDGRAPH_HPP
#define SUBDUE_LABELEDGRAPH_HPP


#include "LabeledEdge.hpp"
#include "LabeledVertex.hpp"
#include "LabelList.hpp"
#include "utility.hpp"
#include "Params.hpp"
using namespace std;

class InstanceList;
class Instance;
class VertexMap;
class ReferenceGraph;

    class LabeledGraph {
    private:
        unsigned long  numVertices; // number of vertices in graph
        unsigned long numEdges;    // number of edges in graph
        LabeledVertex *vertices;   // array of graph vertices
        LabeledEdge  *edges;      // array of graph edges
        unsigned long vertexListSize; // allocated size of vertices array
        unsigned long  edgeListSize;     // allocated size of edges array
    public:
        //----------Getters and Setter ----///
        unsigned long getNumVertices() {
            return numVertices;
        }
        void setNumVertices(unsigned long numVertices){
            LabeledGraph::numVertices =numVertices;
        }
        unsigned long getNumEdges() {
            return numEdges;
        }
        void setNumEdges(unsigned long numEdges){
            LabeledGraph::numEdges = numEdges ;
        }
        LabeledVertex *getVertices() {
            return vertices;
        }
        void setVertices(LabeledVertex *vertices){
            LabeledGraph::vertices =vertices;
        }
        LabeledEdge *getEdges() {
            return edges;
        }
        void setEdges(LabeledEdge *edges){
            LabeledGraph::edges = edges;
        }
        unsigned long getVertexListSize() {
            return vertexListSize ;
        }
        void setVertexListSize(unsigned long vertexListSize){
            LabeledGraph::vertexListSize =vertexListSize;
        }
        unsigned long getEdgeListSize() {
            return edgeListSize;
        }
        void setEdgeListSize(unsigned long edgeListSize){
            LabeledGraph::edgeListSize = edgeListSize;
        }
        /*********************************************
         * Functions tho manipulate a labeled Graph
         ********************************************/
         void PrintGraph(LabelList *labelList);
         void PrintVertex( unsigned long vertexIndex, unsigned long vertexOffset, LabelList *labelList);
         void PrintEdge(unsigned long edgeIndex, unsigned long vertexOffset, LabelList *labelList);
        
        
        void MarkExample(ULONG egStartVertexIndex, ULONG egEndVertexIndex,BOOLEAN value);
        void FreeGraph();
        LabeledGraph *CopyGraph();
        void AddEdge(ULONG sourceVertexIndex, ULONG targetVertexIndex,BOOLEAN directed, ULONG labelIndex, BOOLEAN spansIncrement);
        void AddEdgeToVertices( ULONG edgeIndex);
        double MDL( ULONG numLabels, Params *parameters);
        ULONG NumUniqueEdges(ULONG v1);
        ULONG MaxEdgesToSingleVertex( ULONG v1);
        void CopyUnmarkedGraph( LabeledGraph *g2, ULONG vertexIndex,Params *parameters);
      
        void AddOverlapEdges(LabeledGraph *compressedGraph,
            InstanceList *instanceList, ULONG overlapLabelIndex,
            ULONG startVertex, ULONG startEdge);
        
        void MarkInstanceVertices(Instance *instance,  BOOLEAN value);
        void MarkInstanceEdges(Instance *instance,  BOOLEAN value);
        double ExternalEdgeBits( LabeledGraph *subGraph,
                                              ULONG numInstances);
        ULONG GraphSize();
        
        ULONG SizeOfCompressedGraph( InstanceList *instanceList,Params *parameters, ULONG graphType);
        ULONG NumOverlapEdges( InstanceList *instanceList);
        BOOLEAN GraphMatch( LabeledGraph *g2, LabelList *labelList,
                           double threshold, double *matchCost, VertexMap *mapping);
        
        double InexactGraphMatch(LabeledGraph *g2, LabelList *labelList,double threshold, VertexMap *mapping);
        
        bool NewEdgeMatch( Instance *inst1, LabeledGraph *g2,
                                        Instance *inst2, Params *parameters,
                          double threshold, double *cost);
        double InsertedVerticesCost( ULONG *mapped2);
        double InsertedEdgesCost(ULONG v2, ULONG *mapped2);
        double DeletedEdgesCost( LabeledGraph *g2, ULONG v1, ULONG v2, ULONG *mapped1, LabelList *labelList);
        void OrderVerticesByDegree( ULONG *orderedVertices);
        
        void ReadVertex(FILE *fp, LabelList *labelList, ULONG *pLineNo, ULONG vertexOffset);
        
        void AddVertex( ULONG labelIndex);
        
        void ReadEdge( FILE *fp, LabelList *labelList, ULONG *pLineNo, BOOLEAN directed, ULONG vertexOffset);
        
        
        void  ReadIncrementEdge( FILE *fp, LabelList *labelList, ULONG *pLineNo, BOOLEAN directed,
            ULONG startVertexIndex, ULONG vertexOffset,Params *parameters);
        
        void ReadIncrementVertex( FILE *fp, LabelList *labelList,
                                               ULONG *pLineNo, ULONG vertexOffset,Params *parameters);
        
        InstanceList *FindInstances(LabeledGraph *g1, Params *parameters);
        
        InstanceList *FindSingleVertexInstances( LabeledVertex *vertex,Params *parameters);
        
        BOOLEAN PositiveExample( LabeledGraph **subGraphs,
                                              ULONG numSubGraphs, Params *parameters);
        
        void MarkGraphEdgesUsed(ReferenceGraph *refGraph, BOOLEAN value);
        
        void MarkGraphEdgesValid(ReferenceGraph *refGraph, BOOLEAN mark);
        BOOLEAN VertexInSub( LabeledVertex *vertex);
        BOOLEAN CheckForMatch(InstanceList *instanceList, LabeledGraph *graph,
                                            Params *parameters);
        BOOLEAN CheckForSubgraph( LabeledGraph *g2, Params *parameters);
        
        //---------------------------------------------------------------------------
        // NAME: EdgesMatch
        //
        // INPUTS: (Graph *g1) - graph containing edge e1
        //         (Edge *edge1) - edge from g1 to be matched
        //         (Graph *g2) - graph containing edge e2
        //         (Edge *edge2) - edge from g2 being matched
        //         (Parameters *paramters)
        //
        // RETURN: (BOOLEAN) - TRUE if edges and their vertices match; else FALSE
        //
        // PURPOSE: Check that the given edges match in terms of their labels,
        // directedness, and their starting and ending vertex labels.
        //
        // ***** do inexact label matches?
        //---------------------------------------------------------------------------
        
        static BOOLEAN EdgesMatch(LabeledGraph *g1, LabeledEdge *edge1, LabeledGraph *g2, LabeledEdge *edge2, Params *parameters)
        {
            ULONG vertex11Label;
            ULONG vertex12Label;
            ULONG vertex21Label;
            ULONG vertex22Label;
            BOOLEAN match;
            
            match = FALSE;
            vertex11Label = g1->getVertices()[edge1->getE().first].getLabel();
            vertex12Label = g1->getVertices()[edge1->getE().second].getLabel();
            vertex21Label = g2->getVertices()[edge2->getE().first].getLabel();
            vertex22Label = g2->getVertices()[edge2->getE().second].getLabel();
            if ((edge1->getLabel() == edge2->getLabel()) &&
                (edge1->isDirected() == edge2->isDirected()))
            {
                if ((vertex11Label == vertex21Label) &&
                    (vertex12Label == vertex22Label))
                    match = TRUE;
                else if ((! edge1->isDirected()) &&
                         (vertex11Label == vertex22Label) &&
                         (vertex12Label == vertex21Label))
                    match = TRUE;
            }
            return match;
        }

        //---------------------------------------------------------------------------
        // NAME:    ReadGraph
        //
        // INPUTS:  (char *filename) - graph file to read from
        //          (LabelList *labelList) - list of labels to be added to from graph
        //          (BOOLEAN directed) - TRUE if 'e' edges should be directed
        //
        // RETURN:  (Graph *) - graph read from file
        //
        // PURPOSE: Parses graph file, checking for formatting errors, and builds
        // all necessary structures for the graph, which is returned.  labelList
        // is destructively changed to hold any new labels.
        //---------------------------------------------------------------------------
        
        static LabeledGraph *ReadGraph(char *filename, LabelList *labelList, BOOLEAN directed)
        {
            LabeledGraph *graph;
            FILE *graphFile;
            ULONG lineNo;             // Line number counter for graph file
            char token[TOKEN_LEN];
            ULONG vertexOffset = 0;   // Dummy argument to ReadVertex and ReadEdge
            
            // Allocate graph
            graph = AllocateGraph(0,0);
            
            // Open graph file
            graphFile = fopen(filename,"r");
            if (graphFile == NULL)
            {
                fprintf(stderr, "Unable to open graph file %s.\n", filename);
                exit(1);
            }
            
            // Parse graph file
            lineNo = 1;
            while (utility::ReadToken(token, graphFile, &lineNo) != 0)
            {
                if (strcmp(token, "v") == 0)         // read vertex
                    graph->ReadVertex( graphFile, labelList, &lineNo, vertexOffset);
                
                else if (strcmp(token, "e") == 0)    // read 'e' edge
                    graph->ReadEdge( graphFile, labelList, &lineNo, directed, vertexOffset);
                
                else if (strcmp(token, "u") == 0)    // read undirected edge
                    graph->ReadEdge( graphFile, labelList, &lineNo, FALSE, vertexOffset);
                
                else if (strcmp(token, "d") == 0)    // read directed edge
                    graph->ReadEdge( graphFile, labelList, &lineNo, TRUE, vertexOffset);
                
                else
                {
                    fclose(graphFile);
                    graph->FreeGraph();
                    fprintf(stderr, "Unknown token %s in line %lu of graph file %s.\n",
                            token, lineNo, filename);
                    exit(1);
                }
            }
            fclose(graphFile);
            
            //***** trim vertex, edge and label lists
            
            return graph;
        }
        



        
        











         
        


        //---------------------------------------------------------------------------
        // NAME:    AllocateGraph
        //
        // INPUTS:  (unsigned long v) - number of vertices in graph
        //          (unsigned long e) - number of edges in graph
        //
        // RETURN:  (Graph *) - pointer to newly-allocated graph
        //
        // PURPOSE: Allocate memory for new graph containing v vertices and e
        // edges.
        //---------------------------------------------------------------------------

           static LabeledGraph *AllocateGraph(unsigned long v, unsigned long e)
           {
               LabeledGraph *graph;

               graph = new LabeledGraph();
               if (graph == NULL)
                   utility::OutOfMemoryError("AllocateGraph:graph");

               graph->setNumVertices(v);
               graph->setNumEdges(e);
               graph->setVertices(NULL);
               graph->setEdges(NULL);
               if (v > 0)
               {
                   graph->setVertices(new LabeledVertex[v]);
                   if (graph->getVertices() == NULL)
                       utility::OutOfMemoryError("AllocateGraph:graph->vertices");
               }
               graph->setVertexListSize (v);
               if (e > 0)
               {
                   graph->setEdges(new LabeledEdge[e]);
                   if (graph->getEdges() == NULL)
                       utility::OutOfMemoryError("AllocateGraph:graph->edges");
               }
               graph->setEdgeListSize(e);
                
               return graph;
           }
        
     





};




#endif //SUBDUE_LABELEDGRAPH_HPP
