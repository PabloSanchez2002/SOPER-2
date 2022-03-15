#include "inc.h"
/*
    Instrucción main crea procesos y comprueba señales.
*/
int main(int argc, char *argv[])
{
    int N_PROCESOS;
    pid_t proceso_act;
    float N_SECS;
    FILE *procesos = NULL;
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
    int proces[N_PROCESOS];

    if (N_PROCESOS < 2 || N_SECS <= 0)
    {
        printf("Valores no validos de argumentos.");
        return EXIT_FAILURE;
    }

    procesos = fopen("procesos.bin", "wb");
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
            // fprintf(procesos, "%i\n", proceso_act);
            proces[i] = proceso_act;
            fflush(procesos);
        }
    }
    fwrite((const void *)proces, sizeof(proces), 1, procesos);
    fclose(procesos);
    for (int i = 0; i < N_PROCESOS; i++)
    {
        printf("%i\n", proces[i]);
        kill(proces[i], SIGUSR1);
    }
    sleep(4);
    return EXIT_SUCCESS;
}