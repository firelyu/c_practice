#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>

#define		STRING_BUF_SIZE	128

#define		clean_input()	while(getchar() != '\n') continue;

typedef struct car {
	char		brand[STRING_BUF_SIZE + 1];
	char		country[STRING_BUF_SIZE + 1];
	struct car	*left;
	struct car	*right;
} car;

car *newitem(char *brand, char *country);
car *insert(car *treep, car *newp);
car *lookup(car *treep, char *brand);
void applyinorder(car *treep, void (*fn)(car*, void*), void *arg);
void show(car *treep, void *arg);
void show_menu();

car *newitem(char *brand, char *country) {
	car *newp = (car*)malloc(sizeof(car));
	
	newp->brand[STRING_BUF_SIZE + 1] = '\0';
	newp->country[STRING_BUF_SIZE + 1] = '\0';

	strncpy(newp->brand, brand, STRING_BUF_SIZE);
	strncpy(newp->country, country, STRING_BUF_SIZE);
	newp->left = NULL;
	newp->right = NULL;

	return newp;
}

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

void applyinorder(car *treep, void (*fn)(car*, void*), void *arg){
	if (treep == NULL) return;

	applyinorder(treep->left, fn, arg);
	(*fn)(treep, arg);
	applyinorder(treep->right, fn, arg);
}

void show(car *treep, void *arg) {
	printf("Brand : %s\t\tCountry : %s\n", treep->brand, treep->country);
}

void show_menu() {
	printf("Btree menu:\n");
	printf(" A/a - Add a node.\n");
	printf(" S/s - Show all the node in the tree.\n");
	printf(" H/h/? - Show the menu.\n");
	printf(" Q/q - Quit the menu.\n");
}

int main() {
	char	choice;
	car	*top, *newp;
	char	ibrand[STRING_BUF_SIZE + 1], icountry[STRING_BUF_SIZE + 1];
	
	top = NULL;
	printf("Input your choice : ");
	while (choice = getchar()) {
		switch(choice) {
			case 'a':
			case 'A':
				printf("Input the brand : ");
				scanf("%s", ibrand);
				ibrand[STRING_BUF_SIZE] = '\0';
				printf("Input the country : ");
				scanf("%s", icountry);
				icountry[STRING_BUF_SIZE] = '\0';
				
				newp = newitem(ibrand, icountry);
				top = insert(top, newp);
				break;
			case 's':
			case 'S':
				if (top == NULL) printf("There is no node in the Btree.\n"); 
				else applyinorder(top, show, NULL);
				break;
			case 'h':
			case 'H':
			case '?':
				show_menu();
				break;
			case 'q':
			case 'Q':
				printf("Goodbye, guys.\n");
				goto end;
			default:
				printf("\033[31m");
				printf("Invalid choise.");
				printf("\033[0m");
				printf("\n");
				break;
		}
		clean_input();
		printf("\n");
		printf("Input your choice : ");
	}

end:
	return 0;
}
