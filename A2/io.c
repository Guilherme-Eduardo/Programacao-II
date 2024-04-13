#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "io.h"

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

// Cria toda a estrutura da arquivo/tabela e retorna um ponteiro FILE*
struct arq_csv *abreArquivoCsv(char *nome) {
    if (!nome) {
        fprintf(stderr, "Impossivel abrir arquivo\n");
        return NULL;
    }

    struct arq_csv *csv;
    csv = (struct arq_csv *)malloc(sizeof(struct arq_csv));
    if (!csv) {
        fprintf(stderr, "Erro ao alocar memoria para csv\n");
        return NULL;
    }

    csv->arquivo = fopen(nome, "r");
    if (!csv->arquivo) {
        fprintf(stderr, "Erro ao ler o arquivo csv\n");
        return NULL;
    }

    csv->linhas = 0;
    csv->colunas = 0;
    csv->sizes = NULL;
    csv->tipos = NULL;
    csv->matriz = NULL;
    return csv;
}

// Funcao que limpa o buffer de memoria para as leituras com scanf
void limparBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void pressioneEnterParaContinuar() {
    limparBuffer();
    printf("Pressione ENTER para continuar\n");
    int tecla;
    do {
        tecla = getchar();
    } while (tecla != '\n');
}

// Funcao que conta os digitos de um valor para ser usado na tabulacao da coluna que informa as linhas [0 - csv->linhas -1]
int contarDigitos(int numero) {
    int contador = 0;
    // Trata o caso especial de 0
    if (numero == 0) {
        return 1;
    }
    // Conta os dígitos enquanto o número não for 0
    while (numero != 0) {
        numero = numero / 10;
        contador++;
    }
    return contador;
}

// Funcao responsavel por separar as palavras dependendo de um char separador
char *separa(char *linha, char separador) {
    if (!linha)
        return NULL; // Se a linha passada por parametro já for nula

    char *posSeparador;
    posSeparador = strchr(linha, separador);

    if (!posSeparador) { // Se nao encontrar a virgula, verifica se nao chegou na ultima parte a ser lida (antes do \)
        if (strlen(linha))
            return linha;
        else
            return NULL;
    }
    *posSeparador = '\0'; // Substitui o separador por um '\0' que indicara o final da string
    return linha;
}


//Verifica se a coluna eh de string ou Numerica [S] ou [N]
void verificaTipoColuna (struct arq_csv *csv) {
    if (!csv) {
        fprintf (stderr, "Erro ao abrir arquivo csv\n");
        exit(1);
    }

    for (int i = 0; i < csv->colunas; i++) {
        csv->tipos[i] = 'N';
    }

    for (int i = 1; i < csv->linhas; i++) {
        for (int j = 0; j < csv->colunas; j++) {
            if (strlen (csv->matriz[i][j]) > 0)
                if (atof (csv->matriz[i][j]) == 0)
                    csv->tipos[j] = 'S';
        }
    }
}

//Funcao responsavel por identificar qual eh a coluna (matriz[][coluna]) solicitada pelo usuario (var)
unsigned long encontraColuna (struct arq_csv *csv, char *var) {
    unsigned long coluna = 0;

    for (int i = 0; i < csv->colunas; i++) {
        if (strcmp(csv->matriz[0][i], var) == 0) {
            coluna = i;
            break;
        }        
    }
    return coluna;
}

// Funcao responsavel por formatar impressao alinhado a direita
char *formata(char **formatacao, unsigned long tamanhos,
              unsigned long posVetor, char *linha) {

    int tam, i;
    char *nan = "NaN"; // quando a variavel recebida por 'linha' for nula, será retornada o NaN
    if (!strlen(linha))
        tam = tamanhos - strlen(nan); // verifica a posicao onde serao preenchidos espaços '_'
    else
        tam = tamanhos - strlen(linha);

    for (i = 0; i < tam; i++) {
        formatacao[posVetor][i] = ' ';
    }
    if (!strlen(linha))
        strcpy(formatacao[posVetor] + tam, nan); // caso seja invalido o valor, sera preenchido com Nan no final do vetor
    else
        strcpy(formatacao[posVetor] + tam, linha); // Apos os espacos, sera preenchido com a palavra informada por 'linha'
    return formatacao[posVetor];
}

// Funcao responsavel por verificar o tamanho da maior string de cada coluna
// 5 primeiras linhas e 5 ultimas linhas do arquivo
unsigned long *tamColunas(char ***copia, struct arq_csv *csv) {
    unsigned long *tam;
    int i, j;
    tam = malloc(csv->colunas * sizeof(unsigned long));

    // Inicializa todos os tamanhos com 0
    for (j = 0; j < csv->colunas; j++) {
        tam[j] = 0;
    }

    if (csv->linhas > 10) {
        // Verifica as 5 primeiras linhas
        for (i = 0; i < 6; i++) {
            for (int j = 0; j < csv->colunas; j++) {
                if (strlen(copia[i][j]) > tam[j])
                    tam[j] = strlen(copia[i][j]);
            }
        }

        // verifica as ultimas 5 linhas do arquivo
        for (i = csv->linhas - 5; i < csv->linhas; i++) {
            for (int j = 0; j < csv->colunas; j++) {
                if (strlen(copia[i][j]) > tam[j])
                    tam[j] = strlen(copia[i][j]);
            }
        }
    }
    else {
        for (i = 0; i < csv->linhas; i++) {
            for (int j = 0; j < csv->colunas; j++) {
                if (strlen(copia[i][j]) > tam[j])
                    tam[j] = strlen(copia[i][j]);
            }
        }
    }
    return tam;
}

//Cria a variavel** que receberá o tamanho de cada coluna para que seja inserido espacos e a palavra indicada
char **criaFormatacao(struct arq_csv *csv, unsigned long *sizes) {
    if (!csv) {
        fprintf (stderr, "Erro ao acessar arquivo csv.\n");
        exit(1);
    }
    char **formatacao = (char **)malloc(csv->colunas * sizeof(char *));
    if (!formatacao)
        return NULL;

    for (int i = 0; i < csv->colunas; ++i) {
        formatacao[i] = (char *) malloc((sizes[i] + 1) * sizeof(char)); // + 1 para que seja ins

        if (!formatacao[i]) {
            // Tratar erro de alocação
            for (int j = 0; j < i; ++j) {
                free(formatacao[j]);
            }
            free(formatacao);
            return NULL;
        }
        // Inicializa os vetores com zeros
        memset(formatacao[i], 0, sizes[i] + 1);
    }
    return formatacao;
}

//Funcao responsavel por criar um vetor com o tamanho de colunas
//Ajuda para escolher qual coluna sera imprimida (1 para sim  --- 0 para não)
unsigned long *criaBitmapColunas(struct arq_csv *csv) {
    unsigned long *bitmapCol = (unsigned long *)malloc(csv->colunas * sizeof(unsigned long));
    if (!bitmapCol) {
        fprintf(stderr, "Erro ao alocar memoria para o bitmapCol\n");
        return 0;
    }

    // Inicializa o bitmap
    for (int i = 0; i < csv->colunas; i++) {
        bitmapCol[i] = 1;
    }
    return bitmapCol;
}

