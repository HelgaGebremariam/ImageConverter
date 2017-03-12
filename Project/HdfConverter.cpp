#include "stdafx.h"
#include "HdfConverter.h"


void HdfConverter::ConvertHdfToBitmap()
{

}

bool HdfConverter::Save2HDF(double* dArray2D, const int nRows, const int nColumns, const char* sFileName)
{
	hid_t       fileId, datasetId, dataspaceId;
	herr_t      writeStatus, datasetCloseStatus, fileCloseStatus;
	hsize_t     dims[2];
	double** datasetData = new double*[nRows];

	for (int i = 0; i< nRows; i++)
		datasetData[i] = new double[nColumns];

	for (int i = 0; i < nRows; i++)
		for (int j = 0; j < nColumns; j++)
			datasetData[i][j] = dArray2D[i*j + j];

	dims[0] = nRows;
	dims[1] = nColumns;
	dataspaceId = H5Screate_simple(2, dims, NULL);

	fileId = H5Fcreate(sFileName, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
	datasetId = H5Dcreate2(fileId, "/dset", H5T_STD_I32BE, dataspaceId,
		H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
	writeStatus = H5Dwrite(datasetId, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT,
		datasetData);

	datasetCloseStatus = H5Dclose(datasetId);
	fileCloseStatus = H5Fclose(fileId);

	return (writeStatus == 0 && datasetCloseStatus == 0 && fileCloseStatus == 0);
	return true;
}