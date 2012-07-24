#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>

#define		STRING_BUF_SIZE		128
#define		INOUT_THE_CHOICE	"Please input your choise : "
#define		clean_input()		while(getchar() != '\n') continue;

typedef	struct car {
	char		brand[STRING_BUF_SIZE + 1];
	char		model[STRING_BUF_SIZE + 1];
	struct car	*next;
} car;

void show_menu();
int show_item(car *head);
car *new_item(char *brand, char *model);
car *add_item(car *head, car *new);
int add_item2(car **head, car *new);
car *delete_item(car *head, unsigned int index);
int delete_item2(car **head, unsigned int index);

int show_item(car *head) {
	unsigned int	index = 0;

	for (; head != NULL; head = head->next) {
		index ++;
		printf("%u Brand : %s\t\tModel : %s\n", index,  head->brand, head->model);
	}

	return 0;
}

car *new_item(char *brand, char *model) {
	car	*new = (car*)malloc(sizeof(car));
	
	new->brand[STRING_BUF_SIZE] = '\0';
	new->model[STRING_BUF_SIZE] = '\0';

	strncpy(new->brand, brand, STRING_BUF_SIZE);
	strncpy(new->model, model, STRING_BUF_SIZE);
	new->next = NULL;

	return new;
}

car *add_item(car *head, car *new) {
	new->next = head;
	head = new;
	
	return head;
}

int add_item2(car **head, car *new) {
	new->next = *head;
	*head = new;

	return 0;
}

car *delete_item(car *head, unsigned int index) {
	car	*p, *pre;
	int i;
	
	if (head == NULL) {
		printf("The list is empty.\n");
		return NULL;
	}

	pre = NULL;
	i = 1;
	for (p = head; p != NULL; p = p->next, i++) {
		if (i == index) {
			if (pre == NULL) head = p->next;
			else pre->next = p->next;

			free(p);
			return head;
		}
		pre = p;
	}

	printf("Index %u not in the list.\n", index);
	return head;
}

int delete_item2(car **head, unsigned int index) {
	car *p, *pre;
	int i;

	if (head == NULL) {
		printf("The list is empty.\n");
		return 1;
	}
	
	if (index < 1) {
		printf("The index must be larger than 1.\n");
		return 1;
	}
	else {
		pre = NULL;
		
		i = 1;
		for (p = *head; p != NULL; p = p->next, i++) {
			if (i == index) {
				if (pre == NULL) *head = p->next;
				else pre->next = p->next;
				
				free(p);
				return 0;
			}
			pre = p;
		}
	}

	printf("Index %u not in the list.\n", index);
	return 1;
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
	char	ibrand[STRING_BUF_SIZE + 1], imodel[STRING_BUF_SIZE + 1];
	unsigned int	index;

	show_menu();
	printf(INOUT_THE_CHOICE);
	while(choice = getchar()) {
		switch(choice) {
			case 'A':
			case 'a':
				clean_input();

				printf("Input the brand : ");
				scanf("%s", ibrand);
				ibrand[STRING_BUF_SIZE] = '\0';
				printf("Input the model : ");
				scanf("%s", imodel);
				imodel[STRING_BUF_SIZE] = '\0';

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
				//add_item2(&head, new);
				break;
			
			case 'D':
			case 'd':
				clean_input();

				printf("Delete the item from the list, input the index : ");
				scanf("%u", &index);
				head = delete_item(head, index);
				//delete_item2(&head, index);
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
		clean_input();
		printf("\n");
		printf(INOUT_THE_CHOICE);
	}

end:
	return 0;
}
