//
// Created by Macbook on 10/02/2019.
//

#ifndef SUBDUE_REFINSTANCELIST_HPP
#define SUBDUE_REFINSTANCELIST_HPP


#include "RefInstanceListNode.hpp"
#include "List.hpp"




class RefInstanceList : public List<RefInstanceListNode> {
    public :
    
    
    
    //------------------------------------------------------------------------------
    // NAME: AllocateRefInstanceList
    //
    // RETURN:  (RefInstanceList *)
    //
    // PURPOSE:  Create a RefInstanceList.
    //------------------------------------------------------------------------------
    void RemoveFromList(
                                         RefInstanceListNode **currentNode);
    static RefInstanceList *AllocateRefInstanceList()
    {
        RefInstanceList *refInstanceList = (RefInstanceList *)malloc(sizeof(RefInstanceList));
        
        refInstanceList->setHead(NULL) ;
        
        return refInstanceList;
    }
    
    
    void ProcessInstancesForSub(Substructure *bestSub,
                                                 InstanceList *finalInstanceList,
                                                 Params *parameters);
 
        
    };





#endif //SUBDUE_REFINSTANCELIST_HPP
