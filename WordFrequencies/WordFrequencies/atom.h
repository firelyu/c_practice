//
//  atom.h
//  WordFrequencies
//
//  Created by Liao Yu on 13-4-18.
//  Copyright (c) 2013年 Liao Yu. All rights reserved.
//

#ifndef WordFrequencies_atom_h
#define WordFrequencies_atom_h

extern int Atom_length(const char *str);
extern const char *Atom_new(const char *str, int len);
extern const char *Atom_string(const char *str);
extern const char *Atom_int(long n);

#endif
