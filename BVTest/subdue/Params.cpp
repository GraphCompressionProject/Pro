//
// Created by Macbook on 11/02/2019.
//
#include "stdafx.h"
#include "Params.hpp"
#include "IncrementListNode.hpp"
#include "LabeledGraph.hpp"
#include "LabelList.hpp"
#include "IncrementList.hpp"
#include "Increment.hpp"
#include "InstanceVertexList.hpp"
#include "AvlTable.hpp"
#include "AvlTableNode.hpp"
#include "utility.hpp"
//globals for this module only




//---------------------------------------------------------------------------
// NAME: GetCurrentIncrementNum
//
// INPUTS:  (Parameters *) - system parameters
//
// RETURN:  (ULONG) - current increment number
//
// PURPOSE: returns the sequential number of the current increment
//---------------------------------------------------------------------------

unsigned long Params::GetCurrentIncrementNum()
{
    IncrementListNode *incNodePtr = incrementList->getHead();

    if (incNodePtr == NULL) //empty list, this is the first increment
        return 0;
    while (incNodePtr->getNext() != NULL)
        incNodePtr = (IncrementListNode *)incNodePtr->getNext();

    return incNodePtr->getInstance()->getIncrementNum();
}

//---------------------------------------------------------------------------
// NAME: GetCurrentIncrement
//
// INPUTS:  (Parameters *) - system parameters
//
// RETURN:  (Increment *) - current increment structure
//
// PURPOSE: returns a pointer to the current increment structure
//---------------------------------------------------------------------------

Increment *Params::GetCurrentIncrement()
{
    unsigned long currentIncrementNum = GetCurrentIncrementNum();
    Increment *increment = GetIncrement(currentIncrementNum);
    if (currentIncrementNum == 0)
        return NULL;
    return increment;
}

//---------------------------------------------------------------------------
// NAME GetIncrement
//
// INPUTS:  (ULONG incrementNum) - current increment
//          (Parameters *parameters) - system parameters
//
// RETURN:  (Increment *) - increment structure
//
// PURPOSE: returns the actual increment structure, with its parameters
//---------------------------------------------------------------------------

Increment *Params::GetIncrement(unsigned long incrementNum)
{
    IncrementListNode *listNode = GetIncrementListNode(incrementNum);
    if (listNode == NULL)
        return NULL;

    return listNode->getInstance();
}

//---------------------------------------------------------------------------
// NAME: GetIncrementListNode
//
// INPUTS:  (ULONG incrementNum) - current increment
//          (Parameters *parameters) - system parameters
//
// RETURN:  (IncrementListNode *) - corresponding increment nocde
//
// PURPOSE: Return a node from the increment list, corresponding to a particular
//          increment number.  Only used internal to this module.
//---------------------------------------------------------------------------

IncrementListNode *Params::GetIncrementListNode(unsigned long  incrementNum)
{
    IncrementListNode *incNodePtr = incrementList->getHead();

    if (incNodePtr == NULL) //empty list, this is the first increment
        return NULL;
    while (incNodePtr->getNext() != NULL)
        incNodePtr = (IncrementListNode *)incNodePtr->getNext();
    return incNodePtr;
}

//---------------------------------------------------------------------------
// NAME: ReadInputFile
//
// INPUTS: (Parameters *parameters)
//
// RETURN: (void)
//
// PURPOSE: Reads in the SUBDUE input file, which may consist of
// positive graphs and/or negative graphs, which are collected into
// the positive and negative graph fields of the parameters.  Each
// example in the input file is prefaced by the appropriate token defined
// in subdue.h.  The first graph in the file is assumed positive
// unless the negative token is present.  Each graph is assumed to
// begin at vertex #1 and therefore examples are not connected to one
// another.
//---------------------------------------------------------------------------

