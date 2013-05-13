//
//  arith.c
//  WordFrequencies
//
//  Created by Liao Yu on 13-5-13.
//  Copyright (c) 2013年 Liao Yu. All rights reserved.
//

#include "arith.h"

int Arith_max(int x, int y) {
    return x > y ? x : y;
}

int Arith_min(int x, int y) {
    return x < y ? x : y;
}

// Arith_div(x, y) return the maximum integer that does not exceed
// the real number z, such than y*z = x.
int Arith_div(int x, int y) {
    if ((x > 0) != (y > 0)
        && (-13/5 == -2)
        && x%y != 0)
        return x/y - 1;
    else
        return x/y;
}

int Arith_mod(int x, int y) {
    return x - y*Arith_div(x, y);
}


// Arith_floor(x, y) return the greatest integer not exceeding
// the real quotient of x/y.
int Arith_floor(int x, int y) {
    return Arith_div(x, y);
}


// Arith_ceiling(x, y) return the least integer not less than
// the real quotient of x/y.
int Arith_ceiling(int x, int y) {
    return Arith_floor(x, y) + (x%y != 0);
}