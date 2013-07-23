#include <cxcore.h>
#include <iostream>
using namespace std;

#include "GlobalVariables.h"
#include "Messages.h"
#include "ArenaObjects.h"
#include "Ball.h"
#include "Robot.h"
#include "Threshold.h"
#include "Task.h"

void ClearTask(_Task* Task)
{
	Task->BallColour = -1;
	Task->BallID = -1;
	Task->CompletionCoord = cvPoint(-1, -1);
	Task->Task = -1;
	Task->TaskProgress = 0;
}

void UpdateTask(CvSeq* BallLists[3], _Task* CurrentTask)
{
	int i;
	_Ball* TempBall;

	if(Robot.InTransit)
	{
		switch(CurrentTask->Task)
		{
#pragma region Task Move Right to Catch Ball
		case TaskMRCB:
			for(i = 0; i < BallLists[CurrentTask->BallColour]->total; i++)
			{
				TempBall = (_Ball*)cvGetSeqElem(BallLists[CurrentTask->BallColour], i);
				if(TempBall->BallID == CurrentTask->BallID)
				{
					if(TempBall->ExpectedPosition.x < 0 || TempBall->ExpectedPosition.y < 0)
					{
						PointRightTo(&Ramp.Center, &CurrentTask->CompletionCoord, Ramp.HorizontalSpan/2);
						if(Axis == X)
							CurrentTask->CompletionCoord.x = Robot.PatchCenter.x;
						else
							CurrentTask->CompletionCoord.y = Robot.PatchCenter.y;

						if(CurrentTask->BallColour == Blue)
							PointRightTo(&(CurrentTask->CompletionCoord), &(CurrentTask->CompletionCoord), Robot.HorizontalSpan/4);
						else if(CurrentTask->BallColour == Red)
							PointLeftTo(&(CurrentTask->CompletionCoord), &(CurrentTask->CompletionCoord), Robot.HorizontalSpan/4);

						CurrentTask->TaskProgress = 0;
						cout<<"Global Time: "<<GlobalTime<<"\n";
						cout<<"TaskMRCB\nBall ID: "<<CurrentTask->BallID<<"\nBall to hit wall\n";
						cout<<"**********************************************************\n";
						break;
					}
					if(Pt1wrtPt2(TempBall->ExpectedPosition, Robot.PatchCenter, 0) == Left 
						&& !ClosePoints(TempBall->ExpectedPosition, Robot.PatchCenter, Robot.HorizontalSpan))
					{
						cout<<"Global Time: "<<GlobalTime<<"\n";
						cout<<"TaskMRCB\nBall ID: "<<CurrentTask->BallID<<"\nBall went to left\n";
						cout<<"**********************************************************\n";

						SendMessageToMCU(MoveLeft);
						CurrentTask->Task = TaskMLCB;
						CurrentTask->TaskProgress = 0;

						if(CurrentTask->BallColour == Blue)
							PointRightTo(&(TempBall->ExpectedPosition), &(CurrentTask->CompletionCoord), Robot.HorizontalSpan/4);
						else if(CurrentTask->BallColour == Red)
							PointLeftTo(&(TempBall->ExpectedPosition), &(CurrentTask->CompletionCoord), Robot.HorizontalSpan/4);
						return;
					}
					if(CurrentTask->BallColour == Blue)
						PointRightTo(&(TempBall->ExpectedPosition), &(CurrentTask->CompletionCoord), Robot.HorizontalSpan/4);
					else if(CurrentTask->BallColour == Red)
						PointLeftTo(&(TempBall->ExpectedPosition), &(CurrentTask->CompletionCoord), Robot.HorizontalSpan/4);
					break;
				}
			}			
			switch(CurrentTask->TaskProgress)
			{
			case 0:
				if(i == BallLists[CurrentTask->BallColour]->total)
				{
					cout<<"Global Time: "<<GlobalTime<<"\n";
					cout<<"TaskMRCB\nBall ID: "<<CurrentTask->BallID<<"\nBall Lost\nTerminating Task\n";
					cout<<"**********************************************************\n";

					SendMessageToMCU(StopMoving);
					ClearTask(CurrentTask);
					Robot.InTransit = 0;
				}
				else if(ClosePoints(CurrentTask->CompletionCoord, Robot.PatchCenter, Robot.HorizontalSpan/2))
				{
					if(CurrentTask->BallColour == Blue && Pt1wrtPt2(Robot.PatchCenter, CurrentTask->CompletionCoord, 0) == Right)
					{
						cout<<"Global Time: "<<GlobalTime<<"\n";
						cout<<"TaskMRCB\nBall ID: "<<CurrentTask->BallID<<"\nReached expected postion(Blue Ball)\n";
						cout<<"**********************************************************\n";

						SendMessageToMCU(StopMoving);
						CurrentTask->TaskProgress = 1;
					}
					else if(CurrentTask->BallColour == Red && Pt1wrtPt2(Robot.PatchCenter, CurrentTask->CompletionCoord, 0) == Left)
					{
						
						cout<<"Global Time: "<<GlobalTime<<"\n";
						cout<<"TaskMRCB\nBall ID: "<<CurrentTask->BallID<<"\nReached Expected Postion(Red ball)\n";
						cout<<"**********************************************************\n";

						SendMessageToMCU(StopMoving);
						CurrentTask->TaskProgress = 1;
					}
				}
				else
					SendMessageToMCU(MoveRight);
				break;
			case 2://Wait for atleast 2 frames to confirm that ball is no longer on the ramp
				cout<<"Global Time: "<<GlobalTime<<"\n";
				cout<<"TaskMRCB\nBall ID: "<<CurrentTask->BallID<<"\nTask Over\n";
				cout<<"**********************************************************\n";

				if(CurrentTask->BallColour == Red)
					Robot.NumberOfRedBalls++;
				else if(CurrentTask->BallColour == Blue)
					Robot.NumberOfBlueBalls++;
				ClearTask(CurrentTask);
				Robot.InTransit = 0;
				break;
			default:
				if(i == BallLists[CurrentTask->BallColour]->total)
				{
					cout<<"Global Time: "<<GlobalTime<<"\n";
					cout<<"TaskMRCB\nBall ID: "<<CurrentTask->BallID<<"\nTaskProgress = 1\n";
					cout<<"**********************************************************\n";
					//ball is not on ramp in this frame
					CurrentTask->TaskProgress++;
				}
				else
				{
					CurrentTask->TaskProgress = 0;
				}
				break;
			}	
			break;
#pragma endregion

#pragma region Task Move Left to Catch Ball
		case TaskMLCB:
			for(i = 0; i < BallLists[CurrentTask->BallColour]->total; i++)
			{
				TempBall = (_Ball*)cvGetSeqElem(BallLists[CurrentTask->BallColour], i);
				if(TempBall->BallID == CurrentTask->BallID)
				{
					if(TempBall->ExpectedPosition.x < 0 || TempBall->ExpectedPosition.y < 0)
					{
						PointLeftTo(&Ramp.Center, &CurrentTask->CompletionCoord, Ramp.HorizontalSpan/2);
						if(Axis == X)
							CurrentTask->CompletionCoord.x = Robot.PatchCenter.x;
						else
							CurrentTask->CompletionCoord.y = Robot.PatchCenter.y;

						if(CurrentTask->BallColour == Blue)
							PointRightTo(&(CurrentTask->CompletionCoord), &(CurrentTask->CompletionCoord), Robot.HorizontalSpan/4);
						else if(CurrentTask->BallColour == Red)
							PointLeftTo(&(CurrentTask->CompletionCoord), &(CurrentTask->CompletionCoord), Robot.HorizontalSpan/4);

						CurrentTask->TaskProgress = 0;
						cout<<"Global Time: "<<GlobalTime<<"\n";
						cout<<"TaskMLCB\nBall ID: "<<CurrentTask->BallID<<"\nBall to hit wall\n";
						cout<<"**********************************************************\n";

						CurrentTask->TaskProgress = 0;
						break;
					}
					if(Pt1wrtPt2(TempBall->ExpectedPosition, Robot.PatchCenter, 0) == Right 
						&& !ClosePoints(TempBall->ExpectedPosition, Robot.PatchCenter, Robot.HorizontalSpan))
					{
						cout<<"Global Time: "<<GlobalTime<<"\n";
						cout<<"TaskMlCB\nBall ID: "<<CurrentTask->BallID<<"\nBall went to right\n";
						cout<<"**********************************************************\n";

						SendMessageToMCU(MoveRight);
						CurrentTask->Task = TaskMRCB;
						CurrentTask->TaskProgress = 0;

						if(CurrentTask->BallColour == Blue)
							PointRightTo(&(TempBall->ExpectedPosition), &(CurrentTask->CompletionCoord), Robot.HorizontalSpan/4);
						else if(CurrentTask->BallColour == Red)
							PointLeftTo(&(TempBall->ExpectedPosition), &(CurrentTask->CompletionCoord), Robot.HorizontalSpan/4);
						return;
					}
					if(CurrentTask->BallColour == Blue)
						PointRightTo(&(TempBall->ExpectedPosition), &(CurrentTask->CompletionCoord), Robot.HorizontalSpan/4);
					else if(CurrentTask->BallColour == Red)
						PointLeftTo(&(TempBall->ExpectedPosition), &(CurrentTask->CompletionCoord), Robot.HorizontalSpan/4);
					break;
				}
			}			
			switch(CurrentTask->TaskProgress)
			{
			case 0:
				if(i == BallLists[CurrentTask->BallColour]->total)
				{
					cout<<"Global Time: "<<GlobalTime<<"\n";
					cout<<"TaskMLCB\nBall ID: "<<CurrentTask->BallID<<"\nBall Lost\nTerminating Task\n";
					cout<<"**********************************************************\n";

					SendMessageToMCU(StopMoving);
					ClearTask(CurrentTask);
					Robot.InTransit = 0;
				}
				else if(ClosePoints(CurrentTask->CompletionCoord, Robot.PatchCenter, Robot.HorizontalSpan/2))
				{
					if(CurrentTask->BallColour == Blue && Pt1wrtPt2(Robot.PatchCenter, CurrentTask->CompletionCoord, 0) == Right)
					{
						cout<<"Global Time: "<<GlobalTime<<"\n";
						cout<<"TaskMLCB\nBall ID: "<<CurrentTask->BallID<<"\nReached expected postion(Blue Ball)\n";
						cout<<"**********************************************************\n";

						SendMessageToMCU(StopMoving);
						CurrentTask->TaskProgress = 1;
					}
					else if(CurrentTask->BallColour == Red && Pt1wrtPt2(Robot.PatchCenter, CurrentTask->CompletionCoord, 0) == Left)
					{
						cout<<"Global Time: "<<GlobalTime<<"\n";
						cout<<"TaskMLCB\nBall ID: "<<CurrentTask->BallID<<"\nReached expected postion(Red Ball)\n";
						cout<<"**********************************************************\n";

						SendMessageToMCU(StopMoving);
						CurrentTask->TaskProgress = 1;
					}
				}
				else
					SendMessageToMCU(MoveLeft);
				break;
			case 2://Wait for atleast 2 frames to confirm that ball is no longer on the ramp
				cout<<"Global Time: "<<GlobalTime<<"\n";
				cout<<"TaskMLCB\nBall ID: "<<CurrentTask->BallID<<"\nTask Complete\n";
				cout<<"**********************************************************\n";

				if(CurrentTask->BallColour == Red)
					Robot.NumberOfRedBalls++;
				else if(CurrentTask->BallColour == Blue)
					Robot.NumberOfBlueBalls++;
				ClearTask(CurrentTask);
				Robot.InTransit = 0;
				break;
			default:
				if(i == BallLists[CurrentTask->BallColour]->total)
				{
					cout<<"Global Time: "<<GlobalTime<<"\n";
					cout<<"TaskMLCB\nBall ID: "<<CurrentTask->BallID<<"\nTask Progress = 1\n";
					cout<<"**********************************************************\n";

					//ball is not on ramp in this frame
					CurrentTask->TaskProgress++;
				}
				else
				{
					CurrentTask->TaskProgress = 0;
				}
				break;
			}	
			break;
#pragma endregion

#pragma region Task Move to Center
		case TaskMC:
			if(ClosePoints(Robot.PatchCenter, Platform.Center, Robot.HorizontalSpan/4))
			{
				cout<<"Global Time: "<<GlobalTime<<"\n";
				cout<<"TaskMC\nReached Center\n";
				cout<<"**********************************************************\n";

				SendMessageToMCU(StopMoving);
				ClearTask(CurrentTask);
				Robot.InTransit = 0;
			}
			break;
#pragma endregion 

#pragma region Task Move Right
		case TaskMR:
			if(Pt1wrtPt2(Robot.PatchCenter, CurrentTask->CompletionCoord, 0) == Right)
			{
				cout<<"Global Time: "<<GlobalTime<<"\n";
				cout<<"TaskMR\nReached Right Coord\n";
				cout<<"**********************************************************\n";

				SendMessageToMCU(StopMoving);
				ClearTask(CurrentTask);
				Robot.InTransit = 0;
			}
			break;
#pragma endregion

#pragma region Task Move Left
		case TaskML:
			if(Pt1wrtPt2(Robot.PatchCenter, CurrentTask->CompletionCoord, 0) == Left)
			{
				cout<<"Global Time: "<<GlobalTime<<"\n";
				cout<<"TaskML\nReached Left Coord\n";
				cout<<"**********************************************************\n";

				SendMessageToMCU(StopMoving);
				ClearTask(CurrentTask);
				Robot.InTransit = 0;
			}
			break;
#pragma endregion

#pragma region Task Move to Left Pit and Tilt Pan
		case TaskMLTP:
			switch(CurrentTask->TaskProgress)
			{
			case 0:
				if(ClosePoints(CurrentTask->CompletionCoord, Robot.PatchCenter, Robot.HorizontalSpan/2))
				{
					SendMessageToMCU(StopMoving);
					SendMessageToMCU(TiltLeft);
					CurrentTask->TaskProgress++;
				}
				break;
			case 90://Wait for 15 frames for Robot to Tilt pan and be stable
				cout<<"Setting to Neutral\n";
				ClearTask(CurrentTask);
				Robot.NumberOfBlueBalls = 0;
				Robot.InTransit = 0;
				break;
			case 30:
				SendMessageToMCU(Neutral);
				//SendMessageToMCU(Neutral);
				CurrentTask->TaskProgress++;
				break;
			case 60:
				SendMessageToMCU(TiltLeft);
				//SendMessageToMCU(TiltLeft);
				CurrentTask->TaskProgress++;
				break;
			default:
				CurrentTask->TaskProgress++;
				break;
			}
			break;
#pragma endregion

#pragma region Task Move to Right Pit and Tilt Pan
		case TaskMRTP:
			switch(CurrentTask->TaskProgress)
			{
			case 0:
				if(ClosePoints(CurrentTask->CompletionCoord, Robot.PatchCenter, Robot.HorizontalSpan/2))
				{
					SendMessageToMCU(StopMoving);
					SendMessageToMCU(TiltRight);
					CurrentTask->TaskProgress++;
				}
				break;
			case 90://Wait for 15 frames for Robot to Tilt pan and be stable
				ClearTask(CurrentTask);
				Robot.NumberOfRedBalls = 0;
				Robot.InTransit = 0;
				break;
			case 30:
				SendMessageToMCU(Neutral);
				//SendMessageToMCU(Neutral);
				CurrentTask->TaskProgress++;
				break;
			case 60:
				SendMessageToMCU(TiltRight);
				//SendMessageToMCU(TiltRight);
				CurrentTask->TaskProgress++;
				break;
			default:
				CurrentTask->TaskProgress++;
				break;
			}
			break;
#pragma endregion

		}
	}
}

