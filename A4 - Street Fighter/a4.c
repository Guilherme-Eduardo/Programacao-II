#include <stdio.h>
#include <stdlib.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include <allegro5/keyboard.h>
#include <allegro5/allegro_primitives.h>    

#define STEP 12
#define X_SCREEN 1280
#define Y_SCREEN 720    
#define MAX_FRAME 4
#define ROUNDS 3
#define GRAVITY 5.0
#define JUMP_STRENGTH -250
#define GROUND_LEVEL 420
#define CLOCK 90
#define LEFT 0
#define RIGHT 1
#define UP 2
#define DOWN 3
#define PUSH 4
#define KICK 5

/***********************************************************************************************/

/*Controle do jogador*/
struct joystick_t {
    unsigned char right;
    unsigned char left;
    unsigned char up;
    unsigned char down;
    unsigned char push;
    unsigned char kick;
};

/*Personagem do jogador*/
struct character_t {
    float x_sprite;
    float y_sprite;
    float x_display;
    float y_display;
    float maxX;
    float maxY;
    float currentFrame;
    float maxFrame;    
    int side;
    int life;
    int rounds_won;
    float vel_y;
	float vel_x;
    int jump;	
    float frame;
    int direction;
    struct joystick_t *joystick;
    ALLEGRO_BITMAP *fighter;
};

/* Cria a estrutura responsavel por receber os comandos do personagem */
struct joystick_t *joystick_create () {
    struct joystick_t *element = (struct joystick_t*) malloc (sizeof(struct joystick_t));
    if (!element) exit(1);
    element->right = 0;
    element->left = 0;
    element->up = 0;
    element->down = 0;
    element->push = 0;
    element->kick = 0;
    return element;
}
/* Funcao responsavel pela criacao do personagem e com as suas caracteristicas */
struct character_t* create_character (ALLEGRO_BITMAP *nome, unsigned int xS, unsigned int yS, unsigned int xD,
                                    unsigned int yD, unsigned int maxX, unsigned int maxY, unsigned int currentFrame, int direction) {

    struct character_t *player = (struct character_t*) malloc (sizeof(struct character_t));
    if (!player) exit(1);
    player->x_sprite = xS;
    player->y_sprite = yS;
    player->x_display = xD;
    player->y_display = yD;
    player->maxX = maxX;
    player->maxY = maxY;
    player->currentFrame = currentFrame;
    player->fighter = nome;    
    player->maxFrame = 4;
    player->side = 100;
    player->life = 600;
    player->rounds_won = 0;
    player->vel_y = 0;
    player->jump = 0;	
    player->frame = 0.f;
    player->joystick = joystick_create();
    player->direction = direction;
    return player;
}

void joystick_destroy (struct joystick_t *element){ free(element);}							//Implementação da função "joystick_destroy"; libera a memória do elemento na heap (!)]

void destroy_character (struct character_t *player) {
    if (!player) return;

    al_destroy_bitmap (player->fighter);
    joystick_destroy (player->joystick);
    free (player);
}

/* Função responsavel por aplicar o efeito de gravidade caso o personagem esteja pulando */
void apply_gravity (struct character_t *player) {
    if (player->jump) {
        if (player->y_display <= 100 && player->vel_y < 0) {
            player->vel_y = -player->vel_y;                                                                      // Inverte a direção do movimento caso ele alcance a altura limite
        } 
		else {
            player->vel_y += GRAVITY;                                                                           // Aplica a gravidade (Diminui a altura do salto até encostar no chao)
            player->maxFrame = 7;                                                                               // Informações sobre o frame correto da spritesheet 
            player->currentFrame = player->currentFrame * 8;	
        }
    }
}

/* Verifica se houve colisao entre personagens (sem ataque) */
int collision (struct character_t *p1, struct character_t *p2) {
	if (!p1 || !p2) return 0;
	if (p1->x_display+p1->side > p2->x_display && 
        p1->x_display< p2->x_display + p2->side) 
        return 1;
	else 
    return 0;
}

