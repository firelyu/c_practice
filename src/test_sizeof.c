//
//  testSizeof.c
//  WordFrequencies
//
//  Created by Liao Yu on 13-6-2.
//  Copyright (c) 2013年 Liao Yu. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>

int test_sizeof();
char *int2str(const long n);

int test_sizeof() {
    char    str[]   = "Hello";
    char    *s      = str + sizeof(str);
    char    *pc     = NULL;

    printf("str=%s sizeof(str) is %ld.\n",
           str, sizeof(str));
    printf("s=%s\n", s);
    
    printf("Map of str\n");
    for (pc = str; pc != str + sizeof(str); pc++) {
        printf("%p : %c\n", pc, *pc);
    }
    printf("\n");
    
    printf("Map of s\n");
    printf("%p : %c\n", s, *s);
    
    return 0;
}

char *int2str(const long n){
    char    str[34];
    char    *s = str + sizeof(str);
    char    *result;
    unsigned long   m;
    size_t  len;
    
    if (n == LONG_MIN)  m = LONG_MAX + 1UL;
    else if (n < 0)     m = -n;
    else                m = n;
    
    do {
        *--s = m % 10 + '0';
    } while ((m /= 10) > 0);
    
    if (n < 0) {
        *--s = '-';
    }
    
    len = str + sizeof(str) - s;
    result = (char*)malloc((len + 1) * sizeof(char));
    
    strncpy(result, s, len);
    result[len] = '\0';
    
    return result;
}

int main(int argc, char *argv[]) {
    long    num;
    char    *str;
    
    while (1) {
        printf("Input a number, I will convert it to the string : ");
        scanf("%ld", &num);
        str = int2str(num);
        printf("The input number is %ld, the string is %s(%zd).\n",
               num, str, strlen(str));
        free(str);
    }
    
    return 0;
}