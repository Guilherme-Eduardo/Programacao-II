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

struct joystick {
    unsigned char right;
    unsigned char left;
    unsigned char up;
    unsigned char down;
    unsigned char push;
    unsigned char kick;
};

struct jogador {
    float x_sprite;
    float y_sprite;
	float x_display;
    float y_display;
    float maxX;
    float maxY;
    float currentFrame;
	float maxFrame;
	int direcao;
	int side;
	int life;
	int rounds;
    struct joystick *controle;
    ALLEGRO_BITMAP *lutador;

};


struct joystick* joystick_create(){														//Implementação da função "joystick_create" (!)

	struct joystick *element = (struct joystick*) malloc (sizeof(struct joystick));						//Aloca memória na heap para um novo controle (!)
	element->right = 0;																//Insere o estado de desligado para o botão de movimentação à esquerda (!)
	element->left = 0;																//Insere o estado de desligado para o botão de movimentação à direita (!)
	element->up = 0;																//Insere o estado de desligado para o botão de movimentação para cima (!)
	element->down = 0;			
	element->push = 0;
	element->kick = 0;													//Insere o estado de desligado para o botão de movimentação para baixo (!)
	return element;																	//Retorna o novo controle (!)
}

struct jogador *criaJogador (ALLEGRO_BITMAP *nome, unsigned int xS, unsigned int yS, unsigned int xD, unsigned int yD, unsigned int maxX, unsigned int maxY, unsigned int currentFrame) {
	struct jogador *player;
	player = (struct jogador*) malloc (sizeof (struct jogador));
	if (!player) exit(1);
	player->x_sprite = xS;
	player->y_sprite = yS;
	player->x_display = xD;
	player->y_display = yD;
	player->maxX = maxX;
	player->maxY = maxY;
	player->currentFrame = currentFrame;
	player->lutador = nome;	
	player->maxFrame = 4;
	player->side = 100;
	player->life = 600;
	player->rounds = 0;
	player->controle = joystick_create();
	return player;
}


int colisao (struct jogador *p1, struct jogador *p2) {
	if (!p1 || !p2) return 0;
	if (p1->x_display+p1->side > p2->x_display && p1->x_display< p2->x_display + p2->side) return 1;
	else return 0;
}
int colisaoGolpe (struct jogador *p1, struct jogador *p2) {
	if (!p1 || !p2) return 0;
	if (p1->x_display+50+p1->side > p2->x_display && p1->x_display < p2->x_display + 50 + p2->side) return 1;	
	else return 0;
}

void jogadorMov(struct jogador *element, char steps, unsigned char trajectory, unsigned short max_x, unsigned short max_y){							//Implementação da função "jogadorMov" (-1)

	if (!trajectory){ 
		if ((element->x_display - steps*STEP) + element->side/2 >= 0) {
			element->x_display = element->x_display - steps*STEP;  				//Verifica se a movimentação para a esquerda é desejada e possível; se sim, efetiva a mesma
			element->currentFrame = element->currentFrame * 3;
			element->maxFrame = 5;			
		}
	}
	else if (trajectory == 1){ 
		if ((element->x_display + steps*STEP)  + 200 <= max_x) {
			element->x_display = element->x_display + steps*STEP; //Verifica se a movimentação para a direita é desejada e possível; se sim, efetiva a mesma
			element->currentFrame = element->currentFrame * 3;
			element->maxFrame = 5;
		}
	}	
	else if (trajectory == 2){ 
		if ((element->y_display - steps*STEP) - element->side >= 0) {
			element->y_display = element->y_display - 200; 		//Verifica se a movimentação para cima é desejada e possível; se sim, efetiva a mesma
			element->x_sprite = 70;
			element->maxFrame = 7;
			element->currentFrame = element->currentFrame * 8;
			//element->y_display = element->x_display - 5;
		}			
	}
	else if (trajectory == 3){ 
		if ((element->y_display + steps*STEP) + element->side/2 <= max_y ) {
			//element->y_display = element->y_display + steps*STEP; //Verifica se a movimentação para baixo é desejada e possível; se sim, efetiva a mesma
			element->currentFrame = element->currentFrame * 9;
			element->maxFrame = 1;			
		}
			
	}	
	else if (trajectory == 4){ 
		if (element->y_display + steps*STEP)  {
			//element->y_display = element->y_display + steps*STEP; //Verifica se a movimentação para baixo é desejada e possível; se sim, efetiva a mesma
			element->currentFrame = element->currentFrame * 2;
			element->maxFrame = 3;			
		}			
	}	
	else if (trajectory == 5){ 
		if ((element->y_display + steps*STEP) ) {
			//element->y_display = element->y_display + steps*STEP; //Verifica se a movimentação para baixo é desejada e possível; se sim, efetiva a mesma
			element->currentFrame = element->currentFrame * 6;
			element->maxFrame = 5;	
		}			
	}	
}