/* Verifica se houve colisao entre os personagem quando ocorre um ataque */
int collision_hit (struct character_t *p1, struct character_t *p2) {
	if (!p1 || !p2) return 0;
	if (p1->x_display + 50 + p1->side > p2->x_display && 
        p1->x_display < p2->x_display + 50 + p2->side) 
        return 1;	
	else 
    return 0;
}

/* Atualiza a posicao do jogador, caso ele esteja pulando*/
void update_position_jump (struct character_t *player) {
    player->y_display += player->vel_y;
    
    if (player->y_display >= GROUND_LEVEL) {  // Caso o jogador encoste no chão, ele atualiza os parametros para o pulo ser falso
        player->y_display = GROUND_LEVEL;
        player->vel_y = 0;
        player->jump = 0;
    }
}

/* Apos realizar movimentacoes, o personagem retorna para a posicao padrao */
void default_position (struct character_t *player1, struct character_t *player2) {
    if (!player1 || !player2) return;

    player1->maxFrame = 4;
    player1->currentFrame = 80;
    player1->y_display = 420;

    player2->maxFrame = 4;
    player2->currentFrame = 80;
    player2->y_display = 420;

}

void default_joystick (struct character_t *player) {
    if (!player) return;

    player->joystick->down = 0;
    player->joystick->right = 0;
    player->joystick->left= 0;
    player->joystick->up = 0;
    player->joystick->kick = 0;
    player->joystick->push = 0;

}

/* Funcao responsavel por implementar o pulo do personagem*/
void character_jump (struct character_t *player) {
    if (!player->jump) {
        player->vel_y = JUMP_STRENGTH; // Vel_y recebe a altura do salto 
        player->jump = 1;
    }
}

void character_move (struct character_t *element, char steps, unsigned char trajectory, unsigned short max_x, unsigned short max_y){

	if (trajectory == LEFT){ 
		if ((element->x_display - steps * STEP) + element->side/2 >= 0) {
			element->x_display = element->x_display - steps*STEP;  				//Verifica se a movimentação para a esquerda é desejada e possível; se sim, efetiva a mesma
			if (steps > 0 && element->jump == 0) {
				element->currentFrame = element->currentFrame * 3;
				element->maxFrame = 5;			
			}			
		}
	}
	else if (trajectory == RIGHT){ 
		if ((element->x_display + steps*STEP)  + 200 <= max_x) {
			element->x_display = element->x_display + steps * STEP;             //Verifica se a movimentação para a direita é desejada e possível; se sim, efetiva a mesma
			if (steps > 0 && element->jump == 0) {
				element->currentFrame = element->currentFrame * 3;
				element->maxFrame = 5;
			}			
		}
	}	
	else if (trajectory == UP){ 
		if ((element->y_display - steps*STEP) - element->side >= 0) {           //Verifica se a movimentação para cima é desejada e possível; se sim, efetiva a mesma			
			element->x_sprite = 70;
			element->maxFrame = 7;
			element->currentFrame = element->currentFrame * 8;						
            element->frame = 0;			
		}			
	}
	else if (trajectory == DOWN){ 
		if ((element->y_display + steps*STEP) + element->side/2 <= max_y ) {			
			if (steps > 0 && element->jump == 0 ) {                            //Verifica se a movimentação para baixo é desejada e possível; se sim, efetiva a mesma
				element->currentFrame = element->currentFrame * 9;
				element->maxFrame = 1;			
			}			
		}			
	}	
	else if (trajectory == PUSH){  
		if (element->y_display + steps*STEP) {                                //Verifica se a movimentação para o soco é desejada e possível; se sim, efetiva a mesma			
			if (steps > 0 && element->jump == 0) {
				element->currentFrame = element->currentFrame * 2;
				element->maxFrame = 3;			
			}			
		}			
	}	
	else if (trajectory == KICK){ 
		if ((element->y_display + steps*STEP) ) {                           //Verifica se a movimentação para o chute é desejada e possível; se sim, efetiva a mesma			
			if (steps > 0 && element->jump == 0) {
				element->currentFrame = element->currentFrame * 6;
				element->maxFrame = 5;	
			}			
		}			
	}	
}

