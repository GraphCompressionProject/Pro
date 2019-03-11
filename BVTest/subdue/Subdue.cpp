//
// Created by Macbook on 10/02/2019.
//
#include "stdafx.h"
#include "Subdue.hpp"

#include <iostream>
#include <iomanip>
#include <chrono>
#include <ctime>
#include <thread>


#include "Compressor.hpp"
#include "LabeledGraph.hpp"
#include "Discoverer.hpp"
#include "Substructure.hpp"
#include "GraphWriter.hpp"
#include "Increment.hpp"
#include "IncrementList.hpp"
#include "InstanceVertexList.hpp"

using namespace std;
//---------------------------------------------------------------------------
// NAME:    main
//
// INPUTS:  (int argc) - number of arguments to program
//          (char **argv) - array of strings of arguments to program
//
// RETURN:  (int) - 0 if all is well
//
// PURPOSE: Main SUBDUE function that processes command-line arguments
//          and initiates discovery.
//---------------------------------------------------------------------------

int Subdue::main (int argc, char **argv)
{
   // struct tms tmsstart, tmsend;
    //std::clock_t startTime, endTime;
    static long clktck = 0;
    time_t iterationStartTime;
    time_t iterationEndTime;
    SubList *subList;
    Params *parameters;
    FILE *outputFile;
    ULONG iteration;
    BOOLEAN done;
    Compressor c;
    Discoverer d;
    GraphWriter wr;
    //clktck = sysconf(_SC_CLK_TCK);
   // startTime = times(&tmsstart);

	std::clock_t c_start = std::clock();
	auto t_start = std::chrono::high_resolution_clock::now();


    printf("SUBDUE %s\n\n", SUBDUE_VERSION);

    parameters = GetParameters(argc, argv);
      
    if (parameters->incremental)
        ISubdue(parameters);
    else
    {

        // compress pos and neg graphs with predefined subs, if given
        if (parameters->numPreSubs > 0)
            c.CompressWithPredefinedSubs(parameters);
        
        
        PostProcessParameters(parameters);
        PrintParameters(parameters);
        
        if (parameters->iterations > 1)
            printf("----- Iteration 1 -----\n\n");
        
        iteration = 1;
        done = FALSE;
        while ((iteration <= parameters->iterations) && (!done))
        {
            iterationStartTime = time(NULL);
            if (iteration > 1)
                printf("----- Iteration %lu -----\n\n", iteration);
            
            printf("%lu positive graphs: %lu vertices, %lu edges",
                   parameters->numPosEgs, parameters->posGraph->getNumVertices(),
                   parameters->posGraph->getNumEdges());
            
            if (parameters->evalMethod == EVAL_MDL)
                printf(", %.0f bits\n", parameters->posGraphDL);
            else
                printf("\n");
            if (parameters->negGraph != NULL)
            {
                printf("%lu negative graphs: %lu vertices, %lu edges",
                       parameters->numNegEgs, parameters->negGraph->getNumVertices(),
                       parameters->negGraph->getNumEdges());
                if (parameters->evalMethod == EVAL_MDL)
                    printf(", %.0f bits\n", parameters->negGraphDL);
                else
                    printf("\n");
            }
            printf("%lu unique labels\n", parameters->labelList->getNumLabels());
            printf("\n");


            subList = d.DiscoverSubs(parameters);
            
            if (subList->getHead() == NULL)
            {
                done = TRUE;
                printf("No substructures found.\n\n");
            }
            else
            {
                // write output to stdout
                if (parameters->outputLevel > 1)
                {
                    printf("\nBest %lu substructures:\n\n", subList->CountSubs ());
                    subList->PrintSubList( parameters);
                }
                else
                {
                    printf("\nBest substructure:\n\n");
                    subList->getHead()->getInstance()->PrintSub( parameters);
                }
                
                // write machine-readable output to file, if given
                if (parameters->outputToFile)
                {
                    outputFile = fopen(parameters->outFileName, "a");
                    if (outputFile == NULL)
                    {
                        printf("WARNING: unable to write to output file %s,",
                               parameters->outFileName);
                        printf("disabling\n");
                        parameters->outputToFile = FALSE;
                    }
                    wr.WriteGraphToFile(outputFile, subList->getHead()->getInstance()->getDefinition(),
                                     parameters->labelList, 0, 0,
                                     subList->getHead()->getInstance()->getDefinition()->getNumVertices(),
                                     TRUE);
                    fclose(outputFile);
                }
                
                if (iteration < parameters->iterations)
                {                                    // Another iteration?
                    if (parameters->evalMethod == EVAL_SETCOVER)
                    {
                        printf("Removing positive examples covered by");
                        printf(" best substructure.\n\n");
                        parameters->RemovePosEgsCovered(subList->getHead()->getInstance());
                    }
                    else
                        subList->getHead()->getInstance()->CompressFinalGraphs( parameters, iteration,
                                            FALSE);
                    
                    // check for stopping condition
                    // if set-covering, then no more positive examples
                    // if MDL or size, then positive graph contains no edges
                    if (parameters->evalMethod == EVAL_SETCOVER)
                    {
                        if (parameters->numPosEgs == 0)
                        {
                            done = TRUE;
                            printf("Ending iterations - ");
                            printf("all positive examples covered.\n\n");
                        }
                    }
                    else
                    {
                        if (parameters->posGraph->getNumEdges() == 0)
                        {
                            done = TRUE;
                            printf("Ending iterations - graph fully compressed.\n\n");
                        }
                    }
                }
                if ((iteration == parameters->iterations) && (parameters->compress))
                {
                    if (parameters->evalMethod == EVAL_SETCOVER)
                        wr.WriteUpdatedGraphToFile(subList->getHead()->getInstance(), parameters);
                    else
                        wr.WriteCompressedGraphToFile(subList->getHead()->getInstance(), parameters,
                                                   iteration);
                }
            }
            subList->FreeSubList();
            if (parameters->iterations > 1)
            {
                iterationEndTime = time(NULL);
                printf("Elapsed time for iteration %lu = %lu seconds.\n\n",
                       iteration, (iterationEndTime - iterationStartTime));
            }
            iteration++;
        }
    }
    
//    FreeParameters(parameters);
	std::clock_t c_end = std::clock();
	auto t_end = std::chrono::high_resolution_clock::now();

	std::cout << std::fixed << std::setprecision(2) << "CPU time used: "
		<< 1000.0 * (c_end - c_start) / CLOCKS_PER_SEC << " ms\n"
		<< "Wall clock time passed: "
		<< std::chrono::duration<double, std::milli>(t_end - t_start).count()
		<< " ms\n";
    return 0;
}