// Funcao faz a copia do arquivo csv para uma matriz***
char ***copiaArquivo(struct arq_csv *csv) {
    if (!csv || !csv->arquivo || csv->linhas <= 0 || csv->colunas <= 0) {
        fprintf(stderr, "Erro na copia do arquivo\n");
        return NULL;
    }
    rewind(csv->arquivo);
    char linha[1024] = {0};
    char *keeper;
    char ***matriz = (char ***)malloc((csv->linhas) * sizeof(char **));
    int colunas, numLinhas = 0;

    while (fgets(linha, sizeof(linha), csv->arquivo) != NULL) { // Comeca a leitura do arquivo csv
        if (linha[strlen(linha) - 1] == '\n') { // Remove o \n da linha
            linha[strlen(linha) - 1] = '\0';
        }
        matriz[numLinhas] = (char **)malloc(csv->colunas * sizeof(char *));
        if (matriz[numLinhas] == NULL) {
            fprintf(stderr, "Erro no arquivo CSV\n");
            return NULL;
        }

        colunas = 0;
        keeper = separa(linha, ','); // Encontra a primeira palavra antes até o separador
        do {
            matriz[numLinhas][colunas] = strdup(keeper); // Copia essa palavra encontrada para a nova matriz criada
            if (!matriz[numLinhas][colunas])
                fprintf(stderr, "Erro de alocação de memória\n");
            colunas++;
            keeper += strlen(keeper) + 1;
        } while ((keeper = separa(keeper, ',')) != NULL);

        numLinhas++;
    }
    return matriz;
}

// Função para gravar uma matriz em um arquivo CSV dependendo do bitmap de colunas
void gravarMatriz(char ***matriz, unsigned long *bitMapCol, int linhas, int colunas, const char *nomeArquivo) {
    // Abre o arquivo para escrita
    FILE *arquivo = fopen(nomeArquivo, "w");
    if (arquivo == NULL) {
        fprintf(stderr, "Erro ao abrir o arquivo %s\n", nomeArquivo);
        exit(1);
    }

    // Escreve os dados da matriz no arquivo
    for (int i = 0; i < linhas; i++) {
        int colunas_gravadas = 0;
        for (int j = 0; j < colunas; j++) {
            if (bitMapCol[j] == 1) {
                // Adiciona uma vírgula se não for a primeira palavra da linha
                if (colunas_gravadas > 0) {
                    fprintf(arquivo, ",");
                }
                // Escreve a palavra atual
                fprintf(arquivo, "%s", matriz[i][j]);
                colunas_gravadas++;
            }
        }
        // Adiciona uma quebra de linha ao final de cada linha
        fprintf(arquivo, "\n");
    }

    printf("Arquivo gravado com sucesso\n\n");
    // Fecha o arquivo
    fclose(arquivo);
}

void GravaMatrizNoArquivo(char ***matriz, unsigned long *bitmapCol, unsigned long linha, unsigned long coluna) {
    char nomeArquivo[1024];
    printf("Entre com o nome do arquivo: ");
    scanf("%s", nomeArquivo);
    gravarMatriz(matriz, bitmapCol, linha, coluna, nomeArquivo);
    printf("Arquivo gravado com sucesso\n");
}


// Funcao responsavel por imprimir pontos indicando um gap na impressao
void imprimePontos(char **formatacao, struct arq_csv *csv, unsigned long *size, int colunas, unsigned long *bitMapCol) {
    char *pontos;
    printf("...");
    int tam = contarDigitos(csv->linhas) - 3;
    for (int i = 1; i < tam; i++)
        printf(" ");

    for (int i = 0; i < colunas; i++) {
        if (bitMapCol[i] == 1) {
            pontos = formata(formatacao, size[i], i, "...");
            printf(" %s ", pontos);
        }
    }
    printf("\n");
}

void imprimeMatriz(struct arq_csv *csv, char ***matriz, unsigned long *bitmapColunas) {
    short int cabecalho = 0;
    char *palavra;
    int i, j;
    unsigned long *tam;
    char **formatacao;

    tam = tamColunas(matriz, csv);
    formatacao = criaFormatacao(csv, tam);

    for (i = 0; i < csv->linhas; i++) {
        if (!cabecalho) {
            for (j = 0; j < contarDigitos(csv->linhas); j++)
                printf(" ");
            cabecalho = 1;
        }
        else {
            if (i < 6) {
                printf("%d", i - 1);
                for (int j = 0; j < contarDigitos(csv->linhas) - 1; j++)
                    printf(" ");
            }
            else {
                int diff = contarDigitos(csv->linhas) - contarDigitos(i);
                printf("%d", i);
                for (int j = 0; j < diff; j++)
                    printf(" ");
            }
        }
        for (j = 0; j < csv->colunas; j++) {
            if (bitmapColunas[j] == 1) {
                palavra = formata(formatacao, tam[j], j, matriz[i][j]);
                printf(" %s ", palavra);
            }
        }
        printf("\n");
        if (i == 5 && csv->linhas > 10) {
            i = csv->linhas - 6;
            imprimePontos(formatacao, csv, tam, csv->colunas, bitmapColunas) ;
        }
    }
    printf("\n[%ld rows x %ld columns]\n\n", csv->linhas, csv->colunas);
}

void imprimeMatrizAscendente(struct arq_csv *csv, char ***matriz, unsigned long *vetorPosicoes) {
    short int cabecalho = 0;
    char *palavra;
    int i, j;
    unsigned long *tam;
    char **formatacao;
    unsigned long *bitmapCol;

    tam = tamColunas(matriz, csv);
    formatacao = criaFormatacao(csv, tam);
    bitmapCol = criaBitmapColunas(csv);


    for (i = 0; i < csv->linhas; i++) {
        if (!cabecalho) {
            printf("%ld", vetorPosicoes[i]);
            for (j = 0; j < contarDigitos(csv->linhas) - 1; j++)
                printf(" ");
            cabecalho = 1;
        }
        else {
            int diff = contarDigitos(csv->linhas) - contarDigitos(vetorPosicoes[i]);
            printf("%ld", vetorPosicoes[i]);
            for (int j = 0; j < diff; j++)
                printf(" ");            
        }
        for (j = 0; j < csv->colunas; j++) {
            if (bitmapCol[j] == 1) {
                palavra = formata(formatacao, tam[j], j, matriz[i][j]);
                printf(" %s ", palavra);
            }
        }
        printf("\n");
        if (i == 5 && csv->linhas > 10) {
            i = csv->linhas - 6;
            imprimePontos(formatacao, csv, tam, csv->colunas, bitmapCol);
        }
    }
}