void Params::ReadInputFile()
{
    FILE *inputFile = NULL;
    LabeledGraph *graph = NULL;
    LabeledGraph *posGraph= NULL;
    LabeledGraph *negGraph = NULL;
    LabelList *labelList = NULL;
    ULONG numPosEgs = 0;
    ULONG numNegEgs = 0;
    ULONG *posEgsVertexIndices = NULL;
    ULONG *negEgsVertexIndices = NULL;
    BOOLEAN readingPositive = TRUE;
    ULONG vertexOffset = 0;
    BOOLEAN directed = TRUE;
    ULONG lineNo = 1;
    char token[TOKEN_LEN];
    
    labelList = this->labelList;
    directed = this->directed;
    
    // Open input file
    inputFile = fopen(this->inputFileName,"r");
    if (inputFile == NULL)
    {
        fprintf(stderr, "Unable to open input file %s.\n",
                this->inputFileName);
        exit(1);
    }
    
    // Parse input file
    while (utility::ReadToken(token, inputFile, &lineNo) != 0)
    {

        if (strcmp(token, POS_EG_TOKEN) == 0)
        { // reading positive eg
            if (posGraph == NULL)
                posGraph = LabeledGraph::AllocateGraph(0,0);
            numPosEgs++;
            vertexOffset = posGraph->getNumVertices();
            posEgsVertexIndices = utility::AddVertexIndex(posEgsVertexIndices, numPosEgs, vertexOffset);
            graph = posGraph;
            readingPositive = TRUE;
        }
        else if (strcmp(token, NEG_EG_TOKEN) == 0)
        { // reading negative eg
            if (negGraph == NULL)
                negGraph = LabeledGraph::AllocateGraph(0,0);
            numNegEgs++;
            vertexOffset = negGraph->getNumVertices();
            negEgsVertexIndices = utility::AddVertexIndex(negEgsVertexIndices,
                                                 numNegEgs, vertexOffset);
            graph = negGraph;
            readingPositive = FALSE;
        }
        else if (strcmp(token, "v") == 0)
        {  // read vertex
            if (readingPositive && (posGraph == NULL))
            {
                // first graph starts without positive token, so assumed positive
                posGraph = LabeledGraph::AllocateGraph(0,0);
                numPosEgs++;
                vertexOffset = 0;
                posEgsVertexIndices = utility::AddVertexIndex(posEgsVertexIndices, numPosEgs, vertexOffset);
                graph = posGraph;
            }
        graph->ReadVertex( inputFile, labelList, &lineNo, vertexOffset);

        }
        else if (strcmp(token, "e") == 0)    // read 'e' edge
            graph->ReadEdge( inputFile, labelList, &lineNo, directed, vertexOffset);
        
        else if (strcmp(token, "u") == 0)    // read undirected edge
            graph->ReadEdge(  inputFile, labelList, &lineNo, FALSE, vertexOffset);
        
        else if (strcmp(token, "d") == 0)    // read directed edge
            graph->ReadEdge(  inputFile, labelList, &lineNo, TRUE, vertexOffset);
        
        else
        {
            fclose(inputFile);
            fprintf(stderr, "Unknown token %s in line %lu of input file %s.\n",
                    token, lineNo, this->inputFileName);
            exit(1);
        }
    }
    fclose(inputFile);
    
    //***** trim vertex, edge and label lists
    
    this->posGraph = posGraph;
    this->negGraph = negGraph;
    this->labelList = labelList;
    this->numPosEgs = numPosEgs;
    this->numNegEgs = numNegEgs;
    this->posEgsVertexIndices = posEgsVertexIndices;
    this->negEgsVertexIndices = negEgsVertexIndices;
}


//---------------------------------------------------------------------------
// NAME: ReadPredefinedSubsFile
//
// INPUTS: (Parameters *parameters)
//
// RETURN: (void)
//
// PURPOSE: Reads in one or more graphs from the given file and stores
// these on the predefined substructure list in parameters.  Each
// graph is prefaced by the predefined substructure token defined in
// subdue.h.
//
// Right now, these substructures will be used to compress the graph,
// if present, and therefore any labels not present in the input graph
// will be discarded during compression.  If the predefined
// substructures are ever simply put on the discovery queue, then care
// should be taken to not include labels that do not appear in the
// input graph, as this would bias the MDL computation. (*****)
//---------------------------------------------------------------------------

void Params::ReadPredefinedSubsFile()
{
    ULONG numPreSubs = 0;
    LabeledGraph **preSubs = NULL;
    
    preSubs = ReadSubGraphsFromFile(this->psInputFileName,
                                    PREDEF_SUB_TOKEN,
                                    & numPreSubs);
    this->numPreSubs = numPreSubs;
    this->preSubs = preSubs;
}

//---------------------------------------------------------------------------
// NAME: ReadSubGraphsFromFile
//
// INPUTS: (char *fileName) - file containing substructure graphs
//         (char *subToken) - token used to separate sub graphs in file
//         (ULONG *numSubGraphs) - call-by-reference variable returning
//                                 number of substructure graphs read
//         (Parameters *parameters)
//
// RETURN: (Graph **) - array of substructure graphs
//
// PURPOSE: Reads in one or more graphs from the given file and stores
// them in an array of graphs.  Each graph (except optionally the
// first) is assumed to be prefaced by the given substructure token.
//---------------------------------------------------------------------------

