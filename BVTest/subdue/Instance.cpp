//
// Created by Macbook on 10/02/2019.
//

#include "stdafx.h"

#include "Instance.hpp"
#include "LabelList.hpp"
#include "LabeledGraph.hpp"
#include "Substructure.hpp"
#include "ReferenceEdge.hpp"
#include "ReferenceGraph.hpp"
#include "AvlTable.hpp"
#include "SubListNode.hpp"
#include "Increment.hpp"



unsigned long Instance::getNumVertices() {
    return numVertices;
}

void Instance::setNumVertices(unsigned long numVertices) {
    Instance::numVertices = numVertices;
}

unsigned long Instance::getNumEdges() {
    return numEdges;
}

void Instance::setNumEdges(unsigned long numEdges) {
    Instance::numEdges = numEdges;
}

unsigned long *Instance::getVertices() {
    return vertices;
}

void Instance::setVertices(unsigned long *vertices) {
    Instance::vertices = vertices;
}

unsigned long *Instance::getEdges() {
    return edges;
}

void Instance::setEdges(unsigned long *edges) {
    Instance::edges = edges;
}

double Instance::getMinMatchCost() {
    return minMatchCost;
}

void Instance::setMinMatchCost(double minMatchCost) {
    Instance::minMatchCost = minMatchCost;
}

unsigned long Instance::getNewVertex() {
    return newVertex;
}

void Instance::setNewVertex(unsigned long newVertex) {
    Instance::newVertex = newVertex;
}

unsigned long Instance::getNewEdge() {
    return newEdge;
}

void Instance::setNewEdge(unsigned long newEdge) {
    Instance::newEdge = newEdge;
}

unsigned long Instance::getRefCount() {
    return refCount;
}

void Instance::setRefCount(unsigned long refCount) {
    Instance::refCount = refCount;
}

VertexMap *Instance::getMapping() {
    return mapping;
}



void Instance::setMapping(VertexMap *mapping) {
    Instance::mapping = mapping;
}

unsigned long Instance::getMappingIndex1() {
    return mappingIndex1;
}

void Instance::setMappingIndex1(unsigned long mappingIndex1) {
    Instance::mappingIndex1 = mappingIndex1;
}

unsigned long Instance::getMappingIndex2() {
    return mappingIndex2;
}

void Instance::setMappingIndex2(unsigned long mappingIndex2) {
    Instance::mappingIndex2 = mappingIndex2;
}

bool Instance::isUsed() {
    return used;
}

void Instance::setUsed(bool used) {
    Instance::used = used;
}

Instance *Instance::getParentInstance() {
    return parentInstance;
}

void Instance::setParentInstance(Instance *parentInstance) {
    Instance::parentInstance = parentInstance;
}

//---------------------------------------------------------------------------
// NAME: CreateExtendedInstance
//
// INPUTS: (Instance *instance) - instance being extended
//         (ULONG v) - vertex in graph where new edge being added
//         (ULONG e) - edge in graph being added to instance
//         (Graph *graph) - graph containing instance and new edge
//
// RETURN: (Instance *) - new extended instance
//
// PURPOSE: Create and return a new instance, which is a copy of the
// given instance extended by one edge e along vertex v.  Edge e may
// introduce a new vertex.  Make sure that instance's vertices and
// edges arrays are kept in increasing order, which is important for
// fast instance matching.
//---------------------------------------------------------------------------

