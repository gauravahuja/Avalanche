#ifndef _Threshold_H
#define _Threshold_H

#include <cxcore.h>

struct _Threshold
{
	CvScalar Min;
	CvScalar Max;
	CvScalar Avg;
};

extern void ReadThresholds(char *FileName, _Threshold BallThresholds[3], _Threshold *RobotPacthThreshold);
extern int inRangeSS(CvScalar Scalar, _Threshold BallThreshold);

#endif