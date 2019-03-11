//
// Created by Macbook on 11/02/2019.
//

#ifndef SUBDUE_PARAMS_HPP
#define SUBDUE_PARAMS_HPP

#define FILE_NAME_LEN 512  // maximum length of file names
#include "Subdue.hpp"
typedef unsigned long  ULONG;
typedef bool BOOLEAN ;

using namespace std;

class IncrementListNode;
class Increment;
class LabeledGraph;
class LabelList;
class IncrementList;
class InstanceVertexList;
class SubList;
class Substructure;
class AvlTable;
   
    class Params {
    public:
        char inputFileName[FILE_NAME_LEN];   // main input file
        char psInputFileName[FILE_NAME_LEN]; // predefined substructures input file
        char outFileName[FILE_NAME_LEN];     // file for machine-readable output
        LabeledGraph *posGraph;      // LabeledGraph of positive examples
        LabeledGraph *negGraph;      // LabeledGraph of negative examples
        double posGraphDL;    // Description length of positive input graph
        double negGraphDL;    // Description length of negative input graph
        unsigned long numPosEgs;      // Number of positive examples
        unsigned long numNegEgs;      // Number of negative examples
        unsigned long *posEgsVertexIndices; // vertex indices of where positive egs begin
        unsigned long *negEgsVertexIndices; // vertex indices of where negative egs begin
        LabelList *labelList; // List of unique labels in input graph(s)
        LabeledGraph **preSubs;      // Array of predefined substructure graphs
        unsigned long numPreSubs;     // Number of predefined substructures read in
        bool predefinedSubs; // TRUE is predefined substructures given
        bool outputToFile; // TRUE if file given for machine-readable output
        bool directed;     // If TRUE, 'e' edges treated as directed
        unsigned long beamWidth;      // Limit on size of substructure queue (> 0)
        unsigned long limit;          // Limit on number of substructures expanded (> 0)
        unsigned long maxVertices;    // Maximum vertices in discovered substructures
        unsigned long minVertices;    // Minimum vertices in discovered substructures
        unsigned long numBestSubs;    // Limit on number of best substructures
        //   returned (> 0)
        bool valueBased;   // If TRUE, then queues are trimmed to contain
        //   all substructures with the top beamWidth
        //   values; otherwise, queues are trimmed to
        //   contain only the top beamWidth substructures.
        bool prune;        // If TRUE, then expanded substructures with lower
        //   value than their parents are discarded.
        unsigned long outputLevel;    // More screen (stdout) output as value increases
        bool allowInstanceOverlap; // Default is FALSE; if TRUE, then instances
        // may overlap, but compression costlier
        unsigned long evalMethod;     // One of EVAL_MDL (default), EVAL_SIZE or
        //   EVAL_SETCOVER
        double threshold;     // Percentage of size by which an instance can differ
        // from the substructure definition according to
        // LabeledGraph match transformation costs
        unsigned long iterations;     // Number of SUBDUE iterations; if more than 1, then
        // LabeledGraph compressed with best sub between iterations
        double *log2Factorial;   // Cache array A[i] = lg(i!); grows as needed
        unsigned long log2FactorialSize; // Size of log2Factorial array
        unsigned long numPartitions;  // Number of partitions used by parallel SUBDUE
        bool recursion;    // If TRUE, recursive LabeledGraph grammar subs allowed
        bool variables;    // If TRUE, variable vertices allowed
        bool relations;    // If TRUE, relations between vertices allowed
        bool incremental;  // If TRUE, data is processed incrementally
        bool compress;     // If TRUE, write compressed LabeledGraph to file
        
        
        IncrementList *incrementList;   // Set of increments
        
        InstanceVertexList *vertexList; // List of avl trees containing
        
        // instance vertices
        unsigned long posGraphSize;
        unsigned long negGraphSize;
        
         int incrementCount ;
         bool start ;
         bool readingPositive ;
         ULONG numIncrementPosVertices;
         ULONG numIncrementPosEdges;
         ULONG numIncrementNegVertices;
         ULONG numIncrementNegEdges;
        
        
        
        Params(){
             incrementCount = 1;
             start = TRUE;
             readingPositive = TRUE;
        }
        
        
        ULONG IncrementSize( ULONG incrementNum, ULONG graphType);
        unsigned long GetCurrentIncrementNum();
        IncrementListNode *GetIncrementListNode(unsigned long  incrementNum);
        Increment *GetIncrement(unsigned long incrementNum);
        Increment *GetCurrentIncrement();
        
        
        void ReadInputFile();
        LabeledGraph **ReadSubGraphsFromFile(char *fileName, const char *subToken, unsigned long  *numSubGraphs);
        
        void InitializeGraph();
        bool  GetNextIncrement();
        void ReadPredefinedSubsFile();
        
        bool ReadIncrement(char *filename,unsigned long startPosVertex, unsigned long startNegVertex);
        
        bool  CreateFromFile(unsigned long startPosVertexIndex, unsigned long startNegVertexIndex);
        
        
        void  AddNewIncrement(ULONG startPosVertexIndex, ULONG startPosEdgeIndex,
                                     ULONG startNegVertexIndex, ULONG startNegEdgeIndex,
                                     ULONG numPosVertices, ULONG numPosEdges,
                                     ULONG numNegVertices, ULONG numNegEdges);
    
        ULONG IncrementNumExamples( ULONG incrementNum, ULONG graphType);
        SubList *ComputeBestSubstructures( int listSize);
        void AddInstanceVertexList(Substructure *sub, AvlTable *avlTable);
        
        ULONG GetStartVertexIndex(ULONG incrementNum, ULONG graphType);
        void SetIncrementNumExamples();
        void AddVertexTrees(SubList *subList);
        void StoreSubs(SubList *subList);
        void RemovePosEgsCovered(Substructure *sub);
        void MarkPosEgsCovered(Substructure *sub);
    
    };





#endif //SUBDUE_PARAMS_HPP
