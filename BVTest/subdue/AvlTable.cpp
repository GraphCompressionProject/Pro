//
// Created by Macbook on 10/02/2019.
//

#include "stdafx.h"
#include "AvlTable.hpp"
#include "Instance.hpp"
#include "AvlAllocator.hpp"
#include "AvlNode.hpp"
#include "Labval.hpp"
//---------------------------------------------------------------------------
// NAME:  AddInstanceToTree
//
// INPUTS:  (struct avl_table *avlTable) - tree being traversed
//          (Instance *instance) - substructure instance
//
// RETURN:  void
//
// PURPOSE:  Add a substructure instance to the avl tree.
//---------------------------------------------------------------------------

void AvlTable::AddInstanceToTree( Instance *instance)
{
    ULONG *vertices;
    ULONG *j;
    ULONG i;
    
    vertices = instance->getVertices();
    for (i=0;i<instance->getNumVertices();i++)
    {
        j = (ULONG*) malloc(sizeof(ULONG));
        *j = vertices[i];
        avl_insert( j);
    }
}


/* Copies |org| to a new_ly created tree, which is returned.
 If |copy != NULL|, each data item in |org| is first passed to |copy|,
 and the return values are inserted into the tree,
 with |NULL| return values taken as indications of failure.
 On failure, destroys the partially created new_ tree,
 applying |destroy|, if non-null, to each item in the new_ tree so far,
 and returns |NULL|.
 If |allocator != NULL|, it is used for allocation in the new_ tree.
 Otherwise, the same allocator used for |org| is used. */
AvlTable * AvlTable::avl_copy ( avl_copy_func *copy,
          avl_item_func *destroy, AvlAllocator *allocator)
{
    AvlNode *stack[2 * (AVL_MAX_HEIGHT + 1)];
    int height = 0;
    
    AvlTable *new_;
    AvlNode *x;
    AvlNode *y;
    
    new_ = AvlTable::avl_create (avl_compare, avl_param,
                       allocator != NULL ? allocator : avl_alloc);
    if (new_ == NULL)
        return NULL;
    new_->setAvl_count(avl_count) ;
    if (new_->getAvl_count() == 0)
        return new_;
    
    x = avl_root;
    y =  new_->getAvl_root();
    for (;;)
    {
        while (x->getAvl_link(0) != NULL)
        {
            assert (height < 2 * (AVL_MAX_HEIGHT + 1));
            
            y->setAvl_link( (AvlNode *)new_->getAvl_alloc()->avl_malloc (
                                                            sizeof y->getAvl_link(0)), 0);
           
            if (y->getAvl_link(0) == NULL)
            {
                if (y != (AvlNode *) new_->getAvl_root())
                {
                    y->setAvl_data(NULL);
                    y->setAvl_link(NULL, 1);
                }
                
                copy_error_recovery (stack, height, new_, destroy);
                return NULL;
            }
            
            stack[height++] = (AvlNode *) x;
            stack[height++] = y;
            x = x->getAvl_link(0);
            y = y->getAvl_link(1);
        }
        y->setAvl_link(NULL, 0);
        
        for (;;)
        {
            y->setAvl_balance(x->getAvl_balance());
            if (copy == NULL)
                y->setAvl_data(x->getAvl_data());
            else
            {
                y->setAvl_data((Labval *)copy (x->getAvl_data(), avl_param));
                if (y->getAvl_data() == NULL)
                {
                    y->setAvl_link(NULL, 1);
                    copy_error_recovery (stack, height, new_, destroy);
                    return NULL;
                }
            }
            
            if (x->getAvl_link(1) != NULL)
            {
                y->setAvl_link((AvlNode *)new_->getAvl_alloc()->avl_malloc (
                                                               sizeof *y->getAvl_link(1)), 1);
                if (y->getAvl_link(1) == NULL)
                {
                    copy_error_recovery (stack, height, new_, destroy);
                    return NULL;
                }
                
                x = x->getAvl_link(1);
                y = y->getAvl_link(1);
                break;
            }
            else
                y->setAvl_link(NULL, 1);
            
            if (height <= 2)
                return new_;
            
            y = stack[--height];
            x = stack[--height];
        }
    }
}

/* Frees storage allocated for |tree|.
 If |destroy != NULL|, applies it to each data item in inorder. */
