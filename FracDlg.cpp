// FracDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Frac.h"
#include "FracDlg.h"
#include "StopDialog.h"
#include "MemoryAllocate.hpp"
#include "ColorSettings.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

wchar_t StopTitle[100];
wchar_t sOpenFile[CCHMAXPATH+2];
wchar_t sWindowTitle[CCHMAXPATH+20];
wchar_t sMenuWindowTitle[CCHMAXPATH+20];
wchar_t sSeriesName[CCHMAXPATH+20];
extern HWND hDlgCurrent;

/////////////////////////////////////////////////////////////////////////////
// CFracDlg dialog

CFracDlg::CFracDlg(rtvals *rt1, CWnd* pParent /*=NULL*/)
	: m_rt1(rt1), m_bIsStartRunning(false), CDialog()
{
    Create( CFracDlg::IDD, pParent );
    ShowWindow( SW_HIDE );
    wchar_t buffer[DECIMALDIGITS];
    long digits = (long)(rt1->zoom.log()) + EXTRADIGITS;
    if ( digits > DECIMALDIGITS-1 )
    {
        digits = DECIMALDIGITS-1;
    }
    m_rt1->xc.Format(buffer, digits); m_Xc = buffer;
    m_rt1->yc.Format(buffer, digits); m_Yc = buffer;
    m_rt1->zoom.Format(buffer, 86); m_Zoom = buffer;
    digits = (long)(rt1->jzoom.log()) + EXTRADIGITS;
    if ( digits > DECIMALDIGITS-1 )
    {
        digits = DECIMALDIGITS-1;
    }
	m_rt1->jx.Format(buffer, digits); m_Jx = buffer;
	m_rt1->jy.Format(buffer, digits); m_Jy = buffer;
    m_rt1->jzoom.Format(buffer, 86); m_JZoom = buffer;
    //{{AFX_DATA_INIT(CFracDlg)
	m_SimpleScan = m_rt1->simple;
	m_DrawPosition = m_rt1->drawposition;
    m_RestartScan = m_rt1->restartimage;
	m_Count = m_rt1->count;
	m_BlankScreen = m_rt1->blankscreen;
	m_Angle = m_rt1->angle;
	m_fillRegion = m_rt1->fillrect;
    m_DrawMandle = (m_rt1->drawmandle) ? 0 : 1;
	m_bUseWindowSize = TRUE;
	m_DynamicMax = L"";
	m_RenderTime = L"";
	m_lBits = m_rt1->bits;
    m_lThreads = m_rt1->threads;
    m_dOrder = m_rt1->order;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
    swprintf_s( buffer, DECIMALDIGITS, L"%i", m_rt1->dynamicCount );
    m_DynamicMax = buffer;

    int hours = (int)(m_rt1->timetaken / 3600);
    int minutes = (int)(m_rt1->timetaken / 60) - hours * 60;
    double seconds = m_rt1->timetaken - hours * 3600 - minutes * 60;
    if ( (minutes > 9) && (seconds > 9) )
        swprintf_s(buffer, DECIMALDIGITS, L"Render Time: %i Hours, %i Minutes, %.3lf Seconds", hours, minutes, seconds );
    else if ( minutes > 9 )
        swprintf_s(buffer, DECIMALDIGITS, L"Render Time: %i Hours, %i Minutes, 0%.3lf Seconds", hours, minutes, seconds );
    else if ( seconds > 9 )
        swprintf_s(buffer, DECIMALDIGITS, L"Render Time: %i Hours, 0%i Minutes, %.3lf Seconds", hours, minutes, seconds );
    else
        swprintf_s(buffer, DECIMALDIGITS, L"Render Time: %i Hours, 0%i Minutes, 0%.3lf Seconds", hours, minutes, seconds );
    m_RenderTime = buffer;
    
    swprintf_s(buffer, DECIMALDIGITS, L"%i", m_rt1->newBmpWidth);
    m_Width.SetWindowText(buffer);
    swprintf_s(buffer, DECIMALDIGITS, L"%i", m_rt1->newBmpHeight);
    m_Height.SetWindowText(buffer);
    m_Width.EnableWindow( !m_bUseWindowSize );
    m_Height.EnableWindow( !m_bUseWindowSize );
    UpdateData(FALSE);
}

void CFracDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFracDlg)
	DDX_Control(pDX, IDC_WIDTH, m_Width);
	DDX_Control(pDX, IDC_HEIGHT, m_Height);
	DDX_Text(pDX, IDC_XC, m_Xc);
	DDV_MaxChars(pDX, m_Xc, DECIMALDIGITS);
	DDX_Text(pDX, IDC_YC, m_Yc);
	DDV_MaxChars(pDX, m_Yc, DECIMALDIGITS);
	DDX_Check(pDX, IDC_SIMPLESCAN, m_SimpleScan);
	DDX_Text(pDX, IDC_SCALE, m_Zoom);
	DDX_Text(pDX, IDC_JY, m_Jy);
	DDV_MaxChars(pDX, m_Jy, DECIMALDIGITS);
	DDX_Text(pDX, IDC_JX, m_Jx);
	DDV_MaxChars(pDX, m_Jx, DECIMALDIGITS);
	DDX_Check(pDX, IDC_DRAWPOSITION, m_DrawPosition);
	DDX_Text(pDX, IDC_COUNT, m_Count);
	DDX_Check(pDX, IDC_BLANK, m_BlankScreen);
	DDX_Text(pDX, IDC_ANGLE, m_Angle);
	DDX_Check(pDX, IDC_REGION, m_fillRegion);
	DDX_Radio(pDX, IDC_MANDLE, m_DrawMandle);
	DDX_Check(pDX, IDC_USE_WINDOW_SIZE, m_bUseWindowSize);
	DDX_Text(pDX, IDC_DYNAMICMAX, m_DynamicMax);
	DDX_Text(pDX, IDC_RENDERTIME, m_RenderTime);
	DDX_Text(pDX, IDC_JSCALE, m_JZoom);
	DDX_Text(pDX, IDC_BITS, m_lBits);
    DDX_Text(pDX, IDC_THREADS, m_lThreads);
    DDX_Text(pDX, IDC_ORDER, m_dOrder);
    DDX_Check(pDX, IDC_RESTARTSCAN, m_RestartScan);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CFracDlg, CDialog)
	//{{AFX_MSG_MAP(CFracDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_EN_CHANGE(IDC_XC, OnChangeXc)
	ON_EN_CHANGE(IDC_YC, OnChangeYc)
	ON_EN_CHANGE(IDC_SCALE, OnChangeScale)
	ON_EN_CHANGE(IDC_ANGLE, OnChangeAngle)
	ON_EN_CHANGE(IDC_JX, OnChangeJx)
	ON_EN_CHANGE(IDC_JY, OnChangeJy)
	ON_EN_CHANGE(IDC_COUNT, OnChangeCount)
	ON_BN_CLICKED(IDC_SIMPLESCAN, OnSimplescan)
	ON_BN_CLICKED(IDC_BLANK, OnBlank)
	ON_BN_CLICKED(IDC_REGION, OnRegion)
	ON_BN_CLICKED(IDC_DRAWPOSITION, OnDrawposition)
	ON_BN_CLICKED(IDC_MANDLE, OnMandle)
	ON_BN_CLICKED(IDC_JULIA, OnJulia)
	ON_BN_CLICKED(ID_RENDER, OnRender)
	ON_BN_CLICKED(ID_RENDERSERIES, OnRenderseries)
	ON_BN_CLICKED(ID_LOAD, OnLoad)
	ON_BN_CLICKED(ID_SAVE, OnSave)
	ON_BN_CLICKED(ID_COLORSETTINGS, OnColorsettings)
	ON_BN_CLICKED(ID_QUIT, OnQuit)
	ON_WM_MOVE()
	ON_BN_CLICKED(IDC_USE_WINDOW_SIZE, OnUseWindowSize)
	ON_EN_CHANGE(IDC_JSCALE, OnChangeJscale)
	ON_EN_CHANGE(IDC_BITS, OnChangeBits)
    ON_BN_CLICKED(IDC_RESTARTSCAN, &CFracDlg::OnBnClickedRestartscan)
	//}}AFX_MSG_MAP
    ON_EN_CHANGE(IDC_THREADS, &CFracDlg::OnEnChangeThreads)
    ON_EN_CHANGE(IDC_ORDER, &CFracDlg::OnEnChangeOrder)
    ON_WM_ACTIVATE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFracDlg message handlers

BOOL CFracDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CFracDlg::OnPaint() 
{
	CDialog::OnPaint();
}

void CFracDlg::OnChangeXc() 
{
    UpdateData(TRUE);
    m_rt1->xc = CLargeFixedInt(BIGESTSIZE, (LPCTSTR)m_Xc);
    if ( m_rt1->drawposition )
    {
        ((CWnd*)(m_rt1->hwndWindow))->Invalidate();
    }
}

