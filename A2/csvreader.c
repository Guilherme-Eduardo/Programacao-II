#include <stdio.h>
#include <stdlib.h>
#include "io.h"

#define SUMARIO 1
#define MOSTRAR 2
#define FILTROS 3
#define DESCRICAO_DOS_DADOS 4
#define ORDENACAO 5
#define SELECAO 6
#define DADOS_FALTANTES 7
#define SALVAR_DADOS 8
#define FIM 9

int main(int argc, char **argv) {
    if (argc != 2 ) {
        fprintf (stderr, "Forma de uso: ./csvreader.c <arquivo.csv>\n");
        return 1;
    } 

    struct arq_csv *csv;    
    int menu = -1;

    csv = abreArquivoCsv (argv[1]);
    csv = verificaCsv (csv); 
    
    if (!csv){
        fprintf (stderr, "Erro ao abrir ao arquivo\n");
        return 0;    }    

    while (menu != 9) {
        menu = menuPrincipal ();
        switch (menu) {
            case SUMARIO:
                imprimeSumario (csv);
                break;
            case MOSTRAR:
                exibe (csv);
                break;
            case FILTROS:
                filtros(csv);
                break;
            case DESCRICAO_DOS_DADOS:
                descricao_de_dados(csv);
                break;
            case ORDENACAO:
                ordenacao (csv);
                break;
            case SELECAO:
                selecao (csv);
                break;
            case DADOS_FALTANTES:
                DadosFaltantes(csv);
                break;
            case SALVAR_DADOS:
                salvarDados(csv);
                break;
            case FIM:
                break;
        }
    }   
    
    //destroiTabela (tab, numLinha);
    //encerra arquivo;
    
    return 0;
}
       
