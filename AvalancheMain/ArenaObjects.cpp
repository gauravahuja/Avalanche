#include <cv.h>
#include <cxcore.h>
#include <highgui.h>

#include "GlobalVariables.h"
#include "Robot.h"
#include "ArenaObjects.h"

/*
	Selection Order:

	1. Ramp
	2. Platform
	3. Right Pit
	4. Left Pit
	5. Robot
	6. Robot Patch
*/

int Select_Object = 0;
int SelectionNumber = 0;
CvRect Selection;
CvPoint Origin;

void OnMouse(int Event, int x, int y, int flags, void* param)
{
	if(Select_Object)
	{
		Selection.x = MIN(x,Origin.x);
        Selection.y = MIN(y,Origin.y);
		Selection.width = abs(x - Origin.x);
        Selection.height = abs(y - Origin.y);
	}
	switch(Event)
	{
	case CV_EVENT_LBUTTONDOWN:
		if(!Select_Object)
		{
			Origin = cvPoint(x, y);
			Selection = cvRect(x, y, 0, 0);
			Select_Object = 1;
		}
		break;
	case CV_EVENT_LBUTTONUP:
		Select_Object = 0;
		SelectionNumber++;
		break;
	}
}

void FindArenaObjects(IplImage* Image, CvFont Font, _ArenaObject *pRamp, _ArenaObject* pPlatform, _ArenaObject* pRightPit, _ArenaObject* pLeftPit, _Robot* pRobot)
{
	IplImage* ImageCopy = cvCloneImage(Image);
	IplImage* ImageCopy2 = cvCloneImage(Image);
	SelectionNumber = 0;
	Select_Object = 0;
	int PrevSelectionNumber = -1;

	cvNamedWindow("Arena");
	cvShowImage("Arena", ImageCopy);
	cvSetMouseCallback("Arena", OnMouse);	

	while(SelectionNumber < 6 && cvWaitKey(10) != 27)
	{
		if(SelectionNumber - PrevSelectionNumber > 0)
		{
			PrevSelectionNumber = SelectionNumber;
			cvCopyImage(Image, ImageCopy);
			switch(SelectionNumber)
			{
			case 0:
				cvPutText(ImageCopy, "Select Temp Ramp", cvPoint(0, 20), &Font, cvScalarAll(255));
				break;
			case 1:
				if(pRamp)
				{
					pRamp->BoundingRect = Selection;
					pRamp->Center = cvPoint(pRamp->BoundingRect.x + pRamp->BoundingRect.width/2, pRamp->BoundingRect.y + pRamp->BoundingRect.height/2);
				}
				cvPutText(ImageCopy, "Select Temp Platform", cvPoint(0, 20), &Font, cvScalarAll(255));
				break;
			case 2:
				if(pPlatform)
				{
					pPlatform->BoundingRect = Selection;
					pPlatform->Center = cvPoint(pPlatform->BoundingRect.x + pPlatform->BoundingRect.width/2, pPlatform->BoundingRect.y + pPlatform->BoundingRect.height/2);
				}
				cvPutText(ImageCopy, "Select Right Pit", cvPoint(0, 20), &Font, cvScalarAll(255));
				break;
			case 3:
				if(pRightPit)
				{
					pRightPit->BoundingRect = Selection;
					pRightPit->Center = cvPoint(pRightPit->BoundingRect.x + pRightPit->BoundingRect.width/2, pRightPit->BoundingRect.y + pRightPit->BoundingRect.height/2);
				}
				cvPutText(ImageCopy, "Select Left Pit", cvPoint(0, 20), &Font, cvScalarAll(255));
				break;
			case 4:
				if(pLeftPit)
				{
					pLeftPit->BoundingRect = Selection;
					pLeftPit->Center = cvPoint(pLeftPit->BoundingRect.x + pLeftPit->BoundingRect.width/2, pLeftPit->BoundingRect.y + pLeftPit->BoundingRect.height/2);
				}
				cvPutText(ImageCopy, "Select Robot", cvPoint(0, 20), &Font, cvScalarAll(255));
				break;
			case 5:
				if(pRobot)
				{
					pRobot->BoundingRect = Selection;
				}
				cvPutText(ImageCopy, "Select Robot Patch", cvPoint(0, 20), &Font, cvScalarAll(255));
				break;
			case 6:
				if(pRobot)
				{
					pRobot->Patch = Selection;
					pRobot->PatchCenter = cvPoint(pRobot->Patch.x + pRobot->Patch.width/2, pRobot->Patch.y + pRobot->Patch.height/2);
					pRobot->Updated = 1;
				}
				cvPutText(ImageCopy, "Press Escape to Continue...", cvPoint(0, 20), &Font, cvScalarAll(255));
				break;
			default:
				break;
			}
			cvShowImage("Arena", ImageCopy);
		}
		if(Select_Object && Selection.width > 0 && Selection.height > 0 )
        {
			cvCopyImage(ImageCopy, ImageCopy2);
            cvSetImageROI(ImageCopy2, Selection);
            
			cvXorS(ImageCopy2, cvScalarAll(255), ImageCopy2);
            
			cvResetImageROI(ImageCopy2);
			cvShowImage("Arena", ImageCopy2);
        }
	}
	cvReleaseImage(&ImageCopy);
	cvReleaseImage(&ImageCopy2);
	cvDestroyWindow("Arena");
}


