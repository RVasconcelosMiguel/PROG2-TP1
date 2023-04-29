/*****************************************************************/
/*          Biblioteca plantas | PROG2 | MIEEC | 2020/21         */
/*****************************************************************/

#include "plantas.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

planta *planta_nova(const char *ID, const char *nome_cientifico, char **alcunhas, int n_alcunhas, int n_sementes){
	if (strlen(ID) > 10 || strlen(nome_cientifico) > 250){
		return NULL;
	}

	planta* p;
	p = (planta*)malloc(sizeof(planta));

    if(p==NULL)
    return NULL;

	strcpy(p->ID, ID);
	strcpy(p->nome_cientifico, nome_cientifico);
	p->n_alcunhas = n_alcunhas;
	p->n_sementes = n_sementes;
	if (alcunhas == NULL) {
		p->alcunhas = NULL;
	}
	else {
		p->alcunhas = (char**)malloc(p->n_alcunhas * sizeof(char*));

        if(p->alcunhas==NULL)
        return NULL;

		for (int i = 0; i < n_alcunhas; i++) {
			p->alcunhas[i] = (char*)malloc((strlen(alcunhas[i]) + 1) * (sizeof(char)));
            if (p->alcunhas[i]==NULL)
            return NULL;
            strcpy(p->alcunhas[i], alcunhas[i]);
		}
	}
	return p;
}

colecao *colecao_nova(const char *tipo_ordem){
	colecao* c;
	c = (colecao*)malloc(sizeof(colecao));
    if(c == NULL)
    return NULL;
	strcpy(c->tipo_ordem, tipo_ordem);

	c->tamanho = 0;
	c->plantas = NULL;
	return c;
}
int planta_insere(colecao *c, planta *p)
{
    if(c==NULL || p==NULL)
    return -1;
	int flag =0;
	//Verificao de existencia de planta na colecao
	if (c->tamanho != 0) {
		for (int i = 0; i < c->tamanho; i++) {
			//Se a planta existe na colecao
			if (strcmp(c->plantas[i]->ID, p->ID) == 0) {
				c->plantas[i]->n_sementes = c->plantas[i]->n_sementes + p->n_sementes; //Numero de sementes.
				//Alcunhas
				for (int j = 0; j < p->n_alcunhas; j++) {
					for (int k = 0; k < c->plantas[i]->n_alcunhas; k++) {
						flag = 0;
						char s1[20], s2[20];
						strcpy(s1, c->plantas[i]->alcunhas[k]);
						strcpy(s2, p->alcunhas[j]);
						if (strcmp(s1, s2) == 0){
							flag = 1;
							break;
						}
					}
					if (flag == 0) {
						c->plantas[i]->n_alcunhas++;
						c->plantas[i]->alcunhas = (char**)realloc(c->plantas[i]->alcunhas, c->plantas[i]->n_alcunhas * sizeof(char*));
                        if(c->plantas[i]->alcunhas == NULL)
                        return -1;
						c->plantas[i]->alcunhas[c->plantas[i]->n_alcunhas - 1] = (char*)malloc((strlen(p->alcunhas[j]) + 1) * sizeof(char));
                        if(c->plantas[i]->alcunhas[c->plantas[i]->n_alcunhas - 1] == NULL)
                        return -1;
						strcpy(c->plantas[i]->alcunhas[c->plantas[i]->n_alcunhas - 1], (p->alcunhas[j]));
					}
				}
				return 1;
				break;
			}

		}
	}
	c->tamanho++;
	c->plantas = (planta**)realloc(c->plantas, c->tamanho * sizeof(planta*));
    if(c->plantas == NULL)
    return -1;
	c->plantas[c->tamanho - 1] = p;
	colecao_reordena(c, c->tipo_ordem);
	return 0;
}
int colecao_tamanho(colecao *c)
{
	if (c == NULL) {
		return -1;
	}
	else return c->tamanho;
}

