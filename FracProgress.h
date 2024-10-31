#pragma once
#include "afxcmn.h"
#include "RTWIN.hpp"

// FracProgress dialog

class FracProgress : public CDialogEx
{
	DECLARE_DYNAMIC(FracProgress)

public:
	FracProgress(CWnd* pParent, RTVALS &rt, const wchar_t * sFilename, bool Save, int &Result, bool SaveCounts = true);   // standard constructor
	virtual ~FracProgress();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG1 };
#endif

private:
    static int IOThreadEntry(void *arg);
    int IOThread();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
    CProgressCtrl Progress;
    RTVALS &m_rt;
    const wchar_t *m_sFilename;
    bool m_Save;
    bool m_SaveCounts;
    int &m_Result;
    bool m_Visible;
    virtual BOOL OnInitDialog();
    HANDLE m_Thread;

protected:
    afx_msg LRESULT OnIoprogress(WPARAM wParam, LPARAM lParam);
public:
    afx_msg void OnWindowPosChanging(WINDOWPOS* lpwndpos);
};
