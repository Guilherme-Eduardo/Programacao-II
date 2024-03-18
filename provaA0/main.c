#include <stdlib.h>
#include <stdio.h>

/* Structs */

typedef struct teddy_machine{
    unsigned int id;
    unsigned int probability;

    struct teddy_machine *next;
    struct teddy_machine *previous;
} teddy_machine;


/* List functions */

teddy_machine* create_list (unsigned int machines){
    if (machines <= 0) return NULL;

	teddy_machine *primeiro;
	primeiro = malloc (sizeof (struct teddy_machine));
	if (!primeiro) return NULL;

    //instancia o primeiro elemento da lista
    primeiro->id = 1;
	primeiro->probability = 5;
	primeiro->next = primeiro;
	primeiro->previous = primeiro;	

    teddy_machine *aux;
    aux = primeiro;

    //incrementa os elementos da fila conforme a variavel machines
	for (int i = 2; i <= machines; ++i) {
	    teddy_machine *novo;
        novo = malloc (sizeof (struct teddy_machine));
        if (!novo) return NULL;

        novo->id = i;
        novo->probability = 5;
        aux->next = novo;
        primeiro->previous = novo;
        novo->next = primeiro;
        novo->previous = aux;
        aux = novo;	
	}
	return primeiro; 
 }

//Retorna o elemento da lista conforme posicao desejada pelo parametro (place)
teddy_machine* select_machine (teddy_machine *list, unsigned int place){
    teddy_machine *aux;
    aux = list;
    for (int i = 1; i <= place; ++i) {
        aux = aux->next;
    }
    return aux;
}

//Remove o elemento indicado e retorna o ponteiro para a primeira posicao da lista
teddy_machine* remove_machine(teddy_machine* list, teddy_machine* remove) {
    if (!list) return NULL;

    teddy_machine* topo, * aux;
    topo = list;

    // Caso o elemento a ser removido for o primeiro elemento da lista
    if (list == remove) {
        if (list->next == list) { // Quando somente sao esse elemento na fila (primeira posicao)
            free(list);
            return NULL;
        } else {
            list->previous->next = list->next;
            list->next->previous = list->previous;
            topo = list->next;
            free(list);
            return topo;
        }
    }

    // Remover o 'ultimo' elemento da fila
    if (list->previous == remove) { // Remover o último elemento
        list->previous = remove->previous;
        remove->previous->next = list;
        free(remove);
        return topo;
    }

    // Quando nao eh o primeiro elemento da fila
    aux = list->next;
    while (aux != list) {
        if (aux == remove) {
            aux->previous->next = aux->next;
            remove->next->previous = aux->previous;
            free(remove);
            break;
        }
        aux = aux->next;
    }

    return topo;
}

//Verifica se ainda há maquina com ursos
int maquinaComUrso (teddy_machine *list) {
    if (!list) return 0;
    return 1;
}

void destroy_list (teddy_machine *list){
    if (!list) return;
    teddy_machine *aux, *tmp;
    aux = list->next;
    tmp = aux;
    while (list != aux) {
        tmp = aux;
        aux = aux->next;
        free(tmp);
    }
    free (list);
}


/* Randomization functions */

unsigned int get_place(unsigned int machines){
    return rand() % machines;
}

unsigned int get_attempt(){
    return rand() % 100 + 1;
}

/* Printing functions */

void print_attempt(teddy_machine *machine, unsigned int attempt){

    if (attempt <= machine->probability) printf("O URSO DA MAQUINA %u [FOI] OBTIDO!\n", machine->id);
    else printf("O URSO DA MAQUINA %u [NAO FOI] OBTIDO!\n", machine->id);
}
void print_available_machines(teddy_machine *list){

    if (!list) printf("NAO HA MAQUINAS DISPONIVEIS!\n");
    else{
        teddy_machine *i = list;
        do {
            printf("PROBABILIDADE DA MAQUINA %u: %u\n", i->id, i->probability);
            i = (teddy_machine*) i->next;
        } while((i) && (i != list));
    }

}

//Seleciona o elemento da lista desejado a compara as probabilidades
teddy_machine *verificaTentativa (teddy_machine *list, unsigned int pos, unsigned int tentativa) {
    if (!list) return NULL;
    teddy_machine *aux;
    aux = select_machine (list, pos);
    print_attempt (aux, tentativa);
    if(tentativa <= aux->probability)
        list = remove_machine (list, aux);
    else aux->probability = aux->probability + 2;
    return list;
}


/* Main function */

int main(int argc, char *argv[]){
    unsigned int machines, rounds, seed;
    int rv = 0;

    if (argc != 4) rv = -1;
    else {
        machines = atoi(argv[1]);
        if (!machines) rv = -2;
        else {
            rounds = atoi(argv[2]);
            if (!rounds) rv = -3;
            else {
                seed = atoi(argv[3]);
                if (!seed) rv = -4;
            }
        }
    }

    if (rv){
        printf("USO: main [NR. DE MAQUINAS] [NR. DE RODADAS] [SEMENTE DE RAND.]\n");
        return rv;
    }

    teddy_machine *list = create_list(machines);
    srand(seed);

    unsigned int machine_place, machine_attempt;
    for (unsigned int r = 0; r < rounds; r++){
        printf("\n============================ ROUND %u ============================\n", r+1);
        machine_place = get_place(machines); /* Define a localização da máquina da rodada, não considera máquinas sem urso */
        machine_attempt = get_attempt(); /* Define a tentativa da rodada; se for menor ou igual à probabilidade da máquina selecionada, o urso foi pego */

        list = verificaTentativa (list, machine_place, machine_attempt);
        if (!maquinaComUrso(list)) break;
        print_available_machines(list);
        printf("==================================================================\n");
    }
    
    destroy_list (list);
    return 0;
}
