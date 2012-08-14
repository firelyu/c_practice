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

State	*statetab[NHASH];
char	NONWORD[] = "\n";

unsigned int hash(char *s[NPREF]);
State* lookup(char *prefix[NPREF], int create);
void build(char *prefix[NPREF], FILE *f);
void add(char *prefix[NPREF], char *suffixes);
void addsuffix(State *sp, char *suffix);
void generate(int nwords);
void usage();

unsigned int hash(char *s[NPREF]) {
	unsigned int	h;
	unsigned char	*p;
	int		i;

	for (i = 0; i < NPREF; i++)
		for (p = (unsigned char *)s[i]; *p != '\0'; p++)
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

	sprintf(fmt, "%%%lds", sizeof(buf) - 1);
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

void generate(int nwords) {
	State	*sp;
	Suffix	*suf;
	char	*prefix[NPREF], *w;
	int	i, nmatch;

	for (i = 0; i < NPREF; i++) prefix[i] = NONWORD;

	for (i = 0; i < nwords; i++) {
		sp = lookup(prefix, 0);
		nmatch = 0;
		for (suf = sp->suf; suf != NULL; suf = suf->next)
			if (rand() % ++nmatch == 0)
				w = suf->word;
		if (strcmp(w, NONWORD) == 0) break;
		printf("%s\n", w);
		memmove(prefix, prefix + 1, (NPREF - 1) * sizeof(prefix[0]));
		prefix[NPREF - 1] = w;
	}
}

void usage() {
	printf("Usage:\n");
	printf("	markov <filename>\n");
	printf("<filename> - The filename contain the plain text.\n");
}

int main(int argc, char	*argv[]) {
	int	i, nwords = MAXGEN;
	char	*prefix[NPREF], *filename;
	FILE	*fh;

	if (argc < 2) {
		usage();
		return 1;
	}
	
	filename = argv[1];
	fh = fopen(filename, "r");
	if (fh == NULL) {
		perror("Open file");
		return 1;
	}

	for (i = 0; i < NPREF; i++) prefix[i] = NONWORD;
	build(prefix, fh);
	add(prefix, NONWORD);
	generate(nwords);

	if (fclose(fh) != 0 ) {
		perror("Close file");
		return 1;
	}
	return 0;
}
