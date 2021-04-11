#include <stdio.h>
#include <signal.h>

#define MAX_CALLS 3

int call_idx = 0;
int call_depth = 0;

void signal_info_handler(int sig, siginfo_t* info, void* ucontext)
{
    printf("Signal info: \n");
    printf("- Signal number: %d\n", info->si_signo);
    printf("- The signal has been sent by process %d\n", info->si_pid);
    printf("- Real user ID of sending process %d\n", info->si_uid);
    printf("- User time consumed %ld\n", info->si_utime);
    printf("- System time consumed %ld\n", info->si_stime);
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

void setup_sigaction(int flags, void (*action)(int, siginfo_t*, void*))
{
    static struct sigaction act;
    act.sa_sigaction = action;
    act.sa_flags = flags;
    sigemptyset(&act.sa_mask);

    call_idx = 0;
    sigaction(SIGUSR1, &act, NULL);
}

int main(int argc, char** argv)
{
    /*
     * === Testing SA_SIGINFO ===
     */

    printf("\n-- Testing SA_SIGINFO\n");
    setup_sigaction(SA_SIGINFO, signal_info_handler);

    // Expecting handler
    raise(SIGUSR1);

    /*
     * === Testing SA_NODEFER ===
     */

    printf("\n-- Testing SA_NODEFER\n");
    setup_sigaction(SA_NODEFER, signal_handler);

    // Expecting handler, with depth.
    raise(SIGUSR1);

    /*
     * === Testing SA_RESETHAND ===
     */

    printf("\n-- Testing SA_RESETHAND\n");
    setup_sigaction(SA_RESETHAND, signal_handler);

    // Expecting handler
    raise(SIGUSR1);

    // Expecting default behaviour
    raise(SIGUSR1);

    return 0;
}
