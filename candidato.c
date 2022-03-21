#include "inc.h"
typedef struct _process_info process_info;
void soy_candidato(process_info pinf)
{

    // Reserva memoria para todos los votos posibles
    pinf.votos = (char *)malloc(sizeof(char) * N_PROCESOS);
    if (!pinf.votos)
        kill(SIGINT, getppid());
    int leidos;
    // Para ver si es o no elegido
    int counterPos = 0;
    int counterNeg = 0;

    while (1)
    {
        // Se crea el fichero, wb para que sea binario y se borre lo que anteriormente estuviera escrito
        FILE *votos = fopen("votos", "wb");
        if (!votos)
        {
            exit(EXIT_FAILURE);
        }
        fclose(votos);
        for (int i = 0; i < N_PROCESOS; i++)
        {
            if (getpid() != pinf.pid_Arr[i])
            {
                kill(pinf.pid_Arr[i], SIGUSR2); // PARA NO TENER PROBLEMAS CON EL SIGUSR1 ENVIADO POR MAIN HEMOS DECIDIDO ENVIAR SIGUSR2
            }
        }

        while (1)
        {
            // Baja el semaforo para asegurarse de que es el unico que lee y nadie escribe
            sem_wait(sem_vot);
            FILE *votos = fopen("votos", "rb");
            if (!votos)
            {
                exit(EXIT_FAILURE);
            }
            // Cambia la máscara para no recibir señales durante la lectura ya que podría dejar el archivo abierto si se reciviera sigint
            if (sigprocmask(SIG_BLOCK, &set, &oset) < 0)
            {
                perror("sigprocmask");

                exit(EXIT_FAILURE);
            }
            // Comprueba el número de votos escritos.
            if ((leidos = fread(pinf.votos, sizeof(char), N_PROCESOS - 1, votos)) == ((N_PROCESOS - 1)))
            {

                fclose(votos);
                // Escritura del mensaje en cuestión
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
                // Se sube el semaforo que permite votar y además el de elección de candidato para la siguiente ronda
                sem_post(sem_vot);
                sem_post(sem);

                for (int i = 0; i < N_PROCESOS; i++)
                {
                    if (getpid() != pinf.pid_Arr[i])
                    {
                        // Envia la señal a el resto de procesos para que vuelva a comenzar la siguiente ronda
                        kill(pinf.pid_Arr[i], SIGUSR1);
                    }
                }
                // Descanso de 250 ms
                free(pinf.votos);
                usleep(250000);
                fight_candidato(pinf);
            }
            // Cierro la lectura, levanto el semaforo para poder escribir de nuevo votos y cambio la máscara para tratar señales que puedan llegarme.
            fclose(votos);
            sem_post(sem_vot);
            if (sigprocmask(SIG_SETMASK, &nomask, NULL) < 0)
            {
                perror("sigprocmask");
                exit(EXIT_FAILURE);
            }
            usleep(1000);
        }
    }
}
