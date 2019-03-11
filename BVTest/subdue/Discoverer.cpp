//
//  Discoverer.cpp
//  Subdue_v1
//
//  Created by Macbook on 18/02/2019.
//  Copyright © 2019 Macbook. All rights reserved.
//

#include "stdafx.h"

#include "Discoverer.hpp"

#include "Label.hpp"

#include "Substructure.hpp"
#include "LabeledGraph.hpp"
#include "Instance.hpp"
#include "IncrementList.hpp"
#include "IncrementListNode.hpp"
#include "SubList.hpp"


//---------------------------------------------------------------------------
// NAME: DiscoverSubs
//
// INPUTS: (Parameters *parameters)
//
// RETURN: (SubList) - list of best discovered substructures
//
// PURPOSE: Discover the best substructures in the graphs according to
// the given parameters.  Note that we do not allow a single-vertex
// substructure of the form "SUB_#" on to the discovery list to avoid
// continually replacing "SUB_<n>" with "SUB_<n+1>".
//---------------------------------------------------------------------------

SubList *Discoverer::DiscoverSubs(Params *parameters)
{
   SubList *parentSubList;
   SubList *childSubList;
   SubList *extendedSubList;
   SubList *discoveredSubList;
   SubListNode *parentSubListNode;
   SubListNode *extendedSubListNode;
   Substructure *parentSub;
   Substructure *extendedSub;
   Substructure *recursiveSub = NULL;

   // parameters used
   unsigned long limit          = parameters->limit;
   unsigned long numBestSubs    = parameters->numBestSubs;
   unsigned long beamWidth      = parameters->beamWidth;
   bool valueBased   = parameters->valueBased;
   LabelList *labelList = parameters->labelList;
   bool prune        = parameters->prune;
   unsigned long maxVertices    = parameters->maxVertices;
   unsigned long minVertices    = parameters->minVertices;
   unsigned long outputLevel    = parameters->outputLevel;
   bool recursion    = parameters->recursion;
   unsigned long evalMethod     = parameters->evalMethod;

  // get initial one-vertex substructures

   parentSubList = GetInitialSubs(parameters);
   
   //parentSubList->PrintSubList(parameters);

   discoveredSubList = SubList::AllocateSubList();

   while ((limit > 0) && (parentSubList->getHead() != NULL))
   {
      parentSubListNode = parentSubList->getHead();
      childSubList = SubList::AllocateSubList();
      // extend each substructure in parent list

      while (parentSubListNode != NULL)
      {
         parentSub = parentSubListNode->getInstance();
         parentSubListNode->setInstance(NULL);

         if (outputLevel > 4)
         {
            parameters->outputLevel = 1; // turn off instance printing
            printf("\nConsidering ");
            parentSub->PrintSub(parameters);
            printf("\n");
            parameters->outputLevel = outputLevel;
         }

         if ((((parentSub->getNumInstances() > 1) && (evalMethod != EVAL_SETCOVER)) || (parentSub->getNumNegInstances() > 0)) && (limit > 0))
         {

            limit--;
            if (outputLevel > 3)
               printf("%lu substructures left to be considered\n", limit);
            fflush(stdout);


            extendedSubList = parentSub->ExtendSub(parameters);

			

            extendedSubListNode = extendedSubList->getHead();

            while (extendedSubListNode != NULL)
            {
               extendedSub = extendedSubListNode->getInstance();
               extendedSubListNode->setInstance(NULL) ;

               if (extendedSub->getDefinition()->getNumVertices() <= maxVertices)
               {
                  // evaluate each extension and add to child list
                  extendedSub->EvaluateSub(parameters);
                  if (prune && (extendedSub->getValue() < parentSub->getValue()))
                  {
                     delete extendedSub;
                  }
                  else
                  {
                     childSubList->SubListInsert(extendedSub, beamWidth,
                                   valueBased, labelList);
                  }
               }
               else
               {
                     
                  delete extendedSub;
                    
               }
               extendedSubListNode = (SubListNode *)extendedSubListNode->getNext();

            }

            delete extendedSubList;

         }
         

         // add parent substructure to final discovered list
         if (parentSub->getDefinition()->getNumVertices() >= minVertices)
         {
            if (! SinglePreviousSub(parentSub, parameters))
            {
               // consider recursive substructure, if requested
               if (recursion)
                  recursiveSub = parentSub->RecursifySub( parameters);

               if (outputLevel > 3)
                  parentSub->PrintNewBestSub(discoveredSubList, parameters);
               



               discoveredSubList->SubListInsert(parentSub, numBestSubs, FALSE,
                             labelList);
               if (recursion && (recursiveSub != NULL))
               {
                  if (outputLevel > 4)
                  {
                     parameters->outputLevel = 1; // turn off instance printing
                     printf("\nConsidering Recursive ");
                     recursiveSub->PrintSub( parameters);
                     printf ("\n");
                     parameters->outputLevel = outputLevel;
                  }
                  if (outputLevel > 3)
                     recursiveSub->PrintNewBestSub( discoveredSubList, parameters);
                  discoveredSubList->SubListInsert(recursiveSub, numBestSubs,
                                FALSE, labelList);
               }

            }
         }
         else
         {
            delete parentSub;
         }
         parentSubListNode = (SubListNode *)  parentSubListNode->getNext();
      }

      delete parentSubList;
      parentSubList = childSubList;
   }

   if ((limit > 0) && (outputLevel > 2))
      printf ("\nSubstructure queue empty.\n");

   // try to insert any remaining subs in parent list on to discovered list
   parentSubListNode = parentSubList->getHead();
   while (parentSubListNode != NULL)
   {
      parentSub = parentSubListNode->getInstance();
      parentSubListNode->setInstance(NULL);
      if (parentSub->getDefinition()->getNumVertices() >= minVertices)
      {
         if (! SinglePreviousSub(parentSub, parameters))
         {
            if (outputLevel > 3)
               parentSub->PrintNewBestSub( discoveredSubList, parameters);
            discoveredSubList->SubListInsert(parentSub, numBestSubs, FALSE, labelList);
         }
      }
      else
      {
         delete parentSub;
      }
      parentSubListNode = (SubListNode *) parentSubListNode->getNext();
   }
   delete parentSubList;
   return discoveredSubList;
}


