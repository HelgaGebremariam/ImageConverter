#include "Windows.h"
#include "RichImageShape.h"

#if !defined(AFX_RICHIMAGE_H__BF97F412_279F_11D4_A2F6_0048543D92F7__INCLUDED_)
  #define AFX_RICHIMAGE_H__BF97F412_279F_11D4_A2F6_0048543D92F7__INCLUDED_
#endif

#pragma once

#define RICHIMAGE_WINDOWCLASSNAME  _T("CRichImageMFCCtrl")  // Window class name

enum TRIShapeType {stLine, stRectangle, stRoundRect, stCircle};
enum TRIBorderStyle {bsNone, bsFixedSingle, bsFixedFlat, bsFixed3D};

// функция обратного вызова при рисовании фигур: линия, прямоугольник ...
typedef void (CObject::*TRIShapeChanging)(CSize& ASize);

// ---------------=============== описание класса CRichImage ===============---------------
class CRichImage : public CWnd
{
	DECLARE_DYNAMIC(CRichImage)

private:
  TRIShapeType FShapeType;
	COLORREF FShapeColor;	
	int FShapeWidth;  
	int FShapeMaxCount;
	CRichImageShape* FActiveShape;
	CObArray ShapesList;
	CObject* FOnShapeChangingObj;
	TRIShapeChanging FOnShapeChanging;
	double FResolution;		// разрешение, пикселей/мм
	
	TRIBorderStyle FBorderStyle;
	COLORREF FBackgroundColor;

protected:
	DECLARE_MESSAGE_MAP()

	HDC BufferDC;  // контекст буферного изображения
	BITMAP BufferInfo;  // параметры буферного изображения
	HBITMAP BufferBmp;  // буферное изображение	

  CRect FWindowRect;  // область всего окна
	CRect FClientRect;  // клиентская область (область окна без учета рамки)
	CRect FImageOutRect;  // область вывода изображения
	CRect FBufferBmpRect;  // параметры буфера изображения в координатах CRect
	double FScale;  // коэффициент масштабирования изображения в клиентское окно
	
	BOOL RegisterWindowClass();
	void OnScaleUpdate(void);  // пересчитать коэффициент преобразования (изображение <-> экран)
	void DrawFrame(CDC* pDC);  // нарисовать рамку заданную в FBorderStyle
	void DrawBackground(CDC* pDC);  // закрасить фон

public:
	CRichImage();
	virtual ~CRichImage();

	BOOL Create(CWnd* pParentWnd, UINT nID, const RECT& rect, DWORD dwStyle = WS_VISIBLE | WS_CHILD);

public:
  // тип фигуры графического объекта по умолчанию
	__declspec(property(get = GetShapeType, put = SetShapeType)) TRIShapeType ShapeType;
	TRIShapeType GetShapeType(void);
	void SetShapeType(TRIShapeType AValue);	

  // цвет графических объектов по умолчанию
	__declspec(property(get = GetShapeColor, put = SetShapeColor)) COLORREF ShapeColor;
	COLORREF GetShapeColor(void);
	void SetShapeColor(COLORREF AValue);	

	// толщина линии графического объекта
	__declspec(property(get = GetShapeWidth, put = SetShapeWidth)) int ShapeWidth;
	int GetShapeWidth(void);
	void SetShapeWidth(int AValue);	
	
	// максимально допустимое кол-во графических объектов
	__declspec(property(get = GetShapeMaxCount, put = SetShapeMaxCount)) int ShapeMaxCount;
		int GetShapeMaxCount(void);
		void SetShapeMaxCount(int AValue);	

	// тип границы компонента
	__declspec(property(get = GetBorderStyle, put = SetBorderStyle)) TRIBorderStyle BorderStyle;
	TRIBorderStyle GetBorderStyle(void);
	void SetBorderStyle(TRIBorderStyle AValue);	
	
	// фоновый цвет компонента
	__declspec(property(get = GetBackgroundColor, put = SetBackgroundColor)) COLORREF BackgroundColor;
	COLORREF GetBackgroundColor(void);
	void SetBackgroundColor(COLORREF AValue);	

	// разрешение, пикселей/мм
	__declspec(property(get = GetResolution, put = SetResolution)) double Resolution;
	double GetResolution(void);
	void SetResolution(double AValue);


	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();

	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	
	void LoadFromBitmap(HBITMAP hSourceBmp);
	bool LoadFromFile(LPCWSTR AFileName);	
  void Repaint(HBITMAP ABitmap);

	void BitmapToClient(CPoint& ASourcePoint, CPoint& ADestPoint, bool doFloor);
	void ClientToBitmap(CPoint& ASourcePoint, CPoint& ADestPoint, bool doFloor);

	void SetOnShapeChanging(CObject* AOwner, TRIShapeChanging AFunc);
};
 

