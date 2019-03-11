//
//  mdl_main.hpp
//  Subdue_v1
//
//  Created by Macbook on 28/02/2019.
//  Copyright © 2019 Macbook. All rights reserved.
//

#ifndef mdl_main_hpp
#define mdl_main_hpp

#include <stdio.h>


#include "Params.hpp"
#include "InstanceList.hpp"
#include "Label.hpp"
#include "LabeledGraph.hpp"
#include "Compressor.hpp"

#include "GraphWriter.hpp"


// Function prototypes

class mdl_main{
    
    
public:
    
    Params *GetParameters(int, char **);
    
    //---------------------------------------------------------------------------
    // NAME:    main
    //
    // INPUTS:  (int argc) - number of arguments to program
    //          (char **argv) - array of strings of arguments to program
    //
    // RETURN:  (int) - 0 if all is well
    //
    // PURPOSE: Main function for standalone subgraph isomorphism.  Takes two
    // command-line arguments, which are the graph file names containing
    // the graphs to be matched.
    //---------------------------------------------------------------------------
    
     int main(int argc, char **argv)
    {
        Params *parameters;
        ULONG numLabels;
        ULONG subLabelIndex;
        LabeledGraph *g1;
        LabeledGraph *g2;
        LabeledGraph *g2compressed;
        InstanceList *instanceList;
        ULONG numInstances;
        ULONG subSize, graphSize, compressedSize;
        double subDL, graphDL, compressedDL;
        double value;
        Label label;
        Compressor c;
        GraphWriter writer;
        
        parameters = GetParameters(argc, argv);
        g1->ReadGraph(argv[argc - 2], parameters->labelList,
                      parameters->directed);
        g2->ReadGraph(argv[argc - 1], parameters->labelList,
                      parameters->directed);
        instanceList = g2->FindInstances(g1, parameters);
        numInstances = instanceList->CountInstances();
        printf("Found %lu instances.\n\n", numInstances);
        g2compressed = c.CompressGraph(g2, instanceList, parameters);
        
        // Compute and print compression-based measures
        subSize = g1->GraphSize();
        graphSize = g2->GraphSize();
        compressedSize = g2compressed->GraphSize();
        value = ((double) graphSize) /
        (((double) subSize) + ((double) compressedSize));
        printf("Size of graph = %lu\n", graphSize);
        printf("Size of substructure = %lu\n", subSize);
        printf("Size of compressed graph = %lu\n", compressedSize);
        printf("Value = %f\n\n", value);
        
        // Compute and print MDL based measures
        numLabels = parameters->labelList->getNumLabels();
        subDL = g1->MDL( numLabels, parameters);
        graphDL = g2->MDL( numLabels, parameters);
        subLabelIndex = numLabels; // index of "SUB" label
        numLabels++; // add one for new "SUB" vertex label
        if ((parameters->allowInstanceOverlap) &&
            (instanceList->InstancesOverlap()))
            numLabels++; // add one for new "OVERLAP" edge label
        compressedDL = g2compressed->MDL( numLabels, parameters);
        // add extra bits to describe where external edges connect to instances
        compressedDL += g2compressed->ExternalEdgeBits( g1, subLabelIndex);
        value = graphDL / (subDL + compressedDL);
        printf("DL of graph = %f\n", graphDL);
        printf("DL of substructure = %f\n", subDL);
        printf("DL of compressed graph = %f\n", compressedDL);
        printf("Value = %f\n\n", value);
        
        if (parameters->outputToFile)
        {
            // first, actually add "SUB" and "OVERLAP" labels
            label.setLabelType(STRING_LABEL) ;
            label.getLabelValue().setStringLabel((char *)SUB_LABEL_STRING);
            parameters->labelList->StoreLabel(& label);
            label.getLabelValue().setStringLabel((char *)OVERLAP_LABEL_STRING) ;
            parameters->labelList->StoreLabel(& label);
            parameters->posGraph = g2compressed;
            writer.WriteGraphToDotFile(parameters->outFileName, parameters);
            printf("Compressed graph written to dot file %s\n",
                   parameters->outFileName);
        }
        
        g2compressed->FreeGraph();
        instanceList->FreeInstanceList();
        g1->FreeGraph();
        g2->FreeGraph();
        
        
        return 0;
    }
};

#endif /* mdl_main_hpp */
