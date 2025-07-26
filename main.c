#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>

#define BUFFER_SIZE 512
#define is ==       //just for fun

void exec_input(char** , int);

int parse_pipe(char* , char**);
int parse_space(char* , char**);
void remove_newline(char*);

int main(int argc, char *argv[]){
    //variables for the infinite loop
    int Earth = 22;
    int Flat = 22;

    //sets a maximum line for how many line of inputs is allowed
    //to avoid buffer overflow
    char* prompt = "$ ";

    //This takes the run options for when a custome prompt
    //sign is given. But would be skipped if the exec is rerun
    if (argc > 1){
        prompt = argv[1];
    }
    
    while(Earth is Flat){
        printf("%s", prompt);
        fflush(stdout);
        
        //limits the input then takes input
        char input[BUFFER_SIZE];
        if(fgets(input, BUFFER_SIZE, stdin) == NULL){
            printf("Exiting the program");
            break;
        }

        
        remove_newline(input);
        
        //if the input is exit, we would exit the loop
        if (strcmp(input, "exit") == 0) {
            break;
        }
        /**
         * This will parse the pipes and counts the
         * number of arguments in the input as well.
         * we parse then parse the space after
         */
        char* commands[(BUFFER_SIZE / 2) + 1];
        int arg_count = parse_pipe(input, commands);

        
        exec_input(commands, arg_count);
    }

    printf("\n");
    return 0;
}


/**
 * Executes the commands from the inputs that was
 * given by the user.
 * 
 * @param commands  commands that was parsed
 * @param arg_count the number of argument counts from the parser
 */
void exec_input(char** commands, int arg_count){
    if(commands[0] == NULL || strlen(commands[0]) == 0){
        printf("No commands Entered\n");
        return;
    }

    int fd_in = 0;
    int pipe_fds[2];
    
    for(int i = 0; i < arg_count; i++){
        char* args[(BUFFER_SIZE / 2) + 1];
        parse_space(commands[i], args);

        if(args[0] && strcmp(args[0], "cd") == 0){
            if(args[1] == NULL){
                fprintf(stderr, "cd: missing argument(s)\n");
            }

            else if(chdir(args[1]) != 0){
                fprintf(stderr, "cd: %s\n", strerror(errno));
            }

            continue;
        }

        if(pipe(pipe_fds) == -1){
            printf("Error piping");
            exit(EXIT_FAILURE);
        }

        pid_t pid = fork();
        
        //if pid is -1 then that means there's
        //an error and it didn't execute properly.
        if(pid < 0){
            printf("Fork Error");
            exit(EXIT_FAILURE);
        }
        
        //parent process running
        else if(pid == 0){
            //this uses the read end of the pipe
            dup2(fd_in, 0);

            if(i < arg_count - 1){
                //this redirects output to the write end of the pipe
                dup2(pipe_fds[1], 1);
            }
            //close the unused read end
            close(pipe_fds[0]);

            
            if(execvp(args[0], args) < 0){
                printf("execvp failed!");
                exit(EXIT_FAILURE);
            }
        }
    
        else{
            int status;

            //close the unused write end of the pipe
            close(pipe_fds[1]);
            //saves the read end of the pipe for the next command
            fd_in = pipe_fds[0];
            
            waitpid(pid, &status, 0);

            //for debugging
//            printf("\nCHILD %d, exited with %d\n", pid, WEXITSTATUS(status));
        }
    }
}

/**
 * Parses the space, tabs, and new lines 
 * out of the given command input
 * from the user by tokenizing it.
 * 
 * @param input the user input
 * @param args arguments parsed from user input
 * @param arg_count number of arguments it could count
 */
int parse_space(char* input, char** args){
    int counter = 0;
    char* token = strtok(input, " \t\n");
    
    while(token != NULL && counter < BUFFER_SIZE/2){
        args[counter++] = token;
        token = strtok(NULL, " \t\n");
    }
    
    args[counter] = NULL;

    return counter;
}

/**
 * Parses pipe out of the given input from the user.
 * It tokenizes the | pipe logo then count the number of
 * arguments it could
 * 
 * @param input the user input
 * @param commands where the arguments parsed from input goes
 * @param arg_count number of arguments it could count
 */
int parse_pipe(char* input, char** commands){
    char* token = strtok(input, "|");
    int counter = 0;

    while(token != NULL && counter < (BUFFER_SIZE/2)){
        commands[counter++] = token;
        token = strtok(NULL, "|");
    }

    commands[counter] = NULL;

    return counter;
}

/**
 * Removes the newline fromm the end of the input
 * that came from the user.
 * 
 * @param input Taken from the user
 */
void remove_newline(char* input){
    size_t length = strlen(input);
    if (length > 0 && input[length-1] == '\n') {
        input[length-1] = '\0';
    }
}