Instance *Instance::CreateExtendedInstance( ULONG v, ULONG e, LabeledGraph *graph)
{
    Instance *newInstance;
    ULONG v2;
    BOOLEAN found = FALSE;
    ULONG i;
    
    // get edge's other vertex
    if (graph->getEdges()[e].getE().first == v)
        v2 = graph->getEdges()[e].getE().second;
    else
        v2 = graph->getEdges()[e].getE().first;
    
    // check if edge's other vertex is already in instance
    for (i = 0; ((i < numVertices) && (! found)); i++)
        if (vertices[i] == v2)
            found = TRUE;
    
    // allocate memory for new instance
    if (! found)
        newInstance = AllocateInstance(numVertices + 1, numEdges + 1);
    else
        newInstance = AllocateInstance(numVertices, numEdges + 1);
    
    // save pointer to what the instance was before this
    // extension (to be used as the "parent mapping")
    newInstance->setParentInstance(this) ;
    
    // set vertices and mappings of new instance
    for (i = 0; i < numVertices; i++)
    {
        // copy vertices
        newInstance->getVertices()[i] = vertices[i];
        
        // copy mapping
        newInstance->getMapping()[i].setV1(mapping[i].getV1()) ;
        newInstance->getMapping()[i].setV2( mapping[i].getV2());
        
        // set indices to indicate indices to source and target vertices
        if (newInstance->getMapping()[i].getV2() == graph->getEdges()[e].getE().second)
            newInstance->setMappingIndex2(i);
        if (newInstance->getMapping()[i].getV2() == graph->getEdges()[e].getE().first)
            newInstance->setMappingIndex1(i) ;
    }
    
    newInstance->setNewVertex(VERTEX_UNMAPPED) ;
    if (! found)
    {
        i = numVertices;
        while ((i > 0) && (v2 < newInstance->getVertices()[i-1]))
        {
            newInstance->getVertices()[i] = newInstance->getVertices()[i-1];
            newInstance->getMapping()[i].setV1(i) ;
            newInstance->getMapping()[i].setV2(newInstance->getMapping()[i-1].getV2());
            // if indices moved, move mapping indices
            if (newInstance->getMapping()[i].getV2() == graph->getEdges()[e].getE().second)
                newInstance->setMappingIndex2(i) ;
            if (newInstance->getMapping()[i].getV2() == graph->getEdges()[e].getE().first)
                newInstance->setMappingIndex1(i) ;
            i--;
        }
        newInstance->getVertices()[i] = v2;
        newInstance->setNewVertex(i);
        newInstance->getMapping()[i].setV1(i) ;
        newInstance->getMapping()[i].setV2(v2) ;
        // Since this is a new vertex, need to update the index
        if (newInstance->getMapping()[i].getV2() == graph->getEdges()[e].getE().second)
            newInstance->setMappingIndex2(i) ;
        if (newInstance->getMapping()[i].getV2() == graph->getEdges()[e].getE().first)
            newInstance->setMappingIndex1(i);
    }
    
    // set edges of new instance, kept in increasing order
    for (i = 0; i < numEdges; i++)
        newInstance->getEdges()[i] = edges[i];
    i = numEdges;
    while ((i > 0) && (e < newInstance->getEdges()[i-1]))
    {
        newInstance->getEdges()[i] = newInstance->getEdges()[i-1];
        i--;
    }
    newInstance->getEdges()[i] = e;
    newInstance->setNewEdge(i);
    
    return newInstance;
}


// NAME: FreeInstance
//
// INPUTS: (Instance *instance) - instance to free
//
// RETURN: (void)
//
// PURPOSE: Deallocate memory of given instance, if there are no more
// references to it.
//---------------------------------------------------------------------------

void Instance::FreeInstance()
{
    if ( refCount == 0)
    {
        free(vertices);
        free(mapping);
        free(edges);
    }
}

//---------------------------------------------------------------------------
// NAME: InstanceExampleNumber
//
// INPUTS: (Instance *instance) - instance to look for
//         (unsigned long *egsVertexIndices) - vertex indices of graph's examples
//         (unsigned long numEgs) - number of graph examples
//
// RETURN: (unsigned long) - example number containing instance
//
// PURPOSE: Return which example contains the given instance.
//---------------------------------------------------------------------------

unsigned long Instance::InstanceExampleNumber( unsigned long *egsVertexIndices, unsigned long numEgs)
{
   unsigned long instanceVertexIndex;
   unsigned long egNo;

   instanceVertexIndex = vertices[0];
   egNo = 1;
   while ((egNo < numEgs) && (instanceVertexIndex >= egsVertexIndices[egNo]))
      egNo++;

   return egNo;
}

//---------------------------------------------------------------------------
// NAME: PrintInstance
//
// INPUTS: (Instance *instance) - instance to print
//         (unsigned long vertexOffset) - offset for vertex index for this instance
//         (Graph *graph) - graph containing instance
//         (LabelList *labelList) - labels from graph
//
// RETURN: (void)
//
// PURPOSE: Print given instance.
//---------------------------------------------------------------------------

void Instance::PrintInstance(unsigned long vertexOffset, LabeledGraph *graph, LabelList *labelList)
{
   unsigned long i;

      for (i = 0; i < numVertices; i++) 
      {
         printf("    ");
         graph->PrintVertex(vertices[i], vertexOffset, labelList);
      }
      for (i = 0; i < numEdges; i++) 
      {
         printf("    ");
         graph->PrintEdge(edges[i], vertexOffset, labelList);
      }
}

//---------------------------------------------------------------------------
// NAME: InstanceOverlap
//
// INPUTS: (Instance *instance1)
//         (Instance *instance2)
//
// RETURN: (BOOLEAN) - TRUE if instances overlap
//
// PURPOSE: Determine if given instances overlap, i.e., share at least
// one vertex.  NOTE: instance vertices arrays are assumed to be in
// increasing order.
//---------------------------------------------------------------------------