void joystick_destroy(struct joystick *element){ free(element);}							//Implementação da função "joystick_destroy"; libera a memória do elemento na heap (!)

void joystick_left(struct joystick *element){ element->left = element->left ^ 1;}			//Implementação da função "joystick_left"; muda o estado do botão (!)

void joystick_right(struct joystick *element){ element->right = element->right ^ 1;}		//Implementação da função "joystick_right"; muda o estado do botão (!)

void joystick_up(struct joystick *element){ element->up = element->up ^ 1;}				//Implementação da função "joystick_up"; muda o estado do botão (!)

void joystick_down(struct joystick *element){ element->down = element->down ^ 1;}			//Implementação da função "joystick_down"; muda o estado do botão (!)

void joystick_push(struct joystick *element){ element->push = element->push ^ 1;}

void joystick_kick(struct joystick *element){ element->kick = element->kick ^ 1;}


int update_position(struct jogador *player1, struct jogador *player2){																																				//Função de atualização das posições dos quadrados conforme os comandos do controle (!)
	
	if (player1->controle->left){																																										//Se o botão de movimentação para esquerda do controle do primeiro jogador está ativado... (!)
		jogadorMov(player1, 1, 0, X_SCREEN, Y_SCREEN);																																			//Move o quadrado do primeiro jogador para a esquerda (!)
		if (colisao(player1, player2)) jogadorMov(player1, -1, 0, X_SCREEN, Y_SCREEN);																											//Se o movimento causou uma colisão entre quadrados, desfaça o mesmo (!)
		//return 0;
	}
	if (player1->controle->right){																																										//Se o botão de movimentação para direita do controle do primeir ojogador está ativado... (!)
		jogadorMov(player1, 1, 1, X_SCREEN, Y_SCREEN);																																				//Move o quadrado do primeiro jogador para a direta (!)
		if (colisao(player1, player2)) jogadorMov(player1, -1, 1, X_SCREEN, Y_SCREEN);			 																										//Se o movimento causou uma colisão entre quadrados, desfaça o mesmo (!)
		//return 0;
	}
	if (player1->controle->up) {																																										//Se o botão de movimentação para cima do controle do primeiro jogador está ativado... (!)
		jogadorMov(player1, 1, 2, X_SCREEN, Y_SCREEN);																																				//Move o quadrado do primeiro jogador para cima (!)
		if (colisao(player1, player2)) jogadorMov(player1, -1, 2, X_SCREEN, Y_SCREEN);																											//Se o movimento causou uma colisão entre quadrados, desfaça o mesmo (!)
		//return 0;
	}
	if (player1->controle->down){																																										//Se o botão de movimentação para baixo do controle do primeiro jogador está ativado... (!)
		jogadorMov(player1, 1, 3, X_SCREEN, Y_SCREEN);																																				//Move o quadrado do primeiro jogador para a baixo (!)
		if (colisao(player1, player2)) jogadorMov(player1, -1, 3, X_SCREEN, Y_SCREEN);																											//Se o movimento causou uma colisão entre quadrados, desfaça o mesmo (!)
		//return 0;
	}
    if (player1->controle->push){																																										//Se o botão de movimentação para baixo do controle do primeiro jogador está ativado... (!)
		jogadorMov(player1, 1, 4, X_SCREEN, Y_SCREEN);																																				//Move o quadrado do primeiro jogador para a baixo (!)
		if (colisaoGolpe(player1, player2)) return 2;																					//Se o movimento causou uma colisão entre quadrados, desfaça o mesmo (!)		
		else return 0;
		
	}
    if (player1->controle->kick){																																										//Se o botão de movimentação para baixo do controle do primeiro jogador está ativado... (!)
		jogadorMov(player1, 1, 5, X_SCREEN, Y_SCREEN);																																				//Move o quadrado do primeiro jogador para a baixo (!)
		if (colisaoGolpe(player1, player2)) return 2;
		else return 0;
	}

	if (player2->controle->left){																																										//Se o botão de movimentação para esquerda do controle do segundo jogador está ativado... (!)
		jogadorMov(player2, 1, 0, X_SCREEN, Y_SCREEN);																																				//Move o quadrado do segundo jogador para a direita (!)
		if (colisao(player1, player2)) jogadorMov(player2, -1, 0, X_SCREEN, Y_SCREEN);	
		//return 0;
	}
	
	if (player2->controle->right){ 																																										//Se o botão de movimentação para direita do controle do segundo jogador está ativado... (!)
		jogadorMov(player2, 1, 1, X_SCREEN, Y_SCREEN);																																				//Move o quadrado do segundo jogador para a direita (!)
		if (colisao(player1, player2)) jogadorMov(player2, -1, 1, X_SCREEN, Y_SCREEN);																											//Se o movimento causou uma colisão entre quadrados, desfaça o mesmo (!)
		//return 0;
	}
	
	if (player2->controle->up){																																											//Se o botão de movimentação para cima do controle do segundo jogador está ativado... (!)
		jogadorMov(player2, 1, 2, X_SCREEN, Y_SCREEN);																																				//Move o quadrado do segundo jogador para a cima (!)
		if (colisao(player1, player2)) jogadorMov(player2, -1, 2, X_SCREEN, Y_SCREEN);																											//Se o movimento causou uma colisão entre quadrados, desfaça o mesmo (!)			
		//return 0;
	}
	if (player2->controle->down){																																										//Se o botão de movimentação para baixo do controle do segundo jogador está ativado... (!)
		jogadorMov(player2, 1, 3, X_SCREEN, Y_SCREEN);																																				//Move o quadrado do segundo jogador para a baixo (!)
		if (colisao(player1, player2)) jogadorMov(player2, -1, 3, X_SCREEN, Y_SCREEN);																											//Se o movimento causou uma colisão entre quadrados, desfaça o mesmo (!)
		//return 0;
	}
    if (player2->controle->push){																																										//Se o botão de movimentação para baixo do controle do primeiro jogador está ativado... (!)
		jogadorMov(player2, 1, 4, X_SCREEN, Y_SCREEN);																																				//Move o quadrado do primeiro jogador para a baixo (!)
		if (colisaoGolpe(player1, player2)) return 1;																								//Se o movimento causou uma colisão entre quadrados, desfaça o mesmo (!)
		else return 0;
	}
    if (player2->controle->kick){																																										//Se o botão de movimentação para baixo do controle do primeiro jogador está ativado... (!)
		jogadorMov(player2, 1, 5, X_SCREEN, Y_SCREEN);																																				//Move o quadrado do primeiro jogador para a baixo (!)
		if (colisaoGolpe(player1, player2)) return 1;																										//Se o movimento causou uma colisão entre quadrados, desfaça o mesmo (!)
		else return 0;
	}
	return 0;
}

