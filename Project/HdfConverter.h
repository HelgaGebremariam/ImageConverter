#pragma once
#include "hdf5.h"
class HdfConverter
{
public:

	void ConvertHdfToBitmap();
	bool Save2HDF(double* dArray2D, const int nRows, const int nColumns, const char* sFileName);
};