//---------------------------------------------------------------------------
// NAME:   ISubdue
//
// INPUTS: (Parameters *parameters)
//
// RETURN: void
//
// PURPOSE: Perform incremental discovery.
//---------------------------------------------------------------------------

void Subdue::ISubdue(Params *parameters)
{
    FILE *outputFile;
    BOOLEAN done;
    BOOLEAN ignoreBoundary = FALSE;
    BOOLEAN newData = FALSE;
    ULONG incrementCount = 0;
    ULONG sizeOfPosIncrement;
    ULONG sizeOfNegIncrement;
    Increment *increment;
    InstanceList *boundaryInstances = NULL;
    SubList *localSubList = NULL;
    SubList *globalSubList = NULL;
    Discoverer d;
    GraphWriter wr;
    
    if (parameters->evalMethod == EVAL_SETCOVER)
        ignoreBoundary = TRUE;
    
    while(TRUE)
    {
        // Get next batch of data, either dependent or independent
        newData = parameters->GetNextIncrement();
        if (!newData)
            break;
        
        PostProcessParameters(parameters);
        PrintParameters(parameters);
        
        increment = parameters->GetCurrentIncrement();
        parameters->posGraphSize +=
        (increment->getNumPosVertices() + increment->getNumPosEdges());
        
        if (parameters->evalMethod == EVAL_SETCOVER)
            parameters->SetIncrementNumExamples();
        else
        {
            // We have to set the size here instead of in evaluate, otherwise
            // it gets reset at each iteration to the compressed graph size
            sizeOfPosIncrement =parameters->GetIncrement(parameters->GetCurrentIncrementNum())->IncrementSize(POS);
            
            sizeOfNegIncrement =parameters->GetIncrement(parameters->GetCurrentIncrementNum())->IncrementSize(NEG);
        }
        
        printf("Increment #%lu: %lu positive vertices, %lu positive edges\n",
               incrementCount+1, increment->getNumPosVertices(), increment->getNumPosEdges());
        printf("Accumulated Positive Graph Size: %lu vertices, %lu edges\n",
               parameters->posGraph->getNumVertices(), parameters->posGraph->getNumEdges());
        if (parameters->negGraph != NULL)
        {
            printf("Increment #%lu: %lu negative vertices, %lu negative edges\n",
                   incrementCount+1, increment->getNumNegVertices(), increment->getNumNegEdges());
            printf("Accumulated Negative Graph Size: %lu vertices, %lu edges\n",
                   parameters->negGraph->getNumVertices(), parameters->negGraph->getNumEdges());
        }
        
        done = FALSE;
        printf("%lu unique labels\n", parameters->labelList->getNumLabels());
        printf("\n");
        
        // Return the n best subs for this increment
        localSubList = d.DiscoverSubs(parameters);
        if (localSubList->getHead() == NULL)
        {
            printf("No local substructures found.\n\n");
            done = TRUE;
        }
        else
        {
            if (parameters->outputLevel > 1)
            {
                // print local subs
                printf("\n");
                printf("Best %lu local substructures ", localSubList->CountSubs());
                printf("before boundary processing:\n");
                localSubList->PrintSubList( parameters);
            }
            // Compress only supports EVAL_SETCOVER
            if (parameters->compress)      // Compress using best and write to file
            {
                printf("Removing positive examples covered by");
                printf(" best substructure.\n\n");
                wr.WriteUpdatedIncToFile(localSubList->getHead()->getInstance(), parameters);
            }
            
            // Store a copy of the local subs in the current increment data
            // we only keep the instance list for the current and
            // immediate predecessor increments
            parameters->StoreSubs(localSubList);
            
            // Compute the best overall subs for all increments,
            // before boundary evaluation
            // We do this so we have a more accurate list of best subs for the
            // boundary evaluation, although it is not technically necessary
            globalSubList = parameters->ComputeBestSubstructures( 0);
            if (!ignoreBoundary)
            {
                if (globalSubList != NULL)
                {
                    boundaryInstances =
                    globalSubList->EvaluateBoundaryInstances( parameters);
                    globalSubList->FreeSubList();
                }
                
                if (parameters->outputLevel > 1)
                {
                    if ((boundaryInstances != NULL) &&
                        (boundaryInstances->getHead() != NULL))
                    {
                        printf("Boundary instances found:\n");
                        boundaryInstances->PrintInstanceList(parameters->posGraph,
                                          parameters->labelList);
                    }
                    // NOTE: The following is commented out because with the current
                    //       design, this statement will be true even when there
                    //       are boundary instances.  This design needs to be
                    //       addressed later, but the rest of the boundary processing
                    //       logic does still work as designed.
                    //else
                    //   printf("No Boundary Instances Were Found.\n");
                    
                    printf("\n");
                    printf("Best %lu local substructures ",
                           increment->getSubList()->CountSubs());
                    printf("after boundary evaluation:\n");
                    increment->getSubList()->PrintStoredSubList( parameters);
                }
                
                // Recompute the globally best subs after the boundary instances
                // have been recovered
                globalSubList = parameters->ComputeBestSubstructures( 0);
            }
            printf("\nGlobally Best Substructures - Final:\n");
            globalSubList->PrintStoredSubList( parameters);
            globalSubList->FreeSubList();
        }
        
        // write machine-readable output to file, if given
        if (parameters->outputToFile)
        {
            outputFile = fopen(parameters->outFileName, "a");
            if (outputFile == NULL)
            {
                printf("WARNING: unable to write to output file %s, disabling\n",
                       parameters->outFileName);
                parameters->outputToFile = FALSE;
            }
            if (parameters->evalMethod == EVAL_SETCOVER)
            {
                printf("Removing positive examples covered by");
                printf(" best substructure.\n\n");
                parameters->RemovePosEgsCovered(localSubList->getHead()->getInstance());
                wr.WriteGraphToFile(outputFile, localSubList->getHead()->getInstance()->getDefinition(),
                                 parameters->labelList, 0, 0,
                                 localSubList->getHead()->getInstance()->getDefinition()->getNumVertices(),
                                 TRUE);
            }
            else
                wr.WriteGraphToFile(outputFile, localSubList->getHead()->getInstance()->getDefinition(),
                                 parameters->labelList, 0, 0,
                                 localSubList->getHead()->getInstance()->getDefinition()->getNumVertices(),
                                 TRUE);
            fclose(outputFile);
        }
        incrementCount++;
    }
}


