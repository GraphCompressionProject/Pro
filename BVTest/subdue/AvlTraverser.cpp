

#include "stdafx.h"
#include "AvlTraverser.hpp"
#include "AvlTable.hpp"
#include <stddef.h>





/* Initializes |trav| for use with |tree|
 and selects the null node. */
void
AvlTraverser::avl_t_init (AvlTable *tree)
{
    avl_table = tree;
    avl_node = NULL;
    avl_height = 0;
    avl_generation = tree->getAvl_generation();
}

/* Initializes |trav| for |tree|
 and selects and returns a pointer to its least-valued item.
 Returns |NULL| if |tree| contains no nodes. */
void *
AvlTraverser::avl_t_first (AvlTable *tree)
{
    AvlNode *x;
    
    avl_table = tree;
    avl_height = 0;
    avl_generation = tree->getAvl_generation();
    
    x = tree->getAvl_root();
    if (x != NULL)
        while (x->getAvl_link(0) != NULL)
        {
            assert (avl_height < AVL_MAX_HEIGHT);
            avl_stack[avl_height++] = x;
            x = x->getAvl_link(0);
        }
    avl_node = x;
    
    return x != NULL ? x->getAvl_data() : NULL;
}

/* Initializes |trav| for |tree|
 and selects and returns a pointer to its greatest-valued item.
 Returns |NULL| if |tree| contains no nodes. */
void *
AvlTraverser::avl_t_last (AvlTable *tree)
{
    AvlNode *x;
    
    assert (tree != NULL );
    
    avl_table = tree;
    avl_height = 0;
    avl_generation = tree->getAvl_generation();
    
    x = tree->getAvl_root();
    if (x != NULL)
        while (x->getAvl_link(1) != NULL)
        {
            assert (avl_height < AVL_MAX_HEIGHT);
            avl_stack[avl_height++] = x;
            x = x->getAvl_link(1);
        }
    avl_node = x;
    
    return x != NULL ? x->getAvl_data() : NULL;
}

/* Searches for |item| in |tree|.
 If found, initializes |trav| to the item found and returns the item
 as well.
 If there is no matching item, initializes |trav| to the null item
 and returns |NULL|. */
void *
AvlTraverser::avl_t_find (AvlTable *tree, void *item)
{
    AvlNode *p, *q;
    
    assert ( tree != NULL && item != NULL);
    avl_table = tree;
    avl_height = 0;
    avl_generation = tree->getAvl_generation();
    for (p = tree->getAvl_root(); p != NULL; p = q)
    {
        int cmp = tree->getAvl_compare() (item, p->getAvl_data(), tree->getAvl_param());
        
        if (cmp < 0)
            q = p->getAvl_link(0);
        else if (cmp > 0)
            q = p->getAvl_link(1);
        else /* |cmp == 0| */
        {
            avl_node = p;
            return p->getAvl_data();
        }
        
        assert (avl_height < AVL_MAX_HEIGHT);
        avl_stack[avl_height++] = p;
    }
    
    avl_height = 0;
    avl_node = NULL;
    return NULL;
}

/* Attempts to insert |item| into |tree|.
 If |item| is inserted successfully, it is returned and |trav| is
 initialized to its location.
 If a duplicate is found, it is returned and |trav| is initialized to
 its location.  No replacement of the item occurs.
 If a memory allocation failure occurs, |NULL| is returned and |trav|
 is initialized to the null item. */
void *
AvlTraverser::avl_t_insert (AvlTable *tree, void *item)
{
    void *p;
    
    assert (tree != NULL && item != NULL);
    
    p = tree->avl_probe ( item);
    if (p != NULL)
    {
        avl_table = tree;
//        avl_node =
//        ((AvlNode *)
//         ((char *) p - offsetof (AvlNode, avl_data)));
        avl_generation = tree->getAvl_generation() - 1;
        return p;
    }
    else
    {
        avl_t_init ( tree);
        return NULL;
    }
}

/* Returns the next data item in inorder
 within the tree being traversed with |trav|,
 or if there are no more data items returns |NULL|. */
void *
AvlTraverser::avl_t_next ()
{
    AvlNode *x;
    
    
    if (avl_generation != avl_table->getAvl_generation())
        trav_refresh();
    
    x = avl_node;
    if (x == NULL)
    {
        return avl_t_first (avl_table);
    }
    else if (x->getAvl_link(1) != NULL)
    {
        assert (avl_height < AVL_MAX_HEIGHT);
        avl_stack[avl_height++] = x;
        x = x->getAvl_link(1);
        
        while (x->getAvl_link(0) != NULL)
        {
            assert (avl_height < AVL_MAX_HEIGHT);
            avl_stack[avl_height++] = x;
            x = x->getAvl_link(0);
        }
    }
    else
    {
        AvlNode *y;
        
        do
        {
            if (avl_height == 0)
            {
                avl_node = NULL;
                return NULL;
            }
            
            y = x;
            x = avl_stack[--avl_height];
        }
        while (y == x->getAvl_link(1));
    }
    avl_node = x;
    
    return x->getAvl_data();
}

/* Returns the previous data item in inorder
 within the tree being traversed with |trav|,
 or if there are no more data items returns |NULL|. */
void *
AvlTraverser::avl_t_prev ()
{
    AvlNode *x;
    
    if (avl_generation != avl_table->getAvl_generation())
        trav_refresh ();
    
    x = avl_node;
    if (x == NULL)
    {
        return avl_t_last (avl_table);
    }
    else if (x->getAvl_link(0) != NULL)
    {
        assert (avl_height < AVL_MAX_HEIGHT);
        avl_stack[avl_height++] = x;
        x = x->getAvl_link(0);
        
        while (x->getAvl_link(1) != NULL)
        {
            assert (avl_height < AVL_MAX_HEIGHT);
            avl_stack[avl_height++] = x;
            x = x->getAvl_link(1);
        }
    }
    else
    {
        AvlNode *y;
        
        do
        {
            if (avl_height == 0)
            {
                avl_node = NULL;
                return NULL;
            }
            
            y = x;
            x = avl_stack[--avl_height];
        }
        while (y == x->getAvl_link(0));
    }
    avl_node = x;
    
    return x->getAvl_data();
}

/* Returns |trav|'s current item. */
void *
AvlTraverser::avl_t_cur ()
{
    return avl_node != NULL ? avl_node->getAvl_data(): NULL;
}

/* Replaces the current item in |trav| by |new| and returns the item replaced.
 |trav| must not have the null item selected.
 The new item must not upset the ordering of the tree. */
void *
AvlTraverser::avl_t_replace ( void *new_)
{
    void *old;
    assert ( avl_node != NULL && new_ != NULL);
    old = avl_node->getAvl_data();
    avl_node->setAvl_data((Labval *)new_) ;
    return old;
}



/* Refreshes the stack of parent pointers in |trav|
 and updates its generation number. */
void
AvlTraverser::trav_refresh ()
{
    
    avl_generation = avl_table->getAvl_generation();
    
    if (avl_node != NULL)
    {
        avl_comparison_func *cmp = avl_table->getAvl_compare();
        void *param = avl_table->getAvl_param();
        AvlNode *node = avl_node;
        AvlNode *i;
        
        avl_height = 0;
        for (i = avl_table->getAvl_root(); i != node; )
        {
            assert (avl_height < AVL_MAX_HEIGHT);
            assert (i != NULL);
            
            avl_stack[avl_height++] = i;
            i = i->getAvl_link(cmp (node->getAvl_data(), i->getAvl_data(), param) > 0);
        }
    }
}




























































