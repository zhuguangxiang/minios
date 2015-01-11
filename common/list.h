/**INC+************************************************************************/
/* Header:  list.h                                                            */
/*                                                                            */
/* Purpose: Common List/Queue                                                 */
/*                                                                            */
/* Author:  ZhuGuangXiang                                                     */
/*                                                                            */
/* Version: V1.00                                                             */
/*                                                                            */
/* (C) Copyright 2014-2024 ZhuGuangXiang NanJing China                        */
/*                                                                            */
/**INC-************************************************************************/

#ifndef _MINIOS_LIST_H_
#define _MINIOS_LIST_H_

#include "types.h"

/**STRUCT+*********************************************************************/
/* Structure: LQE                                                             */
/*                                                                            */
/* Description: List/Queue element                                            */
/**STRUCT-*********************************************************************/
typedef struct lqe {
    /**************************************************************************/
    /* Pointer to next LQE                                                    */
    /**************************************************************************/
    struct lqe *next;

    /**************************************************************************/
    /* Pointer to previous LQE                                                */
    /**************************************************************************/
    struct lqe *prev;
} LQE;

/******************************************************************************/
/* Macro to initialize list when declaring it                                 */
/******************************************************************************/
#define LIST_INIT(name) {&(name), &(name)}

/**PROC+***********************************************************************/
/* Name:     init_list                                                        */
/*                                                                            */
/* Purpose:  Initialize a LQE                                                 */
/*                                                                            */
/* Returns:  None                                                             */
/*                                                                            */
/* Params:   IN lqhe   - list/queue head or element to be intialized          */
/*                                                                            */
/**PROC-***********************************************************************/
STATIC INLINE VOID init_list(LQE *lqhe)
{
    lqhe->next = lqhe;
    lqhe->prev = lqhe;
}

/**PROC+***********************************************************************/
/* Name:     list_empty                                                       */
/*                                                                            */
/* Purpose:  Test whether a list is empty                                     */
/*                                                                            */
/* Returns:  FALSE if this list is not empty                                  */
/*                                                                            */
/* Params:   IN lqh   - list/queue head                                       */
/*                                                                            */
/**PROC-***********************************************************************/
STATIC INLINE BOOL list_empty(CONST LQE *lqh)
{
    return ((lqh->next == lqh) && (lqh->prev == lqh)) ? TRUE : FALSE;
}

/**PROC+***********************************************************************/
/* Name:     lqe_in_list                                                      */
/*                                                                            */
/* Purpose:  Test whether a LQE is in a list                                  */
/*                                                                            */
/* Returns:  FALSE if this LQE is not in a list                               */
/*                                                                            */
/* Params:   IN lqe   - list/queue element                                    */
/*                                                                            */
/**PROC-***********************************************************************/
STATIC INLINE BOOL lqe_in_list(CONST LQE *lqe)
{
    return ((lqe->next == lqe) && (lqe->prev == lqe)) ? FALSE : TRUE;
}

/**PROC+***********************************************************************/
/* Name:     list_first                                                       */
/*                                                                            */
/* Purpose:  Returns a pointer to the first element in the list               */
/*                                                                            */
/* Returns:  The first LQE, or NULL if the list is empty                      */
/*                                                                            */
/* Params:   IN lqh   - list/queue head                                       */
/*                                                                            */
/**PROC-***********************************************************************/
STATIC INLINE LQE *list_first(CONST LQE *lqh)
{
    return list_empty(lqh) ? NULL : lqh->next;
}

/**PROC+***********************************************************************/
/* Name:     list_last                                                        */
/*                                                                            */
/* Purpose:  Returns a pointer to the last element in the list                */
/*                                                                            */
/* Returns:  The last LQE, or NULL if the list is empty                       */
/*                                                                            */
/* Params:   IN lqh   - list/queue head                                       */
/*                                                                            */
/**PROC-***********************************************************************/
STATIC INLINE LQE *list_last(CONST LQE *lqh)
{
    return list_empty(lqh) ? NULL : lqh->prev;
}

