//
// Created by Macbook on 10/02/2019.
//
#include "stdafx.h"
#include "Label.hpp"
#include "Subdue.hpp"

#include "LabelList.hpp"

//---------------------------------------------------------------------------
// NAME:    PrintLabel
//
// INPUTS:  (unsigned long index) - index into label list
//          (LabelList *labelList) - list of labels
//
// RETURN:  void
//
// PURPOSE: Print label of given index.
//---------------------------------------------------------------------------

void Label::PrintLabel()
{
   switch(labelType) 
   {
      case STRING_LABEL:
         fprintf(stdout, "%s", labelValue.getStringLabel() );
         break;
      case NUMERIC_LABEL:
         fprintf(stdout, "%.*g", NUMERIC_OUTPUT_PRECISION,
                 labelValue.getNumericLabel());
         break;
      default:
         break;
   }
}

//---------------------------------------------------------------------------
// NAME: WriteLabelToFile
//
// INPUTS: (FILE *outFile) - file to write to
//         (unsigned long index) - index of label in label list
//         (LabelList *labelList) - label list
//         (BOOLEAN suppressQuotes) - if TRUE, then delimiting quotes not
//                                    printed
//
// RETURN: (void)
//
// PURPOSE: Write label to given file.  If suppressQuotes
// is TRUE and the label has delimiting end quotes, then do not print them.
//---------------------------------------------------------------------------

void Label::WriteLabelToFile(FILE *outFile, bool suppressQuotes)
{
  
   char labelStr[TOKEN_LEN];
   int strLength;
   int i;


   switch(labelType) 
   {
      case STRING_LABEL:
         strcpy(labelStr, labelValue.getStringLabel());
         if (suppressQuotes) 
         { // remove delimiting DOUBLEQUOTES if there
            strLength = strlen(labelStr);
            if ((labelStr[0] == DOUBLEQUOTE) &&
                (labelStr[strLength - 1] == DOUBLEQUOTE)) 
            {
               for (i = 0; i < strLength; i++)
                  labelStr[i] = labelStr[i+1];
               labelStr[strLength - 2] = '\0';
            }
         }
         fprintf(outFile, "%s", labelStr);
         break;
      case NUMERIC_LABEL:
         fprintf(outFile, "%.*g", NUMERIC_OUTPUT_PRECISION, labelValue.getNumericLabel());
         break;
      default:
         break;
   }
}


//---------------------------------------------------------------------------
// NAME: SubLabelNumber
//
// INPUTS: (unsigned long index) - index of label in labelList
//         (LabelList *labelList) - list of labels
//
// RETURN: (unsigned long) - number from substructure label, or zero if 
//                   label is not a valid substructure label
//
// PURPOSE: Checks if label is a valid substructure label of the form
// <SUB_LABEL_STRING>_<#>, where <#> is greater than zero.  If valid,
// then <#> is returned; otherwise, returns zero.
//---------------------------------------------------------------------------

unsigned long Label::SubLabelNumber()
{
   char *stringLabel;
   char prefix[TOKEN_LEN];
   char rest[TOKEN_LEN];
   BOOLEAN match;
   int i = 0;
   int labelLength;
   int prefixLength;
   unsigned long subNumber;

   match = TRUE;
   subNumber = 0;
   if (labelType == STRING_LABEL) 
   { // string label?
      stringLabel = labelValue.getStringLabel();
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
