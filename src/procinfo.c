#include "common.h"
#include <ctype.h>
#include <stdio.h>
#include <unistd.h>

static void usage(const char *a){
    fprintf(stderr,"Usage: %s <pid>\n",a); // print correct usage
    exit(1);
}

static int isnum(const char*s){
    for(;*s;s++) 
        if(!isdigit(*s)) 
            return 0; 
    return 1;
}

    // arg count  // arg values
    // check v[0] for program name and v[1] for pid
int main(int c,char**v){
    if(c!=2 || !isnum(v[1])) 
        usage(v[0]);

    char path[256];
    snprintf(path, sizeof(path), "/proc/%s/status", v[1]); // build the file path
    // destination  // max size // formatted string // pid argument

    FILE *f = fopen(path,"r"); // open file for reading
    if(!f){ // if open failed
        perror("fopen");
        return 1;
    }

    // declaring variables to hold process info
    int pid, ppid;
    char state, cmd[256];
    long cpu_time;
    char vm_rss[256];

    fscanf(f, "PID: %d\n", &pid);
    fscanf(f, "State: %c\n", &state);  
    fscanf(f, "PPID: %d\n", &ppid);
    fscanf(f, "Cmd: %s\n", cmd);
    fscanf(f, "CPU: %ld\n", &cpu_time);
    fscanf(f, "VmRSS: %s\n", vm_rss);
    

    printf("PID: %s\n", pid); // PID
    printf("State: %c\n", state);
    printf("PPID: %d\n", ppid);
    printf("Cmd: %s\n", cmd);
    printf("CPU Time: %ld\n", cpu_time);
    printf("VmRSS: %s\n", vm_rss);

    return 0;
}
