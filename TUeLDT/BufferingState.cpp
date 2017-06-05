#include "BufferingState.h"

BufferingState::BufferingState()
{
	#ifdef PROFILER_ENABLED
	getOpenClInfo();
	#endif	
}



/* Setup Input Source*/   
#ifdef DIRECTORY_INPUT
	void BufferingState::setSource(const vector<cv::String>& files)
	{
		bufferingGraph.mFiles = files;
		bufferingGraph.mCountFrame =0;
	}

#else

	void BufferingState::setSource()
	{

	}
	
#endif



	/* Setup RootTemplates and BufferPool */  
	void BufferingState::setupDAG(const Templates& templates)
	{
				
		#ifdef PROFILER_ENABLED
			mProfiler.start("SetRootTemplates");
		#endif
			
		//if GRADIENT_TAN_ROOT is defined as UMAT than it should interpoerable
		// In that case we can assign a opencv::UMat to s32v::UMAT
		// Only right class has to be instantiated as bufferingGraph
		
		
		bufferingGraph.mVP_Range_V        = templates.VP_RANGE_V;
		bufferingGraph.mSpan			  = templates.SPAN;
		bufferingGraph.mMargin			  = templates.MARGIN;
		
		bufferingGraph.mGRADIENT_TAN_ROOT = templates.GRADIENT_TAN_ROOT;
		bufferingGraph.mDEPTH_MAP_ROOT    = templates.DEPTH_MAP_ROOT;
		bufferingGraph.mFOCUS_MASK_ROOT   = templates.FOCUS_MASK_ROOT;
		
		const int RES_V = bufferingGraph.mCAMERA.RES_VH(0);
		const int RES_H = bufferingGraph.mCAMERA.RES_VH(1);
		bufferingGraph.mBufferPool.reset(new BufferPool(RES_V, RES_H)); 
		
		this->currentStatus= StateStatus::ACTIVE;
					
		 #ifdef PROFILER_ENABLED
			mProfiler.end();
			LOG_INFO_(LDTLog::BUFFERING_PROFILE) <<endl
			<<"******************************"<<endl
			<<  "Completing Buffering Setup." <<endl
			<<  "ROOT Templates copy Time: "  << mProfiler.getAvgTime("SetRootTemplates")<<endl
			<<"******************************"<<endl<<endl;	
		 #endif
				
		}



	// Run Directed Acyclic Graph
	void BufferingState::run()
	{
		
		#ifdef PROFILER_ENABLED
			mProfiler.start("SingleRun");
		#endif	
			

		if(this->StateCounter < sNbBuffer)
		  this->StateCounter++;
	   //else
		//this->currentStatus = StateStatus::DONE;		
		
		
		if (mSideExecutor.joinable())
			mSideExecutor.join();
				
			mSideExecutor =
			#ifndef s32v2xx
				std::thread(&BufferingDAG_generic::auxillaryTasks, std::ref(bufferingGraph));
			#endif
		
		if (0==bufferingGraph.grabFrame())
			bufferingGraph.executeDAG_buffering();
				
		else
			currentStatus = StateStatus::ERROR;
														

		 #ifdef PROFILER_ENABLED
			 mProfiler.end();
			 LOG_INFO_(LDTLog::BUFFERING_PROFILE) <<endl
			  <<"******************************"<<endl
			  <<  "Completing a run loop." <<endl
			  <<  "Single run-loop time: " << mProfiler.getAvgTime("SingleRun")<<endl
			  <<"******************************"<<endl<<endl;	
		 #endif
	}



/* ********************************************* /*
/*  Low level Class PRIVATE FUnction
 *  getOpenClInfo()
*/

#ifdef PROFILER_ENABLED

	void BufferingState::getOpenClInfo()
	{
		
		if (!cv::ocl::haveOpenCL())
		{
			 LOG_INFO_(LDTLog::BUFFERING_LOG) <<endl
			  <<"******************************"<<endl
			  <<  "OpenCL Info:" <<endl
			  <<  "Sorry, OpenCl is not available. " <<endl
			  <<"******************************"<<endl<<endl;	
			//return;
		}

		cv::ocl::Context context;
		if (!context.create(cv::ocl::Device::TYPE_GPU))
		{
			LOG_INFO_(LDTLog::BUFFERING_LOG) <<endl
							  <<"******************************"<<endl
							  <<  "OpenCL Info:" <<endl
							  <<  "Sorry, Failed creating the Context. " <<endl
							  <<"******************************"<<endl<<endl;	
			//return;
		}

		else
		{
		
		
			for (int i = 0; i < context.ndevices(); i++)
			{
				cv::ocl::Device device = context.device(i);
				std::string name= device.name();
				std::string OpenCL_C_Version= device.OpenCL_C_Version();
				
				LOG_INFO_(LDTLog::BUFFERING_LOG) <<endl
				  <<"***********************************"<<endl
				  <<  "OpenCL Device Detected:" <<endl
				  <<  "name:  							"<< name<< endl
				  <<  "available:						"<< device.available()<< endl
				  <<  "imageSupport:					"<< device.imageSupport()<< endl
				  <<  "globalMemSize:					"<< device.globalMemSize()<< endl
				  <<  "localMemSize:					"<< device.localMemSize()<< endl
				  <<  "maxWorkGroup:					"<< device.maxWorkGroupSize()<< endl
				  <<  "maxWorkItemDim:					"<< device.maxWorkItemDims()<< endl
				  <<  "maxComputeUnits:					"<< device.maxComputeUnits()<< endl
				  <<  "preferredVectorWidthChar:		"<< device.preferredVectorWidthChar()<< endl
				  <<  "preferredVectorWidthDouble:		"<< device.preferredVectorWidthDouble()<< endl
				  <<  "preferredVectorWidthFloat:		"<< device.preferredVectorWidthFloat()<< endl
				  <<  "preferredVectorWidthHalf:		"<< device.preferredVectorWidthHalf()<< endl
				  <<  "preferredVectorWidthLong:		"<< device.preferredVectorWidthLong()<< endl
				  <<  "preferredVectorWidthShort:		"<< device.preferredVectorWidthShort()<< endl
				  <<  "image2DMaxHeight:				"<< device.image2DMaxHeight()<< endl
				  <<  "image2DMaxWidth:					"<< device.image2DMaxWidth()<< endl
				  <<  "OpenCL_C_Version:				"<< OpenCL_C_Version<< endl
				  <<"***********************************"<<endl<<endl;
			}
		
		}

	}
	
#endif


BufferingState::~BufferingState()
{
	if (mSideExecutor.joinable())
		mSideExecutor.join();	
}