BOOLEAN Instance::InstanceOverlap( Instance *instance2)
{
    ULONG v1;
    ULONG i = 0;
    ULONG j = 0;
    BOOLEAN overlap = FALSE;
    ULONG nv1 = numVertices;
    ULONG nv2 = instance2->getNumVertices();
    
    while ((i < nv1) && (j < nv2) && (! overlap))
    {
        v1 = vertices[i];
        while ((j < nv2) && (instance2->getVertices()[j] < v1))
            j++;
        if ((j < nv2) && (v1 == instance2->getVertices()[j]))
            overlap = TRUE;
        i++;
    }
    return overlap;
}
//---------------------------------------------------------------------------
// NAME: CreateSubFromInstance
//
// INPUTS: (Instance *instance) - instance
//         (Graph *graph) - graph containing instance
//
// RETURN: (Substructure *) - new substructure equivalent to instance
//
// PURPOSE: Create and return a new substructure based on the given
// instance.  Right now, the substructure is identical to the
// instance, but may be different. (*****)
//---------------------------------------------------------------------------

Substructure *Instance::CreateSubFromInstance( LabeledGraph *graph)
{
    Substructure *newSub = Substructure::AllocateSub();
    newSub->setDefinition(InstanceToGraph( graph)) ;
    return newSub;
}

//---------------------------------------------------------------------------
// NAME: InstanceToGraph
//
// INPUTS: (Instance *instance) - instance to convert
//         (Graph *graph) - graph containing instance
//
// RETURN: (Graph *) - new graph equivalent to instance
//
// PURPOSE: Convert given instance to an equivalent Graph structure.
//---------------------------------------------------------------------------

LabeledGraph *Instance::InstanceToGraph( LabeledGraph *graph)
{
    LabeledGraph *newGraph;
    LabeledVertex *vertex;
    LabeledEdge *edge;
    ULONG i, j;
    ULONG v1, v2;
    BOOLEAN found1;
    BOOLEAN found2;
    
    v1 = 0;
    v2 = 0;
    newGraph = LabeledGraph::AllocateGraph(numVertices, numEdges);
    
    // convert vertices
    for (i = 0; i < numVertices; i++)
    {
        vertex = & graph->getVertices()[vertices[i]];
        newGraph->getVertices()[i].setLabel(vertex->getLabel()) ;
        newGraph->getVertices()[i].setNumEdges(0) ;
        newGraph->getVertices()[i].setEdges(NULL) ;
        newGraph->getVertices()[i].setUsed(FALSE);
    }
    
    // convert edges
    for (i = 0; i < numEdges; i++)
    {
        edge = & graph->getEdges()[edges[i]];
        // find new indices for edge vertices
        j = 0;
        found1 = FALSE;
        found2 = FALSE;
        while ((! found1) || (! found2))
        {
            if (vertices[j] == edge->getE().first)
            {
                v1 = j;
                found1 = TRUE;
            }
            if (vertices[j] == edge->getE().second)
            {
                v2 = j;
                found2 = TRUE;
            }
            j++;
        }
        // set new edge information
        newGraph->getEdges()[i].getE().first = v1;
        newGraph->getEdges()[i].getE().second = v2;
        newGraph->getEdges()[i].setLabel( edge->getLabel());
        newGraph->getEdges()[i].setDirected(edge->isDirected()) ;
        newGraph->getEdges()[i].setUsed(FALSE) ;
        // add edge to appropriate vertices
        vertex = & newGraph->getVertices()[v1];
        vertex->setNumEdges(vertex->getNumEdges()+1);
        vertex->setEdges((ULONG *) realloc(vertex->getEdges(), sizeof(ULONG) * vertex->getNumEdges())) ;
        if (vertex->getEdges() == NULL)
            utility::OutOfMemoryError("InstanceToGraph:vertex1->edges");
        vertex->getEdges()[vertex->getNumEdges() - 1] = i;
        if (v1 != v2)
        {
            vertex = & newGraph->getVertices()[v2];
            vertex->setNumEdges(vertex->getNumEdges()+1);
            vertex->setEdges((ULONG *) realloc(vertex->getEdges(), sizeof(ULONG) * vertex->getNumEdges()));
            if (vertex->getEdges() == NULL)
                utility::OutOfMemoryError("InstanceToGraph:vertex2->edges");
            vertex->getEdges()[vertex->getNumEdges() - 1] = i;
        }
    }
    return newGraph;
}


