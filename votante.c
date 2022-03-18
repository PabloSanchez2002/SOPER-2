#include "inc.h"

void usr1_handler(int sig)
{
}
void fight_candidato()
{

    if (sem_trywait(sem) == 0)
    {
        // Hasta aquí bien bucle 1
        soy_candidato();
    }
    else
    {
        // Hasta aquí bien bucle 1
        sigsuspend(&oset);
        votar();
    }
}
void votar()
{
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
    int bytes = fwrite(&ch, sizeof(char), sizeof(char), votos);
    fclose(votos);
    fflush(stdout);
    sem_post(sem_vot);
    main_votante();
}
void main_votante()
{
    // Primer bucle hasta aquí bien.
    sigsuspend(&oset);
    // Primer bucle hasta aquí bien.
    fight_candidato();
    exit(EXIT_SUCCESS);
}
