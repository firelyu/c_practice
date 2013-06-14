//
//  except.c
//  WordFrequencies
//
//  Created by Liao Yu on 13-6-14.
//  Copyright (c) 2013年 Liao Yu. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include "except.h"

#define T Except_T

Except_Frame *Except_stack = NULL;

void Except_raise(const T *e, const char *file, int line) {
    Except_Frame *p = Except_stack;
    
    if (p == NULL) {
        fprintf(stderr, "Uncaught exception");
        if (e->reason)
            fprintf(stderr, " %s", e->reason);
        else
            fprintf(stderr, " at 0x%p", e);
        if (file && line > 0)
            fprintf(stderr, " raised at %s:%d\n", file, line);
        fprintf(stderr, "aborting...\n");
        fflush(stderr);
        abort();
    }
    
    p->exception = e;
    p->file = file;
    p->line = line;
    
    Except_stack =  Except_stack->prev;
    longjmp(p->env, Except_raised);
}