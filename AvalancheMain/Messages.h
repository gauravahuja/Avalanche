#ifndef _Messages_H
#define _Messages_H

#define TrainByte1		240	//0b11110000
#define TrainByte2		15	//0b00001111
#define TrainByte3		170	//0b10101010


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
#define MoveLeft		10
#define TurnRight		11
#define TurnLeft		12
#define MoveRight		13
#define StopMoving		14

#define TiltLeft		20
#define Neutral			21
#define TiltRight		22


extern int InitializeMessages(char* PortName, int Baud);
extern int SendMessageToMCU(char Message);
extern void DeInitializeMessages();
extern void SetServoAngle(char option[5]);

#endif