//---------------------------------------------------------------------------
// NAME: GetParameters
//
// INPUTS: (int argc) - number of command-line arguments
//         (char *argv[]) - array of command-line argument strings
//
// RETURN: (Parameters *)
//
// PURPOSE: Initialize parameters structure and process command-line
//          options.
//---------------------------------------------------------------------------

Params *Subdue::GetParameters(int argc, char *argv[])
{
    Params *parameters;
    int i;
    double doubleArg;
    ULONG ulongArg;
    BOOLEAN limitSet = FALSE;
    FILE *outputFile;
    
    parameters = (Params *) malloc(sizeof(Params));
    if (parameters == NULL)
        utility::OutOfMemoryError("parameters");
    
    // initialize default parameter settings
    parameters->directed = TRUE;
    parameters->limit = 0;
    parameters->numBestSubs = 3;
    parameters->beamWidth = 4;
    parameters->valueBased = FALSE;
    parameters->prune = FALSE;
    strcpy(parameters->outFileName, "none");
    parameters->outputToFile = FALSE;
    parameters->outputLevel = 2;
    parameters->allowInstanceOverlap = FALSE;
    parameters->threshold = 0.0;
    parameters->evalMethod = EVAL_MDL;
    parameters->iterations = 1;
    strcpy(parameters->psInputFileName, "none");
    parameters->predefinedSubs = FALSE;
    parameters->minVertices = 1;
    parameters->maxVertices = 0; // i.e., infinity
    parameters->recursion = FALSE;
    parameters->variables = FALSE;
    parameters->relations = FALSE;
    parameters->incremental = FALSE;
    parameters->compress = FALSE;
    
    if (argc < 2)
    {
        fprintf(stderr, "input graph file name must be supplied\n");
        exit(1);
    }
    
    // process command-line options
    i = 1;
    while (i < (argc - 1))
    {
        if (strcmp(argv[i], "-beam") == 0)
        {
            i++;
            sscanf(argv[i], "%lu", &ulongArg);
            if (ulongArg == 0)
            {
                fprintf(stderr, "%s: beam must be greater than zero\n", argv[0]);
                exit(1);
            }
            parameters->beamWidth = ulongArg;
        }
        else if (strcmp(argv[i], "-compress") == 0)
        {

            parameters->compress = TRUE;
        }
        else if (strcmp(argv[i], "-eval") == 0)
        {
            i++;
            sscanf(argv[i], "%lu", &ulongArg);
            if ((ulongArg < 1) || (ulongArg > 3))
            {
                fprintf(stderr, "%s: eval must be 1-3\n", argv[0]);
                exit(1);
            }
            parameters->evalMethod = ulongArg;
        }
        else if (strcmp(argv[i], "-inc") == 0)
        {
            parameters->incremental = TRUE;
        }
        else if (strcmp(argv[i], "-iterations") == 0)
        {
            i++;
            sscanf(argv[i], "%lu", &ulongArg);
            parameters->iterations = ulongArg;
        }
        else if (strcmp(argv[i], "-limit") == 0)
        {
            i++;
            sscanf(argv[i], "%lu", &ulongArg);
            if (ulongArg == 0)
            {
                fprintf(stderr, "%s: limit must be greater than zero\n", argv[0]);
                exit(1);
            }
            parameters->limit = ulongArg;
            limitSet = TRUE;
        }
        else if (strcmp(argv[i], "-maxsize") == 0)
        {
            i++;
            sscanf(argv[i], "%lu", &ulongArg);
            if (ulongArg == 0)
            {
                fprintf(stderr, "%s: maxsize must be greater than zero\n", argv[0]);
                exit(1);
            }
            parameters->maxVertices = ulongArg;
        }
        else if (strcmp(argv[i], "-minsize") == 0)
        {
            i++;
            sscanf(argv[i], "%lu", &ulongArg);
            if (ulongArg == 0)
            {
                fprintf(stderr, "%s: minsize must be greater than zero\n", argv[0]);
                exit(1);
            }
            parameters->minVertices = ulongArg;
        }
        else if (strcmp(argv[i], "-nsubs") == 0)
        {
            i++;
            sscanf(argv[i], "%lu", &ulongArg);
            if (ulongArg == 0)
            {
                fprintf(stderr, "%s: nsubs must be greater than zero\n", argv[0]);
                exit(1);
            }
            parameters->numBestSubs = ulongArg;
        }
        else if (strcmp(argv[i], "-out") == 0)
        {
            i++;
            strcpy(parameters->outFileName, argv[i]);
            parameters->outputToFile = TRUE;
        }
        else if (strcmp(argv[i], "-output") == 0)
        {
            i++;
            sscanf(argv[i], "%lu", &ulongArg);
            if ((ulongArg < 1) || (ulongArg > 5))
            {
                fprintf(stderr, "%s: output must be 1-5\n", argv[0]);
                exit(1);
            }
            parameters->outputLevel = ulongArg;
        }
        else if (strcmp(argv[i], "-overlap") == 0)
        {
            parameters->allowInstanceOverlap = TRUE;
        }
        else if (strcmp(argv[i], "-prune") == 0)
        {
            parameters->prune = TRUE;
        }
        else if (strcmp(argv[i], "-ps") == 0)
        {
            i++;
            strcpy(parameters->psInputFileName, argv[i]);
            parameters->predefinedSubs = TRUE;
        }
        else if (strcmp(argv[i], "-recursion") == 0)
        {
            parameters->recursion = TRUE;
        }
        else if (strcmp(argv[i], "-relations") == 0)
        {
            parameters->relations = TRUE;
            parameters->variables = TRUE; // relations must involve variables
        }
        else if (strcmp(argv[i], "-threshold") == 0)
        {
            i++;
            sscanf(argv[i], "%lf", &doubleArg);
            if ((doubleArg < (double) 0.0) || (doubleArg > (double) 1.0))
            {
                fprintf(stderr, "%s: threshold must be 0.0-1.0\n", argv[0]);
                exit(1);
            }
            parameters->threshold = doubleArg;
        }
        else if (strcmp(argv[i], "-undirected") == 0)
        {
            parameters->directed = FALSE;
        }
        else if (strcmp(argv[i], "-valuebased") == 0)
        {
            parameters->valueBased = TRUE;
        }
        else if (strcmp(argv[i], "-variables") == 0)
        {
            parameters->variables = TRUE;
        }
        else
        {
            fprintf(stderr, "%s: unknown option %s\n", argv[0], argv[i]);
            exit(1);
        }
        i++;
    }
    
    if (parameters->iterations == 0)
        parameters->iterations = MAX_UNSIGNED_LONG; // infinity
    
    // initialize log2Factorial[0..1]
    parameters->log2Factorial = (double *) malloc(2 * sizeof(double));


    if (parameters->log2Factorial == NULL)
        utility::OutOfMemoryError("GetParameters:parameters->log2Factorial");
    parameters->log2FactorialSize = 2;
    parameters->log2Factorial[0] = 0; // lg(0!)
    parameters->log2Factorial[1] = 0; // lg(1!)
    
    // read graphs from input file
    strcpy(parameters->inputFileName, argv[argc - 1]);
    parameters->labelList = LabelList::AllocateLabelList();
         

    parameters->posGraph = NULL;
    parameters->negGraph = NULL;
    parameters->numPosEgs = 0;
    parameters->numNegEgs = 0;
    parameters->posEgsVertexIndices = NULL;
    parameters->negEgsVertexIndices = NULL;
    
    if (parameters->incremental)
    {
        if (parameters->predefinedSubs)
        {
            fprintf(stderr, "Cannot process predefined examples incrementally");
            exit(1);
        }
        
        if (parameters->evalMethod == EVAL_MDL)
        {
            fprintf(stderr, "Incremental SUBDUE does not support EVAL_MDL, ");
            fprintf(stderr, "switching to EVAL_SIZE\n");
            parameters->evalMethod = EVAL_SIZE;
        }
        
        if ((parameters->evalMethod == EVAL_SIZE) && (parameters->compress))
        {
            fprintf(stderr, "Incremental SUBDUE does not support compression, ");
            fprintf(stderr, "with EVAL_SIZE, turning compression off\n");
            parameters->compress = FALSE;
        }
        
        if (parameters->iterations > 1)
        {
            fprintf(stderr,
                    "Incremental SUBDUE only one iteration, setting to 1\n");
            parameters->iterations = 1;
        }
    }
    else
    {

        parameters->ReadInputFile();
        if (parameters->evalMethod == EVAL_MDL)
        {
            parameters->posGraphDL = parameters->posGraph->MDL(parameters->labelList->getNumLabels(), parameters);
            if (parameters->negGraph != NULL)
            {
                parameters->negGraphDL =parameters->negGraph->MDL( parameters->labelList->getNumLabels(),
                    parameters);
            }
        }
    }
    
    // read predefined substructures
    parameters->numPreSubs = 0;
    if (parameters->predefinedSubs)
        parameters->ReadPredefinedSubsFile();

    parameters->incrementList = new IncrementList();
    parameters->incrementList->setHead(NULL) ;
    
    if (parameters->incremental)
    {
        parameters->vertexList = new InstanceVertexList();
        parameters->vertexList->setHead(new AvlTreeList());
        parameters->vertexList->getHead()->setHead(NULL);
    }
    
    // create output file, if given
    if (parameters->outputToFile)
    {
        outputFile = fopen(parameters->outFileName, "w");
        if (outputFile == NULL)
        {
            printf("ERROR: unable to write to output file %s\n",
                   parameters->outFileName);
            exit(1);
        }
        fclose(outputFile);
    }
    
    return parameters;
}


