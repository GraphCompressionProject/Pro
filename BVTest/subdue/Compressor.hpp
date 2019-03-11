//
// Created by Macbook on 12/02/2019.
//

#ifndef SUBDUE_COMPRESSOR_HPP
#define SUBDUE_COMPRESSOR_HPP



#define TRUE 1
#define FALSE 0

class InstanceList ;
class Params ;
class LabeledGraph ;
class LabeledEdge;
    class Compressor {
    public:
        
        LabeledGraph *CompressGraph(LabeledGraph *graph, InstanceList *instanceList,Params *parameters);
        
        LabeledEdge *AddDuplicateEdges(LabeledEdge *overlapEdges, unsigned long  *numOverlapEdgesPtr,LabeledEdge *edge, LabeledGraph *graph, unsigned long sub1VertexIndex, unsigned long  sub2VertexIndex);
        
        void CompressWithPredefinedSubs(Params *parameters);
        
        
    };




#endif //SUBDUE_COMPRESSOR_HPP