/* Sequencia de funcoes para detectar caso uma tecla seja pressionada/solta */
void joystick_left (struct joystick_t *element){ element->left = element->left ^ 1;}			//Implementação da função "joystick_left"; muda o estado do botão (!)

void joystick_right (struct joystick_t *element){ element->right = element->right ^ 1;}		//Implementação da função "joystick_right"; muda o estado do botão (!)

void joystick_up (struct joystick_t *element){ element->up = element->up ^ 1;}				//Implementação da função "joystick_up"; muda o estado do botão (!)

void joystick_down (struct joystick_t *element){ element->down = element->down ^ 1;}			//Implementação da função "joystick_down"; muda o estado do botão (!)

void joystick_push (struct joystick_t *element){ element->push = element->push ^ 1;}         //Implementação da função "joystick_push"; muda o estado do botão (!)

void joystick_kick (struct joystick_t *element){ element->kick = element->kick ^ 1;}         //Implementação da função "joystick_kick"; muda o estado do botão (!)


/* Funcao principal por atualizar o posicionamento de cada personagem e selecionar*/
int update_position (struct character_t *player1, struct character_t *player2) {
    apply_gravity (player1);
    apply_gravity (player2);

    update_position_jump (player1);
    update_position_jump (player2);

    if (player1->joystick->left && !player1->joystick->down && !player1->joystick->right) {
        character_move (player1, 1, 0, X_SCREEN, Y_SCREEN);
        if (collision (player1, player2)) character_move (player1, -1, 0, X_SCREEN, Y_SCREEN);
    }
    if (player1->joystick->right && !player1->joystick->down && !player1->joystick->left) {
        character_move (player1, 1, 1, X_SCREEN, Y_SCREEN);
        if (collision (player1, player2)) character_move (player1, -1, 1, X_SCREEN, Y_SCREEN);
    }
    if (player1->joystick->up) {
        character_jump (player1);		
    }
    if (player1->joystick->down) {
        character_move (player1, 1, 3, X_SCREEN, Y_SCREEN);
        //if (collision (player1, player2)) character_move (player1, -1, 3, X_SCREEN, Y_SCREEN);
    }
    if (player1->joystick->push && !player1->jump && !player1->joystick->down && !player1->joystick->left && !player1->joystick->right) {
        character_move (player1, 1, 4, X_SCREEN, Y_SCREEN);
        if (collision_hit (player1, player2)) return 2;        
    }
    if (player1->joystick->kick && !player1->jump && !player1->joystick->down && !player1->joystick->left && !player1->joystick->right) {
        character_move (player1, 1, 5, X_SCREEN, Y_SCREEN);
        if (collision_hit(player1, player2)) return 2;        
    }
    if (player2->joystick->left && !player2->joystick->down && !player2->joystick->right) {
        character_move (player2, 1, 0, X_SCREEN, Y_SCREEN);
        if (collision (player1, player2)) character_move (player2, -1, 0, X_SCREEN, Y_SCREEN);
    }
    if (player2->joystick->right && !player2->joystick->down && !player2->joystick->left) {
        character_move (player2, 1, 1, X_SCREEN, Y_SCREEN);
        if (collision (player1, player2)) character_move (player2, -1, 1, X_SCREEN, Y_SCREEN);
    }
    if (player2->joystick->up) {
        character_jump (player2);
    }
    if (player2->joystick->down) {
        character_move (player2, 1, 3, X_SCREEN, Y_SCREEN);
        //if (collision (player1, player2)) character_move (player2, -1, 3, X_SCREEN, Y_SCREEN);
    }
    if (player2->joystick->push && !player2->jump && !player2->joystick->down && !player2->joystick->left && !player2->joystick->right) {
        character_move (player2, 1, 4, X_SCREEN, Y_SCREEN);
        if (collision_hit (player1, player2)) return 1;        
    }
    if (player2->joystick->kick && !player2->jump && !player2->joystick->down && !player2->joystick->left && !player2->joystick->right) {
        character_move (player2, 1, 5, X_SCREEN, Y_SCREEN);
        if (collision_hit (player1, player2)) return 1;       
    }
    return 0;
}