void AvlTable::avl_destroy ( avl_item_func *destroy)
{
    AvlNode *p, *q;
    
  
    
    for (p = avl_root; p != NULL; p = q)
        if (p->getAvl_link(0) == NULL)
        {
            q = p->getAvl_link(1);
            if (destroy != NULL && p->getAvl_data() != NULL)
                destroy (p->getAvl_data(), avl_param);
            avl_alloc->avl_free ( p);
        }
        else
        {
            q = p->getAvl_link(0);
            p->setAvl_link(q->getAvl_link(1), 0);
            q->setAvl_link(p, 1);
        }
    
    avl_alloc->avl_free( this);
}


/* Inserts |item| into |tree| and returns a pointer to |item|'s address.
 If a duplicate item is found in the tree,
 returns a pointer to the duplicate without inserting |item|.
 Returns |NULL| in case of memory allocation failure. */
void *
AvlTable::avl_probe ( void *item)
{
    AvlNode *y, *z; /* Top node to update balance factor, and parent. */
    AvlNode *p, *q; /* Iterator, and parent. */
    AvlNode *n;     /* Newly inserted node. */
    AvlNode *w;     /* New root of rebalanced subtree. */
    int dir;                /* Direction to descend. */
    
    unsigned char da[AVL_MAX_HEIGHT]; /* Cached comparison results. */
    int k = 0;              /* Number of cached results. */
    
    assert (item != NULL);
    
    z = (AvlNode *) &avl_root;
    y = avl_root;
    dir = 0;
    for (q = z, p = y; p != NULL; q = p, p = p->getAvl_link(dir))
    {
        int cmp = this->avl_compare (item, p->getAvl_data(), avl_param);
        if (cmp == 0)
            return  p->getAvl_data();
        
        if (p->getAvl_balance() != 0){
            z = q; y = p; k = 0;
        }
        da[k++] = dir = cmp > 0;
    }
    
    n = (AvlNode *)avl_alloc->avl_malloc ( sizeof *n);
    q->setAvl_link((AvlNode *)avl_alloc->avl_malloc ( sizeof *n), dir);
    
    if (n == NULL)
        return NULL;
    
    avl_count++;
    n->setAvl_data((Labval *)item) ;
    n->setAvl_link(NULL, 0);
    n->setAvl_link(NULL, 1);
    n->setAvl_balance(0);
    if (y == NULL)
        return n->getAvl_data();
    
    for (p = y, k = 0; p != n; p = p->getAvl_link(da[k]), k++)
        if (da[k] == 0)
            p->setAvl_balance(p->getAvl_balance()-1);
        else
            p->setAvl_balance(p->getAvl_balance()+1);
    
    if (y->getAvl_balance() == -2)
    {
        AvlNode *x = y->getAvl_link(0);
        if (x->getAvl_balance() == -1)
        {
            w = x;
            y->setAvl_link(x->getAvl_link(1), 0);
            x->setAvl_link(y, 1);
            x->setAvl_balance(0);
            y->setAvl_balance(0);
        }
        else
        {
            assert (x->getAvl_balance() == +1);
            w = x->getAvl_link(1);
            x->setAvl_link(w->getAvl_link(0),1);
            w->setAvl_link(x, 0);
            y->setAvl_link(w->getAvl_link(1), 0);
            w->setAvl_link(y, 1);
            if (w->getAvl_balance() == -1){
                x->setAvl_balance(0) ; y->setAvl_balance(+1) ;
            }
            else if (w->getAvl_balance() == 0){
                x->setAvl_balance(0); y->setAvl_balance(0);
            }
            else /* |w->avl_balance == +1| */{
                x->setAvl_balance(-1) ; y->setAvl_balance(0);
            }
            w->setAvl_balance(0) ;
        }
    }
    else if (y->getAvl_balance() == +2)
    {
        AvlNode *x = y->getAvl_link(1);
        if (x->getAvl_balance() == +1)
        {
            w = x;
            y->setAvl_link(x->getAvl_link(0), 1);
            x->setAvl_link(y, 0);
            x->setAvl_balance(0) ;
            y->setAvl_balance(0);
        }
        else
        {
            assert (x->getAvl_balance() == -1);
            w = x->getAvl_link(0);
            x->setAvl_link(w->getAvl_link(1), 0);
            w->setAvl_link(x, 1);
            y->setAvl_link(w->getAvl_link(0), 1);
            w->setAvl_link(y, 0);
            if (w->getAvl_balance() == +1){
                x->setAvl_balance(0) ; y->setAvl_balance(-1) ;
            }
            else if (w->getAvl_balance() == 0){
                x->setAvl_balance(0);  y->setAvl_balance(0) ;
            }
            else /* |w->avl_balance == -1| */{
                x->setAvl_balance(+1) ; y->setAvl_balance(0) ;
            }
            w->setAvl_balance(0) ;
        }
    }
    else
        return n->getAvl_data();
    z->setAvl_link(w, y != z->getAvl_link(0));
    
    avl_generation++;
    return n->getAvl_data();
}

