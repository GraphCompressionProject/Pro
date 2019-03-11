//
//  IncrementList.cpp
//  Subdue_v1
//
//  Created by Macbook on 18/02/2019.
//  Copyright © 2019 Macbook. All rights reserved.
//
#include "stdafx.h"
#include "IncrementList.hpp"
#include "Subdue.hpp"
#include "IncrementListNode.hpp"
//---------------------------------------------------------------------------
// NAME: GetStartVertexIndex
//
// INPUTS:  (ULONG incrementNum) - current increment
//          (Parameters *parameters) - system parameters
//          (ULONG graphType) - POS or NEG
//
// RETURN:  ULONG - starting vertex number for corresponding increment
//
// PURPOSE: Return the index of the starting positive vertex for the increment.
//---------------------------------------------------------------------------

unsigned long  IncrementList::GetStartVertexIndex( unsigned long graphType)
{
    IncrementListNode *incNodePtr = getHead();

    if (incNodePtr == NULL) // Empty list, this is the first increment
    {
        printf("Error GetStartVertexIndex: increment list empty\n");
        exit(1);
    }
    while (incNodePtr->getNext() != NULL)
        incNodePtr = (IncrementListNode *)incNodePtr->getNext();

    if (graphType == POS)
        return incNodePtr->getInstance()->getStartPosVertexIndex();
    else
        return incNodePtr->getInstance()->getStartNegVertexIndex();
}


