#include <cv.h>
#include "GlobalVariables.h"
#include "Threshold.h"

inline void ReadThresholds(char *FileName, _Threshold BallThreshold[3], _Threshold *RobotPacthThreshold)
{

	//Movie6.avi

	//BallThreshold[Red].Max = cvScalar(85/*Blue*/, 110/*Green*/, 120/*Red*/);
	//BallThreshold[Red].Min = cvScalar(35/*Blue*/, 60/*Green*/, 80/*Red*/);

	//BallThreshold[Blue].Max = cvScalar(155/*Blue*/, 155/*Green*/, 115/*Red*/);
	//BallThreshold[Blue].Min = cvScalar(90/*Blue*/, 105/*Green*/, 75/*Red*/);

	//BallThreshold[Black].Max = cvScalar(83/*Blue*/, 105/*Green*/, 75/*Red*/);
	//BallThreshold[Black].Min = cvScalar(38/*Blue*/, 65/*Green*/, 45/*Red*/);

	BallThreshold[Red].Max = cvScalar(80/*Blue*/, 100/*Green*/, 120/*Red*/);
	BallThreshold[Red].Min = cvScalar(55/*Blue*/, 65/*Green*/, 75/*Red*/);

	BallThreshold[Blue].Max = cvScalar(125/*Blue*/, 130/*Green*/, 110/*Red*/);
	BallThreshold[Blue].Min = cvScalar(80/*Blue*/, 80/*Green*/, 65/*Red*/);

	BallThreshold[Black].Max = cvScalar(80/*Blue*/, 100/*Green*/, 85/*Red*/);
	BallThreshold[Black].Min = cvScalar(50/*Blue*/, 80/*Green*/, 60/*Red*/);

	//5:30 pm PC Camera

	//BallThreshold[Red].Max = cvScalar(110/*Blue*/, 150/*Green*/, 130/*Red*/);
	//BallThreshold[Red].Min = cvScalar(60/*Blue*/, 95/*Green*/, 80/*Red*/);

	//BallThreshold[Blue].Max = cvScalar(150/*Blue*/, 175/*Green*/, 100/*Red*/);
	//BallThreshold[Blue].Min = cvScalar(90/*Blue*/, 128/*Green*/, 50/*Red*/);

	//BallThreshold[Black].Max = cvScalar(90/*Blue*/, 135/*Green*/,90 /*Red*/);
	//BallThreshold[Black].Min = cvScalar(0/*Blue*/, 0/*Green*/, 0/*Red*/);

	//Movie3_2.avi
	//BallThreshold[Red].Max = cvScalar(80/*Blue*/, 80/*Green*/, 255/*Red*/);
	//BallThreshold[Red].Min = cvScalar(0/*Blue*/, 0/*Green*/, 150/*Red*/);

	//BallThreshold[Blue].Max = cvScalar(255/*Blue*/, 50/*Green*/, 50/*Red*/);
	//BallThreshold[Blue].Min = cvScalar(180/*Blue*/, 0/*Green*/, 0/*Red*/);

	//BallThreshold[Black].Max = cvScalar(20/*Blue*/, 20/*Green*/, 20/*Red*/);
	//BallThreshold[Black].Min = cvScalar(0/*Blue*/, 0/*Green*/, 0/*Red*/);

	//Moive5.3gp
	//BallThreshold[Red].Max = cvScalar(110/*Blue*/, 140/*Green*/, 100/*Red*/);
	//BallThreshold[Red].Min = cvScalar(70/*Blue*/, 100/*Green*/, 30/*Red*/);

	//BallThreshold[Blue].Max = cvScalar(160/*Blue*/, 175/*Green*/, 20/*Red*/);
	//BallThreshold[Blue].Min = cvScalar(115/*Blue*/, 126/*Green*/, 0/*Red*/);

	//BallThreshold[Black].Max = cvScalar(110/*Blue*/, 145/*Green*/, 10/*Red*/);
	//BallThreshold[Black].Min = cvScalar(65/*Blue*/, 100/*Green*/, 0/*Red*/);

	//Movie1.avi
	//BallThreshold[Red].Max = cvScalar(115/*Blue*/, 160/*Green*/, 150/*Red*/);
	//BallThreshold[Red].Min = cvScalar(55/*Blue*/, 90/*Green*/, 80/*Red*/);

	//BallThreshold[Blue].Max = cvScalar(255/*Blue*/, 195/*Green*/, 110/*Red*/);
	//BallThreshold[Blue].Min = cvScalar(150/*Blue*/, 140/*Green*/, 60/*Red*/);

	//BallThreshold[Black].Max = cvScalar(95/*Blue*/, 150/*Green*/, 70/*Red*/);
	//BallThreshold[Black].Min = cvScalar(40/*Blue*/, 75/*Green*/, 25/*Red*/);
}

inline int inRangeSS(CvScalar Scalar, _Threshold BallThreshold)
{
	int inRange = 1;
	int i;
	for(i = 0; i < 3; i++)
	{
		/*cout<<"Max.val["<<i<<"]    : "<<Max.val[i]<<"\n";
		cout<<"Scalar.val["<<i<<"] : "<<Scalar.val[i]<<"\n";
		cout<<"Min.val["<<i<<"]    : "<<Min.val[i]<<"\n\n";*/
		if(Scalar.val[i] > BallThreshold.Max.val[i] || Scalar.val[i] < BallThreshold.Min.val[i])
		{
			inRange = 0;
			break;
		}
	}
	/*cout<<"InRange :"<<inRange<<"\n\n";
	cvWaitKey();*/
	return inRange;
}