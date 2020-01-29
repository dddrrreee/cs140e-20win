// engler, cs140e: brain-dead generic queue. don't want to use STL/c++ in the kernel.
#ifndef __Q_H__
#define __Q_H__
#ifndef E
#	error "Client must define the Q datatype <E>"
#endif

typedef struct Q {
    E *head, *tail;
} Q_t;

// used for iteration.
static E *Q_start(Q_t *q)   { return q->head; }
static E *Q_next(E *e)      { return e->next; }
static int Q_empty(Q_t *q)  { return q->head == 0; }

// remove from front of list.
static E *Q_pop(Q_t *q) {
    demand(q, bad input);
    E *e = q->head;

    if(!e) {
        demand(!q->tail, invalid Q);
        return 0;
    }
    q->head = e->next;
    if(!q->head)
        q->tail = 0;
    return e;
}

// insert at tail. (for FIFO)
static void Q_append(Q_t *q, E *e) {
    e->next = 0;
    if(!q->tail) 
        q->head = q->tail = e;
    else {
        q->tail->next = e;
        q->tail = e;
    }
}

// insert at head (for LIFO)
static void Q_push(Q_t *q, E *e) {
    e->next = q->head;
    q->head = e;
    if(!q->tail)
        q->tail = e;
}

// insert <e_new> after <e>: <e>=<null> means put at head.
static void Q_insert_after(Q_t *q, E *e, E *e_new) {
    if(!e)
        Q_push(q,e_new);
    else if(q->tail == e)
        Q_append(q,e_new);
    else {
        e_new->next = e->next;
        e->next = e_new;
    }
}
#endif