void imprimeMatrizDecrescente(struct arq_csv *csv, char ***matriz, unsigned long *vetorPosicoes) {
    char *palavra;
    int i, j;
    unsigned long *tam;
    char **formatacao;
    unsigned long *bitmapCol;

    tam = tamColunas(matriz, csv);
    formatacao = criaFormatacao(csv, tam);
    bitmapCol = criaBitmapColunas(csv);

    printf("%ld", vetorPosicoes[0]);
    for (j = 0; j < contarDigitos(csv->linhas) - 1; j++)
        printf(" ");
    for (j = 0; j < csv->colunas; j++) {
        if (bitmapCol[j] == 1)
        {
            palavra = formata(formatacao, tam[j], j, matriz[0][j]);
            printf(" %s ", palavra);
        }
    }
    printf("\n");

    for (i = csv->linhas - 1; i >= 1; i--) {
        if (i < 6) {
            printf("%ld", vetorPosicoes[i]);
            for (int j = 0; j < contarDigitos(csv->linhas) - contarDigitos(vetorPosicoes[i]); j++)
                printf(" ");
        }
        else {
            int diff = contarDigitos(csv->linhas) - contarDigitos(vetorPosicoes[i]);
            printf("%ld", vetorPosicoes[i]);
            for (int j = 0; j < diff; j++)
                printf(" ");
        }

        for (j = 0; j < csv->colunas; j++) {
            if (bitmapCol[j] == 1) {
                palavra = formata(formatacao, tam[j], j, matriz[i][j]);
                printf(" %s ", palavra);
            }
        }
        printf("\n");
        if (i == csv->linhas - 5 && csv->linhas > 10) {
            i = 6;
            imprimePontos(formatacao, csv, tam, csv->colunas, bitmapCol);
        }
    }
}

// retorna o numero de colunas do arquivo .csv
struct arq_csv *contaColuna(struct arq_csv *csv) {
    char *keeper, linha[1024] = {0};

    rewind(csv->arquivo);
    fgets(linha, sizeof(linha), csv->arquivo); // Le o cabecalho(primeira linha)
    keeper = separa(linha, ',');

    if (!keeper) {
        fprintf(stderr, "Cabecalho invalido\n");
        return NULL;
    }
    // se nao tiver, na primeira linha?
    do {
        csv->colunas++;
        keeper += strlen(keeper) + 1; // Avança para a próxima palavra
    } while ((keeper = separa(keeper, ',')) != NULL);
    return csv;
}

// Funcao responsavel por verificar as principais informacoes do arquivo csv e armazena-las na struct arq_csv
struct arq_csv *verificaCsv(struct arq_csv *csv) {
    if (!csv) {
        fprintf(stderr, "Erro ao verificar aquivo: CSV NULL");
        return NULL;
    }

    unsigned long pos;
    char *keeper, linha[1024] = {0};

    rewind(csv->arquivo);
    csv = contaColuna(csv); //Obtem o numero de colunas
    rewind(csv->arquivo);

    csv->sizes = malloc(csv->colunas * sizeof(unsigned long));
    csv->tipos = malloc(csv->colunas * sizeof(char));
    unsigned long pos5[5] = {0};

    if (!csv->sizes || !csv->tipos) {
        fprintf(stderr, "Erro ao alocar memoria para sizes / tipos\n");
        return NULL;
    }

    // O tamanho da palavra de cada coluna inicia em zero
    for (int i = 0; i < csv->colunas; i++) {
        csv->sizes[i] = 0;
    }

    while (fgets(linha, sizeof(linha), csv->arquivo) != NULL) {
        if (linha[strlen(linha) - 1] == '\n') // Remove a \n da linha lida do arquivo
            linha[strlen(linha) - 1] = '\0';

        pos = 0;
        keeper = separa(linha, ',');

        if (keeper) {
            do {
                if (pos > csv->colunas) {
                    exit(1);
                }
                if (strlen(keeper) > csv->sizes[pos]) { //Comparacao para encontrar o tamanho da maior palavra da coluna
                    csv->sizes[pos] = strlen(keeper);
                }
                ++pos;
                keeper += strlen(keeper) + 1;
            } while ((keeper = separa(keeper, ',')) != NULL);
        }
        csv->linhas++;
        pos5[csv->linhas % 5] = ftell(csv->arquivo); // Armazena a posicao para imprimir a 5° ultima linha
    }
    if (csv->linhas > 10) {
        csv->pos5 = pos5[(csv->linhas % 5 + 1) % 5];
    }
    else
        csv->pos5 = 0;
    csv->matriz = copiaArquivo(csv);
    verificaTipoColuna(csv);
    return csv;
}

// imprime a tela do menu inicial e aguarda um valor do usuario
int menuPrincipal() {
    int menu;
    printf("\n1) Sumário do Arquivo\n"
           "2) Mostrar\n"
           "3) Filtros\n"
           "4) Descrição dos Dados\n"
           "5) Ordenação\n"
           "6) Seleção\n"
           "7) Dados Faltantes\n"
           "8) Salvar Dados\n"
           "9) Fim\n"
           "Digite sua opcao: ");
    scanf("%d", &menu);
    return menu;
}

void imprimeSumario(struct arq_csv *csv) {
    if (!csv) {
        fprintf(stderr, "Erro ao imprimir sumario\n");
        return;
    }

    int pos = 0;
    char *keeper, linha[1024];
    rewind(csv->arquivo);
    fgets(linha, sizeof(linha), csv->arquivo);
    keeper = separa(linha, ',');
    printf("\n");

    do {
        if (keeper[strlen(keeper) - 1] == '\n') { // Remove o char \n da linha
            keeper[strlen(keeper) - 1] = '\0';
        }
        printf("%s [%c]\n", keeper, csv->tipos[pos++]); // Imprime o nome do cabecalho e o tipo da variavel (string ou Num)

        keeper += strlen(keeper) + 1;
    } while ((keeper = separa(keeper, ',')) != NULL);

    printf("\n%ld variaveis encontradas\n\n", csv->colunas);
    pressioneEnterParaContinuar();
}

// Funcao responsavel por exibir o arquivo na saida padrao
void exibe(struct arq_csv *csv) {
    if (!csv)
        return;
    unsigned long *bitmapCol;
    bitmapCol = criaBitmapColunas(csv);
    imprimeMatriz(csv, csv->matriz, bitmapCol);
    pressioneEnterParaContinuar();
}

int igual (struct arq_csv *csv, char *valor1, char *valor2, unsigned long coluna) {
    if (!valor1 || !valor2) return 0;
    if (csv->tipos[coluna] == 'S') {
        if (strcmp(valor1, valor2) == 0) return 1;
        else return 0;
    }
    else {
        if (atof(valor1) == atof(valor2)) return 1;
        else return 0;
    }
}

int maior (struct arq_csv *csv, char *valor1, char *valor2, unsigned long coluna) {
    if (!valor1 || !valor2) return 0;
    if (csv->tipos[coluna] == 'S') {
        if (strcmp (valor1,valor2) > 0) return 1;
        else return 0;
    }
    else {
        if (atof(valor1) > atof(valor2)) return 1;
        else return 0;   
    }
}

int maiorIgual (struct arq_csv *csv, char *valor1, char *valor2, unsigned long coluna) {
    if (!valor1 || !valor2) return 0;
    if (csv->tipos[coluna] == 'S') {
        if (strcmp (valor1,valor2) >= 0) return 1;
        else return 0;
    }
    else {
        if (atof(valor1) >= atof(valor2)) return 1;
        else return 0;     
    }
}

int menor (struct arq_csv *csv, char *valor1, char *valor2, unsigned long coluna) {
    if (!valor1 || !valor2) return 0;
    if (csv->tipos[coluna] == 'S') {
        if (strcmp (valor1, valor2) < 0) return 1;
        else return 0;
    }
    else {
        if (atof(valor1) < atof(valor2)) return 1;
        else return 0;          
    }
}

