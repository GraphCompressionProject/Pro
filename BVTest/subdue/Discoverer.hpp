//
//  Discoverer.hpp
//  Subdue_v1
//
//  Created by Macbook on 18/02/2019.
//  Copyright © 2019 Macbook. All rights reserved.
//

#ifndef Discoverer_hpp
#define Discoverer_hpp


#include <stdio.h>
#include "SubList.hpp"
#include "Params.hpp"

class Substructure;
class LabeledGraph;
    
    class Discoverer{
    public:
      
        SubList *DiscoverSubs(Params *parameters);
        
        
        SubList *GetInitialSubs(Params *parameters);
        
        
        BOOLEAN SinglePreviousSub(Substructure *sub, Params *parameters);
        
    };



#endif /* Discoverer_hpp */
