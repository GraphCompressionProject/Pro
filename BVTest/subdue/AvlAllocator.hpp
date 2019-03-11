//
// Created by Macbook on 11/02/2019.
//

#ifndef SUBDUE_AVLALLOCATOR_HPP
#define SUBDUE_AVLALLOCATOR_HPP

#include <stddef.h>
#include <assert.h>
using namespace std;

    class AvlAllocator {
        
    public:
        /* Allocates |size| bytes of space using |malloc()|.
         Returns a null pointer if allocation fails. */
        void *avl_malloc ( size_t size)
        {
            assert ( size > 0);
            return malloc (size);
        }
        
        /* Frees |block|. */
        void avl_free ( void *block)
        {
            assert ( block != NULL);
            free (block);
        }
        
    };



#endif //SUBDUE_AVLALLOCATOR_HPP