/* Limpa a fila de eventos do teclado. Usado quando o jogo/rodada são iniciados */
void clear_event_queue(ALLEGRO_EVENT_QUEUE *queue) {
    ALLEGRO_EVENT event;
    while (!al_is_event_queue_empty(queue)) {
        al_get_next_event(queue, &event);
    }
}

/* Usuario decide entre qual personagem ele irá escolher para jogar*/
char choose_character (ALLEGRO_FONT* font) {
    char letter = '\0';
    al_clear_to_color(al_map_rgb(0, 0, 0));
    ALLEGRO_BITMAP *fundoMenu = al_load_bitmap("./fundoMenuPrincipal.jpg");
    ALLEGRO_BITMAP *ryu = al_load_bitmap("./SelecionaRyu.png");
    ALLEGRO_BITMAP *ken = al_load_bitmap("./SelecionaKen.png");
    al_draw_bitmap (fundoMenu, 0, 0, 0);
    al_draw_text (font, al_map_rgb(255,255,255), 525, 50, 0, "Player 1: SELECT THE CHARACTER");
    
    al_draw_scaled_bitmap(ryu, 0, 0, 736 ,1233, 400, 180, 300, 300, 0);                     
    al_draw_scaled_bitmap(ken, 0, 0, 736 ,1233, 800, 180, 300, 300, 0);                     
    al_flip_display ();
    
    ALLEGRO_EVENT_QUEUE *event_queue = al_create_event_queue();
    al_register_event_source (event_queue, al_get_keyboard_event_source());
	ALLEGRO_EVENT event;
    do {        
        al_wait_for_event (event_queue, &event);
        if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
            if (event.keyboard.keycode == ALLEGRO_KEY_R) {
                letter = 'R';
            } else if (event.keyboard.keycode == ALLEGRO_KEY_K) {
                letter = 'K';
            } else if( event.type == ALLEGRO_EVENT_DISPLAY_CLOSE ){
            break;
        }
        }
    } while (letter != 'R' && letter != 'K');

    clear_event_queue (event_queue);
    al_destroy_event_queue(event_queue);
    al_destroy_bitmap (fundoMenu);
    al_destroy_bitmap (ryu);
    al_destroy_bitmap (ken);
    
    al_clear_to_color(al_map_rgb(0, 0, 0));
	return letter;
}

/* Funcao que pausa o jogo durante o round */
void game_paused (ALLEGRO_FONT *font) {    
    if (!font) return;
    
    ALLEGRO_EVENT_QUEUE *event_queue = al_create_event_queue();    // Cria uma fila de eventos para capturar eventos do teclado
    al_register_event_source(event_queue, al_get_keyboard_event_source());

    ALLEGRO_EVENT event;
    int pause = 1;

    while (pause) {

        al_wait_for_event(event_queue, &event); // Aguarda um evento        
        if (event.type == ALLEGRO_EVENT_KEY_DOWN && event.keyboard.keycode == ALLEGRO_KEY_P) // Verifica se a tecla P foi pressionada para retomar o jogo
            pause = 0; // Define que o jogo não está mais pausado
                
        al_draw_text(font, al_map_rgb(0, 0, 0), 530, 330, 0, "PAUSE");
        al_draw_text(font, al_map_rgb(255, 255, 255), 530, 325, 0, "PAUSE");
        al_flip_display();
    } 
    clear_event_queue (event_queue);
    al_destroy_event_queue(event_queue);
}