//---------------------------------------------------------------------------
// NAME: UpdateMapping
//
// INPUTS: (Instance *instance1) - first instance (to be compared to)
//         (Instance *instance2) - second instance (to see if it belongs)
//
// RETURN: (void)
//
// PURPOSE: instance1 is the main instance and instance2 is the instance from
//          the list that is being compared to the main one to see if it
//          supposed to go on the list.  The logic below loops through
//          instance1 and its parent, find the correponding entry, then sets
//          the associated entry in instance2.  If a new vertex was added in
//          this extension (as opposed to just an edge extension), the slot
//          that has not been filled in instance2 gets the new vertex.
//
//---------------------------------------------------------------------------
void Instance::UpdateMapping( Instance *instance2)
{
    ULONG i,j;
    ULONG counter = 0;
    ULONG firstIndexValue = 0;
    ULONG secondIndexValue = 0;
    
    BOOLEAN found;
    
    // Allocate space for array of flags that indicate which map slots are set
    BOOLEAN *mapSet;
    mapSet = (BOOLEAN *) malloc(sizeof(BOOLEAN) * instance2->getNumVertices());
    if (mapSet == NULL)
        utility::OutOfMemoryError("UpdateMapping: mapSet");
    
    // reset flags indicating whether individual map entries have been set
    for (i=0;i<instance2->getNumVertices();i++)
        mapSet[i] = FALSE;
    
    // First, need to get vertex values for mappingIndex1 and mappingIndex2
    // so that when they get rearranged below, we can reset their values.
    for (i=0;i<instance2->numVertices;i++)
    {
        if (i == instance2->getMappingIndex1())
            firstIndexValue = instance2->getMapping()[i].getV2();
        if (i == instance2->getMappingIndex2())
            secondIndexValue = instance2->getMapping()[i].getV2();
    }
    
    // Loop through mapping of instance2
    // "i" is used for parent instances; "j" is used for current instances
    for (i=0;i<instance2->getParentInstance()->getNumVertices();i++)
    {
        j = 0;
        found = FALSE;
        while ((j < numVertices) && (!found))
        {
            // If found corresponding entry, then set the appropriate
            // entry in instance2
            if (parentInstance->getMapping()[i].getV2() == mapping[j].getV2())
            {
                instance2->getMapping()[j].setV1(j) ;
                instance2->getMapping()[j].setV2(instance2->getParentInstance()->getMapping()[i].getV2()) ;
                mapSet[j] = TRUE;
                //
                // Now, if this vertex was one of the mapping indices,
                // reset the appropriate index
                //
                if (instance2->getMapping()[j].getV2() == firstIndexValue)
                    instance2->setMappingIndex1(j) ;
                if (instance2->getMapping()[j].getV2() == secondIndexValue)
                    instance2->setMappingIndex2(j);
                counter++;
                found = TRUE;
            }
            j++;
        }
    }
    
    // If there is one left, it would be the new vertex
    if (counter < instance2->getNumVertices())
    {
        for (i=0;i<instance2->getNumVertices();i++)
        {
            if (!mapSet[i])
            {
                // Add new vertex in open spot
                instance2->getMapping()[i].setV1(i) ;
                instance2->getMapping()[i].setV2(instance2->getVertices()[instance2->getNewVertex()]) ;
                //
                // And move the appropriate index...
                //
                if (instance2->getMapping()[i].getV2() == firstIndexValue)
                    instance2->setMappingIndex1(i) ;
                if (instance2->getMapping()[i].getV2() == secondIndexValue)
                    instance2->setMappingIndex2(i) ;
                
                free(mapSet);
                return;
            }
        }
    }
    
    free(mapSet);
    return;
}

//---------------------------------------------------------------------------
// NAME: InstanceContainsVertex
//
// INPUTS: (Instance *instance) - instance to look in
//         (ULONG v) - vertex index to look for
//
// RETURN: (BOOLEAN) - TRUE if vertex found in instance
//
// PURPOSE: Determine if the given vertex is in the given instance.
// NOTE: instance vertices array is assumed to be in increasing order.
//---------------------------------------------------------------------------

BOOLEAN Instance::InstanceContainsVertex( ULONG v)
{
    ULONG i = 0;
    BOOLEAN found = FALSE;
    ULONG nv = numVertices;
    
    while ((i < nv) && (vertices[i] < v))
        i++;
    if ((i < nv) && (vertices[i] == v))
        found = TRUE;
    return found;
}

//---------------------------------------------------------------------------
// NAME: AddInstanceToInstance
//
// INPUTS: (Instance *instance1) - instance to add
//         (Instance *instance2) - instance to add to
//
// RETURN: (void)
//
// PURPOSE: Add vertices and edges of instance1 to instance2.  Duplicates
// are not added.
//---------------------------------------------------------------------------