int menorIgual (struct arq_csv *csv, char *valor1, char *valor2, unsigned long coluna) {
    if (!valor1 || !valor2) return 0;
    if (csv->tipos[coluna] == 'S') {
        if (strcmp (valor1, valor2) <= 0) return 1;
        else return 0;
    }
    else {
        if (atof(valor1) <= atof(valor2)) return 1;
        else return 0;         
    }
}

int diferente (struct arq_csv *csv, char *valor1, char *valor2, unsigned long coluna) {
    if (!valor1 || !valor2) return 0;
    if (csv->tipos[coluna] == 'S') {
        if (strcmp (valor1, valor2) != 0) return 1;
        else return 0;
    }
    else {
        if (atof(valor1) != atof(valor2)) return 1;
        else return 0;            
    }
}

int comparacao(struct arq_csv *csv, unsigned long var, int (*filtro)(struct arq_csv *csv, char *valor1, char *valor2, unsigned long coluna), char *elemento) {
    if (!csv) {
        fprintf (stderr, "Erro ao acessar o arquivo csv.\n");
        return 0;
    }

    char ***matrizCopia;
    char **formatacao;
    char *palavra;
    unsigned long *vetorLinha, *tam, *bitMapCol;
    unsigned long linhaFiltrada = 1;
    unsigned long k; 
    char tipoImpressao;

    tam = tamColunas(csv->matriz, csv);
    formatacao = criaFormatacao(csv, tam);
    bitMapCol = criaBitmapColunas(csv);

    for (int i = 1; i < csv->linhas; i++) {      
        if (filtro(csv, csv->matriz[i][var], elemento, var)) {           
            linhaFiltrada++; 
        }
                   
    }

    vetorLinha = (unsigned long*) malloc (linhaFiltrada * sizeof (unsigned long));
    matrizCopia = (char***) malloc (linhaFiltrada * sizeof (char**));
    
    for (int i = 0; i < linhaFiltrada; i++) {
        vetorLinha[i] = 0;
    }
    k  = 1;
    for (int i = 1; i < csv->linhas; i++) {      
        if (filtro(csv, csv->matriz[i][var], elemento, var)) {
            vetorLinha[k++] = i;
        }                
    }

    for (int i = 0; i < linhaFiltrada; i++) {
        matrizCopia[i] = (char **)malloc(csv->colunas * sizeof(char *));
        for (int j = 0; j < csv->colunas; j++) {
            matrizCopia[i][j] = strdup(csv->matriz[vetorLinha[i]][j]);
        }
    }


    for (int i = 0; i < linhaFiltrada; i++) {
        int diff = contarDigitos(csv->linhas) - contarDigitos(vetorLinha[i]);
        printf("%ld", vetorLinha[i]);
        for (int j = 0; j < diff; j++)
            printf(" ");
        for (int k = 0; k < csv->colunas; k++) {
            palavra = formata(formatacao, tam[k], k, csv->matriz[vetorLinha[i]][k]);
            printf(" %s ", palavra);
        }
        printf ("\n");
        if (i == 5 && linhaFiltrada > 10) {
            i = linhaFiltrada - 5;
            imprimePontos(formatacao, csv, tam, csv->colunas, bitMapCol);            
        }
    }


    printf("\n[%ld rows x %ld columns]\n", linhaFiltrada, csv->colunas);
    limparBuffer();
    do {
        printf("Deseja gravar um arquivo com as variaveis selecionadas? [S][N]: ");
        scanf(" %c", &tipoImpressao);
    } while (tipoImpressao != 'S' && tipoImpressao != 'N');

    if (tipoImpressao == 'S') {
        GravaMatrizNoArquivo(matrizCopia, bitMapCol, linhaFiltrada, csv->colunas);
    }

    limparBuffer();
    do {
        printf("Deseja descartar os dados originais? [S|N]: ");
        scanf(" %c", &tipoImpressao);
    } while (tipoImpressao != 'S' && tipoImpressao != 'N');

    if (tipoImpressao == 'S') {

        bitMapCol = criaBitmapColunas(csv);
        printf ("cheguei aqui1\n");
        csv->matriz = matrizCopia;
        printf ("cheguei aqui2\n");
        csv->linhas = linhaFiltrada;
            // destr2oiMatriz ();
    }
    imprimeMatriz(csv, csv->matriz, bitMapCol);
    pressioneEnterParaContinuar();
    return 1;
}

void filtros (struct arq_csv *csv) {
    char nomeColuna[1024], filtro[3], valor[1024];
    unsigned long coluna;

    printf ("Entre com a variavel: ");
    scanf ("%s", nomeColuna);
    printf ("Escolha um filtro ( == > >= < <= != ): ");
    scanf ("%s", filtro);
    printf ("Digite um valor: ");
    scanf ("%s", valor);

    coluna = encontraColuna(csv, nomeColuna);
    
    int (*funcao)(struct arq_csv *, char *, char *, unsigned long int) = NULL;


    if (strcmp(filtro, "==") == 0) {
        funcao = igual;
    }
    else if (strcmp(filtro, ">") == 0) {
        funcao = maior;
    }
    else if (strcmp(filtro, ">=") == 0) {
        //printf ("cheguei aqui aqui no filtro\n");
        funcao = maiorIgual;
    }
    else if (strcmp(filtro, "<") == 0) {
        funcao = menor;
    }
    else if (strcmp(filtro, "<=") == 0) {
        funcao = menorIgual;
    }
    else if (strcmp(filtro, "!=") == 0) {
        funcao = diferente;
    }
    //printf ("cheguei aqui 1\n");
    //printf ("Tamanho do filtro %ld\n", strlen(filtro));
    comparacao (csv, coluna, funcao, valor);
}


// Funcao responsavel por criar uma struct que obtem os dados de uma coluna determinada pelo usuario
struct dados_t *criaDado() {
    struct dados_t *dados;
    dados = (struct dados_t *)malloc(sizeof(struct dados_t));
    if (!dados)
    {
        fprintf(stderr, "Erro ao alocar memoria para var dados\n");
        return NULL;
    }
    dados->cont = 0;
    dados->media = 0;
    dados->desvio = 0;
    dados->mediana = 0;
    dados->moda = 0;
    dados->qtdModa = 0;
    dados->min = 0;
    dados->max = 0;
    dados->vetorDados = NULL;
    return dados;
}

// Funcao responsavel por ordenar uma matriz por meio de um vetor de posicoes
// Somente é feito a comparacao na matriz. A troca de posicoes é feito no vetor enumerado com as linhas
void merge(struct arq_csv *csv,unsigned long *vetorPos, int inicio, 
            int meio, int fim, char ***matriz, int coluna) {

    int i, j, k;
    int n1 = meio - inicio + 1; // Calcula o tamanho das duas metades
    int n2 = fim - meio;

    // Aloca memória para os vetores temporários
    unsigned long *esquerda = malloc(n1 * sizeof(unsigned long));
    unsigned long *direita = malloc(n2 * sizeof(unsigned long));

    // Copia as posições das linhas para os vetores temporários
    for (i = 0; i < n1; i++)
        esquerda[i] = vetorPos[inicio + i];
    for (j = 0; j < n2; j++)
        direita[j] = vetorPos[meio + 1 + j];

    i = 0;
    j = 0;
    k = inicio;

    // Combina e ordena as duas metades
    while (i < n1 && j < n2) {

        if (csv->tipos[coluna] == 'N') {
            if (atof(matriz[esquerda[i]][coluna]) <= atof(matriz[direita[j]][coluna])) {
                vetorPos[k] = esquerda[i];
                i++;
            }
            else {
                vetorPos[k] = direita[j];
                j++;
            }
            k++;
        }
        else {
            if (strcmp(matriz[esquerda[i]][coluna], matriz[direita[j]][coluna]) <= 0) {
                vetorPos[k] = esquerda[i];
                i++;
            }
            else {
                vetorPos[k] = direita[j];
                j++;
            }
            k++;
        }
    }
    while (i < n1) {
        vetorPos[k] = esquerda[i];
        i++;
        k++;
    }

    while (j < n2) {
        vetorPos[k] = direita[j];
        j++;
        k++;
    }

    free(esquerda);
    free(direita);
}

