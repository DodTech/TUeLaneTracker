/******************************************************************************
* NXP Confidential Proprietary
* 
* Copyright (c) 2017 NXP Semiconductor;
* All Rights Reserved
*
* AUTHOR : Rameez Ismail
*
* THIS SOFTWARE IS PROVIDED BY NXP "AS IS" AND ANY EXPRESSED OR
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
* OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL NXP OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
* IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
* THE POSSIBILITY OF SUCH DAMAGE.
* ****************************************************************************/ 

#include "BufferingDAG_generic.h"

BufferingDAG_generic::BufferingDAG_generic() : mBufferPos(0)
{
	
}

int BufferingDAG_generic::init_DAG(const Templates & TEMPLATES, const size_t & BUFFER_SIZE)
{
	const int RES_H 	= mCAMERA.RES_VH(1);
	mVP_RANGE_V   		= TEMPLATES.VP_RANGE_V;
	mSPAN			= TEMPLATES.SPAN;
	mHORIZON_ICCS		= TEMPLATES.HORIZON_ICCS;
	
	mBufferPool.reset(new BufferPool<BufferingDAG_generic>(mSPAN, RES_H, BUFFER_SIZE));

	mGRADIENT_TAN_ROOT 	= TEMPLATES.GRADIENT_TAN_ROOT;
	mDEPTH_MAP_ROOT    	= TEMPLATES.DEPTH_MAP_ROOT;
	mFOCUS_MASK_ROOT   	= TEMPLATES.FOCUS_MASK_ROOT;
	mY_ICS	 	  	= TEMPLATES.Y_ICS;
	mX_ICS    	  	= TEMPLATES.X_ICS;

        mX_ICCS   		= mX_ICS + mCAMERA.O_ICS_ICCS.x;
        mY_ICCS   		= mY_ICS + mCAMERA.O_ICS_ICCS.y;

	return 0;
}


