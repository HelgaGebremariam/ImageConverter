// MainFrm.h : interface of the CMainFrame class
#pragma once
#include "RichImageConverter.h"
#include "OpenFileDialog.h"
#include "HdfConverter.h"

class CMainFrame : public CFrameWnd
{
protected:
	CComboBox ComboBox_ColorType;
	CRichImageConverter<double> RichImageConverter;
	CButton openFileButton;

	void OnResizeComponents(void);

	DECLARE_DYNAMIC(CMainFrame)

public:
	CMainFrame();

	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

protected:
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnClose();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnCB_ColorTypeChange();
	afx_msg void OnBN_SelectFileClick();
	DECLARE_MESSAGE_MAP()
};


