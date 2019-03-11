//
//  Tester.hpp
//  Subdue_v1
//
//  Created by Macbook on 25/02/2019.
//  Copyright © 2019 Macbook. All rights reserved.
//

#ifndef Tester_hpp
#define Tester_hpp

#include <stdio.h>
typedef unsigned long  ULONG;
typedef bool BOOLEAN;
class Params;

class Tester{
public:
    
    void Test(char *subsFileName, char *graphFileName, Params *parameters,
                      ULONG *TPp, ULONG *TNp, ULONG *FPp, ULONG *FNp);
    
    
    
};




#endif /* Tester_hpp */
