#ifndef _Ball_H
#define _Ball_H

#include <cxcore.h>
#include <windows.h>
#include "Threshold.h"

struct _Ball
{
	static int TotalBalls;
	
	CvPoint Center;
	CvPoint BirthCenter;
	CvPoint PrevPosition;
	CvPoint ExpectedPosition;
	
	DWORD BirthTime;
	DWORD StagnationDuration;
	unsigned int BallID;
	int Radius;
	int Colour;
	int Updated;
};

extern double ProportionalDistance(_Ball *Ball1, _Ball *Ball2);
extern int CloseBalls(_Ball *Ball1, _Ball *Ball2, int Distance);
extern int ClosePoints(CvPoint Pt1, CvPoint Pt2, int Distance);
extern int isNewBall(CvRect ContourBoundingRect, CvPoint ContourCenter, int ContourRadius);
extern int isDeadBall(_Ball *Ball);
extern int SetExpectedPoint(_Ball *Ball);
extern int UpdateBallList(IplImage* Image, CvSeq* BallList[], _Threshold BallThreshold, CvRect ROI, int Colour);
extern CvScalar BGR2HSV(CvScalar BGR);
extern int PossibleColour(CvScalar BGR);

#endif