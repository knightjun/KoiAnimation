/**
 * dhlist.h
 *  - deque list and hash list from Linux Kernel
 *
 * from Linux Kernel
 * for Windows and Linux
 *
 * modified by cheungmine
 * 2013-4, 2018-02-04
 */
#ifndef _DH_LIST_H
#define _DH_LIST_H
 
#if defined(__cplusplus)
extern "C" {
#endif
 
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <malloc.h>
 
 
/**
 * BKDR Hash Function
 *   http://www.cnblogs.com/-clq/archive/2012/05/31/2528153.html
 */
static inline unsigned int BKDRHash (char *str)
{
    /* seed: 31 131 1313 13131 131313 etc.. */
    unsigned int seed = 131;
    unsigned int hash = 0;
 
    while (*str) {
        hash = hash * seed + (*str++);
    }
 
    return (hash & 0x7FFFFFFF);
}
 
 
static inline unsigned int BKDRHash2 (char *str, int hashlen)
{
    /* seed: 31 131 1313 13131 131313 etc.. */
    unsigned int seed = 131;
    unsigned int hash = 0;
 
    while (*str) {
        hash = hash * seed + (*str++);
    }
 
    return ((hash & 0x7FFFFFFF) & hashlen);
}
 
 
#ifdef CONFIG_ILLEGAL_POINTER_VALUE
#  define POISON_POINTER_DELTA  _AC(CONFIG_ILLEGAL_POINTER_VALUE, UL)
#else
#  define POISON_POINTER_DELTA  0
#endif
 
/**
 * These are non-NULL pointers that will result in page faults
 * under normal circumstances, used to verify that nobody uses
 * non-initialized list entries.
 */
#if defined _MSC_VER || WIN32
#  define LIST_POISON1  ((void *) 0x00100100)
#  define LIST_POISON2  ((void *) 0x00200200)
#else
#  define LIST_POISON1  ((void *) 0x00100100 + POISON_POINTER_DELTA)
#  define LIST_POISON2  ((void *) 0x00200200 + POISON_POINTER_DELTA)
#endif
 
#ifdef typeof
 
static inline void prefetch(const void *x) {;}
static inline void prefetchw(const void *x) {;}
 
/**
 * filename: linux-2.6.7/include/linux/stddef.h
 */
#define OFFSET_OF(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
 
#define CONTAINER_OF(ptr, type, member) ( { \
    const typeof( ((type *)0)->member ) *__mptr = (ptr); \
    (type *)( (char *)__mptr - OFFSET_OF(type,member) ); } )
 
#else
 
static inline int prefetch(const void *x) {;return 1;}
static inline int prefetchw(const void *x) {;return 1;}
 
#define OFFSET_OF(type, field)    ((unsigned int) (uintptr_t) (void*) &((type *)0)->field)
 
#define CONTAINER_OF(address, type, field) \
    ((type *)((char *)(address) - OFFSET_OF(type, field)))
 
#endif  /* typeof */
 
struct list_head
{
    struct list_head *next, *prev;
};
 
#define LIST_HEAD_INIT(name) { &(name), &(name) }
 
#define LIST_HEAD(name) \
    struct list_head name = LIST_HEAD_INIT(name)
 
#define INIT_LIST_HEAD(ptr) do { \
        (ptr)->next = (ptr); (ptr)->prev = (ptr); \
    } while (0)
 
/*
 * Insert a new entry between two known consecutive entries.
 *
 * This is only for internal list manipulation where we know
 * the prev/next entries already!
 */
static inline void __list_add(struct list_head *_new,
                              struct list_head *prev,
                              struct list_head *next)
{
    next->prev = _new;
    _new->next = next;
    _new->prev = prev;
    prev->next = _new;
}
 
/**
 * list_add - add a new entry
 * @new: new entry to be added
 * @head: list head to add it after
 *
 * Insert a new entry after the specified head.
 * This is good for implementing stacks.
 */
static inline void list_add(struct list_head *_new, struct list_head *head)
{
    __list_add(_new, head, head->next);
}
 
/**
 * list_add_tail - add a new entry
 * @new: new entry to be added
 * @head: list head to add it before
 *
 * Insert a new entry before the specified head.
 * This is useful for implementing queues.
 */
static inline void list_add_tail(struct list_head *_new, struct list_head *head)
{
    __list_add(_new, head->prev, head);
}
 
static inline void __list_del(struct list_head * prev, struct list_head * next)
{
    next->prev = prev;
    prev->next = next;
}
 
static inline void list_del(struct list_head *entry)
{
    __list_del(entry->prev, entry->next);
    entry->next = (struct list_head*) LIST_POISON1;
    entry->prev = (struct list_head*) LIST_POISON2;
}
 
static inline void list_del_init(struct list_head *entry)
{
    __list_del(entry->prev, entry->next);
    INIT_LIST_HEAD(entry);
}
 
static inline void list_move(struct list_head *list, struct list_head *head)
{
    __list_del(list->prev, list->next);
    list_add(list, head);
}
 
