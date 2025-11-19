#include"Headers.h"
#define len 1000

struct process
{
    // properly retrieved information.
    int pid;
    int ppid;
    int sid;
    char cmd[len];
    char cmd_full[len];
    char status;
    unsigned long long  int rss;
    long int sz;
    int uid;
    char username[MAX];
    int psr;            // at 39th field
    char tty_name[MAX];
    // related to time
    long long int utime; 
    long long int stime; // the one which is there in stat file
    long long cuttime;     
    long long int cstime;
    time_t  STIME;  // to be printed(after formatting)
    char formatted_time[MAX];
    char formatted_STIME[MAX];
    int c;

};

typedef struct process process;
bool checkIfValidProcessFolder(char * process_name)
{
    for(int i = 0 ; i< strlen(process_name) ; i++)
    {
        if(process_name[i]>57 || process_name[i]<48)
        {
            return 0;
        }
    }
    return 1;
}


void extract_from_stat(process *current , char* address)
{
    // gets pid ppid status rss correctly
    FILE* file = fopen(address , "r");
    if(file == NULL) return;
    fscanf(file , "%d %s %c %d %*d %d %*d %*d %*u %*d %*d %*d %*d %lld %lld %lld %lld %*d %*d %*d %*d %ld %*d %llu %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %d %d",
                   &current->pid , current->cmd , &current->status , &current->ppid,&current->sid
                    ,&current->utime, &current->stime  , &current->cuttime,&current->cstime,&current->STIME
                   , &current->rss , &current->psr , &current->c);
    current->c = current->stime/sysconf(_SC_CLK_TCK);
    //This retrieves the number of clock ticks per second on the system (a constant value such as 100, 250, or 1000 depending on the system).
    //ticks to seconds
    fclose(file);
    current->rss *= 4;
}

void extract_from_statm(process *current ,char* address)
{
    //gets sz correctly
    
    FILE* file = fopen(address , "r");
    if(file == NULL) return;
    fscanf(file, "%ld %llu", &current->sz  , &current->rss);
    current->rss*=4;
    fclose(file);
}

void extract_fullcmd(process *current , char* address)
{
    FILE* file = fopen(address , "r");
    if(file == NULL) return;
    fgets(current->cmd_full , 70 , file);
    fclose(file);
}

void get_username(process * current , char* address)
{
    // gets uid and username from status file
    FILE* file = fopen(address , "r");
    if(file == NULL) return;
    char line[MAX];
    while (fgets(line, sizeof(line), file)) {
        if (sscanf(line, "Uid:\t%d", &(current->uid)) == 1) {
            break;
        }
    }
    fclose(file);
    struct passwd* pw = malloc(sizeof(struct passwd));
    pw = getpwuid(current->uid);
    strcpy(current->username , pw->pw_name);
}

void get_tty(process* current , char* address)
{
    int fd = open(address , O_RDONLY ,0);
    if(ttyname(fd) != NULL) strcpy(current->tty_name ,ttyname(fd) );
    else strcpy(current->tty_name , "?");
    close(fd);
}

void getData(char* pidp , process* current)
{
    char address[300];
    //get data from stat
    sprintf(address , "/proc/%s/stat" , pidp);
    extract_from_stat(current , address);
    // get data from statm
    sprintf(address , "/proc/%s/statm" , pidp);
    extract_from_statm(current , address);
    // get data from cmdline
    sprintf(address , "/proc/%s/cmdline" , pidp);
    extract_fullcmd(current , address);
    //get data from status
    sprintf(address , "/proc/%s/status" , pidp);
    get_username(current , address);
    // get ttyname from fd
    sprintf(address, "/proc/%s/fd/0" , pidp);
    get_tty(current , address);
    // get uptime
    time_t uptime;
    FILE* file = fopen("/proc/uptime" , "r");
    if(file == NULL) return;
    fscanf(file , "%ld" , &uptime);
    fclose(file);
    current->STIME = (uptime - (current->STIME)/sysconf(_SC_CLK_TCK));
    // make time formatted
    long long int x = (long long int )60;
    long long int progtime = (current->utime  + current->stime)/sysconf(_SC_CLK_TCK);
    sprintf(current->formatted_time , "%.02lld:%.02lld:%.02lld" , (progtime/(x*x))%(x*x), (progtime/x)%(x) ,progtime%x);

    // format cmd
    char new[len];
    memset(new ,'\0' , MAX);
    strcpy(new, current->cmd);
    for(int i = 0 ; i< strlen(new);i++)
    {
        if(i < strlen(new) -2)
        current->cmd[i] = new[i+1];
        else current->cmd[i] = '\0';
    }
    
    //format tty_name
    if(current->tty_name[0] != '?')
    {
        memset(new ,'\0' , MAX);
        strcpy(new, current->tty_name);
        for(int i = 0 ; i< strlen(new);i++)
        {
            current->tty_name[i] = new[i+5];
        }
    }

    //get STIME & formatted_STIME
    time_t now = time(0);
    current->STIME = now -  current->STIME;
    struct tm* timer = localtime(&(current->STIME));
    strftime(current->formatted_STIME ,MAX, "%H:%M" ,timer );
}


