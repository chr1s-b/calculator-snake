#include <display_syscalls.h>

#include <color.h>

#include "graphic_functions.h"
#include "key.h"
#include "rand.h"
#include "time.h"
#include "text.h"

void wait(int ticks)
{
	static int time=0;
	while(time_getTicks()-time < ticks);
	time = time_getTicks();
}

int main(void) {
	char str[20];
	Bdisp_EnableColor();
	
	const INTERVAL = LCD_HEIGHT_PX / 20;
	const GRID_WIDTH = LCD_WIDTH_PX / INTERVAL;
	const GRID_HEIGHT = LCD_HEIGHT_PX / INTERVAL;
	
	int xv = 0;
	int	yv = 0;
	int X = GRID_WIDTH / 2;
	int Y = GRID_HEIGHT / 2;
	
	int trail_y[GRID_HEIGHT*GRID_WIDTH]; // maximum size of trail
	int trail_x[GRID_HEIGHT*GRID_WIDTH]; // maximum size of trail
	trail_x[0] = X;
	trail_y[0] = Y;
	
	
	srand(time_getTicks());
	int start = time_getTicks();
	
	int fX = rand()%GRID_WIDTH;
	int fY = rand()%(GRID_HEIGHT-1)+1;
	
	int menu = 1;
	int run = 0;
	int gameover = 0;
	int speed = 0;
	int score = 0;
	int paused = 0;
	
	int i;
	while(key_down(K_EXE));
	while(!key_down(K_EXIT)) {
		if (paused) {
			RoundedSquareAlpha((LCD_WIDTH_PX-200)/2, (LCD_HEIGHT_PX-70)/2, 200, 80, 10, 0x0000, 16);
			text_print((LCD_WIDTH_PX-3*text_width("PAUSE"))/2, (LCD_HEIGHT_PX-80)/2+20, "PAUSE", 3, 0xFFFF);
			text_print((LCD_WIDTH_PX-2*text_width("EXE to continue"))/2, (LCD_HEIGHT_PX-80)/2+45, "EXE to continue", 2, 0xFFFF);
			if (key_down(K_EXE)) {
				paused = 0;
				wait(20);
			}
			
		} else if (run) {
			// handle key presses
			if (key_down(K_UP)) {
				xv = 0; yv = -1;
			} else if (key_down(K_DOWN)) {
				xv = 0; yv = 1;
			} else if (key_down(K_LEFT)) {
				xv = -1; yv = 0;
			} else if (key_down(K_RIGHT)) {
				xv = 1; yv = 0;
			} else if (key_down(K_F5)) {
				paused = 1;
				wait(20);
			}
			if ((time_getTicks()-start)%(15-speed) == 0) {
				// update
				X = X + xv;
				Y = Y + yv;
				
				// check for collisions
				if (X < 0 || X >= GRID_WIDTH || Y < 1 || Y >= GRID_HEIGHT) {
					gameover = 1;
					run = 0;
				}
				
				for (i=1;i<=score;i++) {
					if (X == trail_x[i] && Y == trail_y[i]) {
						gameover = 1;
						run = 0;
					}
				}
				
				// shift list
				int i_ = score;
				while (i_>=0) {
					trail_x[i_+1] = trail_x[i_];
					trail_y[i_+1] = trail_y[i_];
					i_--;
				}
				trail_x[0] = X;
				trail_y[0] = Y;
				
				if (X == fX && Y == fY) {
					score++;
					fX = rand()%GRID_WIDTH;
					fY = rand()%(GRID_HEIGHT-1)+1;
				}
				
				// render
				Bdisp_AllClr_VRAM();
				// borders
				Line(0,INTERVAL-1,GRID_WIDTH*INTERVAL+1,INTERVAL-1,COLOR_BLACK);
				Line(GRID_WIDTH*INTERVAL+1,INTERVAL-1,GRID_WIDTH*INTERVAL+1,GRID_HEIGHT*INTERVAL+1,COLOR_BLACK);
				Line(GRID_WIDTH*INTERVAL+1,GRID_HEIGHT*INTERVAL+1,0,GRID_HEIGHT*INTERVAL+1,COLOR_BLACK);
				Line(0,GRID_HEIGHT*INTERVAL+1,0,INTERVAL-1,COLOR_BLACK);
				// trail
				if (!gameover){
					for (i=0; i<=score; i++) {
						RoundedSquareAlpha(trail_x[i]*INTERVAL, trail_y[i]*INTERVAL, INTERVAL, INTERVAL, 1, 0x00F0, 16);
					}
				}
				RoundedSquareAlpha(fX*INTERVAL, fY*INTERVAL, INTERVAL, INTERVAL, INTERVAL/2, COLOR_RED, 16); // food
				text_print(2+text_width("Score: "), 2, itoa(score, str), 1, 0x0000);
				text_print(2, 2, "Score:", 1, 0x0000); // SCORE
				text_print(LCD_WIDTH_PX-text_width("PAUSE: F5")-2, 2, "PAUSE: F5", 1, 0x0000);
				text_print((LCD_WIDTH_PX-text_width("SNAKE"))/2, 2, "SNAKE", 1, 0x0000);
				//wait(17-speed);
				speed = score/5;
			}				
		} else if (gameover) {
			RoundedSquareAlpha((LCD_WIDTH_PX-200)/2, (LCD_HEIGHT_PX-70)/2, 200, 80, 10, 0x0000, 16);
			text_print((LCD_WIDTH_PX-3*text_width("GAME OVER"))/2, (LCD_HEIGHT_PX-80)/2+20, "GAME OVER", 3, 0xFFFF);
			text_print((LCD_WIDTH_PX-2*text_width("SCORE:   "))/2, (LCD_HEIGHT_PX-80)/2+40, "SCORE:", 2, 0xFFFF);
			text_print((LCD_WIDTH_PX+2*text_width("SCORE: "))/2, (LCD_HEIGHT_PX-80)/2+40, itoa(score, str), 2, 0xFFFF);
			gameover = 0;
			menu = 1;
			Bdisp_PutDisp_DD();
			wait(300);
		} else if (menu) {
			RoundedSquareAlpha((LCD_WIDTH_PX-200)/2, (LCD_HEIGHT_PX-70)/2, 200, 80, 10, 0x0000, 16);
			text_print((LCD_WIDTH_PX-3*text_width("SNAKE"))/2, (LCD_HEIGHT_PX-80)/2+20, "SNAKE", 3, 0xFFFF);
			text_print((LCD_WIDTH_PX-2*text_width("EXE to start"))/2, (LCD_HEIGHT_PX-80)/2+55, "EXE to start", 2, 0xFFFF);
			text_print((LCD_WIDTH_PX-text_width("By ChrisB"))/2, (LCD_HEIGHT_PX-80)/2+75, "By ChrisB", 1, 0xFFFF);
			text_print((LCD_WIDTH_PX-text_width("V1.0"))/2, INTERVAL+2, "V1.0", 1, 0x0000);
			if (score>0) {
				text_print((LCD_WIDTH_PX-2*text_width("SCORE: "))/2, (LCD_HEIGHT_PX-80)/2+40, "SCORE: ", 2, 0xFFFF);
				text_print((LCD_WIDTH_PX+2*text_width("SCORE: "))/2, (LCD_HEIGHT_PX-80)/2+40, itoa(score, str), 2, 0xFFFF);
			}
			
			if (key_down(K_EXE)) {
				run = 1; menu = 0;
				score = 0;
				X=GRID_WIDTH/2;
				Y=GRID_HEIGHT/2;
				xv=0;
				yv=0;
			}
		}
		Bdisp_PutDisp_DD();
	}  
	return 1;
}