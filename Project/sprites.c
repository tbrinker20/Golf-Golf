#include "sprites.h"
#include <stdint.h>
#include "ST7735.h"
#include "Images.h"
#include "line.h"


#define MAXOBSTACLES 3
typedef enum {alive, dying, dead} status_t; 
struct ball{
	int16_t x,y; 
	int16_t oldX,oldY;
	int16_t h,w; 
	const uint16_t* aliveImgPt;
	const uint16_t* deadImgPt;
	line_t* lineHitPt; //points to line Ball is on
	int8_t mod; // holds num horz movements left when ball first hits lines
	int8_t stepCtr; //used to move ball down steps of line it has hit
	uint8_t moveInY; // used to denote if ball needs to move in y or x step of line hit
	uint8_t onLine; //used to denote if ball is on line or not
	status_t life; //alive, dying, dead states of ball
};
typedef struct ball ball_t;

struct obstacle{
	int16_t x,y;
	int16_t h,w;
	const uint16_t* imgPt;
	const uint16_t* deadBallPt; //denotes color of obstacle
};
typedef struct obstacle obstacle_t; //define struct obstacle as a data type

obstacle_t Obstacles[MAXOBSTACLES];
	
ball_t Balls[MAXBALLS];


void Balls_Init(void){
	for(int i = 0; i<MAXBALLS; i++){
			Balls[i].x = 74; //replace 74 with loc under ball machine
			Balls[i].y = 30;//replace 30
			Balls[i].w = 8;
			Balls[i].h = 8;
			Balls[i].aliveImgPt = whiteBall;
		  Balls[i].deadImgPt = greenBall; //can be changed to another obstacles color during collisions func
			Balls[i].stepCtr = 0;
		  Balls[i].moveInY = 1; //ball shld move in y dir firsr after mod handled(horz step always comes first) on line
			Balls[i].onLine = 0;
			Balls[i].life = dead;
	}	

}

int16_t yValOnLine(line_t* Line, ball_t * Ball){
	int16_t xDif,numVertSteps;
	//case 1
	if(Line->caseNum == 1){
		xDif = Ball->x - Line->x2; //neg num or 0
		numVertSteps = xDif / Line->dx ; //always pos number bc dx is neg and shld be pos number
		Ball->mod =  Line->dx+ (xDif % Line->dx); //shld be neg bc we wanna move left in case 1(used in move on balls func(mod)
		return Line->y2 + numVertSteps*Line->dy;
	}
	
	//case 2
	if(Line->caseNum == 2){
		xDif = Ball->x - Line->x1; //neg num or 0
		numVertSteps = xDif / Line->dx ; //always pos number bc dx is neg and shld be pos number
		Ball->mod = Line->dx + (xDif % Line->dx); //shld be neg bc we wanna move left in case 1(used in move on balls func(mod)
		return Line->y1 + numVertSteps*Line->dy;
	}
	
	//case 3
	if(Line->caseNum == 3){
		xDif = Ball->x - Line->x1; //pos num or 0
		numVertSteps = xDif / Line->dx ; //always pos number bc dx is pos and shld be pos number
		Ball->mod = Line->dx -(xDif % Line->dx); //shld be neg bc we wanna move left in case 1(used in move on balls func(mod)
		return Line->y1 + numVertSteps*Line->dy;
	}
	
	//case 4
	if(Line->caseNum == 4){
		xDif = Ball->x - Line->x2; //pos num or 0
		numVertSteps = xDif / Line->dx ; //always pos number bc dx is pos and shld be pos number
		Ball->mod = Line->dx - (xDif % Line->dx); //shld be neg bc we wanna move left in case 1(used in move on balls func(mod)
		return Line->y2 + numVertSteps*Line->dy ;
	}

}


void Move_Ball_OnLine(ball_t * Ball){
	//check if Ball at end of line
	if(Ball->x != Ball->lineHitPt->xFinish && Ball->y != Ball->lineHitPt->yFinish){
		//check if ball still on 1st horz step
		if(Ball->mod !=0){
			//ball still on 1st horz step
			//move ball to left
			if(Ball->mod < 0){
				Ball->mod +=1;
				Ball-> x -=1;
			}
			//move ball to right
			else{
				Ball->mod -=1;
				Ball-> x +=1;
			}
			return;				
		}
		//ball past 1 horz step and not at end of line
		if(Ball->moveInY){
			if(Ball->stepCtr < Ball->lineHitPt->dy){
				Ball->y +=1;
				Ball->stepCtr +=1;
			}
			else{
				Ball->moveInY = 0; //move in X dir now
				Ball->stepCtr = 0;
			}
			return;
		}
		//here if moving in Xdir
		//checking if at end of horz step
		if(Ball->stepCtr != Ball->lineHitPt->dx){
			//move ball to left
			if(Ball->lineHitPt->dx < 0){
				Ball->x -=1;
				Ball->stepCtr -=1;
			}
			else{
				//move ball to right
				Ball->x +=1;
				Ball->stepCtr +=1;
			}
		}
		//at end of horz step, so move in YDir bc not at end of line yet
		else{
			Ball->stepCtr = 0;
			Ball->moveInY = 1; //move in Y Dir
		}
		return;
	}
	//decalare ball not on line when reach end of line
	Ball->onLine = 0; 
	return;
}



