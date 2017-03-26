// CRichImage.cpp : implementation file
#include "stdafx.h"
#include "math.h"
#include <algorithm> 
#include "RichImage.h"

#define TRIM_INT(AMinValue, AMaxValue, AValue)  max(AMinValue, min(AMaxValue, AValue));

// CRichImage
IMPLEMENT_DYNAMIC(CRichImage, CWnd)

// ---------------=============== реализация класса CRichImage ===============---------------
CRichImage::CRichImage()
{
	BufferDC = NULL;
	BufferBmp = NULL;	

	FScale = 1;
	FActiveShape = NULL;
	FResolution = 0;

	FShapeColor = RGB(255, 0, 0);
	FShapeType = stLine;
	FShapeWidth = 1;
	FShapeMaxCount = 50;	
	FOnShapeChanging = NULL;
	FBorderStyle = bsFixed3D;
	FBackgroundColor = GetSysColor(COLOR_WINDOW);

	RegisterWindowClass();
}

CRichImage::~CRichImage()
{
  // очистить список графических объектов
	for (int i=0; i < ShapesList.GetCount(); i++)
	{
		CRichImageShape* ObjectPtr = (CRichImageShape*)ShapesList[i];

		if (ObjectPtr!= NULL)
			delete ObjectPtr; 
	}

	ShapesList.RemoveAll();

	if (BufferDC != NULL)
		DeleteDC(BufferDC);

	if (BufferBmp != NULL)
		DeleteObject(BufferBmp);		
}

BOOL CRichImage::Create(CWnd* pParentWnd, UINT nID, const RECT& rect, DWORD dwStyle)
{
  return CWnd::Create(RICHIMAGE_WINDOWCLASSNAME, _T(""), dwStyle, rect, pParentWnd, nID);
}

// Register the window class if it has not already been registered.
BOOL CRichImage::RegisterWindowClass()
{
  WNDCLASS wndcls;
  HINSTANCE hInst = AfxGetInstanceHandle();

  if (!(::GetClassInfo(hInst, RICHIMAGE_WINDOWCLASSNAME, &wndcls)))
  {
    // otherwise we need to register a new class
    wndcls.style            = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
    wndcls.lpfnWndProc      = ::DefWindowProc;
    wndcls.cbClsExtra       = wndcls.cbWndExtra = 0;
    wndcls.hInstance        = hInst;
    wndcls.hIcon            = NULL;
    wndcls.hCursor          = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
    wndcls.hbrBackground    = (HBRUSH) (COLOR_3DFACE + 1);
    wndcls.lpszMenuName     = NULL;
    wndcls.lpszClassName    = RICHIMAGE_WINDOWCLASSNAME;

    if (!AfxRegisterClass(&wndcls))
    {
      AfxThrowResourceException();
      return FALSE;
    }
  }

  return TRUE;
}

// CCameraView message handlers
BEGIN_MESSAGE_MAP(CRichImage, CWnd)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

BOOL CRichImage::OnEraseBkgnd(CDC* pDC)
{
  return TRUE;
	
  // Obviously we don't have a bitmap - let the base class deal with it.
  //return CWnd::OnEraseBkgnd(pDC);
}

void CRichImage::OnPaint()
{
	// получить контекст устройства для рисования
	CPaintDC dc(this);	
		
	// 1. Вывести на экран изображение (Bitmap)
	if ((BufferDC != NULL) && (BufferBmp != NULL))
  {
		// установить режим копирования изображения
		SetStretchBltMode(dc, COLORONCOLOR); 

		// скопировать изображение из буфера на экран
		StretchBlt(dc, FImageOutRect.left, FImageOutRect.top, FImageOutRect.Width(), FImageOutRect.Height(), 
               BufferDC, FBufferBmpRect.left, FBufferBmpRect.top, FBufferBmpRect.Width(), FBufferBmpRect.Height(), 
               SRCCOPY);
  }

	// 2. Нарисовать векторные объекты из списка ShapesList
	for (int i=0; i < ShapesList.GetCount(); i++)
	{
		CRichImageShape* ObjectPtr = (CRichImageShape*)ShapesList[i];

		if (ObjectPtr!= NULL)
			ObjectPtr->Paint(dc, true);
	}

	// 3. Закрасить фон, где нет изображения
	DrawBackground(&dc);

	// 4. Нарисовать границу
	DrawFrame(&dc);

	// Do not call CWnd::OnPaint() for painting messages
}

