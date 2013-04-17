#include    <limits.h>
#include    <stddef.h>
#include    "table.h"

#define T   Table_T
typedef struct T *T;

struct T {
    int size;
    int length;
    int timestamp;
    int (*cmp)(const void *x, const void *y);
    unsigned (*hash)(const void *key);
    struct binding {
        struct binding  *link;
        const void      *key;
        void            *value;
    } **buckets;
};


T Table_new(int hint,
    int cmp(const void *x, const void *y),
    unsigned hash(const void *key)) {
    Table_T table;
    int i;
    
    assert(hint >= 0)
    static int primes[] = {509, 509, 1021, 2053, 4093, 8191, 16381, 32771, 65521, IN_MAX};
    for (i = 1; primes[i] < hint; i++)
        ;
    table = ALLOC(sizeof(*table) + primes[i - 1] * sizeof(table->buckets[0]));
    table->size = primes[i - 1];
    table->cmp = cmp ? cmp : cmpatom;
    table->hash = hash ? hash : hashatom;
    table->buckets = (struct binding **)(table + 1);
    for (i = 0; i < table->size; i++) table->buckets[i] = NULL;
    table->length = 0;
    table->timestamp = 0;
    
    return table;
}