uint8_t Move_Balls(void){
	int i = 0,j= 0,needDrawBalls = 0;
	for(i = 0; i<MAXBALLS; i++){
		if(Balls[i].life == alive){
				needDrawBalls = 1;
				//saving ball's old x and y to cover up their old position
				Balls[i].oldX = Balls[i].x;
				Balls[i].oldY = Balls[i].y;
				//if ball on line, just move it on Line and dont check if ball on other lines bc didnt have 2 pixel border, so have to redraw them
				if(Balls[i].onLine){
					//ball 1st time detected on line
					Move_Ball_OnLine(&Balls[i]);
				}
				//ball not detected to be on line yet
				else{
					for(j =0; j<MAXLINES; j++){
						if(Balls[i].y == yValOnLine(&Lines[j],&Balls[i])){
							//ball 1st time detected on line
							Balls[i].onLine = 1; 
							Balls[i].lineHitPt = &Lines[j];
							Move_Ball_OnLine(&Balls[i]);
							break;
						}
					}
					//here if ball not detected on line still
					//kill balls that are going off grid of LCD(golf hole)
					if(Balls[i].x < 0){
						Balls[i].life = dying;
					}
					if(Balls[i].x > 127 - Balls[i].w){
						Balls[i].life = dying;
					}
					if(Balls[i].y > 159){
						Balls[i].life = dying;
					}
					if(Balls[i].y < Balls[i].h){
						Balls[i].life = dying;
					}
					//if ball alive and not on line, move it 1 down on grid
					if(Balls[i].life != dying && Balls[i].onLine == 0){
						Balls[i].y +=1;
					}
			}
	}
	
}
	return needDrawBalls;
}


//Inputs: none
//Outputs: 1: need to draw Ball(bc collision occured), 0: don't drawball(no collision)
//checks if a ball collided with one of the obstacles on the hole
uint8_t Check_Collisions(void){
	uint8_t ballCollision = 0; //assumed initally, no collisions have occured
	int16_t r,c;
	for(int i = 0; i< MAXBALLS; i++){
		if(Balls[i].life == alive){
			//if ball is alive, check if it collided w an obstacle
			for(int j =0; j<MAXOBSTACLES; j++){
				//checking if ball collided w obstacle 
				if((Balls[i].x >= Obstacles[j].x && Balls[i].x<= Obstacles[j].x + Obstacles[j].w) && (Balls[i].y <= Obstacles[j].y && Balls[i].y>=Obstacles[j].y - Obstacles[j].h)){
					r = Obstacles[j].h -(Obstacles[j].y-Balls[i].y);
					c = Balls[i].x - Obstacles[j].x;
				//ensures ball is far enough down in obstacle so no buldge of obstacle when ball 1st collides w it 
					if(r >= 8 && c >=2 && Obstacles[j].x + Obstacles[j].w - Balls[i].x >5){
						Balls[i].life =  dying;
						Balls[i].deadImgPt = Obstacles[j].deadBallPt;
						ballCollision = 1;
					}
				}
			}
		}
	}
	return ballCollision;
}



void Draw_Balls(void){
	int i;
	for(i=0; i< MAXBALLS; i++){
		//handles covering up old images of balls and redrawing their updated("moved") positions
		ST7735_DrawBitmap(Balls[i].oldX,Balls[i].oldY,Balls[i].deadImgPt,Balls[i].w, Balls[i].h);
		if(Balls[i].life == alive){
			ST7735_DrawBitmap(Balls[i].x,Balls[i].y,Balls[i].aliveImgPt,Balls[i].w,Balls[i].h);
		}
		if(Balls[i].life == dying){
			ST7735_DrawBitmap(Balls[i].x,Balls[i].y,Balls[i].deadImgPt,Balls[i].w,Balls[i].h);
			Balls[i].life = dead;
		}
	}
}


void Drop_New_Ball(uint8_t ballNum){
	Balls[ballNum].life = alive;
}



void Level_Init(uint8_t currentLevel){
	if(currentLevel == 0){
		//setting obstacle properties
		ST7735_FillScreen(0xE0C9); // set screen to green
		Obstacles[0].h = 20;
		Obstacles[0].imgPt = Pond1;
		Obstacles[0].deadBallPt = blueBall;
		Obstacles[0].w = 44;
		Obstacles[0].x = 83;
		Obstacles[0].y = 55;
		Obstacles[1].h = 12;
		Obstacles[1].imgPt = hole;
		Obstacles[1].deadBallPt = blackBall;
		Obstacles[1].w = 12;
		Obstacles[1].x = 100;
		Obstacles[1].y = 100;
		Obstacles[2].h = 20;
		Obstacles[2].imgPt = Sand1;
		Obstacles[2].deadBallPt = yellowBall;
		Obstacles[2].w = 44;
		Obstacles[2].x = 5;
		Obstacles[2].y = 80;
		Obstacles[2].h = 21;
		
		//2) Drawing obstacles and images
		ST7735_DrawBitmap(100,100,hole,12,12);
		ST7735_DrawBitmap(106,86,Flag0,8,15);
		ST7735_DrawBitmap(83,55,Pond1,44,20);
		ST7735_DrawBitmap(5,80,Sand1,44,21);
		ST7735_DrawBitmap(59,18,BallMachine,30,15);
		
		//3) Initalizing Ball properties
		Balls_Init();
	}
}

void Redraw_Obstacles(void){
	for(int i=0; i<MAXOBSTACLES; i++){
		ST7735_DrawBitmap(Obstacles[i].x,Obstacles[i].y,Obstacles[i].imgPt,Obstacles[i].w,Obstacles[i].h);
	}
}
	