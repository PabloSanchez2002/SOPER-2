#include "inc.h"
typedef struct _process_info process_info;
void soy_candidato()
{
    // HASTA AQUI BIEN
    process_info pinf;
    pinf.pid_Arr = (pid_t *)malloc(sizeof(pid_t) * N_PROCESOS);
    pinf.votos = (char *)malloc(sizeof(char) * N_PROCESOS);
    int leidos;
    int counterPos = 0;
    int counterNeg = 0;
    FILE *procesos = fopen("procesos.bin", "rb");

    if (!procesos)
    {
        exit(EXIT_FAILURE);
    }

    fread(pinf.pid_Arr, sizeof(int), N_PROCESOS, procesos);
    fclose(procesos);
    // Hasta aquí bien
    sleep(1);
    while (1)
    {
        counterPos = 0;
        counterNeg = 0;
        FILE *votos = fopen("votos", "wb");
        if (!votos)
        {
            exit(EXIT_FAILURE);
        }
        fclose(votos);
        // Hasta aquí bien
        for (int i = 0; i < N_PROCESOS; i++)
        {
            if (getpid() != pinf.pid_Arr[i])
            {
                kill(pinf.pid_Arr[i], SIGUSR1);
            }
        }
        while (1)
        {
            sem_wait(sem_vot);
            FILE *votos = fopen("votos", "rb");
            if (!votos)
            {
                exit(EXIT_FAILURE);
            }
            if ((leidos = fread(pinf.votos, sizeof(char), N_PROCESOS - 1, votos)) == ((N_PROCESOS - 1)))
            {
                fflush(stdout);
                fclose(votos);
                printf("Candidate %i => [ ", getpid());
                for (int i = 0; i < N_PROCESOS - 1; i++)
                {
                    printf("%c ", pinf.votos[i]);
                    if (pinf.votos[i] == 'Y')
                    {
                        counterPos++;
                    }
                    else
                    {
                        counterNeg++;
                    }
                }
                if (counterPos > counterNeg)
                {
                    printf("] => Accepted\n");
                }
                else
                {
                    printf("] => Rejected\n");
                }
                sem_post(sem_vot);
                sleep(0.25);
                sem_post(sem);
                kill(getppid(), SIGUSR2);
                main_votante();
            }
            fclose(votos);

            sem_post(sem_vot);
            sleep(0.01);
        }
    }
}
