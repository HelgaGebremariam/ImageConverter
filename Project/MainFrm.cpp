// MainFrm.cpp : implementation of the CMainFrame class
#include "stdafx.h"
#include "ImageConvertion.h"
#include "MainFrm.h"

// CMainFrame
IMPLEMENT_DYNAMIC(CMainFrame, CFrameWnd)

// CMainFrame construction/destruction
CMainFrame::CMainFrame()
{
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.lpszClass = AfxRegisterWndClass(CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW, 
		                                 AfxGetApp()->LoadStandardCursor(IDC_ARROW),
																		 (HBRUSH) (COLOR_3DFACE + 1),
																		 NULL);
	cs.x = 50;
	cs.y = 50;
	cs.cx = 1024;
	cs.cy = 800;

	if(cs.hMenu!=NULL)
	{
		::DestroyMenu(cs.hMenu);      // delete menu if loaded
		cs.hMenu = NULL;              // no menu for this window
	}

	return TRUE;
}


BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_SHOWWINDOW()
	ON_WM_CLOSE()
	ON_WM_SIZE()
	ON_CBN_SELCHANGE(ID_CB_ColorType, &CMainFrame::OnCB_ColorTypeChange)
	ON_BN_CLICKED(ID_OPENFILEBUTTON, &CMainFrame::OnBN_SelectFileClick)
END_MESSAGE_MAP()

void CMainFrame::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CWnd::OnShowWindow(bShow, bShow);

	if (bShow &&(nStatus==0))
	{
		ComboBox_ColorType.Create(WS_CHILD|WS_VISIBLE|CBS_DROPDOWNLIST, CRect(70,15,200,15+20*(1+12)), this, ID_CB_ColorType);

		ComboBox_ColorType.ResetContent();
		ComboBox_ColorType.AddString(_T("Autumn"));
		ComboBox_ColorType.AddString(_T("Bone"));
		ComboBox_ColorType.AddString(_T("Cool"));
		ComboBox_ColorType.AddString(_T("Copper"));
		ComboBox_ColorType.AddString(_T("Hot"));
		ComboBox_ColorType.AddString(_T("HSV"));
		ComboBox_ColorType.AddString(_T("Jet"));
		ComboBox_ColorType.AddString(_T("Parula"));
		ComboBox_ColorType.AddString(_T("Pink"));
		ComboBox_ColorType.AddString(_T("Spring"));
		ComboBox_ColorType.AddString(_T("Summer"));
		ComboBox_ColorType.AddString(_T("Winter"));

		ComboBox_ColorType.SetCurSel(0);

		openFileButton.Create(_T("Open file"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, CRect(140, 30, 250, 15 + 20 * (1 + 12)), this, ID_OPENFILEBUTTON);
		RichImageConverter.Create(this, ID_RichImage, CRect(10, 10, 50, 50)); 	
		//RichImageConverter.Load(_T("Image.bmp"));
		RichImageConverter.ConvertToImage(pcAutumn);
		RichImageConverter.RefreshImage();
		OnResizeComponents();
	}
}

void CMainFrame::OnClose()
{
	RichImageConverter.DestroyWindow();

	CWnd::OnClose();
}

void CMainFrame::OnSize(UINT nType, int cx, int cy)
{
	CFrameWnd::OnSize(nType, cx, cy);

	OnResizeComponents();
}

void CMainFrame::OnCB_ColorTypeChange()
{
	RichImageConverter.ConvertToImage((TPseudoColormap)ComboBox_ColorType.GetCurSel());
	RichImageConverter.RefreshImage();	
}

void CMainFrame::OnBN_SelectFileClick()
{
	OpenFileDialog* openFileDialog1 = new OpenFileDialog();

	if (openFileDialog1->ShowDialog())
	{
		HdfConverter converter;
		int width, height;
		double *data = converter.ConvertHdfImageToCImage(openFileDialog1->FileName, "octimage", width, height);
		RichImageConverter.Load(data, width, height);

		RichImageConverter.ConvertToImage(pcAutumn);
		RichImageConverter.RefreshImage();
		OnResizeComponents();
	}
}

void CMainFrame::OnResizeComponents(void)
{
	CRect ClientRect;
	this->GetClientRect(ClientRect);

	int FImageSize = min(ClientRect.Width()-210, ClientRect.Height()-20);

	RichImageConverter.SetImagePos(ClientRect.left+10, ClientRect.top+10, FImageSize, FImageSize, SWP_NOZORDER);
	ComboBox_ColorType.SetWindowPos(NULL, ClientRect.right-10-210, ClientRect.top+15, 210, 20*(1+12), SWP_NOZORDER);	
	openFileButton.SetWindowPos(NULL, ClientRect.right - 10 - 210, ClientRect.top + 50, 210, 20 * (1 + 12), SWP_NOZORDER);
}