//---------------------------------------------------------------------------
// NAME: GetInitialSubs
//
// INPUTS: (Parameters *parameters)
//
// RETURN: (SubList *)
//
// PURPOSE: Return a list of substructures, one for each unique vertex
// label in the positive graph that has at least two instances.
//---------------------------------------------------------------------------

SubList *Discoverer::GetInitialSubs(Params *parameters)
{
   SubList *initialSubs;
   unsigned long i, j;
   unsigned long vertexLabelIndex;
   unsigned long numInitialSubs;
   LabeledGraph *g;
   Substructure *sub;
   Instance *instance;

   // parameters used
   LabeledGraph *posGraph      = parameters->posGraph;
   LabeledGraph *negGraph      = parameters->negGraph;
   LabelList *labelList = parameters->labelList;
   unsigned long outputLevel    = parameters->outputLevel;
   unsigned long currentIncrement = 0;
   unsigned long startVertexIndex;

   if (parameters->incremental)
   {
      currentIncrement = parameters->GetCurrentIncrementNum();
      // Index for first vertex in increment
      // Begin with the index for the first vertex in this increment and
      // move up through all remaining vertices.  Relies on the fact that
      // each new increment is placed on the end of the vertex array and that
      // we are only interested in the current (last) increment
      startVertexIndex = parameters->incrementList[currentIncrement].GetStartVertexIndex(POS);
      if (parameters->outputLevel > 2)
         printf("Start vertex index = %lu\n", startVertexIndex);
   }
   else
      startVertexIndex = 0;



   // reset labels' used flag
   for (i = 0; i < labelList->getNumLabels(); i++)
      labelList->getHead()[i].setused(FALSE);
  
   numInitialSubs = 0;
   initialSubs = SubList::AllocateSubList();

   for (i = startVertexIndex; i < posGraph->getNumVertices(); i++)
   {
      vertexLabelIndex = posGraph->getVertices()[i].getLabel();


      if (! labelList->getHead()[vertexLabelIndex].is_used())
      {
          labelList->getHead()[vertexLabelIndex].setused(TRUE);

         // create one-vertex substructure definition
          g = LabeledGraph::AllocateGraph(1, 0);
          g->getVertices()->setLabel(vertexLabelIndex);
          g->getVertices()->setNumEdges(0);
          g->getVertices()->setEdges(NULL);

         
         // allocate substructure
          sub = Substructure::AllocateSub();
          sub->setDefinition (g);
          sub->setInstances(InstanceList::AllocateInstanceList());
         // collect instances in positive graph
         j = posGraph->getNumVertices();
         do
         {
            j--;
            if (posGraph->getVertices()[j].getLabel() == vertexLabelIndex)
            {
               // ***** do inexact label matches here? (instance->minMatchCost
               // ***** too)
                instance = Instance::AllocateInstance(1, 0);
               instance->getVertices()[0]=j;
               instance->getMapping()->setV1(0);
               instance->getMapping()->setV2(j);
               instance->setMinMatchCost(0.0) ;
               sub->getInstances()->InstanceListInsert(instance, FALSE);
               sub->setNumInstances(sub->getNumInstances()+1);
            }
         } while (j > i);
         // only keep substructure if more than one positive instance
         if (sub->getNumInstances() > 1)
         {
            if (negGraph != NULL)
            {
               // collect instances in negative graph
                sub->setNegInstances(InstanceList::AllocateInstanceList()) ;
               j = negGraph->getNumVertices();
               if (parameters->incremental)
                  startVertexIndex =    parameters->incrementList[currentIncrement].GetStartVertexIndex(POS);
               else
                  startVertexIndex = 0;
               do
               {
                  j--;
                  if (negGraph->getVertices()[j].getLabel() == vertexLabelIndex)
                  {
                     // ***** do inexact label matches here?
                     // ***** (instance->minMatchCost too)
                      instance = Instance::AllocateInstance(1, 0);
                      instance->getVertices()[0]=j;
                     instance->getMapping()[0].setV1(0) ;
                     instance->getMapping()[0].setV2(j) ;
                     instance->setMinMatchCost(0.0);
                     sub->getNegInstances()->InstanceListInsert(instance, FALSE);
                     sub->setNumNegInstances(sub->getNumNegInstances()+1);
                  }
                              // We need to try all negative graph labels
               } while (j > startVertexIndex);
            }
            sub->EvaluateSub( parameters);
            // add to initialSubs
            initialSubs->SubListInsert(sub, 0, FALSE, labelList);
            numInitialSubs++;
         }
         else
         { // prune single-instance substructure
            delete sub;
         }
      }
   }
   if (outputLevel > 1)
      printf("%lu initial substructures\n", numInitialSubs);


   return initialSubs;
}


//---------------------------------------------------------------------------
// NAME: SinglePreviousSub
//
// INPUTS: (Substructure *sub) - substructure to check
//         (Parameters *parameters)
//
// RETURN: (bool)
//
// PURPOSE: Returns TRUE if the given substructure is a single-vertex
// substructure and the vertex refers to a previously-discovered
// substructure, i.e., the vertex label is of the form "SUB_#".  This
// is used to prevent repeatedly compressing the graph by replacing a
// "SUB_<n>" vertex by a "SUB_<n+1>" vertex.
//---------------------------------------------------------------------------

bool Discoverer::SinglePreviousSub(Substructure *sub, Params *parameters)
{
   bool match;
   // parameters used
   LabelList *labelList = parameters->labelList;
   unsigned long tmp =  labelList->SubLabelNumber(sub->getDefinition()->getVertices()[0].getLabel());
   match = FALSE;
   if ((sub->getDefinition()->getNumVertices()== 1) &&  // single-vertex sub?
       (tmp > 0))
       // valid substructure label
      match = TRUE;

   return match;
}
