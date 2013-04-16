#ifndef     GETWORD_H
#define     GETWORD_H

extern int getword(FILE *fp, char *buf, int size,
    int first(int c), int rest(int c));

#endif