#include "stdafx.h"
#include "HdfConverter.h"
const int    NX_SUB = 3;    // hyperslab dimensions
const int    NY_SUB = 4;
const int    NX = 7;        // output buffer dimensions
const int    NY = 7;
const int    NZ = 3;
const int    RANK_OUT = 3;
struct RGBA
{
	
	byte r;
	byte g;
	byte b;
	byte a;
	
};
union DoubleColor
{
	RGBA color;
	double doubleColor;
};
herr_t op_func(hid_t loc_id, const char *name, const H5O_info_t *info,
	void *operator_data)
{

	if (name[0] == '.')         /* Root group, do not print '.' */
		printf("  (Group)\n");
	else
		switch (info->type) {
		case H5O_TYPE_GROUP:
			printf("%s  (Group)\n", name);
			break;
		case H5O_TYPE_DATASET:
			printf("%s  (Dataset)\n", name);
			break;
		case H5O_TYPE_NAMED_DATATYPE:
			printf("%s  (Datatype)\n", name);
			break;
		default:
			printf("%s  (Unknown)\n", name);
		}

	return 0;
}

double* HdfConverter::ConvertHdfImageToCImage(CString sFileName, const char* sDatasetName, int& width, int& height)
{
	int nRows = 0, nColumns = 0;
	char *filename = new char[sFileName.GetLength() + 1];
	strcpy(filename, (LPCTSTR)sFileName);
	H5File file(filename, H5F_ACC_RDWR);
	int c = file.getNumObjs();
	DataSet dataset = file.openDataSet(sDatasetName);

	hid_t hFile;
	herr_t status;

	hFile = H5Fopen(filename, H5F_ACC_RDONLY, H5P_DEFAULT);
	status = H5Ovisit(hFile, H5_INDEX_NAME, H5_ITER_NATIVE, op_func, NULL);

	H5T_class_t type_class = dataset.getTypeClass();

	DataSpace dataspace = dataset.getSpace();

	int rank = dataspace.getSimpleExtentNdims();

	hsize_t *dims_out = new hsize_t[rank];
	int ndims = dataspace.getSimpleExtentDims(dims_out, NULL);
	DataSpace dataspaceOut = DataSpace(rank, dims_out);
	double  *data_out = new double[dims_out[1] * dims_out[0]];

	dataset.read(data_out, PredType::IEEE_F64LE, dataspaceOut, dataspace);

	width = dims_out[1];
	height = dims_out[0];

	//Mat img = Mat(height, width, CV_64F, data_out);

	//cv::imwrite("C://testimage.bmp", img);

	//CImage cimage;
	//cimage.Create(width, height, 8);

	//for(int  i =0; i < width; i ++)
	//	for (int j = 0; j < height; j++)
	//	{
	//		DoubleColor color;
	//		color.doubleColor = data_out[i*j + j];
	//		cimage.SetHasAlphaChannel( false);
	//		float alfa = color.color.a / 255;
	//		cimage.SetPixelRGB(i, j, byte(color.color.r * alfa), byte(color.color.g* alfa), byte(color.color.b*alfa));
	//	}
	delete[] filename;
	return data_out;

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
	delete[]datasetData;
	return (writeStatus == 0 && datasetCloseStatus == 0 && fileCloseStatus == 0);
}