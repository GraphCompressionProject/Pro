//
//  GraphWriter.cpp
//  Subdue_v1
//
//  Created by Macbook on 24/02/2019.
//  Copyright © 2019 Macbook. All rights reserved.
//
#include "stdafx.h"
#include "GraphWriter.hpp"
#include "Params.hpp"
#include "LabeledGraph.hpp"
#include "Subdue.hpp"
#include "InstanceList.hpp"
#include "LabelList.hpp"
#include "Substructure.hpp"
#include "Increment.hpp"
#include "Compressor.hpp"


//---------------------------------------------------------------------------
// dot.c
//
// Functions for writing Subdue graphs in dot format.  The dot format
// is part of AT&T Labs GraphViz project.
//
// Subdue 5
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// NAME: WriteGraphToDotFile
//
// INPUTS: (char *dotFileName) - file to write dot format
//         (Parameters *parameters)
//
// RETURN: (void)
//
// PURPOSE: Writes the positive and negative (if exists) graphs from
// parameters to dotFileName in dot format.  The positive graph is written
// in black, and the negative graph is written in red.
//---------------------------------------------------------------------------

void GraphWriter::WriteGraphToDotFile(char *dotFileName, Params *parameters)
{
    FILE *dotFile;
    ULONG v;
    ULONG e;
    LabeledGraph *posGraph;
    LabeledGraph *negGraph;
    ULONG vertexOffset;
    
    // parameters used
    LabelList *labelList = parameters->labelList;
    posGraph = parameters->posGraph;
    negGraph = parameters->negGraph;
    
    // open dot file for writing
    dotFile = fopen(dotFileName, "w");
    if (dotFile == NULL)
    {
        printf("ERROR: unable to write to dot output file %s\n", dotFileName);
        exit (1);
    }
    
    // write beginning of dot file
    fprintf(dotFile, "// Subdue %s graph in dot format\n\n",SUBDUE_VERSION);
    fprintf(dotFile, "digraph SubdueGraph {\n");
    
    // write positive graph to dot file
    vertexOffset = 0;
    for (v = 0; v < posGraph->getNumVertices(); v++)
        WriteVertexToDotFile(dotFile, v, vertexOffset, posGraph, labelList,
                             "black");
    for (e = 0; e < posGraph->getNumEdges(); e++)
        WriteEdgeToDotFile(dotFile, e, vertexOffset, posGraph, labelList,
                           "black");
    
    // write negative graph to dot file
    if (negGraph != NULL)
    {
        vertexOffset = posGraph->getNumVertices();
        for (v = 0; v < negGraph->getNumVertices(); v++)
            WriteVertexToDotFile(dotFile, v, vertexOffset, negGraph, labelList,"red");
        for (e = 0; e < negGraph->getNumEdges(); e++)
            WriteEdgeToDotFile(dotFile, e, vertexOffset, negGraph, labelList,
                               "red");
    }
    
    // write end of dot file
    fprintf(dotFile, "}\n");
    
    fclose(dotFile);
}


//---------------------------------------------------------------------------
// NAME: WriteGraphWithInstancesToDotFile
//
// INPUTS: (char *dotFileName) - file to write dot format
//         (Graph *graph) - graph to write
//         (InstanceList *instanceList) - instances to highlight in graph
//         (Parameters *parameters)
//
// RETURN: (void)
//
// PURPOSE: Write graph to dotFileName in dot format, highlighting
// instances in blue.
//---------------------------------------------------------------------------

