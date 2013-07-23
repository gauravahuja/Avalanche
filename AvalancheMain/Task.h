#ifndef _Task_H
#define _Task_H

#include <cxcore.h>

//Move Right To Catch Ball
//Completion Criteria : Reach the coord of expected postion of ball + absence of BallID in Ball list
#define TaskMRCB	1

//Move Left To Catch Ball
//Completion Criteria : Reach the coord of expected postion of ball + absence of BallID in Ball list
#define TaskMLCB	2

//Move To Right Pit to tilt pan
//Completion Criteria : Reach coord of Right pit and tilt pan
#define TaskMRTP	3

//Move To Left Pit to tilt pan
//Completion Criteria : Reach coord of Left pit and tilt pan
#define TaskMLTP	4

//Move to Center
//Completion Criteria : Reach Center Coord
#define TaskMC		5

//Move to Right
//Completion Criteria : Reach Specified Coord
#define TaskMR		6

//Move to Left
//Completion Criteria: Reach Specified Coord
#define TaskML		7

struct _Task
{
	int Task;
	CvPoint CompletionCoord;
	int BallID;
	int BallColour;
	int TaskProgress;
};

extern void ClearTask(_Task* Task);
extern void UpdateTask(CvSeq* BallLists[3], _Task* CurrentTask);
extern void SetTask(CvSeq* BallList[3], _Task* CurrentTask);

#endif