void CFracDlg::OnChangeYc() 
{
    UpdateData(TRUE);
    m_rt1->yc = CLargeFixedInt(BIGESTSIZE, (LPCTSTR)m_Yc);
    if ( m_rt1->drawposition )
    {
        ((CWnd*)(m_rt1->hwndWindow))->Invalidate();
    }
}

void CFracDlg::OnChangeScale() 
{
    UpdateData(TRUE);
    m_rt1->zoom = LDOUBLE(0,(LPCTSTR)m_Zoom);
    if ( m_rt1->drawposition )
    {
        ((CWnd*)(m_rt1->hwndWindow))->Invalidate();
    }
}

void CFracDlg::OnChangeAngle() 
{
    UpdateData(TRUE);
    m_rt1->angle = m_Angle;
    if ( m_rt1->drawposition )
    {
        ((CWnd*)(m_rt1->hwndWindow))->Invalidate();
    }
}

void CFracDlg::OnChangeJx() 
{
    UpdateData(TRUE);
    m_rt1->jx = CLargeFixedInt(BIGESTSIZE, (LPCTSTR)m_Jx);
}

void CFracDlg::OnChangeJy() 
{
    UpdateData(TRUE);
    m_rt1->jy = CLargeFixedInt(BIGESTSIZE, (LPCTSTR)m_Jy);
}

void CFracDlg::OnChangeCount() 
{
    UpdateData(TRUE);
    m_rt1->count = m_Count;
}

void CFracDlg::OnSimplescan() 
{
    UpdateData(TRUE);
    m_rt1->simple = (m_SimpleScan != FALSE);
}

void CFracDlg::OnBlank() 
{
    UpdateData(TRUE);
    m_rt1->blankscreen = (m_BlankScreen != FALSE);
}

void CFracDlg::OnRegion() 
{
    UpdateData(TRUE);
    m_rt1->fillrect = (m_fillRegion != FALSE);
}

void CFracDlg::OnDrawposition() 
{
    UpdateData(TRUE);
    m_rt1->drawposition = (m_DrawPosition != FALSE);
    ((CWnd*)(m_rt1->hwndWindow))->Invalidate();
}

void CFracDlg::OnMandle() 
{
    UpdateData(TRUE);
    m_rt1->drawmandle = (m_DrawMandle == 0);
}

void CFracDlg::OnJulia() 
{
    UpdateData(TRUE);
    m_rt1->drawmandle = (m_DrawMandle == 0);
}

void CFracDlg::OnRender() 
{
    UpdateData(TRUE);
	ShowWindow( SW_HIDE );
    m_bIsSeries = false;

    CString string;
    m_Width.GetWindowText( string );
    m_rt1->newBmpWidth = _wtol( string );
    m_Height.GetWindowText( string );
    m_rt1->newBmpHeight = _wtol( string );
    if ( (m_rt1->newBmpWidth != m_rt1->bmpWidth) || (m_rt1->newBmpHeight != m_rt1->bmpHeight) )
    {
        m_rt1->bmpWidth = m_rt1->newBmpWidth;
        m_rt1->bmpHeight = m_rt1->newBmpHeight;
        m_rt1->paintBmpWidth = m_rt1->newBmpWidth;
        m_rt1->paintBmpHeight = m_rt1->newBmpHeight;
        FreeMemory( m_rt1->bmp );
        m_rt1->bmp = (UCHAR*) AllocateBitmapMemory( m_rt1->bmpWidth, m_rt1->bmpHeight, m_rt1->bmpPixelStep, &(m_rt1->bmpRowSize) );
    }

    m_bIsStartRunning = true;
}

void CFracDlg::OnRenderseries() 
{
    CFileDialog *pfdFiledlg;
    pfdFiledlg = new CFileDialog( FALSE, L"POS", L"*.POS", 0 );

    INT_PTR ret = pfdFiledlg->DoModal();
    if (ret == IDOK)
    {
        wcscpy_s( sSeriesName, CCHMAXPATH+20, pfdFiledlg->GetPathName() );

        ShowWindow( SW_HIDE );
	    ShowWindow( SW_HIDE );
        m_bIsSeries = true;

        CString string;
        m_Width.GetWindowText( string );
        m_rt1->newBmpWidth = _wtol( string );
        m_Height.GetWindowText( string );
        m_rt1->newBmpHeight = _wtol( string );
        if ( (m_rt1->newBmpWidth != m_rt1->bmpWidth) || (m_rt1->newBmpHeight != m_rt1->bmpHeight) )
        {
            m_rt1->bmpWidth = m_rt1->newBmpWidth;
            m_rt1->bmpHeight = m_rt1->newBmpHeight;
            m_rt1->paintBmpWidth = m_rt1->bmpWidth;
            m_rt1->paintBmpHeight = m_rt1->bmpHeight;
            FreeMemory( m_rt1->bmp );
            m_rt1->bmp = (UCHAR*) AllocateBitmapMemory( m_rt1->bmpWidth, m_rt1->bmpHeight, m_rt1->bmpPixelStep, &(m_rt1->bmpRowSize) );
        }

        m_bIsStartRunning = true;
    }
    delete pfdFiledlg;

}

