//
//  SubListNode.cpp
//  Subdue_v1
//
//  Created by Macbook on 18/02/2019.
//  Copyright © 2019 Macbook. All rights reserved.
//
#include "stdafx.h"
#include "SubListNode.hpp"
#include "Substructure.hpp"

//---------------------------------------------------------------------------
// NAME: FreeSubListNode
//
// INPUTS: (SubListNode *subListNode) - SubListNode to be freed
//
// RETURN: (void)
//
// PURPOSE: Deallocate memory of subListNode, including substructure
// if exists.
//---------------------------------------------------------------------------

void SubListNode::FreeSubListNode()
{
 
        getInstance()->FreeSub();
   

}
