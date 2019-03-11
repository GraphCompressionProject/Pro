//
// Created by Macbook on 11/02/2019.
//

#ifndef SUBDUE_INCREMENT_HPP
#define SUBDUE_INCREMENT_HPP
#include "SubList.hpp"
#include "Substructure.hpp"



    class Increment {
        
    private:
        SubList *subList;           // List of subs for an increment
        unsigned long  incrementNum;         // Increment in which these substructures were
        // discovered
        unsigned long  numPosVertices;       // Number of pos vertices in this increment
        unsigned long numPosEdges;          // Number of pos edges in this increment
        unsigned long  numNegVertices;       // Number of neg vertices in this increment
        unsigned long numNegEdges;          // Number of neg edges in this increment
        unsigned long startPosVertexIndex;  // Index of the first vertex in this increment
        unsigned long startPosEdgeIndex;    // Index of the first edge in this increment
        unsigned long startNegVertexIndex;  // Index of the first vertex in this increment
        unsigned long startNegEdgeIndex;    // Index of the first edge in this increment
        double numPosEgs;           // Number of pos examples in this increment
        double numNegEgs;           // DNumber of pos examples in this increment


    public:
        SubList *getSubList() const {
            return subList;
        }

        void setSubList(SubList *subList) {
            Increment::subList = subList;
        }

        unsigned long getIncrementNum() const {
            return incrementNum;
        }

        void setIncrementNum(unsigned long incrementNum) {
            Increment::incrementNum = incrementNum;
        }

        unsigned long getNumPosVertices() const {
            return numPosVertices;
        }

        void setNumPosVertices(unsigned long numPosVertices) {
            Increment::numPosVertices = numPosVertices;
        }

        unsigned long getNumPosEdges() const {
            return numPosEdges;
        }

        void setNumPosEdges(unsigned long numPosEdges) {
            Increment::numPosEdges = numPosEdges;
        }

        unsigned long getNumNegVertices() const {
            return numNegVertices;
        }

        void setNumNegVertices(unsigned long numNegVertices) {
            Increment::numNegVertices = numNegVertices;
        }

        unsigned long getNumNegEdges() const {
            return numNegEdges;
        }

        void setNumNegEdges(unsigned long numNegEdges) {
            Increment::numNegEdges = numNegEdges;
        }

        unsigned long getStartPosVertexIndex() const {
            return startPosVertexIndex;
        }

        void setStartPosVertexIndex(unsigned long startPosVertexIndex) {
            Increment::startPosVertexIndex = startPosVertexIndex;
        }

        unsigned long getStartPosEdgeIndex() const {
            return startPosEdgeIndex;
        }

        void setStartPosEdgeIndex(unsigned long startPosEdgeIndex) {
            Increment::startPosEdgeIndex = startPosEdgeIndex;
        }

        unsigned long getStartNegVertexIndex() const {
            return startNegVertexIndex;
        }

        void setStartNegVertexIndex(unsigned long startNegVertexIndex) {
            Increment::startNegVertexIndex = startNegVertexIndex;
        }

        unsigned long getStartNegEdgeIndex() const {
            return startNegEdgeIndex;
        }

        void setStartNegEdgeIndex(unsigned long startNegEdgeIndex) {
            Increment::startNegEdgeIndex = startNegEdgeIndex;
        }

        double getNumPosEgs() const {
            return numPosEgs;
        }

        void setNumPosEgs(double numPosEgs) {
            Increment::numPosEgs = numPosEgs;
        }

        double getNumNegEgs() const {
            return numNegEgs;
        }

        void setNumNegEgs(double numNegEgs) {
            Increment::numNegEgs = numNegEgs;
        }
        
        ULONG IncrementSize( ULONG graphType);
        ULONG IncrementNumExamples(ULONG graphType);


    };





#endif //SUBDUE_INCREMENT_HPP