/* Inserts |item| into |table|.
 Returns |NULL| if |item| was successfully inserted
 or if a memory allocation error occurred.
 Otherwise, returns the duplicate item. */
void *
AvlTable::avl_insert ( void *item)
{
    void *p = avl_probe ( item);
    return p == NULL || p == item ? NULL : p;
}


/* Inserts |item| into |table|, replacing any duplicate item.
 Returns |NULL| if |item| was inserted without replacing a duplicate,
 or if a memory allocation error occurred.
 Otherwise, returns the item that was replaced. */
void *
AvlTable::avl_replace ( void *item)
{
    void *p = avl_probe ( item);
    if (p == NULL || p == item)
        return NULL;
    else
    {
        void *r = p;
        p = item;
        return r;
    }
}


/* Search |tree| for an item matching |item|, and return it if found.
 Otherwise return |NULL|. */
void *
AvlTable::avl_find ( const void *item)
{
    AvlNode *p;
    
    assert ( item != NULL);
    for (p = avl_root; p != NULL; )
    {
        int cmp = this->avl_compare (item, p->getAvl_data(), this->avl_param);
        
        if (cmp < 0)
            p = p->getAvl_link(0);
        else if (cmp > 0)
            p = p->getAvl_link(1);
        else /* |cmp == 0| */
            return p->getAvl_data();
    }
    
    return NULL;
}

/* Asserts that |avl_insert()| succeeds at inserting |item| into |table|. */
void
AvlTable::avl_assert_insert (  void *item)
{
    void *p = avl_probe ( item);
    assert (&p != NULL && p == item);
}

/* Asserts that |avl_delete()| really removes |item| from |table|,
 and returns the removed item. */
void *
AvlTable::avl_assert_delete ( void *item)
{
    void *p = avl_delete ( item);
    assert (p != NULL);
    return p;
}

/* Deletes from |tree| and returns an item matching |item|.
 Returns a null pointer if no matching item found. */