void GraphWriter::WriteGraphWithInstancesToDotFile(char *dotFileName, LabeledGraph *graph,
                                      InstanceList *instanceList,
                                      Params *parameters)
{
    FILE *dotFile;
    InstanceListNode *instanceListNode;
    Instance *instance;
    ULONG i;
    ULONG v;
    ULONG e;
    ULONG vertexOffset;
    
    // parameters used
    LabelList *labelList = parameters->labelList;
    
    // open dot file for writing
    dotFile = fopen(dotFileName, "w");
    if (dotFile == NULL)
    {
        printf("ERROR: unable to write to dot output file %s\n", dotFileName);
        exit (1);
    }
    
    // write beginning of dot file
    fprintf(dotFile, "// Subdue %s graph in dot format\n\n", SUBDUE_VERSION);
    fprintf(dotFile, "digraph SubdueGraph {\n");
    
    vertexOffset = 0; // always zero for writing just one graph
    // first write instances of graph to dot file
    i = 0;
    instanceListNode = (InstanceListNode *)instanceList->getHead();
    while (instanceListNode != NULL)
    {
        instance = instanceListNode->getInstance();
        for (v = 0; v < instance->getNumVertices(); v++)
            WriteVertexToDotFile(dotFile, instance->getVertices()[v], vertexOffset,
                                 graph, labelList, "blue");
        for (e = 0; e < instance->getNumEdges(); e++)
            WriteEdgeToDotFile(dotFile, instance->getEdges()[e], vertexOffset,
                               graph, labelList, "blue");
        graph->MarkInstanceVertices(instance, TRUE);
        graph->MarkInstanceEdges(instance, TRUE);
        instanceListNode = (InstanceListNode *)instanceListNode->getNext();
        i++;
    }
    
    // write rest of graph to dot file
    for (v = 0; v < graph->getNumVertices(); v++)
        if (! graph->getVertices()[v].isUsed())
            WriteVertexToDotFile(dotFile, v, vertexOffset, graph, labelList,
                                 "black");
    for (e = 0; e < graph->getNumEdges(); e++)
        if (! graph->getEdges()[e].isUsed())
            WriteEdgeToDotFile(dotFile, e, vertexOffset, graph, labelList,
                               "black");
    
    // write end of dot file
    fprintf(dotFile, "}\n");
    fclose(dotFile);
    
    // unmark instance vertices and edges
    instanceListNode =(InstanceListNode *) instanceList->getHead();
    while (instanceListNode != NULL)
    {
        instance = instanceListNode->getInstance();
        graph->MarkInstanceVertices(instance, FALSE);
        graph->MarkInstanceEdges(instance, FALSE);
        instanceListNode = (InstanceListNode *)instanceListNode->getNext();
    }
}


//---------------------------------------------------------------------------
// NAME: WriteSubsToDotFile
//
// INPUTS: (char *dotFileName) - file to write dot format
//         (Graph **subGraphs) - substructure graphs to write
//         (ULONG numSubGraphs) - number of substructures to write
//
// RETURN: (void)
//
// PURPOSE: Write given substructure graphs in dot format to
// dotFileName.  Substructures are defined as dot subgraphs, and their
// IDs must begin with "cluster" in order to support edges between
// substructures.
//---------------------------------------------------------------------------

