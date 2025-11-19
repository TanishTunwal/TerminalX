#include"Headers.h"
#include <limits.h>//limits for various types of data types

//PATH_MAX is defined in limits.h and represents the maximum length of a path on the system
static char previous_dir[PATH_MAX] = "";
static char dir_history[100][PATH_MAX];
static int history_count = 0;
static char bookmarks[20][2][PATH_MAX];
//bookmarks[i][0] -> name
//bookmarks[i][1] -> path
static int bookmark_count = 0;

//handle directory with ' or " format
//matching quotes
char* checkMultiWordFolder(char* input)
{
    strcpy(input , lstrip(input));
    int len = strlen(input);
    char *modinput = (char *)malloc(MAX * sizeof(char));
    if ((input[0] == '\'' && input[len - 1] == '\'') ||(input[0] == '\"' && input[len - 1] == '\"'))
    {
        //just copies from index 1 to len-2
        for (int i = 0; i < len - 2; i++)
        {
            modinput[i] = input[i + 1];
        }
        modinput[len - 2] = '\0';
        return modinput;
    }
    else
        return input;
}

//scroll feature implementation
// Helper functions for enhanced navigation
void update_directory_history(const char *dir) {
    // Add to history, avoid duplicates
    for (int i =    0; i < history_count; i++) {
        if (strcmp(dir_history[i], dir) == 0) {
            return; // Already in history
        }
    }
    //add to history
    if (history_count < 100) {
        strcpy(dir_history[history_count], dir);
        history_count++;
    } else {
        // Shift history and add new entry
        for (int i = 0; i < 99; i++) {
            strcpy(dir_history[i], dir_history[i + 1]);
        }
        strcpy(dir_history[99], dir);
    }
}

void show_recent_directories() {
    printf("\nRecent Directories:\n");
    printf("──────────────────────\n");
    
    if (history_count == 0) {
        printf("No recent directories found.\n");
        return;
    }
    
    // Show last 10 directories in reverse order (most recent first)
    int start = (history_count > 10) ? history_count - 10 : 0;
    
    for (int i = history_count - 1; i >= start; i--) {
        printf("%2d. %s\n", history_count - i, dir_history[i]);
    }
    
    printf("\nuse: chngdir - to go back to previous directory\n");
}

//this feature is for bookmarks
void save_bookmark(const char *name, const char *path) {
    // Check if bookmark already exists
    for (int i = 0; i < bookmark_count; i++) {
        if (strcmp(bookmarks[i][0], name) == 0) {
            strcpy(bookmarks[i][1], path);
            printf("Updated bookmark '%s' -> %s\n", name, path);
            return;
        }
    }
    
    // Add new bookmark
    if (bookmark_count < 20) {
        strcpy(bookmarks[bookmark_count][0], name);
        strcpy(bookmarks[bookmark_count][1], path);
        bookmark_count++;
        printf("Saved bookmark '%s' -> %s\n", name, path);
    } else {
        printf("Maximum bookmarks(20) reached remove some first.\n");
    }
}

void show_bookmarks() {
    printf("\nSaved Bookmarks:\n");
    printf("──────────────────────\n");
    
    if (bookmark_count == 0) {
        printf("No bookmarks saved.\n");
        printf("Use: chngdir --save <name> to save current directory\n");
        return;
    }
    
    for (int i = 0; i < bookmark_count; i++) {
        printf("%-15s -> %s\n", bookmarks[i][0], bookmarks[i][1]);
    }
    
    printf("\nUse: chngdir --go <name> to jump to bookmark\n");
}

void go_to_bookmark(const char *name) {
    for (int i = 0; i < bookmark_count; i++) {
        if (strcmp(bookmarks[i][0], name) == 0) {
            char current_dir[PATH_MAX];
            getcwd(current_dir, sizeof(current_dir));
            //change the current working dir of the program to a specified directory
            if (chdir(bookmarks[i][1]) == 0) {
                update_directory_history(current_dir);
                strcpy(previous_dir, current_dir);
                printf("Jumped to bookmark '%s': %s\n", name, bookmarks[i][1]);
            } else {
                printf("Bookmark path no longer exists: %s\n", bookmarks[i][1]);
            }
            return;
        }
    }
    printf("Bookmark '%s' not found.\n", name);
    show_bookmarks();
}

