#include "compileConfig.h"
#include <iostream>
#include <string>
#include <memory>
#include <sys/stat.h>
#include <Eigen/Dense>
#include "opencv2/opencv.hpp"
#include <opencv2/core/eigen.hpp>
#include <math.h>
#include "LDT_profiler.h"
#include "LDT_logger.h"



#ifdef s32v2xx
#include "frame_output_v234fb.h"
#endif

#ifndef STATE_H
#define STATE_H

using namespace std;
using namespace Eigen;
using namespace cv;


enum States{BOOTING, BUFFERING, DETECTING_LANES, RESETING, DISPOSING }; //^TODO: Add PAUSED state as well
enum StateStatus {DONE, ACTIVE, INACTIVE, ERROR };


class State
{

	
protected:
	#ifdef PROFILER_ENABLED
	ProfilerLDT mProfiler;
	#endif
	
public:

	static const  int      sNbBuffer =3;
	int 			       StateCounter;     	
	StateStatus 	       currentStatus;
	
	void printStatus();
	
	State();
	~State();
};





/******************************************************************************************* */
								/*  TYPE_DEFINITIONS */
/******************************************************************************************* */

struct Templates
{
	
	public:
		Mat GRADIENT_TAN_ROOT;
		Mat FOCUS_MASK_ROOT;
		Mat DEPTH_MAP_ROOT;

  
  
	Templates(const int RES_V, const int RES_H, const int VP_RANGE_V)
	{
		   
		/* Create Focus Template */
			const int Margin  = 80;
			const int span = (RES_V/2)-Margin + VP_RANGE_V;
			
			MatrixXi FOCUS_ROOT     = MatrixXi::Zero(span + 2*VP_RANGE_V, RES_H);
			FOCUS_ROOT.block(2*VP_RANGE_V, 0, span, RES_H) = MatrixXi::Constant(span, RES_H, 1);			
			eigen2cv(FOCUS_ROOT, FOCUS_MASK_ROOT);

			
		/* Create Depth Template */

			MatrixXf DEPTH_ROOT  = MatrixXf::Zero(RES_V,RES_H);			
			const float step 	= 45.0/RES_V;
			float angle = 90- step;
			
			for(int n = 0; n < RES_V ; n++)
			{
			  float x= 1.75 * tan(angle * M_PI/180.0 );
			   
			   if( x > 100) x=100;
			  DEPTH_ROOT.row(n) = ArrayXf::Constant(RES_H, x);
			  angle = angle - step;
			}
			
			eigen2cv(DEPTH_ROOT, DEPTH_MAP_ROOT);
			



	  /* Load Gradient Tangent Template */
	    std::stringstream formattedString;
		string templateFile, prefix, format;
		prefix= "Templates/GradientTangent_";
		formattedString<<prefix<<std::to_string(RES_H)<<"x"<<std::to_string(RES_V);
		templateFile = formattedString.str();
		struct stat buf;
		int statResult = stat(templateFile.c_str(),&buf);
		if (statResult || buf.st_ino < 0) 
		{
			cout << "File not found: " << templateFile.c_str() << endl;
			exit(-2);
		}
		else
		{
			FileStorage loadGradientTemplate( templateFile, FileStorage::READ);
			loadGradientTemplate["ROOT_DIR_TEMPLATE"]>> GRADIENT_TAN_ROOT;
			GRADIENT_TAN_ROOT.convertTo(GRADIENT_TAN_ROOT,CV_16SC1);
		}		
	}	
  
}; 



struct VanishingPt
{
  int  V;
  int  H;
  int  V_prev;
  int  H_prev;
  VanishingPt()
  : V(0),H(0),V_prev(0),H_prev(0){}
};



struct Likelihoods
{
		std::array<Mat, State::sNbBuffer> TOT_ALL;
		std::array<Mat, State::sNbBuffer> GRADIENT_DIR_ALL;
		
		Mat  TOT_MAX;		
		Mat  GRADIENT_DIR_TOT_MAX;
		Mat  TOT_MAX_FOCUSED;
		
		
		Likelihoods(const int RES_V, const int RES_H)
		{
			
				for (int i=0; i< State::sNbBuffer; i++)
				{
					TOT_ALL[i]= Mat::zeros(RES_V, RES_H,  CV_8UC1);
					GRADIENT_DIR_ALL[i]= Mat::zeros(RES_V,RES_H, CV_16SC1);
				}
				
				TOT_MAX =  Mat::zeros(RES_V,RES_H, CV_32F);
				GRADIENT_DIR_TOT_MAX = Mat::zeros(RES_V,RES_H, CV_16SC1);
				TOT_MAX_FOCUSED = Mat::zeros(RES_V,RES_H, CV_8UC1);	
		}		
};

#endif // STATE_H
