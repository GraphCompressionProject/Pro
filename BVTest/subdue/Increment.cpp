//
// Created by Macbook on 11/02/2019.
//
#include "stdafx.h"
#include "Increment.hpp"


//---------------------------------------------------------------------------
// NAME: IncrementSize
//
// INPUTS: (Parameters *parameters) - system parameters
//         (ULONG incrementNum) - current increment
//         (ULONG graphType) - POS or NEG
//
// RETURN: (ULONG) - size of increment
//
// PURPOSE: Return size of graph as vertices plus edges.
//---------------------------------------------------------------------------

ULONG Increment::IncrementSize( ULONG graphType)
{
    
    if (graphType == POS)
        return(numPosVertices + numPosEdges);
    else
        return(numNegVertices + numNegEdges);
}

//---------------------------------------------------------------------------
// NAME: IncrementNumExamples
//
// INPUTS: (Parameters *parameters) - system parameters
//         (ULONG incrementNum) - current increment
//         (ULONG graphType) - POS or NEG
//
// RETURN: (ULONG) - number of edges
//
// PURPOSE: Return number of positive or negative examples in increment.
//---------------------------------------------------------------------------

ULONG Increment::IncrementNumExamples(ULONG graphType)
{
   
    
    if (graphType == POS)
        return((ULONG) numPosEgs);
    else
        return((ULONG) numNegEgs);
}