LabeledGraph **Params::ReadSubGraphsFromFile(char *fileName,const char *subToken, ULONG *numSubGraphs)
{
    ULONG numSubs = 0;
    LabeledGraph **subGraphs = NULL;
    FILE *inputFile = NULL;
    LabeledGraph *graph = NULL;
    LabelList *labelList = NULL;
    BOOLEAN directed = TRUE;
    ULONG vertexOffset = 0;   // Dummy argument to ReadVertex and ReadEdge
    ULONG lineNo = 1;
    char token[TOKEN_LEN];
    
    labelList = this->labelList;
    directed = this->directed;
    
    // Open input file
    inputFile = fopen(fileName,"r");
    if (inputFile == NULL)
    {
        fprintf(stderr, "Unable to open input file %s.\n", fileName);
        exit(1);
    }
    
    // Parse input file
    while (utility::ReadToken(token, inputFile, &lineNo) != 0)
    {
        if (strcmp(token, subToken) == 0)
        { // new sub-graph
            numSubs++;
            subGraphs = (LabeledGraph **) realloc(subGraphs, (sizeof(LabeledGraph *) * numSubs));
            if (subGraphs == NULL)
                utility::OutOfMemoryError("ReadSubGraphsFromFile:subGraphs");
            subGraphs[numSubs - 1] = LabeledGraph::AllocateGraph(0, 0);
            graph = subGraphs[numSubs - 1];
        }
        else if (strcmp(token, "v") == 0)
        {        // read vertex
            if (subGraphs == NULL)
            {
                // first sub-graph not preceded by subToken
                numSubs++;
                subGraphs = (LabeledGraph **) realloc(subGraphs,(sizeof(LabeledGraph *) * numSubs));
                if (subGraphs == NULL)
                    utility::OutOfMemoryError("ReadSubGraphsFromFile:subGraphs");
                subGraphs[numSubs - 1] = LabeledGraph::AllocateGraph(0, 0);
                graph = subGraphs[numSubs - 1];
            }
            graph->ReadVertex( inputFile, labelList, &lineNo, vertexOffset);
        }
        else if (strcmp(token, "e") == 0)    // read 'e' edge
            graph->ReadEdge( inputFile, labelList, &lineNo, directed, vertexOffset);
        
        else if (strcmp(token, "u") == 0)    // read undirected edge
            graph->ReadEdge(  inputFile, labelList, &lineNo, FALSE, vertexOffset);
        
        else if (strcmp(token, "d") == 0)    // read directed edge
            graph->ReadEdge(  inputFile, labelList, &lineNo, TRUE, vertexOffset);
        
        else
        {
            fclose(inputFile);
            fprintf(stderr, "Unknown token %s in line %lu of input file %s.\n",
                    token, lineNo, fileName);
            exit(1);
        }
    }
    fclose(inputFile);
    
    //***** trim vertex, edge and label lists
    
    *numSubGraphs = numSubs;
    return subGraphs;
}

//----------------------------------------------------------------------------
// NAME:  InitializeGraph
//
// INPUTS:  (Parameters *parameters)
//
// RETURN:  void
//
// PURPOSE:  Initialize graph variables.
//----------------------------------------------------------------------------

void Params::InitializeGraph()
{
   posGraph = LabeledGraph::AllocateGraph(0,0);
   negGraph = NULL;
    numPosEgs = 0;
    numNegEgs = 0;
    posEgsVertexIndices = NULL;
    negEgsVertexIndices = NULL;
}

//---------------------------------------------------------------------------
// NAME: GetStartVertexIndex
//
// INPUTS:  (ULONG incrementNum) - current increment
//          (Parameters *parameters) - system parameters
//          (ULONG graphType) - POS or NEG
//
// RETURN:  ULONG - starting vertex number for corresponding increment
//
// PURPOSE: Return the index of the starting positive vertex for the increment.
//---------------------------------------------------------------------------

ULONG Params::GetStartVertexIndex(ULONG incrementNum,
                          ULONG graphType)
{
    IncrementListNode *incNodePtr = incrementList->getHead();
    
    if (incNodePtr == NULL) // Empty list, this is the first increment
    {
        printf("Error GetStartVertexIndex: increment list empty\n");
        exit(1);
    }
    while (incNodePtr->getNext() != NULL)
        incNodePtr = (IncrementListNode *)incNodePtr->getNext();
    
    if (graphType == POS)
        return incNodePtr->getInstance()->getStartPosVertexIndex();
    else
        return incNodePtr->getInstance()->getStartNegVertexIndex();
}

//---------------------------------------------------------------------------
// NAME: SetIncrementNumExamples
//
// INPUTS: (Parameters *parameters) - system parameters
//
// RETURN:  void
//
// PURPOSE:  Set the description lengths of an increment.  This is only
//           called for the current increment, so we assume that parameter
//---------------------------------------------------------------------------

void Params::SetIncrementNumExamples()
{
    Increment *increment = GetCurrentIncrement();
    ULONG i;
    ULONG numEgs;
    ULONG start;
    
    start = increment->getStartPosVertexIndex();
    numEgs = numPosEgs;
    for (i=0; i<numEgs; i++)
        if (posEgsVertexIndices[i] >= start)  // Examples in increment
            break;
    increment->setNumPosEgs((double) (numEgs - i)) ;
    
    start = increment->getStartNegVertexIndex();
    numEgs = numNegEgs;
    for (i=0; i<numEgs; i++)
        if (negEgsVertexIndices[i] >= start)  // Examples in increment
            break;
    increment->setNumNegEgs((double) (numEgs - i)) ;
}

