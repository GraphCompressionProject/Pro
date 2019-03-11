//
//  Tester_main.cpp
//  Subdue_v1
//
//  Created by Macbook on 28/02/2019.
//  Copyright © 2019 Macbook. All rights reserved.
//
#include "stdafx.h"
#include "Tester_main.hpp"
#include "LabelList.hpp"
#include "Tester.hpp"


//---------------------------------------------------------------------------
// NAME: GetParameters
//
// INPUTS: (int argc) - number of command-line arguments
//         (char *argv[]) - array of command-line argument strings
//
// RETURN: (Parameters *)
//
// PURPOSE: Initialize parameters structure and process command-line
// options.
//---------------------------------------------------------------------------

Params *Tester_main::GetParameters(int argc, char *argv[])
{
    Params *parameters;
    
    parameters = (Params *) malloc(sizeof(Params));
    if (parameters == NULL)
        utility::OutOfMemoryError("GetParameters:parameters");
    
    // initialize default parameter settings
    parameters->directed = TRUE;
    parameters->labelList = LabelList::AllocateLabelList();
    
    return parameters;
}



//---------------------------------------------------------------------------
// NAME:    main
//
// INPUTS:  (int argc) - number of arguments to program
//          (char **argv) - array of strings of arguments to program
//
// RETURN:  (int) - 0 if all is well
//
// PURPOSE: Main function for computing test statistics for a concept
// (disjunction of substructures) against a st of examples.  Takes two
// command-line arguments, which are the input substructures file and the
// input example graphs file.
//---------------------------------------------------------------------------

int Tester_main::main(int argc, char **argv)
{
    Params *parameters;
    ULONG FP = 0;
    ULONG FN = 0;
    ULONG TP = 0;
    ULONG TN = 0;
    double FPRate;
    double TPRate;
    double error;
    
    if (argc != 3)
    {
        fprintf(stderr, "USAGE: %s <subsfile> <graphfile>\n", argv[0]);
        exit(1);
    }
    
    parameters = GetParameters(argc, argv);
    Tester t;
    t.Test(argv[1], argv[2], parameters, &TP, &TN, &FP, &FN);
    
    // compute and output stats
    FPRate = ((double) FP) / ((double) (TN+FP));
    TPRate = ((double) TP) / ((double) (FN+TP));
    error = ((double) (FP+FN)) / ((double) (TP+TN+FP+FN));
    fprintf(stdout, "TP = %lu\nTN = %lu\nFP = %lu\nFN = %lu\n",
            TP, TN, FP, FN);
    fprintf(stdout, "(TP+FN) = %lu\n(TN+FP) = %lu\n", (TP+FN), (TN+FP));
    fprintf(stdout, "(TP+TN+FP+FN) = %lu\n", (TP+TN+FP+FN));
    fprintf(stdout, "(FP/(TN+FP)) = %f\n", FPRate);
    fprintf(stdout, "(TP/(FN+TP)) = %f\n", TPRate);
    fprintf(stdout, "Error = %f\n", error);
    fflush(stdout);
    
    
    return 0;
}
