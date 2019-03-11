//
// Created by Macbook on 10/02/2019.
//

#ifndef SUBDUE_AVLTABLENODE_HPP
#define SUBDUE_AVLTABLENODE_HPP


#include "ListNode.hpp"
#include "AvlTable.hpp"
#include "Substructure.hpp"

class AvlTableNode : ListNode<AvlTable>{
    private:
        
        Substructure *sub;
        
    public:
    
    AvlTable *getInstance() const {
        return ListNode<AvlTable>::getInstance();
    }
    
    void setInstance(AvlTable *instance) {
        ListNode::setInstance(instance) ;
    }
    
    AvlTableNode *getNext() const {
        return (AvlTableNode *)ListNode<AvlTable>::getNext();
    }
    
    void setNext(AvlTableNode *next) {
        ListNode<AvlTable>::setNext(next) ;
    }
        
        Substructure *getSub() const {
            return sub;
        };
        
        void setSub(Substructure *sub) {
            AvlTableNode::sub = sub;
        };
        
    };



#endif //SUBDUE_AVLTABLENODE_HPP
