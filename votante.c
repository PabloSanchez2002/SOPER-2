#include "inc.h"
typedef struct _process_info process_info;

// Se encarga de elegir al candidato y hace esperar al resto de procesos.
void fight_candidato(process_info pinf)
{
    // El primero que llega entra y baja el semaforo volviendose el único candidato
    if (sem_trywait(sem) == 0)
    {
        soy_candidato(pinf);
    }
    else
    {
        // Los demás se bloquean a la espera de que el candidato les envie sigusr2
        sigsuspend(&oset);
        votar(pinf);
    }
}
void votar(process_info pinf)
{
    int bytes = 0;
    sem_wait(sem_vot);
    char ch;
    int r = rand() % 2;
    FILE *votos = fopen("votos", "ab");
    if (!votos)
    {
        exit(EXIT_FAILURE);
    }
    if (r == 0)
    {
        ch = 'Y';
    }
    else
    {
        ch = 'N';
    }
    if ((bytes = fwrite(&ch, sizeof(char), sizeof(char), votos)) != sizeof(char)) kill(SIGINT, getppid());
    fclose(votos);
    fflush(stdout);
    sem_post(sem_vot);
    sigsuspend(&oset);
    fight_candidato(pinf);
}
void main_votante()
{
    // Se bloquea el proceso hasta que se recibe la señal SIGUSR1
    sigsuspend(&oset);
    int bytes = 0;
    // Se le la información de los procesos
    process_info pinf;
    pinf.pid_Arr = (pid_t *)malloc(sizeof(pid_t) * N_PROCESOS);
    if (pinf.pid_Arr == NULL)
        kill(SIGINT, getppid());
    FILE *procesos = fopen("procesos.bin", "rb");

    if (!procesos)
    {
        exit(EXIT_FAILURE);
    }
    // Se comprueba que se ha leido todo y sino se envia un kill al padre para que termine con la ejecución
    // Como todas las operaciones son de lectura y ninguna de escritura, no hace falta un semaforo.
    if ((bytes = fread(pinf.pid_Arr, sizeof(int), N_PROCESOS, procesos)) == N_PROCESOS * sizeof(int))
    {
        kill(SIGINT, getppid());
    }
    fclose(procesos);
    // Se llama a la función para pelear por ser el candidato.
    fight_candidato(pinf);
    exit(EXIT_SUCCESS);
}