//Mode = 0 Functions returns Left-Right Orientaion of Point 1 wrt Point 2
//Mode = 1 Functions returns Up-Down Orientaion of Point 1 wrt Point 2
inline int Pt1wrtPt2(CvPoint Pt1, CvPoint Pt2, int mode)
{
	switch(mode)
	{
	case 0:
		switch(Axis)
		{
		case X:
			if(Ramp.BoundingRect.x < Platform.BoundingRect.x)
			{
				if(Pt1.y <= Pt2.y)
					return Right;
				else
					return Left;
			}
			else
			{
				if(Pt1.y <= Pt2.y)
					return Left;
				else
					return Right;
			}
			break;
		case Y:
			if(Ramp.BoundingRect.y < Platform.BoundingRect.y)
			{
				if(Pt1.x > Pt2.x)
					return Right;
				else
					return Left;
			}
			else
			{
				if(Pt1.x > Pt2.x)
					return Left;
				else
					return Right;
			}
			break;
		}
		break;
	case 1:
		switch(Axis)
		{
		case X:
			if(Ramp.BoundingRect.x < Platform.BoundingRect.x)
			{
				if(Pt1.x <= Pt2.x)
					return Up;
				else
					return Down;
			}
			else
			{
				if(Pt1.x <= Pt2.x)
					return Down;
				else
					return Up;
			}
			break;
		case Y:
			if(Ramp.BoundingRect.y < Platform.BoundingRect.y)
			{
				if(Pt1.y <= Pt2.y)
					return Up;
				else
					return Down;
			}
			else
			{
				if(Pt1.y <= Pt2.y)
					return Down;
				else
					return Up;
			}
			break;
		}
		break;
	default:
		return -1;
	}
	return -1;
}


void PointRightTo(CvPoint* Point, CvPoint* Dst, int Distance)
{
	switch(Axis)
	{
	case X:
		Dst->x = Point->x;
		if(Ramp.BoundingRect.x < Platform.BoundingRect.x)
		{
			Dst->y = Point->y - Distance;
		}
		else
		{
			Dst->y = Point->y + Distance;
		}
		break;
	case Y:
		Dst->y = Point->y;
		if(Ramp.BoundingRect.y < Platform.BoundingRect.y)
		{
			Dst->x = Point->x + Distance;
		}
		else
		{
			Dst->x = Point->x - Distance;
		}
		break;
	}
}

void PointLeftTo(CvPoint* Point, CvPoint* Dst, int Distance)
{
	switch(Axis)
	{
	case X:
		Dst->x = Point->x;
		if(Ramp.BoundingRect.x < Platform.BoundingRect.x)
		{
			Dst->y = Point->y + Distance;
		}
		else
		{
			Dst->y = Point->y - Distance;
		}
		break;
	case Y:
		Dst->y = Point->y;
		if(Ramp.BoundingRect.y < Platform.BoundingRect.y)
		{
			Dst->x = Point->x - Distance;
		}
		else
		{
			Dst->x = Point->x + Distance;
		}
		break;
	}
}

int GetHorizontalDistance(CvPoint Point1, CvPoint Point2)
{
	if(Axis == X)
		return abs(Point1.y - Point2.y);
	else
		return abs(Point1.x - Point2.x);
}

int GetVerticalDistance(CvPoint Point1, CvPoint Point2)
{
	if(Axis == X)
		return abs(Point1.x - Point2.x);
	else
		return abs(Point1.y - Point2.y);
}

void FindRampnPlatform(IplImage* Frame, _ArenaObject* pRamp, _ArenaObject* pPlatform)
{
	IplImage* HSVFrame = cvCreateImage(cvGetSize(Frame), 8, 3);
	cvCvtColor(Frame, HSVFrame, CV_BGR2HSV);

	CvScalar Lower = cvScalar(40, 50, 50);
	CvScalar Upper = cvScalar(150, 256, 256);

	IplImage* Thresholded = cvCreateImage(cvGetSize(Frame), 8, 1);

	cvInRangeS(HSVFrame, Lower, Upper, Thresholded);

	cvErode(Thresholded, Thresholded, 0, 2);
	cvDilate(Thresholded, Thresholded, 0, 4);

	cvShowImage("RamnPlatform", Thresholded);
	cvWaitKey(0);

	CvMemStorage* Storage = cvCreateMemStorage(0);

	CvSeq* Contours;
	CvSeq* CurrentContour;
	cvFindContours(Thresholded, Storage, &Contours);

	CvSeq* RampContour = NULL;
	CvSeq* PlatformContour = NULL;

	CurrentContour = Contours;
	
	while(CurrentContour)
	{
		if(RampContour == NULL)
		{
			RampContour = CurrentContour;
		}
		else
		{
			if(cvContourArea(CurrentContour) > cvContourArea(RampContour))
			{
				RampContour = CurrentContour;
			}
		}
		CurrentContour = CurrentContour->h_next;
	}

	pRamp->BoundingRect = cvBoundingRect(RampContour, 1);
	pRamp->Center = cvPoint(pRamp->BoundingRect.x + pRamp->BoundingRect.width/2,
							pRamp->BoundingRect.y + pRamp->BoundingRect.height/2);

	CurrentContour = Contours;
	
	while(CurrentContour)
	{
		if(cvContourArea(RampContour) - cvContourArea(CurrentContour) > 10)
		{
			if(PlatformContour == NULL)
			{
				PlatformContour = CurrentContour;
			}
			else
			{
				if(cvContourArea(CurrentContour) > cvContourArea(PlatformContour))
				{
					PlatformContour = CurrentContour;
				}
			}
		}
		CurrentContour = CurrentContour->h_next;
	}

	pPlatform->BoundingRect = cvBoundingRect(PlatformContour, 1);
	pPlatform->Center = cvPoint(pPlatform->BoundingRect.x + pPlatform->BoundingRect.width/2,
							pPlatform->BoundingRect.y + pPlatform->BoundingRect.height/2);

	cvReleaseImage(&Thresholded);
	cvReleaseImage(&HSVFrame);
	cvReleaseMemStorage(&Storage);
}

	



