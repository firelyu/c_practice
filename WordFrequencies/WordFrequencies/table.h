#ifndef     TABLE_H
#define     TABLE_H

#define T Table_T
typedef struct T *T;

extern T Table_new(int hint,
    int cmp(const void *x, const void *y),
    unsigned hash(const void *key));
extern void Table_free(T *table);
extern int Table_length(T table);
// Add the key-value pair to the table.
// If the key exist, overwrite its value with the new one. And return the previous value.
// Otherwise, add the key-value pair. Return the NULL. 
extern void *Table_put(T table, const void *key, const void *value);
// Search the table for the key. If found, return the value.
// Otherwise, return the NULL.
extern void *Table_get(T table, const void *key);
// Search the table for the key. If found, remove the key-value pair.
// Return the removed value.
// Otherwise, no change to the table, and return NULL.
extern void *Table_remove(T table, const void *key);
extern void Table_map(T table,
    void apply(const void *key, void **value, void *cl),
    void *cl);
// Give a table of N key-value pairs, Table_toArray builds a array with 2N + 1 elements.
// Return the 1st element in the array.
extern void **Table_toArray(T table, void *end);

static int cmpatom(const void *x, const void *y);
static unsigned hashatom(const void *key);

#undef T
#endif