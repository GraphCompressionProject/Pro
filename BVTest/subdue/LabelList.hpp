//
//  LabelList.hpp
//  Subdue_v1
//
//  Created by Macbook on 18/02/2019.
//  Copyright © 2019 Macbook. All rights reserved.
//

#ifndef LabelList_hpp
#define LabelList_hpp

#include <stdio.h>
#include "utility.hpp"

#include "List.hpp"
#include "Label.hpp"

typedef unsigned long ULONG ;

class Instance ;
    class LabelList : public List<Label>{
        
    private:
        unsigned long  size;      // Number of label slots currently allocated in array
        unsigned long  numLabels; // Number of actual labels stored in list

    public:
        unsigned long getSize() const {
            return size;
        }

        void setSize(unsigned long size) {
            LabelList::size = size;
        }

        unsigned long getNumLabels() const {
            return numLabels;
        }

        Label *getHead(){
            return (Label *)List<Label>::getHead();
        }

        void setNumLabels(unsigned long numLabels) {
            LabelList::numLabels = numLabels;
        }

   
        void PrintLabelList();
        void FreeLabelList();
        double LabelMatchFactor(ULONG labelIndex1, ULONG labelIndex2);
        ULONG SubLabelNumber(ULONG index);
        ULONG ReadLabel(FILE *fp, ULONG *pLineNo);
        ULONG StoreLabel(Label *label);
        ULONG GetLabelIndex(Label *label);
        void CompressLabelListWithGraph( LabeledGraph *graph,
                                                   Params *parameters);
        
        
        //---------------------------------------------------------------------------
        // NAME:    AllocateLabelList
        //
        // INPUTS:  (void)
        //
        // RETURN:  (LabelList *)
        //
        // PURPOSE: Allocate memory for empty label list.
        //---------------------------------------------------------------------------
        
        static LabelList *AllocateLabelList(void)
        {
            LabelList *labelList;
            labelList = new LabelList();
            if (labelList == NULL)
                utility::OutOfMemoryError("AllocateLabelList:labelList");
            labelList->setSize(0);
            labelList->setNumLabels(0) ;
            labelList->setHead(NULL) ;
            return labelList;
        }
        
        
        
        
        
    };


#endif /* LabelList_hpp */
