#include	<stdio.h>

#define		INOUT_THE_CHOICE	"Please input your choise:"

void show_menu()
{
	printf("Choice menu:");
	printf(" A/a - Add a new item to the list.\n");
	printf(" D/d - Delete a item from the list.\n");
	printf(" S/s - Show the items in the list.\n");
	printf(" C/c - Create the list.\n");
	printf(" H/h/? - Show the help.\n");
	printf(" Q/q - Quit.");
}

int main(int argc, char *argv[])
{
	char	choice;
	
	show_menu();
	printf(INOUT_THE_CHOICE);
	while(choice = getchar()) {
		switch(choice) {
			case 'A':
			case 'a':
				printf("Add a new item to the list.\n");
				break;
			case 'D':
			case 'd':
				printf("Delete a item from the list.\n");
				break;
			case 'S':
			case 's':
				printf("Show the items in the list.\n");
				break;
			case 'C':
			case 'c':
				printf("Create the list.\n");
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
				printf("Invalid choice.\n");
				show_menu();
				break;
		}
		while(getchar() != '\n') continue;
		printf(INOUT_THE_CHOICE);
	}

end:
	return 0;
}
