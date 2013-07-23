#include <cv.h>
#include <cxcore.h>
#include <highgui.h>
#include <stdlib.h>
#include <iostream>
using namespace std;

#include "GlobalVariables.h"
#include "Ball.h"
#include "Threshold.h"
#include "ArenaObjects.h"

inline int UpdateBallList(IplImage* Image, CvSeq* BallList[3], _Threshold BallThreshold, CvRect ROI, int Colour)
{
#pragma region Variables
	static CvMemStorage* Storage = cvCreateMemStorage(0);
	static CvSize Size = cvGetSize(Image);
	static IplImage* Thresholded = cvCreateImage(Size, IPL_DEPTH_8U, 1);
	static IplImage* ImageCopy = cvCreateImage(Size, IPL_DEPTH_8U, 3);
	
	CvSeq* Contours;
	CvRect ContourBoundingRect;
	CvPoint ContourCenter;
	int ContourRadius;
	CvScalar ContourAvg;
	CvScalar Mean, Std;
	int ThresholdValue;

	_Ball* TempBall;
		
	int i,j;
	int NewBalls = 0;	
	int ContourUsed = 0;
#pragma endregion
	
	for(i = 0; i < 3; i++)
	{
		for(j = 0; j < BallList[i]->total; j++)
		{
			TempBall = (_Ball*)cvGetSeqElem(BallList[i], j);
			TempBall->Updated = 0;
			if(ClosePoints(TempBall->BirthCenter, TempBall->Center, TempBall->Radius))
			{
				TempBall->StagnationDuration = GlobalTime - TempBall->BirthTime;
				if(TempBall->StagnationDuration > BallMaxTimeOnRamp/6)
				{
					cvSeqRemove(BallList[i], j);
					j--;
				}
			}
		}
	}

	cvAbsDiff(Background, Image, ImageCopy);
	cvCvtColor(ImageCopy, Thresholded, CV_BGR2GRAY);
	
	cvAvgSdv(Thresholded, &Mean, &Std);
	ThresholdValue = cvRound(Mean.val[0] + 2*Std.val[0]);
	cvThreshold(Thresholded, Thresholded, ThresholdValue, 255, CV_THRESH_BINARY);
	cvErode(Thresholded, Thresholded);
			
	cvSetImageROI(Thresholded, ROI);
	cvDilate(Thresholded, Thresholded);
	cvFindContours(Thresholded, Storage, &Contours, sizeof(CvContour), CV_RETR_CCOMP, 2, cvPoint(ROI.x, ROI.y));
	cvResetImageROI(Thresholded);
	
	/*cvDrawContours(ImageCopy, Contours, cvScalar(0, 0, 255), cvScalar(255, 0, 0), 5, 2);
	cvShowImage("Contours", ImageCopy);*/

	while(Contours)
	{					
		ContourBoundingRect = cvBoundingRect(Contours, 1);
		ContourCenter = cvPoint(ContourBoundingRect.x + (ContourBoundingRect.width/2),
								ContourBoundingRect.y + (ContourBoundingRect.height/2));
		ContourRadius = (ContourBoundingRect.width + ContourBoundingRect.height)/4;
		
		ContourAvg = cvGet2D(Image, ContourCenter.y, ContourCenter.x);
		ContourUsed = 0;

		for(i = 0; i < 3 && ContourUsed == 0; i++)
		{
			//switch(i)
			//{
			//case Red:
			//	cout<<"Red \n";
			//	break;
			//case Blue:
			//	cout<<"Blue \n";
			//	break;
			//case Black:
			//	cout<<"Black \n";
			//	break;
			//}		
			for(j = 0; j < BallList[i]->total; j++)
			{		
				TempBall = (_Ball*)cvGetSeqElem(BallList[i], j);
				if(ClosePoints(ContourCenter, TempBall->Center, (TempBall->Radius)*4) && ContourRadius <= 4*(TempBall->Radius))
				{
					TempBall->PrevPosition = TempBall->Center;
					TempBall->Center = ContourCenter;
					TempBall->Radius = (TempBall->Radius + ContourRadius)/2;
					TempBall->Updated = 1;
					SetExpectedPoint(TempBall);
					ContourUsed = 1;
					break;
				}
			}
			if(PossibleColour(ContourAvg) == i)
			{
				if(j == BallList[i]->total)
				{
					//cout<<"New Ball Possibility\n";
					ContourUsed = 1;
					if(isNewBall(ContourBoundingRect, ContourCenter, ContourRadius))
					{
						//cout<<"Yes\n";
						_Ball::TotalBalls++;
						TempBall = (_Ball*)malloc(sizeof(_Ball));
						TempBall->Center = ContourCenter;
						TempBall->BirthCenter = ContourCenter;
						TempBall->PrevPosition =ContourCenter;
						TempBall->ExpectedPosition = cvPoint(-1, -1);
						TempBall->Colour = i;
						TempBall->Radius = ContourRadius;
						TempBall->Updated = 1;
						TempBall->BirthTime = GlobalTime;
						TempBall->BallID = _Ball::TotalBalls;
						cvSeqPush(BallList[i], TempBall);
						NewBalls++;
					}
				}
			}
		}
		//cvWaitKey();
		//cout<<"\n\n";
		Contours = Contours->h_next;
	}
	for(i = 0; i < 3; i++)
	{
		for(j = 0; j < BallList[i]->total; j++)
		{
			TempBall = (_Ball*)cvGetSeqElem(BallList[i], j);
			if(TempBall->Updated == 0)
			{
				if(isDeadBall(TempBall))
				{
					cvSeqRemove(BallList[i], j);
					j--;
				}
			}
			else
			{
				if(GlobalTime - TempBall->BirthTime >= 2*BallMaxTimeOnRamp)
				{
					cvSeqRemove(BallList[i], j);
					j--;
				}
			}
		}
	}

	cvClearMemStorage(Storage);
	return NewBalls;
}

