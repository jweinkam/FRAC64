// ColorSettings.cpp : implementation file
//

#include "stdafx.h"
#include "frac.h"
#include "RTWIN.hpp"
#include "ColorSettings.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CColorSettings dialog


CColorSettings::CColorSettings(RTVALS *rt, CWnd* pParent /*=NULL*/)
	: m_rt(rt), CDialog(CColorSettings::IDD, pParent)
{
    m_rtcopy = *m_rt;
	//{{AFX_DATA_INIT(CColorSettings)
	m_Blue = m_rtcopy.colorParams.m_Blue[0];
	m_Cycle = m_rtcopy.colorParams.m_Cycle;
	m_Green = m_rtcopy.colorParams.m_Green[0];
	m_CountScaling = m_rtcopy.colorParams.m_CountScaling;
	m_NumberColors = m_rtcopy.colorParams.m_numColors;
	m_Red = m_rtcopy.colorParams.m_Red[0];
	m_Method = m_rtcopy.colorParams.m_Hsv ? 1 : 0;
	m_Sat = m_rtcopy.colorParams.m_Sat;
	m_Val = m_rtcopy.colorParams.m_Val;
	//}}AFX_DATA_INIT
    m_CurrentPosition = 0;
    m_bInited = FALSE;
}

void CColorSettings::InitComboBox()
{
    int i;
    m_Positions.ResetContent();
    wchar_t data[10];
    for ( i = 0; i < m_rtcopy.colorParams.m_numColors; ++i )
    {
        swprintf_s( data, 10, L"%i", i );
        m_Positions.AddString( data );
    }
    if ( m_CurrentPosition >= m_rtcopy.colorParams.m_numColors )
    {
        m_CurrentPosition = 0;
    }
    m_Positions.SetCurSel( m_CurrentPosition );
}

void CColorSettings::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CColorSettings)
	DDX_Control(pDX, IDC_POSITION, m_Positions);
	DDX_Text(pDX, IDC_BLUE, m_Blue);
	DDX_Text(pDX, IDC_CYCLE, m_Cycle);
	DDX_Text(pDX, IDC_GREEN, m_Green);
	DDX_Radio(pDX, IDC_LOG, m_CountScaling);
	DDX_Text(pDX, IDC_NUMBERCOLORS, m_NumberColors);
	DDX_Text(pDX, IDC_RED, m_Red);
	DDX_Radio(pDX, IDC_RGB, m_Method);
	DDX_Text(pDX, IDC_SAT, m_Sat);
	DDX_Text(pDX, IDC_VAL, m_Val);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CColorSettings, CDialog)
	//{{AFX_MSG_MAP(CColorSettings)
	ON_EN_CHANGE(IDC_NUMBERCOLORS, OnChangeNumbercolors)
	ON_EN_CHANGE(IDC_RED, OnChangeRed)
	ON_EN_CHANGE(IDC_GREEN, OnChangeGreen)
	ON_EN_CHANGE(IDC_BLUE, OnChangeBlue)
	ON_WM_PAINT()
	ON_CBN_SELCHANGE(IDC_POSITION, OnSelchangePosition)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColorSettings message handlers

void CColorSettings::OnChangeNumbercolors() 
{
    UpdateData(TRUE);
    m_rtcopy.colorParams.m_numColors = m_NumberColors;
    InitComboBox();
}

void CColorSettings::OnChangeRed() 
{
    UpdateData(TRUE);
    m_rtcopy.colorParams.m_Red[m_CurrentPosition] = (UCHAR)m_Red;
}

void CColorSettings::OnChangeGreen() 
{
    UpdateData(TRUE);
    m_rtcopy.colorParams.m_Green[m_CurrentPosition] = (UCHAR)m_Green;
}

void CColorSettings::OnChangeBlue() 
{
    UpdateData(TRUE);
    m_rtcopy.colorParams.m_Blue[m_CurrentPosition] = (UCHAR)m_Blue;
}

void CColorSettings::OnOK() 
{
    UpdateData(TRUE);
	m_rtcopy.colorParams.m_Cycle = m_Cycle;
	m_rtcopy.colorParams.m_CountScaling = m_CountScaling;
	m_rtcopy.colorParams.m_Hsv = (m_Method == 1);
	m_rtcopy.colorParams.m_Sat = m_Sat;
	m_rtcopy.colorParams.m_Val = m_Val;

    *m_rt = m_rtcopy;
    CDialog::OnOK();
}

void CColorSettings::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
    if ( !m_bInited )
    {
        InitComboBox();
        m_bInited = TRUE;
    }
}

void CColorSettings::OnSelchangePosition() 
{
    m_CurrentPosition = m_Positions.GetCurSel();
    m_Red = m_rtcopy.colorParams.m_Red[m_CurrentPosition];
    m_Green = m_rtcopy.colorParams.m_Green[m_CurrentPosition];
    m_Blue = m_rtcopy.colorParams.m_Blue[m_CurrentPosition];
    UpdateData(FALSE);
}
