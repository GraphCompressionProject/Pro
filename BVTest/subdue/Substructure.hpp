//
// Created by Macbook on 10/02/2019.
//

#ifndef SUBDUE_SUBSTRUCTURE_HPP
#define SUBDUE_SUBSTRUCTURE_HPP



#include "InstanceList.hpp"
#include "LabeledGraph.hpp"
#include "utility.hpp"

class Params;
class SubList;
class Compressor;
class AvlTable;
class Substructure {
    private:
        LabeledGraph  *definition;         // graph definition of substructure
        unsigned long  numInstances;        // number of positive instances
        unsigned long numExamples;         // number of unique positive examples
        InstanceList *instances;    // instances in positive graph
        unsigned long numNegInstances;     // number of negative instances
        unsigned long  numNegExamples;      // number of unique negative examples
        InstanceList *negInstances; // instances in negative graph
        double value;               // value of substructure
        bool recursive;          // is this substructure recursive?
        unsigned long recursiveEdgeLabel;   // index into label list of recursive edge label
        double posIncrementValue;   // DL/#Egs value of sub for positive increment
        double negIncrementValue;   // DL/#Egs value of sub for negative increment
        

        //---- Setters and Getters of the properties ---//

    public:
        LabeledGraph *getDefinition() const;

        void setDefinition(LabeledGraph *definition);

        unsigned long getNumInstances() const;

        void setNumInstances(unsigned long numInstances);

        unsigned long getNumExamples() const;

        void setNumExamples(unsigned long numExamples);

        InstanceList *getInstances() const;

        void setInstances(InstanceList *instances);

        unsigned long getNumNegInstances() const;

        void setNumNegInstances(unsigned long numNegInstances);

        unsigned long getNumNegExamples() const;

        void setNumNegExamples(unsigned long numNegExamples);

        InstanceList *getNegInstances() const;

        void setNegInstances(InstanceList *negInstances);

        double getValue() const;

        void setValue(double value);

        bool isRecursive() const;

        void setRecursive(bool recursive);

        unsigned long getRecursiveEdgeLabel() const;

        void setRecursiveEdgeLabel(unsigned long recursiveEdgeLabel);

        double getPosIncrementValue() const;

        void setPosIncrementValue(double posIncrementValue);

        double getNegIncrementValue() const;

        void setNegIncrementValue(double negIncrementValue);

   


        //-------------- Printing methods --------------//
        void PrintSub( Params *parameters);
        void PrintPosInstanceList(Params parameters);
        void PrintNegInstanceList(Params parameters);
        void  PrintStoredSub ( Params *parameters);

        //----------------------------------------------//
        
        SubList *ExtendSub(Params *);
        ULONG PosExamplesCovered( Params *parameters);
        ULONG NegExamplesCovered( Params *parameters);
        void EvaluateSub( Params *);
        AvlTable *GetSubTree( Params *parameters);
    
    BOOLEAN CheckVertexForOverlap(ULONG vertex,
                                                Params *parameters);
   
        //---------------------------------------------------------------------------
        // NAME: AllocateSub
        //
        // INPUTS: void
        //
        // RETURN: (Substructure *) - pointer to newly allocated substructure.
        //
        // PURPOSE: Allocate and initialize new substructure.  A negative
        // value indicates not yet computed.
        //---------------------------------------------------------------------------

        static Substructure *AllocateSub()
        {
           Substructure *sub;

           sub = (Substructure *) malloc(sizeof(Substructure));
           if (sub == NULL)
               utility::OutOfMemoryError("substructure");
           sub->setDefinition(NULL);
           sub->setNumInstances (0);
           sub->setNumExamples(0);
           sub->setInstances(NULL);
           sub->setNumNegInstances(0);
           sub->setNumNegExamples(0);
           sub->setNegInstances (NULL);
           sub->setValue(-1.0);
           sub->setRecursive(FALSE);
           sub->setRecursiveEdgeLabel(0);

           return sub;
        }



    //---------------------------------------------------------------------------
    // NAME: FreeSub
    //
    // INPUTS: (Substructure *sub) - Substructure to be freed.
    //
    // RETURN: void
    //
    // PURPOSE: Free memory used by given substructure.
    //---------------------------------------------------------------------------

        void FreeSub()
        {
                if(definition != NULL)  definition->FreeGraph();
                if(instances !=NULL)    instances->FreeInstanceList();
                if (negInstances!= NULL)    negInstances->FreeInstanceList();
        }

        Substructure *RecursifySub( Params *);
        void PrintNewBestSub( SubList *subList, Params *parameters);
        void PrintNegInstanceList( Params *parameters);
        void PrintPosInstanceList( Params *parameters);
        void AddPosInstancesToSub( Instance *subInstance, InstanceList *instanceList, Params *parameters, ULONG index);
        void AddNegInstancesToSub( Instance *subInstance,InstanceList *instanceList, Params *parameters);
        Substructure *MakeRecursiveSub( ULONG edgeLabel, Params *parameters);
    
        void AdjustMetrics( Params *parameters);
        
    Substructure *CopySub();
    
    double ComputeValue(IncrementListNode *incNodePtr,
                                      LabelList *labelList, ULONG *numPosInstances,
                                      ULONG *numNegInstances, ULONG *graphPosSize,
                                      ULONG *graphNegSize, Params *parameters);
    void CompressFinalGraphs( Params *parameters,
                                           ULONG iteration, BOOLEAN predefinedSub);

    };
    




#endif //SUBDUE_SUBSTRUCTURE_HPP
