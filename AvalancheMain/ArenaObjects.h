#ifndef _ArenaObjects_H
#define _ArenaObjects_H

#include <cxcore.h>
#include "Robot.h"

#define Left	0
#define Right	1
#define Up		2
#define Down	3

struct _ArenaObject
{
	CvRect BoundingRect;
	CvPoint Center;
	int HorizontalSpan;
	int VerticalSpan;
};

extern void FindArenaObjects(IplImage* Image, CvFont Font, _ArenaObject *Ramp, _ArenaObject* Platform, _ArenaObject* RightPit, _ArenaObject* LeftPit, _Robot *Robot);

extern void FindRampnPlatform(IplImage* Frame, _ArenaObject* pRamp, _ArenaObject* pPlatform);
//Mode = 0 Functions returns Left-Right Orientaion of Point 1 wrt Point 2
//Mode = 1 Functions returns Up-Down Orientaion of Point 1 wrt Point 2
extern int Pt1wrtPt2(CvPoint Pt1, CvPoint Pt2, int mode);

extern void PointRightTo(CvPoint* Point, CvPoint* Dst, int Distance);
extern void PointLeftTo(CvPoint* Point, CvPoint* Dst, int Distance);
extern int GetVerticalDistance(CvPoint Point1, CvPoint Point2);
extern int GetHorizontalDistance(CvPoint Point1, CvPoint Point2);


#endif