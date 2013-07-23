#ifndef _Messages_H
#define _Messages_H

#define StartByte		65
#define EndByte			97

#define StopTimer		1	//Stops the Timer
#define SetTime			2	//MCU Expects 2 bytes after this 
/**
	Data format : x.y ms
	x => 1st byte
	y => 2nd byte
**/
#define StartTimer		3	//Starts the Timer

#define MoveForward		10
#define MoveRight		11
#define MoveLeft		12
#define MoveBackward	13
#define StopMoving		14

#define TiltLeft		20
#define Neutral			21
#define TiltRight		22

#endif
