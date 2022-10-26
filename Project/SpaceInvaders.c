// SpaceInvaders.c
// Runs on TM4C123
// Jonathan Valvano and Daniel Valvano
// This is a starter project for the EE319K Lab 10

// Last Modified: 1/12/2022 
// http://www.spaceinvaders.de/
// sounds at http://www.classicgaming.cc/classics/spaceinvaders/sounds.php
// http://www.classicgaming.cc/classics/spaceinvaders/playguide.php

// ******* Possible Hardware I/O connections*******************
// Slide pot pin 1 connected to ground
// Slide pot pin 2 connected to PD2/AIN5
// Slide pot pin 3 connected to +3.3V 
// buttons connected to PE0-PE3
// 32*R resistor DAC bit 0 on PB0 (least significant bit)
// 16*R resistor DAC bit 1 on PB1
// 8*R resistor DAC bit 2 on PB2 
// 4*R resistor DAC bit 3 on PB3
// 2*R resistor DAC bit 4 on PB4
// 1*R resistor DAC bit 5 on PB5 (most significant bit)
// LED on PD1
// LED on PD0


#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "ST7735.h"
#include "Print.h"
#include "Random.h"
#include "TExaS.h"
#include "ADC.h"
#include "Images.h"
#include "Sound.h"
#include "Timer1.h"
#include "line.h"
#include "sprites.h"
#include "images.h"

#define PF1       (*((volatile uint32_t *)0x40025008))


//Globals
uint16_t oldXCoord, xCoord,oldYCoord, yCoord;
//flags(might have to reinitalize in levels but shouldnt have to)
uint8_t currentLevel,needDrawBalls,needRecordCoords,needDrawCursor,needMoveBalls,needDropBall,ballCollision,startBallTimer,numBallsMoved,needDrawBalls,drewABall ;


//prototypes
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void Delay100ms(uint32_t count); // time delay in 0.1 seconds


//Initialization Functions
void SysTick_Init(uint32_t period){
NVIC_ST_CTRL_R = 0x00; //turning off interrupts during initialization
NVIC_ST_RELOAD_R = period-1; //changing reload to period
NVIC_ST_CURRENT_R = 0; //writing to control register to set ctrl reg value to 0 so on next tick timer starts counting down from new reload value just set
//NVIC_PRI3_R = (NVIC_PRI3_R&0x0FFFFFFF)|0x40000000; //priority 2
NVIC_ST_CTRL_R = 0x07; // activating clock to start ticking and operating @ 80 MHz clock and can start interuptting
}

void PortF_Init(void){
  SYSCTL_RCGCGPIO_R |= 0x20;      // activate port F
  while((SYSCTL_PRGPIO_R&0x20) != 0x20){};
  GPIO_PORTF_DIR_R |=  0x0E;   // output on PF3,2,1 (built-in LED)
  GPIO_PORTF_PUR_R |= 0x10;
  GPIO_PORTF_DEN_R |=  0x1E;   // enable digital I/O on PF
}

void PortE_Init(void){
//SYSCTL_RCGCGPIO_R |= 0x10;
//__nop();       // extra time to stabilize
// __nop();       // extra time to stabilize
// __nop();         // extra time to stabilize
// __nop();  
GPIO_PORTE_DIR_R &= ~0x0E;    // (c) make PE3-1 input
  GPIO_PORTE_DEN_R |= 0x0E;     //     enable digital I/O on PE3-1
GPIO_PORTE_IS_R &= ~0x0E;     // (d) PE3-1 is edge-sensitive
  GPIO_PORTE_IBE_R &= ~0x0E;    //     make PE3-1 0's in this register to make input a falling edge trigger
  GPIO_PORTE_IEV_R &= ~0x0E;    //     PE3-1 is falling edge event so make it 0's in those locations
GPIO_PORTE_ICR_R = 0x0E;      // (e) clear flag4 in RIS Reg (write to this register to acknowledge interrupt and tell hardware to reset and not send signal until next falling edge so we dont keep interrupting even tho not another button press)
 GPIO_PORTE_IM_R |= 0x0E;      // (f) specifying interrupt synchronization not busy wait synchornization on PE3-1
NVIC_PRI1_R = (NVIC_PRI1_R&0xFFFFF00F)|0x00000020; // priority 1
NVIC_EN0_R = 0x00000010;      // (h) enable interrupt 0 in NVIC
}

	

