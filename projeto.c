#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>


struct Quantidade
{
	int nos;
	int enlaces;
};

struct Noh
{
	int id;
        float ip[6];
        int porta;
};

struct Entrada
{
	char nome_arq[25];
	int numero_no;
};

struct Entrada entrada_usuario()
{

	struct Entrada ent;

	printf("Digite o nome do arquivo de Configuracao Inicial (CI)\n");
	gets(ent.nome_arq);	

	printf("Digite o numero do identificador do no a emular\n");
	scanf("%d",&ent.numero_no);

	return ent;
}

struct Quantidade manipular_arquivo (struct Entrada ent)
{
	FILE *fp;
	FILE *fp2;
	FILE *fp3;

	struct Quantidade qtd;

	char str[100];
	int i = 0;
	int x;
	int count = 0;

	qtd.nos = 0;
	qtd.enlaces = 0;

	fp = fopen(ent.nome_arq,"r");

	fp2 = fopen("nos.cnf","w");

	while (!feof(fp))
	{
		fscanf (fp, "%s\n", str);
		//printf("%s\n", str);

		if(isdigit(str[i]) && i == 0)
		{	
			if(((str[i] == 49 || str[i+1] == 49) && count > 1))
			{
				break;
			}

			fprintf(fp2, "%c:",str[i]);
			i++;
		}

		if(isdigit(str[i]) && i == 1)
		{	
			fprintf(fp2, "%s",str);
			i++;
		}

		if(isdigit(str[i]) && i == 2)
		{	
			fprintf(fp2, "%s",str);
			fprintf(fp2, "%c",'\n');
			qtd.nos ++;	
			i = 0;
		}
		count ++;
	}	

	fclose(fp2);
	
	fp3 = fopen("ligacoes.cnf","w");

	while (!feof(fp))
	{	
		if(isdigit(str[i]) && i == 0)
		{
			fprintf(fp3, "%c:",str[i]);
			i++;
		}

		if(((isdigit(str[i]))) && i == 1)
		{	
			fprintf(fp3, "%c,",str[i]);
			i++;
		}

		if(isdigit(str[i]) && i == 2)
		{	
			fprintf(fp3, "%s",str);
			fprintf(fp2, "%c",'\n');
			i = 0;
			qtd.enlaces++;
		}
	
		fscanf (fp, "%s\n", str);
		count ++;
	}	

	fclose(fp);
	fclose(fp3);

	return qtd;
}

void main()
{
	struct Entrada ent;
	struct Quantidade qtd;

	ent = entrada_usuario();

	qtd = manipular_arquivo(ent);
	
	printf("quantidade de nos: %d\n",qtd.nos);

	printf("quantidade de enlaces: %d\n",qtd.enlaces);

	struct Noh n[qtd.nos];
}
