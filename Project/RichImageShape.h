#pragma once
#include "afx.h"


// ---------------=============== описание класса CRichImageShape ===============---------------
class CRichImageShape : public CObject
{
protected:
	bool FIsInserting;  // фигура в процессе добавления на изображение

	CPoint FStartPoint;
	CPoint FFinishPoint;
	CPoint FPreviousPoint;

	int FPenWidth;
	COLORREF FPenColor;

	CWnd* FOwnerPtr;

	void BitmapToClient(CPoint& ASourcePoint, CPoint& ADestPoint, bool doFloor);
	void ClientToBitmap(CPoint& ASourcePoint, CPoint& ADestPoint, bool doFloor);

public:
	CRichImageShape();	
	CRichImageShape(CWnd* AOwnerPtr);
	~CRichImageShape(void);

	void StartInsert(CPoint point, int APenWidth, COLORREF APenColor);
	void UpdateInsert(CPoint point);
	void FinishInsert(CPoint point);	

  virtual void Paint(HDC hDC, bool ABackgroundErased);
	void GetSize(CSize& ASize);  // получить размер объекта
};


// ---------------=============== описание класса CRIShapeLine(CRichImageShape) ===============---------------
class CRIShapeLine : public CRichImageShape
{
public:
	CRIShapeLine(CWnd* AOwnerPtr) : CRichImageShape(AOwnerPtr) {};

	virtual void Paint(HDC hDC, bool ABackgroundErased);
};


// ---------------=============== описание класса CRIShapeRectangle(CRichImageShape) ===============---------------
class CRIShapeRectangle : public CRichImageShape
{
public:
	CRIShapeRectangle(CWnd* AOwnerPtr) : CRichImageShape(AOwnerPtr) {};

	virtual void Paint(HDC hDC, bool ABackgroundErased);
};


// ---------------=============== описание класса CRIShapeRoundRect(CRichImageShape) ===============---------------
class CRIShapeRoundRect : public CRichImageShape
{
private:
  int GetEllipseWidth(CPoint& AStartPoint, CPoint& AFinishPoint);
	int GetEllipseHeight(CPoint& AStartPoint, CPoint& AFinishPoint);

public:
	CRIShapeRoundRect(CWnd* AOwnerPtr) : CRichImageShape(AOwnerPtr) {};

	virtual void Paint(HDC hDC, bool ABackgroundErased);
};


// ---------------=============== описание класса CRIShapeCircle(CRichImageShape) ===============---------------
class CRIShapeCircle : public CRichImageShape
{
private:
  int GetCircleWidth(CPoint& AStartPoint, CPoint& AFinishPoint);
	int GetCircleHeight(CPoint& AStartPoint, CPoint& AFinishPoint);

public:
	CRIShapeCircle(CWnd* AOwnerPtr) : CRichImageShape(AOwnerPtr) {};

	virtual void Paint(HDC hDC, bool ABackgroundErased);
};