//Algoritmo de ordenacao do MergeSort
void mergeSort(struct arq_csv *csv, unsigned long *vetorPos, int inicio, int fim, char ***matriz, int coluna) {
    if (inicio < fim) {
        int meio = inicio + (fim - inicio) / 2;
        mergeSort(csv, vetorPos, inicio, meio, matriz, coluna);
        mergeSort(csv, vetorPos, meio + 1, fim, matriz, coluna);
        merge(csv, vetorPos, inicio, meio, fim, matriz, coluna);
    }
}



void mergeVetor(struct arq_csv *csv, char **vetor, int inicio, int meio, int fim, unsigned long coluna) {
    int i, j, k;
    int n1 = meio - inicio + 1;
    int n2 = fim - meio;

    // Cria vetores temporários para as strings
    char **L = malloc(n1 * sizeof(char*));
    char **R = malloc(n2 * sizeof(char*));

    // Copia os dados para os vetores temporários L[] e R[]
    for (i = 0; i < n1; i++)
        L[i] = vetor[inicio + i];
    for (j = 0; j < n2; j++)
        R[j] = vetor[meio + 1 + j];

    // Faz o merge dos vetores temporários de volta para o vetor principal
    i = 0; // Índice inicial do primeiro subarray
    j = 0; // Índice inicial do segundo subarray
    k = inicio; // Índice inicial do vetor mesclado
    while (i < n1 && j < n2) {
        if (csv->tipos[coluna] == 'S') {
            if (strcmp(L[i], R[j]) <= 0) {
                vetor[k] = L[i];
                i++;
            } else {
                vetor[k] = R[j];
                j++;
            }
            k++;
        }
        else {
                if (atof(L[i]) <= atof(R[j])){
                    vetor[k] = L[i];
                    i++;
                } else {
                    vetor[k] = R[j];
                    j++;
                }
            k++;
        }
    }

    // Copia os elementos restantes de L[], se houver
    while (i < n1) {
        vetor[k] = L[i];
        i++;
        k++;
    }

    // Copia os elementos restantes de R[], se houver
    while (j < n2) {
        vetor[k] = R[j];
        j++;
        k++;
    }

    // Libera os vetores temporários
    free(L);
    free(R);
}

void mergeSortVetor(struct arq_csv *csv, char **vetor, int inicio, int fim, unsigned long coluna) {
    if (inicio < fim) {
        // Encontra o ponto médio
        int meio = inicio + (fim - inicio) / 2;

        // Ordena a primeira e a segunda metade
        mergeSortVetor(csv, vetor, inicio, meio, coluna);
        mergeSortVetor(csv, vetor, meio + 1, fim, coluna);

        // Faz o merge das duas metades ordenadas
        mergeVetor(csv, vetor, inicio, meio, fim, coluna);
    }
}

//Funcao que realiza o somatorio da coluna solicitada ()
unsigned long somaColuna (struct arq_csv *csv, unsigned long coluna) {
	if (!csv) {
		fprintf (stderr, "Erro ao somar coluna de dados.\n");
		exit(1);
	}
	
	unsigned long somador = 0;

    if (csv->tipos[coluna] == 'S') {
        fprintf (stderr, "Coluna de strings [S].\n");
        return 0;
    }
	
	for (int i = 1; i < csv->linhas; i++) {
        if (strlen (csv->matriz[i][coluna]) > 0)
		    somador += atof(csv->matriz[i][coluna]);
	}

    //printf ("O resultado do soma da coluna %ld é: %ld\n", coluna, somador);
	return somador;
}

unsigned long somaLinhaInvalida (struct arq_csv *csv, unsigned long coluna) {
	if (!csv) {
		fprintf (stderr, "Erro ao somar coluna de dados.\n");
		exit(1);
	}
	
	unsigned long somador = 0;

	
	for (int i = 1; i < csv->linhas; i++) {
        if (strlen (csv->matriz[i][coluna]) != 0)
		    somador ++;
	}
    printf ("O resultado de qtd da coluna %ld é: %ld\n", coluna, somador);
	return somador;
}

//Funcao que retorna os valores de min e max de uma determinada coluna
void maxMinMedia (struct dados_t *dados, struct arq_csv *csv, unsigned long coluna) {
	if (!dados || !csv) {
		fprintf (stderr, "Erro ao calcular min e max.\n");
		exit(1);
	}

    if (csv->tipos[coluna] == 'S') {
        fprintf (stderr, "Coluna de strings [S].\n");
        return;
    }
    
    unsigned long somador;
    somador = 0;

	dados->min = strdup (csv->matriz[1][coluna]);
	dados->max = strdup (csv->matriz[1][coluna]); 

    for (int i = 1; i < csv->linhas; i++) {
        if (strlen(csv->matriz[i][coluna]) > 0) { // Verifica se eh um elemento valido
            
            if (csv->tipos[coluna] == 'N') {

                if (atof(csv->matriz[i][coluna]) < atof(dados->min)) // vefifica o menor
                    dados->min = strdup (csv->matriz[i][coluna]);

                if (atof(csv->matriz[i][coluna]) > atof(dados->max))// verifica o maior
                    dados->max = strdup (csv->matriz[i][coluna]);

                somador += atof(csv->matriz[i][coluna]);
                dados->cont++; 
            }
        }         
    }

    dados->media = (float)somador / dados->cont;
}

//Funcao responsavel pelo calculo da moda de uma determinada coluna
void calculaModa (struct dados_t *dados, struct arq_csv *csv) {
    if (!dados || !csv) {
        fprintf (stderr, "Erro na alocacao das variaveis dados/csv.\n");
        exit(1);
    }

    unsigned long i, j, somador;
    char *var;
    dados->moda = strdup (dados->vetorDados[0]);
    dados->qtdModa = 1;

    //Identifica a variavel e o numero de aparicoes
    for (i = 0; i < dados->cont; i++) {
        var = strdup (dados->vetorDados[i]);
        somador = 1;

        for (j = i + 1; j < dados->cont; j++) {
            if (strcmp(var, dados->vetorDados[j]) == 0)
            {
                somador++;
                if (somador > dados->qtdModa) {
                    dados->qtdModa = somador;
                    dados->moda = strdup (dados->vetorDados[i]);
                }
            }
            else {
                i = j - 1;
                somador = 1;
                break;
            }
        }
    }

}

