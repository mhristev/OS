#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>


int main(int argc, char *argv[]) {
  int child_status;
  char* name = argv[1];

    pid_t pid;

    while(1){
        if((pid =fork()) == 0){
            if((execvp(argv[1], argv+1)) < 0){
                perror(argv[1]);
                break;
            }
        }
        if(pid < 0){
            perror("fork");
        }

        if(pid > 0){
            waitpid(pid, &child_status, 0);
            if(child_status > 0){
                perror("error");
                return 1;      
            }
        }

        sleep(2);
    }

    return 0;

}