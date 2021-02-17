//------------------------------------------------------------------------
// NAME: Martin Hristev
// CLASS: XIv
// NUMBER: 13
// PROBLEM: #2
// FILE NAME: shell.c
// FILE PURPOSE:
// да се реализира прост команден интерпретатор Shell
// 
//------------------------------------------------------------------------

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#define BUF_SIZE 1024


//------------------------------------------------------------------------
// FUNCTION: parse_cmdline
// It takes c string and turns it into 2d array with c strings 
// [0] = "example\0"
// [1] = "example2\0"
// PARAMETERS:
// takes const c string
// returns array with c strings
//------------------------------------------------------------------------

char** parse_cmdline(const char* cmdline)
{
  int buf_size = BUF_SIZE; 
  char **commands = malloc(BUF_SIZE * sizeof(char*)); // preparing the array with the c strings

    if(!commands){ // if malloc is not successful
      perror("malloc");
    }

  
  char string[sysconf(_SC_ARG_MAX)]; //initialise string with ARG_MAX (maximum length of arguments for a new process) size

  memcpy(string, cmdline, strlen(cmdline)+1);

  char *token; // this will get every single command divided with " "    
  
  int i = 0; // prep for counting
    

  token = strtok(string, " "); // strtok takes the "string" and read chars till finds " ", token is equal to those chars before " "

  while (token != NULL) // till we cant find new words, strtok returns NULL if there are no new tokens left
  {

    if(i == buf_size) // if the buf is full we need to realloc more memory to handle more and more commands
    {
      buf_size *= 2; // makes the buffer bigger
      commands = realloc(commands, (buf_size*sizeof(char*))); // realloc memory with the bigger buffer
      if(!commands) // check if the realloc is successful
      {
        perror("realloc");
      }
    }
    
    commands[i] = token; // push every single token found in the cmdline 
    i++;
    token = strtok(NULL, " "); // NULL means that we stay in the same string but now we tell the compiler to find the next token
  }

  commands[i] = NULL; // prepare for execvp which needs NULL as last argument

  free(token); // free the alocated memory for the tokens
    
  return commands; // returns all the commands with NULL in the end  
}

//------------------------------------------------------------------------
// FUNCTION: execute
// It takes array of c strings (commands) and executes them with execvp()  
// [0] = "/bin/ls\0"
// [1] = "-l\0"
// PARAMETERS:
// takes array of c strings
// returns 1 for successful execution
// returns 0 for execvp() error
//------------------------------------------------------------------------

int execute(char **commands) {
  pid_t pid;
  int status;

  pid = fork(); // trying to make child process

  if (pid == 0) { // check if the fork is successful
    if ((execv(commands[0], commands)) == -1)
    {
      perror(commands[0]);
      return 0;
    }
  } 
  if (pid < 0) // fork is not successful
  { 
    perror("fork");
  } else { // if it's successful
    waitpid(pid, &status, WUNTRACED); //tell the parent process to wait the child process to terminate (finish)
    // WUNTRACED - return if the child process has stoped fully, we get special status for the traced children which have stopped 
    // WNOHANG check for termination without waiting the child
  }

  return 1;

}

//------------------------------------------------------------------------
// FUNCTION: read_input
// reading from stdin
// 
// PARAMETERS:
// none
// returns c string (terminated with '\0')
//------------------------------------------------------------------------

char *read_input() {
  int buff_size = BUF_SIZE;
  int i = 0;
  char *buf = malloc(sizeof(char) * buff_size); // initialise start buffer
  char c;

  if(!buf)
  { //check if the malloc was successful
    perror("malloc");
  }

  char p_buf[2] = "$ ";
  int write_res = write(STDOUT_FILENO, p_buf, 2);
  if(write_res == -1)
  {
    perror("write");
  }

  while (1) //reading from standart input
  {
    c = getchar(); 
    if (c == '\n' || c == EOF) // check if the "enter"('\n') was pressed 
    {
      buf[i] = '\0'; // we need to terminate the c string before returning it to main()
      if(c == EOF) // check if (ctr+d) and shut down the program
      {
        free(buf); // we need to free before shuting down the program
        exit(0); // exit the program fully
      }
      return buf; 
    }
    else 
    {
      buf[i] = c; // we are still reading information from stdin
    }
    i++;

    if(i == buff_size) // check if the buffer is full
    {
      buff_size *= 2;
      buf = realloc(buf, buff_size);

      if(!buf) // check if realloc was not successful
      {
        perror("realloc");
      }
    }
  }
}

int main()
{         
    char* input;
    char **commands; // array with c strings
    int status; // status for the execute
    while(1){
      input = read_input();
      commands = parse_cmdline(input);
      status = execute(commands); // if status = 1 - there're no errors
      free(input); // free to begin the next read_input()
      free(commands);
      if(status == 0){ // execvp() error found! we need to stop the program now
        return 0;
      }
    }    

    return 0;
}