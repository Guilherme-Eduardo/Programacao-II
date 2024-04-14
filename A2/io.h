#ifndef IO_H
#define IO_H

#include <stdio.h>

/************************* structs *************************/

struct arq_csv {
    FILE *arquivo;
    unsigned long linhas;
    unsigned long colunas;
    char *tipos;
    unsigned long pos5;
    unsigned long *sizes;
    char ***matriz;
};

struct dados_t {
    unsigned long cont;
    float media;
    float desvio;
    float mediana;
    char *moda;
    unsigned long qtdModa;
    char *min;
    char *max;
    char **vetorDados;
};

/*Cria toda a estrutura da arquivo/tabela e retorna um ponteiro FILE*/
struct arq_csv* abreArquivoCsv (char *nome) ;

/*Funcao responsavel por verificar as principais informacoes do arquivo csv*/
/*e armazena-las na struct arq_csv*/
struct arq_csv* verificaCsv (struct arq_csv *csv);

/*retorna o numero de colunas do arquivo .csv*/
struct arq_csv* contaColuna(struct arq_csv *csv);

/*Funcao responsavel por exibir o arquivo na saida padrao*/
void exibe (struct arq_csv *csv);

void imprimeSumario (struct arq_csv *csv);

/*Imprime as as funcoes disponiveis para o usuario selecionar*/
int menuPrincipal();

/*Libera memoria da struct que armazena os dados do arquivo csv*/
void deletaCsv (struct arq_csv *csv);

/*Imprime a Matriz na tela de forma tabulada e alinhada a direita*/
void imprimeMatriz(struct arq_csv *csv, char ***matriz, unsigned long *bitmapColunas);

/*Trasfere os dados do arquivo csv para uma matriz na memoria*/
char*** copiaArquivo (struct arq_csv *csv);

/*Funcao responsavel por descrever as principais informacoes de uma coluna*/
void descricao_de_dados (struct arq_csv *csv);

/*Possibilita o usuario escolher qual coluna quer imprimir*/
void selecao (struct arq_csv *csv);
void ordenacao (struct arq_csv *csv);

/*Ordena determinada coluna de ordem ascendente ou decrescente*/
void DadosFaltantes (struct arq_csv *csv);

/*Os dados que estao em memorias sao salvos em um arquivo csv*/
void salvarDados (struct arq_csv *csv);

/*Filtra coluna conforme escolha do usuario*/
void filtros (struct arq_csv *csv);

#endif