/* Menu principal do jogo. Usuario pode continuar a jogar ou quitar*/
void menu (ALLEGRO_FONT* font, ALLEGRO_BITMAP* menuBitmap, ALLEGRO_BITMAP* logo) {
    al_clear_to_color(al_map_rgb(0, 0, 0));
    al_draw_bitmap(menuBitmap, 0, 0, 0);
    al_draw_bitmap(logo, 440, 150, 0);   
    al_draw_text(font, al_map_rgb(0,0,0), 530, 330, 0, "Press ENTER to begin");
    al_draw_text(font, al_map_rgb(255,255,255), 530, 325, 0, "Press ENTER to begin");

    al_draw_text(font, al_map_rgb(0,0,0), 530, 380, 0, "Press ESC to quit");
    al_draw_text(font, al_map_rgb(255,255,255), 530, 375, 0, "Press ESC to quit");

    al_flip_display();

    ALLEGRO_EVENT_QUEUE * event_queue = al_create_event_queue();
    al_register_event_source(event_queue, al_get_keyboard_event_source());
	ALLEGRO_EVENT event;
    while (1) {        

        al_wait_for_event (event_queue, &event);
        if( event.type == ALLEGRO_EVENT_DISPLAY_CLOSE ){
            break;
        }

        if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
            if (event.keyboard.keycode == ALLEGRO_KEY_ENTER) {
                break;
            } else if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
                exit(1);
            }            
        }
    }
    clear_event_queue (event_queue);
    al_destroy_event_queue (event_queue);
	al_clear_to_color (al_map_rgb(255,255,255));
}

/* Reseta as configuracoes para o inicio de um novo round*/
void reset_character (struct character_t *player1, struct character_t *player2) {

    //Redefini a vida do jogador
	player1->life = 600;
	player2->life = 600;

    //Posicionamento no cenario
	player1->x_display = 200;
	player2->x_display = 800;
}

/* Verifica se ha um vencedor na partida. Nessario possuir 2 rounds vencidos */
int has_winner_match (ALLEGRO_FONT *font, struct character_t *player1, struct character_t *player2) {
	if (player1->rounds_won >=2 || player2->rounds_won >= 2) return 1; 
    else return 0;
}

/* Imprime a mensagem de qual jogador venceu o jogo */
void print_winner (ALLEGRO_FONT *font, struct character_t *player1, struct character_t *player2) {
	if (player1->rounds_won >=2) {
        al_draw_text(font, al_map_rgb(0,0,0), 530, 330, 0, "Player 1 venceu o jogo!");
        al_draw_text(font, al_map_rgb(255,255,255), 530, 325, 0, "Player 1 venceu o jogo!");
    }    
    else {
        al_draw_text(font, al_map_rgb(0,0,0), 530, 330, 0, "Player 2 venceu o jogo!");
        al_draw_text(font, al_map_rgb(255,255,255), 530, 325, 0, "Player 2 venceu o jogo!");
    }
}

/* Verifica se o round acabou por tempo ou por quantidade de vida/hp */
int end_round (struct character_t *player1, struct character_t *player2, int *timming) {
    if (player1->life <= 0 || player2->life <= 0 || *timming <= 0) return 1;
	else return 0;
}

/* Verifica se algum jogador venceu a rodada*/
void check_winner (struct character_t *player1, struct character_t *player2) {
    if (player1->life > player2->life) player1->rounds_won++;
    else if (player2->life > player1->life) player2->rounds_won++;
    else {
        player1->rounds_won++;
        player2->rounds_won++;
    }    
}