//---------------------------------------------------------------------------
// NAME: PostProcessParameters
//
// INPUTS: (Parameters *parameters)
//
// RETURN: (void)
//
// PURPOSE: Initialize parts of the parameters structure.  This must be called
//          after graph data is obtained.
//
// NOTE:    This code was separated from the other parametric processing
//          because of the need to handle some parameters differentely when
//          the incremental version of SUBDUE is executed.
//---------------------------------------------------------------------------

Params *Subdue::PostProcessParameters(Params *parameters)
{
    Increment *increment = NULL;
    
    if (parameters->incremental)
        increment = parameters->GetCurrentIncrement();
    
    // Code from this point until end of function was moved from GetParameters
    if (parameters->numPosEgs == 0)
    {
        fprintf(stderr, "ERROR: no positive graphs defined\n");
        exit(1);
    }
    
    // Check bounds on discovered substructures' number of vertices
    if (parameters->maxVertices == 0)
        parameters->maxVertices = parameters->posGraph->getNumVertices();
    if (parameters->maxVertices < parameters->minVertices)
    {
        fprintf(stderr, "ERROR: minsize exceeds maxsize\n");
        exit(1);
    }
    
    // Set limit accordingly
    if (parameters->limit == 0)
    {
        if (parameters->incremental)
            parameters->limit = increment->getNumPosEdges() / 2;
        else
            parameters->limit = parameters->posGraph->getNumEdges() / 2;
    }
    
    return parameters;
}