char SelecionaPersonagem (ALLEGRO_FONT* font) {
    char letra = '\0';
    al_clear_to_color(al_map_rgb(0, 0, 0));
    ALLEGRO_BITMAP *fundoMenu = al_load_bitmap("./fundoMenuPrincipal.jpg");
    ALLEGRO_BITMAP *ryu = al_load_bitmap("./SelecionaRyu.png");
    ALLEGRO_BITMAP *ken = al_load_bitmap("./SelecionaKen.png");
    al_draw_bitmap(fundoMenu, 0, 0, 0);
    al_draw_text(font, al_map_rgb(255,255,255), 525, 50, 0, "Player 1: Selecione o personagem");
    
    al_draw_scaled_bitmap(ryu, 0, 0, 736 ,1233, 400, 180, 300, 300, 0);                     
    al_draw_scaled_bitmap(ken, 0, 0, 736 ,1233, 800, 180, 300, 300, 0);                     
    al_flip_display();
    
    ALLEGRO_EVENT_QUEUE *fila_eventos = al_create_event_queue();
    al_register_event_source(fila_eventos, al_get_keyboard_event_source());
	ALLEGRO_EVENT event;
    do {        
        al_wait_for_event(fila_eventos, &event);
        if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
            if (event.keyboard.keycode == ALLEGRO_KEY_R) {
                letra = 'R';
            } else if (event.keyboard.keycode == ALLEGRO_KEY_K) {
                letra = 'K';
            } else if( event.type == ALLEGRO_EVENT_DISPLAY_CLOSE ){
            break;
        }
        }
    } while (letra != 'R' && letra != 'K');

    al_destroy_event_queue(fila_eventos);
    al_clear_to_color(al_map_rgb(0, 0, 0));
	return letra;
}


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

        al_wait_for_event(event_queue, &event);
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
    al_destroy_event_queue(event_queue);
	al_clear_to_color(al_map_rgb(255,255,255));
}


void resetaPlayers (struct jogador *player1, struct jogador *player2) {
	player1->life = 600;
	player2->life = 600;
	player1->x_display = 200;
	player2->x_display = 800;
}

