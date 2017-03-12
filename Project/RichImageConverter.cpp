// RichImageConverter.cpp : implementation file
#include "stdafx.h"
#include "RichImageConverter.h"

template class CRichImageConverter<byte>;
template class CRichImageConverter<int>;
template class CRichImageConverter<double>;

// CRichImageConverter
template <typename T>
CRichImageConverter<T>::CRichImageConverter()
{
	FDataPtr = NULL;
	FWidth = 0;
	FHeight = 0;
	FMinSignal = 0;
	FMaxSignal = 0;

	InBitmap = NULL;
	InBitmap_BitsPtr = NULL;
}

template <typename T>
CRichImageConverter<T>::~CRichImageConverter()
{
	ClearData();
}

template <typename T>
bool CRichImageConverter<T>::CreateBitmapDIB(int AWidth, int AHeight)
{
  // ��������� ��������� BITMAPINFO, ���������� ������ �����������
  BITMAPINFO BitmapInfo;
  memset(&BitmapInfo, 0, sizeof(BITMAPINFO));

	BitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	BitmapInfo.bmiHeader.biWidth = AWidth;
	BitmapInfo.bmiHeader.biHeight = -AHeight;
	BitmapInfo.bmiHeader.biPlanes = 1;
	BitmapInfo.bmiHeader.biBitCount = 24;
	BitmapInfo.bmiHeader.biCompression = BI_RGB;

  // ������� �������� ����������, ����������� � ������� �����
  HDC hDC = CreateCompatibleDC(NULL); 

	// �������� ��������� �� �������� ������ �����������
	InBitmap = CreateDIBSection(hDC, &BitmapInfo, DIB_RGB_COLORS, (void**)&(InBitmap_BitsPtr), NULL, 0);

	DeleteDC(hDC);
	
	return (InBitmap != NULL);
}

template <typename T>
void CRichImageConverter<T>::DeleteBitmapDIB(void)
{
	if (InBitmap != NULL)
	{
		DeleteObject(InBitmap);		
		InBitmap = NULL;
	}
}

// ������� ���� ��� mRichImage
template <typename T>
BOOL CRichImageConverter<T>::Create(CWnd* pParentWnd, UINT nID, const RECT& rect, DWORD dwStyle)
{
	return mRichImage.Create(pParentWnd, nID, rect); 
}

// ������� ���� mRichImage
template <typename T>
void CRichImageConverter<T>::DestroyWindow(void)
{
	mRichImage.DestroyWindow();
}

// ������������ ����������� mRichImage
template <typename T>
void CRichImageConverter<T>::RefreshImage(void)
{
	mRichImage.Invalidate();
}

// �������� ��������� � ������ mRichImage
template <typename T>
void CRichImageConverter<T>::SetImagePos(int x, int y, int cx, int cy, UINT nFlags)
{
	mRichImage.SetWindowPos(NULL, x, y, cx, cy, nFlags);
}



// ���������� ������ ������� �������� ����������
template <typename T>
void CRichImageConverter<T>::ClearData(void)
{
	if (FDataPtr)
		delete [] FDataPtr;

	FDataPtr = NULL;

	FWidth = 0;
	FHeight = 0;

	FMinSignal = 0;
	FMaxSignal = 0;

	DeleteBitmapDIB();
}

// ��������� ����������� �� ������������ �����
template <typename T>
void CRichImageConverter<T>::Load(const CString& AFileName)
{
	// �������� ������
	ClearData();

	// ��������� ����������� � Bitmap
	CImage LocalImage;
	LocalImage.Load(AFileName);

	// �������� ������ ��� ������� ����������
	FWidth = LocalImage.GetWidth();
	FHeight = LocalImage.GetHeight();
	T* pBuffer = new T[FWidth*FHeight];

	// ����������� ����������� � ������ FDataPtr
	byte* pBits = (byte *)LocalImage.GetBits();	
	byte* pRow = pBits;

	int ScanLine = LocalImage.GetPitch();
	int BytesPerPixel = LocalImage.GetBPP()/8;

	for (int i=0; i < FHeight; i++)
	{	
		for (int j=0; j < FWidth; j++)
		{
			byte bValue = pRow[j*BytesPerPixel + 0];
			byte gValue = pRow[j*BytesPerPixel + 1];
			byte rValue = pRow[j*BytesPerPixel + 2];

			pBuffer[i*FWidth + j] = (T)(0.299*rValue + 0.587*gValue + 0.114*bValue);
		}
		pRow += ScanLine;	
	}

	// ��������� ����������� � ������ ���������� byte
	this->Load(pBuffer, FHeight, FWidth);

	// ���������� _Min  _Max ������� �������
	SetSignalMinMax(0, 255);

	// ���������� ������
	delete [] pBuffer;
}