void changeDirectoryEnhanced(char *input) {
    char current_dir[PATH_MAX];
    getcwd(current_dir, sizeof(current_dir));

    // remove "chngdir" from the beginning and trim spaces
    char *args_part = input + 7; // skip "chngdir"
    while (*args_part == ' ') args_part++; // skip spaces
    
    // handle empty argument (go to home)
    if (strlen(args_part) == 0) {
        //get the value of env var HOME and change to that directory
        if (chdir(getenv("HOME")) == 0) {
            update_directory_history(current_dir);
            strcpy(previous_dir, current_dir);
            //your home directory is: /home/tanish
            printf("Changed to home directory: %s\n", getenv("HOME"));
        } else {
            perror("chngdir");
        }
        return;
    }
    
    // Handle special cases
    if (strcmp(args_part, "-") == 0) {
        if (strlen(previous_dir) == 0) {
            printf("chngdir: no previous directory\n");
            return;
        }
        //chngdir ..
        if (chdir(previous_dir) == 0) {
            printf("Back to: %s\n", previous_dir);
            char temp[PATH_MAX];
            strcpy(temp, previous_dir);
            strcpy(previous_dir, current_dir);
            update_directory_history(temp);
        } else {
            perror("chngdir");
        }
        return;
    }
    
    if (strcmp(args_part, "--recent") == 0) {
        show_recent_directories();
        return;
    }
    
    if (strcmp(args_part, "--list") == 0) {
        show_bookmarks();
        return;
    }
    
    if (strncmp(args_part, "--save ", 7) == 0) {
        char *bookmark_name = args_part + 7;
        while (*bookmark_name == ' ') bookmark_name++; // Trim spaces
        if (strlen(bookmark_name) > 0) {
            save_bookmark(bookmark_name, current_dir);
        } else {
            printf("chngdir: bookmark name required\n");
            printf("Usage: chngdir --save <bookmark_name>\n");
        }
        return;
    }
    
    if (strncmp(args_part, "--go ", 5) == 0) {
        char *bookmark_name = args_part + 5;
        while (*bookmark_name == ' ') bookmark_name++; // Trim spaces
        if (strlen(bookmark_name) > 0) {
            go_to_bookmark(bookmark_name);
        } else {
            show_bookmarks();
        }
        return;
    }
    
    if (strcmp(args_part, "--go") == 0) {
        show_bookmarks();
        return;
    }
    
    // Regular directory change
    char *target_dir = checkMultiWordFolder(args_part);
    if (chdir(target_dir) == 0) {
        update_directory_history(current_dir);
        strcpy(previous_dir, current_dir);
        char new_path[PATH_MAX];
        getcwd(new_path, sizeof(new_path));
        printf("Changed to: %s\n", new_path);
    } else {
        printf("Invalid Directory: %s\n", target_dir);
        printf("Tip: Use 'chngdir --recent' to see recent directories\n");
    }
}

void changeDirectory(char parsedInput[MAX][MAX] , int args , char input[MAX] )
{
    char current_dir[PATH_MAX];//4096
    getcwd(current_dir, sizeof(current_dir));
    
    if (args >= 1) {
        // Handle "chngdir -" (go back)
        if (strcmp(parsedInput[2], "-") == 0) {
            if (strlen(previous_dir) == 0) {
                printf("chngdir: no previous directory\n");
                return;
            }
            if (chdir(previous_dir) == 0) {
                printf("Back to: %s\n", previous_dir);
                char temp[PATH_MAX];
                strcpy(temp, previous_dir);
                strcpy(previous_dir, current_dir);
                update_directory_history(temp);
            } else {
                perror("chngdir");
            }
            return;
        }
        
        // Handle "--recent"
        if (strcmp(parsedInput[2], "--recent") == 0) {
            show_recent_directories();
            return;
        }
        
        // Handle "--save <name>"
        if (strcmp(parsedInput[2], "--save") == 0) {
            if (args < 2) {
                printf("chngdir: bookmark name required\n");
                printf("Usage: chngdir --save <bookmark_name>\n");
                return;
            }
            save_bookmark(parsedInput[3], current_dir);
            return;
        }
        
        // Handle "--go <name>" or "--go" (list bookmarks)
        if (strcmp(parsedInput[2], "--go") == 0) {
            if (args < 2) {
                show_bookmarks();
                return;
            }
            go_to_bookmark(parsedInput[3]);
            return;
        }
        
        // Handle "--list"
        if (strcmp(parsedInput[2], "--list") == 0) {
            show_bookmarks();
            return;
        }
    }
    
    // Regular directory change
    char newdir[MAX];
    memset(newdir , '\0' , MAX);
    for(int i = 0 ;  i< MAX- 8 ;i++)
    {
        newdir[i] = input[i+8];//skip chngdir and space
    }
    strcpy(newdir, checkMultiWordFolder(newdir));
    
    if(chdir(newdir) == 0) {
        update_directory_history(current_dir);
        strcpy(previous_dir, current_dir);
        char new_path[PATH_MAX];
        getcwd(new_path, sizeof(new_path));
        printf("Changed to: %s\n", new_path);
    } else {
        printf("Invalid Directory\n");
        printf("Tip: Use 'chngdir --recent' to see recent directories\n");
    }
}