#include <cv.h>
#include <cxcore.h>
#include <highgui.h>
#include <windows.h>
#include <iostream>
using namespace std;

#include "GlobalVariables.h"
#include "Messages.h"
#include "ArenaObjects.h"
#include "Ball.h"
#include "Robot.h"
#include "Threshold.h"
#include "Task.h"

int _Ball::TotalBalls = 0;

int main(int argc, char* argv[])
{
	_Threshold BallThreshold[3];
	CvSeq* BallList[3];
	_Task CurrentTask;
	ClearTask(&CurrentTask);
	CvMemStorage *BallSeqStorage = cvCreateMemStorage(0);
		
	IplImage *Frame;
	IplImage *FrameCopy;
	
	CvFont Font;
	CvCapture *Capture = NULL;
	
	char c = 0;
	int i = 0, j = 0;
	_Ball* TempBall = NULL;

	
	char FrameRatestr[10];
	char BallID[50];
	
	DWORD StartTime;
	
	CvVideoWriter* Writer;
	
	cvInitFont(&Font, CV_FONT_HERSHEY_PLAIN, 1.0f, 1.0f);

	if(argc == 2)
	{
		Capture = cvCaptureFromCAM(-1);
		i = InitializeMessages(argv[1], 1200);
	}
		
	else if(argc == 3)
	{
		Capture = cvCaptureFromAVI(argv[1]);
		VideoFPS = cvRound(cvGetCaptureProperty(Capture, CV_CAP_PROP_FPS));
		i = InitializeMessages(argv[2], 1200);
		i = 1;
	}
	else
		return -1;
	if(Capture == NULL || !i)
	{
		if(Capture == NULL)
			cout<<"Camera error\n";
		else if(i == 0)
			cout<<"COM port error\n";

		cvReleaseCapture(&Capture);
		return -1;
	}

	Frame = cvQueryFrame(Capture);
	if(Frame == NULL)
	{	
		cout<<"Frame error\n";
		return -1;
	}

	for(i = 0; i < 3; i++)
	{
		BallList[i] = cvCreateSeq(0, sizeof(CvSeq), sizeof(_Ball), BallSeqStorage);
	}

	Writer = cvCreateAVIWriter("Output.avi", 0, 15, cvGetSize(Frame));
	FrameCopy = cvCreateImage(cvGetSize(Frame), Frame->depth, Frame->nChannels);
	Background = cvCreateImage(cvGetSize(Frame), Frame->depth, Frame->nChannels);

	cvNamedWindow("Capture");
	while(Frame && c != 27)
	{
		cvCopyImage(Frame, FrameCopy);
		cvPutText(FrameCopy, "Press ESC when image quality gets stable", cvPoint(0, 20), &Font, cvScalar(255, 255, 255));
		cvShowImage("Capture", FrameCopy);
		c = cvWaitKey(34);
		Frame = cvQueryFrame(Capture);
	}

	cvCopyImage(Frame, FrameCopy);
	cvCopyImage(Frame, Background);
	
	_ArenaObject Tempramp;
	_ArenaObject Tempplatform;

	FindArenaObjects(FrameCopy, Font, &Ramp, &Platform, &RightPit, &LeftPit, &Robot);
	//FindRampnPlatform(FrameCopy, &Ramp, &Platform);

	if(Ramp.BoundingRect.height > Ramp.BoundingRect.width)
	{
		Axis = Y;

		cout<<"Axis = Y\n\n";
		Robot.VerticalSpan = Robot.BoundingRect.height;
		Robot.HorizontalSpan = Robot.BoundingRect.width;
		Ramp.VerticalSpan = Ramp.BoundingRect.height;
		Ramp.HorizontalSpan = Ramp.BoundingRect.width;
		Platform.VerticalSpan = Platform.BoundingRect.height;
		Platform.HorizontalSpan = Platform.BoundingRect.width;
		RightPit.VerticalSpan = RightPit.BoundingRect.height;
		RightPit.HorizontalSpan = RightPit.BoundingRect.width;
		LeftPit.VerticalSpan = LeftPit.BoundingRect.height;
		LeftPit.HorizontalSpan = LeftPit.BoundingRect.width;
	}
	else
	{
		Axis = X;
		cout<<"Axis = X\n\n";

		Robot.VerticalSpan = Robot.BoundingRect.width;
		Robot.HorizontalSpan = Robot.BoundingRect.height;
		Ramp.VerticalSpan = Ramp.BoundingRect.width;
		Ramp.HorizontalSpan = Ramp.BoundingRect.height;
		Platform.VerticalSpan = Platform.BoundingRect.width;
		Platform.HorizontalSpan = Platform.BoundingRect.height;
		RightPit.VerticalSpan = RightPit.BoundingRect.width;
		RightPit.HorizontalSpan = RightPit.BoundingRect.height;
		LeftPit.VerticalSpan = LeftPit.BoundingRect.width;
		LeftPit.HorizontalSpan = LeftPit.BoundingRect.height;
	}

	cout<<"Robot.VerticalSpan:	"<<Robot.VerticalSpan<<"\n";
	cout<<"Robot.HorizontalSpan:	"<<Robot.HorizontalSpan<<"\n";
	cout<<"Ramp.VerticalSpan:	"<<Ramp.VerticalSpan<<"\n";
	cout<<"Ramp.HorizontalSpan:	"<<Ramp.HorizontalSpan<<"\n";
	cout<<"Platform.VerticalSpan:	"<<Platform.VerticalSpan<<"\n";
	cout<<"Platform.HorizontalSpan:"<<Platform.HorizontalSpan<<"\n";
	cout<<"RightPit.VerticalSpan:	"<<RightPit.VerticalSpan<<"\n";
	cout<<"RightPit.HorizontalSpan:"<<RightPit.HorizontalSpan<<"\n";
	cout<<"LeftPit.VerticalSpan:	"<<LeftPit.VerticalSpan<<"\n";
	cout<<"LeftPit.HorizontalSpan:	"<<LeftPit.HorizontalSpan<<"\n\n";
	
	cvSetImageROI(FrameCopy, Robot.Patch);
	CvScalar Temp = cvAvg(FrameCopy);
	cvResetImageROI(FrameCopy);
	
	Robot.PatchThreshold.Max = cvScalar(Temp.val[0] + 25, Temp.val[1] + 25, Temp.val[2] + 25, 0);
	Robot.PatchThreshold.Min = cvScalar(Temp.val[0] - 25, Temp.val[1] - 25, Temp.val[2] - 25, 0);	
	ReadThresholds(NULL, BallThreshold, NULL);
	
	Robot.InTransit = 0;
	//SendMessageToMCU(TiltLeft);
	
	c = 0;
	StartTime = GetTickCount();

	int TimeOver = 0;
	
	while(Frame && c != 27)
	{
#pragma region Image Copying and Updating Global Time
		cvCopyImage(Frame, FrameCopy);
		FrameCount++;
		if(argc == 3)
			GlobalTime =  FrameCount*1000/VideoFPS;
		else
			GlobalTime = GetTickCount() - StartTime;
		FPS = (FrameCount*1000)/(GetTickCount() - StartTime + 1);
#pragma endregion

		if(TimeOver == 0 && GlobalTime > 240000)
		{
			TimeOver  = 1;
			Robot.NumberOfBlueBalls = 4;
			Robot.NumberOfRedBalls = 4;
		}

#pragma region Updating Balls and Robot and Task
		UpdateBotPostion(FrameCopy, &Robot, Platform.BoundingRect);
		UpdateBallList(FrameCopy, BallList, BallThreshold[Red], Ramp.BoundingRect, Red);
		//UpdateBallList(FrameCopy, BallList[Blue], BallThreshold[Blue], Ramp.BoundingRect, Blue);
		//UpdateBallList(FrameCopy, BallList[Black], BallThreshold[Black], Ramp.BoundingRect, Black);
		UpdateTask(BallList, &CurrentTask);
		SetTask(BallList, &CurrentTask);
#pragma endregion

#pragma region Drawing Functions
		_itoa(FPS, FrameRatestr, 10);
		strcat(FrameRatestr, " fps");
		cvPutText(FrameCopy, FrameRatestr, cvPoint(10, 20), &Font, cvScalarAll(255));
		for(i = 0; i < 3; i++)
		{
			for(j = 0; j <BallList[i]->total; j++)
			{
				TempBall = (_Ball*) cvGetSeqElem(BallList[i], j);
				cvCircle(FrameCopy, TempBall->Center, TempBall->Radius*2, cvScalarAll(255), 1);
				cvCircle(FrameCopy, TempBall->Center, 1, cvScalar(0, 255, 255), 2);
				_itoa(TempBall->BallID, BallID, 10);
				switch(TempBall->Colour)
				{
				case Red:
					strcat(BallID, " Red");
					break;
				case Blue:
					strcat(BallID, " Blue");
					break;
				case Black:
					strcat(BallID, " Black");
					break;
				}
				if(TempBall->ExpectedPosition.x == -1 || TempBall->ExpectedPosition.y == -1)
				{
					//strcat(BallID, " ,will hit wall");
				}
				else
				{
					//strcat(BallID, " ,will not hit wall");
					cvLine(FrameCopy, TempBall->Center, TempBall->ExpectedPosition, cvScalarAll(255), 2);
				}
				cvPutText(FrameCopy, BallID, cvPoint(TempBall->Center.x + 2*(TempBall->Radius), TempBall->Center.y), &Font, cvScalarAll(255));
			}
		}
		cvCircle(FrameCopy, Robot.PatchCenter, 1, cvScalar(0, 0, 255), 2);
		cvRectangleR(FrameCopy, Robot.BoundingRect, cvScalarAll(255));
		cvRectangleR(FrameCopy, Ramp.BoundingRect, cvScalarAll(255));
		cvRectangleR(FrameCopy, Platform.BoundingRect, cvScalarAll(255));
		cvRectangleR(FrameCopy, RightPit.BoundingRect, cvScalarAll(255));
		cvRectangleR(FrameCopy, LeftPit.BoundingRect, cvScalarAll(255));
		
#pragma endregion

#pragma region Frame Display, Save and Capture New Frame
		cvShowImage("Capture", FrameCopy);
		cvWriteFrame(Writer, FrameCopy);
		c = cvWaitKey(2);
		Frame = cvQueryFrame(Capture);
#pragma endregion

	}
	
#pragma region Memory release
	cvReleaseVideoWriter(&Writer);
	cvReleaseMemStorage(&BallSeqStorage);
	cvReleaseImage(&FrameCopy);
	cvReleaseImage(&Background);
	cvDestroyAllWindows();
	cvReleaseCapture(&Capture);
	DeInitializeMessages();
#pragma endregion

	return 0;
}