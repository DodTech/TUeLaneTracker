#ifndef VANSIHINGPOINFILTER_H
#define VANSIHINGPOINFILTER_H
#include  "LaneFilter.h"
#include <memory>
#include <Eigen/Dense>
#include <math.h>
#include "Lane.h"
#include "Camera.h"

using namespace std;
using namespace Eigen;

	class VanishingPtFilter
	{

		
	public:

		VanishingPtFilter(const Ref<const VectorXi>& BINS_LANE_HISTOGRAM,  const CameraProperties& CAMERA);
		~VanishingPtFilter();

	private: 
			//const VectorXi  mBINS_LANE_HISTOGRAM;
			const int    	mFRAME_CENTER_V;
			const int    	mFRAME_CENTER_H;
			const int 		mLANE_FILTER_OFFSET_V;   	 //CONSTANT: might be need outside ----> make public
			const int  	 	mVP_FILTER_OFFSET_V;        //CONSTANT: might be needed outside ---> make public
			const float     mVP_LANE_RATIO;
			void  			createPrior();
		

	public:

		const int 	 	mRANGE_V;
		const int 	 	mRANGE_H;
		const int    	mSTEP;
		const int    	mNb_BINS_V;    	// number of bins in the  vertical   direction.
		const int    	mNb_BINS_H;    	// number of bins in the horizental direction.
		const VectorXi 	mBINS_V;	    //Histogram Bins in Pixels.
		const VectorXi 	mBINS_H;     	// Histogram Bins in Pixels.
		const VectorXi mBINS_HISTOGRAM;	    //Histogram Bins in Pixels.
		
		MatrixXf     mPrior;
		Matrix3d	 mTransition; 
		MatrixXf     mFilter;
		 
		

		//const Ref<const MatrixXd>& getFilter();
		//shared_ptr<MatrixXd> getPrior();
		//shared_ptr<MatrixXd> getTransition(); //^TODO: Make it Private Memeber if only used by LaneFilter class.
		
		
		//LaneFilter(const shared_ptr<const Lane>, const shared_ptr<const Camera>);
		 

	};
	
#endif // VANSIHINGPOINFILTER_H
