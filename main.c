#include "inc.h"

// Estructura de datos que guarda los pids de la votación y los votos
typedef struct _process_info process_info;
// Manejador de la señal SIGINT
void int_sig_handler(int sig)
{
    if (getpid() == MAIN_PID)
    {
        FLAG = 2;
        return;
    }
    else
    {
        
        exit(EXIT_FAILURE);
    }
}
// Manejador de la señal SIGUSR2

void usr2_sig_handler(int sig)
{
    return;
}
// Manejador de la señal SIGUSR1

void usr1_handler(int sig)
{
    return;
}
// Manejador de la señal SIGALRM
void alarm_sig_handler(int sig)
{

    FLAG = 3;
    return;
}
// Mascaras necesarias para el funcionamiento del programa
void mascaras()
{
    sigemptyset(&oset);
    sigemptyset(&nomask);
    sigemptyset(&set);
    sigaddset(&set, SIGUSR1);
    sigaddset(&set, SIGUSR2);
    sigaddset(&set, SIGALRM);
    sigaddset(&set, SIGINT);

    if (sigprocmask(SIG_BLOCK, &set, &oset) < 0)
    {
        perror("sigprocmask");
        exit(EXIT_FAILURE);
    }
}
// Manejadores necesarios
void manejadores()
{
    // SIGUSR 1 manejador
    usr1_han.sa_handler = usr1_handler;
    sigemptyset(&(usr1_han.sa_mask));
    sigaddset(&(usr1_han.sa_mask), SIGUSR2);
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
    sigaddset(&(usr1_han.sa_mask), SIGUSR1);
    usr2_han.sa_flags = 0;

    if (sigaction(SIGUSR2, &usr2_han, NULL) < 0)
    {
        perror(" sigaction ");
        exit(EXIT_FAILURE);
    }

    // ALARM MANEJADOR
    alarm_han.sa_handler = alarm_sig_handler;
    sigemptyset(&(alarm_han.sa_mask));
    alarm_han.sa_flags = 0;
    if (sigaction(SIGALRM, &alarm_han, NULL) < 0)
    {
        perror(" sigaction ");
        exit(EXIT_FAILURE);
    }
}
// SEMAFOROS UTILIZADOS EN EL PROGRAMA
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
}

int main(int argc, char *argv[])
{
    // creación de variables útiles
    pid_t proceso_act;
    FILE *procesos = NULL;
    process_info pinf;
    int bytes = 0;

    /*
        Comprobación de argumentos entregados y errores
    */
    if (argc != 3)
    {
        printf("El número de argumentos es incorrecto.");
        return EXIT_FAILURE;
    }
    // Asignación de variables globales
    N_PROCESOS = atoi(argv[1]);
    N_SECS = atof(argv[2]);
    FLAG = 1;
    CONTADOR = 0;
    MAIN_PID = getpid();
    pinf.pid_Arr = (pid_t *)malloc(sizeof(pid_t) * N_PROCESOS);
    if (!pinf.pid_Arr)
        return EXIT_FAILURE;
    // Comprobación argumentos
    if (N_PROCESOS < 2 || N_SECS <= 0)
    {
        printf("Valores no validos de argumentos.");
        return EXIT_FAILURE;
    }
    // Comprobación alarma
    if (alarm(N_SECS))
    {
        printf("Esa alarma ya existe");
        return EXIT_FAILURE;
    }
    // Creación y comprobación de fichero
    procesos = fopen("procesos.bin", "wb");

    if (!procesos)
    {
        printf("Error al inicializar el fichero procesos.");
        return EXIT_FAILURE;
    }

    // Se crean las mascaras, manejadores y semaforos
    manejadores();
    mascaras();
    semaforos();

    for (int i = 0; i < N_PROCESOS; i++)
    {
        // Se crean todos los procesos y llaman a la función main_votante()
        if ((proceso_act = fork()) == 0)
        {
            fflush(stdout);
            main_votante();
        }
        else
        {
            // Proceso padre añade a la estructura los pids de los procesos
            pinf.pid_Arr[i] = proceso_act;
        }
    }
    // Escribe la estructura en un fichero binario
    if ((bytes = fwrite(pinf.pid_Arr, sizeof(int), N_PROCESOS, procesos)) != N_PROCESOS)
    {
        printf("%i,\n", bytes);
        free(pinf.pid_Arr);
        sem_close(sem);
        sem_close(sem_vot);
        sem_unlink("Create_Candidato");
        sem_unlink("Votar_Candidato");
        return EXIT_FAILURE;
    }
    fflush(procesos);
    // Cierra el fichero
    fclose(procesos);

    // Se envia señal
    for (int i = 0; i < N_PROCESOS; i++)
    {
        fflush(stdout);
        // se envia una señal a los procesos creados
        kill(pinf.pid_Arr[i], SIGUSR1);
    }
    // Se suspende el proceso main hasta recibir señal
    sigsuspend(&oset);
    // Se ha mandado la señal SIGINT o SIGALARM asi que se sale y se envia la señal al resto de procesos
    for (int i = 0; i < N_PROCESOS; i++)
    {

        kill(pinf.pid_Arr[i], SIGINT);
    }
    // Se espera a que finalicen los procesos
    for (int i = 0; i < N_PROCESOS; i++)
    {
        waitpid(pinf.pid_Arr[i], NULL, 0);
    }
    // FLAG que indica que ha salido por señal
    if (FLAG == 2)
    {
        printf("Terminated by signal\n");
    }
    // FLAG que indica que ha salido por alarma
    else
    {
        printf("Terminated by alarm\n");
    }
    // Liberación de recursos.
    free(pinf.pid_Arr);
    sem_close(sem);
    sem_close(sem_vot);
    sem_unlink("Create_Candidato");
    sem_unlink("Votar_Candidato");

    return EXIT_SUCCESS;
}
