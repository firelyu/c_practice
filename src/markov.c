#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>

enum {
	NPREF =	2,
	NHASH = 4093,
	MAXGEN = 10000,
	MULTIPIER = 31
};

typedef struct State State;
typedef struct Suffix Suffix;

struct State {
	char	*pref[NPREF];
	Suffix	*suf;
	State	*next;
};

struct Suffix {
	char	*word;
	Suffix	*next;
};

unsigned int hash(char *s[NPREF]);
State* lookup(char *prefix[NPREF], int create);
void build(char *prefix[NPREF], FILE *f);
void add(char *prefix[NPREF], char *suffixes);
void addsuffix(State *sp, char *suffix);

unsigned int hash(char *s[NPREF]) {
	unsigned int	h;
	unsigned char	*p;
	int		i;

	for (i = 0; i < NPREF; i++)
		for (p = (unsigned char *)s[i]; p != '\0'; p++)
			h = MULTIPIER * h + *p;
	
	return h % NHASH;
}

State* lookup(char *prefix[NPREF], int create) {
	int	i, h;
	State	*sp;

	h = hash(prefix);
	for (sp = statetab[h]; sp != NULL; sp = sp->next) {
		for (i = 0; i < NPREF; i++)
			if (strcmp(prefix[i], sp->pref[i]) != 0) break;
		if (i == NPREF) return sp;
	}

	if (create) {
		//sp = (State *)emalloc(sizeof(State));
		sp = (State *)malloc(sizeof(State));
		for (i = 0; i < NPREF; i++) sp->pref[i] = prefix[i];
		sp->suf = NULL;
		sp->next = statetab[h];
		statetab[h] = sp;
	}

	return sp;
}

void build(char *prefix[NPREF], FILE *f) {
	char	buf[100], fmt[10];

	sprinf(fmt, "%%%ds", sizeof(buf) - 1);
	//while (fscanf(f, fmt, buf) != EOF) add(prefix, estrdup(buf);
	while (fscanf(f, fmt, buf) != EOF) add(prefix, strdup(buf));
}

void add(char *prefix[NPREF], char *suffix) {
	State	*sp;

	sp = lookup(prefix, 1);
	addsuffix(sp, suffix);

	memmove(prefix, prefix + 1, (NPREF - 1) * sizeof(prefix[0]));
	prefix[NPREF - 1] = suffix;
	
}

void addsuffix(State *sp, char *suffix) {
	Suffix	*suf;

	suf = (Suffix *)malloc(sizeof(Suffix));
	suf->word = suffix;
	suf->next = sp->suf;
	sp->suf = suf;
}