//---------------------------------------------------------------------------
// NAME:  StoreSubs
//
// INPUTS:  (SubList *subList) - pointer to list of substructures
//          (Parameters *parameters) - system parameters
//
// RETURN:  void
//
// PURPOSE: Store the local subs discovered in this increment.
//          There is no need to store the actual instances.  We never need them
//          again, only the count.  To deal with overlapping vertices in
//          boundary instances, we need to keep the instances for the current
//          and previous increment.  We can get rid of instances for all other
//          previous increments.
//---------------------------------------------------------------------------

void Params::StoreSubs(SubList *subList)
{
    SubListNode *subListNode;
    ULONG currentIncrement = GetCurrentIncrementNum();
    Increment *increment = GetIncrement(currentIncrement);
    
    increment->setSubList(subList) ;
    
    AddVertexTrees(subList);
    
    subListNode = increment->getSubList()->getHead();
    while(subListNode != NULL)
    {
        subListNode->getInstance()->getInstances()->FreeInstanceList();
        subListNode->getInstance()->setInstances(NULL) ;
        subListNode = (SubListNode *) subListNode->getNext();
    }
}

//---------------------------------------------------------------------------
// NAME:  AddVertexTrees
//
// INPUTS:  (SubList *subList) - pointer to list of substructures
//          (Parameters *parameters) - system parameters
//
// RETURN:  void
//
// PURPOSE:  For the list of subs in this increment, collect all of the vertex
//           indices from the instance lists and store them in an avl tree.
//           Store one avl tree for each sub in corresponding order
//---------------------------------------------------------------------------

void Params::AddVertexTrees(SubList *subList)
{
    SubListNode *subListNode;
    Substructure *sub;
    InstanceListNode *instanceListNode;
    Instance *instance;
    AvlTable* avlTable;
    
    subListNode = subList->getHead();
    while (subListNode != NULL)
    {
        sub = subListNode->getInstance();
        avlTable = sub->GetSubTree(this);
        if (avlTable == NULL)
        {
            avlTable = AvlTable::avl_create((avl_comparison_func *)utility::compare_ints, NULL, NULL);
            AddInstanceVertexList(sub,avlTable);
        }
        instanceListNode = sub->getInstances()->getHead();
        while (instanceListNode != NULL)
        {
            instance = instanceListNode->getInstance();
            avlTable->AddInstanceToTree( instance);
            instanceListNode = (InstanceListNode *)instanceListNode->getNext();
        }
        subListNode = (SubListNode *)subListNode->getNext();
    }
}

//---------------------------------------------------------------------------
// NAME: GetNextIncrement
//
// INPUTS:  (Parameters *parameters)
//
// RETURN:  BOOLEAN
//
// PURPOSE: Update the posGraph held in the parameters with the next data
//          increment.  Add a new Increment structure to the list,
//        representing the new data increment.
//---------------------------------------------------------------------------

BOOLEAN Params::GetNextIncrement()
{
    //long ltime;
    //int stime;
    BOOLEAN newData;
    ULONG startPosVertexIndex = 0;
    ULONG startPosEdgeIndex = 0;
    ULONG startNegVertexIndex = 0;
    ULONG startNegEdgeIndex = 0;
    
    // If this is the first increment then set up the graph structure info
    if (start)
    {
        InitializeGraph();
        start = FALSE;
    }
    
    startPosVertexIndex = posGraph->getNumVertices();  // total vertices
    if (negGraph != NULL)
        startNegVertexIndex = negGraph->getNumVertices();
    
    startNegEdgeIndex = posGraph->getNumEdges();  // total edges
    if (negGraph != NULL)
        startNegEdgeIndex = negGraph->getNumEdges();
    
    numIncrementPosVertices = 0;    // counter for num vertices in this increment
    numIncrementPosEdges = 0;       // counter for num edges in this increment
    numIncrementNegVertices = 0;    // counter for num vertices in this increment
    numIncrementNegEdges = 0;       // counter for num edges in this increment
    
    //ltime = std::clock();
    //stime = (unsigned) ltime/2;
    //srand(stime);
    
    newData = CreateFromFile(startPosVertexIndex, startNegVertexIndex);

    AddNewIncrement(startPosVertexIndex, startPosEdgeIndex,
                    startNegVertexIndex, startNegEdgeIndex,
                    numIncrementPosVertices, numIncrementPosEdges,
                    numIncrementNegVertices, numIncrementNegEdges);
    
    incrementCount++;
    
    return newData;
}

//----------------------------------------------------------------------------
// NAME:  CreateFromFile
//
// INPUTS:  (Parameters *parameters)
//          (ULONG startPosVertexIndex)
//          (ULONG startNegVertexIndex)
//
// RETURN:  BOOLEAN
//
// PURPOSE:  Read data for the next increment.
//----------------------------------------------------------------------------

