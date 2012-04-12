#ifndef _PROJETO_
#define _PROJETO_

//#include "garbler.h"
#include <pthread.h>
#include <stdio.h> 
#include <stdlib.h>
#include <errno.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h> 
#include <sys/types.h>
#include <sys/stat.h> 
#include <unistd.h>
#include <fcntl.h> 
#include <ctype.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int iniciar_programa(char *, char *);
int validar_id(void);
int descobrir_id(char [], char []);
int validar_mtu(char [], char []);
int enviar_servidor (char *, int); // tem
void new_delim(char [], int); // tem 
void limpa(char[]); // tem
int ler_arquivo(void); // tem
void *receber_mensagem(void);
int entrada_usuario(int,char []); // tem
void enviar_mensagem(char []);

#endif