inline double ProportionalDistance(_Ball *Ball1, _Ball *Ball2)
{
	double Delx = Ball2->Center.x - Ball1->Center.x;
	double Dely = Ball2->Center.y - Ball1->Center.y;
	double Distance = Delx*Delx + Dely*Dely;
	return Distance;
}

inline int CloseBalls(_Ball *Ball1, _Ball *Ball2, int Distance = 0)
{
	if(!Distance)
		Distance = (Ball1->Radius + Ball2->Radius);
	int Delx = abs(Ball1->Center.x - Ball2->Center.x);
	int Dely = abs(Ball1->Center.y - Ball2->Center.y);
	if(Delx <= Distance && Dely <= Distance)
		return 1;
	return 0;
}

inline int ClosePoints(CvPoint Pt1, CvPoint Pt2, int Distance = 50)
{
	int Delx = abs(Pt1.x - Pt2.x);
	int Dely = abs(Pt1.y - Pt2.y);
	if(Delx <= Distance && Dely <= Distance)
		return 1;
	return 0;
}

inline int isNewBall(CvRect ContourBoundingRect, CvPoint ContourCenter, int ContourRadius)
{
	int BirthDistance;
	static int MaxBirthDistance = Ramp.VerticalSpan/4;
	if(Axis == X)
	{
		BirthDistance = abs(Ramp.BoundingRect.x - ContourCenter.x);
	}
	else
	{
		BirthDistance = abs(Ramp.BoundingRect.y - ContourCenter.y);
	}

	if(	ContourRadius >= MinBallRadius && ContourRadius <= MaxBallRadius 
		&& abs(ContourBoundingRect.width - ContourBoundingRect.height) <= MIN(ContourBoundingRect.width, ContourBoundingRect.height)
		&& Pt1wrtPt2(ContourCenter, Ramp.Center, 1) == Up
		/*&& BirthDistance <= MaxBirthDistance*/)
	{
		return 1;
	}
	else
		return 0;
}

inline int isDeadBall(_Ball *Ball)
{
	if(GlobalTime - Ball->BirthTime >= BallMaxTimeOnRamp)
		return 1;
	static int Max = cvRound(3*Ramp.VerticalSpan/4);
	if(Axis == X)
	{
		if(abs(Ball->BirthCenter.x - Ball->Center.x) > Max)
			return 1;
	}
	else
	{
		if(abs(Ball->BirthCenter.y - Ball->Center.y) > Max)
			return 1;
	}
	return 0;
}