void GraphWriter::WriteSubsToDotFile(char *dotFileName, LabeledGraph **subGraphs,
                        ULONG numSubGraphs, Params *parameters)
{
    FILE *dotFile;
    LabeledGraph *graph;
    ULONG *subVertexIndices;
    ULONG vertexOffset;
    ULONG subNumber;
    ULONG i;
    ULONG v;
    ULONG e;
    
    // parameters used
    LabelList *labelList = parameters->labelList;
    
    // open dot file for writing
    dotFile = fopen(dotFileName, "w");
    if (dotFile == NULL)
    {
        printf("ERROR: unable to write to dot output file %s\n", dotFileName);
        exit(1);
    }
    
    // allocate array to hold last vertex number of each sub-graph
    subVertexIndices = (ULONG *) malloc(sizeof(ULONG) * numSubGraphs);
    if (subVertexIndices == NULL)
        utility::OutOfMemoryError("WriteSubsToDotFile:subVertexIndices");
    
    // write beginning of dot file
    fprintf(dotFile, "// Subdue %s output in dot format\n\n", SUBDUE_VERSION);
    fprintf(dotFile, "digraph Subdue {\n\n");
    fprintf(dotFile, "  compound=true;\n\n"); // allows edges between subs
    
    vertexOffset = 0;
    // write each sub-graph to dot file
    for (i = 0; i < numSubGraphs; i++) {
        
        fprintf(dotFile, "subgraph cluster_%s_%lu {\n",
                SUB_LABEL_STRING, (i + 1));
        graph = subGraphs[i];
        for (v = 0; v < graph->getNumVertices(); v++)
            WriteVertexToDotFile(dotFile, v, vertexOffset, graph, labelList,
                                 "black");
        for (e = 0; e < graph->getNumEdges(); e++)
            WriteEdgeToDotFile(dotFile, e, vertexOffset, graph, labelList,
                               "black");
        fprintf(dotFile, "  label=\"%s_%lu\";\n}\n",
                SUB_LABEL_STRING, (i + 1));
        
        // add edges from other sub-graphs, if any
        for (v = 0; v < graph->getNumVertices(); v++)
        {
            subNumber = labelList->SubLabelNumber(graph->getVertices()[v].getLabel());
            if (subNumber > 0)
            { // then valid reference to previous sub
                // write edge between substructure cluster sub-graphs
                fprintf(dotFile, "%lu -> %lu ",
                        subVertexIndices[subNumber - 1], (vertexOffset + 1));
                fprintf(dotFile, "[ltail=cluster_%s_%lu,lhead=cluster_%s_%lu];\n",
                        SUB_LABEL_STRING, subNumber, SUB_LABEL_STRING, (i + 1));
            }
        }
        fprintf(dotFile, "\n");
        vertexOffset += graph->getNumVertices();
        subVertexIndices[i] = vertexOffset; // set index to last vertex of sub
    }
    
    // write end of dot file
    fprintf(dotFile, "}\n");
    
    fclose(dotFile);
}


//---------------------------------------------------------------------------
// NAME: WriteVertexToDotFile
//
// INPUTS: (FILE *dotFile) - file stream for writing dot output
//         (ULONG v) - vertex index in graph to write
//         (ULONG vertexOffset) - offset for v's printed vertex number
//         (Graph *graph) - graph containing vertex
//         (LabelList *labelList) - graph's labels
//         (char *color) - string indicating color for vertex
//
// RETURN: (void)
//
// PURPOSE: Writes the vertex to the dot file.
//---------------------------------------------------------------------------

void GraphWriter::WriteVertexToDotFile(FILE *dotFile, ULONG v, ULONG vertexOffset,
                          LabeledGraph *graph, LabelList *labelList, const char *color)
{
    fprintf(dotFile, "  %lu [label=\"", (v + vertexOffset + 1));
    WriteLabelToFile(dotFile, graph->getVertices()[v].getLabel(), labelList, TRUE);
    fprintf(dotFile, "\",color=%s,fontcolor=%s];\n", color, color);
}


//---------------------------------------------------------------------------
// NAME: WriteEdgeToDotFile
//
// INPUTS: (FILE *dotFile) - file stream for writing dot output
//         (ULONG e) - edge index in graph to write
//         (ULONG vertexOffset) - offset for vertex's printed numbers
//         (Graph *graph) - graph containing edge
//         (LabelList *labelList) - graph's labels
//         (char *color) - string indicating color for vertex
//
// RETURN: (void)
//
// PURPOSE: Writes the edge to the dot file.
//---------------------------------------------------------------------------

void GraphWriter::WriteEdgeToDotFile(FILE *dotFile, ULONG e, ULONG vertexOffset,
                        LabeledGraph *graph, LabelList *labelList, const char *color)
{
    LabeledEdge *edge;
    
    edge = & graph->getEdges()[e];
    fprintf(dotFile, "  %lu -> %lu [label=\"",
            (edge->getE().first + vertexOffset + 1),
            (edge->getE().second + vertexOffset + 1));
    WriteLabelToFile(dotFile, edge->getLabel(), labelList, TRUE);
    fprintf(dotFile, "\"");
    if (! edge->isDirected())
        fprintf(dotFile, ",arrowhead=none");
    fprintf(dotFile, ",color=%s,fontcolor=%s];\n", color, color);
}



