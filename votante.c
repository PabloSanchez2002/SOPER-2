#include "inc.h"

void handler(int sig)
{
    printf("Hola\n");
    return;
}

void main_votante()
{

    struct sigaction act;
    sigset_t set, oset;

    act.sa_handler = handler;
    sigemptyset(&(act.sa_mask));
    sigaddset(&(act.sa_mask), SIGUSR2);
    sigaddset(&(act.sa_mask), SIGINT);
    act.sa_flags = 0;

    if (sigaction(SIGUSR1, &act, NULL) < 0)
    {
        perror(" sigaction ");
        exit(EXIT_FAILURE);
    }

    sigemptyset(&set);
    sigaddset(&set, SIGUSR1);

    if (sigprocmask(SIG_BLOCK, &set, &oset) < 0)
    {
        perror("sigprocmask");
        exit(EXIT_FAILURE);
    }
    sigsuspend(&oset);
}