void CRichImage::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	OnScaleUpdate();
}

void CRichImage::OnLButtonDown(UINT nFlags, CPoint point)
{	
	CWnd::OnLButtonDown(nFlags, point);

	if (FActiveShape == NULL) 
	{
		// создать фигуру требуемого типа
		switch (ShapeType)
		{
			case stLine:
				FActiveShape = new CRIShapeLine(this);
				break;

			case stRectangle:
				FActiveShape = new CRIShapeRectangle(this);
				break;

			case stRoundRect:
				FActiveShape = new CRIShapeRoundRect(this);
				break;

			case stCircle:
				FActiveShape = new CRIShapeCircle(this);
				break;

			default:
				FActiveShape = NULL;
  	}
		
		// если превысили допустимое кол-во объектов - удалить объект 0
		if (ShapesList.GetCount() >= ShapeMaxCount)
		{
			delete (CRichImageShape*)ShapesList[0];
			ShapesList.RemoveAt(0);			
			
			this->Invalidate();
		}

		// добавить объект в список, для последующей отрисовки на окне
		ShapesList.Add(FActiveShape);

		// сигнализировать о начале вставки
		if (FActiveShape != NULL)	
			FActiveShape->StartInsert(point, ShapeWidth, FShapeColor);
	}
}

void CRichImage::OnLButtonUp(UINT nFlags, CPoint point)
{
	CWnd::OnLButtonUp(nFlags, point);
		
  if (FActiveShape != NULL) 
  {
		// сигнализировать о завершении вставки
		FActiveShape->FinishInsert(point);
		
		// вызвать внешний обработчик 
		if (FOnShapeChangingObj && FOnShapeChanging)
		{
			CSize ShapeSize;
			FActiveShape->GetSize(ShapeSize);

      (FOnShapeChangingObj->*FOnShapeChanging)( ShapeSize );
		}

		FActiveShape = NULL;
	}

}

void CRichImage::OnMouseMove(UINT nFlags, CPoint point)
{	
	// сигнализировать о изменении параметров объекта
  if (FActiveShape != NULL)
	{
		FActiveShape->UpdateInsert(point);

		// вызвать внешний обработчик 
		if (FOnShapeChangingObj && FOnShapeChanging)
		{
			CSize ShapeSize;
			FActiveShape->GetSize(ShapeSize);

      (FOnShapeChangingObj->*FOnShapeChanging)( ShapeSize );
		}
	}
	
	CWnd::OnMouseMove(nFlags, nFlags);
}

void CRichImage::LoadFromBitmap(HBITMAP hSourceBmp)
{
	if (hSourceBmp == NULL)
		return;

	// получить параметры входного изображения
	BITMAP SourceBitmapInfo;
	ASSERT( GetObject(hSourceBmp, sizeof(BITMAP), &SourceBitmapInfo) != 0 );

	// создать контекст входного изображения
	CDC SourceDC;
	SourceDC.CreateCompatibleDC(NULL);

	// установить новый Bitmap у созданного контекста
	HBITMAP OldSourceBitmap = (HBITMAP)SelectObject(SourceDC, hSourceBmp);

	// проверить необходимо ли создавать новый контекст и изображение
	if (BufferDC == NULL)
	{
		BufferDC = ::CreateCompatibleDC(NULL);
		BufferBmp = ::CreateCompatibleBitmap(SourceDC, SourceBitmapInfo.bmWidth, SourceBitmapInfo.bmHeight); 

	  // установить новый Bitmap у созданного контекста
	  HBITMAP OldDestBitmap = (HBITMAP)SelectObject(BufferDC, BufferBmp);
		::DeleteObject(OldDestBitmap);

	  // считать параметры буферного изображения
	  ASSERT( GetObject(BufferBmp, sizeof(BITMAP), &BufferInfo) != 0 );

	  OnScaleUpdate();
	}
	else
	{
		// сверить параметры входного Bitmap и буферного
		if ((SourceBitmapInfo.bmType != BufferInfo.bmType) ||
			  (SourceBitmapInfo.bmWidth != BufferInfo.bmWidth) ||
				(SourceBitmapInfo.bmHeight != BufferInfo.bmHeight) ||
				(SourceBitmapInfo.bmBitsPixel != BufferInfo.bmBitsPixel))
		{
			HBITMAP OldDestBitmap = BufferBmp;
			BufferBmp = ::CreateCompatibleBitmap(SourceDC, SourceBitmapInfo.bmWidth, SourceBitmapInfo.bmHeight); 
			
			SelectObject(BufferDC, BufferBmp);		
		  ::DeleteObject(OldDestBitmap);
  	
			// считать параметры буферного изображения
	    ASSERT( GetObject(BufferBmp, sizeof(BITMAP), &BufferInfo) != 0 );

	    OnScaleUpdate();
		}
	}

	// скопировать исходный Bitmap на заданный
	BitBlt(BufferDC, 0, 0, SourceBitmapInfo.bmWidth, SourceBitmapInfo.bmHeight, SourceDC, 0, 0, SRCCOPY);

	// очистить память
	SelectObject(SourceDC, OldSourceBitmap);
	DeleteDC(SourceDC);
}

