//
// Created by Macbook on 10/02/2019.
//

#ifndef SUBDUE_INSTANCELIST_HPP
#define SUBDUE_INSTANCELIST_HPP


#include <stdarg.h>


 
    
    template<class T>
    class List {
    protected:
        T *head;
        
    public:
        ~List(){
            free(head);
        }
        T *getHead() const {
            return head;
        }
        
        void setHead(T *head) {
            List::head = head;
        }
        
        
    };



#endif //SUBDUE_INSTANCELIST_HPP
