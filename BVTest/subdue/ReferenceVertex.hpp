//
// Created by Macbook on 10/02/2019.
//

#ifndef SUBDUE_REFERENCEVERTEX_HPP
#define SUBDUE_REFERENCEVERTEX_HPP

typedef unsigned long  ULONG;
typedef bool BOOLEAN;



    class ReferenceVertex {
    private:
        ULONG label;    // index into label list of vertex's label
        ULONG numEdges; // number of edges defined using this vertex
        ULONG *edges;   // indices into edge array of edges using this vertex
        ULONG map;      // used to store mapping of this vertex to corresponding
        // vertex in another graph
        BOOLEAN used;   // flag for marking vertex used at various times
        // used flag assumed FALSE, so always reset when done
        BOOLEAN vertexValid;

    public:
        void setLabel(ULONG label){
            ReferenceVertex::label = label;
        }
        ULONG getLabel(){
            return label;
        }
        
        void setNumEdges(ULONG numedges){
            ReferenceVertex::numEdges = numedges;
        }
        ULONG getNumEdges(){
            return numEdges;
        }
        
        void setMap(ULONG map){
            ReferenceVertex::map = map;
        }
        ULONG getMap(){
            return map;
        }
        
        void setEdges(ULONG *edges){
            ReferenceVertex::edges = edges;
        }
        ULONG *getEdges(){
            return edges;
        }
        
        bool isUsed(){
            return used ;
        }
        
        void setUsed(BOOLEAN val){
            used =val;
        }
        
        bool isVertexValid(){
            return vertexValid ;
        }
        
        void setVertexValid(bool vertexValid){
            this->vertexValid = vertexValid;
        }
        

    };





#endif //SUBDUE_REFERENCEVERTEX_HPP
