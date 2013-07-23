#include <cxcore.h>
#include <windows.h>

#include "GlobalVariables.h"
#include "Robot.h"
#include "ArenaObjects.h"

DWORD GlobalTime;
unsigned int FPS;
unsigned int FrameCount;
unsigned int VideoFPS;
IplImage* Background;

//Axis is along the ramp height
//Assumption invloved in program that Axis is either x or y
//Axis = 0 for x
//Axis = 1 for y;
int Axis; 

_Robot Robot;
_ArenaObject Ramp;
_ArenaObject Platform;
_ArenaObject RightPit;
_ArenaObject LeftPit;

DWORD BallMaxTimeOnRamp = 6000; //Milli seconds

int MaxBallRadius = 10;
int MinBallRadius = 2;

int ValueBlackMax = 70;
int HueRmax1 = 120;	//less than this
int HueRmax2 = 300;	//greater than this
int HueBmax = 270;
int HueBmin = 170;