//---------------------------------------------------------------------------
// NAME: WriteLabelToFile
//
// INPUTS: (FILE *outFile) - file to write to
//         (ULONG index) - index of label in label list
//         (LabelList *labelList) - label list
//         (BOOLEAN suppressQuotes) - if TRUE, then delimiting quotes not
//                                    printed
//
// RETURN: (void)
//
// PURPOSE: Write label labelList[index] to given file.  If suppressQuotes
// is TRUE and the label has delimiting end quotes, then do not print them.
//---------------------------------------------------------------------------

void GraphWriter::WriteLabelToFile(FILE *outFile, ULONG index, LabelList *labelList,
                      BOOLEAN suppressQuotes)
{
    unsigned char labelType;
    char labelStr[TOKEN_LEN];
    int strLength;
    int i;
    
    labelType = labelList->getHead()[index].getLAbelType();
    switch(labelType)
    {
        case STRING_LABEL:
            strcpy(labelStr, labelList->getHead()[index].getStringLabel());
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
            fprintf(outFile, "%.*g", NUMERIC_OUTPUT_PRECISION,
                    labelList->getHead()[index].getNumericLabel());
            break;
        default:
            break;
    }
}



//---------------------------------------------------------------------------
// NAME:    WriteGraphToFile
//
// INPUTS:  (FILE *outFile) - file stream to write graph
//          (Graph *graph) - graph to be written
//          (LabelList *labelList) - indexed list of vertex and edge labels
//          (ULONG vOffset) - vertex offset for compressed increments
//          (ULONG start) - beginning of vertex range to print
//          (ULONG finish) - end of vertex range to print
//          (BOOLEAN printPS) - flag indicating output is for predefined sub
//
// RETURN:  void
//
// PURPOSE: Write the vertices and edges of the graph to the given
//          file, prefaced by the SUB_TOKEN defined in subdue.h
//          (when WriteSubToken is TRUE).
//---------------------------------------------------------------------------

void GraphWriter::WriteGraphToFile(FILE *outFile, LabeledGraph *graph, LabelList *labelList,
                      ULONG vOffset, ULONG start, ULONG finish, BOOLEAN printPS)
{
    ULONG v;
    ULONG e;
    LabeledEdge *edge = NULL;
    
    if (graph != NULL)
    {
        if (printPS)
            fprintf(outFile, "%s\n", SUB_TOKEN);
        // write vertices
        for (v = start; v < finish; v++)
        {
            fprintf(outFile, "v %lu ", (v + 1 + vOffset - start));
            WriteLabelToFile(outFile, graph->getVertices()[v].getLabel(), labelList, FALSE);
            fprintf(outFile, "\n");
        }
        // write edges
        for (e = 0; e < graph->getNumEdges(); e++)
        {
            edge = &graph->getEdges()[e];
            if ((edge->getE().first >= start) && (edge->getE().first < finish))
            {
                if (edge->isDirected())
                    fprintf(outFile, "d");
                else
                    fprintf(outFile, "u");
                fprintf(outFile, " %lu %lu ",
                        (edge->getE().first + 1 + vOffset - start),
                        (edge->getE().second + 1 + vOffset - start));
                WriteLabelToFile(outFile, edge->getLabel(), labelList, FALSE);
                fprintf(outFile, "\n");
            }
        }
        if (printPS)
            fprintf(outFile, "\n");
    }
}

//---------------------------------------------------------------------------
// NAME: WriteUpdatedGraphToFile
//
// INPUTS: (Substructure *sub) - substructure whose instances to be removed
//         (Parameters *parameters)
//
// RETURN: (void)
//
// PURPOSE: Write compressed graphs to file, including separators for examples.
//---------------------------------------------------------------------------

