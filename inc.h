#ifndef INC_H
#define INC_H

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include "votante.h"
#include "candidato.h"
#include <time.h>

struct sigaction usr1_han;
struct sigaction int_han;
struct sigaction usr2_han;
struct sigaction alarm_han;
sigset_t set, oset, nomask;
sem_t *sem;
sem_t *sem_vot;
float N_SECS;
int N_PROCESOS;
int FLAG;
int CONTADOR;
int MAIN_PID;
int CANDIDATO_PID;

struct _process_info
{
    pid_t *pid_Arr;
    char *votos;
};
#endif