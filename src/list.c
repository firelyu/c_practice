#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>

#define		STR_BUG_SIZE		128
#define		INOUT_THE_CHOICE	"Please input your choise : "

typedef	struct car {
	char		brand[STR_BUG_SIZE];
	char		model[STR_BUG_SIZE];
	struct car	*next;
} car;

int show_item(car *head);
car *new_item(char *brand, char *model);
car *add_item(car *head, car *new);

int show_item(car *head) {
	for (; head != NULL; head = head->next)
		printf("Brand : %s\t\tModel : %s\n", head->brand, head->model);
	
	return 0;
}

car *new_item(char *brand, char *model) {
	car	*new = (car*)malloc(sizeof(car));
	
	new->brand[STR_BUG_SIZE] = '\0';
	new->model[STR_BUG_SIZE] = '\0';

	strncpy(new->brand, brand, STR_BUG_SIZE);
	strncpy(new->model, model, STR_BUG_SIZE);
	new->next = NULL;

	return new;
}

car *add_item(car *head, car *new) {
	new->next = head;
	head = new;
	
	return head;
}

void show_menu()
{
	printf("Choice menu:\n");
	printf(" A/a - Add a new item to the list.\n");
	printf(" D/d - Delete a item from the list.\n");
	printf(" S/s - Show the items in the list.\n");
	printf(" H/h/? - Show the help.\n");
	printf(" Q/q - Quit.\n");
}

int main(int argc, char *argv[])
{
	char	choice;
	car	*head = NULL, *new = NULL;
	char	ibrand[STR_BUG_SIZE], imodel[STR_BUG_SIZE];
	
	show_menu();
	printf(INOUT_THE_CHOICE);
	while(choice = getchar()) {
		switch(choice) {
			case 'A':
			case 'a':
				printf("Input the brand : ");
				scanf("%s", ibrand);
				printf("Input the model : ");
				scanf("%s", imodel);

				new = new_item(ibrand, imodel);
				if (new == NULL) {
					printf("Can't new the item.\n");
					break;
				}

				head = add_item(head, new);
				if (head == NULL) {
					printf("Can't add the item to the list");
					break;
				}
				break;
			
			case 'D':
			case 'd':
				printf("Delete a item from the list.\n");
				break;
			
			case 'S':
			case 's':
				if (head == NULL) {
					printf("There is no item in the list.\n");
					break;
				}
				show_item(head);
				break;
			
			case 'H':
			case 'h':
			case '?':
				show_menu();
				break;
			
			case 'Q':
			case 'q':
				printf("Goodbye, Guy.\n");
				goto end;
			
			default:
				printf("\033[31m");
				printf("Invalid choice.");
				printf("\033[0m");
				printf("\n");
				show_menu();
				break;
		}
		while(getchar() != '\n') continue;
		printf(INOUT_THE_CHOICE);
	}

end:
	return 0;
}
