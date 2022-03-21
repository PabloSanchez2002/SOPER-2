#ifndef VOTANTE_H
#define VOTANTE_H

typedef struct _process_info process_info;

// extern int VARIABLES
void main_votante();
void fight_can_handler(int sig);
void int_sig_handler(int sig);
void soy_candidato(process_info pinf);
void fight_candidato(process_info pinf);
void usr1_handler(int sig);
void votar(process_info pinf);

#endif