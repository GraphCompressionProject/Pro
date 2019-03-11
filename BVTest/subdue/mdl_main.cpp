//
//  mdl_main.cpp
//  Subdue_v1
//
//  Created by Macbook on 28/02/2019.
//  Copyright © 2019 Macbook. All rights reserved.
//
#include "stdafx.h"
#include "mdl_main.hpp"

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

Params *mdl_main::GetParameters(int argc, char *argv[])
{
    Params *parameters;
    int i;
    double doubleArg;
    
    if (argc < 3)
    {
        fprintf(stderr, "%s: not enough arguments\n", argv[0]);
        exit(1);
    }
    
    parameters = (Params *) malloc(sizeof(Params));
    if (parameters == NULL)
        utility::OutOfMemoryError("GetParameters:parameters");
    
    // initialize default parameter settings
    parameters->directed = TRUE;
    parameters->allowInstanceOverlap = FALSE;
    parameters->threshold = 0.0;
    parameters->outputToFile = FALSE;
    parameters->posGraph = NULL;
    parameters->negGraph = NULL;
    parameters->incremental = FALSE;
    
    // process command-line options
    i = 1;
    while (i < (argc - 2))
    {
        if (strcmp(argv[i], "-dot") == 0)
        {
            i++;
            strcpy(parameters->outFileName, argv[i]);
            parameters->outputToFile = TRUE;
        }
        else if (strcmp(argv[i], "-overlap") == 0)
        {
            parameters->allowInstanceOverlap = TRUE;
        }
        else if (strcmp(argv[i], "-threshold") == 0)
        {
            i++;
            sscanf(argv[i], "%lf", &doubleArg);
            if ((doubleArg < 0.0) || (doubleArg > 1.0))
            {
                fprintf(stderr, "%s: threshold must be 0.0-1.0\n", argv[0]);
                exit(1);
            }
            parameters->threshold = doubleArg;
        }
        else
        {
            fprintf(stderr, "%s: unknown option %s\n", argv[0], argv[i]);
            exit(1);
        }
        i++;
    }
    
    // initialize log2Factorial[0..1]
    parameters->log2Factorial = (double *) malloc(2 * sizeof(double));
    if (parameters->log2Factorial == NULL)
        utility::OutOfMemoryError("GetParameters:paramters->log2Factorial");
    parameters->log2FactorialSize = 2;
    parameters->log2Factorial[0] = 0; // lg(0!)
    parameters->log2Factorial[1] = 0; // lg(1!)
    
    parameters->labelList = LabelList::AllocateLabelList();
    
    return parameters;
}



