//
//  IncrementList.hpp
//  Subdue_v1
//
//  Created by Macbook on 18/02/2019.
//  Copyright © 2019 Macbook. All rights reserved.
//

#ifndef IncrementList_hpp
#define IncrementList_hpp

#include <stdio.h>
#include "Subdue.hpp"
#include "List.hpp"
#include "IncrementList.hpp"

class IncrementListNode;

    class IncrementList: public List<IncrementListNode>{

public :
       
        unsigned long GetStartVertexIndex( unsigned long graphType);
        IncrementListNode *getHead(){
            return  (IncrementListNode *)List<IncrementListNode>::getHead();
        }
        
    };


#endif /* IncrementList_hpp */