void Instance::AddInstanceToInstance(Instance *instance1)
{
    ULONG v1, v2, e1, e2, i;
    ULONG nv2, ne2;
    
    // search for vertices of instance1 in instance2; if not found, then add
    // them, keeping instance2's vertices in order
    v2 = 0;
    for (v1 = 0; v1 < instance1->getNumVertices(); v1++)
    {
        nv2 = numVertices;
        while ((v2 < nv2) && (vertices[v2] < instance1->vertices[v1]))
            v2++;
        if ((v2 == nv2) || (vertices[v2] > instance1->vertices[v1]))
        {
            // vertex not in instance2, so make room
            vertices = (ULONG *) realloc(vertices, (sizeof(ULONG) * (nv2 + 1)));
            if (vertices == NULL)
                utility::OutOfMemoryError("AddInstanceToInstance:instance2->vertices");
            for (i = nv2; i > v2; i--)
                vertices[i] = vertices[i-1];
            vertices[v2] = instance1->getVertices()[v1];
            numVertices++;
        }
    }
    // insert new edges from instance1 into instance2
    e2 = 0;
    for (e1 = 0; e1 < instance1->getNumEdges(); e1++)
    {
        ne2 = numEdges;
        while ((e2 < ne2) && (edges[e2] < instance1->edges[e1]))
            e2++;
        if ((e2 == ne2) || (edges[e2] > instance1->edges[e1]))
        {
            // edge not in instance2, so make room
            edges = (ULONG *) realloc(edges, (sizeof(ULONG) * (ne2 + 1)));
            if (edges == NULL)
                utility::OutOfMemoryError("AddInstanceToInstance:instance2->edges");
            for (i = ne2; i > e2; i--)
                edges[i] = edges[i-1];
            edges[e2] = instance1->getEdges()[e1];
            numEdges++;
        }
    }
}

//---------------------------------------------------------------------------
// NAME: AddEdgeToInstance
//
// INPUTS: (ULONG) edgeIndex - index of edge in graph
//         (Edge *edge) - edge to add
//         (Instance *instance2) - instance to add to
//
// RETURN: (void)
//
// PURPOSE: Add edge to instance.  Duplicate vertices or edges are not
// added.
//---------------------------------------------------------------------------

void Instance::AddEdgeToInstance(ULONG edgeIndex, LabeledEdge *edge)
{
    Instance *instance1;
    
    // convert edge to instance
    if (edge->getE().first  == edge->getE().second)
    { // self-edge
        instance1 = AllocateInstance(1, 1);
        instance1->getVertices()[0] = edge->getE().first;
    }
    else
    {
        instance1 = AllocateInstance(2, 1);
        if (edge->getE().first < edge->getE().second)
        {
            instance1->getVertices()[0] = edge->getE().first;
            instance1->getVertices()[1] = edge->getE().second;
        }
        else
        {
            instance1->getVertices()[0] = edge->getE().first;
            instance1->getVertices()[1] = edge->getE().second;
        }
    }
    instance1->getEdges()[0] = edgeIndex;
    AddInstanceToInstance( instance1);
    instance1->FreeInstance();
}


//---------------------------------------------------------------------------
// NAME: InstanceMatch
//
// INPUTS: (Instance *instance1)
//         (Instance *instance2)
//
// RETURN: (BOOLEAN) - TRUE if instance1 matches instance2
//
// PURPOSE: Determine if two instances are the same by checking if
// their vertices and edges arrays are the same.  NOTE: InstanceMatch
// assumes the instances' vertices and edges arrays are in increasing
// order.
//---------------------------------------------------------------------------

BOOLEAN Instance::InstanceMatch(Instance *instance2)
{
    ULONG i;
    
    // check that instances have same number of vertices and edges
    if ((this->numVertices != instance2->getNumVertices()) ||
        (this->numEdges != instance2->getNumEdges()))
        return FALSE;
    
    // check that instances have same edges
    for (i = 0; i < this->numEdges; i++)
        if (this->edges[i] != instance2->getEdges()[i])
            return FALSE;
    
    // check that instances have same vertices
    for (i = 0; i < this->numVertices; i++)
        if (this->vertices[i] != instance2->getVertices()[i])
            return FALSE;
    
    return TRUE;
}

//---------------------------------------------------------------------------
// NAME: InstanceListOverlap
//
// INPUTS: (Instance *instance) - instance to check for overlap
//         (InstanceList *instanceList) - instances to check for overlap with
//                                        instance
//
// RETURN: (BOOLEAN)
//
// PURPOSE: Check if given instance overlaps at all with any instance
// in the given instance list.
//---------------------------------------------------------------------------

BOOLEAN Instance::InstanceListOverlap( InstanceList *instanceList)
{
    InstanceListNode *instanceListNode;
    BOOLEAN overlap = FALSE;

        instanceListNode = instanceList->getHead();
        while ((instanceListNode != NULL) && (! overlap))
        {
            if (instanceListNode->getInstance() != NULL)
                if (InstanceOverlap( instanceListNode->getInstance()))
                    overlap = TRUE;
            instanceListNode = (InstanceListNode *)instanceListNode->getNext();
        }
 
    return overlap;
}


//------------------------------------------------------------------------------
// NAME: CopyInstance
//
// RETURN:  (Instance *) - new instance copy.
//
// PURPOSE:  Create a new copy of an instance.
//------------------------------------------------------------------------------

Instance *Instance::CopyInstance()
{
    Instance *newInstance;
    ULONG i;
    
    newInstance = AllocateInstance( numVertices,  numEdges);
    for (i = 0; i <  numVertices; i++)
        newInstance->getVertices()[i] =  vertices[i];
    for (i = 0; i < numEdges; i++)
        newInstance->getEdges()[i] = edges[i];
    
    return newInstance;
}


