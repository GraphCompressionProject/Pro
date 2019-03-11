//
// Created by Macbook on 10/02/2019.
//

#ifndef SUBDUE_AVLTABLE_HPP
#define SUBDUE_AVLTABLE_HPP

#include "AvlNode.hpp"
#include <stddef.h>
#include "AvlAllocator.hpp"
#include <assert.h>

/* Maximum AVL height. */
#ifndef AVL_MAX_HEIGHT
#define AVL_MAX_HEIGHT 32
#endif

#ifndef LIBAVL_ALLOCATOR
#define LIBAVL_ALLOCATOR
/* Memory allocator. */
struct libavl_allocator
{
    void *(*libavl_malloc) (AvlAllocator *, size_t libavl_size);
    void (*libavl_free) (AvlAllocator *, void *libavl_block);
};
#endif

/* Default memory allocator. */
extern AvlAllocator avl_allocator_default;
void *avl_malloc (AvlAllocator *, size_t);
void avl_free (AvlAllocator *, void *);



class Instance;

typedef int avl_comparison_func (const void *avl_a, const void *avl_b,
void *avl_param);
typedef void avl_item_func (void *avl_item, void *avl_param);
typedef void *avl_copy_func (void *avl_item, void *avl_param);
    
    class AvlTable {
    private:
        AvlNode *avl_root;          /* Tree's root. */
        void *avl_param;                    /* Extra argument to |avl_compare|. */
        AvlAllocator *avl_alloc; /* Memory allocator. */
        size_t avl_count;                   /* Number of items in tree. */
        unsigned long avl_generation;
        avl_comparison_func *avl_compare;
    public:
        
        void setAvlCompare(avl_comparison_func *avl_compare){
            this->avl_compare = avl_compare;
        }
        
        avl_comparison_func *getAvl_compare(){
            return this->avl_compare;
        }
        
        
        
        AvlNode *getAvl_root() const {
            return avl_root;
        }

        void setAvl_root(AvlNode *avl_root) {
            AvlTable::avl_root = avl_root;
        }

        void *getAvl_param() const {
            return avl_param;
        }

        void setAvl_param(void *avl_param) {
            AvlTable::avl_param = avl_param;
        }

        AvlAllocator *getAvl_alloc() const {
            return avl_alloc;
        }

        void setAvl_alloc(AvlAllocator *avl_alloc) {
            AvlTable::avl_alloc = avl_alloc;
        }

        size_t getAvl_count() const {
            return avl_count;
        }

        void setAvl_count(size_t avl_count) {
            AvlTable::avl_count = avl_count;
        }

        unsigned long getAvl_generation() const {
            return avl_generation;
        }

        void setAvl_generation(unsigned long avl_generation) {
            AvlTable::avl_generation = avl_generation;
        }
        /* Generation number. */
        
        void AddInstanceToTree( Instance *instance);

        AvlTable *avl_copy ( avl_copy_func *,
                                    avl_item_func *, AvlAllocator*);
        void avl_destroy ( avl_item_func *);
        void *avl_probe ( void *);
        void *avl_insert ( void *);
        void *avl_replace ( void *);
        void *avl_delete ( const void *);
        void *avl_find ( const void *);
        void avl_assert_insert ( void *);
        void *avl_assert_delete ( void *);
        
        /* Creates and returns a new table
         with comparison function |compare| using parameter |param|
         and memory allocator |allocator|.
         Returns |NULL| if memory allocation failed. */
        static AvlTable *avl_create (avl_comparison_func *compare, void *param, AvlAllocator *allocator)
        {
            AvlTable *tree;
            
            assert (compare != NULL);
            
//            if (allocator == NULL)
//                allocator = &avl_allocator_default;
            
            tree = (AvlTable *) allocator->avl_malloc( sizeof(*tree));
            if (tree == NULL)
                return NULL;
            
            tree->setAvl_root(NULL) ;
            tree->setAvlCompare(compare) ;
            tree->setAvl_param(param) ;
            tree->setAvl_alloc(allocator);
            tree->setAvl_count(0) ;
            tree->setAvl_generation(0) ;
            
            return tree;
        }

        static void
        copy_error_recovery (AvlNode **stack, int height,
                             AvlTable *new_, avl_item_func *destroy)
        {
            assert (stack != NULL && height >= 0 && new_ != NULL);
            
            for (; height > 2; height -= 2)
                stack[height - 1]->setAvl_link(NULL, 1);
            new_->avl_destroy (destroy);
        }
        
        
        
        
    };

    


#endif //SUBDUE_AVLTABLE_HPP
