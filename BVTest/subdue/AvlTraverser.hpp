//
// Created by Macbook on 10/02/2019.
//

#ifndef SUBDUE_AVLTRAVERSER_HPP
#define SUBDUE_AVLTRAVERSER_HPP
#include <stddef.h>
/* Maximum AVL height. */
#ifndef AVL_MAX_HEIGHT
#define AVL_MAX_HEIGHT 32
#endif

#include "AvlTable.hpp"
#include "AvlNode.hpp"
#include <iostream>

    class AvlTraverser {
    private:
        AvlTable *avl_table;        /* Tree being traversed. */
        AvlNode *avl_node;          /* Current node in tree. */
        AvlNode *avl_stack[AVL_MAX_HEIGHT];
        /* All the nodes above |avl_node|. */
        size_t avl_height;                  /* Number of nodes in |avl_parent|. */
        unsigned long avl_generation;
        
    public:
        AvlTable *getAvl_table() {
            return avl_table;
        }
        
        void setAvl_table(AvlTable *avl_table) {
            AvlTraverser::avl_table = avl_table;
        }
        
        AvlNode *getAvl_node() const {
            return avl_node;
        }
        
        void setAvl_node(AvlNode *avl_node) {
            AvlTraverser::avl_node = avl_node;
        }
        
        AvlNode *const *getAvl_stack() {
            return avl_stack;
        }

        void setAvl_stack(AvlNode *t[AVL_MAX_HEIGHT]) {
            for (int i=0; i<AVL_MAX_HEIGHT; i++) {
                avl_stack[i] =t[i];
            }
            
        }
        
        size_t getAvl_height()  {
            return avl_height;
        }
        
        void setAvl_height(size_t avl_height) {
            AvlTraverser::avl_height = avl_height;
        }
        
        unsigned long getAvl_generation() {
            return avl_generation;
        }
        
        void setAvl_generation(unsigned long avl_generation) {
            AvlTraverser::avl_generation = avl_generation;
        }
 
        
        void avl_t_init ( AvlTable *);
        void *avl_t_first ( AvlTable *);
        void *avl_t_last ( AvlTable *);
        void *avl_t_find ( AvlTable *, void *);
        void *avl_t_insert ( AvlTable *, void *);
        void *avl_t_next ();
        void *avl_t_prev ();
        void *avl_t_cur ();
        void *avl_t_replace ( void *);
        void trav_refresh ();
        
        /* Initializes |trav| to have the same current node as |src|. */
        static void *
        avl_t_copy (AvlTraverser *trav, AvlTraverser *src)
        {
            
            assert ( src != NULL);
            
            trav->avl_table = src->getAvl_table();
            trav->avl_node = src->getAvl_node();
            trav->avl_generation = src->getAvl_generation();
            if (trav->getAvl_generation() == trav->getAvl_table()->getAvl_generation())
            {
                trav->setAvl_height( src->getAvl_height());
                std::memcpy ((void *)trav->getAvl_stack(), (const void *) src->getAvl_stack(),
                        sizeof *trav->getAvl_stack() * trav->getAvl_height());
            }
            
            
            return trav->getAvl_node() != NULL ? trav->getAvl_node()->getAvl_data() : NULL;
        }
        
        
    };





#endif //SUBDUE_AVLTRAVERSER_HPP
