#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "line.h" //need this to access line structs in spaceinvaders.c
#include "ST7735.h"



//static vars only can be changed in this file 
static uint8_t numLines, numPoints;

typedef struct line line_t;

//var declaration
line_t Lines[MAXLINES]; 

//Input: ptr to line struct
//reduces the slope of a line that either needed to be reduced or that couldnt be reduced
//and needed to be slightly changed to draw better stair step slope(smaller dx and dy values)
void Reduce_Slope(line_t* linePt)
{
	int16_t Num, Den ,mod,dx,dy;
	//DenSign set to 1 if pos dx
	int8_t DenSign = 1;
	//Densign set to -1 to make dx negative again after conversion bc conversion algoirtm requires pos dx
	if(linePt->dx < 0){
		DenSign = -1;
		dx = linePt->dx * -1;
	}
	else{
		dx = linePt->dx;
	}
	Num = linePt->dy;
	Den = dx;
	//finding GCF(dx)
	while(Num > 5 && Den > 5){ //can adjust these vals
		dy = Num;
		dx = Den;
		mod = Num % Den;
		while(mod!=0){
			mod = dx % dy;
			dx = dy;
			dy = mod;
		}
		//Divide inital Num and Den by GCF to get reduced fraction 
		Num = Num /dx;
		Den = Den /dx;
		linePt->dy = Num;
		linePt->dx = Den * DenSign;
		//ensuring have reduced enough fraction so its good enough approx of slope
		if(Num < 5 || Den < 5){ //choose 5 bc enough reduction done so it looks more liney less stair steppy and not losing too much precision in ur slope values 
			return;
		}
		//if both Num and den are odd and not reduced enough, change them to even so we can reduce them further to get better line approx
		if(Num % 2!= 0){
			Num ++;
		}
		if(Den % 2!= 0){
			Den ++;
		}
	}
	return;
}



//Input: ptr to line struct
//draws stair steps(line approximations)between 2 points, and line finsihes @ both end points of lines
void Draw_Line(line_t* linePt)
{
	int16_t xStart = 0,yStart = 0;
	int8_t drawStepCount = 0;
	//assign start and finish points based on line's case
	if(linePt->caseNum == 1)
	{
		xStart = linePt->x2;
		yStart = linePt->y2;
		linePt->xFinish = linePt->x1;
		linePt->yFinish = linePt->y1;
		
	}
	if(linePt->caseNum == 2)
	{
		xStart = linePt->x1;
		yStart = linePt->y1;
		linePt->xFinish = linePt->x2;
		linePt->yFinish = linePt->y2;
		
	}
	if(linePt->caseNum == 3)
	{
		xStart = linePt->x1;
		yStart = linePt->y1;
		linePt->xFinish = linePt->x2;
		linePt->yFinish = linePt->y2;
	}
	if(linePt->caseNum == 4)
	{
		xStart = linePt->x2;
		yStart = linePt->y2;
		linePt->xFinish = linePt->x1;
		linePt->yFinish = linePt->y1;
	}

	
	//draw line until at bottom point on line
	 while(yStart!= linePt->yFinish || xStart != linePt->xFinish){
		drawStepCount = 0;
		//draw horizontal step
		while(drawStepCount != linePt->dx){
			//needs to be first bc if hit x already, and still not hit y i dont wanna do anymovement in x dir(just go to y)
			if(xStart == linePt->xFinish){
				break;
			}
			if(linePt->dx > 0){
				xStart++;
				drawStepCount++;
			}
			else{
				xStart--;
				drawStepCount--;
			}
			ST7735_DrawPixel(xStart, yStart, 0x001F);
		}
		drawStepCount = 0;
		//draw vertical step
		while(drawStepCount != linePt->dy){
			if(yStart == linePt->yFinish){
				break;
			}
			drawStepCount++;
			yStart++;
			ST7735_DrawPixel(xStart,yStart, 0x001F);
		}		
}
	return;
}

//Inputs: None
//Output: None
//Functionality:
		//1) Redraws all the lines made so far
void Redraw_Lines(void){
	//case where no lines(drawline func not called), and 1st coord user presses cant be erased
//	if(needDrawFirstPnt){
//		ST7735_DrawSmallCircle(Lines[0].x1,Lines[0].y1,0x0000);
//	}
//if user moving cursor and drew one point, ensures that 1st point is redrawn again and again
	
	ST7735_DrawSmallCircle(Lines[numLines].x1,Lines[numLines].y1,0x0000);
	for(int i = 0; i<numLines; i++){
			//draw start and finish coords again bc when redraw calls this it needs coords to be redrawn so cursor doesent erase them
			ST7735_DrawSmallCircle(Lines[i].x1,Lines[i].y1,0x0000);
			ST7735_DrawSmallCircle(Lines[i].x2,Lines[i].y2,0x0000);
			Draw_Line(&Lines[i]);
			}
		}

		
		
//Inputs: xCoord, yCoord
//Functionality:
	//1) when odd number of coord data given, updates line struct to contain x,y coords of 1st point
	//2) when even # of points, stores 2nd endpoint and draws line(stairstep) with reduced slope 
void Record_Coords(int16_t x, int16_t y){
	//draw coordinate
	if(numPoints %2 == 0){
		Lines[numLines].x1 = x;
		Lines[numLines].y1 = y;
		numPoints++;
		ST7735_DrawSmallCircle(Lines[numLines].x1,Lines[numLines].y1,0x0000);
		return;
	}
	//update line structs w coordinates and proper dx, dy vals
	Lines[numLines].x2 = x;
	Lines[numLines].y2 = y;
	ST7735_DrawSmallCircle(Lines[numLines].x2,Lines[numLines].y2,0x0000);
	if(y < Lines[numLines].y1){
		Lines[numLines].dy = -1*(y - Lines[numLines].y1);
	}
	else{Lines[numLines].dy = y - Lines[numLines].y1;}
	//want ball to move left cases
	//case 1
	if(x>Lines[numLines].x1 && Lines[numLines].y1 > y)
	{
		Lines[numLines].dx = Lines[numLines].x1 - x;
		Lines[numLines].caseNum = 1;
	}
	//case 2
	if(x<Lines[numLines].x1 && Lines[numLines].y1 < y)
	{
		Lines[numLines].dx = x - Lines[numLines].x1;
		Lines[numLines].caseNum = 2;
	}
	//move right cases
	//case 3
	if(x>Lines[numLines].x1 && Lines[numLines].y1 < y)
	{
		Lines[numLines].dx = x - Lines[numLines].x1;
		Lines[numLines].caseNum = 3;
	}
	//case 4
	if(x<Lines[numLines].x1 && Lines[numLines].y1 > y)
	{
		Lines[numLines].dx = Lines[numLines].x1 - x;
		Lines[numLines].caseNum = 4;
	} 
	Reduce_Slope(&Lines[numLines]);
	Draw_Line(&Lines[numLines]);
	numLines++; 
	numPoints++;
}
	
		

