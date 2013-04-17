#include    <limits.h>
#include    <stddef.h>
#include    "table.h"

#define T Table_T

struct T {
    int         size;
    int         length;
    unsigned    timestamp;
    int (*cmp)(const void *x, const void *y);
    unsigned (*hash)(const void *key);
    struct binding {
        struct binding  *link;
        const void      *key;
        void            *value;
    } **buckets;
};

static int cmpatom(const void *x, const void *y) {
    return x != y;
}

static unsigned hashatom(const void *key) {
    return (unsigned long)key >> 2;
}

T Table_new(int hint,
    int cmp(const void *x, const void *y),
    unsigned hash(const void *key)) {
    Table_T     table;
    int         i;
    
    assert(hint >= 0);
    
    static int primes[] = {509, 509, 1021, 2053, 4093, 8191, 16381, 32771, 65521, INT_MAX};
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

void *Table_get(T table, const void *key) {
    unsigned        i;
    struct binding  *pt;
    
    assert(table);
    assert(key);
    
    i = table->hash(key) % table->size;
    for (pt = table->buckets[i]; pt != NULL; pt = pt->link) {
        if (table->cmp(pt->key, key) == 0) {
            break;
        }
    }
    
    return pt ? pt->value : NULL;
}

void *Table_put(T table, const void *key, const void *value) {
    void            *preValue = NULL;
    unsigned        i;
    struct binding  *pt;
    
    assert(table);
    assert(key);
    
    i = table->hash(key) % table->size;
    for (pt = table->buckets[i]; pt != NULL; pt = pt->link) {
        if (table->cmp(pt->key, key) == 0) {
            break;
        }
    }
    
    if (pt == NULL) {
        NEW(pt);
        pt->key = key;
        pt->value = value;
        pt->link = table->buckets[i];
        table->buckets[i] = pt;
        table->length++;
    }
    else {
        preValue = pt->value;
        pt->value = value;
    }
    
    table->timestamp++;
    
    return preValue;
}

int Table_length(T table) {
    assert(table);
    return table->length;
}

void Table_map(T table,
    void apply(const void *key, void **value, void *cl), void *cl) {
    int             i;
    struct binding  *pt;
    unsigned        stamp;
    
    assert(table);
    assert(apply);
    
    stamp = table->timestamp;
    for (i = 0; i < table->size; i++) {
        for (pt = table->buckets[i]; pt != NULL; pt = pt->link) {
            apply(pt->key, &pt->value, cl);
            assert(table->timestamp == stamp);
        }
    }
}

void *Table_remove(T table, const void *key) {
    int             i;
    struct binding  **ppt;
    
    assert(table);
    assert(key);
    
    i = table->hash(key) % table->size;
    // ppt point to the previou struct.
    // *ppt point to the current struct.
    // check the *ppt->key
    for (ppt = &table->buckets[i]; *ppt != NULL; ppt = &(*ppt)->link) {
        if (table->cmp((*ppt)->key, key) == 0) {
            struct binding  *pt = *ppt;
            void            *value = pt->value;
            *ppt = pt->link;
            FREE(pt);
            table->length--;
            return value;
        }
    }
    
    return NULL;
}

void **Table_toArray(T table, void *end) {
    void            **array;
    struct binding  *pt;
    int             i, j = 0;
    
    assert(table);
    
    array = ALLOC((2 * table->length + 1) * sizeof(*array));
    for (i = 0; i < table->size; i++) {
        for (pt = table->buckets[i]; pt != NULL; pt = pt->link) {
            array[j++] = (void *)pt->key;
            array[j++] = pt->value;
        }
    }
    array[j] = end;
    
    return array;
}

void Table_free(T *table) {
    assert(table && *table);
    if ((*table)->length > 0) {
        int             i;
        struct binding  *pt, *qt;
        
        for (i = 0; i < (*table)->size; i++) {
            for (pt = (*table)->buckets[i]; pt != NULL; pt = qt) {
                qt = pt->link;
                FREE(pt);
            }
        }
    }
    FREE(*table);
}