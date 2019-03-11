//
//  InstanceListNode.hpp
//  Subdue_v1
//
//  Created by Macbook on 18/02/2019.
//  Copyright © 2019 Macbook. All rights reserved.
//

#ifndef InstanceListNode_hpp
#define InstanceListNode_hpp

#include <stdio.h>

#include "ListNode.hpp"
#include "Instance.hpp"
#include "utility.hpp"
    class InstanceListNode: public ListNode<Instance>{
    public:

       //---------------------------------------------------------------------------
       // NAME: FreeInstanceListNode
       //
       // INPUTS: (InstanceListNode *instanceListNode)
       //
       // RETURN: (void)
       //
       // PURPOSE: Free memory used by given instance list node.
       //---------------------------------------------------------------------------

        void FreeInstanceListNode()
        {
                if (instance != NULL){
                 instance->setRefCount(instance->getRefCount()-1);
                }
                instance->FreeInstance();
        }
        
        
        
        //---------------------------------------------------------------------------
        // NAME: AllocateInstanceListNode
        //
        // INPUTS: (Instance *instance) - instance to be stored in node
        //
        // RETURN: (InstanceListNode *) - newly allocated InstanceListNode
        //
        // PURPOSE: Allocate a new InstanceListNode.
        //---------------------------------------------------------------------------
        
        static InstanceListNode *AllocateInstanceListNode(Instance *instance)
        {
            InstanceListNode *instanceListNode;
            
            instanceListNode = new InstanceListNode();
            if (instanceListNode == NULL)
                utility::OutOfMemoryError("AllocateInstanceListNode:InstanceListNode");
            instanceListNode->setInstance(instance) ;
            instance->setRefCount(instance->getRefCount()+1);
            instanceListNode->setNext(NULL) ;
            return instanceListNode;
        }
        
    };



#endif /* InstanceListNode_hpp */