inline int SetExpectedPoint(_Ball *Ball)
{
	double Dely;
	double Delx;
	double DelA;
	Dely = Ball->PrevPosition.y - Ball->Center.y;
	Delx = Ball->PrevPosition.x - Ball->Center.x;

	if(Dely == 0 || Delx == 0)
	{
		Ball->ExpectedPosition.x = -1;
		Ball->ExpectedPosition.y = -1;
		return -1;
	}

	if(Axis == X)
	{
		Ball->ExpectedPosition.x = Robot.PatchCenter.x;
		DelA = Ball->ExpectedPosition.x - Ball->PrevPosition.x;
		Ball->ExpectedPosition.y = cvRound(DelA*Dely/Delx + Ball->PrevPosition.y);
		
		//Check if it will hit the wall
		if(Ball->ExpectedPosition.y < Ramp.BoundingRect.y || Ball->ExpectedPosition.y > Ramp.BoundingRect.y + Ramp.HorizontalSpan)
		{
			Ball->ExpectedPosition.x = -1;
		}
	}
	else
	{
		Ball->ExpectedPosition.y = Robot.PatchCenter.y;
		DelA = Ball->ExpectedPosition.y - Ball->PrevPosition.y;
		Ball->ExpectedPosition.x = cvRound(DelA*Delx/Dely + Ball->PrevPosition.x);
		
		//Check if it will hit the wall
		if(Ball->ExpectedPosition.x < Ramp.BoundingRect.x || Ball->ExpectedPosition.x > Ramp.BoundingRect.x + Ramp.HorizontalSpan)
		{
			Ball->ExpectedPosition.y = -1;
		}
	}
	return 0;
}

inline CvScalar BGR2HSV(CvScalar BGR)
{
	int Max, Min;
	double fMax, fMin;
	int B = cvRound(BGR.val[0]);
	int G = cvRound(BGR.val[1]);
	int R = cvRound(BGR.val[2]);

	CvScalar HSV;

	Max = MAX(B, G);
	Max = MAX(Max, R);
	
	Min = MIN(B, G);
	Min = MIN(Min, R);

	fMax = ((double)Max)/255;
	fMin = ((double)Min)/255;
	
	BGR.val[0] = BGR.val[0]/255;
	BGR.val[1] = BGR.val[1]/255;
	BGR.val[2] = BGR.val[2]/255;

	int Case;
	if(Max == B)
		Case = 0;
	else if(Max == G)
		Case = 1;
	else
		Case = 2;

	switch(Case)
	{
	case 0:
		HSV.val[2] = BGR.val[0];
		if(HSV.val[2])
		{
			HSV.val[1] = (HSV.val[2] - fMin)/HSV.val[2];
			HSV.val[0] = 240 + 60*(BGR.val[2] - BGR.val[1])/HSV.val[1];
		}
		else
		{
			HSV.val[0] = 0;
			HSV.val[1] = 0;
		}
		break;
	case 1:
		HSV.val[2] = BGR.val[1];
		if(HSV.val[2])
		{
			HSV.val[1] = (HSV.val[2] - fMin)/HSV.val[2];
			HSV.val[0] = 120 + 60*(BGR.val[0] - BGR.val[2])/HSV.val[1];
		}
		else
		{
			HSV.val[0] = 0;
			HSV.val[1] = 0;
		}
		break;
	case 2:
		HSV.val[2] = BGR.val[2];
		if(HSV.val[2])
		{
			HSV.val[1] = (HSV.val[2] - fMin)/HSV.val[2];
			HSV.val[0] = 60*(BGR.val[1] - BGR.val[0])/HSV.val[1];
		}
		else
		{
			HSV.val[0] = 0;
			HSV.val[1] = 0;
		}
		break;
	}
	if(HSV.val[0] < 0)
		HSV.val[0] += 360;
	HSV.val[1] = HSV.val[1]*255;
	HSV.val[2] = HSV.val[2]*255;
	return HSV;
}

inline int PossibleColour(CvScalar BGR)
{
	CvScalar HSV = BGR2HSV(BGR);

	//cout<<"HSV: "<<HSV.val[0]<<"\t"<<HSV.val[1]<<"\t"<<HSV.val[2]<<"\n";
	if(HSV.val[2] < ValueBlackMax)
	{
		//cout<<"Black Colour Confirmed\n";
		return Black;
	}
	else if(HSV.val[0] >= HueRmax2 || HSV.val[0] <= HueRmax1)
	{
		//cout<<"Red Colour Confirmed\n";
		return Red;
	}
	else if(HSV.val[0] <= HueBmax && HSV.val[0] >= HueBmin)
	{
		//cout<<"Blue Colour Confirmed\n";
		return Blue;
	}
	return -1;
}
