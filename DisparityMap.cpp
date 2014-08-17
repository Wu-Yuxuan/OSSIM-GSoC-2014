//----------------------------------------------------------------------------
//
// License:  See top level LICENSE.txt file.
//
// File: DisparityMap.cpp
//
// Author:  Martina Di Rita
//
// Description: Class provides Disparity Map extraction
//
//----------------------------------------------------------------------------

#include <ossim/base/ossimString.h>
#include <ossim/base/ossimNotify.h>
#include <ossim/base/ossimTrace.h>
#include <ossim/base/ossimIrect.h>
#include <ossim/base/ossimRefPtr.h>
#include <ossim/base/ossimConstants.h>
#include <ossim/elevation/ossimElevManager.h>
#include <ossim/imaging/ossimImageData.h>
#include <ossim/imaging/ossimImageSource.h>
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/mean.hpp>
#include <boost/accumulators/statistics/median.hpp>
#include <boost/accumulators/statistics/variance.hpp>
#include "DisparityMap.h"

#include <opencv/highgui.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/flann/flann.hpp>
#include <opencv2/legacy/legacy.hpp>
// Note: These are purposely commented out to indicate non-use.
// #include <opencv2/nonfree/nonfree.hpp>
// #include <opencv2/nonfree/features2d.hpp>
// Note: These are purposely commented out to indicate non-use.
#include <vector>
#include <iostream>

DisparityMap::DisparityMap()
{
	
}

cv::Mat DisparityMap::execute(cv::Mat master_mat, cv::Mat slave_mat)
{
	cout << "DISPARITY MAP GENERATION..." << endl;
	// Disparity Map generation
	int ndisparities = 16*2; //Maximum disparity minus minimum disparity 
	int SADWindowSize = 3;   //Matched block size

	cv::StereoSGBM sgbm;

	sgbm.preFilterCap = 63;
	sgbm.SADWindowSize = SADWindowSize > 0 ? SADWindowSize : 3;

	int cn = master_mat.channels();

	sgbm.P1 = 8*cn*sgbm.SADWindowSize*sgbm.SADWindowSize;
	sgbm.P2 = 32*cn*sgbm.SADWindowSize*sgbm.SADWindowSize;
	sgbm.minDisparity = -16; // Minimum possible disparity value
	sgbm.numberOfDisparities = ndisparities;
	sgbm.uniquenessRatio = 10;
	sgbm.speckleWindowSize = 100;
	sgbm.speckleRange = 32;
	sgbm.disp12MaxDiff = 1; // Maximum allowed difference (in integer pixel units) in the left-right disparity check
	
	double minVal, maxVal;
	cv::Mat array_disp;
	cv::Mat array_disp_8U;   
	sgbm(master_mat, slave_mat, array_disp);
	minMaxLoc( array_disp, &minVal, &maxVal );
	array_disp.convertTo( array_disp_8U, CV_8UC1, 255/(maxVal - minVal), -minVal*255/(maxVal - minVal));   
    
	cv::namedWindow( "SGM Disparity", CV_WINDOW_NORMAL );
	cv::imshow( "SGM Disparity", array_disp_8U);

	cv::waitKey(0);
	
	return array_disp;
}