//Calculo do desvio padrao
float calculaDesvio (struct dados_t *dados) {
    if (!dados) {
        fprintf (stderr, "Erro ao alocar memoria para dados\n");
        exit(1);
    }
    unsigned long i;
    float variancia, somador;
    variancia = somador = 0;

    for (i = 0; i < dados->cont; i++) {
        somador += pow (atof (dados->vetorDados[i]) - dados->media,2);
    }
    variancia = (float)somador / (dados->cont - 1);
    return sqrt (variancia);

}

//Calcula o valor da Mediana de uma determinada coluna
float calculaMediana (struct dados_t *dados) {
    if (!dados) {
        fprintf (stderr, "Erro ao alocar memoria para dados\n");
        exit(1);
    }
    unsigned long valor1, valor2;
    float resultado;
    if (dados->cont % 2 == 0) {        
        valor1 = atof (dados->vetorDados[dados->cont / 2]);
        valor2 = atof (dados->vetorDados[(dados->cont / 2) -1]);
        resultado = (float)(valor1 + valor2) / 2;
        return resultado;
    }
    else {
        return atof (dados->vetorDados[dados->cont / 2]);
    }
}

//Imprime todos os dados obtidos por uma determinada coluna
void imprimeDados (struct dados_t *dados, struct arq_csv *csv, char *var) {
	if (!dados) return;
    unsigned long coluna = 0;
	unsigned long i, j;
    i = j = 0;

    coluna = encontraColuna(csv, var);
    printf ("\nContador: %ld\n", dados->cont);

    if (csv->tipos[coluna] == 'S') {
        printf ("Moda: %s  %ld vezes\n", dados->moda, dados->qtdModa);
    }
    else {
        printf("Media: %.1f\n"
            "Desvio: %.1f\n"
            "Mediana: %.1f\n"
            "Moda: %s %ld vezes\n"
            "Min.: %s\n"
            "Max.: %s\n", dados->media, dados->desvio, dados->mediana, dados->moda, dados->qtdModa, dados->min, dados->max);
    }   

    printf ("Valores unicos: [\n");
	for (i = 0; i < dados->cont; i++) {
        printf ("%s, \n", dados->vetorDados[i]);        
        j = i + 1;
        while (j < dados->cont && strcmp(dados->vetorDados[i], dados->vetorDados[j]) == 0)    
            j++;                  
        i = j - 1;        
    }
    printf ("]\n\n");		
}


// Funcao responsavel por analisar o arquivo conforme a variavel fornecida pelo usuario
struct dados_t *analisaDadosNum(struct arq_csv *csv, char *var) {
    if (!csv->matriz || !var) {
        fprintf(stderr, "Erro ao analisar os dados\n");
        return NULL;
    }
    int somador = 0;
    unsigned long k = 0;
    unsigned long i, coluna = 0;    
    struct dados_t *dados;
    //unsigned long *vetorPos;


    dados = criaDado();
    coluna = encontraColuna (csv, var);
	maxMinMedia(dados, csv, coluna);

    dados->vetorDados = (char **) malloc(dados->cont * sizeof(char *));

    k = 0;
    for (i = 1; i < csv->linhas; i++) {
        if (strlen(csv->matriz[i][coluna]) > 0) {
            dados->vetorDados[k++] = strdup(csv->matriz[i][coluna]);
        }
    }

    //dados->cont = k;

    mergeSortVetor(csv, dados->vetorDados, 0 , dados->cont - 1, coluna);
    
    calculaModa (dados, csv);
    dados->desvio = calculaDesvio (dados);
    dados->mediana = calculaMediana (dados);
    return dados;
}

// Funcao responsavel por analisar o arquivo conforme a variavel fornecida pelo usuario
struct dados_t *analisaDadoString(struct arq_csv *csv, char *var) {
    if (!csv || !var) {
        fprintf(stderr, "Erro ao analisar os dados\n");
        return NULL;
    }
    int somador = 0;
    unsigned long k = 0;
    unsigned long i, linhas = 0, coluna = 0;    
    struct dados_t *dados;


    dados = criaDado();
    coluna = encontraColuna (csv, var);
    dados->cont = somaLinhaInvalida(csv, coluna);

    dados->vetorDados = (char **) malloc(dados->cont * sizeof(char *));

    k = 0;
    for (i = 1; i < csv->linhas; i++) {
        if (strlen(csv->matriz[i][coluna]) > 0) {
            dados->vetorDados[k++] = strdup(csv->matriz[i][coluna]);
        }
    }

    dados->cont = k;

    mergeSortVetor(csv, dados->vetorDados, 0 , dados->cont - 1, coluna);
    
    calculaModa (dados, csv);
    return dados;
}

void descricao_de_dados(struct arq_csv *csv) {
    char var[1024];
    unsigned long coluna;
    printf("Entre com a variavel: ");
    scanf("%s", var);
    struct dados_t *dados;

    coluna = encontraColuna(csv, var);

    if (csv->tipos[coluna] == 'N') {
        dados = analisaDadosNum(csv, var);
    }
    else {
         dados = analisaDadoString(csv, var);
    }    

    imprimeDados(dados, csv, var);
    pressioneEnterParaContinuar();
}

unsigned long* inverteVetor(unsigned long* vetorA, unsigned long linhas) {
    if (!vetorA) {
        fprintf(stderr, "Invalido. O vetor informado eh nulo.\n");
        return NULL;
    }

    unsigned long *vetorB = (unsigned long *)malloc(linhas * sizeof(unsigned long));
    if (!vetorB) return NULL;

    vetorB[0] = 0;
    for (unsigned long i = 1; i < linhas; i++) {
        vetorB[i] = vetorA[linhas - i ];
    }

    return vetorB;
}