//---------------------------------------------------------------------------
// NAME: PrintParameters
//
// INPUTS: (Parameters *parameters)
//
// RETURN: (void)
//
// PURPOSE: Print selected parameters.
//---------------------------------------------------------------------------

void Subdue::PrintParameters(Params *parameters)
{
    printf("Parameters:\n");
    printf("  Input file..................... %s\n",parameters->inputFileName);
    printf("  Predefined substructure file... %s\n",parameters->psInputFileName);
    printf("  Output file.................... %s\n",parameters->outFileName);
    printf("  Beam width..................... %lu\n",parameters->beamWidth);
    printf("  Compress....................... ");
    utility::PrintBoolean(parameters->compress);
    printf("  Evaluation method.............. ");
    switch(parameters->evalMethod)
    {
        case 1: printf("MDL\n"); break;
        case 2: printf("size\n"); break;
        case 3: printf("setcover\n"); break;
    }
    printf("  'e' edges directed............. ");
    utility::PrintBoolean(parameters->directed);
    printf("  Incremental.................... ");
    utility::PrintBoolean(parameters->incremental);
    printf("  Iterations..................... ");
    if (parameters->iterations == 0)
        printf("infinite\n");
    else
        printf("%lu\n", parameters->iterations);
    printf("  Limit.......................... %lu\n", parameters->limit);
    printf("  Minimum size of substructures.. %lu\n", parameters->minVertices);
    printf("  Maximum size of substructures.. %lu\n", parameters->maxVertices);
    printf("  Number of best substructures... %lu\n", parameters->numBestSubs);
    printf("  Output level................... %lu\n", parameters->outputLevel);
    printf("  Allow overlapping instances.... ");
    utility::PrintBoolean(parameters->allowInstanceOverlap);
    printf("  Prune.......................... ");
    utility::PrintBoolean(parameters->prune);
    printf("  Threshold...................... %lf\n", parameters->threshold);
    printf("  Value-based queue.............. ");
    utility::PrintBoolean(parameters->valueBased);
    printf("  Recursion...................... ");
    utility::PrintBoolean(parameters->recursion);
    printf("\n");
    
    printf("Read %lu total positive graphs\n", parameters->numPosEgs);
    if (parameters->numNegEgs > 0)
        printf("Read %lu total negative graphs\n", parameters->numNegEgs);
    if (parameters->numPreSubs > 0)
        printf("Read %lu predefined substructures\n", parameters->numPreSubs);
    printf("\n");
}


//---------------------------------------------------------------------------
// NAME: FreeParameters
//
// INPUTS: (Parameters *parameters)
//
// RETURN: (void)
//
// PURPOSE: Free memory allocated for parameters.  Note that the
//          predefined substructures are de-allocated as soon as they are
//          processed, and not here.
//---------------------------------------------------------------------------
//
//void FreeParameters(Parameters *parameters)
//{
//    FreeGraph(parameters->posGraph);
//    FreeGraph(parameters->negGraph);
//    FreeLabelList(parameters->labelList);
//    free(parameters->posEgsVertexIndices);
//    free(parameters->negEgsVertexIndices);
//    free(parameters->log2Factorial);
//    free(parameters);
//}
