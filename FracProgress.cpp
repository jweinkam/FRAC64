// FracProgress.cpp : implementation file
//

#include "stdafx.h"
#include "Frac.h"
#include "FracProgress.h"
#include "afxdialogex.h"


// FracProgress dialog

IMPLEMENT_DYNAMIC(FracProgress, CDialogEx)

FracProgress::FracProgress(CWnd* pParent, RTVALS &rt, const wchar_t * sFilename, bool Save, int &Result, bool SaveCounts)
    : CDialogEx(IDD_DIALOG1, pParent),
    m_rt(rt),
    m_sFilename(sFilename),
    m_Save(Save),
    m_SaveCounts(SaveCounts),
    m_Result(Result),
    m_Visible(!(pParent->IsIconic()))
{

}

FracProgress::~FracProgress()
{
    WaitForSingleObject(m_Thread, INFINITE);
    CloseHandle(m_Thread);
}

void FracProgress::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_PROGRESS1, Progress);
}


BEGIN_MESSAGE_MAP(FracProgress, CDialogEx)
    ON_MESSAGE(WM_IOPROGRESS, &FracProgress::OnIoprogress)
    ON_WM_WINDOWPOSCHANGING()
END_MESSAGE_MAP()


// FracProgress message handlers


BOOL FracProgress::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    if (m_Save)
    {
        SetWindowText(L"Saving...");
    }
    else
    {
        SetWindowText(L"Loading...");
    }
    // TODO:  Add extra initialization here
    DWORD id;
    m_Thread = ::CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)IOThreadEntry, this, 0, &id);

    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}

int FracProgress::IOThreadEntry(void *arg)
{
    return ((FracProgress*)arg)->IOThread();
}

extern void saveimage(RTVALS &rt, const wchar_t * sFilename, FracProgress* dialog, bool SaveCounts = true);
extern int loadimage(RTVALS &rt, const wchar_t * filename, FracProgress* dialog);

int FracProgress::IOThread()
{
    if (m_Save)
    {
        saveimage(m_rt, m_sFilename, this, m_SaveCounts);
    }
    else
    {
        m_Result = loadimage(m_rt, m_sFilename, this);
    }
    return 0;
}


afx_msg LRESULT FracProgress::OnIoprogress(WPARAM wParam, LPARAM lParam)
{
    if (((LPARAM)wParam) == lParam)
    {
        EndDialog(0);
    }
    else
    {
        Progress.SetRange32(0, (int)lParam);
        Progress.SetPos((int)wParam);
    }
    return 0;
}


void FracProgress::OnWindowPosChanging(WINDOWPOS* lpwndpos)
{
    if (!m_Visible)
        lpwndpos->flags &= ~SWP_SHOWWINDOW;

    CDialogEx::OnWindowPosChanging(lpwndpos);
}
