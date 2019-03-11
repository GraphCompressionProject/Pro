//
//  Labval.hpp
//  Subdue_v1
//
//  Created by Macbook on 20/02/2019.
//  Copyright © 2019 Macbook. All rights reserved.
//

#ifndef Labval_hpp
#define Labval_hpp

#include <stdio.h>
#include <string.h>
#include <iostream>

union Labval_{
    char *stringLabel ;
    double numericLabel;
};

class Labval{
private:
    Labval_ l;
public :

    char *getStringLabel(){
        return l.stringLabel;
    }
    
    double getNumericLabel(){
        return l.numericLabel;
    }
    
    void setStringLabel(char *s){
        l.stringLabel = (char *) malloc(sizeof(char) * (strlen(s)+1));        
        strcpy(l.stringLabel,s);
    }
    
    void setNumericLabel(double n){
        l.numericLabel = n;
    }
    
};

#endif /* Labval_hpp */