/**PROC+***********************************************************************/
/* Name:     __list_add                                                       */
/*                                                                            */
/* Purpose:  Insert a new LQE between two known consecutive LQEs              */
/*           This is only for internal list manipulation where we know the    */
/*           prev/next LQEs already!                                          */
/*                                                                            */
/* Returns:  None                                                             */
/*                                                                            */
/* Params:   IN lqe   - LQE to be inserted                                    */
/*           IN prev  - previous LQE                                          */
/*           IN next  - next LQE                                              */
/*                                                                            */
/**PROC-***********************************************************************/
STATIC INLINE VOID __list_add(LQE *lqe, LQE *prev, LQE *next)
{
    lqe->next = next;
    lqe->prev = prev;
    prev->next = lqe;
    next->prev = lqe;
}

/**PROC+***********************************************************************/
/* Name:     list_add                                                         */
/*                                                                            */
/* Purpose:  Add a new LQE to list head                                       */
/*           Insert a new LQE after the specified head.                       */
/*           This is good for implementing stacks.                            */
/*                                                                            */
/* Returns:  None                                                             */
/*                                                                            */
/* Params:   IN lqe   - LQE to be added                                       */
/*           IN lqh   - list head                                             */
/*                                                                            */
/**PROC-***********************************************************************/
STATIC INLINE VOID list_add(LQE *lqe, LQE *lqh)
{
    __list_add(lqe, lqh, lqh->next);
}

/**PROC+***********************************************************************/
/* Name:     list_add_tail                                                    */
/*                                                                            */
/* Purpose:  Add a new LQE to list tail                                       */
/*           Insert a new LQE before the specified head.                      */
/*           This is good for implementing queues.                            */
/*                                                                            */
/* Returns:  None                                                             */
/*                                                                            */
/* Params:   IN lqe   - LQE to be added                                       */
/*           IN lqh   - list head element                                     */
/*                                                                            */
/**PROC-***********************************************************************/
STATIC INLINE VOID list_add_tail(LQE *lqe, LQE *lqh)
{
    __list_add(lqe, (lqh)->prev, lqh);
}

/**PROC+***********************************************************************/
/* Name:     __list_del                                                       */
/*                                                                            */
/* Purpose:  Delete a LQE by making the prev/next LQEs                        */
/*           This is only for internal list manipulation where we know the    */
/*           prev/next LQEs already!                                          */
/*                                                                            */
/* Returns:  None                                                             */
/*                                                                            */
/* Params:   IN lqe   - LQE to be deleted                                     */
/*                                                                            */
/**PROC-***********************************************************************/
STATIC INLINE VOID __list_del(LQE *prev, LQE *next)
{
    next->prev = prev;
    prev->next = next;
}

/**PROC+***********************************************************************/
/* Name:     list_del                                                         */
/*                                                                            */
/* Purpose:  Delete a LQE from a list and reintialize it.                     */
/*                                                                            */
/* Returns:  None                                                             */
/*                                                                            */
/* Params:   IN lqe   - LQE to be deleted                                     */
/*                                                                            */
/**PROC-***********************************************************************/
STATIC INLINE VOID list_del(LQE *lqe)
{
    __list_del(lqe->prev, lqe->next);
    init_list(lqe);
}

/**PROC+***********************************************************************/
/* Name:     __list_splice                                                    */
/*                                                                            */
/* Purpose:  Insert a new list between two known consecutive LQEs             */
/*           This is only for internal list manipulation where we know the    */
/*           prev/next LQEs already!                                          */
/*                                                                            */
/* Returns:  None                                                             */
/*                                                                            */
/* Params:   IN lqh   - head of list/queue to be inserted                     */
/*           IN prev  - previous LQE in another list                          */
/*           IN next  - next LQE in another list                              */
/*                                                                            */
/**PROC-***********************************************************************/
STATIC INLINE VOID __list_splice(CONST LQE *lqh, LQE *prev, LQE *next)
{
    LQE *first = lqh->next;
    LQE *last = lqh->prev;

    first->prev = prev;
    prev->next = first;

    last->next = next;
    next->prev = last;
}

