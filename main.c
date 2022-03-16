#include "inc.h"
/*
    Instrucción main crea procesos y comprueba señales.
*/

typedef struct _process_info process_info;
int main(int argc, char *argv[])
{
    int N_PROCESOS;
    pid_t proceso_act;
    float N_SECS;
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

    pinf.pid_Arr = (pid_t *)malloc(sizeof(pid_t) * N_PROCESOS);

    if (N_PROCESOS < 2 || N_SECS <= 0)
    {
        printf("Valores no validos de argumentos.");
        return EXIT_FAILURE;
    }

    procesos = fopen("procesos.bin", "ab");
    if (!procesos)
    {
        printf("Error al inicializar el fichero procesos.");
        return EXIT_FAILURE;
    }
    for (int i = 0; i < N_PROCESOS; i++)
    {
        if ((proceso_act = fork()) == 0)
        {

            main_votante();
        }
        else
        {
            pinf.pid_Arr[i] = proceso_act;
        }
    }

    fwrite(pinf.pid_Arr, sizeof(int), N_PROCESOS, procesos);
    fflush(procesos);
    fclose(procesos);
    for (int i = 0; i < N_PROCESOS; i++)
    {
        kill(pinf.pid_Arr[i], SIGUSR1);
    }
    sleep(1);
    free(pinf.pid_Arr);
    return EXIT_SUCCESS;
}