//
// Created by Macbook on 10/02/2019.
//

#ifndef SUBDUE_MATCHHEAPNODE_HPP
#define SUBDUE_MATCHHEAPNODE_HPP

#include "VertexMap.hpp"




    class MatchHeapNode {
    private:
        unsigned long depth;
        double cost;
        VertexMap *mapping;
        
    public:
        unsigned long getDepth() ;
        
        void setDepth(unsigned long depth);
        
        double getCost() ;
        
        void setCost(double cost);
        
        VertexMap *getMapping() ;
        
        void setMapping(VertexMap *mapping);
        
        void PrintMatchHeapNode();
        
        
    };




#endif //SUBDUE_MATCHHEAPNODE_HPP
