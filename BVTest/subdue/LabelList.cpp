//
//  LabelList.cpp
//  Subdue_v1
//
//  Created by Macbook on 18/02/2019.
//  Copyright © 2019 Macbook. All rights reserved.
//
#include "stdafx.h"
#include "LabelList.hpp"
#include "Subdue.hpp"
#include "utility.hpp"
#include "LabeledGraph.hpp"
//---------------------------------------------------------------------------
// NAME:    PrintLabelList
//
// INPUTS:  (LabelList *labelList) - list of labels
//
// RETURN:  void
//
// PURPOSE: Print labels in given labelList.
//---------------------------------------------------------------------------

void LabelList::PrintLabelList()
{
   unsigned long i;
   printf("Label list:\n");
   for (i = 0; i < numLabels; i++) 
   {
      printf("  labels[%lu] = ", i);
      (getHead()+i)->PrintLabel();
      printf("\n");
   }
}

//---------------------------------------------------------------------------
// NAME: FreeLabelList
//
// INPUTS: (LabelList *labelList)
//
// RETURN: (void)
//
// PURPOSE: Free memory in labelList.
//---------------------------------------------------------------------------

void LabelList::FreeLabelList()
{
   free(getHead());
}




//---------------------------------------------------------------------------
// NAME: LabelMatchFactor
//
// INPUTS: (ULONG labelIndex1)
//         (ULONG labelIndex2) - indices of labels to match
//         (LabelList *labelList) - list of labels
//
// RETURN: (double) - degree of mis-match [0.0,1.0]
//
// PURPOSE: Return the degree of mis-match between the two labels.  If
// the label indices are equal, then the degree of mis-match is 0.0,
// else 1.0.  Values between 0.0 and 1.0 may be implemented based on
// specifics of labels (e.g., threshold or tolerance matching for
// numeric labels, or semantic distance for string labels).
//---------------------------------------------------------------------------

double LabelList::LabelMatchFactor(ULONG labelIndex1, ULONG labelIndex2)
{
    if (labelIndex1 == labelIndex2)
        return 0.0;
    
    // ***** May want more elaborate matching scheme here, i.e., the
    // ***** tolerance or threshold matching of numeric labels.
    
    return 1.0;
}


//---------------------------------------------------------------------------
// NAME: SubLabelNumber
//
// INPUTS: (ULONG index) - index of label in labelList
//         (LabelList *labelList) - list of labels
//
// RETURN: (ULONG) - number from substructure label, or zero if
//                   label is not a valid substructure label
//
// PURPOSE: Checks if label is a valid substructure label of the form
// <SUB_LABEL_STRING>_<#>, where <#> is greater than zero.  If valid,
// then <#> is returned; otherwise, returns zero.
//---------------------------------------------------------------------------

ULONG LabelList::SubLabelNumber(ULONG index)
{
    char *stringLabel;
    char prefix[TOKEN_LEN];
    char rest[TOKEN_LEN];
    BOOLEAN match;
    int i = 0;
    int labelLength;
    int prefixLength;
    ULONG subNumber;
    
    match = TRUE;
    subNumber = 0;
    if (head[index].getLAbelType() == STRING_LABEL)
    { // string label?
        stringLabel = head[index].getStringLabel();
        labelLength = strlen(stringLabel);
        strcpy(prefix, SUB_LABEL_STRING);
        prefixLength = strlen(prefix);
        // check that first part of label matches SUB_LABEL_STRING
        if (labelLength > (prefixLength + 1))
            for (i = 0; ((i < prefixLength) && match); i++)
                if (stringLabel[i] != prefix[i])
                    match = FALSE;
        if (match && (stringLabel[i] != '_')) // underscore present?
            match = FALSE;
        if (match &&                          // rest is a number?
            (sscanf((& stringLabel[i + 1]), "%lu%s", &subNumber, rest) != 1))
            subNumber = 0;
    }
    return subNumber;
}


//---------------------------------------------------------------------------
// NAME:    ReadLabel
//
// INPUTS:  (FILE *fp) - file pointer from which label is read
//          (LabelList *labelList) - list of vertex and edge labels
//          (ULONG *pLineNo) - pointer to line counter in calling function
//
// RETURN:  (ULONG) - index of read label in global label list.
//
// PURPOSE: Reads a label (string or numeric) from the given file and
// stores the label in the given label list if not already there.
// Returns the label's index in the label list.
//---------------------------------------------------------------------------