bool CRichImage::LoadFromFile(LPCWSTR AFileName)
{
  HBITMAP hBitmap = (HBITMAP)::LoadImage(0, (LPCSTR)AFileName, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

	if (hBitmap != NULL)
	{
	  LoadFromBitmap(hBitmap);		
		return true;
	}
	else
		return false;
}

void CRichImage::Repaint(HBITMAP ABitmap)
{
	LoadFromBitmap(ABitmap);

	Invalidate(false);	
}

void CRichImage::BitmapToClient(CPoint& ASourcePoint, CPoint& ADestPoint, bool doFloor)
{
	ADestPoint.x = TRIM_INT(FBufferBmpRect.left, FBufferBmpRect.right-1, ASourcePoint.x); 
	ADestPoint.y = TRIM_INT(FBufferBmpRect.top, FBufferBmpRect.bottom-1, ASourcePoint.y);

	if (doFloor)
	{
	  ADestPoint.x = (int)floor( (double)ADestPoint.x * FScale ) - FClientRect.left;
	  ADestPoint.y = (int)floor( (double)ADestPoint.y * FScale ) - FClientRect.top;
	}
	else
	{
	  ADestPoint.x = (int)ceil( (double)ADestPoint.x * FScale ) - FClientRect.left;
	  ADestPoint.y = (int)ceil( (double)ADestPoint.y * FScale ) - FClientRect.top;	  
	}
}

void CRichImage::ClientToBitmap(CPoint& ASourcePoint, CPoint& ADestPoint, bool doFloor)
{
	if (doFloor)
	{
	  ADestPoint.x = (int)floor( (double)(ASourcePoint.x - FClientRect.left) / FScale );
	  ADestPoint.y = (int)floor( (double)(ASourcePoint.y - FClientRect.top) / FScale );
	}
	else 
	{
	  ADestPoint.x = (int)ceil( (double)(ASourcePoint.x - FClientRect.left) / FScale );
	  ADestPoint.y = (int)ceil( (double)(ASourcePoint.y - FClientRect.top) / FScale );
	}

  ADestPoint.x = TRIM_INT(FBufferBmpRect.left, FBufferBmpRect.right-1, ADestPoint.x); 
	ADestPoint.y = TRIM_INT(FBufferBmpRect.top, FBufferBmpRect.bottom-1, ADestPoint.y);
}

void CRichImage::SetOnShapeChanging(CObject* AOwner, TRIShapeChanging AFunc)
{
	FOnShapeChangingObj = AOwner ;
	FOnShapeChanging = AFunc;
}

// пересчитать коэффициент преобразования (изображение <-> экран)
void CRichImage::OnScaleUpdate(void)
{
	// получить размеры всего окна
	this->GetClientRect(FWindowRect);

	// определить размеры клиентской области - окно за вычетом рамки
	FClientRect.CopyRect(FWindowRect);
	switch (FBorderStyle)
	{
    case bsNone: 
		  break;
    
		case bsFixedSingle:
			FClientRect.InflateRect(-1, -1);
			break;

	  case bsFixedFlat:
			FClientRect.InflateRect(-1, -1);
			break;

		case bsFixed3D:
			FClientRect.InflateRect(-2, -2);
			break;
	}


	// получить параметры буфера изображения
	FBufferBmpRect.SetRect(0, 0, BufferInfo.bmWidth, BufferInfo.bmHeight);

	// определить коэффициент масштабирования изображения в клиентское окно
	FScale = min((float)FClientRect.Width() / FBufferBmpRect.Width(), (float)FClientRect.Height() / FBufferBmpRect.Height());
	
	// установить параметры области вывода изображения
	FImageOutRect.left = FClientRect.left;
	FImageOutRect.top = FClientRect.top;
	FImageOutRect.right = FClientRect.left + (int)floor( FScale * FBufferBmpRect.Width() + 0.5);
	FImageOutRect.bottom = FClientRect.top + (int)floor( FScale * FBufferBmpRect.Height() + 0.5);
	
	FImageOutRect.right = TRIM_INT(FClientRect.left, FClientRect.right, FImageOutRect.right);
	FImageOutRect.bottom = TRIM_INT(FClientRect.top, FClientRect.bottom, FImageOutRect.bottom);
}

// нарисовать рамку заданную в FBorderStyle
void CRichImage::DrawFrame(CDC* pDC)
{
	if (FBorderStyle == bsFixedSingle)
	{
		CPen LocalPen;
    LocalPen.CreatePen(PS_SOLID, 1, RGB(0xA0, 0xA0, 0xA0));

		CBrush LocalBrush;
		LocalBrush.CreateStockObject(NULL_BRUSH);

		CPen* pSavedPen = pDC->SelectObject(&LocalPen); 
		CBrush* pSavedBrush = pDC->SelectObject(&LocalBrush); 

		pDC->Rectangle(FWindowRect);

		// восстановить старые параметры контекста
    pDC->SelectObject(pSavedBrush);
    pDC->SelectObject(pSavedPen);

		LocalPen.DeleteObject();
		LocalBrush.DeleteObject();

		return;
	}

	if (BorderStyle == bsFixedFlat)
	{
		// Верхняя, внешняя граница
		CPen LocalPen;
    LocalPen.CreatePen(PS_SOLID, 1, RGB(0x80, 0x80, 0x80));
		CPen* pSavedPen = pDC->SelectObject(&LocalPen); 

		pDC->MoveTo(FWindowRect.right-1, FWindowRect.top);
    pDC->LineTo(FWindowRect.left, FWindowRect.top);
    pDC->LineTo(FWindowRect.left, FWindowRect.bottom-1);

		// Нижняя, внешняя граница
		LocalPen.DeleteObject();
    LocalPen.CreatePen(PS_SOLID, 1, RGB(0xFF, 0xFF, 0xFF));
		pDC->SelectObject(&LocalPen); 
		
    pDC->MoveTo(FWindowRect.left, FWindowRect.bottom-1);
    pDC->LineTo(FWindowRect.right-1, FWindowRect.bottom-1);
    pDC->LineTo(FWindowRect.right-1, FWindowRect.top);
		
		// восстановить старые параметры контекста
    pDC->SelectObject(pSavedPen);
		LocalPen.DeleteObject();	

	  return;
	}

	if (BorderStyle == bsFixed3D)
	{
		// Верхняя, внешняя граница
		CPen LocalPen;
    LocalPen.CreatePen(PS_SOLID, 1, RGB(0x00, 0x00, 0x00));
		CPen* pSavedPen = pDC->SelectObject(&LocalPen); 

    pDC->MoveTo(FWindowRect.left, FWindowRect.bottom-1);
    pDC->LineTo(FWindowRect.left, FWindowRect.top);
    pDC->LineTo(FWindowRect.right, FWindowRect.top);
		
		// Верхняя, внутреняя граница
		if ((FWindowRect.Width() >= 4) && (FWindowRect.Height() >= 4))
		{
		  LocalPen.DeleteObject();
      LocalPen.CreatePen(PS_SOLID, 1, RGB(0x80, 0x80, 0x80));
		  pDC->SelectObject(&LocalPen); 
		
      pDC->MoveTo(FWindowRect.left+1, FWindowRect.bottom-2);
      pDC->LineTo(FWindowRect.left+1, FWindowRect.top+1);
      pDC->LineTo(FWindowRect.right-1, FWindowRect.top+1);
		}

		// Нижняя, внутреняя граница
		if ((FWindowRect.Width() >= 4) && (FWindowRect.Height() >= 4))
		{
		  LocalPen.DeleteObject();
      LocalPen.CreatePen(PS_SOLID, 1, RGB(0xF4, 0xF4, 0xF4));
		  pDC->SelectObject(&LocalPen); 

      pDC->MoveTo(FWindowRect.left+1, FWindowRect.bottom-2);
      pDC->LineTo(FWindowRect.right-2, FWindowRect.bottom-2);
      pDC->LineTo(FWindowRect.right-2, FWindowRect.top+1);
		}
			
		// Нижняя, внешняя граница
		LocalPen.DeleteObject();
    LocalPen.CreatePen(PS_SOLID, 1, RGB(0xFF, 0xFF, 0xFF));
		pDC->SelectObject(&LocalPen); 

    pDC->MoveTo(FWindowRect.left, FWindowRect.bottom-1);
    pDC->LineTo(FWindowRect.right-1, FWindowRect.bottom-1);
    pDC->LineTo(FWindowRect.right-1, FWindowRect.top);		
	
	
		// восстановить старые параметры контекста
    pDC->SelectObject(pSavedPen);
		LocalPen.DeleteObject();	

	  return;
	}
}

// закрасить фон
void CRichImage::DrawBackground(CDC* pDC)
{
	CBrush* pBrush = NULL;
	
	// установить параметры кисти
	if ((FImageOutRect.left > FClientRect.left) ||
		  (FImageOutRect.top > FClientRect.top) ||
			(FImageOutRect.right < FClientRect.right) ||
			(FImageOutRect.bottom < FClientRect.bottom))
		pBrush = new CBrush( FBackgroundColor );

	// левая от изображения часть фона
	if (FImageOutRect.left > FClientRect.left)
		pDC->FillRect(CRect(FClientRect.left, FClientRect.top, FImageOutRect.left, FClientRect.bottom), pBrush);

	// верхняя от изображения часть фона
	if (FImageOutRect.top > FClientRect.top)
		pDC->FillRect(CRect(FClientRect.left, FClientRect.top, FClientRect.right, FImageOutRect.top), pBrush);

	// правая от изображения часть фона
	if (FImageOutRect.right < FClientRect.right)
		pDC->FillRect(CRect(FImageOutRect.right, FClientRect.top, FClientRect.right, FClientRect.bottom), pBrush);

	// нижняя от изображения часть фона
	if (FImageOutRect.bottom < FClientRect.bottom)
		pDC->FillRect(CRect(FClientRect.left, FImageOutRect.bottom, FClientRect.right, FClientRect.bottom), pBrush);


	// восстановить старые параметры контекста
	if (pBrush)
	  delete pBrush;	
}

TRIShapeType CRichImage::GetShapeType(void)
{
  return FShapeType;
}

void CRichImage::SetShapeType(TRIShapeType AValue)
{
	FShapeType = AValue;
}

COLORREF CRichImage::GetShapeColor(void)
{
	return FShapeColor;
}

void CRichImage::SetShapeColor(COLORREF AValue)
{
  FShapeColor = AValue;
}

int CRichImage::GetShapeWidth(void)
{
	return FShapeWidth;
}

void CRichImage::SetShapeWidth(int AValue)
{
	FShapeWidth = AValue;
}

int CRichImage::GetShapeMaxCount(void)
{
	return FShapeMaxCount;
}

void CRichImage::SetShapeMaxCount(int AValue)
{
	FShapeMaxCount = min(100, max(1, AValue));
}

TRIBorderStyle CRichImage::GetBorderStyle(void)
{
  return FBorderStyle;
}

void CRichImage::SetBorderStyle(TRIBorderStyle AValue)
{
  FBorderStyle = FBorderStyle;
	OnScaleUpdate();
}

COLORREF CRichImage::GetBackgroundColor(void)
{
  return FBackgroundColor;
}

void CRichImage::SetBackgroundColor(COLORREF AValue)
{
  FBackgroundColor = AValue; 
}

double CRichImage::GetResolution(void)
{
	return FResolution;
}

void CRichImage::SetResolution(double AValue)
{
	if (AValue <= 0)
	  FResolution = 1;
	else
		FResolution = AValue;
}
