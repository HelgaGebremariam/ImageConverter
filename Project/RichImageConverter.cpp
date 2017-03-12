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
  // заполнить структуру BITMAPINFO, содержащую формат изображения
  BITMAPINFO BitmapInfo;
  memset(&BitmapInfo, 0, sizeof(BITMAPINFO));

	BitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	BitmapInfo.bmiHeader.biWidth = AWidth;
	BitmapInfo.bmiHeader.biHeight = -AHeight;
	BitmapInfo.bmiHeader.biPlanes = 1;
	BitmapInfo.bmiHeader.biBitCount = 24;
	BitmapInfo.bmiHeader.biCompression = BI_RGB;

  // создать контекст устройства, совместимый с главным окном
  HDC hDC = CreateCompatibleDC(NULL); 

	// получить указатель на байтовый массив изображения
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

// создать окно для mRichImage
template <typename T>
BOOL CRichImageConverter<T>::Create(CWnd* pParentWnd, UINT nID, const RECT& rect, DWORD dwStyle)
{
	return mRichImage.Create(pParentWnd, nID, rect); 
}

// удалить окно mRichImage
template <typename T>
void CRichImageConverter<T>::DestroyWindow(void)
{
	mRichImage.DestroyWindow();
}

// перерисовать изображение mRichImage
template <typename T>
void CRichImageConverter<T>::RefreshImage(void)
{
	mRichImage.Invalidate();
}

// изменить положение и размер mRichImage
template <typename T>
void CRichImageConverter<T>::SetImagePos(int x, int y, int cx, int cy, UINT nFlags)
{
	mRichImage.SetWindowPos(NULL, x, y, cx, cy, nFlags);
}



// освободить память занятую массивом плотностей
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

// загрузить изображение из графического файла
template <typename T>
void CRichImageConverter<T>::Load(const CString& AFileName)
{
	// очистить память
	ClearData();

	// загрузить изображение в Bitmap
	CImage LocalImage;
	LocalImage.Load(AFileName);

	// выделить память для массива плотностей
	FWidth = LocalImage.GetWidth();
	FHeight = LocalImage.GetHeight();
	T* pBuffer = new T[FWidth*FHeight];

	// скопировать изображение в массив FDataPtr
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

	// сохранить изображение в массив плотностей byte
	this->Load(pBuffer, FHeight, FWidth);

	// установить _Min  _Max уровень сигнала
	SetSignalMinMax(0, 255);

	// освободить память
	delete [] pBuffer;
}

// загрузить изображение из массива плотностей byte
template <typename T>
void CRichImageConverter<T>::Load(T* pInData, int AWidth, int AHeight)
{
	// очистить память
	ClearData();

	// выделить память
	FWidth = AWidth;
	FHeight = AHeight;
	FDataPtr = new T[FWidth*FHeight];

	// скопировать изображение	
	memcpy(FDataPtr, pInData, FWidth*FHeight*sizeof(T));

	// выделить память под внутреннее изображение
	CreateBitmapDIB(AWidth, AHeight);
}

// определить шкалу сиганала
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

// преобразовать массив плотностей в изображение, использую цветовую палитру
template <typename T>
bool CRichImageConverter<T>::ConvertToImage(TPseudoColormap AColormap)
{
	if ((!FDataPtr) || (!InBitmap) || (!InBitmap_BitsPtr))
		return false;

	// получить указатель на функцию преобразования плотности в цвет
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

	// определить указатели на массивы изображений
	T* SourcePtr = FDataPtr;
	byte* DestPtr = (byte*) InBitmap_BitsPtr;

	// определить параметры выходного изображения
	BITMAP BitmapInfo;
	ASSERT( GetObject(InBitmap, sizeof(BITMAP), &BitmapInfo) != 0 );  
	int DestLineScan = BitmapInfo.bmWidthBytes;
	int DestBPP = BitmapInfo.bmBitsPixel / 8;

  // скопировать изображение из источнка, в байтовый массив данных
	for (int i=0; i < FHeight; i++)
	{	
		for (int j=0; j < FWidth; j++)
		{
			double PixelValue = SourcePtr[i*FWidth + j];			
			PixelValue = abs(PixelValue - FMinSignal) / abs(FMaxSignal - FMinSignal);
			
			vec4 FColor = (mColormap.*pFunction_GetColor)((float)PixelValue);

			DestPtr[j*DestBPP + 0] = (byte)(255 * FColor.b);		// синий цвет
			DestPtr[j*DestBPP + 1] = (byte)(255 * FColor.g);		// зеленый цвет
			DestPtr[j*DestBPP + 2] = (byte)(255 * FColor.r);		// красный цвет
		}

		DestPtr+= DestLineScan;
	}

	// загрузить изображение в компонент CRichImage
	mRichImage.LoadFromBitmap(InBitmap);

	return true;	
}