void Print_format(bool f , bool F , process*current)
{
    if(F)
    {
        printf("%-13s  %6d  %6d  %2d  " , current->username , current->pid ,current->ppid , current->c);
        printf("%10ld  %10llu  %4d  " , current->sz , current->rss , current->psr);
        printf("%5s  %5s\t  %8s  %-s\n" , current->formatted_STIME , current->tty_name , current->formatted_time , current->cmd_full);
    }
    else if(f)
    {
        printf("%-13s  %6d  %6d  %2d  " , current->username , current->pid ,current->ppid , current->c);
        printf("%5s  %5s\t  %8s  %-s\n" , current->formatted_STIME , current->tty_name , current->formatted_time , current->cmd_full);
    }
    else
    {
        printf("   %6d\t%-5s\t%8s\t%-s\n" , current->pid , current->tty_name , current->formatted_time,current->cmd);
    }
}

void printColumn(bool f , bool F)
{
    if(F)
    {
        printf("%-13s  %6s  %6s  %2s  " , "UID" , "PID" , "PPID" , "C" );
        printf("%10s  %10s  %4s  " , "SZ" , "RSS" , "PSR");
        printf("%5s  %-5s\t  %8s  %-s\n" , "STIME" , "TTY" , "TIME" , "CMD");
    }
    else if(f)
    {
        printf("%-13s  %6s  %6s  %2s  " , "UID" , "PID" , "PPID" , "C" );
        printf("%5s  %-5s\t  %8s  %-s\n" , "STIME" , "TTY" , "TIME" , "CMD");
    }
    else
    {
        printf("   %6s\t%-5s\t%8s\t%-s\n" , "PID" , "TTY" , "TIME" , "CMD");
    }
}

void myPS(char flags[MAX])
{
    bool a = 0 ,f = 0 , F = 0 , e =0;
    for(int i = 0 ; i< strlen(flags) ; i++)
    {
        if(flags[i] == 'a' && !e)
        {
            a = 1;
        }
        else if(flags[i] == 'a' && e)
        {
            continue;
        }
        else if(flags[i] == 'e')
        {
            a = 0;
            e =1;
        }
        else if(flags[i] == 'f' && !F)
        {
            f = 1;
        }
        else if(flags[i] == 'f' && F)
        {
            continue;
        }
        else if(flags[i] == 'F')
        {
            F= 1;
             f= 0;
        }
        else 
        {
            printf("Invalid flag\n");
            return;
        }
    }
    printColumn(f , F);
    DIR* directory;
    directory = opendir("/proc");
    struct dirent* folder ;
    
    process current;
    int cpid = getpid();
    char current_pid[MAX];
    memset(current_pid , '\0' , MAX);
    sprintf(current_pid,"%d", cpid);
    getData(current_pid , &current);
    int size =0;
    while((folder = readdir(directory)) != NULL)
    {
        if(checkIfValidProcessFolder(folder->d_name))
        {
            process working;
            char moving_pid[MAX] ;
            strcpy(moving_pid , folder->d_name);
            getData(moving_pid ,&working);
            if(e) Print_format(f , F , &working);
            else if(a)
            {
                if((working.sid != working.pid) && strcmp(working.tty_name, "?") )
                       Print_format(f, F, &working);
            }
            else 
            {
                if(!strcmp(working.tty_name , current.tty_name)) Print_format(f, F, &working);
            }
        }
    }
    closedir(directory);
}

