//
// Created by Macbook on 10/02/2019.
//

#ifndef SUBDUE_INSTANCE_HPP
#define SUBDUE_INSTANCE_HPP

#include "VertexMap.hpp"

#include <iostream>
#include <cstdlib>
#include "utility.hpp"
#include "LabeledEdge.hpp"


using namespace std;

class LabeledGraph;
class LabelList ;
class Substructure;
class InstanceList;
class ReferenceGraph;
    class Instance {
    private:
        unsigned long  numVertices;   // number of vertices in instance
        unsigned long  numEdges;      // number of edges in instance
        unsigned long  *vertices;     // ordered indices of instance's vertices in graph
        unsigned long  *edges;        // ordered indices of instance's edges in graph
        double minMatchCost; // lowest cost so far of matching this instance to
        // a substructure
        unsigned long  newVertex;     // index into vertices array of newly added vertex
        //    (0 means no new vertex added)
        unsigned long  newEdge;       // index into edges array of newly added edge
        unsigned long  refCount;      // counter of references to this instance; if zero,
        //    then instance can be deallocated
        VertexMap *mapping;  // instance mapped to substructure definition
        unsigned long  mappingIndex1; // index of source vertex of latest mapping
        unsigned long  mappingIndex2; // index of target vertex of latest mapping
        bool used;        // flag indicating instance already associated
        // with a substructure (for the current iteration)
        Instance *parentInstance;  // pointer to parent instance

    public:

		Instance() {
			this->setNumVertices(0);
			this->setNumEdges(0);
			this->setVertices(NULL);
			this->setEdges(NULL);
			this->setMapping(NULL);
			this->setNewVertex(0);
			this->setNewEdge(0);
			this->setMappingIndex1(MAX_UNSIGNED_LONG);
			this->setMappingIndex2(MAX_UNSIGNED_LONG);
			this->setUsed(FALSE);
		};

        unsigned long getNumVertices() ;

        void setNumVertices(unsigned long numVertices);

        unsigned long getNumEdges() ;

        void setNumEdges(unsigned long numEdges);

        unsigned long *getVertices() ;

        void setVertices(unsigned long *vertices);

        unsigned long *getEdges() ;

        void setEdges(unsigned long *edges);

        double getMinMatchCost() ;

        void setMinMatchCost(double minMatchCost);

        unsigned long getNewVertex() ;

        void setNewVertex(unsigned long newVertex);

        unsigned long getNewEdge() ;

        void setNewEdge(unsigned long newEdge);

        unsigned long getRefCount() ;

        void setRefCount(unsigned long refCount);

        VertexMap *getMapping() ;

		unsigned long getMapping(int index,int vertex) {

			switch (index)
			{
			case 0:
				switch (vertex)
				{
				case 0:
					return mapping[mappingIndex1].getV1();
				break;
				case 1:
					return mapping[mappingIndex1].getV2();
					break;
				}
				break;
			case 1:
				switch (vertex)
				{
				case 0:
					return mapping[mappingIndex2].getV1();
					break;
				case 1:
					return mapping[mappingIndex2].getV2();
					break;
				}
				break;
			
			}

		}

        void setMapping(VertexMap *mapping);

        unsigned long getMappingIndex1() ;

        void setMappingIndex1(unsigned long mappingIndex1);

        unsigned long getMappingIndex2() ;

        void setMappingIndex2(unsigned long mappingIndex2);

        bool isUsed() ;

        void setUsed(bool used);

        Instance *getParentInstance() ;

        void setParentInstance(Instance *parentInstance);

        void UpdateMapping( Instance *instance2);
        
         unsigned long InstanceExampleNumber( unsigned long *egsVertexIndices, unsigned long numEgs);
         void PrintInstance(unsigned long vertexOffset, LabeledGraph *graph, LabelList *labelList);
        
         unsigned long GetStartVertexIndex(unsigned long graphType);
        void FreeInstance();
        
        BOOLEAN InstanceOverlap( Instance *instance2);
        Instance *CreateExtendedInstance( ULONG v, ULONG e, LabeledGraph *graph);
        BOOLEAN InstanceContainsVertex( ULONG v);
        BOOLEAN InstanceMatch(Instance *instance2);
        BOOLEAN InstanceListOverlap( InstanceList *instanceList);
        Instance *CopyInstance();
        Instance *CreateGraphRefInstance( ReferenceGraph *refGraph);
        ReferenceGraph *InstanceToRefGraph( LabeledGraph *graph,
                                                     Params *parameters);
        
        BOOLEAN AddInstanceToIncrement(Substructure *newSub, Params *parameters);
        
        BOOLEAN CheckInstanceForOverlap( Substructure *newSub,
                                                  Params *parameters);
		
        //---------------------------------------------------------------------------
        // NAME: AllocateInstance
        //
        // INPUTS: (unsigned long v) - number of vertices in instance
        //         (unsigned long e) - number of edges in instance
        //
        // RETURN: (Instance *) - pointer to newly allocated instance
        //
        // PURPOSE: Allocate and return space for new instance.
        //---------------------------------------------------------------------------
        
        static Instance *AllocateInstance(unsigned long v, unsigned long e)
        {
            Instance *instance;
            
            instance = (Instance *) malloc(sizeof(Instance));
            if (instance == NULL)
                utility::OutOfMemoryError("AllocateInstance:instance");
            instance->setNumVertices(v);
            instance->setNumEdges(e);
            instance->setVertices(NULL);
            instance->setEdges(NULL);
            instance->setMapping(NULL);
            instance->setNewVertex(0);
            instance->setNewEdge(0);
            instance->setMappingIndex1(MAX_UNSIGNED_LONG);
            instance->setMappingIndex2(MAX_UNSIGNED_LONG);
            instance->setUsed(FALSE);
            if (v > 0)
            {
                instance->setVertices((unsigned long *) malloc(sizeof(unsigned long) * v)) ;
                if (instance->getVertices() == NULL)
                    utility::OutOfMemoryError("AllocateInstance: instance->vertices");
                instance->setMapping( (VertexMap *) malloc(sizeof(VertexMap) * v));
                if (instance->getMapping() == NULL)
                    utility::OutOfMemoryError("AllocateInstance: instance->mapping");
            }
            if (e > 0)
            {
                instance->setEdges((unsigned long *) malloc(sizeof(unsigned long) * e)) ;
                if (instance->edges == NULL)
                    utility::OutOfMemoryError("AllocateInstance: instance->edges");
            }
            instance->setMinMatchCost(MAX_DOUBLE);
            instance->setRefCount(0) ;
            instance->setParentInstance(NULL);
            
            return instance;
            
        }
        LabeledGraph *InstanceToGraph( LabeledGraph *graph);
        Substructure *CreateSubFromInstance( LabeledGraph *graph);
        void AddInstanceToInstance( Instance *instance2);
        void AddEdgeToInstance(ULONG edgeIndex, LabeledEdge *edge);
        //---------------------------------------------------------------------------
        // NAME: AddRecursiveInstancePair
        //
        // INPUTS: (ULONG i1) - index of instance1 in instance map
        //         (ULONG i2) - index of instance2 in instance map
        //         (Instance *instance1) - first instance in connected pair to add
        //         (Instance *instance2) - second instance in connected pair to add
        //         (ULONG edgeIndex) - edge's index in graph
        //         (Edge *edge) - edge connecting two instances
        //         (ULONG numInstances) - number of original instances, i.e., length
        //              of instanceMap
        //         (Instance **instanceMap) - map of instances to the new instances
        //              in which they reside
        //
        // RETURN: (void)
        //
        // PURPOSE: Updates the instanceMap to reflect a joining of the new
        // instances containing the original instance1 and instance2, and the
        // addition of the connecting edge.  Assumes no instance pair is added
        // more than once.
        //---------------------------------------------------------------------------
        
        static void AddRecursiveInstancePair(ULONG i1, ULONG i2,
                                             Instance *instance1, Instance *instance2,
                                             ULONG edgeIndex, LabeledEdge *edge,
                                             ULONG numInstances, Instance **instanceMap)
        {
            Instance *tmpInstance = NULL;
            ULONG i;
            
            if ((instanceMap[i1] == instance1) && (instanceMap[i2] == instance2))
            {
                // instances not yet linked to a new recursive instance
                tmpInstance = AllocateInstance(0, 0);
                tmpInstance->AddInstanceToInstance(instance1);
                tmpInstance->AddInstanceToInstance(instance2);
                tmpInstance->AddEdgeToInstance(edgeIndex, edge);
                instanceMap[i1] = tmpInstance;
                instanceMap[i2] = tmpInstance;
            }
            else if (instanceMap[i1] == instance1)
            {
                // instance1 to be linked to new instance at instanceMap[i2]
                instanceMap[i2]->AddInstanceToInstance(instance1);
                instanceMap[i2]->AddEdgeToInstance(edgeIndex, edge);
                instanceMap[i1] = instanceMap[i2];
            }
            else if (instanceMap[i2] == instance2)
            {
                // instance2 to be linked to new instance at instanceMap[i1]
                instanceMap[i1]->AddInstanceToInstance(instance2);
                instanceMap[i1]->AddEdgeToInstance(edgeIndex, edge);
                instanceMap[i2] = instanceMap[i1];
            }
            else if (instanceMap[i1] != instanceMap[i2])
            {
                // both instances already belong to new different recursive instances
                instanceMap[i1]->AddInstanceToInstance(instanceMap[i2]);
                instanceMap[i1]->AddEdgeToInstance(edgeIndex, edge);
                tmpInstance = instanceMap[i2];
                for (i = 0; i < numInstances; i++)
                    if (instanceMap[i] == tmpInstance)
                        instanceMap[i] = instanceMap[i1];
                tmpInstance->FreeInstance();
            }
            else
            {
                // both instances already in same new recursive instance
                instanceMap[i1]->AddEdgeToInstance(edgeIndex, edge);
            }
        }

        Instance *CreateConstrainedExtendedInstance( ULONG v,
                                                              ULONG v2, ULONG e,
                                                    ReferenceGraph *graph);
      
        InstanceList *ExtendConstrainedInstance(
                                                          Substructure *bestSub,
                                                          ReferenceGraph *refGraph,
                                                          LabeledGraph *fullGraph,
                                                          Params *parameters);


    };


#endif //SUBDUE_INSTANCE_HPP