extern void LoadCounts( RTVALS *rt, bool CallCalcColor );
void CFracDlg::OnLoad() 
{
    int retVal = 0;
    if ((retVal = loadimage( *m_rt1, NULL )) != 0)
    {
        LoadCounts(m_rt1, (retVal == 1));
        m_bUseWindowSize = FALSE;
    }
    UpdateData( FALSE );
    m_Width.EnableWindow( !m_bUseWindowSize );
    m_Height.EnableWindow( !m_bUseWindowSize );

    m_rt1->newBmpHeight = m_rt1->bmpHeight;
    m_rt1->newBmpWidth = m_rt1->bmpWidth;

    wchar_t buffer[DECIMALDIGITS];
    swprintf_s( buffer, DECIMALDIGITS, L"%i", m_rt1->newBmpWidth );
    m_Width.SetWindowText( buffer );
    swprintf_s( buffer, DECIMALDIGITS, L"%i", m_rt1->newBmpHeight );
    m_Height.SetWindowText( buffer );

    ((CWnd*)(m_rt1->hwndWindow))->Invalidate();

    swprintf_s( sWindowTitle, CCHMAXPATH+20, L"Frac - %s", m_rt1->sOpenFile );
    ((CWnd*)(m_rt1->hwndWindow))->SetWindowText( sWindowTitle );
    ParameterChanged();
}

void CFracDlg::OnSave() 
{
    saveimage( *m_rt1 );

    swprintf_s( sWindowTitle, CCHMAXPATH+20, L"Frac - %s", m_rt1->sOpenFile );
    ((CWnd*)(m_rt1->hwndWindow))->SetWindowText( sWindowTitle );
}

extern void RecolorBMP(RTVALS *rt, bool CallCalcColor);
void CFracDlg::OnColorsettings() 
{
    CColorSettings colorSettings( m_rt1 );
    colorSettings.DoModal();
    RecolorBMP(m_rt1, true);
    ((CWnd*)(m_rt1->hwndWindow))->Invalidate();
}

void CFracDlg::OnQuit() 
{
    UpdateData(TRUE);
    CWnd * pOwner = GetOwner();
    EndDialog( ID_QUIT );
    pOwner->SendMessage( WM_CLOSE, 0, 0);
}

void CFracDlg::OnMove(int x, int y) 
{
	CDialog::OnMove(x, y);
    if ( m_rt1->menuset )
    {
        RECT r;
        GetWindowRect( &r );
        m_rt1->MenuPos.x = r.left;
        m_rt1->MenuPos.y = r.top;
    }
}