static inline void list_move_tail(struct list_head *list, struct list_head *head)
{
    __list_del(list->prev, list->next);
    list_add_tail(list, head);
}
 
static inline int list_empty(const struct list_head *head)
{
    return head->next == head;
}
 
static inline int list_empty_careful(const struct list_head *head)
{
    struct list_head *next = head->next;
    return (next == head) && (next == head->prev);
}
 
static inline void __list_splice(struct list_head *list, struct list_head *head)
{
    struct list_head *first = list->next;
    struct list_head *last = list->prev;
    struct list_head *at = head->next;
 
    first->prev = head;
    head->next = first;
 
    last->next = at;
    at->prev = last;
}
 
/**
 * list_splice - join two lists
 * @list: the new list to add.
 * @head: the place to add it in the first list.
 */
static inline void list_splice(struct list_head *list, struct list_head *head)
{
    if (!list_empty(list)) {
        __list_splice(list, head);
    }
}
 
/**
 * list_splice_init - join two lists and reinitialise the emptied list.
 * @list: the new list to add.
 * @head: the place to add it in the first list.
 *
 * The list at @list is reinitialised
 */
static inline void list_splice_init(struct list_head *list, struct list_head *head)
{
    if (!list_empty(list)) {
        __list_splice(list, head);
        INIT_LIST_HEAD(list);
    }
}
 
#define list_entry(ptr, type, member) \
    CONTAINER_OF(ptr, type, member)
 
#define list_for_each(pos, head) \
    for (pos = (head)->next; prefetch(pos->next), pos != (head); \
         pos = pos->next)
 
#define __list_for_each(pos, head) \
    for (pos = (head)->next; pos != (head); pos = pos->next)
 
#define list_for_each_prev(pos, head) \
    for (pos = (head)->prev; prefetch(pos->prev), pos != (head); \
         pos = pos->prev)
 
#define list_for_each_safe(pos, n, head) \
    for (pos = (head)->next, n = pos->next; pos != (head); \
         pos = n, n = pos->next)
 
#ifdef typeof
 
#define list_for_each_entry(pos, head, member) \
    for (pos = list_entry((head)->next, typeof(*pos), member); \
         prefetch(pos->member.next), &pos->member != (head); \
         pos = list_entry(pos->member.next, typeof(*pos), member))
 
#define list_for_each_entry_reverse(pos, head, member) \
    for (pos = list_entry((head)->prev, typeof(*pos), member); \
         prefetch(pos->member.prev), &pos->member != (head); \
         pos = list_entry(pos->member.prev, typeof(*pos), member))
 
#define list_prepare_entry(pos, head, member) \
        ((pos) ? : list_entry(head, typeof(*pos), member))
 
#define list_for_each_entry_continue(pos, head, member) \
    for (pos = list_entry(pos->member.next, typeof(*pos), member); \
         prefetch(pos->member.next), &pos->member != (head); \
         pos = list_entry(pos->member.next, typeof(*pos), member))
 
#define list_for_each_entry_safe(pos, n, head, member) \
    for (pos = list_entry((head)->next, typeof(*pos), member), \
           n = list_entry(pos->member.next, typeof(*pos), member); \
         &pos->member != (head); \
         pos = n, n = list_entry(n->member.next, typeof(*n), member))
#else
 
#define list_for_each_entry(pos, typeof_pos, head, member) \
    for (pos = list_entry((head)->next, typeof_pos, member); \
         prefetch(pos->member.next), &pos->member != (head); \
         pos = list_entry(pos->member.next, typeof_pos, member))
 
#define list_for_each_entry_reverse(pos, typeof_pos, head, member) \
    for (pos = list_entry((head)->prev, typeof_pos, member); \
         prefetch(pos->member.prev), &pos->member != (head); \
         pos = list_entry(pos->member.prev, typeof_pos, member))
 
#define list_prepare_entry(pos, typeof_pos, head, member) \
        ((pos) ? : list_entry(head, typeof_pos, member))
 
#define list_for_each_entry_continue(pos, typeof_pos, head, member) \
    for (pos = list_entry(pos->member.next, typeof_pos, member); \
         prefetch(pos->member.next), &pos->member != (head); \
         pos = list_entry(pos->member.next, typeof_pos, member))
 
#define list_for_each_entry_safe(pos, typeof_pos, n, typeof_n, head, member) \
    for (pos = list_entry((head)->next, typeof_pos, member), \
           n = list_entry(pos->member.next, typeof_pos, member); \
         &pos->member != (head); \
         pos = n, n = list_entry(n->member.next, typeof_n, member))
 
#endif  /* typeof */
 
/**
 * HASH LIST
 */
struct hlist_head {
    struct hlist_node *first;
};
 
struct hlist_node {
    struct hlist_node *next, **pprev;
};
 
#define HLIST_HEAD_INIT { .first = NULL }
#define HLIST_HEAD(name) struct hlist_head name = {  .first = NULL }
#define INIT_HLIST_HEAD(ptr) ((ptr)->first = NULL)
#define INIT_HLIST_NODE(ptr) ((ptr)->next = NULL, (ptr)->pprev = NULL)
 