void ordenacao(struct arq_csv *csv) {
    char var[1024];
    char tipoImpressao;
    short int posColuna = -1;
    char ***matrizCopia;
    unsigned long *vetorPosicoes;
    unsigned long *bitmapCol;

    bitmapCol = criaBitmapColunas(csv);
    matrizCopia = (char ***)malloc(csv->linhas * sizeof(char **));
    if (!matrizCopia) {
        fprintf(stderr, "Erro ao alocar memoria para MatrizCopia\n");
        return;
    }

    limparBuffer();
    while (posColuna == -1) { // Continua enquanto a posição não for encontrada
        printf("Entre com a variavel: ");
        scanf("%s", var);
        for (int i = 0; i < csv->colunas; i++) {
            if (strcmp(var, csv->matriz[0][i]) == 0) {
                posColuna = i;
                break;
            }
        }
    }
    limparBuffer();

    do {
        printf("Selecione uma opcao [A]scendente ou [D]escrescente: ");
        scanf(" %c", &tipoImpressao);
    } while (tipoImpressao != 'A' && tipoImpressao != 'D');

    vetorPosicoes = (unsigned long *)malloc(csv->linhas * sizeof(unsigned long));
    if (!vetorPosicoes) {
        fprintf(stderr, "Erro ao alocar vetor de Posicoes\n");
        return;
    }

    for (int i = 0; i < csv->linhas; i++) {
        vetorPosicoes[i] = i;
    }

    mergeSort(csv, vetorPosicoes, 1, csv->linhas - 1, csv->matriz, posColuna);

    // Imprime Ascendente
    if (tipoImpressao == 'A') {
        for (int i = 0; i < csv->linhas; i++) {
            matrizCopia[i] = (char **) malloc(csv->colunas * sizeof(char *));
            for (int j = 0; j < csv->colunas; j++) {
                matrizCopia[i][j] = strdup(csv->matriz[vetorPosicoes[i]][j]);
            }
        }
        imprimeMatrizAscendente(csv, matrizCopia, vetorPosicoes);
    }
    else if (tipoImpressao == 'D') {
        unsigned long *vetorB = inverteVetor(vetorPosicoes, csv->linhas);
        for (int i = 0; i < csv->linhas; i++) {
            matrizCopia[i] = (char **) malloc(csv->colunas * sizeof(char *));
            for (int j = 0; j < csv->colunas; j++) {
                matrizCopia[i][j] = strdup(csv->matriz[vetorB[i]][j]);
            }
        }

        imprimeMatrizAscendente(csv, matrizCopia, vetorB);
    }

    printf("[%ld rows x %ld columns]\n\n", csv->linhas, csv->colunas);

    limparBuffer();
    do {
        printf("Deseja gravar um arquivo com as variaveis selecionadas? [S][N]: ");
        scanf(" %c", &tipoImpressao);
    } while (tipoImpressao != 'S' && tipoImpressao != 'N');

    if (tipoImpressao == 'S') {
        GravaMatrizNoArquivo(matrizCopia, bitmapCol, csv->linhas, csv->colunas);
    }

    limparBuffer();
    do {
        printf("Deseja descartar os dados originais? [S|N]: ");
        scanf(" %c", &tipoImpressao);
    } while (tipoImpressao != 'S' && tipoImpressao != 'N');

    if (tipoImpressao == 'S') {
        csv->matriz = matrizCopia;
        // destr2oiMatriz ();
    }
   
    pressioneEnterParaContinuar();
}

void selecao(struct arq_csv *csv) {
    char tipoImpressao, var[1024];
    int somador = 0;
    unsigned long *bitmapCol;

    limparBuffer();
    printf("\nEntre com as variaveis que deseja selecionar (separadas por espaço): ");
    scanf("%[^\n]", var);
    var[strlen(var) + 1] = '\0';

    bitmapCol = criaBitmapColunas(csv);
    // tam = tamColunas (csv->matriz, csv);
    // formatacao = criaFormatacao(csv, tam);

    // Inicializa o bitmap
    for (int i = 0; i < csv->colunas; i++) {
        bitmapCol[i] = 0;
    }

    // Conta quantas palavras foram inseridas em 'var
    char *keeper = separa(var, ' ');
    do {
        for (int i = 0; i < csv->colunas; i++) {
            if (strcmp(keeper, csv->matriz[0][i]) == 0) {
                bitmapCol[i] = 1;
                break;
            }
        }
        somador++;
        keeper += strlen(keeper) + 1;
    } while ((keeper = separa(keeper, ' ')) != NULL);

    imprimeMatriz(csv, csv->matriz, bitmapCol);
    limparBuffer();
    do {
        printf("Deseja gravar um arquivo com as variaveis selecionadas? [S][N]: ");
        scanf(" %c", &tipoImpressao);
    } while (tipoImpressao != 'S' && tipoImpressao != 'N');

    if (tipoImpressao == 'S') {
        GravaMatrizNoArquivo(csv->matriz, bitmapCol, csv->linhas, csv->colunas);
    }
    pressioneEnterParaContinuar();
}

void listarRegistrosNan(struct arq_csv *csv) {
    char tipoImpressao;
    char *palavra;
    char ***copiaMatriz;
    unsigned long *bitMapCol;
    unsigned long contaLinhaNan = 1;
    unsigned long *vetorLinha;
    char **formatacao;
    unsigned long *tam;
    unsigned long k;

    tam = tamColunas(csv->matriz, csv);
    formatacao = criaFormatacao(csv, tam);
    bitMapCol = criaBitmapColunas(csv);


    //contar linhas NaN
    for (int i = 0; i < csv->linhas; i++) {
        for (int j = 0; j < csv->colunas; j++) {
            if (strlen(csv->matriz[i][j]) == 0) {
                contaLinhaNan++;                
                break;
            }
        }
    }

    vetorLinha = (unsigned long*) malloc (contaLinhaNan * sizeof (unsigned long));
    if (!vetorLinha) {
        fprintf (stderr, "Erro ao alocar memoria para o vetor de linhas.\n");
        exit(1);
    }

    for (int i = 0; i < contaLinhaNan; i++) {
        vetorLinha[i] = 0;
    }

    k = 1;
    for (int i = 1; i < csv->linhas; i++) {
        for (int j = 0; j < csv->colunas; j++) {
            if (strlen(csv->matriz[i][j]) == 0) {
                vetorLinha[k] = i;
                k++;            
                break;
            }
        }
    }

    for (int i = 0; i < contaLinhaNan; i++) {
        int diff = contarDigitos(csv->linhas) - contarDigitos(vetorLinha[i]);
        printf("%ld", vetorLinha[i]);
        for (int j = 0; j < diff; j++)
            printf(" ");
        for (int k = 0; k < csv->colunas; k++) {
            palavra = formata(formatacao, tam[k], k, csv->matriz[vetorLinha[i]][k]);
            printf(" %s ", palavra);
        }
        printf ("\n");
        if (i == 5 && contaLinhaNan > 10) {
            i = contaLinhaNan - 5;
            imprimePontos(formatacao, csv, tam, csv->colunas, bitMapCol);
            
        }
    }

    copiaMatriz = (char***) malloc (contaLinhaNan * sizeof(char**));
    if (!copiaMatriz) {
        fprintf (stderr, "Erro ao alocar matriz copia.\n");
        exit(1);
    }

    for (int i = 0; i < contaLinhaNan; i++) {
        copiaMatriz[i] = (char**) malloc (csv->colunas * sizeof (char*));
        if (!copiaMatriz[i]) {
            fprintf (stderr, "Erro ao alocar linha da matriz copia.\n");
            exit(1);
        }
    }

    for (int i = 0; i < contaLinhaNan; i++) {
        for (int j = 0; j < csv->colunas; j++) {
            copiaMatriz[i][j] = strdup (csv->matriz[vetorLinha[i]][j]);
        }
    }  

    printf("\n[%ld rows x %ld columns]\n", contaLinhaNan, csv->colunas);
    limparBuffer();
    do {
        printf("Deseja gravar um arquivo com as variaveis selecionadas? [S][N]: ");
        scanf(" %c", &tipoImpressao);
    } while (tipoImpressao != 'S' && tipoImpressao != 'N');

    if (tipoImpressao == 'S') {
        GravaMatrizNoArquivo(copiaMatriz, bitMapCol,contaLinhaNan, csv->colunas);
    }

    limparBuffer();
    do {
        printf("Deseja descartar os dados originais? [S|N]: ");
        scanf(" %c", &tipoImpressao);
    } while (tipoImpressao != 'S' && tipoImpressao != 'N');

    if (tipoImpressao == 'S') {

        bitMapCol = criaBitmapColunas(csv);
        printf ("cheguei aqui1\n");
        csv->matriz = copiaMatriz;
        printf ("cheguei aqui2\n");
        csv->linhas = contaLinhaNan;
            // destr2oiMatriz ();
    }
    imprimeMatriz(csv, csv->matriz, bitMapCol);
    pressioneEnterParaContinuar();
}

