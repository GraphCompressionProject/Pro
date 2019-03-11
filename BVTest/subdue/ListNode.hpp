//
// Created by Macbook on 10/02/2019.
//

#ifndef SUBDUE_LISTNODE_HPP
#define SUBDUE_LISTNODE_HPP

    template<class T>
    class ListNode {
    protected:
        T *instance;
        ListNode<T> *next;
    public:
        
        ~ListNode();
        
        T *getInstance() const {
            return instance;
        }
        
        void setInstance(T *instance) {
            ListNode::instance = instance;
        }
        
        ListNode<T> *getNext() const {
            return next;
        }
        
        void setNext(ListNode<T> *next) {
            ListNode::next = next;
        }
        
        
    };




#endif //SUBDUE_INSTANCELISTNODE_HPP
