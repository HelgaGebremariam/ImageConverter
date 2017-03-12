#include "stdafx.h"
#include "RichImageShape.h"
#include "RichImage.h"

// ---------------=============== реализация класса CRichImageShape ===============---------------
CRichImageShape::CRichImageShape()
{
	FIsInserting = false;
	FPenWidth = 1;
	FPenColor = RGB(128, 128, 128);
};

CRichImageShape::CRichImageShape(CWnd* AOwnerPtr)
{
	FOwnerPtr = AOwnerPtr;
}

CRichImageShape::~CRichImageShape(void)
{
};

void CRichImageShape::BitmapToClient(CPoint& ASourcePoint, CPoint& ADestPoint, bool doFloor)
{
	((CRichImage*)FOwnerPtr)->BitmapToClient(ASourcePoint, ADestPoint, doFloor);
}

void CRichImageShape::ClientToBitmap(CPoint& ASourcePoint, CPoint& ADestPoint, bool doFloor)
{
	((CRichImage*)FOwnerPtr)->ClientToBitmap(ASourcePoint, ADestPoint, doFloor);
}

void CRichImageShape::StartInsert(CPoint point, int APenWidth, COLORREF APenColor)
{	
	FIsInserting = true;

	ClientToBitmap(point, FStartPoint, true);
	ClientToBitmap(point, FFinishPoint, true);
	ClientToBitmap(point, FPreviousPoint, true);

	FPenWidth = APenWidth;
	FPenColor = APenColor;
}

void CRichImageShape::UpdateInsert(CPoint point)
{
  // запомнить конечную точку
	ClientToBitmap(point, FFinishPoint, true);

	// нарисовать объект
	HDC hDC = ::GetDC(FOwnerPtr->m_hWnd);
	Paint(hDC, false);
	::ReleaseDC(FOwnerPtr->m_hWnd, hDC);

	// запомнить предыдущую точку
	ClientToBitmap(point, FPreviousPoint, true);
}

void CRichImageShape::FinishInsert(CPoint point)
{
  FIsInserting = false;

  // запомнить конечную точку
	ClientToBitmap(point, FFinishPoint, true);

	// нарисовать объект
	HDC hDC = ::GetDC(FOwnerPtr->m_hWnd);
	Paint(hDC, false);
	::ReleaseDC(FOwnerPtr->m_hWnd, hDC);
}

void CRichImageShape::Paint(HDC hDC, bool ABackgroundErased)
{
  //
}

// получить размер объекта
void CRichImageShape::GetSize(CSize& ASize)
{
	ASize.cx = abs(FFinishPoint.x - FStartPoint.x);
	ASize.cy = abs(FFinishPoint.y - FStartPoint.y);
}


// ---------------=============== реализация класса CRIShapeLine ===============---------------
void CRIShapeLine::Paint(HDC hDC, bool ABackgroundErased)
{
	CPoint DrawPoint1, DrawPoint2;
	
	// режим рисования объекта
	if (!FIsInserting)
	{	
		// установить новые параметры контекста
		HPEN FPen = ::CreatePen(PS_SOLID, FPenWidth, FPenColor);
		HPEN FSavedPen = (HPEN)::SelectObject(hDC, FPen);

		// нарисовать линию
		BitmapToClient(FStartPoint, DrawPoint1, true);
		BitmapToClient(FFinishPoint, DrawPoint2, true);
		
    ::MoveToEx(hDC, DrawPoint1.x, DrawPoint1.y, NULL);
    ::LineTo(hDC, DrawPoint2.x, DrawPoint2.y);		

		// восстановить старые параметры контекста		
		::SelectObject(hDC, FSavedPen);	
		::DeleteObject(FPen);
	}

	// режим вставки объекта
	else
	{
		// установить новые параметры контекста
		int FSavedROP2 = ::GetROP2(hDC);
		::SetROP2(hDC, R2_NOTXORPEN);
		HPEN FPen = ::CreatePen(PS_DASH, 1, RGB(45, 45, 45));
		HPEN FSavedPen = (HPEN)::SelectObject(hDC, FPen);		

		// удалить старую линию
		BitmapToClient(FStartPoint, DrawPoint1, true);
		BitmapToClient(FPreviousPoint, DrawPoint2, true);
		
		if (!ABackgroundErased)
		{
      ::MoveToEx(hDC, DrawPoint1.x, DrawPoint1.y, NULL);
      ::LineTo(hDC, DrawPoint2.x, DrawPoint2.y);
		}

		// нарисовать новую линию		
		BitmapToClient(FFinishPoint, DrawPoint2, true);

    ::MoveToEx(hDC, DrawPoint1.x, DrawPoint1.y, NULL);
    ::LineTo(hDC, DrawPoint2.x, DrawPoint2.y);		

		// восстановить старые параметры контекста
		::SetROP2(hDC, FSavedROP2);		
		::SelectObject(hDC, FSavedPen);	
		::DeleteObject(FPen);	
	}
}


