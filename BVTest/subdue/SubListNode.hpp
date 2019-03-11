//
//  SubListNode.hpp
//  Subdue_v1
//
//  Created by Macbook on 18/02/2019.
//  Copyright © 2019 Macbook. All rights reserved.
//
class Substructure;
#ifndef SubListNode_hpp
#define SubListNode_hpp

#include <stdio.h>
#include <iostream>
#include "utility.hpp"
#include "ListNode.hpp"

  
    class SubListNode : public ListNode<Substructure> {
        
        public :
        void FreeSubListNode();
        
        
        //---------------------------------------------------------------------------
        // NAME: AllocateSubListNode
        //
        // INPUTS: (Substructure *sub) - substructure to be stored in node
        //
        // RETURN: (SubListNode *) - newly allocated SubListNode
        //
        // PURPOSE: Allocate a new SubListNode.
        //---------------------------------------------------------------------------
        
        static SubListNode *AllocateSubListNode(Substructure *sub)
        {
            SubListNode *subListNode;
            
            subListNode = (SubListNode *) malloc(sizeof(SubListNode));
            if (subListNode == NULL)
                utility::OutOfMemoryError("SubListNode");
            subListNode->setInstance(sub);
            subListNode->setNext(NULL);
            return subListNode;
        }

        
    };



#endif /* SubListNode_hpp */