ULONG LabelList::ReadLabel(FILE *fp,ULONG *pLineNo)
{
    char token[TOKEN_LEN];
    char *endptr;
    Label label;
    utility::ReadToken(token, fp, pLineNo);
    label.setLabelType(NUMERIC_LABEL);
    label.setNumericLabel(strtod(token, &endptr)) ;
    if (*endptr != '\0')
    {
        label.setLabelType(STRING_LABEL) ;
        label.setStringLabel(token) ;
    }
    return StoreLabel(&label);
}

//---------------------------------------------------------------------------
// NAME:    StoreLabel
//
// INPUTS:  (Label *label) - label to be stored (if necessary)
//          (LabelList *labelList) - list to contain label
//
// RETURN:  (ULONG) - label's index in given label list
//
// PURPOSE: Stores the given label, if not already present, in the
// given label list and returns the label's index.  The given label's
// memory can be freed after executing StoreLabel.
//---------------------------------------------------------------------------

ULONG LabelList::StoreLabel(Label *label)
{
    ULONG labelIndex ;
    Label *newLabelList;
    char *stringLabel;
    labelIndex = GetLabelIndex(label);

    if (labelIndex == this->numLabels)
    { // i.e., label not found
        // make sure there is room for a new label
         
        if  (this->size == this->numLabels) 
        {
            this->size += LIST_SIZE_INC;
            newLabelList = (Label *) realloc(head,(sizeof(Label) * this->size));
            if (newLabelList == NULL)
                utility::OutOfMemoryError("StoreLabel:newLabelList");
            head=newLabelList ;
        }

        // store label
        head[this->numLabels].setLabelType(label->getLAbelType());
       
        switch(label->getLAbelType())
        {
            case STRING_LABEL:
                stringLabel = (char *) malloc
                (sizeof(char) *
                 (strlen(label->getStringLabel())) + 1);
                if (stringLabel == NULL)
                    utility::OutOfMemoryError("StoreLabel:stringLabel");
                strcpy(stringLabel, label->getStringLabel());
                head[this->numLabels].setStringLabel(stringLabel) ;
              

                break;
            case NUMERIC_LABEL:
                head[this->numLabels].setNumericLabel(label->getNumericLabel()) ;
                break;
            default:
                break;  // error
        }
         
        head[this->numLabels].setused(FALSE) ;
       
        this->numLabels= this->numLabels + 1;

    }
   
    return labelIndex;
}

//---------------------------------------------------------------------------
// NAME:    GetLabelIndex
//
// INPUTS:  (Label *label) - label being sought
//          (LabelList *labelList) - list in which to look for label
//
// RETURN: (ULONG) - index of label in label list, or number of labels if
//                   label not found
//
// PURPOSE: Returns the index of the given label in the given label
// list.  If not found, then the index just past the end (i.e., number
// of stored labels) is returned.
//---------------------------------------------------------------------------

ULONG LabelList::GetLabelIndex(Label *label)
{
    ULONG i = 0;
    ULONG labelIndex = numLabels;
    BOOLEAN found = FALSE;
    

    while ((i < numLabels) && (! found))
    {
        if (head[i].getLAbelType() == label->getLAbelType())
        {
            switch(label->getLAbelType())
            {
                case STRING_LABEL:
                    if (strcmp(head[i].getStringLabel(),
                               label->getStringLabel()) == 0)
                    {
                        labelIndex = i;
                        found = TRUE;
                    }
                    break;
                case NUMERIC_LABEL:
                    if (head[i].getNumericLabel() ==
                        label->getNumericLabel())
                    {

                        labelIndex = i;
                        found = TRUE;
                    }
                    break;
                default:
                    break;  // error
            }
        }
        i++;
    }
    return labelIndex;
}

//---------------------------------------------------------------------------
// NAME: CompressLabelListWithGraph
//
// INPUTS: (LabelList *newLabelList) - compressed label list
//         (Graph *graph) - graph used to compress label list
//         (Parameters *parameters)
//
// RETURN: (void)
//
// PURPOSE: Adds to newLabelList only the labels present in the given
// graph.  The graph's labels are replaced with indices to the new
// label list.
//---------------------------------------------------------------------------

void LabelList::CompressLabelListWithGraph( LabeledGraph *graph,
                                Params *parameters)
{
    ULONG v, e;
    
    // parameters used
    LabelList *labelList = parameters->labelList;
    
    // add graph's vertex labels to new label list
    for (v = 0; v < graph->getNumVertices(); v++)
        graph->getVertices()[v].setLabel(StoreLabel(& labelList->getHead()[graph->getVertices()[v].getLabel()])) ;
    
    // add graph's edge labels to new label list
    for (e = 0; e < graph->getNumEdges(); e++)
        graph->getEdges()[e].setLabel(StoreLabel(& labelList->getHead()[graph->getEdges()[e].getLabel()]));
}





































