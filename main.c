/* Program Menu Test */
#include <X11/Xlib.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

//#include "MenuEntry.h"

typedef struct MenuEntry {
	char displayString[50];
	char execString[50];
	struct MenuEntry *next;
} MenuEntry;

/* head of the list */
MenuEntry *menuHead;

/* The menu window */
Window menWindow;

/* X11 display */
Display *d;

/* Linked list of menu entries */
//MenuEntry *menuEntryHead = NULL;

XEvent e; // event structure

Bool initX(void); // open the x server
void createWindow(void); // initializes the menWindow

Bool addMenuEntry(const char *dispstring, const char *execstring);
Bool fillMenuList(const char *fname);
Bool freeMenuList(void);
void printMenuList(void);


int main(int argc, char **argv)
{
    /* Make sure args were passed */
    if(argc != 2){
        printf("Usage: %s [menu_file]\n", argv[0]);
        return 0;
    }

	/* read from the menu file and fill the list */
	fillMenuList(argv[1]);

	/* test print */
	printMenuList();
 
    /* Open the display */
    if(!initX()) return -1;
    
    /* initialize and start drawing the window */
    createWindow();
    
    while(1)
    {
        XNextEvent(d, &e);
        if(e.type == Expose) {
            /* draw something to the window */
            XDrawRectangle(d, menWindow, DefaultGC(d, DefaultScreen(d)), 20, 20, 10, 10);
        }
        else if(e.type == KeyPress) {
            break;
        }
    }
    
    /* Shut stuff down */
	freeMenuList();
    if(menWindow) XDestroyWindow(d, menWindow);
    if(d) XCloseDisplay(d);
    return 0;
}

Bool initX(void)
{
    /* open the connection to the server */
    d = XOpenDisplay(NULL); // open the default display
    if(!d){
        fprintf(stderr, "Failed to open display!\n");
        return False;
    }
    
    return True;
}

void createWindow(void)
{
    /* create the window */
    menWindow = XCreateSimpleWindow(
        d,
        RootWindow(d, DefaultScreen(d)),    // Display *parent
        0,                                // x coord
        0,                                // y coord
        100,           // window width
        100,              // window height
        1,                       // border size
        BlackPixel(d, DefaultScreen(d)),    // border
        WhitePixel(d, DefaultScreen(d))     // background
    );
    
    /* select input for the window */
    XSelectInput(d, menWindow, ExposureMask | KeyPressMask);
    
    /* Make the window visible */
    XMapWindow(d, menWindow);
    
}

Bool readMenuList(char *fname, MenuEntry *head)
{
	/* initialize the start of the list */
	//menuHead = (MenuEntry *)malloc(sizeof(MenuEntry));
	MenuEntry *m = head;

	FILE *fp = fopen(fname, "r");
	if(!fp){
		fprintf(stderr, "Failed to open '%s'\n", fname);
		return False;
	}

	char currentDisplay[50];
	char currentExec[50];
	while(fgets(currentDisplay, 50, fp) && fgets(currentExec, 50, fp))
	{
		m = (MenuEntry*)malloc(sizeof(MenuEntry));

		/* Todo - check for error */
		m->next = NULL;

		/* copy both strings into the struct */
		strcpy(m->displayString, currentDisplay);
		strcpy(m->execString, currentExec);

		m = m->next; // move toward the next string
	}
	
	if(fp) fclose(fp);
	return True;
}

Bool freeMenuList(void)
{
	MenuEntry *cursor, *temp;
	
	if(menuHead == NULL) return False;

	cursor = menuHead->next;
	menuHead->next = NULL;
	while(cursor != NULL)
	{
		temp = cursor->next;
		free(cursor);
		cursor = temp;
	}

	return True;
}

void printMenuList(void)
{
	MenuEntry *temp = menuHead;
	int entry_count = 0;
	while(temp != NULL)
	{
		printf("Entry %d: %s, %s\n", entry_count++, temp->displayString, temp->execString);
		temp = temp->next;
	}
}

Bool addMenuEntry(const char *dispstring, const char *execstring)
{
	MenuEntry *m; // the new entry

	/* allocate memory for the head or new string if necessary */
	if(menuHead == NULL) {
		menuHead = (MenuEntry*)malloc(sizeof(MenuEntry));
		m = menuHead;
	}
	else {
		MenuEntry *temp = menuHead;
		while(temp->next != NULL){
			temp = temp->next;
		}

		m = (MenuEntry*)malloc(sizeof(MenuEntry));
		temp->next = m;
	}

	m->next = NULL;

	/* copy the strings into the entry */
	strcpy(m->displayString, dispstring);
	strcpy(m->execString, execstring);

	return True;
}

Bool fillMenuList(const char *fname)
{
	/* open the menu file */
	FILE *fp = fopen(fname, "r");
	if(!fp){
		fprintf(stderr, "Failed to open '%s'\n", fname);
		return False;
	}

	/* holds the next 2 lines from the file */
	char displaystring[50];
	char execstring[50];
	while(fgets(displaystring, 50, fp) && fgets(execstring, 50, fp))
	{
		/* remove the newline feeds from the strings */
		strtok(displaystring, "\n");
		strtok(execstring, "\n");

		/* add them as a new entry in the list */
		addMenuEntry(displaystring, execstring);
	}

	if(fp) fclose(fp);

	return True;
}



