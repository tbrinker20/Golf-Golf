// images.h
// contains image buffers for simple space invaders
// Jonathan Valvano, March 5, 2018
// Capture image dimensions from BMP files
#ifndef __images_h
#define __images_h
#include <stdint.h>


// *************************** Images ***************************
//using these vars in multiple c files so have to export them so compiler knows abt them and can allow other
//c files to use them
extern const unsigned short Bunker0[];

extern const unsigned short greenBall[];

extern const unsigned short whiteBall[]; 

extern const unsigned short blackBall[];

extern const unsigned short blueBall[];

extern const unsigned short yellowBall[];

extern const unsigned short cursorGreen[];

extern const unsigned short cursorMain[];

extern const unsigned short hole[];

extern const unsigned short Pond1[];

extern const unsigned short Sand1[];

extern const unsigned short game_over[];

extern const unsigned short Flag0[];

extern const unsigned short BallMachine[];

#endif /* __images_h */

