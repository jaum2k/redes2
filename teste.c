#include "projeto.h"
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void main(int argc, char **argv) 
{
	pthread_t aux;
	int resp = 0;
	int mtu = 0;
	int no_destino;	
	int ret;
	char no;
	char buf[32];
	
		
	if(argc != 3)
	{                                     
		printf("Use: %s topologia.cnf numero_do_noh\n",argv[0]);
		exit(1);
	}

	ret = iniciar_programa(argv[1],argv[2]);                                

	if(ret == 0)
	{
		do
		{
			printf("Digite um numero_do_noh valido!\n");
			scanf("%c",&no);
			ret = iniciar_programa(argv[1],argv[2]); 
		}while(ret == 0);
	}

	while (1)	
	{
		printf("Entre com o nó para quem enviar a mensagem\n");
		fflush(stdin);
		scanf("%d", &no_destino);
		printf("\nEntre com a mensagem que deseja enviar\n");
		fflush(stdin);
		scanf("%s",buf);
		printf("\n");

		buf[sizeof(buf+1)] = '\0';

		ret = entrada_usuario(no_destino,buf);
	}


}

