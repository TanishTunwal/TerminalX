#include"Headers.h"

long  size = 0;
bool l = 0 , S = 0 ,r = 0 , a = 0 , t =0;
char format_time[MAX];

struct data
{
    char name[MAX];
    struct stat stats;
};

struct data files[1000];

int sortBySize(const void*file1 , const void* file2)
{
    long long int x1 = ((struct data*)file1)->stats.st_size;
    long long int x2 = ((struct data*)file2)->stats.st_size;
    return (x2 - x1);
}

int sortByName(const void *file1 , const  void* file2)
{
    char name1[MAX];
    strcpy(name1 , ((struct data*)file1)->name);
    char name2[MAX];
    strcpy(name2, ((struct data*)file2)->name);
    return strcmp(name1 , name2);
}

int sortByTime( const void* file1 , const void* file2)
{
    long long int x1 = ((struct data*)file1)->stats.st_mtime;
    long long int x2 = ((struct data*)file2)->stats.st_mtime;
    return (x2 - x1);
}


void Format_Time(long long int mod)
{
    time_t rawtime = mod;
    struct tm *info;
    info = localtime( &rawtime );

    strftime(format_time,MAX,"%b %d %H:%M", info);
  
}

void printPermissions(struct data file)
{
    printf( (S_ISDIR(file.stats.st_mode)) ? "d" : "-");
    printf( (file.stats.st_mode & S_IRUSR) ? "r" : "-");
    printf( (file.stats.st_mode & S_IWUSR) ? "w" : "-");
    printf( (file.stats.st_mode & S_IXUSR) ? "x" : "-");
    printf( (file.stats.st_mode & S_IRGRP) ? "r" : "-");
    printf( (file.stats.st_mode & S_IWGRP) ? "w" : "-");
    printf( (file.stats.st_mode & S_IXGRP) ? "x" : "-");
    printf( (file.stats.st_mode & S_IROTH) ? "r" : "-");
    printf( (file.stats.st_mode & S_IWOTH) ? "w" : "-");
    printf( (file.stats.st_mode & S_IXOTH) ? "x" : "-");
    printf(" ");
}


void print(struct data file)
{
    printPermissions(file);
    struct passwd *pw = (struct passwd*) malloc(sizeof(struct passwd));
    pw = getpwuid(file.stats.st_uid);
    struct group  *gr = (struct group*) malloc(sizeof(struct group));
    gr = getgrgid(file.stats.st_gid);//#include<pwd.h>   // ← This header provides struct passwd
    Format_Time(file.stats.st_mtime);
    printf("%4ld %15s %15s " , file.stats.st_nlink , pw->pw_name , gr->gr_name);
    printf("%8ld %16s\t%s \n" , file.stats.st_size , format_time ,file.name);
}

// when -l is present
void listWithL()
{
    if(r)
    {
        for(int i = size- 1; i>=0 ;i--)
        {
            if(a) print(files[i]);
            else if(files[i].name[0] != '.') print(files[i]);
        }
    }
    else
    {
        for(int i = 0; i< size ;i++)
        {
            if(a) print(files[i]);
            else if(files[i].name[0] != '.') print(files[i]);
        }
    }
}

int list(char* dir , char parsedInput[MAX][MAX] , int args)
{

    // checking flags
    l = 0;r = 0; a =0;S= 0;t =0;
    char sortflag[2] = {'\0' , '\0'};
    for(int i = 0 ;  i<  strlen(parsedInput[1]) ; i++)
    {
        if(parsedInput[1][i] == 'l') l =1;
        else if(parsedInput[1][i] == 'r') r =1;
        else if(parsedInput[1][i] == 'a') a =1;
        else if(parsedInput[1][i] == 't' || parsedInput[1][i] == 'S')
        {
            if(parsedInput[1][i] == 'S')
            {
                S =1;
            }
            else t =1;
            sortflag[0] = parsedInput[1][i];
        }
        else 
        {
            printf("Invalid Flag -%c with list.\nTry 'help'\n" , parsedInput[1][i]);
            return 0;
        }
    }

    // printing for all directories or only 1 if not mentioned
    for(int p =2 ; p < (args ==0 ?args + 3: args + 2 );p++)
    {
        size = 0;
        long long int total_size = 0 ,hidden_size =0;
        DIR* directory;
        char newDir[MAX];
        if(args > 0)
        {
            if(chdir(parsedInput[p]) == -1)
            {
                printf("\nInvalid Directory\n");
                continue;
            }
            getcwd(newDir, MAX);
            if(args>1)
            printf("\n%s:\n" , parsedInput[p]);
        }
        else strcpy(newDir , dir);
        directory = opendir(newDir);
        struct dirent*file = (struct dirent*)malloc(sizeof(struct dirent));
        while((file = readdir(directory)) != NULL)
        {
            stat(file->d_name , &files[size].stats);
            strcpy(files[size].name, file->d_name);
            total_size+=(files[size].stats.st_blocks);
            if(file->d_name[0] == '.') hidden_size+=(files[size].stats.st_blocks);
            size++;
        }
        closedir(directory);
        free(file);

        
        // sortings(sorted by last flag if both t and S are present)stdlib.h
        qsort(files , size , sizeof(files[0]) , sortByName);
        if(S||t)
        {
            if(sortflag[0] == 'S')qsort(files , size , sizeof(files[0]) , sortBySize);
            else qsort(files , size , sizeof(files[0]) , sortByTime);
        }
        
        // implementation of printing
        if(l)  
        {
            if(a) printf("total %lld\n" , total_size/2);
            else printf("total %lld\n" , (total_size - hidden_size)/2);
            listWithL();
        }
        else
        {
            if(r)   
            {
                for(int i = size-1 ; i>=0 ;i--) // for reversal
                {
                    if(a == 1) printf("%s\n" , files[i].name);
                    else if(files[i].name[0] != '.') printf("%s\n" , files[i].name);  
                }
            }
            else 
            {
                for(int i = 0 ; i< size ;i++) // for no reversal
                {
                    if(a == 1) printf("%s\n" , files[i].name);
                    else if(files[i].name[0] != '.') printf("%s\n" , files[i].name);
                }
            }
        }
        chdir(dir);
    }    
}