colecao *colecao_importa(const char *nome_ficheiro, const char *tipo_ordem)
{
	FILE *fp;
	colecao *c;
	planta *p;
	char linha[500], *id, *nome_cientifico,*parte, **alcunha;
	int sementes, nalcunhas = 0, aux, reor;

	fp = fopen(nome_ficheiro, "r");

	if (fp == NULL){
		return NULL;
	} 

	c = colecao_nova(tipo_ordem);

    if(c==NULL)
    return NULL;

	while (fgets(linha, 500, fp)!=NULL){
		id = strtok(linha, ",");
		nome_cientifico = strtok(NULL, ",");
		sementes = atoi(strtok(NULL, ","));
		parte = strtok(NULL, ",");
		nalcunhas = 0;

		alcunha = (char**) malloc(sizeof(char*));

        if(alcunha ==NULL)
        return NULL;
				
		while (parte != NULL){
			alcunha = (char**)realloc(alcunha, (nalcunhas + 1) * sizeof(char*));

            if(alcunha ==NULL)
            return NULL;

			alcunha[nalcunhas] = (char*)malloc((strlen(parte) + 1) * sizeof(char));

            if(alcunha[nalcunhas] == NULL)
            return NULL;

			strcpy(alcunha[nalcunhas], parte);


			parte = strtok(NULL, ",");
			nalcunhas++;
		}
		p = planta_nova(id, nome_cientifico, alcunha, nalcunhas, sementes);
		aux = planta_insere(c, p);

		if (aux == -1)
        return NULL;
        if (p==NULL)
        return NULL;
    }

	reor = colecao_reordena(c, c->tipo_ordem);
    if(reor == -1)
    return NULL;
	

	fclose(fp);
	return c;
}
planta *planta_remove(colecao *c, const char *nomep)
{
	int indice, flag=0;
    planta *aux;

    if (c == NULL)
	return NULL;

	for (int i = 0; i < c->tamanho; i++){
        if (strcmp(c->plantas[i]->nome_cientifico, nomep)==0){
			flag=1;
			break;
        }
    }
	if(flag==0)
	return NULL;

    if(flag==1){
		for (int i = 0; i < c->tamanho; i++){
			if (strcmp(c->plantas[i]->nome_cientifico, nomep)==0){
				indice = i;
				break;
			}
		}

		aux = c->plantas[indice];
		for (int i = indice; i < (c->tamanho-1); i++){
			c->plantas[i]=c->plantas[i+1];
		}
		
		c->tamanho--;
	
		return aux;
	}
	return NULL;
}


int planta_apaga(planta *p)
{
	if (p == NULL)
		return -1;
		
	for (int i = 0; i < p->n_alcunhas; i++){
		free(p->alcunhas[i]);
	}
	free(p->alcunhas);
	free(p);

	return 0;
}
int colecao_apaga(colecao *c)
{
	if(c==NULL)
	return -1;
	
	for (int i = 0; i < c->tamanho; i++){
		planta_apaga(c->plantas[i]);
	}
	free(c);
	return 0;
	
}
int *colecao_pesquisa_nome(colecao *c, const char *nomep, int *tam)
{
    int *resultado, cont = 0, flag = 0;

    if (c == NULL)
	return 0;

	for (int i = 0; i < c->tamanho; i++){
		for (int j = 0; j < c->plantas[i]->n_alcunhas; j++){
            if (strstr(c->plantas[i]->nome_cientifico, nomep) != NULL || strstr(c->plantas[i]->alcunhas[j], nomep) != NULL){
                flag=1;
            }
        }
    }

	if (flag == 0)
	return NULL;
	
    resultado = (int*)malloc(sizeof(int));
    if(resultado == NULL)
    return NULL;

   	for (int i = 0; i < c->tamanho; i++){
		for (int j = 0; j < c->plantas[i]->n_alcunhas; j++){
            if (strstr(c->plantas[i]->nome_cientifico, nomep) != NULL || strstr(c->plantas[i]->alcunhas[j], nomep) != NULL){
                resultado[cont] = i;    
             	cont++;
				resultado = (int*)realloc(resultado, (cont)*sizeof(int));

    			if (resultado == NULL)
                return NULL;
                
            	continue;
            }
        }
    }
   	*tam = cont;
	return resultado;
}
int colecao_reordena(colecao *c, const char *tipo_ordem)
{
	planta *temp;
	char **ori;
    int flag = 0;

	if (c==NULL)
	return -1;

    ori = (char**)malloc(c->tamanho * sizeof(char*));

    if(ori == NULL)
    return -1;

	for (int i = 0; i < c->tamanho; i++){
        ori[i] = (char*)malloc(strlen((c->plantas[i]->ID) + 1) * sizeof(char));
        if(ori[i]==NULL)
        return -1;
		strcpy(ori[i], c->plantas[i]->ID);
	} 
	
	if (strcmp(tipo_ordem, "nome")==0){
		for (int i = 0; i < c->tamanho; i++){
			for (int j = i+1; j < c->tamanho; j++){
				if (strcmp(c->plantas[i]->nome_cientifico, c->plantas[j]->nome_cientifico)>0){
					temp = c->plantas[i];
            		c->plantas[i] = c->plantas[j];
            		c->plantas[j] = temp;	
				}
			}
		}
		for (int k = 0; k < c->tamanho; k++){
			if (strcmp(c->plantas[k]->ID, ori[k])==0)
			flag = 1;
		}
        if(flag==1)
        return 0;
		
		return 1;
	}

	if (strcmp(tipo_ordem, "id")==0){
		for (int i = 0; i < c->tamanho; i++){
			for (int j = i+1; j < c->tamanho; j++){
				if (strcmp(c->plantas[i]->ID, c->plantas[j]->ID)>0){
					temp = c->plantas[i];
            		c->plantas[i] = c->plantas[j];
            		c->plantas[j] = temp;

				}
			}
		}
		for (int k = 0; k < c->tamanho; k++){
			if (strcmp(c->plantas[k]->ID, ori[k])==0)
			flag = 1;
		}
		if(flag == 1)
        return 0;

		return 1;
	}
	return 1;
}    