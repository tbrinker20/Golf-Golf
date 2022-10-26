#ifndef sprites_h //error
#define sprites_h

#include <stdint.h>

//automatically can use these #defines in other c files(don't need extern)
#define MAXBALLS 6 //golfgolf.c needs this to know when to stop dropping balls
//#define MAXLINES 12
//#define MAXOBSTACLES 2
//#define MAXLEVELS 2

void Balls_Init(void);
uint8_t Move_Balls(void);
void Draw_Balls(void);
void Drop_New_Ball(uint8_t ballNum);
uint8_t Check_Collisions(void);
void Level_Init(uint8_t currentLevel);
void Redraw_Obstacles(void);














//struct obstacle{
//	const uint16_t *imagePtr;
//	uint16_t h,w; 
//	uint32_t pixelColor; // color of obstacle
//	int16_t x,y;
//};
//typedef struct obstacle obstacle_t;

//int16_t yValOnLine(ball_t* Balls, uint16_t ballIdx, line_t *Lines, uint16_t lineIdx);
//uint8_t moveBalls(ball_t* Balls, line_t * Lines );
//void moveOnLine(ball_t* Balls, uint16_t ballIdx, line_t * Lines, uint16_t lineIdx);
//void ballsInit(ball_t* Balls);
//int8_t drawBalls(ball_t * Balls);
//void checkCollisions(obstacle_t* Obstacles, ball_t* Balls);
////void selectLanguage();




#endif
