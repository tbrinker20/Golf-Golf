#ifndef line_h //error
#define line_h
#include <stdint.h>

#define MAXLINES 15

struct line{
	int16_t dx,dy;
	int16_t x1,y1;
	int16_t x2,y2;
	uint8_t caseNum;
	uint16_t xFinish;
	uint16_t yFinish;
};
typedef struct line line_t;

//Lines is array of line structs created external to this file, so telling compiler don't redeclare it
//bc im including line.h in two files, just hey heres the data type and this var exists external to me
//declaration occurs in line.c
extern line_t Lines[MAXLINES];


//Line Module capabilities:	
//1)Everything pertaining to lines(drawing them by passing x and y coord data to record coords func)
//draws lines when 2 coords pressed by user... and even has redraw lines function if making a game where the lines
//will be overwritten on lcd by like another sprite moves(say a cursor, or ball etc)


//Inputs: xCoord, yCoord, ptr to line struct
//Output: returns numLines drawn
//Functionality:
	//1) updates line struct to contain x,y coords and reduced slopes when line has 2 points
	//2) draws coordinate or draws line with reduced slope every 2 points user plots
void Record_Coords(int16_t x, int16_t y);


//Inputs: None
//Output: None
//Functionality:
		//1) Redraws all the lines made so far
void Redraw_Lines(void);
	
#endif



