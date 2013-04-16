#include    <stdio.h>
#include    <stdlib.h>
#include    <errno.h>
#include    <ctype.h>
#include    "table.h"
#include    "getword.h"

void wf(char *filename, FILE *fp);


void wf(char *filename, FILE *fp) {
    Table_T table = Table_new(0, NULL, NULL);
    char buf[128];
    
    while (getword(fp, buf, sizeof(buf), first, rest) {
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
    
    if (filename) print("%s:\n", filename);
    // Print the words.
    // Deallocate the entries and the table.
}

int main(int argc, char *argv[]) {
    int i;
    
    for (i = 1; i < argc, i++) {
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