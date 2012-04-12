#include "garbler.h"
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

#define _GNU_SOURCE

//****************************STRUCTS GLOBAIS****************************

struct Enlaces{
	int id_src;
	int id_dst;
	char mtu[6];
};

struct Nos { 
	int id; 
	char ip[16]; 
	char porta[6]; 
};

struct Dados{
	int no_destino;
	char ip_dst[16];
	char port_dst[6];
	char ip_src[16];	
	char port_src[6];
	char data[32];
};	

//****************************VARIAVEIS GLOBAIS****************************

struct Nos noh[6]; //maximo de 6 n�s
struct Enlaces enlace[18]; //maximo de 3 enlaces por n� (3x6) 
struct Dados dados;

// variaveis globais


int soma;
pthread_t aux;
int resp = 0;
char nome_arq[25]=""; 
int no_destino=0;
char ip[16]="";
char port[6]="";
char mensagem[20]="";
int id = 0;
int nohh =0;
int ok = 0;
int n_nos = 0;
int n_enlaces = 0;//numero nos e enlaces
char buf[32];
char bufferr[32];

//****************************FUNCOES AUXILIARES****************************

void enviar_mensagem()
{
	// variaveis importantes
	int s,server_address_size; 
	unsigned short portt;
	// estrutura server do tipo sockaddr_in
	struct sockaddr_in server;
	// buffer que armazena a string que contem a chamada
	int i;
	int k;
	int l;
	char c;
	int mtu;

	portt = htons(atoi(dados.port_dst));

	if ((s = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		perror("socket()");
		exit(1);
	}
	
	/* Define o endereço IP e a porta do servidor */
	server.sin_family      = AF_INET;            /* Tipo do endereço         */
	server.sin_port        = portt;               /* Porta do servidor        */
	server.sin_addr.s_addr = inet_addr(dados.ip_dst); /* Endereço IP do servidor  */
	
	//printf("\n\nBUFF: %s",buf);

	server_address_size = sizeof(server);


	for (k = 0; k < (strlen(buf)); k++)
	{
		l = buf[k];
		soma += l;
	}

//	printf("soma %d\n",soma);

	 // sizeof(server) envia o tamanho da estrutura server
	//if (sendto(s, buf, sizeof(buf)+1, 0, (struct sockaddr *)&server, sizeof(server)) < 0)
	
	
	if (sendto_garbled(s, buf, sizeof(buf)+1,0,(struct sockaddr *)&server,sizeof(server)) < 0)
	// caso ocorra algum problema, o retorno sera -1 e printa a mensagem senao envia a msg
	{
		perror("sendto()");
		exit(2);
	}
}

void *receber_mensagem()
{
	int sockint,s, namelen, client_address_size;
	struct sockaddr_in client, server;
	int k = 0;	
	int somalocal = 0; 
	int l;

	unsigned short portt;
	
	portt = htons(atoi(noh[nohh-1].porta));

	if ((s = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		perror("socket()");
		exit(1);
	}	
	
	server.sin_family      = AF_INET;   /* Tipo do endereço             */
	server.sin_port        = portt;         /* Escolhe uma porta disponível */
	server.sin_addr.s_addr = inet_addr(noh[nohh-1].ip);/* Endereço IP do servidor      */
	
	   if (bind(s, (struct sockaddr *)&server, sizeof(server)) < 0)
	   {
	       perror("bind()");
	       exit(1);
	   }

	   /* Consulta qual porta foi utilizada. */
	   
	   if (getsockname(s, (struct sockaddr *) &server, &namelen) < 0)
	   {
	       perror("getsockname()");
	       exit(1);
	   }


	   namelen = sizeof(server);
	   /* Imprime qual porta foi utilizada. */
	   client_address_size = sizeof(client);
	
	while(1)
	{
		if(recvfrom(s, buf, sizeof(buf)+1, 0, (struct sockaddr *) &client,&client_address_size)  < 0)
		{
			perror("recvfrom()");
			exit(1);
		}
		else  
		{
		
		printf("Recebida a mensagem %s do endereço IP %s da porta %d\n",buf,inet_ntoa(client.sin_addr),ntohs(server.sin_port));


		somalocal = 0;

		for (k = 0; k < (strlen(buf)); k++)
		{
			l = buf[k];
			somalocal += l;
		}

		if(somalocal == soma)
		{
			printf("checksum feito com sucesso\n");
		}
		else
			printf("falha no checksum...\n");
		}
	}
}

void new_delim(char str[], int tam)//funcao para delimitar ip, e porta;
{
        int i;
        for(i=0;i<tam;i++)
        {
                if(str[i]==',' || str[i]==';')
                {
                        str[i]='\0';
                        break;
                }
        }
}

int iniciar_programa(char *arquivo, char *no)
{
	strcpy(nome_arq,arquivo);
	nohh = atoi(no);
	int ret;
	
	printf("\n\n--------------------PROJETO REDES 2--------------------");
	printf("\n\nArquivo de configuracao: %s\n", nome_arq);
	printf("Noh atual: %d\n\n", nohh);

	ret = ler_arquivo();
	
	return ret;	
}

int validar_id()	
{
    int i,novo=0;

	ok = 0;

    	for (i = 0 ; i < n_nos; i++)
	{
		if(noh[i].id == nohh)
		{
			strcpy(port, noh[i].porta);
			strcpy(ip, noh[i].ip);
			ok++;	
			pthread_create (&aux, NULL, (void* ) &receber_mensagem, NULL); // nivel enlace
			return 1;
		}
	}
	if(ok == 0)
	{
			printf("\n\nNoh digitado nao existe no arquivo de conf.\nDigitar noh valido:"); 
			return 0;
	}
}

int descobrir_id(char ip2[], char port2[])
{
	int i;
	for(i=0;i<n_nos;i++)
	{
		if(!strcmp(noh[i].ip,ip2)) //IPs bateram
			if(!strcmp(noh[i].porta,port2)) //Portas bateram
				return noh[i].id; //retorna id
	}
	return 0; //id nao existe
}

int validar_mtu(char ip[], char port[])
{
	int i;
	
	
	for(i=0;i<n_enlaces;i++)//verifica rfligacao  1 -> 2 ou 2 -> 1
	{
		
		if(enlace[i].id_src == nohh) //encontrou meu id
		{
			if(enlace[i].id_dst == descobrir_id(ip, port)) //encontrou destino valido
			{
				return atoi(enlace[i].mtu);
			}
		}
	
		if(enlace[i].id_dst == nohh) //encontrou meu id
		{
			if(enlace[i].id_src == descobrir_id(ip, port)) //encontrou origem valida
  			{
  				return atoi(enlace[i].mtu); //retorna o mtu
			}	
		}
	}	
	
	return 0; //nao existe enlace entre os nohs				
}

void limpa(char str[])//limpa string
{
   int i;
   for(i=0;i<sizeof(str);i++)
   	str[i]='\0';
}

//****************************FUNCOES PRINCIPAIS****************************
int ler_arquivo()
{
	FILE* fp;
	char buffer[16];
	char limpa[16]="                ";
	int i,j;
	int resp;	

	fp = fopen(nome_arq, "r");


	fscanf(fp,"%s",buffer);
	buffer[strlen(buffer)+1] = '\0';

	printf("--------informacao dos nos--------\n\n");
	
	if(!strcmp(buffer, "Nos") || !strcmp(buffer, "Nós"))
	{
		for(i=0;i<6;i++)
		{
			strcpy(buffer,limpa);//limpa o buffer
			fscanf(fp, "%s", buffer); //le id da maquina ou "enlaces"
			
			if(!strcmp(buffer,"Enlaces")) 
				break;//condicao de parada dos nos
			
			noh[i].id = atoi(buffer); 
			
			for(j=0; j<2; j++)
			{
				fscanf(fp, "%s", buffer);
			}//pula "IP" e "="
			
			strcpy(buffer,limpa);//limpa o buffer
			fscanf(fp, "%s", buffer);//le valor IP
			strcpy(noh[i].ip, buffer);
			new_delim(noh[i].ip,16);
			
			for(j=0; j<2; j++)
			{
				fscanf(fp, "%s", buffer);
			}//pula "Porta" e "="
			
			strcpy(buffer,limpa);//limpa o buffer
			fscanf(fp, "%s", buffer);//le valor porta
			strcpy(noh[i].porta, buffer);
			new_delim(noh[i].porta,6);
			
			printf("ID: %d\nIP: %s\nPORTA: %s\n\n",noh[i].id,noh[i].ip,noh[i].porta);
		
			n_nos++; 
		}  
	}

	if(n_nos == 6)
		fscanf(fp, "%s", buffer);//se houver 6 nos deve se forcar o if-case de "enlaces"

	printf("--------relacao de enlaces--------\n\n");
	
	if(!strcmp(buffer, "Enlaces"))
	{	      
		for(i=0;i<18;i++)
		{
			strcpy(buffer,limpa);				
			fscanf(fp, "%s", buffer); //id src
			
			if(!strcmp(buffer,"Fim"))
				break;
			
			enlace[i].id_src = atoi(buffer);
			fscanf(fp, "%s", buffer);//pula "->" 
			strcpy(buffer,limpa);
			
			fscanf(fp, "%s", buffer);//id dst
			enlace[i].id_dst = atoi(buffer);
			
			for(j=0; j<2; j++)
			{
				fscanf(fp, "%s", buffer);
			}//pula "MTU" e "="
			
			strcpy(buffer,limpa);
			fscanf(fp, "%s", buffer);//le valor MTU
			strcpy(enlace[i].mtu, buffer);
			new_delim(enlace[i].mtu,6);//mtu precisa ser char para rodar na funcao new_delim	
			printf("SOURCE: %d\nDESTINATION: %d\nMTU: %s\n\n",enlace[i].id_src,enlace[i].id_dst,enlace[i].mtu);
				
			n_enlaces++;
		}   
	}
	fclose(fp);
	resp = validar_id();
	
}


void entrada_usuario(int no_destino, char bufferr[])
{
	int i;
	int mtu;

	strcpy(buf,bufferr);

	for(i = 0; i< n_nos; i++)
	{
		if(noh[i].id == nohh){//obtenho o ip e a porta origem
			strcpy(dados.ip_src,noh[i].ip);     
			strcpy(dados.port_src,noh[i].porta);
			printf("IP origem %s e porta origem %s\n", dados.ip_src, dados.port_src);
		
		}
		if(noh[i].id == no_destino){//obtenho o ip e a porta destino relacionados ao no digitado
			strcpy(dados.ip_dst,noh[i].ip);
			strcpy(dados.port_dst,noh[i].porta);
			printf("IP destino %s e porta destino %s\n", dados.ip_dst, dados.port_dst);
		
		}
	}
	
	mtu = validar_mtu(dados.ip_dst,dados.port_dst);
			
	if(mtu == 0)
	{
	    printf("Nao existe enlace entre os nohs.\nDescartando pacote...\n");
	    exit(0);	
	}
	else
	{
		//pthread_create (&aux, NULL, receber_mensagem, NULL);
		enviar_mensagem();	
	}
}