/**PROC+***********************************************************************/
/* Name:     list_splice                                                      */
/*                                                                            */
/* Purpose:  join two lists                                                   */
/*           Add a list to another list head and reinitialize the list        */
/*           This is good for implementing stacks.                            */
/*                                                                            */
/* Returns:  None                                                             */
/*                                                                            */
/* Params:   IN from   - the new list to be added                             */
/*           IN to     - the list to add a new list to it                     */
/*                                                                            */
/**PROC-***********************************************************************/
STATIC INLINE VOID list_splice(LQE *from, LQE *to)
{
    if (!list_empty(from)) {
        __list_splice(from, to, to->next);
        init_list(from);
    }
}

/**PROC+***********************************************************************/
/* Name:     list_splice_tail                                                 */
/*                                                                            */
/* Purpose:  join two lists                                                   */
/*           Add a list to another list tail and reinitialize the list        */
/*           This is good for implementing queues.                            */
/*                                                                            */
/* Returns:  None                                                             */
/*                                                                            */
/* Params:   IN from   - the new list to be added                             */
/*           IN to     - the list to add a new list to it                     */
/*                                                                            */
/**PROC-***********************************************************************/
STATIC INLINE VOID list_splice_tail(LQE *from, LQE *to)
{
    if (!list_empty(from)) {
        __list_splice(from, to->prev, to);
        init_list(from);
    }
}

/******************************************************************************/
/* Macro to get the structure which contains this lqe                         */
/******************************************************************************/
#define LIST_ENTRY(ptr, type, member) CONTAINER_OF(ptr, type, member)

/******************************************************************************/
/* Macro to iterate over a list                                               */
/******************************************************************************/
#define LIST_FOR_EACH(pos, head) \
    for (pos = (head)->next; pos != (head); pos = (pos)->next)

/******************************************************************************/
/* Macro to iterate over a list safe against removal of list                  */
/******************************************************************************/
#define LIST_FOR_EACH_SAFE(pos, n, head)         \
    for (pos = (head)->next, n = (pos)->next;    \
         pos != (head); pos = n, n = (pos)->next)

/******************************************************************************/
/* Macro to iterate over a list of given type                                 */
/******************************************************************************/
#define LIST_FOR_EACH_ENTRY(pos, head, member)                    \
    for (pos = LIST_ENTRY((head)->next, typeof(*pos), member);    \
         &pos->member != (head);                                  \
         pos = LIST_ENTRY(pos->member.next, typeof(*pos), member))

/******************************************************************************/
/* Macro to iterate over a list of given type safe against removal            */
/******************************************************************************/
#define LIST_FOR_EACH_ENTRY_SAFE(pos, n, head, member)               \
    for (pos = LIST_ENTRY((head)->next, typeof(*pos), member),       \
         n = LIST_ENTRY(pos->member.next, typeof(*pos), member);     \
         &pos->member != (head);                                     \
         pos = n, n = LIST_ENTRY(n->member.next, typeof(*n), member))


/**STRUCT+*********************************************************************/
/* Structure: HNODE                                                           */
/*                                                                            */
/* Description: Hash comflict list node                                       */
/**STRUCT-*********************************************************************/
typedef struct hnode {
    /**************************************************************************/
    /* Pointer to next HNODE                                                  */
    /**************************************************************************/
    struct hnode *next;
    /**************************************************************************/
    /* Pointer to previous next field                                         */
    /**************************************************************************/
    struct hnode **pprev;
} HNODE;

/**STRUCT+*********************************************************************/
/* Structure: HLIST                                                           */
/*                                                                            */
/* Description: Hash comflict list head                                       */
/**STRUCT-*********************************************************************/
typedef struct hlist {
    /**************************************************************************/
    /* Pointer to first HNODE                                                 */
    /**************************************************************************/
    struct hnode *first;
} HLIST;

/******************************************************************************/
/* Macro to initialize hash list when declaring it                            */
/******************************************************************************/
#define HLIST_INIT(name) {NULL}