void SetTask(CvSeq* BallList[3], _Task* CurrentTask)
{
	int i, j;
	int FarFromBlackBalls;
	_Ball* BallA;
	_Ball* BallB;
	_Ball* BallFarthest;
	int found = 0;
	int first = 0;

	if(!Robot.InTransit)
	{
		if(Robot.NumberOfBlueBalls <= 3 && BallList[Blue]->total > 0)
		{
			for(i = 0; i < BallList[Blue]->total; i++)
			{
				FarFromBlackBalls = 1;
				BallA = (_Ball*)cvGetSeqElem(BallList[Blue], i);
				if(BallA->ExpectedPosition.x < 0 || BallA->ExpectedPosition.y < 0)
					continue;
				if(first == 0)
				{
					first = 1;
					BallFarthest = BallA;
				}

				for(j = 0; j < BallList[Black]->total; j++)
				{
					BallB = (_Ball*)cvGetSeqElem(BallList[Black], j);
					if(GetHorizontalDistance(BallB->ExpectedPosition, BallA->ExpectedPosition) < Robot.HorizontalSpan)
					{
						FarFromBlackBalls = 0;
						break;
					}
				}
				if(FarFromBlackBalls)
				{
					if(GetVerticalDistance(Robot.PatchCenter, BallA->Center) >= GetVerticalDistance(Robot.PatchCenter, BallFarthest->Center))
					{
						BallFarthest = BallA;
						found = 1;
					}
					else if(GetVerticalDistance(Robot.PatchCenter, BallA->Center) > Platform.HorizontalSpan/2)
					{
						BallFarthest = BallA;
						found = 1;
					}
				}
			}
		}
		first = 0;
		if(Robot.NumberOfRedBalls <= 3 && BallList[Red]->total > 0 && found == 0)
		{
			for(i = 0; i < BallList[Red]->total; i++)
			{
				FarFromBlackBalls = 1;
				BallA = (_Ball*)cvGetSeqElem(BallList[Red], i);
				if(BallA->ExpectedPosition.x < 0 || BallA->ExpectedPosition.y < 0)
					continue;
				if(first == 0)
				{
					first = 1;
					BallFarthest = BallA;
				}

				for(j = 0; j < BallList[Black]->total; j++)
				{
					BallB = (_Ball*)cvGetSeqElem(BallList[Black], j);
					if(GetHorizontalDistance(BallB->ExpectedPosition, BallA->ExpectedPosition) < Robot.HorizontalSpan)
					{
						FarFromBlackBalls = 0;
						break;
					}
				}
				if(FarFromBlackBalls)
				{
					if(GetVerticalDistance(Robot.PatchCenter, BallA->Center) >= GetVerticalDistance(Robot.PatchCenter, BallFarthest->Center))
					{
						BallFarthest = BallA;
						found = 1;
					}
					else if(GetVerticalDistance(Robot.PatchCenter, BallA->Center) > Platform.HorizontalSpan/2)
					{
						BallFarthest = BallA;
						found = 1;
					}
				}
			}
		}
		if(found == 0)
		{
			CvPoint Temp;
			_Ball* BallNearest;
			cout<<"Global Time: "<<GlobalTime<<"\n";
			cout<<"No suitable ball found\n";
			
			first = 0;
			if(BallList[Black]->total > 0)
			{
				for(i = 0; i < BallList[Black]->total; i++)
				{
					BallA =  (_Ball*)cvGetSeqElem(BallList[Black], i);
					if(BallA->ExpectedPosition.x < 0 || BallA->ExpectedPosition.y < 0)
						continue;
					if(first == 0)
					{
						BallNearest = (_Ball*)cvGetSeqElem(BallList[Black], i);
						first = 1;
					}
					else
					{
						if(GetVerticalDistance(Robot.PatchCenter, BallA->Center) < GetVerticalDistance(Robot.PatchCenter, BallNearest->Center))
						{
							BallNearest = BallA;
						}
					}
				}
				if(first)
				{
					if(Pt1wrtPt2(BallNearest->ExpectedPosition, Robot.PatchCenter, 0) == Left)
						Robot.NumberOfRedBalls = 4;
					else
						Robot.NumberOfBlueBalls = 4;
				}
			}			

			if(Robot.NumberOfBlueBalls >= 3)
			{
				cout<<"Going to Left pit\n";
				PointLeftTo(&Platform.Center, &Temp, Platform.HorizontalSpan/2);
				CurrentTask->Task = TaskMLTP;
				Robot.InTransit = 1;
				SendMessageToMCU(MoveLeft);				
			}
			else if(Robot.NumberOfRedBalls >= 3)
			{
				cout<<"Going to Right pit\n";
				PointRightTo(&Platform.Center, &Temp, Platform.HorizontalSpan/2);
				CurrentTask->Task = TaskMRTP;
				Robot.InTransit = 1;
				SendMessageToMCU(MoveRight);
			}
			else
			{
				Temp = Platform.Center;
				if(ClosePoints(Temp, Robot.PatchCenter, Robot.HorizontalSpan/2))
					return;
				cout<<"Going to Center\n";
				CurrentTask->Task = TaskMC;
				if(Pt1wrtPt2(Platform.Center, Robot.PatchCenter, 0) == Left)
				{
					Robot.InTransit = 1;
					SendMessageToMCU(MoveLeft);
				}
				else
				{
					Robot.InTransit = 1;
					SendMessageToMCU(MoveRight);
				}					
			}
			cout<<"**********************************************************\n";

			if(Axis == X)
			{
				Temp.x = Robot.PatchCenter.x;
			}
			else
			{
				Temp.y = Robot.PatchCenter.y;
			}
			CurrentTask->CompletionCoord = Temp;
			CurrentTask->TaskProgress = 0;
		}
		if(found)
		{
			cout<<"Global Time: "<<GlobalTime<<"\n";
			cout<<"Suitable Ball Found\n";
			cout<<"BallID: "<<BallFarthest->BallID<<"\n";

			CurrentTask->BallColour = BallFarthest->Colour;
			CurrentTask->BallID = BallFarthest->BallID;
			CurrentTask->TaskProgress = 0;
			if(CurrentTask->BallColour == Blue)
				PointRightTo(&(BallFarthest->ExpectedPosition), &(CurrentTask->CompletionCoord), Robot.HorizontalSpan/4);
			else if(CurrentTask->BallColour == Red)
				PointLeftTo(&(BallFarthest->ExpectedPosition), &(CurrentTask->CompletionCoord), Robot.HorizontalSpan/4);
			if(Pt1wrtPt2(CurrentTask->CompletionCoord, Robot.PatchCenter, 0) == Left)
			{
				cout<<"TaskMLCB\n";
				CurrentTask->Task = TaskMLCB;
				Robot.InTransit = 1;
				SendMessageToMCU(MoveLeft);
			}
			else
			{
				cout<<"TaskMRCB\n";
				CurrentTask->Task = TaskMRCB;
				Robot.InTransit = 1;
				SendMessageToMCU(MoveRight);
			}
			cout<<"**********************************************************\n";
		}
	}
}