//
//  test_exception.c
//  WordFrequencies
//
//  Created by Liao Yu on 13-6-14.
//  Copyright (c) 2013年 Liao Yu. All rights reserved.
//

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <setjmp.h>

jmp_buf j;

void exception_divide() {
    printf("The divisor is negative.\n");
}

// Divide a by b
double divide(double a, double b) {
//    if (0 == b) {
//        atexit(exception_divide);
//        exit(EXIT_FAILURE);
//    }
    
//    assert(0 != b);
    
//    if (0 == b) {
//        abort();
//    }
    
    if (0 == b) {
        longjmp(j, 1);
    }
    
    return a / b;
}

int main(int argc, char *argv[]) {
    double  a, b, c;
    
    printf("Input the a : ");
    scanf("%lf", &a);
    
    printf("Input the b : ");
    scanf("%lf", &b);
    
    if (setjmp(j) != 0) {
        fprintf(stderr, "The divisor is negative. %s %d\n",
                __FILE__, __LINE__);
        exit(EXIT_FAILURE);
    }
    
    c = divide(a, b);
    
    printf("%lf / %lf = %lf\n",
           a, b, c);
    
    return 0;
}