void SubstituirPelaMedia(struct arq_csv *csv) {
    char **mediaString;
    float *mediaFloat = (float*) malloc (csv->colunas * sizeof (float));
    unsigned long *somador = (unsigned long*) malloc (csv->colunas * sizeof (unsigned long));
    unsigned long *qtdLinhasInvalidas = (unsigned long*) malloc (csv->colunas * sizeof (unsigned long));
    unsigned long *bitmapCol = criaBitmapColunas(csv);


    mediaString = (char **)malloc(csv->colunas * sizeof(char *));

    for (int i = 0; i < csv->colunas; i++) {
        mediaString[i] = (char *)malloc(10 * sizeof(char)); // Tamanho arbitrário para a string
    }

    // Calcular a média de cada coluna
    for (int i = 0; i < csv->colunas; i++) {
        if (bitmapCol[i] == 1) {
            somador[i] = somaColuna(csv, i);
            qtdLinhasInvalidas[i] = somaLinhaInvalida(csv, i);
            mediaFloat[i] = (float)somador[i] / qtdLinhasInvalidas[i];
        }
    }

    // Transpor o vetor de médias para o vetor de strings
    for (int i = 0; i < csv->colunas; i++) {
        if (bitmapCol[i] == 1) {
            sprintf(mediaString[i], "%.1f", mediaFloat[i]); // Formatar o valor de média como uma string com uma casa decimal
            mediaString[i][strlen(mediaString[i])] = '\0'; // Adicionar o caractere nulo ao final da string
        }
    }

    // Preencher coordenadas vazias com a média da coluna
    for (int i = 0; i < csv->linhas; i++) {
        for (int j = 0; j < csv->colunas; j++) {
            if (strlen(csv->matriz[i][j]) == 0) {
                //snprintf(csv->matriz[i][j], 1024, "%.1f", media[j]);
                csv->matriz[i][j] = strdup (mediaString[j]);
            }
        }
    }

    bitmapCol = criaBitmapColunas(csv);
    imprimeMatriz(csv, csv->matriz, bitmapCol);

    // Liberar memória alocada
    /*free(mediaFloat);
    
    free(somador);
    free(qtdLinhasInvalidas);
    free(bitmapCol);*/

}

/*void SubstituirPelaMedia(struct arq_csv *csv) {
	struct dados_t **dados;
	unsigned long *bitmapCol;
	
	bitmapCol = criaBitmapColunas (csv);
    dados = (struct dados_t**) malloc(csv->colunas * sizeof(struct dados_t*));
	
	for (int i = 0; i < csv->colunas; i++) {
		dados[i] = criaDado();
	}	
	
	for (int i = 0; i < csv->colunas; i++) {
		if (csv->tipos[i] == 'N')
			bitmapCol[i] = 1;
		else
			bitmapCol[i] = 0;
	}

 	for (int i = 0; i < csv->colunas; i++) {
		if (bitmapCol[i] == 1) {
			dados[i] = analisaDados (csv, csv->matriz[0][i]);
		}
	}

    for (int i = 0; i < csv->linhas; i++) {
        for (int j = 0; j < csv->colunas; j++) {
            if (strlen(csv->matriz[i][j]) == 0) {
                sprintf(csv->matriz[i][j], "%.1f", dados[j]->media);;
                //strcpy(csv->matriz[i][j],(char*)dados[j]->media);
            }
        }
    }

    bitmapCol = criaBitmapColunas(csv);
    imprimeMatriz(csv, csv->matriz, bitmapCol);
 
}*/

void substituirValorValido (struct arq_csv *csv) {
    if (!csv) {
        fprintf (stderr, "Erro ao acessar arquivo csv.\n");
        exit(1);
    }

    unsigned long *bitMapCol = criaBitmapColunas (csv);
    
    for (int i = 0; i < csv->linhas; i++) {
        for (int j = 0; j < csv->colunas; j++) {
            if (strlen(csv->matriz[i][j]) == 0 && i + 1 != csv->linhas && strlen(csv->matriz[i+1][j]) > 0) {
                strcpy (csv->matriz[i][j],csv->matriz[i+1][j]);
            }
        }
    }

    imprimeMatriz(csv, csv->matriz, bitMapCol);
    pressioneEnterParaContinuar();


}

void removerRegistroNan(struct arq_csv *csv) {
    if (!csv) {
        fprintf (stderr, "Erro ao acessar arquivo csv.\n");
        exit(1);
    }

    unsigned long linhaValida = 0;
    int valida = 1;
    char ***matrizCopia;
    unsigned long i, j, k;
    unsigned long *bitMapCol = criaBitmapColunas(csv);

    for (i = 0; i < csv->linhas; i++) {
        valida = 1;
        for (j = 0; j < csv->colunas; j++) {
            if (strlen(csv->matriz[i][j]) == 0) {
                valida = 0;
                break;
            }
        }
        if (valida)
            linhaValida++;
    }

    matrizCopia = (char***) malloc (linhaValida * sizeof (char**));
    if (!matrizCopia) {
        fprintf (stderr, "Erro na alocacao da matriz.\n");
        exit(1);
    }

    k = 0;
    for (i = 0; i < csv->linhas; i++) {
        valida = 1;
        for (j = 0; j < csv->colunas; j++) {
            if (strlen(csv->matriz[i][j]) == 0) {
                valida = 0;
                break;
            }
        }
        if (valida) {
            matrizCopia[k] = (char **)malloc(csv->colunas * sizeof(char *));
            for (j = 0; j < csv->colunas; j++) {
                matrizCopia[k][j] = strdup(csv->matriz[i][j]);
            }
            k++;
        }
    }

    // Libera a matriz original
    /*for (i = 0; i < csv->linhas; i++) {
        for (j = 0; j < csv->colunas; j++) {
            free(csv->matriz[i][j]);
        }
        free(csv->matriz[i]);
    }
    free(csv->matriz);*/

    csv->matriz = matrizCopia;
    csv->linhas = linhaValida;

    // Liberar memória alocada para o bitmap de colunas (se necessário)
    // liberaBitmapColunas(bitMapCol);

    imprimeMatriz(csv,csv->matriz, bitMapCol);

}


void DadosFaltantes(struct arq_csv *csv) {
    int var;
    printf("\n1) Listar registros com NaN\n"
           "2) Substituir pela media\n"
           "3) Substituir pelo proximo valor valido\n"
           "4) Remover registros com NaN\n"
           "5) Voltar ao menu principal\n");
    scanf("%d", &var);

    
    switch (var) {
    case 1:
        listarRegistrosNan(csv);
        break;
    case 2:
        SubstituirPelaMedia(csv);
        break;
    case 3:
        substituirValorValido(csv);
    case 4:
        removerRegistroNan(csv);
    case 5:
        return;
    
    default:
        printf("Opcao invalida\n");
        break;
    }
}

//Salvo os dados que estão em memoria num arquivo csv
void salvarDados(struct arq_csv *csv) {
    unsigned long *bitMapCol;
    bitMapCol = criaBitmapColunas(csv);
    GravaMatrizNoArquivo(csv->matriz, bitMapCol, csv->linhas, csv->colunas);
}

/*void fim (csv) {
    //liberar memoria

}*/
