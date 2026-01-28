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

    // PROCESS OF BUILDING THE FILE PATH
    char path[256];
    snprintf(path, sizeof(path), "/proc/%s/status", v[1]); // build the file path
    // destination  // max size // formatted string // pid argument

    // PROCESS OF OPENING THE FILE
    FILE *file = fopen(path,"r"); // open file for reading
    if(!file){ // if open failed
        perror("fopen");
        return 1;
    }

    // PROCESS OF INITIALIZING VARIABLES
    char line[512]; // contain the lines read from the file
    int pid = -1, ppid = -1;
    char state = '?';
    char cmd[256] = "";
    long user_time = 0, sys_time = 0;
    char vm_rss[256] = "";

    // PROCESS OF PARSING THE FILE CONTENT
    while (fgets(line, sizeof(line), file)) {
        if (sscanf(line, "Pid:%d", &pid) == 1) {// blank space
        }       
        else if (sscanf(line, "Name:%255s", cmd) == 1) { }
        else if (strncmp(line, "State:", 6) == 0) {
            char *p = line + 6;  // skipping 6 characters
            while (*p && isspace((unsigned char)*p)) // loop until non-space
                p++;
            if (*p) // check not null
                state = *p;
        }
        else if (sscanf(line, "PPid:%d", &ppid) == 1) { }
        else if (sscanf(line, "VmRSS:%255[^\n]", vm_rss) == 1) {
            char *p = vm_rss;
            while (*p && isspace((unsigned char)*p)) p++;
            if (p != vm_rss) memmove(vm_rss, p, strlen(p) + 1);
        }
    }
    fclose(file);

    // PROCESS OF READING /proc/[pid]/stat FOR CPU TIME
    snprintf(path, sizeof(path), "/proc/%s/stat", v[1]);
    FILE *statf = fopen(path, "r");
    if (statf) {
        if (fgets(line, sizeof(line), statf)) {
            char *r = strchr(line, ')');
            if (r && r[1] == ' ') {
                r += 2; /* skip ") " */
                if (sscanf(r, "%c %d %*d %*d %*d %*d %*u %*u %*u %*u %*u %ld %ld",
                           &state, &ppid, &user_time, &sys_time) == 4) {
                }
            }
        }
        fclose(statf);
    }
        
    // PROCESS OF PRINTING THE INFORMATION
    printf("PID: %d\n", pid); // PID
    printf("State: %c\n", state);
    printf("PPID: %d\n", ppid);
    printf("Cmd: %s\n", cmd[0] ? cmd : "unknown");
    {
        long hz = sysconf(_SC_CLK_TCK);
        if (hz > 0) {
            double user_sec = (double)user_time / (double)hz;
            double sys_sec = (double)sys_time / (double)hz;
            printf("CPU: %.3f %.3f\n", user_sec, sys_sec);
        } else {
            printf("CPU: %ld %ld\n", user_time, sys_time);
        }
    }
    printf("VmRSS: %s\n", vm_rss[0] ? vm_rss : "unknown");

    return 0;
}