void *
AvlTable::avl_delete (  const void *item)
{
    /* Stack of nodes. */
    AvlNode *pa[AVL_MAX_HEIGHT]; /* Nodes. */
    unsigned char da[AVL_MAX_HEIGHT];    /* |avl_link[]| indexes. */
    int k;                               /* Stack pointer. */
    
    AvlNode *p;   /* Traverses tree to find node to delete. */
    int cmp;              /* Result of comparison between |item| and |p|. */
    
    assert ( item != NULL);
    
    k = 0;
    p = (AvlNode *) &this->avl_root;
    for (cmp = -1; cmp != 0;
         cmp = this->avl_compare (item, p->getAvl_data(), this->getAvl_param()))
    {
        int dir = cmp > 0;
        
        pa[k] = p;
        da[k++] = dir;
        
        p = p->getAvl_link(dir);
        if (p == NULL)
            return NULL;
    }
    item = p->getAvl_data();
    
    if (p->getAvl_link(1) == NULL)
        pa[k - 1]->setAvl_link(p->getAvl_link(0), da[k - 1]);
    else
    {
        AvlNode *r = p->getAvl_link(1);
        if (r->getAvl_link(0) == NULL)
        {
            r->setAvl_link(p->getAvl_link(0), 0);
            r->setAvl_balance(p->getAvl_balance()) ;
            pa[k - 1]->setAvl_link(r, da[k - 1]);
            da[k] = 1;
            pa[k++] = r;
        }
        else
        {
            AvlNode *s;
            int j = k++;
            
            for (;;)
            {
                da[k] = 0;
                pa[k++] = r;
                s = r->getAvl_link(0);
                if (s->getAvl_link(0) == NULL)
                    break;
                
                r = s;
            }
            
            s->setAvl_link(p->getAvl_link(0), 0);
            r->setAvl_link(s->getAvl_link(1), 0);
            s->setAvl_link(p->getAvl_link(1), 1);
            s->setAvl_balance(p->getAvl_balance()) ;
            
            pa[j - 1]->setAvl_link(s, da[j - 1]);
            da[j] = 1;
            pa[j] = s;
        }
    }
    
    this->avl_alloc->avl_free( p);
    
    assert (k > 0);
    while (--k > 0)
    {
        AvlNode *y = pa[k];
        
        if (da[k] == 0)
        {
            y->setAvl_balance(y->getAvl_balance()+1);
            if (y->getAvl_balance() == +1)
                break;
            else if (y->getAvl_balance() == +2)
            {
                AvlNode *x = y->getAvl_link(1);
                if (x->getAvl_balance() == -1)
                {
                    AvlNode *w;
                    assert (x->getAvl_balance() == -1);
                    w = x->getAvl_link(0);
                    x->setAvl_link(w->getAvl_link(1), 0) ;
                    w->setAvl_link(x, 1);
                    y->setAvl_link(w->getAvl_link(0), 1) ;
                    w->setAvl_link(y, 0);
                    if (w->getAvl_balance() == +1){
                        x->setAvl_balance(0) ; y->setAvl_balance(-1);
                    }
                    else if (w->getAvl_balance() == 0){
                        x->setAvl_balance(0);
                        y->setAvl_balance(0);
                    }
                    else /* |w->avl_balance == -1| */{
                        x->setAvl_balance(+1) ; y->setAvl_balance(0) ;
                    }
                    w->setAvl_balance(0) ;
                    pa[k - 1]->setAvl_link(w, da[k - 1]) ;
                }
                else
                {
                    y->setAvl_link(x->getAvl_link(0), 1);
                    x->setAvl_link(y, 0);
                    pa[k - 1]->setAvl_link(x, da[k - 1]);
                    if (x->getAvl_balance() == 0)
                    {
                        x->setAvl_balance(-1) ;
                        y->setAvl_balance(+1) ;
                        break;
                    }
                    else{
                        x->setAvl_balance(0) ;
                        y->setAvl_balance(0) ;
                    }
                }
            }
        }
        else
        {
            y->setAvl_balance(y->getAvl_balance()-1);
            if (y->getAvl_balance() == -1)
                break;
            else if (y->getAvl_balance() == -2)
            {
                AvlNode *x = y->getAvl_link(0);
                if (x->getAvl_balance() == +1)
                {
                    AvlNode *w;
                    assert (x->getAvl_balance() == +1);
                    w = x->getAvl_link(1);
                    x->setAvl_link(w->getAvl_link(0),1) ;
                    w->setAvl_link(x, 0);
                    y->setAvl_link(w->getAvl_link(1), 0);
                    w->setAvl_link(y, 1);
                    if (w->getAvl_balance() == -1){
                        x->setAvl_balance(0) ; y->setAvl_balance(+1) ;
                    }
                    else if (w->getAvl_balance() == 0){
                        x->setAvl_balance(0) ; y->setAvl_balance(0);
                    }
                    else /* |w->avl_balance == +1| */{
                        x->setAvl_balance(-1); y->setAvl_balance(0);
                    }
                    w->setAvl_balance(0) ;
                    pa[k - 1]->setAvl_link(w, da[k - 1]);
                }
                else
                {
                    y->setAvl_link(x->getAvl_link(1), 0);
                    x->setAvl_link(y, 1);
                    pa[k - 1]->setAvl_link(x, da[k - 1]);
                    if (x->getAvl_balance() == 0)
                    {
                        x->setAvl_balance(+1) ;
                        y->setAvl_balance(-1) ;
                        break;
                    }
                    else{
                        x->setAvl_balance(0);
                        y->setAvl_balance(0);
                    }
                }
            }
        }
    }
    
    this->avl_count--;
    this->avl_generation++;
    return (void *) item;
}































































