#include"Headers.h"

//Flags for copy and move command
char curdir[MAX];
bool I =0, n = 0 , u =0 , v =0;
//-i: interactive (asks before overwrite)

//-v: verbose (shows progress) like how it is done

//-u: update (only copy if source is newer)

//-n: don't overwrite existing files
char openc[MAX];
struct stat current_file_stats;//info about file

split_string destination , current;

//split the path into directory and file name
//name = "/home/user/file.txt"
// path->dir_name = "/home/user/"
// path->file_name = "file.txt"
void splitter(split_string* path , char* name)
{
    int x = strlen(name);
    for(int i = x- 1; i>= 0 ;i--)
    {
        if(name[i] == '/')
        {
            x = i;
            break;
        }
        else if(i == 0)
        { 
            x =-1;
        }
    }
    int o = 0 , p = 0;
    for(int i= 0 ; i < strlen(name) ;i++)
    {
        if(i<=x)
        {
            path->dir_name[o] = name[i];
            path->dir_name[o+1] = '\0';
            o++;
        }
        else
        {
            path->file_name[p] = name[i];
            path->file_name[p+1] = '\0';
            p++;
        }
    }
}


int checkExists(char  destination_name[MAX] , char file_name1[MAX]){
    DIR* directory;
    directory = opendir(destination_name); //
    //with this dirent we can read the contents of the directory(opendir, readdir etc work with dirent)
    struct dirent* file1 = (struct dirent*)malloc(sizeof(struct dirent));
    while((file1 = readdir(directory)) != NULL)
    {
        char newest[MAX];
        if(!strcmp(file1->d_name , file_name1))
        {
            return 1;
        }
    }
    free(file1);
    closedir(directory);

    return 0;
}

// check that should we skip copying?
bool latestChecker(struct stat a ,struct stat b)
{
    long long int time1 = a.st_mtime;
    long long int time2 = b.st_mtime;//gives modification time
    if(time1 > time2) return 0;//no skip copying
    else return 1;// skip copying
}   

//check and 
bool implementFlags(char file_name[MAX])
{
    getcwd(openc, MAX);
    bool checker = checkExists(openc , file_name);
    if(u && checker)
    {
        struct stat  atDestination;
        stat(file_name , &atDestination);
        bool latest = latestChecker(current_file_stats , atDestination);
        if(latest) 
        {
            chdir(curdir);
            return 0;
        }
    }

    if(I && checker)
    {
        printf("%s already exists at destination. Overwrite?(y/n)" , file_name);
        char c;
        
        scanf("%c" , &c);
        while(getchar()!= '\n');

        if(tolower(c) != 'y') 
        {
            chdir(curdir);
            return 0;
        }
    }
    else if(n && checker)
    {
        chdir(curdir);
        return 0;
    }
    
    strcpy(destination.file_name , file_name);
    

    return 1;
}