int temVencedorPartida (struct jogador *player1, struct jogador *player2) {
	if (player1->rounds >=2 || player2->rounds >= 2) return 1;
	else return 0;
}

int temVencedorRound (struct jogador *player1, struct jogador *player2) {
	if (player1->life <= 0 || player2->life <= 0) return 1;
	else return 0;
}

/*void revanche (player1, player2) {


}
*/

void alteraRounds(struct jogador *player1, struct jogador *player2) {
    if (player1->life <= 0) player2->rounds++;
    else if (player2->life <= 0) player1->rounds++;
}

void retiraVida (struct jogador *player1, struct jogador *player2, int jogador) {
	if (jogador == 1) player1->life -= 5;
	else if (jogador == 2) player2->life -= 5;
}

void imprimeCenario (struct jogador *player1, struct jogador *player2, ALLEGRO_BITMAP *bg, ALLEGRO_FONT* font, ALLEGRO_BITMAP * rounds[]) {
	al_clear_to_color(al_map_rgb(0,0,0));
	al_draw_bitmap(bg, 0, 0, 0);
	ALLEGRO_COLOR red = al_map_rgb(255, 0, 0);
	al_draw_filled_rectangle (5, 5, 615, 65, al_map_rgb(255, 255, 255));
	al_draw_filled_rectangle (10, 10, 10 + player1->life, 60, red);
	al_draw_filled_rectangle (665, 5, 1275, 65, al_map_rgb(255, 255, 255));
	al_draw_filled_rectangle (670, 10, 670 + player2->life, 60, red);

	al_draw_text(font, al_map_rgb(0,0,0), 15, 20, 0, "player1");
	al_draw_text(font, al_map_rgb(0,0,0), 1175, 20, 0, "player2");

	al_draw_filled_circle(580, 80, 10, al_map_rgb(0, 0, 0));
	al_draw_filled_circle(600, 80, 10, al_map_rgb(0, 0, 0));

	al_draw_filled_circle(680, 80, 10, al_map_rgb(0, 0, 0));
	al_draw_filled_circle(700, 80, 10, al_map_rgb(0, 0, 0));	

	if (player1->rounds == 1) {
		al_draw_filled_circle(580, 80, 10, al_map_rgb(255, 255, 0));
	}
	if (player2->rounds == 1) {
		al_draw_filled_circle(680, 80, 10, al_map_rgb(255, 255, 0));
	}

	if (player1->rounds + player2->rounds == 0) {
		al_draw_scaled_bitmap (rounds[0],0,0,372,118,597,100,100,50,0);
	}
	else if (player1->rounds + player2->rounds == 1)
		al_draw_scaled_bitmap (rounds[1],0,0,372,118,597,100,100,50,0);

	else if (player1->rounds + player2->rounds == 2)
		al_draw_scaled_bitmap (rounds[2],0,0,372,118,597,100,100,50,0);	
}

void imprimePersonagens(struct jogador *player1, struct jogador *player2, float *frame1, float *frame2) {
    *frame1 += 0.3f;
    *frame2 += 0.3f;

    if (*frame1 > player1->maxFrame) {
        *frame1 -= player1->maxFrame;
    }

    if (*frame2 > player2->maxFrame) {
        *frame2 -= player2->maxFrame;
    }

    al_draw_scaled_bitmap(player1->lutador, player1->x_sprite * (int)(*frame1), player1->currentFrame, player1->x_sprite, player1->y_sprite, player1->x_display, player1->y_display, 250, 250, 0);
    al_draw_scaled_bitmap(player2->lutador, player2->x_sprite * (int)(*frame2), player2->currentFrame, player2->x_sprite, player2->y_sprite, player2->x_display, player2->y_display, 250, 250, 0);    
}