void GraphWriter::WriteUpdatedGraphToFile(Substructure *sub, Params *parameters)
{
    FILE *fp;
    char filename[FILE_NAME_LEN];
    ULONG example;
    ULONG numExamples;
    ULONG start;
    ULONG finish;
    
    parameters->RemovePosEgsCovered(sub );
    
    sprintf(filename, "%s.cmp", parameters->inputFileName);
    if (parameters->posGraph != NULL)       // Write compressed graphs to files
    {
        numExamples = parameters->numPosEgs;
        fp = fopen(filename, "w");
        
        // The indices of each example need to be renumbered to start at 1
        for (example = 0; example < numExamples; example++)
        {
            start = parameters->posEgsVertexIndices[example];
            if (example < (numExamples - 1))
                finish = parameters->posEgsVertexIndices[example + 1];
            else
                finish = parameters->posGraph->getNumVertices();
            fprintf(fp, "XP\n");
            WriteGraphToFile(fp, parameters->posGraph, parameters->labelList, 0,
                             start, finish, FALSE);
        }
        fclose(fp);
    }
    
    if (parameters->negGraph != NULL)
    {
        numExamples = parameters->numNegEgs;
        fp = fopen(filename, "a");
        
        // The indices of each example need to be renumbered to start at 1
        for (example = 0; example < numExamples; example++)
        {
            start = parameters->negEgsVertexIndices[example];
            if (example < (numExamples - 1))
                finish = parameters->negEgsVertexIndices[example + 1];
            else
                finish = parameters->negGraph->getNumVertices();
            fprintf(fp, "XN\n");
            WriteGraphToFile(fp, parameters->negGraph, parameters->labelList, 0,
                             start, finish, FALSE);
        }
        fclose(fp);
    }
}


//---------------------------------------------------------------------------
// NAME: WriteUpdatedIncToFile
//
// INPUTS: (Substructure *sub) - substructure whose instances to be removed
//         (Parameters *parameters)
//
// RETURN: (void)
//
// PURPOSE: Write compressed graphs to file, including separators for examples.
//---------------------------------------------------------------------------

void GraphWriter::WriteUpdatedIncToFile(Substructure *sub, Params *parameters)
{
    FILE *fp;
    char filename[FILE_NAME_LEN];
    ULONG example;
    ULONG numExamples;
    ULONG start = 0;
    ULONG finish;
    ULONG i;
    ULONG first = 0;
    Increment *increment = parameters->GetCurrentIncrement();
    
    if (parameters->incremental)
        first = increment->getStartPosVertexIndex();
    parameters->MarkPosEgsCovered(sub);
    
    sprintf(filename, "%s-com_%lu.g", parameters->inputFileName,
            parameters->GetCurrentIncrementNum());
    if (parameters->posGraph != NULL)       // Write compressed graphs to files
    {
        numExamples = parameters->numPosEgs;
        fp = fopen(filename, "w");
        
        // The indices of each example need to be renumbered to start at 1
        for (example = 0; example < numExamples; example++)
        {
            start = parameters->posEgsVertexIndices[example];
            if (start >= first)
            {
                if (example < (numExamples - 1))
                    finish = parameters->posEgsVertexIndices[example + 1];
                else
                    finish = parameters->posGraph->getNumVertices();
                // Only write positive examples not covered by substructure
                if (!parameters->posGraph->getVertices()[start].isUsed())
                {
                    fprintf(fp, "XP\n");
                    WriteGraphToFile(fp, parameters->posGraph, parameters->labelList,
                                     0, start, finish, FALSE);
                }
            }
        }
        fclose(fp);
    }
    
    if (parameters->incremental)
        first = increment->getStartNegVertexIndex();
    
    if (parameters->negGraph != NULL)
    {
        numExamples = parameters->numNegEgs;
        fp = fopen(filename, "a");
        
        // The indices of each example need to be renumbered to start at 1
        for (example = 0; example < numExamples; example++)
        {
            start = parameters->negEgsVertexIndices[example];
            if (start >= first)
            {
                if (example < (numExamples - 1))
                    finish = parameters->negEgsVertexIndices[example + 1];
                else
                    finish = parameters->negGraph->getNumVertices();
                fprintf(fp, "XN\n");
                WriteGraphToFile(fp, parameters->negGraph, parameters->labelList,
                                 0, start, finish, FALSE);
            }
        }
        fclose(fp);
    }
    
    // Unmark covered vertices
    for (i=start; i<parameters->posGraph->getNumVertices(); i++)
        parameters->posGraph->getVertices()[i].setUsed(FALSE) ;
}