/**PROC+***********************************************************************/
/* Name:     init_hlist                                                       */
/*                                                                            */
/* Purpose:  Initialize the hash list                                         */
/*                                                                            */
/* Returns:  None                                                             */
/*                                                                            */
/* Params:   IN hlist   - hash list head to be intialized                     */
/*                                                                            */
/**PROC-***********************************************************************/
STATIC INLINE VOID init_hlist(HLIST *hlist)
{
    hlist->first = NULL;
}

/**PROC+***********************************************************************/
/* Name:     init_hnode                                                       */
/*                                                                            */
/* Purpose:  Initialize the hash list node                                    */
/*                                                                            */
/* Returns:  None                                                             */
/*                                                                            */
/* Params:   IN hnode   - hash list node to be intialized                     */
/*                                                                            */
/**PROC-***********************************************************************/
STATIC INLINE VOID init_hnode(HNODE *hnode)
{
    hnode->next = NULL;
    hnode->pprev = NULL;
}

/**PROC+***********************************************************************/
/* Name:     hlist_empty                                                      */
/*                                                                            */
/* Purpose:  Test whether a hash list is empty                                */
/*                                                                            */
/* Returns:  FALSE if this hash list is not empty                             */
/*                                                                            */
/* Params:   IN hlist   - hash list head                                      */
/*                                                                            */
/**PROC-***********************************************************************/
STATIC INLINE BOOL hlist_empty(CONST HLIST *hlist)
{
    return (hlist->first == NULL) ? TRUE : FALSE;
}

/**PROC+***********************************************************************/
/* Name:     hlist_unhashed                                                   */
/*                                                                            */
/* Purpose:  Test whether a hash node is hashed                               */
/*                                                                            */
/* Returns:  FALSE if this hash node is already hashed                        */
/*                                                                            */
/* Params:   IN hnode   - hash node                                           */
/*                                                                            */
/**PROC-***********************************************************************/
STATIC INLINE BOOL hlist_unhashed(CONST HNODE *hnode)
{
    return (hnode->pprev != NULL) ? FALSE : TRUE;
}

/**PROC+***********************************************************************/
/* Name:     hlist_add                                                        */
/*                                                                            */
/* Purpose:  Add a new HNODE to hash list                                     */
/*                                                                            */
/* Returns:  None                                                             */
/*                                                                            */
/* Params:   IN hnode   - HNODE to be added                                   */
/*           IN hlist   - hash list head                                      */
/*                                                                            */
/**PROC-***********************************************************************/
STATIC INLINE VOID hlist_add(HNODE *hnode, HLIST *hlist)
{
    HNODE *first = hlist->first;
    hnode->next = first;
    if (first)
        first->pprev = &hnode->next;
    hlist->first = hnode;
    hnode->pprev = &hlist->first;
}

/**PROC+***********************************************************************/
/* Name:     hlist_del                                                        */
/*                                                                            */
/* Purpose:  Delete a HNODE from a list and reintialize it.                   */
/*                                                                            */
/* Returns:  None                                                             */
/*                                                                            */
/* Params:   IN hnode   - HNODE to be deleted                                 */
/*                                                                            */
/**PROC-***********************************************************************/
STATIC INLINE VOID hlist_del(HNODE *hnode)
{
    if (!hlist_unhashed(hnode)) {
        HNODE *next = hnode->next;
        HNODE **pprev = hnode->pprev;
        *pprev = next;
        if (next)
            next->pprev = pprev;
        init_hnode(hnode);
    }
}

/******************************************************************************/
/* Macro to get the structure which contains this HNODE                       */
/******************************************************************************/
#define HLIST_ENTRY(ptr, type, member) CONTAINER_OF(ptr, type, member)

/******************************************************************************/
/* Macro to iterate over a hash list                                          */
/******************************************************************************/
#define HLIST_FOR_EACH(pos, head) \
    for (pos = (head)->first; pos; pos = (pos)->next)

/******************************************************************************/
/* Macro to iterate over a hash list safe against removal of list             */
/******************************************************************************/
#define HLIST_FOR_EACH_SAFE(pos, n, head) \
    for (pos = (head)->first; pos && ({n = pos->next; 1;}); pos = n)

#endif /* _MINIOS_LIST_H_ */

/******************************************************************************/
