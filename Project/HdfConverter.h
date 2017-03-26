#pragma once
#include "hdf5.h"
#include <iostream>
#include <string>
#include "H5Cpp.h"
#ifdef OLD_HEADER_FILENAME
#include <iostream.h>
#else
#include <iostream>
#endif
#ifndef H5_NO_NAMESPACE
#ifndef H5_NO_STD
using std::cout;
using std::endl;
#endif  // H5_NO_STD
#endif
#ifndef H5_NO_NAMESPACE
using namespace H5;
#endif

#include "cv.h"
#include "highgui.h"
#include <opencv2/opencv.hpp>
using namespace cv;

class HdfConverter
{
public:

	CImage ConvertHdfImageToCImage(CString sFileName, const char* sDatasetName);
	bool Save2HDF(double* dArray2D, const int nRows, const int nColumns, const char* sFileName);

private:

};