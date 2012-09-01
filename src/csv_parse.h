#ifndef     CSV_PARSE_HH
#define     CSV_PARSE_HH

#include    <stdio.h>
#include    <stdlib.h>
#include    <string.h>

#define     INIT_LENGTH     32

enum {NOMEN = -2};              // out of memory singal

static char *line           = NULL; // input char
static char *sline          = NULL; // line copy used by split
static int  maxlinesize     = 0;    // size of line[] and sline[]
static char **fields        = NULL; // field pointer array
static int  maxfieldsize    = 0;    // size of field[]
static int  nfield          = 0;    // number of fields in field[]

static char fieldsep[]      = ",";  // field separator chars

void reset();
static int endofline(FILE *fin, int c);
static char* advquoted(char *str);
static int splite();

char* csvgetline(FILE *f);
char* csvfield(int n);
int csvnfield();

#endif