BOOLEAN Params::CreateFromFile(
                       ULONG startPosVertexIndex, ULONG startNegVertexIndex)
{
    char fileName[50];
    char str[20];
    BOOLEAN newData = FALSE;
    
    sprintf(str, "_%d", incrementCount);
    strcpy(fileName, this->inputFileName);
    strcat(fileName, str);
    strcat(fileName, ".g");
    
    newData = ReadIncrement(fileName,
                            startPosVertexIndex, startNegVertexIndex);
    return newData;
}

//-------------------------------------------------------------------------
// NAME:  ReadIncrement
//
// INPUTS:  (char *filename) - filename for next increment
//          (Parameters *parameters)
//          (ULONG startVertex)
//
// RETURN:  BOOLEAN
//
// PURPOSE:  Read graph data from the file for the next increment.
//-------------------------------------------------------------------------

BOOLEAN Params::ReadIncrement(char *filename,
                      ULONG startPosVertex, ULONG startNegVertex)
{
    LabeledGraph *posGraph;
    LabeledGraph *negGraph;
    LabeledGraph *graph;
    FILE *graphFile;
    LabelList *labelList;
    char token[TOKEN_LEN];
    ULONG lineNo;             // Line number counter for graph file
    ULONG *posVertexIndices = this->posEgsVertexIndices;
    ULONG *negVertexIndices = this->negEgsVertexIndices;
    ULONG numPosExamples = this->numPosEgs;
    ULONG numNegExamples = this->numNegEgs;
    ULONG vertexOffset = 0;
    ULONG startVertex=0;
    BOOLEAN directed;
    BOOLEAN newData;
    
    labelList = this->labelList;
    directed = this->directed;
    posGraph = this->posGraph;
    negGraph = this->negGraph;
    graph = posGraph;
    startVertex = startPosVertex;
    
    // Open graph file
    graphFile = fopen(filename,"r");
    if (graphFile == NULL)
    {
        printf("End of Input.\n");
        newData = FALSE;
        return newData;
    }
    else
        newData = TRUE;
    
    // Parse graph file
    lineNo = 1;
    while (utility::ReadToken(token, graphFile, &lineNo) != 0)
    {
        if (strcmp(token, POS_EG_TOKEN) == 0)        // Read positive example
        {
            numPosExamples++;
            vertexOffset = posGraph->getNumVertices();
            posVertexIndices = utility::AddVertexIndex(posVertexIndices, numPosExamples, vertexOffset);
            graph = posGraph;
            startVertex = startPosVertex;
            readingPositive = TRUE;
        }
        else if (strcmp(token, NEG_EG_TOKEN) == 0)   // Read negative example
        {
            if (negGraph == NULL)
            {
                this->negGraph = LabeledGraph::AllocateGraph(0,0);
                negGraph = this->negGraph;
            }
            numNegExamples++;
            vertexOffset = negGraph->getNumVertices();
            negVertexIndices = utility::AddVertexIndex(negVertexIndices, numNegExamples, vertexOffset);
            graph = negGraph;
            startVertex = startNegVertex;
            readingPositive = FALSE;
        }
        else if (strcmp(token, "v") == 0)         // read vertex
        {
            if (readingPositive && numPosExamples == 0)
            {
                numPosExamples++;
                vertexOffset = posGraph->getNumVertices();
                posVertexIndices = utility::AddVertexIndex(posVertexIndices, numPosExamples, vertexOffset);
                graph = posGraph;
                startVertex = startPosVertex;
            }
            graph->ReadIncrementVertex( graphFile, labelList, &lineNo, vertexOffset,this);
        }
        else if (strcmp(token, "e") == 0)    // read 'e' edge
            graph->ReadIncrementEdge( graphFile, labelList, &lineNo, directed, startVertex, vertexOffset,this);
        else if (strcmp(token, "u") == 0)    // read undirected edge
            graph->ReadIncrementEdge(  graphFile, labelList, &lineNo, FALSE, startVertex, vertexOffset,this);
        else if (strcmp(token, "d") == 0)    // read directed edge
            graph->ReadIncrementEdge(  graphFile, labelList, &lineNo, TRUE, startVertex, vertexOffset,this);
        else
        {
            fclose(graphFile);
            posGraph->FreeGraph();
            negGraph->FreeGraph();
            fprintf(stderr, "Unknown token %s in line %lu of graph file %s.\n",
                    token, lineNo, filename);
            exit(1);
        }
    }
    fclose(graphFile);
    this->numPosEgs = numPosExamples;
    this->numNegEgs = numNegExamples;
    this->posEgsVertexIndices = posVertexIndices;
    this->negEgsVertexIndices = negVertexIndices;
    
    // trim vertex, edge and label lists
    return newData;
}