/* Remove os pontos de vida do jogador dependendo da variavel 'jogador' */
void remove_life (struct character_t *player1, struct character_t *player2, int jogador) {
	if (jogador == 1 && player1->joystick->down == 0) {
        player1->life -= 5;
        player1->currentFrame = player1->currentFrame * 10;
        player1->maxFrame = 1;
        player1->frame = 0;

    }
	else if (jogador == 2 && player2->joystick->down == 0) {
        player2->life -= 5;
        player2->currentFrame = player2->currentFrame * 10;
        player2->maxFrame = 1;
        player2->frame = 0;

    }
}

/* Imprime o cenario / background */
void print_scene (struct character_t *player1, struct character_t *player2, ALLEGRO_BITMAP *scene, ALLEGRO_FONT* font, ALLEGRO_BITMAP * rounds[]) {
	al_clear_to_color(al_map_rgb(0,0,0));
	al_draw_bitmap(scene, 0, 0, 0);
	ALLEGRO_COLOR red = al_map_rgb(255, 0, 0);
	al_draw_filled_rectangle (5, 5, 615, 65, al_map_rgb(255, 218, 185));
	al_draw_filled_rectangle (10, 10, 10 + player1->life, 60, red);
	al_draw_filled_rectangle (665, 5, 1275, 65, al_map_rgb(255, 218, 185));
	al_draw_filled_rectangle (670, 10, 670 + player2->life, 60, red);

	al_draw_text(font, al_map_rgb(0,0,0), 15, 20, 0, "player1");
	al_draw_text(font, al_map_rgb(0,0,0), 1175, 20, 0, "player2");

	al_draw_filled_circle(580, 80, 10, al_map_rgb(0, 0, 0));
	al_draw_filled_circle(600, 80, 10, al_map_rgb(0, 0, 0));

	al_draw_filled_circle(680, 80, 10, al_map_rgb(0, 0, 0));
	al_draw_filled_circle(700, 80, 10, al_map_rgb(0, 0, 0));	

	if (player1->rounds_won == 1) {
		al_draw_filled_circle(580, 80, 10, al_map_rgb(255, 255, 0));
	}
	if (player2->rounds_won == 1) {
		al_draw_filled_circle(680, 80, 10, al_map_rgb(255, 255, 0));
	}

	if (player1->rounds_won + player2->rounds_won == 0) {
		al_draw_scaled_bitmap (rounds[0],0,0,372,118,597,100,100,50,0);
	}
	else if (player1->rounds_won + player2->rounds_won == 1)
		al_draw_scaled_bitmap (rounds[1],0,0,372,118,597,100,100,50,0);

	else if (player1->rounds_won + player2->rounds_won == 2)
		al_draw_scaled_bitmap (rounds[2],0,0,372,118,597,100,100,50,0);	
}

/* Imprime o cronometro da rodada */
void print_time (ALLEGRO_FONT*font, int *num, int *fps) {
    if (!font) return;
    if (*fps >= 30) {
        (*num)--;
        *fps = 0;
    }
    char timming[2];
    sprintf (timming, "%d", *num);
    al_draw_text(font, al_map_rgb(255,255,255), 615, 5, 0, timming);
}

/* Imprime os personagem na tela conforme o frame atual */
void print_character(struct character_t *player1, struct character_t *player2) {
    player1->frame += 0.3f;
    player2->frame += 0.3f;

    if (player1->frame > player1->maxFrame)
        player1->frame -= player1->maxFrame;    

    if (player2->frame > player2->maxFrame) 
        player2->frame -= player2->maxFrame;
    
    if (player1->frame > 6 && player1->jump) player1->frame = 5;

    if (player2->frame > 6 && player2->jump) player2->frame = 5;

    al_draw_scaled_bitmap(player1->fighter, player1->x_sprite * (int)(player1->frame), player1->currentFrame, player1->x_sprite, player1->y_sprite, player1->x_display, player1->y_display, 250, 250, player1->direction);
    al_draw_scaled_bitmap(player2->fighter, player2->x_sprite * (int)(player2->frame), player2->currentFrame, player2->x_sprite, player2->y_sprite, player2->x_display, player2->y_display, 250, 250, player2->direction);    
}