void BufferingDAG_generic::execute(cv::Mat& FrameGRAY )
{

#ifdef PROFILER_ENABLED
mProfiler.start("SETUP_ASYNC_TEMPLATES");
#endif
	mFuture = std::async([this]
	{
	   //Local Variables
	   WriteLock  lLock(_mutex, std::defer_lock);	
	   int lRowIndex;
	   cv::Rect lROI;

	   //Extract Depth Template
	   lRowIndex = mCAMERA.RES_VH(0) -  mSPAN; 
	   lROI = cv::Rect(0,lRowIndex,mCAMERA.RES_VH(1), mSPAN);
	
	   lLock.lock();
	    mDEPTH_MAP_ROOT(lROI).copyTo(mDepthTemplate);
	   lLock.unlock();

	   //Extract Focus Template
	   lRowIndex = mVP_RANGE_V	 - mVanishPt.V;
	   lROI = cv::Rect(0, lRowIndex, mCAMERA.RES_VH(1), mSPAN);

	   lLock.lock();
	    mFOCUS_MASK_ROOT(lROI).copyTo(mFocusTemplate);	
	   lLock.unlock();
	});

#ifdef PROFILER_ENABLED
mProfiler.end();
LOG_INFO_(LDTLog::TIMING_PROFILE)<<endl
				<<"******************************"<<endl
				<<  "Extract ROI and Shift Buffer." <<endl
				<<  "Max Time: " << mProfiler.getMaxTime("SETUP_ASYNC_TEMPLATES")<<endl
				<<  "Avg Time: " << mProfiler.getAvgTime("SETUP_ASYNC_TEMPLATES")<<endl
				<<  "Min Time: " << mProfiler.getMinTime("SETUP_ASYNC_TEMPLATES")<<endl
				<<"******************************"<<endl<<endl;	
				#endif




#ifdef PROFILER_ENABLED
mProfiler.start("EXTRACT_ROI");
#endif
	 int lRowIndex	= mCAMERA.RES_VH(0) -mSPAN;
	 int lColIndex;
	 cv::Rect lROI;

	 //Define ROI from the Input Image
	 lROI = cv::Rect(0, lRowIndex, mCAMERA.RES_VH(1), mSPAN);
	 mFrameGRAY_ROI = FrameGRAY(lROI);

	 //Extract Corrseponding Gradient Orientation Template
	 lRowIndex =  mCAMERA.RES_VH(0) - (mVP_RANGE_V + mVanishPt.V);
	 lColIndex =  mCAMERA.RES_VH(1) - (mCAMERA.O_ICCS_ICS.x - mVanishPt.H) ;
	 lROI	   =  cv::Rect(lColIndex, lRowIndex, mCAMERA.RES_VH(1), mSPAN);
	 mGRADIENT_TAN_ROOT(lROI).copyTo(mGradTanTemplate);


#ifdef PROFILER_ENABLED
mProfiler.end();
LOG_INFO_(LDTLog::TIMING_PROFILE)<<endl
				<<"******************************"<<endl
				<<  "Extract ROI from Input Image and the Gradient Direction." <<endl
				<<  "Max Time: " << mProfiler.getMaxTime("EXTRACT_ROI")<<endl
				<<  "Avg Time: " << mProfiler.getAvgTime("EXTRACT_ROI")<<endl
				<<  "Min Time: " << mProfiler.getMinTime("EXTRACT_ROI")<<endl
				<<"******************************"<<endl<<endl;	
				#endif




#ifdef PROFILER_ENABLED
mProfiler.start("GAUSSIAN_BLUR");
#endif 
	
	 GaussianBlur( mFrameGRAY_ROI, mFrameGRAY_ROI, cv::Size( 5, 5 ), 2, 2, cv::BORDER_REPLICATE | cv::BORDER_ISOLATED  );

 #ifdef PROFILER_ENABLED
mProfiler.end();
LOG_INFO_(LDTLog::TIMING_PROFILE)<<endl
				<<"******************************"<<endl
				<<  "Gaussian Filtering." <<endl
				<<  "Max Time: " << mProfiler.getMaxTime("GAUSSIAN_BLUR")<<endl
				<<  "Avg Time: " << mProfiler.getAvgTime("GAUSSIAN_BLUR")<<endl
				<<  "Min Time: " << mProfiler.getMinTime("GAUSSIAN_BLUR")<<endl
				<<"******************************"<<endl<<endl;	
				#endif




#ifdef PROFILER_ENABLED
mProfiler.start("GRADIENT_COMPUTATION");
#endif 								

	int scale = 1;
	int delta = 0;
	int ddepth = CV_16S;
	
	Sobel( mFrameGRAY_ROI, mGradX, ddepth, 1, 0, 3, scale, delta, cv::BORDER_REPLICATE | cv::BORDER_ISOLATED);
	Sobel( mFrameGRAY_ROI, mGradY, ddepth, 0, 1, 3, scale, delta, cv::BORDER_REPLICATE | cv::BORDER_ISOLATED);


	mMask = mGradX> 255;
	mGradX.setTo(255, mMask);
	mMask = mGradX <-255;
	mGradX.setTo(-255, mMask);

	mMask = mGradY> 255;
	mGradY.setTo(255, mMask);
	mMask = mGradY <-255;
	mGradY.setTo(-255, mMask);
	mMask = mGradY ==0;
	mGradY.setTo(1, mMask);
			
	//convert to absolute scale and add weighted absolute gradients 
	mGradX_abs = abs(mGradX);
	mGradY_abs = abs(mGradY );
	
	//addWeighted( mGradX_abs, 0.5, mGradY_abs, 0.5, 0, mFrameGradMag );
	mFrameGradMag = mGradX_abs + mGradY_abs;

	//convertScaleAbs(mFrameGradMag, mFrameGradMag);
	mFrameGradMag.convertTo(mFrameGradMag, CV_8U);

	cv::divide(mGradX, mGradY, mBufferPool->GradientTangent[mBufferPos], 128, -1);

 #ifdef PROFILER_ENABLED
 mProfiler.end();
 LOG_INFO_(LDTLog::TIMING_PROFILE)<<endl
				<<"******************************"<<endl
				<<  "Gradients Computations." <<endl
				<<  "Max Time: " << mProfiler.getMaxTime("GRADIENT_COMPUTATION")<<endl
				<<  "Avg Time: " << mProfiler.getAvgTime("GRADIENT_COMPUTATION")<<endl
				<<  "Min Time: " << mProfiler.getMinTime("GRADIENT_COMPUTATION")<<endl
			  	<<"******************************"<<endl<<endl;	
			 	#endif




#ifdef PROFILER_ENABLED
mProfiler.start("COMPUTE_PROBABILITIES");
#endif 		

	//GrayChannel Probabilities
	subtract(mFrameGRAY_ROI, mLaneMembership.TIPPING_POINT_GRAY, mTempProbMat, cv::noArray(), CV_32S);
	mMask = mTempProbMat <0 ;
	mTempProbMat.setTo(0,mMask);
	mTempProbMat.copyTo(mProbMap_Gray);
	mTempProbMat = mTempProbMat + 10;
	
	divide(mProbMap_Gray, mTempProbMat, mProbMap_Gray, 255, -1);


	//GradientMag Probabilities
	subtract(mFrameGradMag, mLaneMembership.TIPPING_POINT_GRAD_Mag, mTempProbMat, cv::noArray(), CV_32S);
	mTempProbMat.copyTo(mProbMap_GradMag);
	mTempProbMat= abs(mTempProbMat) + 10;
	divide(mProbMap_GradMag, mTempProbMat, mProbMap_GradMag, 255, -1);


	// Intermediate Probability Map
	mBufferPool->Probability[mBufferPos] = mProbMap_GradMag + mProbMap_Gray;
	mMask = mBufferPool->Probability[mBufferPos] <0 ;
	mBufferPool->Probability[mBufferPos].setTo(0,mMask);


	//Gradient Tangent Probability Map
	subtract(mGradTanTemplate, mBufferPool->GradientTangent[mBufferPos], mTempProbMat, cv::noArray(), CV_32S);
	mTempProbMat= abs(mTempProbMat);
	mTempProbMat.copyTo(mProbMap_GradDir);
	mTempProbMat = mTempProbMat + 60;
	divide(mProbMap_GradDir, mTempProbMat, mProbMap_GradDir, 255, -1);
	subtract(255, mProbMap_GradDir, mProbMap_GradDir, cv::noArray(), -1);

	
	//Final Probability Map
	multiply(mBufferPool->Probability[mBufferPos], mProbMap_GradDir, mBufferPool->Probability[mBufferPos]);
	mBufferPool->Probability[mBufferPos].convertTo(mBufferPool->Probability[mBufferPos], CV_8U, 1.0/255, 0);


#ifdef PROFILER_ENABLED
mProfiler.end();
LOG_INFO_(LDTLog::TIMING_PROFILE)<<endl
				<<"******************************"<<endl
				<<  "Compute total LaneMarker Probability ." <<endl
				<<  "Max Time: " << mProfiler.getMaxTime("COMPUTE_PROBABILITIES")<<endl
				<<  "Avg Time: " << mProfiler.getAvgTime("COMPUTE_PROBABILITIES")<<endl
				<<  "Min Time: " << mProfiler.getMinTime("COMPUTE_PROBABILITIES")<<endl
				<<"******************************"<<endl<<endl;	
				#endif


#ifdef PROFILER_ENABLED
mProfiler.start("FOCUS");
#endif 							

	mFuture.wait();
	bitwise_and(mBufferPool->Probability[mBufferPos], mFocusTemplate, mBufferPool->Probability[mBufferPos]);

	if(mBufferPos < ((mBufferPool->Probability.size())-1) )
	mBufferPos ++;
				
#ifdef PROFILER_ENABLED
mProfiler.end();
LOG_INFO_(LDTLog::TIMING_PROFILE) <<endl
				<<"******************************"<<endl
				<<  "Waiting for async task, focussing and adjusting buffer position." <<endl
				<<  "Max Time: " << mProfiler.getMaxTime("FOCUS")<<endl
				<<  "Avg Time: " << mProfiler.getAvgTime("FOCUS")<<endl
				<<  "Min Time: " << mProfiler.getMinTime("FOCUS")<<endl
				<<"******************************"<<endl<<endl;	
				#endif	
}