//---------------------------------------------------------------------------
// NAME: AddNewIncrement
//
// INPUTS: (ULONG startPosVertexIndex) - first positive vertex in new increment
//         (ULONG startPosEdgeIndex) - first positive edge in new increment
//         (ULONG startNegVertexIndex) - first negative vertex in new increment
//         (ULONG startNegEdgeIndex) - index of negative edge in new increment
//       (ULONG numPosVertices) - number of positive vertices in new increment
//       (ULONG numPosEdges) - number of positive edges in new increment
//       (ULONG numNegVertices) - number of negative vertices in new increment
//       (ULONG numNegEdges) - number of negative edges in new increment
//       (Parameters *parameters) - pointer to global parameters
//
// RETURN: void
//
// PURPOSE: Adds a new increment, which consists of an index into the array of
//        vertices, the number of the current increment, and the list of
//          associated subs (to be added later).
//---------------------------------------------------------------------------

void Params::AddNewIncrement(ULONG startPosVertexIndex, ULONG startPosEdgeIndex,
                     ULONG startNegVertexIndex, ULONG startNegEdgeIndex,
                     ULONG numPosVertices, ULONG numPosEdges,
                     ULONG numNegVertices, ULONG numNegEdges)
{
    IncrementListNode *incNodePtr =  incrementList->getHead();
    IncrementListNode *currentIncrementListNode = (IncrementListNode *) malloc(sizeof(IncrementListNode));
    Increment *increment = (Increment *)malloc(sizeof(Increment));
    increment->setSubList(SubList::AllocateSubList()) ;
    
    currentIncrementListNode->setInstance(increment) ;
    if (incNodePtr == NULL) // Empty list, this is the first increment
    {
        increment->setIncrementNum(1) ;
        incrementList->setHead(currentIncrementListNode) ;
    }
    else
    {
        while(incNodePtr->getNext() != NULL)
            incNodePtr = (IncrementListNode *) incNodePtr->getNext();
        increment->setIncrementNum(incNodePtr->getInstance()->getIncrementNum() + 1) ;
        incNodePtr->setNext(currentIncrementListNode) ;
    }
    increment->setStartPosVertexIndex(startPosVertexIndex);
    increment->setStartPosEdgeIndex(startPosEdgeIndex);
    increment->setStartNegVertexIndex(startNegVertexIndex);
    increment->setStartNegEdgeIndex(startNegEdgeIndex);
    increment->setNumPosVertices(numPosVertices);
    increment->setNumPosEdges(numPosEdges);
    increment->setNumNegVertices(numNegVertices);
    increment->setNumNegEdges(numNegEdges);
    currentIncrementListNode->setNext(NULL);
}




//---------------------------------------------------------------------------
// NAME: IncrementSize
//
// INPUTS: (Parameters *parameters) - system parameters
//         (ULONG incrementNum) - current increment
//         (ULONG graphType) - POS or NEG
//
// RETURN: (ULONG) - size of increment
//
// PURPOSE: Return size of graph as vertices plus edges.
//---------------------------------------------------------------------------

ULONG Params::IncrementSize( ULONG incrementNum, ULONG graphType)
{
    Increment *increment = GetIncrement(incrementNum);
    
    if (graphType == POS)
        return(increment->getNumPosVertices() + increment->getNumPosEdges());
    else
        return(increment->getNumNegVertices() + increment->getNumNegEdges());
}


//---------------------------------------------------------------------------
// NAME: IncrementNumExamples
//
// INPUTS: (Parameters *parameters) - system parameters
//         (ULONG incrementNum) - current increment
//         (ULONG graphType) - POS or NEG
//
// RETURN: (ULONG) - number of edges
//
// PURPOSE: Return number of positive or negative examples in increment.
//---------------------------------------------------------------------------

ULONG Params::IncrementNumExamples( ULONG incrementNum, ULONG graphType)
{
    Increment *increment = GetIncrement(incrementNum);
    
    if (graphType == POS)
        return((ULONG) increment->getNumPosEgs());
    else
        return((ULONG) increment->getNumNegEgs());
}

//---------------------------------------------------------------------------
// NAME:  AddInstanceVertexList
//
// INPUTS:  (Substructure *sub) - current substructure
//          (struct avl_table *avlTable) - pointer to avl tree
//          (Parameters *parameters) - system parameters
//
// RETURN:  void
//
// PURPOSE:  Add the new avl tree to the head of the list of avl trees.
//---------------------------------------------------------------------------

void Params::AddInstanceVertexList(Substructure *sub, AvlTable *avlTable)
{
    AvlTableNode *avlTableNode;
    AvlTreeList *avlTreeList;
    
    avlTreeList = vertexList->getHead();
    
    avlTableNode = (AvlTableNode *)malloc(sizeof(AvlTableNode));
    avlTableNode->setInstance(avlTable) ;
    avlTableNode->setSub(sub);
    avlTableNode->setNext((AvlTableNode *)avlTreeList->getHead()) ;
    avlTreeList->setHead(avlTableNode);
}

//---------------------------------------------------------------------------
// NAME: ComputeBestSubstructures
//
// INPUTS:  (Parameters *parameters)
//          (int listSize)
//
// RETURN:  (Sublist *) - substructures in ranked order
//
// PURPOSE: Traverse the top n substructures collected for each increment
//        and compute the best n for the entire graph up to this point.
//          For each substructure that is new, check each increment to see
//          if it contains instances, then calculate its global value and store
//          substructure on the global sublist.
//---------------------------------------------------------------------------