int main () {

    al_init(); //Faz a preparação de requisitos da biblioteca Allegro
    al_init_font_addon();
    al_init_ttf_addon();
    al_init_image_addon();
    al_install_keyboard(); //Habilita a entrada via teclado (eventos de teclado), no programa
	al_init_primitives_addon();	//Faz a inicialização dos addons das imagens básicas

	char fighter;	
	int hp, game_on = 0, timming = CLOCK, fps = 0;
		
	struct character_t* player1;
    struct character_t* player2;

    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 30.0); //Cria o relógio do jogo; isso indica quantas atualizações serão realizadas por segundo (30, neste caso)
    ALLEGRO_DISPLAY *display = al_create_display(X_SCREEN,Y_SCREEN); //Cria uma janela para o programa, define a largura (x) e a altura (y) da tela em píxeis (320x320, neste caso)
    ALLEGRO_FONT* font = al_load_font("./Act_Of_Rejection.ttf", 25, 0);
    ALLEGRO_FONT* time = al_load_font("./DS-DIGIT.TTF", 45, 0);

    /* Carrega as imagens dos personagens e do cenario*/
    ALLEGRO_BITMAP * ken = al_load_bitmap("./Ken.png");
    ALLEGRO_BITMAP * ryu = al_load_bitmap ("./Ryu.png");

    ALLEGRO_BITMAP * scene = al_load_bitmap("./cenario.jpg");
    ALLEGRO_BITMAP * logo = al_load_bitmap("StreetFighterArcTitle2.png");
    ALLEGRO_BITMAP * menuBitmap = al_load_bitmap("./menuNovo.jpg");
	ALLEGRO_BITMAP * rounds[ROUNDS];

    /* Imagem para definir qual eh o round atual que esta sendo jogado */
	rounds[0] = al_load_bitmap("./rounds1.png");
	rounds[1] = al_load_bitmap("./rounds2.png");
	rounds[2] = al_load_bitmap("./rounds3.png");

	ALLEGRO_KEYBOARD_STATE keystate;
    ALLEGRO_EVENT_QUEUE * event_queue = al_create_event_queue();

    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_timer_event_source(timer));
    al_register_event_source(event_queue, al_get_keyboard_event_source() );
	al_get_keyboard_state(&keystate);
    al_start_timer(timer);
	al_set_window_position(display, 200, 200);
    al_set_window_title(display, "Street Fighter!");

    ALLEGRO_EVENT event;
	al_start_timer(timer);	
    while (1) {
		al_wait_for_event(event_queue, &event);	

		if (!game_on) {
			menu (font, menuBitmap, logo);
			fighter = choose_character (font); 
			if (fighter == 'R' || fighter == 'r')   {
				player1 = create_character (ryu, 70, 80, 200, 420, 1000, 1000, 80, 0);
				player2 = create_character (ken, 70, 80, 800, 420, 1000, 1000, 80, 1);
			}
			else {
				player1 = create_character (ken, 70, 80, 200, 420, 1000, 1000, 80, 0);
				player2 = create_character (ryu, 70 ,80, 800, 420, 1000, 1000, 80, 1);
			}
			game_on = 1;
            clear_event_queue (event_queue);
		}		

		if (event.type == ALLEGRO_EVENT_TIMER) {
			hp = update_position(player1, player2);									//O evento tipo 30 indica um evento de relógio, ou seja, verificação se a tela deve ser atualizada (conceito de FPS)			
			remove_life (player1, player2, hp);
			al_clear_to_color(al_map_rgb (0,0,0));
			print_scene (player1, player2, scene, font, rounds);
			print_character (player1, player2);
            print_time (time, &timming, &fps);                
		}	

		if (end_round (player1, player2, &timming)) {
			check_winner (player1, player2);
			reset_character (player1, player2);
            clear_event_queue (event_queue);
            al_rest (1.5);   
            timming = CLOCK;         
		}

		if (has_winner_match (font, player1, player2)) {
            game_on = 0; 
            timming = CLOCK;          
            print_winner(font, player1, player2);
            al_flip_display();
            al_rest (4.0);
        }

	    if ((event.type == ALLEGRO_EVENT_KEY_DOWN) || (event.type == ALLEGRO_EVENT_KEY_UP)){																																				//Verifica se o evento é de botão do teclado abaixado ou levantado (!)
			if (event.keyboard.keycode == ALLEGRO_KEY_A)          joystick_left(player1->joystick);																															//Indica o evento correspondente no joystick do primeiro jogador (botão de movimentação à esquerda) (!)
			else if (event.keyboard.keycode == ALLEGRO_KEY_D)     joystick_right(player1->joystick);																													//Indica o evento correspondente no joystick do primeiro jogador (botão de movimentação à direita) (!)
			else if (event.keyboard.keycode == ALLEGRO_KEY_W)     joystick_up(player1->joystick);																														//Indica o evento correspondente no joystick do primeiro jogador (botão de movimentação para cima) (!)
			else if (event.keyboard.keycode == ALLEGRO_KEY_S )    joystick_down(player1->joystick);	
			else if (event.keyboard.keycode == ALLEGRO_KEY_X)     joystick_push(player1->joystick);																														//Indica o evento correspondente no joystick do primeiro jogador (botão de movimentação para cima) (!)
			else if (event.keyboard.keycode == ALLEGRO_KEY_C)     joystick_kick(player1->joystick);																												//Indica o evento correspondente no joystick do primeiro jogador (botão de movimentação para baixo) (!)
			else if (event.keyboard.keycode == ALLEGRO_KEY_LEFT)  joystick_left(player2->joystick);																													//Indica o evento correspondente no joystick do segundo jogador (botão de movimentação à esquerda) (!)
			else if (event.keyboard.keycode == ALLEGRO_KEY_RIGHT) joystick_right(player2->joystick);																													//Indica o evento correspondente no joystick do segundo jogador (botão de movimentação à direita) (!)
			else if (event.keyboard.keycode == ALLEGRO_KEY_UP)    joystick_up(player2->joystick);																														//Indica o evento correspondente no joystick do segundo jogador (botão de movimentação para cima) (!)
			else if (event.keyboard.keycode == ALLEGRO_KEY_DOWN)  joystick_down(player2->joystick);																													//Indica o evento correspondente no controle do segundo jogador (botão de movimentação para baixo) (!)
			else if (event.keyboard.keycode == ALLEGRO_KEY_PAD_DELETE)     joystick_push(player2->joystick);																														//Indica o evento correspondente no jo do primeiro jogador (botão de movimentação para cima) (!)
			else if (event.keyboard.keycode == ALLEGRO_KEY_PAD_0)     joystick_kick(player2->joystick);
            
		}
	    else if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) break;																																                                    	//Evento de clique no "X" de fechamento da tela. Encerra o programa graciosamente.               
        if (event.type == ALLEGRO_EVENT_KEY_DOWN && event.keyboard.keycode == ALLEGRO_KEY_P) {
            game_paused (font);
            clear_event_queue(event_queue);
        }

        fps++;
        default_position (player1, player2);
		al_flip_display();        
    }
    /* Destruir tudo*/
    clear_event_queue (event_queue);
    al_destroy_font(font);	
    al_destroy_font(time);	
    al_destroy_timer(timer);
    al_destroy_event_queue(event_queue);
    al_destroy_bitmap (scene);
    al_destroy_bitmap (ken);
    al_destroy_bitmap (ryu);
    al_destroy_bitmap (logo);
    al_destroy_bitmap (menuBitmap);    
    for (int i = 0; i < ROUNDS; i++)
        al_destroy_bitmap (rounds[i]);
    destroy_character (player1);
    destroy_character (player2);
    al_destroy_display (display);
	return 0;
}
