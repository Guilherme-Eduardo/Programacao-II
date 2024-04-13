#ifndef IO_H
#define IO_H

#include <stdio.h>

struct arq_csv;
struct dados_t;


struct arq_csv* abreArquivoCsv (char *nome) ;
struct arq_csv* verificaCsv (struct arq_csv *csv);
struct arq_csv* contaColuna(struct arq_csv *csv);
void exibe (struct arq_csv *csv);
void imprimeSumario (struct arq_csv *csv);
int menuPrincipal();


void deletaCsv (struct arq_csv *csv);
void imprimeMatriz(struct arq_csv *csv, char ***matriz, unsigned long *bitmapColunas);
char*** copiaArquivo (struct arq_csv *csv);
void descricao_de_dados (struct arq_csv *csv);
void selecao (struct arq_csv *csv);
void ordenacao (struct arq_csv *csv);
void DadosFaltantes (struct arq_csv *csv);
void salvarDados (struct arq_csv *csv);
void filtros (struct arq_csv *csv);



#endif

