#include "common.h"
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
static void usage(const char *a){
    fprintf(stderr,"Usage: %s <cmd> [args]\n",a); 
    exit(1);
}

static double d(struct timespec a, struct timespec b){
    // calculate difference in seconds
    return (b.tv_sec-a.tv_sec) + (b.tv_nsec-a.tv_nsec) / 1e9;
}

int main(int c,char**v){
    if(c<2) 
        usage(v[0]);

    // PROCESS OF INITIALIZING TIME VARIABLES
    struct timespec start, end;

    // PROCESS OF STARTING THE TIMER AND FORKING
    clock_gettime(CLOCK_MONOTONIC, &start); // start before fork
    pid_t pid = fork();
    if(pid==0){ // if child process fails
        execvp(v[1], &v[1]);
        perror("execvp");
        _exit(1); 
    }

    // PROCESS OF WAITING FOR CHILD PROCESS
    int status;
    waitpid(pid, &status, 0);
    clock_gettime(CLOCK_MONOTONIC, &end);

    // PROCESS OF PRINTING RESULTS
    printf("pid=%d elapsed=%.3f ", pid, d(start, end));
    if(WIFEXITED(status)){ // if exited normally
        printf("exit=%d\n", WEXITSTATUS(status));
    }else if(WIFSIGNALED(status)){ // if terminated by signal
        printf("signal=%d\n", WTERMSIG(status));
    }else{ // error case
        printf("exit=-1\n");
    }


    return 0;
}