//main copy and move command function
void CpMv(char parsedInput[MAX][MAX] , int args)
{
    I =0, n = 0 , u =0 , v =0;
    for(int i =0 ; i < strlen(parsedInput[1]) ; i++)
    {
        if(parsedInput[1][i] == 'i' ) 
        {
            I= 1;n=0;
        }
        else if(parsedInput[1][i] == 'n') 
        {
            n =1;I= 0;
        }
        else if(parsedInput[1][i] == 'u') u =1;
        else if(parsedInput[1][i] == 'v') v = 1;
        else 
        {
            printf("Invalid flag\n");
            return;
        }
    }
    if(args < 2) 
    {
        printf("Invalid syntax for %s\nPlease check help.\n" , parsedInput[0]);
    }
    getcwd(curdir , MAX);
    for(int j = 0 ; j < MAX ; j++) 
    {
        openc[j] = '\0';
        current.file_name[j] = '\0';
        current.dir_name[j] = '\0';
        destination.dir_name[j] = '\0';
        destination.file_name[j] = '\0';
    } 
    FILE* new;
    destination.dir_name[0] = '\0';
    splitter(&destination, parsedInput[args+2 -1]);
    for(int i = 2 ;i < args + 1 ; i++)
    {
        splitter(&current, parsedInput[i]);
        long long int mode;
        FILE* this;
        //rb -> read binary
        //file can be anything like .txt , .c , .jpg etc so we read in binary mode
        if(strlen(current.dir_name) == 0) 
            this = fopen(current.file_name , "rb");
        else if(chdir(current.dir_name) != -1) 
            this = fopen(current.file_name , "rb");
        else 
        {
            printf("%s is invalid path for %s\n" , current.dir_name , current.file_name);
            continue;
        }
        chdir(curdir);
        if(this == NULL)
        {
            printf("Invalid Source file %s\n" , current.file_name);
            continue;
        }
        //retrieve information about a file or directory.(metadata)
        stat(current.file_name , &current_file_stats);
        // is copying the file type and permission information of a file
        mode = current_file_stats.st_mode;
        // checks if the program can change the cwd
        if(chdir(parsedInput[args + 2 -1]) != -1)
        {
            char dump[MAX];
            memset(dump , '\0' ,MAX);
            getcwd(dump , MAX);
            if(!implementFlags(current.file_name)) continue;//check
            if(strncmp(dump , getenv("HOME") , strlen(getenv("HOME"))))//compare number of characters of both strings
            {
                printf("No copy/move in this directory\n");
                chdir(curdir);
                break;
            }
            new = fopen(current.file_name,"wb");
        }
        //if can change the working dir to the dest dir
        else if(chdir(destination.dir_name) != -1)
        {    
            if(args > 2)
            {
                printf("Invalid destination Directory\n");
                chdir(curdir);
                break;
            }
            else  
            {
                char dump[MAX];
                memset(dump , '\0' ,MAX);
                getcwd(dump , MAX);
                
                if(!implementFlags(destination.file_name)) continue;
                if(strncmp(dump , getenv("HOME") , strlen(getenv("HOME"))))
                {
                    printf("No copy/move in this directory\n");
                    chdir(curdir);
                    break;
                }
                new = fopen(destination.file_name,"wb");
            }
            
        }
        else
        //handle case when dest dir is not given
        {
            if(destination.dir_name[0] == '\0')
            {
                if(args>2)
                {
                    printf("Invalid destination Directory\n");
                    chdir(curdir);
                    break;
                }
                else
                {
                    char dump[MAX];
                    memset(dump , '\0' ,MAX);
                    getcwd(dump , MAX);
                    if(!implementFlags(destination.file_name)) continue;
                    if(strncmp(dump , getenv("HOME") , strlen(getenv("HOME"))))
                    {
                        printf("No copy/move in this directory\n");
                        chdir(curdir);
                        break;
                    }
                    
                    new = fopen(destination.file_name,"wb");
                    strcpy(current.dir_name , curdir);
                }
            }
            else
            {
                printf("Destination Directory doesn't exist\n");
                chdir(curdir);
                break;
            }
        }
        if(new == NULL)
        {
            chdir(curdir);
            return;
        }
        chmod(destination.file_name , mode);

        char chunks[1];
        while(fread(chunks, 1 ,1, this) == 1)
        {
            fwrite(chunks, 1 ,1 ,new);
        }
        getcwd(destination.dir_name , MAX);
        chdir(curdir);
        chdir(current.dir_name);
        getcwd(current.dir_name, MAX);
        if(!strcmp(parsedInput[0], "move")) 
        {
            remove(current.file_name);
            if(v)
            printf("renamed ");
        }
        if(v)
        printf("\'%s/%s\' -> \'%s/%s\'\n" , current.dir_name , current.file_name , destination.dir_name, destination.file_name);
        fclose(this);
        fclose(new);
        chdir(curdir);
    }
}

