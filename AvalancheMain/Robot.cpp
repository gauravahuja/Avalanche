#include <cv.h>
#include <cxcore.h>
#include <highgui.h>

#include "GlobalVariables.h"
#include "Robot.h"


inline void UpdateBotPostion(IplImage* Image, _Robot* pRobot, CvRect ROI)
{
	IplImage* Thresholded = cvCreateImage(cvGetSize(Image), IPL_DEPTH_8U, 1);
	
	cvSetZero(Thresholded);
	
	cvSetImageROI(Image, ROI);
	cvSetImageROI(Thresholded, ROI);
	
	cvInRangeS(Image, pRobot->PatchThreshold.Min, pRobot->PatchThreshold.Max, Thresholded);
	
	cvResetImageROI(Image);
	cvResetImageROI(Thresholded);
	
	CvMoments Moments;
	cvMoments(Thresholded, &Moments, 1);
	if(!Moments.m00)
		Moments.m00 = 1;
	
	int x = cvRound(Moments.m10/Moments.m00);
	int y = cvRound(Moments.m01/Moments.m00);
	
	pRobot->PatchCenter.x = x;
	pRobot->PatchCenter.y = y;
	pRobot->Patch.x = x - pRobot->Patch.width/2;
	pRobot->Patch.y = y - pRobot->Patch.height/2;
	pRobot->BoundingRect.x = pRobot->Patch.x - pRobot->BoundingRect.width/2;
	pRobot->BoundingRect.y = pRobot->Patch.y - pRobot->BoundingRect.height/2;
 	pRobot->Updated = 1;

	cvReleaseImage(&Thresholded);
}
