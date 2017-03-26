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
	CRichImage mRichImage;		// ����������� �� ������
	CRichImageColormap mColormap;

public:
	CRichImageConverter();
	virtual ~CRichImageConverter();

	BOOL Create(CWnd* pParentWnd, UINT nID, const RECT& rect, DWORD dwStyle = WS_VISIBLE | WS_CHILD);		// ������� ���� ��� mRichImage
	void DestroyWindow(void);		// ������� ���� mRichImage
	void RefreshImage(void);		// ������������ ����������� mRichImage
	void SetImagePos(int x, int y, int cx, int cy, UINT nFlags);		// �������� ��������� � ������ mRichImage

	void ClearData(void);		// ���������� ������ ������� �������� ����������

	void Load(const CString& AFileName);		// ��������� ����������� �� ������������ �����
	void Load(T* pInData, int AWidth, int AHeight);		// ��������� ����������� �� ������� ����������
	void SetSignalMinMax(T _AMin, T _AMax);		// ���������� ����� ��������

	bool ConvertToImage(TPseudoColormap AColormap);		// ������������� ������ ���������� � �����������, ��������� �������� �������
};