#include"Headers.h"

//all the function have a task to read a particular help file and print it on terminal
void chnhelp(char * helpadd)
{
    char addresshelp[MAX + 25];
    //addresshelp is destination
    memset(addresshelp , '\0' , MAX + 25);//function to set a block of memory to a specific value.
    sprintf(addresshelp , "%s/help/chngdir.txt" , helpadd);//format data and store in string
    FILE* file = fopen(addresshelp , "r");
    char line[1000];
    memset(line , '\0' , 1000);
    while(fgets(line , 1000 ,file))
    {
        printf("%s" , line);
        memset(line , '\0' , 1000);
    }
    fclose(file);
}

void listhelp(char * helpadd)
{
    char addresshelp[MAX + 25];
    memset(addresshelp , '\0' , MAX + 25);
    sprintf(addresshelp , "%s/help/list.txt" , helpadd);
    FILE* file = fopen(addresshelp, "r");
    char line[1000];
    memset(line , '\0' , 1000);
    while(fgets(line , 1000 ,file))
    {
        printf("%s" , line);
        memset(line , '\0' , 1000);
    }
    fclose(file);
}

void copyhelp(char * helpadd)
{
    char addresshelp[MAX + 25];
    memset(addresshelp , '\0' , MAX + 25);
    sprintf(addresshelp , "%s/help/copy.txt" , helpadd);
    FILE* file = fopen(addresshelp, "r");
    char line[1000];
    memset(line , '\0' , 1000);
    while(fgets(line , 1000 ,file))
    {
        printf("%s" , line);
        memset(line , '\0' , 1000);
    }
    fclose(file);
}

void movehelp(char * helpadd)
{
    char addresshelp[MAX + 25];
    memset(addresshelp , '\0' , MAX + 25);
    sprintf(addresshelp , "%s/help/move.txt" , helpadd);
    FILE* file = fopen(addresshelp, "r");
    char line[1000];
    memset(line , '\0' , 1000);
    while(fgets(line , 1000 ,file))
    {
        printf("%s" , line);
        memset(line , '\0' , 1000);
    }
    fclose(file);
}

void exithelp(char * helpadd)
{
    char addresshelp[MAX + 25];
    memset(addresshelp , '\0' , MAX + 25);
    sprintf(addresshelp , "%s/help/exit.txt" , helpadd);
    FILE* file = fopen(addresshelp, "r");
    char line[1000];
    memset(line , '\0' , 1000);
    while(fgets(line , 1000 ,file))
    {
        printf("%s" , line);
        memset(line , '\0' , 1000);
    }
    fclose(file);
}

void searchelp(char * helpadd)
{
    char addresshelp[MAX + 25];
    memset(addresshelp , '\0' , MAX + 25);
    sprintf(addresshelp , "%s/help/search.txt" , helpadd);
    FILE* file = fopen(addresshelp, "r");
    char line[1000];
    memset(line , '\0' , 1000);
    while(fgets(line , 1000 ,file))
    {
        printf("%s" , line);
        memset(line , '\0' , 1000);
    }
    fclose(file);
}

void processhelp(char * helpadd)
{
    char addresshelp[MAX + 25];
    memset(addresshelp , '\0' , MAX + 25);
    sprintf(addresshelp , "%s/help/process.txt" , helpadd);
    FILE* file = fopen(addresshelp, "r");
    char line[1000];
    memset(line , '\0' , 1000);
    while(fgets(line , 1000 ,file))
    {
        printf("%s" , line);
        memset(line , '\0' , 1000);
    }
    fclose(file);
}

void help(int args , char parsedInput[MAX][MAX] , char* helpadd)
{
    if(args == 0)
    {
        printf(" -> Type help list for more info on list\n"
                "-> Type help copy for more info on copy\n"
                "-> Type help move for more info on move\n"
                "-> Type help process for more info on process\n"
                "-> Type help search for more info on search\n"
                "-> Type help exit for more info on exit\n"
                "-> Type help chngdir for more info on chngdir\n");
        printf("\n");
    }
    else if(!strcmp(parsedInput[2] , "chngdir")) chnhelp(helpadd);
    else if(!strcmp(parsedInput[2] , "list")) listhelp(helpadd);
    else if(!strcmp(parsedInput[2] , "copy")) copyhelp(helpadd); 
    else if(!strcmp(parsedInput[2] , "move")) movehelp(helpadd);
    else if(!strcmp(parsedInput[2] , "exit")) exithelp(helpadd);
    else if(!strcmp(parsedInput[2] , "process")) processhelp(helpadd);
    else if(!strcmp(parsedInput[2] , "search")) searchelp(helpadd);
    else 
    {
        printf("Type a valid command\n");
    }
}
    