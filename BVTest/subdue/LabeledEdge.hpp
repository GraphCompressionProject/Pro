//
// Created by Macbook on 10/02/2019.
//

#ifndef SUBDUE_LABELEDEDGE_HPP
#define SUBDUE_LABELEDEDGE_HPP
#include "utility.hpp"
#include <iostream>


#define TRUE 1
#define FALSE 0

using namespace std;

    typedef std::pair<unsigned long , unsigned  long > edge;
    
    class LabeledEdge {
    private:
        edge  e;
        unsigned long label;
        bool directed;
        bool used;
        bool spanIncrement;
        bool validPath;

    public:
         edge &getE() ;

        void setE( edge &e);

        unsigned long getLabel() ;

        void setLabel(unsigned long label);

        bool isDirected() ;

        void setDirected(bool directed);

        bool isUsed() ;

        void setUsed(bool used);

        bool isSpanIncrement() ;

        void setSpanIncrement(bool spanIncrement);

        bool isValidPath() ;

        void setValidPath(bool validPath);
        
        //---------------------------------------------------------------------------
        // NAME: AddOverlapEdge
        //
        // INPUTS: (Edge *overlapEdges) - edge array possibly realloc-ed to store
        //                                edge
        //         (ULONG *numOverlapEdgesPtr) - pointer to variable holding number
        //           of total overlapping edges; this may or may not be incremented
        //           depending on uniqueness of "OVERLAP" edge
        //         (ULONG sub1VertexIndex) - "SUB" vertex index for first instance
        //         (ULONG sub2VertexIndex) - "SUB" vertex index for second instance
        //         (ULONG overlapLabelIndex) - index to "OVERLAP" label
        //
        // RETURN: (Edge *) - pointer to possibly realloc-ed edge array
        //
        // PURPOSE: If an "OVERLAP" edge does not already exist between Sub1
        // and Sub2, then this function adds the edge to the given
        // overlapEdges array and increments the call-by-reference variable
        // numOverlapEdgesPtr.  Assumes sub1VertexIndex < sub2VertexIndex.
        //---------------------------------------------------------------------------
        
        static LabeledEdge *AddOverlapEdge(LabeledEdge *overlapEdges,
                                           unsigned long *numOverlapEdgesPtr,unsigned long sub1VertexIndex, unsigned long sub2VertexIndex,unsigned long overlapLabelIndex)
        {
            unsigned long numOverlapEdges;
            unsigned long e;
            bool found;
            
            found = FALSE;
            numOverlapEdges = *numOverlapEdgesPtr;
            for (e = 0; ((e < numOverlapEdges) && (! found)); e++)
                if ((overlapEdges[e].getE().first == sub1VertexIndex) &&
                    (overlapEdges[e].getE().second == sub2VertexIndex))
                    found = TRUE;
            if (! found)
            {
                overlapEdges = (LabeledEdge *) realloc(overlapEdges,
                                                       ((numOverlapEdges + 1) * sizeof(LabeledEdge)));
                if (overlapEdges == NULL)
                    utility::OutOfMemoryError("AddOverlapEdge:overlapEdges");
                StoreEdge(overlapEdges, numOverlapEdges, sub1VertexIndex,
                          sub2VertexIndex, overlapLabelIndex, FALSE, FALSE);
                numOverlapEdges++;
            }
            *numOverlapEdgesPtr = numOverlapEdges;
            return overlapEdges;
        }
        
        
        //---------------------------------------------------------------------------
        // NAME: StoreEdge
        //
        // INPUTS: (Edge *overlapEdges) - edge array where edge is stored
        //         (ULONG edgeIndex) - index into edge array where edge is stored
        //         (ULONG v1) - vertex1 of edge
        //         (ULONG v2) - vertex2 of edge
        //         (ULONG label) - edge label index
        //         (BOOLEAN directed) - edge directedness
        //         (BOOLEAN spansIncrement) - edge crossing increment boundary
        //
        // RETURN: (void)
        //
        // PURPOSE: Procedure to store an edge in edge array.
        //---------------------------------------------------------------------------
        
        static void StoreEdge(LabeledEdge* overlapEdges, ULONG edgeIndex, ULONG v1, ULONG v2, ULONG label, BOOLEAN directed,
                                     BOOLEAN spansIncrement)
        {
            overlapEdges[edgeIndex].getE().first = v1;
            overlapEdges[edgeIndex].getE().second = v2;
            overlapEdges[edgeIndex].setLabel(label);
            overlapEdges[edgeIndex].setDirected(directed);
            overlapEdges[edgeIndex].setUsed(FALSE) ;
            overlapEdges[edgeIndex].setSpanIncrement(spansIncrement) ;
        }
        BOOLEAN PositiveExample( LabeledGraph **subGraphs,
                                              ULONG numSubGraphs, Params *parameters);
        
        

        


    };





#endif //SUBDUE_LABELEDEDGE_HPP