int main () {

    al_init();
    al_init_font_addon();
    al_init_ttf_addon();
    al_init_image_addon();
    al_install_keyboard();
	al_init_primitives_addon();	

	char personagem;
	float frame1 = 0.f, frame2 = 0.f;
	int vida;
	int gameIniciado = 0;
	int *numRounds = 0;
	
	
	struct jogador* player1;
    struct jogador* player2;

    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 30.0);
    ALLEGRO_DISPLAY *display = al_create_display(1280,720);
    ALLEGRO_FONT* font = al_load_font("./Act_Of_Rejection.ttf", 25, 0);

    ALLEGRO_BITMAP * ken = al_load_bitmap("./Ken.png");
    ALLEGRO_BITMAP * ryu = al_load_bitmap ("./Ryu.png");
	ALLEGRO_BITMAP * kenEsq = al_load_bitmap("./KenEsq.png");
    ALLEGRO_BITMAP * ryuEsq = al_load_bitmap ("./RyuEsq.png");
    ALLEGRO_BITMAP * bg = al_load_bitmap("./cenario.jpg");
    ALLEGRO_BITMAP * logo = al_load_bitmap("StreetFighterArcTitle2.png");
    ALLEGRO_BITMAP * menuBitmap = al_load_bitmap("./menuNovo.jpg");
	ALLEGRO_BITMAP * rounds[ROUNDS];
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

		if (!gameIniciado) {
			menu (font, menuBitmap, logo);
			personagem = SelecionaPersonagem (font); 
			if (personagem == 'R' || personagem == 'r')   {
				player1 = criaJogador (ryu, 70, 80, 200, 420, 1000, 1000, 80);
				player2 = criaJogador (kenEsq, 70, 80, 800, 420, 1000, 1000, 80);
			}
			else {
				player1 = criaJogador (ken, 70, 80, 200, 420, 1000, 1000, 80);
				player2 = criaJogador (ryuEsq, 70 ,80, 800, 420, 1000, 1000, 80);
			}
			gameIniciado = 1;
		}		

		if (event.type == 30) {
			vida = update_position(player1, player2);									//O evento tipo 30 indica um evento de relógio, ou seja, verificação se a tela deve ser atualizada (conceito de FPS)			
			retiraVida (player1, player2, vida);
			al_clear_to_color(al_map_rgb(0,0,0));
			imprimeCenario(player1, player2, bg, font, rounds);
			imprimePersonagens (player1, player2, &frame1, &frame2);					
		}		
		if (temVencedorRound(player1, player2)) {
			alteraRounds (player1, player2);
			resetaPlayers (player1, player2);
			al_rest (1.5);
		}
		if (temVencedorPartida (player1, player2)) break;

		else if ((event.type == 10) || (event.type == 12)){																																				//Verifica se o evento é de botão do teclado abaixado ou levantado (!)
			if (event.keyboard.keycode == ALLEGRO_KEY_A) joystick_left(player1->controle);																															//Indica o evento correspondente no controle do primeiro jogador (botão de movimentação à esquerda) (!)
			else if (event.keyboard.keycode == ALLEGRO_KEY_D) joystick_right(player1->controle);																													//Indica o evento correspondente no controle do primeiro jogador (botão de movimentação à direita) (!)
			else if (event.keyboard.keycode == ALLEGRO_KEY_W) joystick_up(player1->controle);																														//Indica o evento correspondente no controle do primeiro jogador (botão de movimentação para cima) (!)
			else if (event.keyboard.keycode == ALLEGRO_KEY_S) joystick_down(player1->controle);	
			else if (event.keyboard.keycode == ALLEGRO_KEY_X) joystick_push(player1->controle);																														//Indica o evento correspondente no controle do primeiro jogador (botão de movimentação para cima) (!)
			else if (event.keyboard.keycode == ALLEGRO_KEY_C) joystick_kick(player1->controle);																												//Indica o evento correspondente no controle do primeiro jogador (botão de movimentação para baixo) (!)
			else if (event.keyboard.keycode == ALLEGRO_KEY_LEFT) joystick_left(player2->controle);																													//Indica o evento correspondente no controle do segundo jogador (botão de movimentação à esquerda) (!)
			else if (event.keyboard.keycode == ALLEGRO_KEY_RIGHT) joystick_right(player2->controle);																													//Indica o evento correspondente no controle do segundo jogador (botão de movimentação à direita) (!)
			else if (event.keyboard.keycode == ALLEGRO_KEY_UP) joystick_up(player2->controle);																														//Indica o evento correspondente no controle do segundo jogador (botão de movimentação para cima) (!)
			else if (event.keyboard.keycode == ALLEGRO_KEY_DOWN) joystick_down(player2->controle);																													//Indica o evento correspondente no controle do segundo jogador (botão de movimentação para baixo) (!)
			else if (event.keyboard.keycode == ALLEGRO_KEY_M) joystick_push(player2->controle);																														//Indica o evento correspondente no controle do primeiro jogador (botão de movimentação para cima) (!)
			else if (event.keyboard.keycode == ALLEGRO_KEY_N) joystick_kick(player2->controle);
		}
		else if (event.type == 42) break;																																								//Evento de clique no "X" de fechamento da tela. Encerra o programa graciosamente.
        

		//funcao reset
		player1->maxFrame = 4;
		player1->currentFrame = 80;
		//player1->x_sprite = 70;
		player1->y_display = 420;

		player2->maxFrame = 4;
		player2->currentFrame = 80;
		player2->x_sprite = 70;
		player2->y_display = 420;
		al_flip_display();
    }

	return 0;
}
