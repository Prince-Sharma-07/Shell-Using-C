#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include<unistd.h>
#include <sys/types.h>
#include<sys/wait.h>

#define LSH_RL_BUFSIZE 1024
#define LSH_TOK_BUFSIZE 64
#define LSH_TOK_DELIM " \t\r\n\a"

//step 1: Reading a line
char *lsh_read_line(void)
{
/*------------------------------------Old Method (without getline) (Prefered)-------------------------------------*/

    int bufsize = LSH_RL_BUFSIZE;
    int position = 0;
    char *buffer = malloc(sizeof(char) * bufsize);
    int c;
    
    if(!buffer){
        fprintf(stderr , "lsh: allocation error\n");
        exit(EXIT_FAILURE);     // EXIT_FAILURE = 1
    }

 while(1){
    // Read a character for our line
        c = getchar();    
        
    //If we hit EOF, replace it with a null character and return.
        if(c == EOF || c == '\n'){
            buffer[position] = '\0';
            return buffer;
        }
        else{
            buffer[position] = c;
        }
        position++;

   // If we have exceeded the buffer, reallocate.
         if(position >= bufsize){

            bufsize += LSH_RL_BUFSIZE;
            buffer = realloc(buffer , bufsize);

            if(!buffer){
                fprintf(stderr , "lsh: allocation error\n");
                exit(EXIT_FAILURE);    
            }
         }
     }  
}

/*---------------------------------New Method (with getline)---------------------------------------------*/

/* char *line;                      // ssize_t for signed int and size_t for unsigned int. 
 ssize_t bufsize = 0;            // We've taken bufsize as signed as some errors can be negetive.   
                                      
if(getline(&line , &bufsize , stdin) == -1){
    if(feof(stdin)){
        exit(EXIT_SUCCESS);
    }
    else{
        perror("readline");
        exit(EXIT_FAILURE);
    }
}

 return line;
}
*/

// step 2: Parsing a line
char** lsh_split_line(char* line)
{
   int bufsize = LSH_TOK_BUFSIZE , position = 0;
   char **tokens = malloc(bufsize * sizeof(char*));
   char *token;

   if(!tokens){
    fprintf(stderr , "lsh: allocation error\n");
    exit(EXIT_FAILURE);
   }

   token = strtok(line , LSH_TOK_DELIM);
   
   while(token != NULL){
     tokens[position] = token;
     position++;

     if(position >= bufsize){
        bufsize += LSH_TOK_BUFSIZE;
        tokens = realloc(tokens , bufsize * sizeof(char*));
        if(!tokens){
            fprintf(stderr , "lsh: allocation error\n");
            exit(EXIT_FAILURE);
        }
     }

     token = strtok(NULL , LSH_TOK_DELIM);
   }

   tokens[position] = NULL;
   return tokens;
}

int lsh_cd(char **args);
int lsh_help(char **args);
int lsh_exit(char **args);

char *builtin_str[] = {
    "cd",
    "help",
    "exit"
};

int (*builtin_func[]) (char **) = {
    &lsh_cd,
    &lsh_help,
    &lsh_exit
};

int lsh_num_builtins(){         
    return sizeof(builtin_str) / sizeof(char *);
}

// Implementation of builtin_funtions.

int lsh_cd(char **args){
    if(args[1] == NULL){
        fprintf(stderr, "\033[1;31m"); // Set text color to red
        fprintf(stderr , ":( ");
        fprintf(stderr, "lsh: Expected argument to \"cd\"\n");
        fprintf(stderr, "\033[0m");   // Reset text color
    }
    else{
        if(chdir(args[1]) != 0){
            fprintf(stderr, "\033[1;31m"); // Set text color to red
            fprintf(stderr , ":( ");
            fprintf(stderr , "%s\n" , strerror(errno));
            fprintf(stderr, "\033[0m");   // Reset text color
        }
    }
    return 1;
}

int lsh_help(char **args){
    int i;
    printf("Prince Sharma's LSH\n");
    printf("Type program names and argumnets, and hit enter.\n");
    printf("The following are built in:\n");

    for(i = 0; i<lsh_num_builtins(); i++){
        printf(" %s\n" , builtin_str[i]);
    }

    printf("Use the man command for information on other programs.\n");
    return 1;
}

int lsh_exit(char **args){
    return 0;
}

int lsh_launch(char **args){
    pid_t pid, wpid;
    int status;

    pid = fork();
    if(pid == 0){
        //Child Process
        if(execvp(args[0] , args) == -1){
            fprintf(stderr, "\033[1;31m"); // Set text color to red
            fprintf(stderr , ":( ");
            fprintf(stderr , "Lsh: %s\n" , strerror(errno));
            fprintf(stderr, "\033[0m");   // Reset text color
        }
        exit(EXIT_FAILURE);
    }
    else if(pid < 0){
        //Error Forking
        perror("lsh");
    }
    else{
        // Parent Process
        do {
            wpid = waitpid(pid , &status, WUNTRACED);
        }
        while(!WIFEXITED(status) && !WIFSIGNALED(status));
    }

    return 1;
}

//step 3: Execution and processing
int lsh_execute(char** args)
{
   int i;

   if(args[0] == NULL){
    //An empty command was entered.
    return 1;
   }

   for(i = 0; i < lsh_num_builtins(); i++){
     if(strcmp(args[0] , builtin_str[i]) == 0){
        return (*builtin_func[i])(args);
     }
   }

   return lsh_launch(args);
}


void lsh_loop(void){    // putting void in the parameter list of a function explicitly                      
    char *line;        //  means that the function does not take any parameters.    
    char **args;
    int status;

  do{
    printf(":) ");
    line = lsh_read_line();
    args = lsh_split_line(line);
    status = lsh_execute(args);

    free(line);
    free(args);
  } while (status);
}

int main(int argc , char **argv){    //char *argv[] and char **argv implies the same meaning.
     
    lsh_loop();

    return EXIT_SUCCESS;    // EXIT_SUCCESS = 1
}

