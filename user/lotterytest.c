#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "user/pinfo.h"

#define RUN_TIME 200

void
cpu_work(void){
    volatile unsigned long x = 0;

    while(1){
        x++;
        x = x * 3 + 1;
    }
}

int main(void){
    int pid1, pid2;
    struct pinfo info;

    printf("\n----------------------------------\n");
    printf(" xv6 Lottery Scheduler Perf Test\n");
    printf("----------------------------------\n");

    // First CPU bound process
    pid1 = fork();
    if(pid1 < 0){
        printf("Fork failed for process 1\n");
        exit(1);
    }

    if(pid1 == 0){
        settickets(10); // setting tickets for process 1

        printf("Process 1 started with PID: %d and Tickets: 10\n", getpid());

        cpu_work(); // CPU bound work
        
    }

    // Second CPU bound process
    pid2 = fork();

    if(pid2 < 0){
        printf("Fork failed for process 2\n");
        kill(pid1); // kill the first process if second fork fails
        exit(1);
    }

    if(pid2 == 0){
        settickets(90); // setting tickets for process 2
        printf("Process 2 started with PID: %d and Tickets: 90\n", getpid());

        cpu_work(); // CPU bound work
    }

    // Parent process waits while the child processes compete for CPU
    printf("Running CPU competition...\n");

    // Let the processes run for a while
    uint start_time = uptime();
    while(uptime() - start_time < RUN_TIME){
        // Sleep for a short duration to allow other processes to run until RUN_TIME timer ticks have passed
    }

    // Fetch process information before killing the child processes
    if(getpinfo(&info) < 0){
        printf("Failed to get process info getinfop failed.\n");
        kill(pid1);
        kill(pid2);
        wait(0);
        wait(0);
        exit(1);
    }

    printf("\n----------------------------------\n");
    printf("              Results\n");
    printf("\n----------------------------------\n");
    printf("PID\tTickets\tRuntime\n");

    uint64 runtime1 = 0, runtime2 = 0;
    int tickets1 = 0, tickets2 = 0;

    // finding the child processes in pinfo.
    for(int i = 0; i < NPROC; i++){
        if(info.inuse[i]){
            if(info.pid[i] == pid1){
                runtime1 = info.runtime_ticks[i];
                tickets1 = info.tickets[i];
            }

            else if(info.pid[i] == pid2){
                runtime2 = info.runtime_ticks[i];
                tickets2 = info.tickets[i];
            }
        }
    }

    printf("%d\t%d\t%ld\n", pid1, tickets1, runtime1);
    printf("%d\t%d\t%ld\n", pid2, tickets2, runtime2);

    uint64 total_runtime = runtime1 + runtime2;

    if(total_runtime > 0){
        uint64 share1 = (runtime1 * 10000) / total_runtime;
        uint64 share2 = (runtime2 * 10000) / total_runtime;

        printf("\nCPU Share:\n");
        printf("Process 1: %d.%d%%\n", (int)(share1 / 100), (int)(share1 % 100));
        printf("Process 2: %d.%d%%\n", (int)(share2 / 100), (int)(share2 % 100));

        printf("\nExpected CPU Share:\n");
        printf("Process 1: approx 10%%\n");
        printf("Process 2: approx 90%%\n");
    }

    // terminating the cpu bound children processes
    kill(pid1);
    kill(pid2);

    wait(0);
    wait(0);

    printf("\nTest completed.\n");

    exit(0);
}
