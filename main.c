#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h> // Include for wait() function

#define MAX_SIZE 20
#define delim "\t \r\n"


#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_WHITE   "\x1b[37m"
#define ANSI_COLOR_RESET   "\x1b[0m"



int Exit(char **args);
int history(char **args);
int cd(char **args);
int export(char **args);
int unset(char **args);



char *cp = NULL;


char *builtin_commands[] = {
    "exit",
    "history",
    "cd",
    "export",
    "unset",
    // built-in commands here
};

int (*builtin_functions[]) (char **) = {
    &Exit,
    &history,
    &cd,
    &export,
    &unset
    // Add function pointers for  built-in 
};

int num_builtin_commands() {
    return sizeof(builtin_commands) / sizeof(char *);
}





char *read_line(void)
{
    char *line = NULL;
    size_t size;
    getline(&line, &size, stdin);
    return line;
}

int Exit(char **args)
{
       printf("exiting ....");
       exit(0);
}




void saveToHistory(char *command){
    FILE *his = fopen("history.txt","a+");
    int len = strlen(command);

    if(len>0 && command[len-1]=='\n'){
       command[len-1]='\0'; 
    }

    fputc('\n',his);
    fputs(command,his);
    fclose(his);
}


int unset(char **args){
    if(args[1]==NULL){
     printf("unset is missing variable name [unset VAR_NAME] ");
     return -1;
   }
     if (unsetenv(args[1]) != 0) {
        perror("unsetenv");
        return -1; // Return an error code if unsetenv fails
    }
    else{
        return 0;
    }

}

int history(char **args){

    FILE *file = fopen("history.txt","r");

     if (fseek(file, 0, SEEK_SET) != 0) {
        perror("fseek failed");
        return -1;
    }

   int ch = fgetc(file);
   int line_number = 0;

   while(ch!=EOF){

    putchar(ch);

    if(ch=='\n'){
    line_number++;
    printf("%d- ",line_number);
    }

    ch = fgetc(file);

   }

   printf("\n");

   fclose(file);

   return 0;
}



int export(char **args){
   if(args[1]==NULL){
   printf("export is missing variable name and value [export VAR_NAME=VAR_VALUE] ");
     return -1;
   }
   char *var_name = strtok(args[1],"=");
   char *var_value = strtok(NULL,"=");
   if (setenv(var_name, var_value, 1) != 0) {
        perror("setenv");
        return -1; // Return an error code if setenv fails
    }
    else{
        return 0;
    }
    
  }

char *cwd(){
    char *currentPath =(char *) malloc(sizeof(char)*1024);
    getcwd(currentPath, sizeof(char)*1024);
    return currentPath;        
}


int cd(char **args){
     if(args[1]==NULL){
        printf("cd is missing the target directory --> cd [target]");
        return -1;
     }
     if(chdir(args[1])!=0){
        perror("chdir");
        return -1;
     }
     else{
        cp = cwd();
        return 0;
     }
}
 

int execute_command(char *command)
{
    char *args[MAX_SIZE];
    int id = fork();
    char *token = strtok(command, delim);
    size_t i = 0;

    if (id < 0)
    {
        printf("\nan error occured during the system call process!\n");
        perror("fork");
    }

    if (id == 0)
    {

        while (token != NULL)
        {

            if (i > MAX_SIZE - 1)
            {
                printf("\n the command is too long !\n");
                return -1;
            }
            args[i] = token;
            token = strtok(NULL, delim);
            i++;
        }

        args[i] = NULL;

        for (int i = 0; i < num_builtin_commands(); i++) {
        if (strcmp(args[0], builtin_commands[i]) == 0) {
            return (*builtin_functions[i])(args);
         }
        }

        if (execvp(args[0], args) == -1)
        {
            perror("execvp"); // Print error message if execvp fails
        }
    }

    else
    {
        wait(NULL);
        if(strcmp(args[0],"exit")==0){
            exit(0);
        }
        return 0;
    }
}



int main()
{
    char *command;
    cp = cwd(); 

    while (1)
    {
        printf(ANSI_COLOR_CYAN"\n%s>"ANSI_COLOR_WHITE,cp);
        command = read_line();
        saveToHistory(command);
        execute_command(command);
        free(command);
    }

    return 0;
}








