#pragma once

#include <list.h>

typedef struct priority_queue {
    list_node_t head;
    int (*compare)(const void *, const void *);
} priority_queue_t;

static inline void priority_queue_init(priority_queue_t *pq, int (*compare)(const void *, const void *)) {
    list_init_head(&pq->head);
    pq->compare = compare;
}

static inline void priority_queue_push(priority_queue_t *pq, list_node_t *node) {
    list_node_t *pos;
    list_for_each(&pq->head, pos) {
        if (pq->compare(node, pos) < 0) {
            list_insert(pos, node);
            return;
        }
    }
    list_push_back(&pq->head, node);
}

static inline list_node_t *priority_queue_pop(priority_queue_t *pq) {
    if (list_empty(&pq->head)) {
        return NULL;
    }
    list_node_t *front = list_front(&pq->head);
    list_pop_front(&pq->head);
    return front;
}

static inline list_node_t *priority_queue_top(priority_queue_t *pq) {
    return list_empty(&pq->head) ? NULL : list_front(&pq->head);
}

static inline void priority_queue_remove(priority_queue_t *pq __attribute__((unused)), list_node_t *node) {
    list_remove(node);
}

static inline bool priority_queue_empty(priority_queue_t *pq) {
    return list_empty(&pq->head);
}