void CFracDlg::ParameterChanged()
{
    wchar_t buffer[DECIMALDIGITS];
    long digits = (long)(m_rt1->zoom.log()) + EXTRADIGITS;
    if ( digits > DECIMALDIGITS-1 )
    {
        digits = DECIMALDIGITS-1;
    }
    m_rt1->xc.Format(buffer, digits); m_Xc = buffer;
    m_rt1->yc.Format(buffer, digits); m_Yc = buffer;
    m_rt1->zoom.Format(buffer, 86); m_Zoom = buffer;
    digits = (long)(m_rt1->jzoom.log()) + EXTRADIGITS;
    if ( digits > DECIMALDIGITS-1 )
    {
        digits = DECIMALDIGITS-1;
    }
	m_rt1->jx.Format(buffer, digits); m_Jx = buffer;
	m_rt1->jy.Format(buffer, digits); m_Jy = buffer;
    m_rt1->jzoom.Format(buffer, 86); m_JZoom = buffer;
	m_SimpleScan = m_rt1->simple;
	m_DrawPosition = m_rt1->drawposition;
	m_Count = m_rt1->count;
	m_BlankScreen = m_rt1->blankscreen;
	m_Angle = m_rt1->angle;
	m_fillRegion = m_rt1->fillrect;
    m_DrawMandle = (m_rt1->drawmandle) ? 0 : 1;

    swprintf_s( buffer, DECIMALDIGITS, L"%i", m_rt1->dynamicCount );
    m_DynamicMax = buffer;

    if ( m_bUseWindowSize )
    {
        swprintf_s( buffer, DECIMALDIGITS, L"%i", m_rt1->newBmpWidth );
        m_Width.SetWindowText( buffer );
        swprintf_s( buffer, DECIMALDIGITS, L"%i", m_rt1->newBmpHeight );
        m_Height.SetWindowText( buffer );
    }
    int hours = (int)(m_rt1->timetaken / 3600);
    int minutes = (int)(m_rt1->timetaken / 60) - hours * 60;
    double seconds = m_rt1->timetaken - hours * 3600 - minutes * 60;
    if ( (minutes > 9) && (seconds > 9) )
        swprintf_s(buffer, DECIMALDIGITS, L"Render Time: %i Hours, %i Minutes, %.3lf Seconds", hours, minutes, seconds );
    else if ( minutes > 9 )
        swprintf_s(buffer, DECIMALDIGITS, L"Render Time: %i Hours, %i Minutes, 0%.3lf Seconds", hours, minutes, seconds );
    else if ( seconds > 9 )
        swprintf_s(buffer, DECIMALDIGITS, L"Render Time: %i Hours, 0%i Minutes, %.3lf Seconds", hours, minutes, seconds );
    else
        swprintf_s(buffer, DECIMALDIGITS, L"Render Time: %i Hours, 0%i Minutes, 0%.3lf Seconds", hours, minutes, seconds );
    m_RenderTime = buffer;
    m_lBits = m_rt1->bits;
    m_lThreads = m_rt1->threads;
    m_dOrder = m_rt1->order;
    m_RestartScan = m_rt1->restartimage;

    UpdateData(FALSE);
    Invalidate();
}

void CFracDlg::WindowSizeChanged()
{
    wchar_t buffer[DECIMALDIGITS];
    if ( m_bUseWindowSize )
    {
        swprintf_s( buffer, DECIMALDIGITS, L"%i", m_rt1->newBmpWidth );
        m_Width.SetWindowText( buffer );
        swprintf_s( buffer, DECIMALDIGITS, L"%i", m_rt1->newBmpHeight );
        m_Height.SetWindowText( buffer );
    }

    UpdateData(FALSE);
    Invalidate();
}

void CFracDlg::OnUseWindowSize() 
{
    wchar_t buffer[DECIMALDIGITS];
    UpdateData(TRUE);
    m_Width.EnableWindow( !m_bUseWindowSize );
    m_Height.EnableWindow( !m_bUseWindowSize );

    if ( m_bUseWindowSize )
    {
        m_rt1->newBmpWidth = m_rt1->WindowSize.x;
        swprintf_s( buffer, DECIMALDIGITS, L"%i", m_rt1->newBmpWidth );
        m_Width.SetWindowText( buffer );
        m_rt1->newBmpHeight = m_rt1->WindowSize.y;
        swprintf_s( buffer, DECIMALDIGITS, L"%i", m_rt1->newBmpHeight );
        m_Height.SetWindowText( buffer );
    }

}

void CFracDlg::OnChangeJscale() 
{
    UpdateData(TRUE);
    m_rt1->jzoom = LDOUBLE(0,(LPCTSTR)m_JZoom);
}

void CFracDlg::OnChangeBits() 
{
    UpdateData(TRUE);
    m_rt1->bits = m_lBits;
}

void CFracDlg::OnBnClickedRestartscan()
{
    UpdateData(TRUE);
    m_rt1->restartimage = (m_RestartScan != FALSE);
}


void CFracDlg::OnEnChangeThreads()
{
    UpdateData(TRUE);
    m_rt1->threads = m_lThreads;
}

void CFracDlg::OnEnChangeOrder()
{
    UpdateData(TRUE);
//    if (m_dOrder < 2)
//    {
//        m_dOrder = 2;
//        UpdateData(FALSE);
//    }
#ifdef ONLY_INT_POWER
    m_rt1->order = (ULONG)m_dOrder;
    if (m_rt1->order < 2)
    {
        m_rt1->order = 2;
    }
#endif
    m_rt1->order = m_dOrder;
}

void CFracDlg::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
    CDialog::OnActivate(nState, pWndOther, bMinimized);

    if (nState == WA_INACTIVE)  // becoming inactive
    {
        hDlgCurrent = NULL;
    }
    else                        // becoming active
    {
        hDlgCurrent = GetSafeHwnd();
    }
}
