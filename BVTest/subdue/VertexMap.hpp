//
// Created by Macbook on 10/02/2019.
//

#ifndef SUBDUE_VERTEXMAP_HPP
#define SUBDUE_VERTEXMAP_HPP
#include "Subdue.hpp"
#include "utility.hpp"




    class VertexMap {
    private:
        unsigned long v1;
        unsigned long v2;
    public:
        unsigned long getV1() const;
        
        void setV1(unsigned long v1);
        
        unsigned long getV2() const;
        
        void setV2(unsigned long v2);
        
        
        //---------------------------------------------------------------------------
        // NAME: AllocateNewMapping
        //
        // INPUTS: (ULONG depth) - depth (vertices mapped) of new node in search space
        //         (VertexMap *mapping) - partial mapping to be augmented
        //         (ULONG v1)
        //         (ULONG v2) - node's mapping includes v1 -> v2
        //
        // RETURN:  (VertexMap *) - newly-allocated vertex mapping
        //
        // PURPOSE: Allocate a new vertex mapping as a copy of the given mapping
        // appended with the new map v1 -> v2.
        //---------------------------------------------------------------------------
        
        static VertexMap *AllocateNewMapping(ULONG depth, VertexMap *mapping,
                                      ULONG v1, ULONG v2)
        {
            VertexMap *newMapping;
            ULONG i;
            
            newMapping = (VertexMap *) malloc(sizeof(VertexMap) * depth);
            if (newMapping == NULL)
                utility::OutOfMemoryError("AllocateNewMapping: newMapping");
            for (i = 0; i < (depth - 1); i++)
            {
                newMapping[i].setV1(mapping[i].getV1()) ;
                newMapping[i].setV2(mapping[i].getV2()) ;
            }
            newMapping[depth-1].setV1(v1) ;
            newMapping[depth-1].setV2(v2) ;
            
            return newMapping;
        }
        
    };





#endif //SUBDUE_VERTEXMAP_HPP