SubList *Params::ComputeBestSubstructures( int listSize)
{
    SubList *globalSubList;
    SubList *completeSubList;
    SubListNode *subIndex;
    SubListNode *incrementSubListNode;
    IncrementListNode *incNodePtr;
    Increment *increment;
    Increment *currentInc;
    double subValue;
    BOOLEAN found = FALSE;
    ULONG numPosInstances = 0;
    ULONG numNegInstances = 0;
    ULONG graphPosSize;
    ULONG graphNegSize;
    int i;
    
    completeSubList = SubList::AllocateSubList();
    incNodePtr = this->incrementList->getHead();
    currentInc = GetCurrentIncrement();
    
    // traverse each increment starting with the first increment
    while (incNodePtr != NULL)
    {
        increment = incNodePtr->getInstance();
        incrementSubListNode = increment->getSubList()->getHead();
        
        // Traverse each sub in the increment
        while (incrementSubListNode != NULL)
        {
            found = FALSE;
            subIndex = completeSubList->getHead();
            
            // Add sub to global list if not already there
            while (subIndex != NULL)
            {
                if (subIndex->getInstance()->getDefinition()->GraphMatch(
                               incrementSubListNode->getInstance()->getDefinition(),
                               this->labelList, 0.0, NULL, NULL))
                {
                    found = TRUE;
                    break;
                }
                subIndex = (SubListNode *)subIndex->getNext();
            }
            
            if (!found)                  // add to globalSubList
            {
                subValue = incrementSubListNode->getInstance()->ComputeValue(incNodePtr,
                                        this->labelList, &numPosInstances,
                                        &numNegInstances, &graphPosSize, &graphNegSize,
                                        this);
                
                 // we just add it to the end of the list
                completeSubList->InsertSub( incrementSubListNode->getInstance(), subValue,
                          numPosInstances, numNegInstances);
            }
            incrementSubListNode = (SubListNode *)incrementSubListNode->getNext();
        }
        incNodePtr =(IncrementListNode *)incNodePtr->getNext();
    }
    
    // create the truncated sublist
    if (listSize > 0)
    {
        globalSubList = SubList::AllocateSubList();
        subIndex = completeSubList->getHead();
        for (i=0; ((i<listSize) && (subIndex != NULL)); i++)
        {
            globalSubList->InsertSub( subIndex->getInstance(), subIndex->getInstance()->getValue(),
                      subIndex->getInstance()->getNumInstances(), subIndex->getInstance()->getNumNegInstances());
            subIndex = (SubListNode *)subIndex->getNext();
        }
        completeSubList->FreeSubList();
    }
    else
        globalSubList = completeSubList;
    
    return globalSubList;
}

//---------------------------------------------------------------------------
// NAME: RemovePosEgsCovered
//
// INPUTS: (Substructure *sub) - substructure whose instances are to be removed
//         (Parameters *parameters)
//
// RETURN: (void)
//
// PURPOSE: This is basically compression for the set-covering
// evaluation method.  Creates a new positive graph containing only
// those examples not covered (in whole or in part) by an instance of
// the substructure.  The parameters related to the positive graph and
// the label list are updated accordingly.
//---------------------------------------------------------------------------

