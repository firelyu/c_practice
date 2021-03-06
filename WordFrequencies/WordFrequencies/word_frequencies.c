#include    <stdio.h>
#include    <stdlib.h>
#include    <errno.h>
#include    <ctype.h>
#include    "table.h"
#include    "getword.h"

int first(int c);
int rest(int c);
int compare(const void *x, const void *y);
void vfree(const void *key, void **count, void *cl);
void wf(char *filename, FILE *fp);

int first(int c) {
    return isalpha(c);
}

int rest(int c) {
    return isalpha(c) || c == '_';
}

int compare(const void *x, const void *y) {
    return strcmp(*(char **)x, *(char **)y);
}

void vfree(const void *key, void **count, void *cl) {
    FREE(*count);
}

void wf(char *filename, FILE *fp) {
    Table_T table = Table_new(0, NULL, NULL);
    char buf[128];
    
    while (getword(fp, buf, sizeof(buf), first, rest)) {
        int i, *count;
        const char *word;
        
        for (i = 0; buf[i] != '\0'; i++) {
            buf[i] = tolower(buf[i]);
        }
        word = Atom_string(buf);
        count = Table_get(table, word);
        if (count) {
            (*count)++;
        }
        else {
            NEW(count);
            *count = 1;
            Table_put(table, word, count);
        }
    }
    
    if (filename) printf("%s:\n", filename);
    // Print the words.
    {
        int i = 0;
        void **array = Table_toArray(table, NULL);
        qsort(array, Table_length(table), 2 * sizeof(*array), compare);
        for (i = 0; array[i] != NULL; i += 2) {
            printf("%d\t%s\n", *(int *)array[i + 1], (char *)array[i]);
        }
        FREE(array);
    }
    
    // Deallocate the entries and the table.
    Table_map(table, vfree, NULL);
    Table_free(&table);
    
}

int main(int argc, char *argv[]) {
    int i;
    
    for (i = 1; i < argc; i++) {
        FILE *fp = fopen(argv[i], 'r');
        if (fp == NULL) {
            fprintf(stderr, "%s: Can't open '%s' (%s)\n",
                argv[0], argv[i], strerror(errno));
            return EXIT_FAILURE;
        }
        else {
            wf(argv[i], fp);
            fclose(fp);
        }
    }
    if (argc == 1) wf(NULL, stdin);
    return EXIT_SUCCESS;
}