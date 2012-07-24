#include	<stdio.h>
#include	

#define		STR_BUF_SIZE	128

typedef struct car {
	char		brand[STR_BUF_SIZE];
	unsigned int	country;
	struct car	*left;
	struct car	*right;
} car

car *insert(car *treep, car *newp);
car *lookup(car *treep, char *brand);

car *insert(car *treep, car *newp) {
	int	cmp;

	if (treep == NULL) return newp;

	cmp = strcmp(newp->brand, treep->brand);

	if (cmp == 0) printf("Insert: duplicate entry %s ignored.", newp->brand);
	else if (cmp < 0) treep->left = insert(treep->left, newp);
	else treep->right = insert(treep->right, newp);

	return treep;
}

car *lookup(car *treep, char *brand) {
	int cmp;

	if (treep == NULL) return NULL;

	cmp = strcmp(brand, treep->brand);

	if (cmp == 0) return treep;
	else if (cmp < 0) return lookup(treep->left, brand);
	else return lookup(treep->right, brand);

}