//-----------------------------------------------------------------------------
// NAME: InstanceToRefGraph
//
// INPUT: (Instance *instance) - the seed instance
//        (Graph *graph) - the complete graph
//        (Parameters *parameters) - system parms
//
// RETURN:  (ReferenceGraph *) - the new ref graph
//
// PURPOSE:  Convert an instance to a ref graph.  We use this to convert the
//           initial seed instances (2 vertices spanning the increment boundary)
//           to the start of our ref graph.  Remaps the instance to the new
//           ref graph.
//-----------------------------------------------------------------------------

ReferenceGraph *Instance::InstanceToRefGraph( LabeledGraph *graph,
                                   Params *parameters)
{
    ReferenceGraph *newGraph;
    LabeledVertex *vertex;
    LabeledEdge *edge;
    ULONG i, j;
    ULONG v1, v2;
    BOOLEAN found1;
    BOOLEAN found2;
    ReferenceVertex *refVertex;
    
    v1 = 0;
    v2 = 0;
    newGraph = ReferenceGraph::AllocateReferenceGraph(numVertices, numEdges);
    
    // convert vertices
    for (i = 0; i < numVertices; i++)
    {
        vertex = & graph->getVertices()[vertices[i]];
        newGraph->getVertices()[i].setLabel( vertex->getLabel()) ;
        newGraph->getVertices()[i].setNumEdges(0) ;
        newGraph->getVertices()[i].setEdges(NULL) ;
        newGraph->getVertices()[i].setUsed(FALSE) ;
        newGraph->getVertices()[i].setMap(vertices[i]);
        newGraph->getVertices()[i].setVertexValid(TRUE) ;
    }
    
    // convert edges
    for (i = 0; i < numEdges; i++)
    {
        edge = & graph->getEdges()[ edges[i]];
        // find new indices for edge vertices
        j = 0;
        found1 = FALSE;
        found2 = FALSE;
        while ((! found1) || (! found2))
        {
            if (vertices[j] == edge->getE().first)
            {
                v1 = j;
                found1 = TRUE;
            }
            if ( vertices[j] == edge->getE().second)
            {
                v2 = j;
                found2 = TRUE;
            }
            j++;
        }
        
        // set new edge information
        newGraph->getEdges()[i].setVertex1(v1) ;
        newGraph->getEdges()[i].setVertex2(v2);
        newGraph->getEdges()[i].setMap(edges[i]) ;
        newGraph->getEdges()[i].setLabel(edge->getLabel());
        newGraph->getEdges()[i].setDirected(edge->isDirected());
        newGraph->getEdges()[i].setUsed(FALSE) ;
        newGraph->getEdges()[i].setFailed(FALSE) ;
        
        // add edge to appropriate vertices
        refVertex = & newGraph->getVertices()[v1];
        refVertex->setVertexValid(TRUE) ; //this should be unnecessary
        refVertex->setNumEdges(refVertex->getNumEdges()+1);
        refVertex->setEdges( (ULONG *) realloc(refVertex->getEdges(),
                                               sizeof (ULONG) * refVertex->getNumEdges()));
        if (refVertex->getEdges() == NULL)
            utility::OutOfMemoryError ("InstanceToGraph:refVertex1->edges");
        refVertex->getEdges()[refVertex->getNumEdges() - 1] = i;
        if (v1 != v2)
        {
            refVertex = &newGraph->getVertices()[v2];
            refVertex->setVertexValid(TRUE) ;  // this should be unnecessary
            refVertex->setNumEdges(refVertex->getNumEdges()+1);
            refVertex->setEdges( (ULONG *) realloc(refVertex->getEdges(),
                                                              sizeof (ULONG) * refVertex->getNumEdges())) ;

            if (refVertex->getEdges() == NULL)
                utility::OutOfMemoryError ("InstanceToGraph:refVertex2->edges");
            refVertex->getEdges()[refVertex->getNumEdges() - 1] = i;
        }
    }
    
    // remap instance to refGraph
    for (i = 0; i <  numVertices; i++)
         vertices[i] = i;
    
    for (i = 0; i <  numEdges; i++)
         edges[i] = i;
    newVertex = 0;
     newEdge = 0;
    
    return newGraph;
}


//------------------------------------------------------------------------------
// NAME: CreateGraphRefInstance
//
// INPUT: (Instance *instance1) - instance referencing the ref graph
//        (ReferenceGraph *refGraph) - ref graph being referenced by instance
//
// RETURN:  (Instance *) - an instance referencing the full graph
//
// PURPOSE:  Take an instance that is referencing a ref graph and create an
//           instance that references the full graph.
//------------------------------------------------------------------------------

