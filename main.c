#include "inc.h"
/*
    Instrucción main crea procesos y comprueba señales.
*/

typedef struct _process_info process_info;

void int_sig_handler(int sig)
{
    if (sem_trywait(sem_print) == 0)
    {
        FLAG = 2;
        return;
    }
    else
    {
        exit(EXIT_FAILURE);
    }
}
void usr2_sig_handler(int sig)
{
    return;
}
void alarm_sig_handler(int sig)
{
    if (sem_trywait(sem_print) == 0)
    {
        FLAG = 3;
    }
    return;
}
void mascaras()
{
    sigemptyset(&set);
    sigaddset(&set, SIGUSR1);
    sigaddset(&set, SIGUSR2);
    if (sigprocmask(SIG_BLOCK, &set, &oset) < 0)
    {
        perror("sigprocmask");
        exit(EXIT_FAILURE);
    }
}

void manejadores()
{
    // SIGUSR 1 manejador
    usr1_han.sa_handler = usr1_handler;
    sigemptyset(&(usr1_han.sa_mask));
    sigaddset(&(usr1_han.sa_mask), SIGUSR2);
    sigaddset(&(usr1_han.sa_mask), SIGINT);
    usr1_han.sa_flags = 0;
    if (sigaction(SIGUSR1, &usr1_han, NULL) < 0)
    {
        perror(" sigaction ");
        exit(EXIT_FAILURE);
    }
    // INT MANEJADOR

    int_han.sa_handler = int_sig_handler;
    sigemptyset(&(int_han.sa_mask));
    int_han.sa_flags = 0;

    if (sigaction(SIGINT, &int_han, NULL) < 0)
    {
        perror(" sigaction ");
        exit(EXIT_FAILURE);
    }

    // SIGUSR 2 MANEJADOR

    usr2_han.sa_handler = usr2_sig_handler;
    sigemptyset(&(usr2_han.sa_mask));
    usr2_han.sa_flags = 0;

    if (sigaction(SIGUSR2, &usr2_han, NULL) < 0)
    {
        perror(" sigaction ");
        exit(EXIT_FAILURE);
    }

    // ALARM HANDLER
    alarm_han.sa_handler = alarm_sig_handler;
    sigemptyset(&(alarm_han.sa_mask));
    alarm_han.sa_flags = 0;
    if (sigaction(SIGALRM, &alarm_han, NULL) < 0)
    {
        perror(" sigaction ");
        exit(EXIT_FAILURE);
    }
}

void semaforos()
{
    if ((sem = sem_open("Create_Candidato", O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 1)) == SEM_FAILED)
    {
        perror("sem_create");
        exit(EXIT_FAILURE);
    }
    if ((sem_vot = sem_open("Votar_Candidato", O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 1)) == SEM_FAILED)
    {
        perror("sem_create");
        exit(EXIT_FAILURE);
    }
    if ((sem_print = sem_open("Print_Final", O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 1)) == SEM_FAILED)
    {
        perror("sem_create");
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char *argv[])
{

    pid_t proceso_act;
    FILE *procesos = NULL;
    process_info pinf;
    /*
        Comprobación de argumentos entregados
    */
    if (argc != 3)
    {
        printf("El número de argumentos es incorrecto.");
        return EXIT_FAILURE;
    }
    N_PROCESOS = atoi(argv[1]);
    N_SECS = atof(argv[2]);
    FLAG = 1;
    CONTADOR = 0;
    printf("Main: %i\n",getpid());
    pinf.pid_Arr = (pid_t *)malloc(sizeof(pid_t) * N_PROCESOS);

    if (N_PROCESOS < 2 || N_SECS <= 0)
    {
        printf("Valores no validos de argumentos.");
        return EXIT_FAILURE;
    }
    if (alarm(N_SECS))
    {
        printf("Esa alarma ya existe");
    }
    procesos = fopen("procesos.bin", "wb");

    if (!procesos)
    {
        printf("Error al inicializar el fichero procesos.");
        return EXIT_FAILURE;
    }

    // HASTA AQUI BIEN
    manejadores();
    mascaras();
    semaforos();

    // HASTA AQUÍ BIEN
    for (int i = 0; i < N_PROCESOS; i++)
    {
        if ((proceso_act = fork()) == 0)
        {
            fflush(stdout);
            main_votante();
        }
        else
        {
            pinf.pid_Arr[i] = proceso_act;
        }
    }
    // HASTA AQUI BIEN
    fwrite(pinf.pid_Arr, sizeof(int), N_PROCESOS, procesos);
    fflush(procesos);
    fclose(procesos);
    // HASTA AQUI BIEN
    while (FLAG == 1)
    {
        for (int i = 0; i < N_PROCESOS; i++)
        {
            fflush(stdout);
            kill(pinf.pid_Arr[i], SIGUSR1);
        }
        sigsuspend(&oset);
    }
    for (int i = 0; i < N_PROCESOS; i++)
    {
        kill(pinf.pid_Arr[i], SIGKILL);
    }

    for (int i = 0; i < N_PROCESOS; i++)
    {
        waitpid(pinf.pid_Arr[i], NULL, 0);
    }

    if (FLAG == 2)
    {
        printf("Terminated by signal\n");
    }
    else
    {
        printf("Terminated by alarm\n");
    }
    free(pinf.pid_Arr);
    sem_close(sem);
    sem_close(sem_vot);
    sem_close(sem_print);
    sem_unlink("Print_Final");
    sem_unlink("Create_Candidato");
    sem_unlink("Votar_Candidato");

    return EXIT_SUCCESS;
}
