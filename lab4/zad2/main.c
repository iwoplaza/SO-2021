#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <wait.h>

#define MAX_CALLS 3

int call_idx = 0;
int call_depth = 0;

void signal_info_handler(int sig, siginfo_t* info, void* ucontext)
{
    printf("Signal info: \n");
    printf("- Signal number: %d\n", info->si_signo);
    printf("- The signal has been sent by process %d\n", info->si_pid);
    printf("- Real user ID of sending process %d\n", info->si_uid);
}

void signal_data_info_handler(int sig, siginfo_t* info, void* ucontext)
{
    printf("Signal info: \n");
    printf("- Signal number: %d\n", info->si_signo);
    printf("- Custom integer data sent %d\n", info->si_value.sival_int);
}

void signal_child_info_handler(int sig, siginfo_t* info, void* ucontext)
{
    printf("Signal info: \n");
    printf("- Signal number: %d\n", info->si_signo);
    printf("- Child pid: %d\n", info->si_pid);
    printf("- Child exit status: %d\n", info->si_status);
}

void signal_handler(int sig, siginfo_t* info, void* ucontext)
{
    int current_idx = call_idx;
    printf("start IDX=%d, DEPTH=%d\n", current_idx, call_depth);
    call_idx++;

    call_depth++;
    if (call_idx < MAX_CALLS)
    {
        raise(SIGUSR1);
    }
    call_depth--;

    printf("end  IDX=%d, DEPTH=%d\n", current_idx, call_depth);
}

void setup_sigaction(int signalidx, int flags, void (*action)(int, siginfo_t*, void*))
{
    static struct sigaction act;
    act.sa_sigaction = action;
    act.sa_flags = flags;
    sigemptyset(&act.sa_mask);

    call_idx = 0;
    sigaction(signalidx, &act, NULL);
}

int main(int argc, char** argv)
{
    /*
     * === Testing SA_SIGINFO ===
     */

    printf("\n-- Testing SA_SIGINFO\n");
    setup_sigaction(SIGUSR1, SA_SIGINFO, signal_info_handler);
    setup_sigaction(SIGUSR2, SA_SIGINFO, signal_data_info_handler);
    setup_sigaction(SIGCHLD, SA_SIGINFO, signal_child_info_handler);

    // Expecting handler
    raise(SIGUSR1);

    // Expecting handler with custom data
    sigval_t signal_value = { 15 };
    sigqueue(getpid(), SIGUSR2, signal_value);

    // Expecting SIGCHLD to know the exit status of forked child.
    pid_t pid = fork();
    if (pid == 0)
    {
        // We're the child.
        printf("(Hello from child process.)\n");

        // Exiting with status 1.
        exit(1);
    }
    else
    {
        printf("(Created child process with pid: %d)\n", pid);
    }

    // Waiting for the child process to end.
    int status = 0;
    while (wait(&status) > 0);

    /*
     * === Testing SA_NODEFER ===
     */

    printf("\n-- Testing SA_NODEFER\n");
    setup_sigaction(SIGUSR1, SA_NODEFER, signal_handler);

    // Expecting handler, with depth.
    raise(SIGUSR1);

    /*
     * === Testing SA_RESETHAND ===
     */

    printf("\n-- Testing SA_RESETHAND\n");
    setup_sigaction(SIGUSR1, SA_RESETHAND, signal_handler);

    // Expecting handler
    raise(SIGUSR1);

    // Expecting default behaviour, termination of the program.
    raise(SIGUSR1);

    return 0;
}
