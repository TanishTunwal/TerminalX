#include"Headers.h"

char user[MAX];
char dir[MAX];
char hostname[MAX];
char helpadd[MAX];//ccurrent directory to fetch help files from
char prompt[MAX];   
int args = 0;

//creates a colored prompt like tanish@ubuntu:/home/tanish/...
void getAddress()
{
    getcwd(dir, MAX);
    getlogin_r(user, MAX);
    gethostname(hostname, MAX);
    strcat(prompt, red);
    strcat(prompt, user);
    strcat(prompt, "@");
    strcat(prompt, hostname);
    strcat(prompt, ":");
    strcat(prompt, cyan);
    strcat(prompt, dir);
    strcat(prompt, "$");
    strcat(prompt, rmcolor);
}

//removes extra spaces from the start of input
char* lstrip(char* input)
{
    char *modInput = (char *)malloc(MAX * sizeof(char));
    bool isInitialSpace = 0;
    int j = 0;
    for (int i = 0; i < MAX; i++)
    {
        if (input[i] != ' ' && !isInitialSpace)
        {
            isInitialSpace = 1;
            modInput[j] = input[i];
            j++;
        }
        else if (isInitialSpace)
        {
            modInput[j] = input[i];
            j++;
        }
    }
    return modInput;

}

void Parse(char * input , char parsedInput[MAX][MAX])
{
    int z =0;//z is used to keep track of the position while adding chars to parsedInput[0]
    bool switc =0;
    bool command =1;
    int flag = 0;
    for(int i = 0 ; i < strlen(input); i++)
    {
        //check if its command
        if(command && input[i]!= ' ')
        {
            parsedInput[0][z] = input[i];
            z++;
        }  
        //end of command
        else if(input[i] == ' ')
        {
            command = 0;
            switc =1;
            z = 0;
        }
        //check if its flag
        else if(switc == 1 && input[i] == ' ')
        {
            continue;
        }
        //Parsing Flags (Options that start with -)
        else if(switc == 1 && input[i] == '-')
        {
            i++;
            while(input[i]!=' ' && i < strlen(input))
            {
                parsedInput[1][flag] = input[i];
                flag++;
                i++;
            }
        }
        //arguments (non-flag options)
        else if(switc == 1 && input[i] != '-')
        {
            while(input[i]!= ' ' && i < strlen(input))
            {
                parsedInput[args+2][z] = input[i];
                i++;
                z++;
            }
            parsedInput[args+2][z] = '\0';
            z=0;
            args++;
        }
    }

    lstrip(parsedInput[0]);//to remove any leading spaces in command
}


int main()
{
    memset(helpadd , '\0' , MAX);//clear help add
    getcwd(helpadd , MAX);//store current directory as help address
    while(1)
    {
        prompt[0] = '\0';
        getAddress();
        args = 0;
        char parsedInput[MAX][MAX];
        char input[MAX];
        //ensuring no leftover data from previous command
        for(int i = 0 ; i< MAX ;i++)
        {
            input[i] = '\0';
            for(int j = 0 ; j<MAX ;j++)
            {
                parsedInput[i][j] = '\0';
            }
            
        }
        strcpy(input , readline(prompt));
        strcpy(input , lstrip(input));
        Parse(input , parsedInput);
        getcwd(parsedInput[args+2] , MAX);
        if(strlen(input) != 0)
            add_history(input);//from GNU to access up down feature history
        if(!strcmp(parsedInput[0] , "chngdir"))
        {
            if(args ==0 && parsedInput[1][0] == '\0')chdir(getenv("HOME"));
            else  changeDirectoryEnhanced(input);
        }
        else if(!strcmp(parsedInput[0], "exit")) 
        {
            if(args> 0 || parsedInput[1][0] != '\0')
            {
                printf("Invalid syntax \nPlease check help exit\n");
                continue;
            }
            printf("Exiting...\n");
            break;
        }
        else if(!strcmp(parsedInput[0] , "list") ) 
        {
            list(dir , parsedInput , args);
        }
        else if(!strcmp(parsedInput[0] , "copy") || !strcmp(parsedInput[0] , "move"))
        {
            CpMv(parsedInput , args);
        }
        else if(!strcmp(parsedInput[0] , "process"))
        {
            if(args!= 0)
            {
                printf("Invalid Syntax for process\nPlease check help\n");
                continue;
            }
            myPS(parsedInput[1]);
        }
        else if(!strcmp(parsedInput[0] , "search"))
        {
            myGrep(parsedInput , args);
        }
        else if(!strcmp(parsedInput[0] , "help"))
        {
            if(parsedInput[1][0] != '\0')
            {
                printf("Please try help [command-name]\n");
                continue;
            }
            else if(args > 1)
            {
                printf("Please try help [command-name]\n");
                continue;
            }

            help(args , parsedInput , helpadd);
        }
        //to clear the terminal screen
        else if(!strcmp(parsedInput[0] , "clear"))
        {
            system("clear");
        }
        else printf("Invalid Command\n");
    }
    return 0;
}