// ---------------=============== реализация класса CRIShapeRectangle ===============---------------
void CRIShapeRectangle::Paint(HDC hDC, bool ABackgroundErased)
{
	CPoint DrawPoint1, DrawPoint2;

	// режим рисования объекта
	if (!FIsInserting)
	{
		// установить новые параметры контекста
		HPEN FPen = ::CreatePen(PS_SOLID, FPenWidth, FPenColor);
		HPEN FSavedPen = (HPEN)::SelectObject(hDC, FPen);
		HBRUSH FSavedBrush = (HBRUSH)::SelectObject(hDC, GetStockObject( NULL_BRUSH ));

		// нарисовать прямоугольник 
		BitmapToClient(FStartPoint, DrawPoint1, true);
		BitmapToClient(FFinishPoint, DrawPoint2, true);
		
		::Rectangle(hDC, DrawPoint1.x, DrawPoint1.y, DrawPoint2.x, DrawPoint2.y);

		// восстановить старые параметры контекста
		::SelectObject(hDC, FSavedBrush);	
		::SelectObject(hDC, FSavedPen);	
		::DeleteObject(FPen);
	}

	// режим вставки объекта
	else
	{
		// установить новые параметры контекста
		int FSavedROP2 = ::GetROP2(hDC);
		::SetROP2(hDC, R2_NOTXORPEN);
		HPEN FPen = ::CreatePen(PS_DASH, 1, RGB(45, 45, 45));
		HPEN FSavedPen = (HPEN)::SelectObject(hDC, FPen);
		HBRUSH FSavedBrush = (HBRUSH)::SelectObject(hDC, GetStockObject( NULL_BRUSH ));

		// удалить старый прямоугольник
		BitmapToClient(FStartPoint, DrawPoint1, true);
		BitmapToClient(FPreviousPoint, DrawPoint2, true);
		
		if (!ABackgroundErased)		
		  ::Rectangle(hDC, DrawPoint1.x, DrawPoint1.y, DrawPoint2.x, DrawPoint2.y);

		// нарисовать новый прямоугольник
		BitmapToClient(FFinishPoint, DrawPoint2, true);

		::Rectangle(hDC, DrawPoint1.x, DrawPoint1.y, DrawPoint2.x, DrawPoint2.y);

		// восстановить старые параметры контекста
		::SetROP2(hDC, FSavedROP2);
		::SelectObject(hDC, FSavedBrush);	
		::SelectObject(hDC, FSavedPen);	
		::DeleteObject(FPen);	
	}
}


// ---------------=============== реализация класса CRIShapeRoundRect ===============---------------
int CRIShapeRoundRect::GetEllipseWidth(CPoint& AStartPoint, CPoint& AFinishPoint)
{
	int iWidth = abs(AFinishPoint.x - AStartPoint.x);
	int iHeight = abs(AFinishPoint.y - AStartPoint.y);

	int Result = min(iWidth, iHeight) / 3;

	return max(Result, 4);
}

int CRIShapeRoundRect::GetEllipseHeight(CPoint& AStartPoint, CPoint& AFinishPoint)
{
	int iWidth = abs(AFinishPoint.x - AStartPoint.x);
	int iHeight = abs(AFinishPoint.y - AStartPoint.y);

	int Result = min(iWidth, iHeight) / 3;

	return max(Result, 4);
}

void CRIShapeRoundRect::Paint(HDC hDC, bool ABackgroundErased)
{
	CPoint DrawPoint1, DrawPoint2;

	// режим рисования объекта
	if (!FIsInserting)
	{
		// установить новые параметры контекста
		HPEN FPen = ::CreatePen(PS_SOLID, FPenWidth, FPenColor);
		HPEN FSavedPen = (HPEN)::SelectObject(hDC, FPen);
		HBRUSH FSavedBrush = (HBRUSH)::SelectObject(hDC, GetStockObject( NULL_BRUSH ));

		// нарисовать закругленный прямоугольник 
		BitmapToClient(FStartPoint, DrawPoint1, true);
		BitmapToClient(FFinishPoint, DrawPoint2, true);
		
		::RoundRect(hDC, DrawPoint1.x, DrawPoint1.y, 
			               DrawPoint2.x, DrawPoint2.y, 
										 GetEllipseWidth(DrawPoint1, DrawPoint2),
										 GetEllipseHeight(DrawPoint1, DrawPoint2) );

		// восстановить старые параметры контекста
		::SelectObject(hDC, FSavedBrush);	
		::SelectObject(hDC, FSavedPen);	
		::DeleteObject(FPen);
	}

	// режим вставки объекта
	else
	{
		// установить новые параметры контекста
		int FSavedROP2 = ::GetROP2(hDC);
		::SetROP2(hDC, R2_NOTXORPEN);
		HPEN FPen = ::CreatePen(PS_DASH, 1, RGB(45, 45, 45));
		HPEN FSavedPen = (HPEN)::SelectObject(hDC, FPen);
		HBRUSH FSavedBrush = (HBRUSH)::SelectObject(hDC, GetStockObject( NULL_BRUSH ));

		// удалить старый закругленный прямоугольник
		BitmapToClient(FStartPoint, DrawPoint1, true);
		BitmapToClient(FPreviousPoint, DrawPoint2, true);

		if (!ABackgroundErased)
		  ::RoundRect(hDC, DrawPoint1.x, DrawPoint1.y, 
			                 DrawPoint2.x, DrawPoint2.y, 
				  						 GetEllipseWidth(DrawPoint1, DrawPoint2),
					  					 GetEllipseHeight(DrawPoint1, DrawPoint2) );

		// нарисовать новый закругленный прямоугольник
		BitmapToClient(FFinishPoint, DrawPoint2, true);

		::RoundRect(hDC, DrawPoint1.x, DrawPoint1.y, 
			               DrawPoint2.x, DrawPoint2.y, 
										 GetEllipseWidth(DrawPoint1, DrawPoint2),
										 GetEllipseHeight(DrawPoint1, DrawPoint2) );

		// восстановить старые параметры контекста
		::SetROP2(hDC, FSavedROP2);
		::SelectObject(hDC, FSavedBrush);	
		::SelectObject(hDC, FSavedPen);	
		::DeleteObject(FPen);	
	}
}


