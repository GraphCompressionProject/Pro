//
//  SubList.hpp
//  Subdue_v1
//
//  Created by Macbook on 18/02/2019.
//  Copyright © 2019 Macbook. All rights reserved.
//

#ifndef SubList_hpp
#define SubList_hpp

#include <stdio.h>

#include "List.hpp"
#include "SubListNode.hpp"
#include "utility.hpp"

class LabelList ;
class Substructure ;
class InstanceList ;
class LabeledVertex;

class SubList : public List<SubListNode>{
        
    public :
        //---------------------------------------------------------------------------
        // NAME: AllocateSubList
        //
        // INPUTS: (void)
        //
        // RETURN: (SubList *) - newly-allocated substructure list
        //
        // PURPOSE: Allocate and return an empty list to hold substructures.
        //---------------------------------------------------------------------------
        ~SubList(){
            FreeSubList();
        }
       static SubList *AllocateSubList(void)
       {
           SubList *subList;
           
           subList = (SubList *) malloc(sizeof(SubList));
           if (subList == NULL)
               utility::OutOfMemoryError("AllocateSubList:subList");
           subList->setHead(NULL);
           return subList;
       };

      
        void FreeSubList();
        void SubListInsert(Substructure *sub, ULONG max,BOOLEAN valueBased, LabelList *labelList);
        BOOLEAN MemberOfSubList(Substructure *sub, LabelList *labelList);
        void PrintSubList( Params *parameters);
    
        ULONG CountSubs();
    
        void InsertSub( Substructure *sub, double subValue,
                            ULONG numPosInstances, ULONG numNegInstances);
    
        void PrintStoredSubList( Params *parameters);
        InstanceList *EvaluateBoundaryInstances(Params *parameters);
        BOOLEAN VertexInSubList( LabeledVertex *vertex);
    
        BOOLEAN FindInitialBoundaryInstances( LabeledGraph *graph,
                                                  LabelList *labelList,
                                                  ULONG startVertexIndex,
                                         Params *parameters);
    
        void  WriteResultsToFile( FILE *subsFile,
                                     Increment *increment, Params *parameters);

        
};



#endif /* SubList_hpp */
