
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
			if (strcmp(prefix[i], sp->pref[i]) != 0) then break;
		if (i == NPREF) return sp;
	}

	if (create) {
		sp = (State *)emalloc(sizeof(State));
		for (i = 0; i < NPREF; i++) sp->pref[i] = prefix[i];
		sp->suf = NULL;
		sp->next = statetab[h];
		statetab[h] = sp;
	}

	return sp;
}
