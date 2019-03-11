//
//  GraphWriter.hpp
//  Subdue_v1
//
//  Created by Macbook on 24/02/2019.
//  Copyright © 2019 Macbook. All rights reserved.
//

#ifndef GraphWriter_hpp
#define GraphWriter_hpp

#include <stdio.h>

class Params ;
class LabeledGraph;
class InstanceList;
class LabelList ;
class Substructure;


typedef unsigned long ULONG;
typedef bool BOOLEAN;

class GraphWriter{
    public :
    
    
    
    void WriteGraphToDotFile(char *dotFileName, Params *parameters);
    
    void WriteGraphWithInstancesToDotFile(char *dotFileName, LabeledGraph *graph,InstanceList *instanceList, Params *parameters);
    
    void WriteSubsToDotFile(char *dotFileName, LabeledGraph **subGraphs, unsigned long numSubGraphs, Params *parameters);
    
    void WriteVertexToDotFile(FILE *dotFile, ULONG v, ULONG vertexOffset, LabeledGraph *graph, LabelList *labelList, const char *color);
    
    void WriteEdgeToDotFile(FILE *dotFile, ULONG e, ULONG vertexOffset, LabeledGraph *graph, LabelList *labelList, const char *color);
    
    void WriteLabelToFile(FILE *outFile, ULONG index, LabelList *labelList,BOOLEAN suppressQuotes);
    
    void WriteGraphToFile(FILE *outFile, LabeledGraph *graph, LabelList *labelList,
                          ULONG vOffset, ULONG start, ULONG finish, BOOLEAN printPS);
    
    void WriteUpdatedGraphToFile(Substructure *sub, Params *parameters);
    
    void WriteUpdatedIncToFile(Substructure *sub, Params *parameters);

    void WriteCompressedGraphToFile(Substructure *sub, Params *parameters,ULONG iteration);




};




#endif /* GraphWriter_hpp */