// ��������� ����������� �� ������� ���������� byte
template <typename T>
void CRichImageConverter<T>::Load(T* pInData, int AWidth, int AHeight)
{
	// �������� ������
	ClearData();

	// �������� ������
	FWidth = AWidth;
	FHeight = AHeight;
	FDataPtr = new T[FWidth*FHeight];

	// ����������� �����������	
	memcpy(FDataPtr, pInData, FWidth*FHeight*sizeof(T));

	// �������� ������ ��� ���������� �����������
	CreateBitmapDIB(AWidth, AHeight);
}

// ���������� ����� ��������
template <typename T>
void CRichImageConverter<T>::SetSignalMinMax(T _AMin, T _AMax)
{
	if (_AMin < _AMax)
	{
		FMinSignal = _AMin;
		FMaxSignal = _AMax;	
	}
	else
	{
		FMinSignal = _AMax;
		FMaxSignal = _AMin;	
	}
}

// ������������� ������ ���������� � �����������, ��������� �������� �������
template <typename T>
bool CRichImageConverter<T>::ConvertToImage(TPseudoColormap AColormap)
{
	if ((!FDataPtr) || (!InBitmap) || (!InBitmap_BitsPtr))
		return false;

	// �������� ��������� �� ������� �������������� ��������� � ����
	vec4 (CRichImageColormap::*pFunction_GetColor)(float);
		
	switch(AColormap)
	{
		case pcAutumn:
			pFunction_GetColor = &CRichImageColormap::GetColor_Autumn;
			break;
			
		case pcBone:
			pFunction_GetColor = &CRichImageColormap::GetColor_Bone;	
			break;

		case pcCool:
			pFunction_GetColor = &CRichImageColormap::GetColor_Cool;
			break;

		case pcCopper:
			pFunction_GetColor = &CRichImageColormap::GetColor_Copper;
			break;

		case pcHot:
			pFunction_GetColor = &CRichImageColormap::GetColor_Hot;	
			break;

		case pcHSV:
			pFunction_GetColor = &CRichImageColormap::GetColor_HSV;
			break;

		case pcJet:
			pFunction_GetColor = &CRichImageColormap::GetColor_Jet;
			break;

		case pcParula:
			pFunction_GetColor = &CRichImageColormap::GetColor_Parula;
			break;

		case pcPink:
			pFunction_GetColor = &CRichImageColormap::GetColor_Pink;	
			break;

		case pcSpring:
			pFunction_GetColor = &CRichImageColormap::GetColor_Spring;
			break;

		case pcSummer:
			pFunction_GetColor = &CRichImageColormap::GetColor_Summer;	
			break;

		case pcWinter:
			pFunction_GetColor = &CRichImageColormap::GetColor_Winter;
			break;
	}	

	// ���������� ��������� �� ������� �����������
	T* SourcePtr = FDataPtr;
	byte* DestPtr = (byte*) InBitmap_BitsPtr;

	// ���������� ��������� ��������� �����������
	BITMAP BitmapInfo;
	ASSERT( GetObject(InBitmap, sizeof(BITMAP), &BitmapInfo) != 0 );  
	int DestLineScan = BitmapInfo.bmWidthBytes;
	int DestBPP = BitmapInfo.bmBitsPixel / 8;

  // ����������� ����������� �� ��������, � �������� ������ ������
	for (int i=0; i < FHeight; i++)
	{	
		for (int j=0; j < FWidth; j++)
		{
			double PixelValue = SourcePtr[i*FWidth + j];			
			PixelValue = abs(PixelValue - FMinSignal) / abs(FMaxSignal - FMinSignal);
			
			vec4 FColor = (mColormap.*pFunction_GetColor)((float)PixelValue);

			DestPtr[j*DestBPP + 0] = (byte)(255 * FColor.b);		// ����� ����
			DestPtr[j*DestBPP + 1] = (byte)(255 * FColor.g);		// ������� ����
			DestPtr[j*DestBPP + 2] = (byte)(255 * FColor.r);		// ������� ����
		}

		DestPtr+= DestLineScan;
	}

	// ��������� ����������� � ��������� CRichImage
	mRichImage.LoadFromBitmap(InBitmap);

	return true;	
}

