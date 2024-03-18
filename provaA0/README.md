Avaliação #0
Máquinas de Pelúcias
Você já esteve em algum shopping, loja, bar ou restaurante e encontrou aquelas máquinas
conhecidas de pegar pelúcias? Imagino que sim, porque elas estão se espalhando como fogo
em mato seco. Mesmo que pareça inacreditável, essas máquinas têm uma história que
começou em 1893, embora nas primeiras versões não fossem para pelúcias, mas sim para
doces.
Para além das clássicas “claw machines” (aquelas de garra, que sempre soltam a pelúcia
mesmo quando parecem perfeitamente encaixadas 😢), outros formatos dessas máquinas
surgiram recentemente. Entre esses novos formatos, estão as chamadas “máquinas de corte
do fio”, onde uma pelúcia grande é presa por um fio e o jogador tentar acertar uma lâmina no
mesmo, o cortando e obtendo a pelúcia como prêmio. Veja um exemplo da máquina descrita
abaixo.
Na avaliação #0, vamos simular um dia em uma loja com uma ou mais dessas máquinas. A
simulação consiste no seguinte:
● Uma ou mais máquinas são carregadas com pelúcias no início do dia em uma
determinada loja (a quantidade de máquinas é definida por uma variável chamada
“machines”);
● Durante um dia, múltiplas tentativas de obtenção das pelúcias ocorrerão - essas
tentativas são feitas em rodadas, e cada rodada representa uma pessoa usando uma
máquina (o número de rodadas durante um dia é definido em uma variável chamada
“rounds”);
● Uma máquina é representada por uma estrutura de dados que contém um identificador
e uma probabilidade da máquina permitir que o jogador ganhe a pelúcia;
○ O identificador é um número inteiro único, iniciado em 1 e com passo 1 (“id”);
○ A probabilidade é um número inteiro que varia entre 0 e 100 (note que, na
verdade, uma probabilidade é um número real entre 0 e 1, mas adotamos
números inteiros entre 0 e 100 para facilitar a geração pseudo aleatória), sendo
inicializada, por padrão, em 5 (“probability”).
● O conjunto de máquinas disponíveis na loja deve ser implementado por meio de uma
lista circular duplamente encadeada;
● A cada rodada, uma máquina aleatória que contenha uma pelúcia é selecionada
(representada por uma variável chamada “machine_place”), e a tentativa do jogador é
representada por um número entre 0 e 100 gerado aleatoriamente (este armazenado
em uma variável chamada “machine_attempt”);
● A variável “machine_place” indica o número de passos a serem dados na lista de
máquinas com pelúcias disponíveis. Essa variável representa o processo decisório do
jogador sobre em qual máquina jogar. O jogador sempre começa a inspeção pela
primeira máquina com pelúcia (a com o menor identificador) e as analisa linearmente
até tomar uma decisão (podendo analisar a mesma máquina mais de uma vez, se
necessário);
○ Exemplo 1
■ Total de máquinas: 4
■ Máquinas com pelúcias disp.: 1 → 2 → 3 → 4
■ machine_place: 2
■ Máquina selecionada: 3
○ Exemplo 2
■ Total de máquinas: 4
■ Máquinas com pelúcias disp.: 1 → 4
■ machine_place: 2
■ Máquina selecionada: 1
○ Exemplo 3
■ Total de máquinas: 4
■ Máquinas com pelúcias disp.: 2→3
■ machine_place: 3
■ Máquina selecionada: 3
● O jogador ganha a pelúcia se o valor armazenado na variável “machine_attempt” for
menor ou igual ao valor de probabilidade da máquina escolhida (“probability”);
○ Se o jogador [ganhou] a pelúcia, a máquina é considerada indisponível e deve
ser removida da lista de máquinas com pelúcias disponíveis;
○ Se o jogador [não ganhou] a pelúcia, a máquina continua na disponível e
presente na lista, e a sua probabilidade (“probability”) deve ser acrescida de
duas unidades (+2).
● Se, em uma determinada rodada, todas as máquinas ficarem sem pelúcias, a próxima
rodada não deve ser executada e o programa encerrado.
========================================================================
INSTRUÇÕES GERAIS
========================================================================
Você deve implementar a sua solução do problema a partir do arquivo main.c fornecido como
parte do trabalho:
● As funções que devem ser implementadas e utilizadas são: create_list,
select_machine, remove_machine e destroy_list, além de complementar a função
main;
● Você não deve modificar o protótipo das funções citadas;
● Você não deve modificar o nome das estruturas e variáveis já implementadas no arquivo
main.c;
● Você pode, se julgar necessário, implementar outras funções além das citadas.
Não deixe nenhuma exibição diferente daquelas já programadas nas funções “print_attempt” e
“print_available_machines” no arquivo de entrega:
● As funções de exibição devem ser executadas uma, e apenas uma, vez por rodada;
○ A função “print_attempt” deve ser sempre executada antes da função
“print_available_machines”.
● A função “print_available_machines” já está devidamente incluída no código;
● A função “print_attempt” deve ser alocada no código por você.
O programa deve ser usado da seguinte forma: main [NR. DE MAQUINAS] [NR. DE
RODADAS] [SEMENTE DE RAND.]
● Exemplo 1: ./main 10 10 303030 (10 máquinas, 10 rodadas e semente de aleatorização
303030)
● Exemplo 2: ./main 4 100 1000 (4 máquinas, 100 rodadas e semente de aleatorização
1000)
========================================================================
EXEMPLO
========================================================================
Para a seguinte configuração de execução ./main 10 10 303030 (10 máquinas, 10 rodadas e
semente de aleatorização 303030), o resultado (saída) esperado é:
============================ ROUND 1 ============================
O URSO DA MAQUINA 8 [NAO FOI] OBTIDO!
PROBABILIDADE DA MAQUINA 1: 5
PROBABILIDADE DA MAQUINA 2: 5
PROBABILIDADE DA MAQUINA 3: 5
PROBABILIDADE DA MAQUINA 4: 5
PROBABILIDADE DA MAQUINA 5: 5
PROBABILIDADE DA MAQUINA 6: 5
PROBABILIDADE DA MAQUINA 7: 5
PROBABILIDADE DA MAQUINA 8: 7
PROBABILIDADE DA MAQUINA 9: 5
PROBABILIDADE DA MAQUINA 10: 5
==================================================================
============================ ROUND 2 ============================
O URSO DA MAQUINA 3 [NAO FOI] OBTIDO!
PROBABILIDADE DA MAQUINA 1: 5
PROBABILIDADE DA MAQUINA 2: 5
PROBABILIDADE DA MAQUINA 3: 7
PROBABILIDADE DA MAQUINA 4: 5
PROBABILIDADE DA MAQUINA 5: 5
PROBABILIDADE DA MAQUINA 6: 5
PROBABILIDADE DA MAQUINA 7: 5
PROBABILIDADE DA MAQUINA 8: 7
PROBABILIDADE DA MAQUINA 9: 5
PROBABILIDADE DA MAQUINA 10: 5
==================================================================
============================ ROUND 3 ============================
O URSO DA MAQUINA 3 [NAO FOI] OBTIDO!
PROBABILIDADE DA MAQUINA 1: 5
PROBABILIDADE DA MAQUINA 2: 5
PROBABILIDADE DA MAQUINA 3: 9
PROBABILIDADE DA MAQUINA 4: 5
PROBABILIDADE DA MAQUINA 5: 5
PROBABILIDADE DA MAQUINA 6: 5
PROBABILIDADE DA MAQUINA 7: 5
PROBABILIDADE DA MAQUINA 8: 7
PROBABILIDADE DA MAQUINA 9: 5
PROBABILIDADE DA MAQUINA 10: 5
==================================================================
============================ ROUND 4 ============================
O URSO DA MAQUINA 10 [NAO FOI] OBTIDO!
PROBABILIDADE DA MAQUINA 1: 5
PROBABILIDADE DA MAQUINA 2: 5
PROBABILIDADE DA MAQUINA 3: 9
PROBABILIDADE DA MAQUINA 4: 5
PROBABILIDADE DA MAQUINA 5: 5
PROBABILIDADE DA MAQUINA 6: 5
PROBABILIDADE DA MAQUINA 7: 5
PROBABILIDADE DA MAQUINA 8: 7
PROBABILIDADE DA MAQUINA 9: 5
PROBABILIDADE DA MAQUINA 10: 7
==================================================================
============================ ROUND 5 ============================
O URSO DA MAQUINA 5 [NAO FOI] OBTIDO!
PROBABILIDADE DA MAQUINA 1: 5
PROBABILIDADE DA MAQUINA 2: 5
PROBABILIDADE DA MAQUINA 3: 9
PROBABILIDADE DA MAQUINA 4: 5
PROBABILIDADE DA MAQUINA 5: 7
PROBABILIDADE DA MAQUINA 6: 5
PROBABILIDADE DA MAQUINA 7: 5
PROBABILIDADE DA MAQUINA 8: 7
PROBABILIDADE DA MAQUINA 9: 5
PROBABILIDADE DA MAQUINA 10: 7
==================================================================
============================ ROUND 6 ============================
O URSO DA MAQUINA 10 [NAO FOI] OBTIDO!
PROBABILIDADE DA MAQUINA 1: 5
PROBABILIDADE DA MAQUINA 2: 5
PROBABILIDADE DA MAQUINA 3: 9
PROBABILIDADE DA MAQUINA 4: 5
PROBABILIDADE DA MAQUINA 5: 7
PROBABILIDADE DA MAQUINA 6: 5
PROBABILIDADE DA MAQUINA 7: 5
PROBABILIDADE DA MAQUINA 8: 7
PROBABILIDADE DA MAQUINA 9: 5
PROBABILIDADE DA MAQUINA 10: 9
==================================================================
============================ ROUND 7 ============================
O URSO DA MAQUINA 1 [NAO FOI] OBTIDO!
PROBABILIDADE DA MAQUINA 1: 7
PROBABILIDADE DA MAQUINA 2: 5
PROBABILIDADE DA MAQUINA 3: 9
PROBABILIDADE DA MAQUINA 4: 5
PROBABILIDADE DA MAQUINA 5: 7
PROBABILIDADE DA MAQUINA 6: 5
PROBABILIDADE DA MAQUINA 7: 5
PROBABILIDADE DA MAQUINA 8: 7
PROBABILIDADE DA MAQUINA 9: 5
PROBABILIDADE DA MAQUINA 10: 9
==================================================================
============================ ROUND 8 ============================
O URSO DA MAQUINA 5 [FOI] OBTIDO!
PROBABILIDADE DA MAQUINA 1: 7
PROBABILIDADE DA MAQUINA 2: 5
PROBABILIDADE DA MAQUINA 3: 9
PROBABILIDADE DA MAQUINA 4: 5
PROBABILIDADE DA MAQUINA 6: 5
PROBABILIDADE DA MAQUINA 7: 5
PROBABILIDADE DA MAQUINA 8: 7
PROBABILIDADE DA MAQUINA 9: 5
PROBABILIDADE DA MAQUINA 10: 9
==================================================================
============================ ROUND 9 ============================
O URSO DA MAQUINA 2 [NAO FOI] OBTIDO!
PROBABILIDADE DA MAQUINA 1: 7
PROBABILIDADE DA MAQUINA 2: 7
PROBABILIDADE DA MAQUINA 3: 9
PROBABILIDADE DA MAQUINA 4: 5
PROBABILIDADE DA MAQUINA 6: 5
PROBABILIDADE DA MAQUINA 7: 5
PROBABILIDADE DA MAQUINA 8: 7
PROBABILIDADE DA MAQUINA 9: 5
PROBABILIDADE DA MAQUINA 10: 9
==================================================================
============================ ROUND 10 ============================
O URSO DA MAQUINA 1 [NAO FOI] OBTIDO!
PROBABILIDADE DA MAQUINA 1: 9
PROBABILIDADE DA MAQUINA 2: 7
PROBABILIDADE DA MAQUINA 3: 9
PROBABILIDADE DA MAQUINA 4: 5
PROBABILIDADE DA MAQUINA 6: 5
PROBABILIDADE DA MAQUINA 7: 5
PROBABILIDADE DA MAQUINA 8: 7
PROBABILIDADE DA MAQUINA 9: 5
PROBABILIDADE DA MAQUINA 10: 9
==================================================================
############################################################################
IMPORTANTE: A ENTREGA DO TRABALHO DEVE CONTEMPLAR OS REQUISITOS
ELENCADOS NO PLANO DA DISCIPLINA. O NÃO CUMPRIMENTO DAS INSTRUÇÕES DE
ENTREGA ESTABELECIDAS RESULTARÁ EM DESCONTO DE NOTA.
############################################################################
