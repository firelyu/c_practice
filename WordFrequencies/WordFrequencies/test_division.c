//
//  test_division.c
//  WordFrequencies
//
//  Created by Liao Yu on 13-5-14.
//  Copyright (c) 2013年 Liao Yu. All rights reserved.
//

#include <stdio.h>
#include <string.h>
#include "arith.h"

int main(int argc, char *argv[]) {
    int x, y;
    
    printf("Input the x: ");
    if (scanf("%d", &x) == 0)
        perror("Input the x");
    printf("Input the y: ");
    if (scanf("%d", &y) == 0)
        perror("Input the y");
    
    printf("System division\n");
    printf("%d / %d = %d\n", x, y, x/y);
    printf("%d %% %d = %d\n", x, y, x%y);
    
    printf("arith.h division\n");
    printf("Arith_div(%d, %d) = %d\n", x, y, Arith_div(x, y));
    printf("Arith_mod(%d, %d) = %d\n", x, y, Arith_mod(x, y));
    
    return 0;
}