void Params::RemovePosEgsCovered(Substructure *sub)
{
    InstanceList *instanceList;
    InstanceListNode *instanceListNode;
    ULONG posEg;
    ULONG posEgStartVertexIndex;
    ULONG posEgEndVertexIndex;
    ULONG instanceVertexIndex;
    BOOLEAN found;
    ULONG e;
    LabeledGraph *newPosGraph;
    ULONG newNumPosEgs;
    ULONG newNumVertices;
    ULONG newNumEdges;
    ULONG vertexOffset;
    ULONG *newPosEgsVertexIndices;
    LabelList *newLabelList;
    
    // parameters used
    LabeledGraph *posGraph            = this->posGraph;
    LabeledGraph *negGraph            = this->negGraph;
    ULONG numPosEgs            = this->numPosEgs;
    ULONG *posEgsVertexIndices = this->posEgsVertexIndices;
    
    // if no instances, then no changes to positive graph
    if (sub->getInstances() == NULL)
        return;
    
    newNumPosEgs = 0;
    newNumVertices = 0;
    newNumEdges = 0;
    newPosEgsVertexIndices = NULL;
    instanceList = sub->getInstances();
    // for each example, look for a covering instance
    for (posEg = 0; posEg < numPosEgs; posEg++)
    {
        // find example's starting and ending vertex indices
        posEgStartVertexIndex = posEgsVertexIndices[posEg];
        if (posEg < (numPosEgs - 1))
            posEgEndVertexIndex = posEgsVertexIndices[posEg + 1] - 1;
        else
            posEgEndVertexIndex = posGraph->getNumVertices() - 1;
        // look for an instance whose vertices are in range
        instanceListNode = instanceList->getHead();
        found = FALSE;
        while ((instanceListNode != NULL) && (! found))
        {
            // can check any instance vertex, so use the first
            instanceVertexIndex = instanceListNode->getInstance()->getVertices()[0];
            if ((instanceVertexIndex >= posEgStartVertexIndex) &&
                (instanceVertexIndex <= posEgEndVertexIndex))
            {
                // found an instance covering this example
                found = TRUE;
            }
            instanceListNode = (InstanceListNode *)instanceListNode->getNext();
        }
        if (found)
        {
            // mark vertices and edges of example (note: these will not be
            // unmarked, because this posGraph will soon be de-allocated)
            posGraph->MarkExample(posEgStartVertexIndex, posEgEndVertexIndex, TRUE);
        }
        else
        {
            newNumPosEgs++;
            vertexOffset = newNumVertices;
            newPosEgsVertexIndices = utility::AddVertexIndex(newPosEgsVertexIndices,
                                                    newNumPosEgs, vertexOffset);
            newNumVertices += (posEgEndVertexIndex - posEgStartVertexIndex + 1);
        }
    }
    // count number of edges in examples left uncovered
    for (e = 0; e < posGraph->getNumEdges(); e++)
        if (! posGraph->getEdges()[e].isUsed())
            newNumEdges++;
    
    // create new positive graph and copy unmarked part of old
    newPosGraph = LabeledGraph::AllocateGraph(newNumVertices, newNumEdges);
    posGraph->CopyUnmarkedGraph( newPosGraph, 0, this);
    
    // compress label list and recompute graphs' labels
    newLabelList = LabelList::AllocateLabelList();
    newLabelList->CompressLabelListWithGraph( newPosGraph, this);
    if (negGraph != NULL)
        newLabelList->CompressLabelListWithGraph( negGraph, this);
    this->labelList->FreeLabelList();
    this->labelList = newLabelList;
    
    // reset positive graph stats and recompute MDL (only needed for printing)
    this->posGraph->FreeGraph();
    this->posGraph = newPosGraph;
    free(this->posEgsVertexIndices);
    this->posEgsVertexIndices = newPosEgsVertexIndices;
    this->numPosEgs = newNumPosEgs;
    if (this->evalMethod == EVAL_MDL)
        this->posGraphDL = newPosGraph->MDL( newLabelList->getNumLabels(),
                                     this);
}

//---------------------------------------------------------------------------
// NAME: MarkPosEgsCovered
//
// INPUTS: (Substructure *sub) - substructure whose instances are to be marked
//         (Parameters *parameters)
//
// RETURN: (void)
//
// PURPOSE: For the set-covering evaluate method, mark vertices covered by
//          the substructure so they will not be written to the compressed
//          file.
//---------------------------------------------------------------------------

void Params::MarkPosEgsCovered(Substructure *sub)
{
    InstanceList *instanceList;
    InstanceListNode *instanceListNode;
    ULONG posEg;
    ULONG posEgStartVertexIndex;
    ULONG posEgEndVertexIndex;
    ULONG instanceVertexIndex;
    BOOLEAN found;
    
    // parameters used
    LabeledGraph *posGraph            = this->posGraph;
    ULONG numPosEgs            = this->numPosEgs;
    ULONG *posEgsVertexIndices = this->posEgsVertexIndices;
    
    // if no instances, then no changes to positive graph
    if (sub->getInstances() == NULL)
        return;
    
    instanceList = sub->getInstances();
    // for each example, look for a covering instance
    for (posEg = 0; posEg < numPosEgs; posEg++)
    {
        // find example's starting and ending vertex indices
        posEgStartVertexIndex = posEgsVertexIndices[posEg];
        if (posEg < (numPosEgs - 1))
            posEgEndVertexIndex = posEgsVertexIndices[posEg + 1] - 1;
        else
            posEgEndVertexIndex = posGraph->getNumVertices() - 1;
        // look for an instance whose vertices are in range
        instanceListNode = (InstanceListNode *)instanceList->getHead();
        found = FALSE;
        while ((instanceListNode != NULL) && (! found))
        {
            // can check any instance vertex, so use the first
            instanceVertexIndex = instanceListNode->getInstance()->getVertices()[0];
            if ((instanceVertexIndex >= posEgStartVertexIndex) &&
                (instanceVertexIndex <= posEgEndVertexIndex))
            {
                // found an instance covering this example
                found = TRUE;
            }
            instanceListNode = (InstanceListNode *)instanceListNode->getNext();
        }
        if (found)
        {
            // mark vertices and edges of example (note: these will not be
            // unmarked, because this posGraph will soon be de-allocated)
            posGraph->MarkExample(posEgStartVertexIndex, posEgEndVertexIndex, TRUE);
        }
    }
}




