Instance *Instance::CreateGraphRefInstance( ReferenceGraph *refGraph)
{
    int i;
    Instance *instance2 =
    AllocateInstance(numVertices,  numEdges);
    
    for(i=0;i< numVertices;i++)
    {
        // assign the vertices to the full graph
        instance2->getVertices()[i] = refGraph->getVertices()[vertices[i]].getMap();
    }
    
    // reorder indices based on the vertices in the full graph
    utility::SortIndices(instance2->getVertices(), 0, instance2->getNumVertices()-1);
    
    for(i=0;i<numEdges;i++)
    {
        // assign the vertices to the full graph
        instance2->getEdges()[i] = refGraph->getEdges()[edges[i]].getMap();
    }
    
    // reorder indices based on the vertices in the full graph
    utility::SortIndices(instance2->getEdges(), 0, instance2->getNumEdges()-1);
    
    return instance2;
}

//----------------------------------------------------------------------------
// NAME: CheckInstanceForOverlap
//
// INPUT: (Instance *instance) - the new instance we are attempting to add to
//                               the increment
//        (Substructure *newSub) - contains the graph definition of the
//                                 instance needed for graph matching
//        (Parameters *parameters) - system parameters
//
// RETURN:  BOOLEAN
//
// PURPOSE:  Check for overlapping vertices in all previously collected
//           instances for a specific substructure.
//----------------------------------------------------------------------------

BOOLEAN Instance::CheckInstanceForOverlap( Substructure *newSub,
                                Params *parameters)
{
    AvlTable *avlTable;
    ULONG *vertices;
    ULONG *result;
    ULONG i;
    
    vertices = this->vertices;
    avlTable = newSub->GetSubTree( parameters);
    if (avlTable == NULL)
        return FALSE;
    
    for(i=0; i<numVertices; i++)
    {
        result = (ULONG *)avlTable->avl_find( &vertices[i]);
        if((result != NULL) && (*result == vertices[i]))
            return TRUE;
    }
    return FALSE;
}

//----------------------------------------------------------------------------
// NAME: AddInstanceToIncrement
//
// INPUT: (Substructure *newSub) - the new substructure being added to the
//                                 increment
//        (Instance instance) - the new instance found
//        (Parameters *parameters) - system parameters
//
// RETURN:  BOOLEAN
//
// PURPOSE:  We have found a new instance of a known sub so add it to that
//           sub's instance list.  If the sub does not exist in this increment,
//           we add it.  We are not actually storing the instances, just the
//           count.  We currently do not handle overlapping vertices, so
//           instead we check the uniqueness of each vertex in an instance
//           before we add it to a sub's instance list.
//----------------------------------------------------------------------------

BOOLEAN Instance::AddInstanceToIncrement(Substructure *newSub, Params *parameters)
{
    Increment *increment;
    SubList *subList;
    SubListNode *subListNode;
    Substructure *localSub;
    BOOLEAN found = FALSE;
    BOOLEAN overlapInOldIncrements = FALSE;
    BOOLEAN instanceAdded = FALSE;
    AvlTable *avlTable;
    
    increment = parameters->GetCurrentIncrement();
    subList = increment->getSubList();
    
    // check for the sub in the current increment's sublist
    subListNode = subList->getHead();
    while ((subListNode != NULL) && (!found))
    {
        if (newSub->getDefinition()->GraphMatch( subListNode->getInstance()->getDefinition(),
                       parameters->labelList, 0.0, NULL, NULL))
        {
            found = TRUE;
            // check for overlap in previous increments
            overlapInOldIncrements =
            CheckInstanceForOverlap( newSub, parameters);
            if (!overlapInOldIncrements)
            {
                // add the new instances to the sub
                subListNode->getInstance()->setNumInstances(subListNode->getInstance()->getNumInstances()+1);
                subListNode->getInstance()->AdjustMetrics( parameters);
                instanceAdded = TRUE;
                // need to reorder the subs now that we have updated the value
                // (if we care about local ordering)
            }
        }
        subListNode = (SubListNode *)subListNode->getNext();
    }
    // if sub not found in the current increment, then we add it
    if (!found)
    {
        // check for overlap in the previous increment
        overlapInOldIncrements =
        CheckInstanceForOverlap( newSub, parameters);
        if (!overlapInOldIncrements)
        {
            localSub = newSub->CopySub();
            localSub->setNumInstances(1) ;
            localSub->AdjustMetrics( parameters);
            subList->SubListInsert(localSub, 0, FALSE, parameters->labelList);
            instanceAdded = TRUE;
        }
    }
    if (instanceAdded)
    {
        if (parameters->outputLevel > 3)
            newSub->PrintSub( parameters);
        avlTable = newSub->GetSubTree( parameters);
        if (avlTable == NULL)
        {
            avlTable = AvlTable::avl_create((avl_comparison_func *)utility::compare_ints, NULL, NULL);
            parameters->AddInstanceVertexList(newSub, avlTable);
        }
        avlTable->AddInstanceToTree(this);
    }
    
    // indicates whether the instance was added
    return(instanceAdded);
}

