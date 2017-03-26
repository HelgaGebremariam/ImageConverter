#pragma once
#include <memory.h>
#include <iomanip>
#include "RichImage.h"
#include "RichImageColormap.h"
#include "OpenFileDialog.h"

enum TPseudoColormap
	{pcAutumn, pcBone,	pcCool, pcCopper, pcHot, pcHSV, pcJet, pcParula, pcPink, pcSpring, pcSummer, pcWinter};

template <typename T>
class CRichImageConverter
{
private:
	T* FDataPtr;
	int FWidth;
	int FHeight;
	T FMinSignal;
	T FMaxSignal;

	HBITMAP InBitmap;
	void* InBitmap_BitsPtr;

	bool CreateBitmapDIB(int AWidth, int AHeight);
	void DeleteBitmapDIB(void);

protected:
	CRichImage mRichImage;		// изображение на экране
	CRichImageColormap mColormap;

public:
	CRichImageConverter();
	virtual ~CRichImageConverter();

	BOOL Create(CWnd* pParentWnd, UINT nID, const RECT& rect, DWORD dwStyle = WS_VISIBLE | WS_CHILD);		// создать окно для mRichImage
	void DestroyWindow(void);		// удалить окно mRichImage
	void RefreshImage(void);		// перерисовать изображение mRichImage
	void SetImagePos(int x, int y, int cx, int cy, UINT nFlags);		// изменить положение и размер mRichImage

	void ClearData(void);		// освободить память занятую массивом плотностей

	void Load(const CString& AFileName);		// загрузить изображение из графического файла
	void Load(T* pInData, int AWidth, int AHeight);		// загрузить изображение из массива плотностей
	void SetSignalMinMax(T _AMin, T _AMax);		// определить шкалу сиганала

	bool ConvertToImage(TPseudoColormap AColormap);		// преобразовать массив плотностей в изображение, использую цветовую палитру
};