static inline int hlist_unhashed(const struct hlist_node *h)
{
    return !h->pprev;
}
 
static inline int hlist_empty(const struct hlist_head *h)
{
    return !h->first;
}
 
static inline void __hlist_del(struct hlist_node *n)
{
    struct hlist_node *next = n->next;
    struct hlist_node **pprev = n->pprev;
    *pprev = next;
    if (next) {
        next->pprev = pprev;
    }
}
 
static inline void hlist_del(struct hlist_node *n)
{
    __hlist_del(n);
    n->next = (struct hlist_node*) LIST_POISON1;
    n->pprev = (struct hlist_node**) LIST_POISON2;
}
 
static inline void hlist_del_init(struct hlist_node *n)
{
    if (n->pprev) {
        __hlist_del(n);
        INIT_HLIST_NODE(n);
    }
}
 
static inline void hlist_add_head(struct hlist_node *n, struct hlist_head *h)
{
    struct hlist_node *first = h->first;
    n->next = first;
    if (first) {
        first->pprev = &n->next;
    }
    h->first = n;
    n->pprev = &h->first;
}
 
/* next must be != NULL */
static inline void hlist_add_before(struct hlist_node *n, struct hlist_node *next)
{
    n->pprev = next->pprev;
    n->next = next;
    next->pprev = &n->next;
    *(n->pprev) = n;
}
 
static inline void hlist_add_after(struct hlist_node *n, struct hlist_node *next)
{
    next->next = n->next;
    n->next = next;
    next->pprev = &n->next;
 
    if (next->next) {
        next->next->pprev  = &next->next;
    }
}
 
#define hlist_entry(ptr, type, member) \
    CONTAINER_OF(ptr, type, member)
 
#ifdef typeof
 
#define hlist_for_each(pos, head) \
    for (pos = (head)->first; pos && ({ prefetch(pos->next); 1; }); \
         pos = pos->next)
 
#define hlist_for_each_safe(pos, n, head) \
    for (pos = (head)->first; pos && ({ n = pos->next; 1; }); \
         pos = n)
 
#define hlist_for_each_entry(tpos, pos, head, member) \
    for (pos = (head)->first; \
         pos && ({ prefetch(pos->next); 1;}) && \
            ({ tpos = hlist_entry(pos, typeof(*tpos), member); 1;}); \
         pos = pos->next)
 
#define hlist_for_each_entry_continue(tpos, pos, member) \
    for (pos = (pos)->next; \
         pos && ({ prefetch(pos->next); 1;}) && \
            ({ tpos = hlist_entry(pos, typeof(*tpos), member); 1;}); \
         pos = pos->next)
 
#define hlist_for_each_entry_from(tpos, pos, member) \
    for (; pos && ({ prefetch(pos->next); 1;}) && \
            ({ tpos = hlist_entry(pos, typeof(*tpos), member); 1;}); \
         pos = pos->next)
 
#define hlist_for_each_entry_safe(tpos, pos, n, head, member) \
    for (pos = (head)->first; \
         pos && ({ n = pos->next; 1; }) && \
            ({ tpos = hlist_entry(pos, typeof(*tpos), member); 1;}); \
         pos = n)
#else
 
#define hlist_for_each(pos, head) \
    for (pos = (head)->first; pos && prefetch(pos->next); pos = pos->next)
 
 
#if defined _MSC_VER || WIN32
    /**
     * fix warning on gcc: operation on ‘hn’ may be undefined [-Wsequence-point
     * but will cause error on Windows MSVC
     */
    #define hlist_for_each_safe(pos, n, head) \
        for (pos = (head)->first; pos && ((n=pos->next) == n); pos = n)
#else
    #define hlist_for_each_safe(pos, n, head) \
        for (pos = (head)->first; pos && ({ n = pos->next; 1; }); pos = n)
#endif
 
 
#define hlist_for_each_entry(tpos, typeof_tpos, pos, head, member) \
    for (pos = (head)->first; \
         pos && prefetch(pos->next) && \
           ((tpos = hlist_entry(pos, typeof_tpos, member)) == tpos); \
         pos = pos->next)
 
#define hlist_for_each_entry_continue(tpos, typeof_tpos, pos, member) \
    for (pos = (pos)->next; \
         pos && prefetch(pos->next) && \
            ((tpos = hlist_entry(pos, typeof_tpos, member)) == tpos); \
         pos = pos->next)
 
#define hlist_for_each_entry_from(tpos, typeof_tpos, pos, member) \
    for (; pos && prefetch(pos->next) && \
            ((tpos = hlist_entry(pos, typeof_tpos, member)) == tpos); \
         pos = pos->next)
 
#define hlist_for_each_entry_safe(tpos, typeof_tpos, pos, n, head, member) \
    for (pos = (head)->first; \
         pos && ((n = pos->next) == n) && \
            ((tpos = hlist_entry(pos, typeof_tpos, member)) == tpos); \
         pos = n)
 
#endif /* typeof */
 
#if defined(__cplusplus)
}
#endif
 
#endif /* _DH_LIST_H */