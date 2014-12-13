/*--------------------------------------------------------------------------*/
/*                                  MINIOS                                  */
/*                        The Embedded Operating System                     */
/*             Copyright (C) 2014-2024, ZhuGuangXiang, Nanjing, China       */
/*                           All Rights Reserved                            */
/*--------------------------------------------------------------------------*/
#ifndef _MINIOS_LIST_H_
#define _MINIOS_LIST_H_

#include "minios_type.h"

typedef struct list_head
{
    struct list_head *next;
    struct list_head *prev;
} list_head_t;

#define LIST_HEAD_INIT(name) {&(name), &(name)}

#define LIST_HEAD(name) list_head_t name = LIST_HEAD_INIT(name)

#define INIT_LIST_HEAD(head) do { \
    (head)->next = (head); (head)->prev = (head); \
} while (0)

#define LIST_INLIST(node)  (((node)->next != node) ? TRUE : FALSE)

#define LIST_EMPTY(head)   (((head)->next == head) ? TRUE : FALSE)

static inline void __list_add(list_head_t *new,
    list_head_t *prev, list_head_t *next)
{
    new->next = next;
    new->prev = prev;
    prev->next = new;
    next->prev = new;
}

#define LIST_ADD(head, node) __list_add(node, head, (head)->next)
#define LIST_ADD_TAIL(head, node) __list_add(node, (head)->prev, head)

static inline void __list_del(list_head_t *prev, list_head_t *next)
{
    next->prev = prev;
    prev->next = next;
}

#define LIST_DEL(node) do { \
    __list_del((node)->prev, (node)->next); INIT_LIST_HEAD(node); \
} while (0)

#define LIST_FIRST(head) (LIST_EMPTY(head) ? NULL : (head)->next)

#define LIST_ENTRY(ptr, type, member) PARENT_STRUCT(ptr, type, member)

#define LIST_FOR_EACH(pos, head) \
    for (pos = (head)->next; pos != (head); pos = pos->next)

#define LIST_FOR_EACH_SAFE(pos, n, head) \
    for (pos = (head)->next, n = pos->next; \
         pos != (head); pos = n, n = pos->next)

#define LIST_FOR_EACH_ENTRY(pos, head, member) \
    for (pos = LIST_ENTRY((head)->next, typeof(*pos), member); \
         &pos->member != (head); \
         pos = LIST_ENTRY(pos->member.next, typeof(*pos), member))

#define LIST_FOR_EACH_ENTRY_SAFE(pos, n, head, member)           \
    for (pos = LIST_ENTRY((head)->next, typeof(*pos), member),   \
         n = LIST_ENTRY(pos->member.next, typeof(*pos), member); \
         &pos->member != (head); \
         pos = n, n = LIST_ENTRY(n->member.next, typeof(*n), member))

/*--------------------------------------------------------------------------*/

typedef struct hlist_node {
    struct hlist_node *next;
    struct hlist_node **pprev;
} hlist_node_t;

typedef struct hlist_head {
    hlist_node_t *first;
} hlist_head_t;

#define HLIST_HEAD_INIT(name) {NULL}

#define HLIST_HEAD(name) hlist_head_t name = HLIST_HEAD_INIT(name);

#define INIT_HLIST_HEAD(head) do {\
    (head)->first = NULL; \
} while (0)

#define INIT_HLIST_NODE(node) do {\
    (node)->next = NULL; (node)->pprev = NULL; \
} while (0)

#define HLIST_UNHASHED(node) ((node)->pprev == NULL ? TRUE : FALSE)

#define HLIST_EMPTY(head)    ((head)->first == NULL ? TRUE : FALSE)

static inline void HLIST_DEL(hlist_node_t *node)
{
    if (node->pprev) {
        hlist_node_t *next = node->next;
        hlist_node_t **pprev = node->pprev;
        *pprev = next;
        if (next)
            next->pprev = pprev;
        INIT_HLIST_NODE(node);
    }
}

static inline void HLIST_ADD(hlist_head_t *head, hlist_node_t *node)
{
    hlist_node_t *first = head->first;
    node->next = first;
    if (first)
        first->pprev = &node->next;
    head->first = node;
    node->pprev = &head->first;
}

#define HLIST_ENTRY(ptr, type, member) PARENT_STRUCT(ptr, type, member)

#define HLIST_FOR_EACH(pos, head) \
    for (pos = (head)->first; pos; pos = (pos)->next)

#define HLIST_FOR_EACH_SAFE(pos, n, head) \
    for (pos = (head)->first; pos && ({n = pos->next; 1;}); pos = n)

#endif // _MINIOS_LIST_H_
// EOF list.h