// ---------------=============== реализация класса CRIShapeCircle ===============---------------
int CRIShapeCircle::GetCircleWidth(CPoint& AStartPoint, CPoint& AFinishPoint)
{
	int iWidth = AFinishPoint.x - AStartPoint.x;
	int iHeight = AFinishPoint.y - AStartPoint.y;

  int Result = max( abs(iWidth), abs(iHeight));
	
	return (iWidth < 0) ? -max(Result, 3) : max(Result, 3);
}

int CRIShapeCircle::GetCircleHeight(CPoint& AStartPoint, CPoint& AFinishPoint)
{
	int iWidth = AFinishPoint.x - AStartPoint.x;
	int iHeight = AFinishPoint.y - AStartPoint.y;

  int Result = max( abs(iWidth), abs(iHeight));
	
	return (iHeight < 0) ? -max(Result, 3) : max(Result, 3);
}

void CRIShapeCircle::Paint(HDC hDC, bool ABackgroundErased)
{
	CPoint DrawPoint1, DrawPoint2;

	// режим рисования объекта
	if (!FIsInserting)
	{
		// установить новые параметры контекста
		HPEN FPen = ::CreatePen(PS_SOLID, FPenWidth, FPenColor);
		HPEN FSavedPen = (HPEN)::SelectObject(hDC, FPen);
		HBRUSH FSavedBrush = (HBRUSH)::SelectObject(hDC, GetStockObject( NULL_BRUSH ));

		// нарисовать круг
		BitmapToClient(FStartPoint, DrawPoint1, true);
		BitmapToClient(FFinishPoint, DrawPoint2, true);

		::Ellipse(hDC, 
			        DrawPoint1.x, 
							DrawPoint1.y, 
			        DrawPoint1.x + GetCircleWidth(DrawPoint1, DrawPoint2),
							DrawPoint1.y + GetCircleHeight(DrawPoint1, DrawPoint2) );

		// восстановить старые параметры контекста
		::SelectObject(hDC, FSavedBrush);	
		::SelectObject(hDC, FSavedPen);	
		::DeleteObject(FPen);
	}

	// режим вставки объекта
	else
	{
		// установить новые параметры контекста
		int FSavedROP2 = ::GetROP2(hDC);
		::SetROP2(hDC, R2_NOTXORPEN);
		HPEN FPen = ::CreatePen(PS_DASH, 1, RGB(45, 45, 45));
		HPEN FSavedPen = (HPEN)::SelectObject(hDC, FPen);
		HBRUSH FSavedBrush = (HBRUSH)::SelectObject(hDC, GetStockObject( NULL_BRUSH ));

		// удалить старый круг
		BitmapToClient(FStartPoint, DrawPoint1, true);
		BitmapToClient(FPreviousPoint, DrawPoint2, true);

		if (!ABackgroundErased)
		  ::Ellipse(hDC, 
			          DrawPoint1.x, 
				  			DrawPoint1.y, 
			          DrawPoint1.x + GetCircleWidth(DrawPoint1, DrawPoint2),
						  	DrawPoint1.y + GetCircleHeight(DrawPoint1, DrawPoint2) );

		// нарисовать новый круг
		BitmapToClient(FFinishPoint, DrawPoint2, true);

		::Ellipse(hDC, 
			        DrawPoint1.x, 
							DrawPoint1.y, 
			        DrawPoint1.x + GetCircleWidth(DrawPoint1, DrawPoint2),
							DrawPoint1.y + GetCircleHeight(DrawPoint1, DrawPoint2) );

		// восстановить старые параметры контекста
		::SetROP2(hDC, FSavedROP2);
		::SelectObject(hDC, FSavedBrush);	
		::SelectObject(hDC, FSavedPen);	
		::DeleteObject(FPen);	
	}
}

