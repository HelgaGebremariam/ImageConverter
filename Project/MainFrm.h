// MainFrm.h : interface of the CMainFrame class
#pragma once
#include "RichImageConverter.h"

class CMainFrame : public CFrameWnd
{
protected:
	CComboBox ComboBox_ColorType;
	CRichImageConverter<double> RichImageConverter;

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

	DECLARE_MESSAGE_MAP()
};


