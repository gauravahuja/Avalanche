#ifndef _Robot_H
#define _Robot_H

#include <cxcore.h>
#include "Threshold.h"

struct _Robot
{
	CvRect BoundingRect;
	CvRect Patch;
	CvPoint PatchCenter;
	_Threshold PatchThreshold;
	int Updated;
	int InTransit;
	int NumberOfRedBalls;
	int NumberOfBlueBalls;
	int HorizontalSpan;
	int VerticalSpan;
};

extern void UpdateBotPostion(IplImage* Image, _Robot* pRobot, CvRect ROI);


#endif