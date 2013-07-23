#ifndef _GlobalVariables_H
#define _GLobalVariables_H

#include <windows.h>
#include <cxcore.h>
#include "ArenaObjects.h"
#include "Robot.h"

#define Y	0
#define X	1

#define Red		0
#define Blue	1
#define	Black	2

extern DWORD GlobalTime;
extern unsigned int FPS;
extern unsigned int FrameCount;
extern unsigned int VideoFPS;
extern DWORD BallMaxTimeOnRamp;
extern IplImage* Background;
extern int Axis;
extern _Robot Robot;
extern _ArenaObject Ramp;
extern _ArenaObject Platform;
extern _ArenaObject RightPit;
extern _ArenaObject LeftPit;
extern int MaxBallRadius;
extern int MinBallRadius;

extern int ValueBlackMax;
extern int HueRmax1;
extern int HueRmax2;
extern int HueBmax;
extern int HueBmin;

#endif