//-----------------------------------------------------------------------------
// NAME:  ExtendConstrainedInstance
//
// INPUTS:  (Instance *instance) - current instance
//          (Substructure *bestSub) - currently best substructure
//          (ReferenceGraph *refGraph) - reference graph
//          (Graph *fullGraph) - complete graph
//          (Parameters *parameters) - system parameters
//
// RETURN:  (InstanceList *) - list of extended instances
//
// PURPOSE:  Extend an instance in all possible ways.
//-----------------------------------------------------------------------------

InstanceList *Instance::ExtendConstrainedInstance(
                                        Substructure *bestSub,
                                        ReferenceGraph *refGraph,
                                        LabeledGraph *fullGraph,
                                        Params *parameters)
{
    InstanceList *newInstanceList;
    Instance *newInstance;
    ULONG v;
    ULONG e;
    ULONG v2;
    ReferenceVertex *vertex;
    ReferenceVertex *vertex2;
    ReferenceEdge *edge;
    BOOLEAN listEmpty = TRUE;
    
    newInstanceList = NULL;
    refGraph->MarkRefGraphInstanceEdges(this, TRUE);
    for (v = 0; v < numVertices; v++)
    {
        vertex = &refGraph->getVertices()[vertices[v]];
        if (vertex->isVertexValid())
        {                // I do not think we can ever have an invalid vertex here
            for (e = 0; e < vertex->getNumEdges(); e++)
            {
                edge = &refGraph->getEdges()[vertex->getEdges()[e]];
                if (!edge->isUsed() && !edge->isFailed())
                {
                    // add new instance to list
                    if (edge->getVertex1() == vertices[v])
                        v2 = edge->getVertex2();
                    else
                        v2 = edge->getVertex1();
                    vertex2 = &refGraph->getVertices()[v2];
                    // Now check to see if this vertex is a candidate for expansion
                    if (bestSub->getDefinition()->VertexInSub(
                                    &fullGraph->getVertices()[vertex2->getMap()]) &&
                        !bestSub->CheckVertexForOverlap(vertex2->getMap(), parameters))
                    {
                        newInstance =
                        CreateConstrainedExtendedInstance(
                                                          vertices[v], v2, vertex->getEdges()[e], refGraph);
                        if (listEmpty)
                        {
                            newInstanceList = InstanceList::AllocateInstanceList();
                            listEmpty = FALSE;
                        }
                        newInstanceList->InstanceListInsert(newInstance, TRUE);
                    }
                }
            }
        }
    }
    refGraph->MarkRefGraphInstanceEdges(this, FALSE);
    
    return newInstanceList;
}


//--------------------------------------------------------------------------
// NAME:  CreateConstrainedExtendedInstance
//
// INPUTS: (Instance *instance) - current instance
//         (ULONG v) - vertex being extended from
//         (ULONG v2) - vertex being extended to
//         (ULONG e) - edge being extended
//         (ReferenceGraph *graph)
//
// RETURN: (Instance *) - extended instance
//
// PURPOSE:  Create one new extended instance.
//--------------------------------------------------------------------------

Instance *Instance::CreateConstrainedExtendedInstance( ULONG v,
                                            ULONG v2, ULONG e,
                                            ReferenceGraph *graph)
{
    Instance *newInstance;
    BOOLEAN found = FALSE;
    ULONG i;
    
    // check if edge's other vertex is already in instance
    for (i = 0; ((i < numVertices) && (! found)); i++)
    {
        if (vertices[i] == v2)
            found = TRUE;
    }
    
    if (!found)
        newInstance = Instance::AllocateInstance(numVertices + 1, numEdges + 1);
    else
        newInstance = AllocateInstance(numVertices, numEdges + 1);
    
    // Set vertices of new instance, kept in increasing order
    for (i = 0; i < numVertices; i++)
        newInstance->getVertices()[i] = vertices[i];
    newInstance->setNewVertex(VERTEX_UNMAPPED) ;
    if (!found)
    {
        i = numVertices;
        while ((i > 0) && (v2 < newInstance->getVertices()[i-1]))
        {
            newInstance->getVertices()[i] = newInstance->getVertices()[i-1];
            i--;
        }
        newInstance->vertices[i] = v2;
        newInstance->newVertex = i;
    }
    
    // set edges of new instance, kept in increasing order
    for (i = 0; i < numEdges; i++)
        newInstance->getEdges()[i] = edges[i];
    i = numEdges;
    while ((i > 0) && (e < newInstance->getEdges()[i-1]))
    {
        newInstance->getEdges()[i] = newInstance->getEdges()[i-1];
        i--;
    }
    newInstance->getEdges()[i] = e;
    newInstance->setNewEdge(i) ;
    
    return newInstance;
}















