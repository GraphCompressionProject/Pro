//
// Created by Macbook on 10/02/2019.
//

#ifndef SUBDUE_AVLNODE_HPP
#define SUBDUE_AVLNODE_HPP
#include "Labval.hpp"


    class AvlNode {
    private:
        AvlNode *avl_link[2];       /* Subtrees. */
        Labval *avl_data;                /* Pointer to data. */
        signed char avl_balance;
        
    public:
        AvlNode *getAvl_link(int p) const {
            return avl_link[p];
        }
        void setAvl_link(AvlNode *p,int i)  {
             avl_link[i]=p;
        }
        
        Labval *getAvl_data() const {
            return avl_data;
        }
        
        void setAvl_data(Labval *avl_data) {
            AvlNode::avl_data = avl_data;
        }
        
        signed char getAvl_balance() const {
            return avl_balance;
        }
        
        void setAvl_balance(signed char avl_balance) {
            AvlNode::avl_balance = avl_balance;
        }
        
    };




#endif //SUBDUE_AVLNODE_HPP