//comes in here when user presses button(PE3-1)
void GPIOPortE_Handler(void){
	if(GPIO_PORTE_RIS_R & 0x8){ 
		GPIO_PORTE_ICR_R = 0x8; //acknowledging interrupt being handled, bit 3 in RIS_R gets set back to 0 so dont need to interrupt again until H/W makes it 1 when PE3 pressed 
		needRecordCoords =1; //set recordCoordsFlag to tell we need to recoord Coords
	}
	if(GPIO_PORTE_RIS_R & 0x04){
		needMoveBalls = 1;
		TIMER1_CTL_R = 0x00000001; //starting ball release timer(every 1 sec)
		GPIO_PORTE_ICR_R = 0x04;
	}
//	if(GPIO_PORTE_RIS_R & 0x02){
//		spanishFlag ^= 1;
//	GPIO_PORTE_ICR_R = 0x02;
//	}


	
}


// runs @ 30Hz
void SysTick_Handler(void){ 
	//1) getting new Cursor data and setting flag to update cursor on lcd
	uint16_t data[2]; //12 bit ADC dumps digValues in this array
	oldXCoord = xCoord; //used to redraw cursor
	oldYCoord = yCoord; //used to redraw cursor
	ADC_In89(data); //new ADC digVals stored in data
	//converting ADC digVals to LCD grid system
	xCoord = (128*data[0])/4069; 
  yCoord = (160*data[1])/4069;
	needDrawCursor =1;
	
	//2) if user presses start game button, balls shld always move and new balls begin
	//falling every 1 sec
	if(needMoveBalls){
		//start ball timer as soon as user presses start button
		if(needDropBall && numBallsMoved < MAXBALLS){
		  Drop_New_Ball(numBallsMoved);
			numBallsMoved++;
			needDropBall = 0;
		}
			//always move balls
			needDrawBalls = Move_Balls();
			
			//checking if ball collided w an obstacle
			ballCollision = Check_Collisions();
		
		}
	
}


void redraw(void){
	//check if need to redraw cursor bc ADC got new sample(@30Hz)
	if(needDrawCursor){
		//cover up old cursor and draw new cursor location
		ST7735_DrawBitmap(oldXCoord,oldYCoord,cursorGreen,5,5); 
		ST7735_DrawBitmap(xCoord,yCoord,cursorMain,5,5); 
		ST7735_DrawBitmap(106,86,Flag0,8,15);
		ST7735_DrawBitmap(59,18,BallMachine,30,15);
		
		//redrawing balls
		if(drewABall){
			Draw_Balls(); 
		}
		
		//redrawing Obstacles
		Redraw_Obstacles();
		
		//redrawing Lines and coords
		Redraw_Lines(); 
		
		needDrawCursor = 0;
	}
}
void Timer1A_Handler(void){
	needDropBall = 1;
	TIMER1_ICR_R = TIMER_ICR_TATOCINT;
}


int main(void){
  DisableInterrupts();
  TExaS_Init(NONE);       // Bus clock is 80 MHz
  ST7735_InitR(INITR_REDTAB);
  ADC_Init();    // initialize to sample ADC
  PortF_Init();
	PortE_Init();
	Level_Init(currentLevel); //CL:0 initally
	Timer1_Init(80000000,2); //lower priority(higher num) than sound
  SysTick_Init(80000000/30); // Interrupt at 30Hz
	EnableInterrupts();
	while(1){
			redraw();
			//draws coord or line @ 30 Hz, and after cursor redrawing so dont cover up cursor!
			if(needRecordCoords){
				Record_Coords(xCoord,yCoord);
				needRecordCoords = 0;
			}
			if(needDrawBalls || ballCollision){
				Draw_Balls();
				needDrawBalls = 0;
				drewABall = 1;
		}
	}
}