//---------------------------------------------------------------------------
// NAME: WriteCompressedGraphToFile
//
// INPUTS: (Substructure *sub) - substructure for compression
//         (Parameters *parameters) - contains graphs and label list
//         (ULONG iteration) - SUBDUE iteration after which compressing
//
// RETURN: void
//
// PURPOSE: Compresses the positive and negative (if defined) graphs
// with the given substructure, then writes the resulting graph to a file.
//---------------------------------------------------------------------------

void GraphWriter::WriteCompressedGraphToFile(Substructure *sub, Params *parameters,
                                ULONG iteration)
{
    FILE *fp = NULL;
    LabeledGraph *compressedPosGraph;
    LabeledGraph *compressedNegGraph;
    char subLabelString[TOKEN_LEN];
    char overlapLabelString[TOKEN_LEN];
    char filename[FILE_NAME_LEN];
    Label label;
    Compressor c;
    // parameters used
    LabeledGraph *posGraph              = parameters->posGraph;
    LabeledGraph *negGraph              = parameters->negGraph;
    BOOLEAN allowInstanceOverlap = parameters->allowInstanceOverlap;
    LabelList *labelList         = parameters->labelList;
    
    compressedPosGraph = posGraph;
    compressedNegGraph = negGraph;
    
    if (sub->getNumInstances() > 0)
        compressedPosGraph = c.CompressGraph(posGraph, sub->getInstances(), parameters);
    if (sub->getNumNegInstances() > 0)
        compressedNegGraph = c.CompressGraph(negGraph, sub->getNegInstances(),
                                           parameters);
      

    // add "SUB" and "OVERLAP" (if used) labels to label list
    sprintf(subLabelString, "%s_%lu", SUB_LABEL_STRING, iteration);
    
    label.setLabelType(STRING_LABEL) ;
    label.setStringLabel(subLabelString) ;
    labelList->StoreLabel(&label);
    if (allowInstanceOverlap &&
        ((sub->getInstances()->InstancesOverlap()) ||
         (sub->getNegInstances()->InstancesOverlap())))
    {
        sprintf(overlapLabelString, "%s_%lu", OVERLAP_LABEL_STRING,
                iteration);
        label.setStringLabel(overlapLabelString) ;
        labelList->StoreLabel(&label);
    }
    
    if (compressedPosGraph != NULL)         // Write compressed graphs to files
    {
        sprintf(filename, "%s.cmp", parameters->inputFileName);
        fp = fopen(filename, "w");
        // Positive examples have been combined into one file
        fprintf(fp, "XP\n");
        WriteGraphToFile(fp, compressedPosGraph, parameters->labelList,
                         0, 0, compressedPosGraph->getNumVertices(), FALSE);
    }
    if (compressedNegGraph != NULL)
    {
        if (fp == NULL)
        {
            sprintf(filename, "%s.cmp", parameters->inputFileName);
            fp = fopen(filename, "w");
        }
        // Negative examples have been combined into one file
        fprintf(fp, "XN\n");
        WriteGraphToFile(fp, compressedNegGraph, parameters->labelList,
                         0, 0, compressedNegGraph->getNumVertices(), FALSE);
    }
    if (fp != NULL)
        fclose(fp);
}


































