#include"Headers.h"
#define line_size 256
#define reset "\033[0m" 
#define green "\033[1;32m" 
//-i = Ignore case (case-insensitive) may be in between
//  -n = Show line numbers]
// -c = Count only (don't show lines)
//-w = Whole word match

//checks if a pattern matches at a specific position in a line.
bool check(char* pattern, char* line, int start, bool I, bool w)
{
    //w is whole word match
    int pattern_len = strlen(pattern);
    int line_len = strlen(line);
    
    if (start + pattern_len > line_len) return 0;
    for(int i = 0; i < pattern_len; i++)
    {
        char pattern_char = pattern[i];
        char line_char = line[start + i];
        
        if(pattern_char == '.')
        {
            if(line_char == '\n' || line_char == '\0') return 0;
            continue;
        }
        
        if(I)
        {
            pattern_char = tolower(pattern_char);
            line_char = tolower(line_char);
        }
        
        if(pattern_char != line_char) return 0;
    }
    
    if(w)
    {
        int pattern_start = start;
        int pattern_end = start + pattern_len - 1;
        
        if(pattern_start > 0)
        {
            char prev_char = line[pattern_start - 1];
            if(isalnum(prev_char)) return 0;//types.h (return non zero value if letter)
        }
        
        if(pattern_end + 1 < line_len)
        {
            char next_char = line[pattern_end + 1];
            if(isalnum(next_char)) return 0;
        }
    }
    
    return 1;
}

//Removes quotes from quoted strings to handle patterns with spaces.
char* Unpack(char* input)
{
    int len = strlen(input);
    static char modinput[MAX];
    memset(modinput, 0, MAX);
    
    if ((input[0] == '\'' && input[len - 1] == '\'') || (input[0] == '\"' && input[len - 1] == '\"'))
    {
        strncpy(modinput, input + 1, len - 2);
        modinput[len - 2] = '\0';
        return modinput;
    }
    return input;
}


void Search(char* pattern, FILE* file, bool I, bool c, bool n, bool w, int args, char* filer)
{
    char line[line_size];
    int count_line = 0;
    int line_num = 1;
    
    while(fgets(line, line_size, file))
    {
        bool found = 0;
        int line_len = strlen(line);
        
        if(line_len > 0 && line[line_len - 1] == '\n')
        {
            line[line_len - 1] = '\0';
            line_len--;
        }
        
        int pattern_len = strlen(pattern);
        if(pattern_len > 0 && line_len >= pattern_len)
        {
            for(int i = 0; i <= line_len - pattern_len; i++)
            {
                if(check(pattern, line, i, I, w))
                {
                    found = 1;
                    count_line++;
                    break;
                }
            }
        }
        
        if(c)
        {
            line_num++;
            continue;
        }
        
        if(found)
        {
            if(args > 2) printf("%s%s%s:", green, filer, reset);
            if(n) printf("%s%d%s:", yellow, line_num, reset);
            
            int i = 0;
            int pattern_len = strlen(pattern);
            while(i < line_len)
            {
                if(i <= line_len - pattern_len && check(pattern, line, i, I, w))
                {
                    printf("%s", red);
                    for(int j = 0; j < pattern_len; j++)
                    {
                        printf("%c", line[i + j]);
                    }
                    printf("%s", reset);
                    i += pattern_len;
                }
                else
                {
                    printf("%c", line[i]);
                    i++;
                }
            }
            printf("\n");
        }
        line_num++;
    }
    
    if(c)
    {
        if(args == 2) printf("%d\n", count_line);
        else printf("%s%s%s:%d\n", green, filer, reset, count_line);
    }
}

    //myGrep() 
    // ↓
    // Parse flags and validate input
    // ↓
    // For each file:
    //     ↓
    //     Open file → Search() 
    //                   ↓
    //                   For each line:
    //                       ↓
    //                       check() → Pattern matching
    //                       ↓
    //                       Display results with colors
void myGrep(char parsedInput[MAX][MAX], int args)
{
    bool I = 0, c = 0, n = 0, w = 0;
    
    for(int i = 0; i < strlen(parsedInput[1]); i++)
    {
        if(parsedInput[1][i] == 'i') I = 1;
        else if(parsedInput[1][i] == 'c') c = 1;
        else if(parsedInput[1][i] == 'w') w = 1;
        else if(parsedInput[1][i] == 'n') n = 1;
        else 
        {
            printf("Invalid flag -%c\n", parsedInput[1][i]);
            return;
        }
    }
    
    if(args < 2) 
    {
        printf("Usage: search [options] pattern [files...]\n");
        return;
    }
    
    char pattern[MAX];
    strcpy(pattern, parsedInput[2]);
    strcpy(pattern, Unpack(pattern));
    
    char curdir[MAX];
    getcwd(curdir, MAX);
    
    for(int i = 3; i < args + 2; i++)
    {
        char filer[MAX];
        strcpy(filer, parsedInput[i]);
        
        split_string searched;
        memset(&searched, 0, sizeof(searched));
        splitter(&searched, filer);
        
        FILE* file;
        if(searched.dir_name[0] == '\0')
        {
            file = fopen(searched.file_name, "r");
        }
        else 
        {
            if(chdir(searched.dir_name) != 0)
            {
                printf("Cannot access directory: %s\n", searched.dir_name);
                continue;
            }
            file = fopen(searched.file_name, "r");
        }
        
        if(file == NULL)
        {
            printf("Cannot open file: %s\n", searched.file_name);
            chdir(curdir);
            continue;
        }
        
        Search(pattern, file, I, c, n, w, args, filer);
        fclose(file);
        chdir(curdir);
    }
}