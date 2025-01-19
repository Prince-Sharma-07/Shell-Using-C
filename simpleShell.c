#include<stdio.h>
#include<stdlib.h>
#define LSH_RL_BUFSIZE 1024

char *lsh_read_line(void)
{
/*------------------------------------Old Method (without getline)-------------------------------------*/
/*
    int bufsize = LSH_RL_BUFSIZE;
    int position = 0;
    char *buffer = malloc(sizeof(char) * bufsize);
    int c;
    
    if(!buffer){
        fprintf(stderr , "lsh: allocation error\n");
        exit(EXIT_FAILURE);     // EXIT_FAILURE = 1
    }

 while(1){
    Read a character for our line
        c = getchar();    
        
    If we hit EOF, replace it with a null character and return.
        if(c == EOF || c == '\n'){
            buffer[position] = '\0';
            return buffer;
        }
        else{
            buffer[position] = c;
        }
        position++;

    If we have exceeded the buffer, reallocate.
         if(position >= bufsize){

            bufsize += LSH_RL_BUFSIZE;
            buffer = realloc(buffer , bufsize);

            if(!buffer){
                fprintf(stderr , "lsh: allocation error\n");
                exit(EXIT_FAILURE);    
            }
         }
     }  
*/

/*---------------------------------New Method (with getline)---------------------------------------------*/

 char *line;                      // ssize_t for signed int and size_t for unsigned int. 
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

void lsh_loop(void){    // putting void in the parameter list of a function explicitly                      
    char *line;        //  means that the function does not take any parameters.    
    char **args;
    int status;

  do{
    printf("> ");
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