// 2DMD_SIMLATEDlg.cpp : implementation file
//

#include "stdafx.h"
#include "2DMD_SIMLATE.h"
#include "2DMD_SIMLATEDlg.h"
#include "ImagePointNewProc.h"
#include <math.h>
#include <time.h>
#include ".\2dmd_simlatedlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define GFC 2.3548 					/* gauss halfwidth = GFC * std dev */
#define MM 65						/* basic size of filter array */
#define GGG 765						/* 3*8 bit expanded greylevel range */
#define chan_no 4					//pre-defined number of multichannels in use

// define processing variables and arrays
int DELAY = 50;					// AM Oct08 less delay in simulation
int filter[chan_no][MM][MM];		// filter array >> must be the same size in MakeFilter !!!
int MIDF = (MM-1)/2;				// centre of filter array
int WIDF = 32;						// width (radius) of active filter region
int SCAF = 1000;					// filter scale: integrated filter weights
int FRAMFACT = 8;					// number of simulation steps per frame
int channel;
double resp_max[chan_no+2][2];

// define external string and image variables
char in_name_str[50] = "test";			// file name:	same initialisation as in Dlg
int  n_first = 1;						// first frame: same initialisation as in Dlg
int  n_last  = 20;						// last frame:	same initialisation as in Dlg
int	 direction = 0;						// direction of sequence
int  max_norm = 0;
int	 start_x = 100;		int start_y = 100;
int  end_x = 156;		int end_y = 156;

// define external model parameters
double delphi[chan_no];      	// sampling base array of [4] in pixels !! initialise in Dlg !!!
double timcon[chan_no]; 		// time constant array of [4], in input frames !! initialise in Dlg !!!
double sigfac = 0.5;		// std.dev of centre to delphi ratio (halfwidth = 1.17 delta phi)
double gainctr = 1.0;		// centre amlification: balanced centre & surround
double amplifi = 4.0;		// input amplification (gain factor of DOG output) !! initialise in Dlg !!!
double balanc = 1.0;		// optimally balanced (in subtraction) EMD
double outgain = 2.0;		// output amplification (gain factor of EMD output) !! initialise in Dlg !!!
int	   prepro = 0;			// preprocessing (0|1|2|3) none|HWR|FWR|SQR

double surwid = 1.6; 		// surround-centre width ratio to produce reasonable CTF: kept constant at present
double pi = 3.1415926535;
double  edge_thresh = 0.002; // threshold amplitude of edge channel AM Oct08 !!!
FILE	*fo;

// define external image parameters and arrays
int sizx = 0;							// horizontal image size (# columns)
int sizy = 0;							// vertical image size (# rows)
int	gmax = 0;							// maximum input intensity
int	gmin = 0;							// minimum input intensity
int	fmax = 0;							// maximum filtered intensity
int	fmin = 0;							// minimum filtered intensity
unsigned short int * ppGvalues;			// pointer to sequence greylevels (3D data array)
unsigned short int * ppFvalues;			// pointer to sequence filtered levels (3D data array)
double * ppHvalues;						// pointer to sequence of horizontal response
double * ppVvalues;						// pointer to sequence of vertical response   
int gval_heap = 0;						// flag for allocation of heap space for gvalues
int fval_heap = 0;						// flag for allocation of heap space for fvalues
int hval_heap = 0;						// flag for allocation of heap space for hvalues
int vval_heap = 0;						// flag for allocation of heap space for vvalues
/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMy2DMD_SIMLATEDlg dialog

CMy2DMD_SIMLATEDlg::CMy2DMD_SIMLATEDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMy2DMD_SIMLATEDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMy2DMD_SIMLATEDlg)
	m_in_filename = _T("test");	// file name
	m_in_first_n = 1;				// first frame
	m_in_last_n = 20;				// last frame
	m_in_emdgain = _T("2.0");		// output amplification (gain factor of EMD output)
	m_in_filtergain = _T("4.0");	// input amplification (gain factor of DOG output)
	m_in_emddist = _T("2.0");		// default sampling base in pixels (same as above!!)
	m_in_emdtimc = _T("2.0");		// time constant in input frames (same as above!!)
	m_in_emddist2 = _T("4.0");		// default sampling base in pixels (same as above!!)
	m_in_emdtimc2 = _T("2.0");		// time constant in input frames (same as above!!)
	m_in_emddist3 = _T("8.0");		// default sampling base in pixels (same as above!!)
	m_in_emdtimc3 = _T("2.0");		// time constant in input frames (same as above!!)
	m_in_emddist4 = _T("16.0");		// default sampling base in pixels (same as above!!)
	m_in_emdtimc4 = _T("2.0");		// time constant in input frames (same as above!!)
	m_in_edgethr = _T("0.002");		// default threshold for Edge-W combo
	m_in_preproc = 0;				// preprocessing (0|1|2|3) none|HWR|FWR|SQR
	m_in_sizeratio = _T("0.5");		// std.dev of centre to delphi ratio 
	m_in_balanc = _T("1.0");		// balanced factor in EMD subtraction
	m_in_cntramp = _T("1.0");		// DOG filter centre amlification:
	m_in_status_load = _T("BLANK");	// image sequence buffer status
	m_in_status_filt = _T("BLANK");	// filter buffer status
	m_in_status_mot = _T("BLANK");	// motion response buffer status
	m_in_status_comb = _T("BLANK");	// combination buffer status
	m_direction = 0;			// direction of frames forward or reverse
	m_max_norm = 0;
	m_start_x = 100;			// horizontal start point of region of interest
	m_start_y = 100;			// vertical start point of region of interest
	m_end_x = 156;				// horizontal end point of region of interest
	m_end_y = 156;				// vertical end point of region of interest
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMy2DMD_SIMLATEDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMy2DMD_SIMLATEDlg)
	DDX_Text(pDX, IDC_InFilename, m_in_filename);
	DDX_Text(pDX, IDC_InFirstN, m_in_first_n);
	DDX_Text(pDX, IDC_InLastN, m_in_last_n);
	DDX_Text(pDX, IDC_InGainEMD, m_in_emdgain);
	DDX_Text(pDX, IDC_InGainDOG, m_in_filtergain);
	DDX_Text(pDX, IDC_InDistEMD, m_in_emddist);
	DDX_Text(pDX, IDC_InTimcEMD, m_in_emdtimc);
	DDX_Text(pDX, IDC_InDistEMD2, m_in_emddist2);
	DDX_Text(pDX, IDC_InTimcEMD2, m_in_emdtimc2);
	DDX_Text(pDX, IDC_InDistEMD3, m_in_emddist3);
	DDX_Text(pDX, IDC_InTimcEMD3, m_in_emdtimc3);
	DDX_Text(pDX, IDC_InDistEMD4, m_in_emddist4);
	DDX_Text(pDX, IDC_InTimcEMD4, m_in_emdtimc4);
	DDX_Text(pDX, IDC_Inedge_thr, m_in_edgethr);
	DDX_Text(pDX, IDC_InPreproNL, m_in_preproc);
	DDX_Text(pDX, IDC_InSizeDOG, m_in_sizeratio);
	DDX_Text(pDX, IDC_InBalanEMD, m_in_balanc);
	DDX_Text(pDX, IDC_InCentAmpDOG, m_in_cntramp);
	DDX_Text(pDX, IDC_STATUS_LOAD, m_in_status_load);
	DDX_Text(pDX, IDC_STATUS_FILT, m_in_status_filt);
	DDX_Text(pDX, IDC_STATUS_MOT, m_in_status_mot);
	DDX_Text(pDX, IDC_STATUS_COMB, m_in_status_comb);
	DDX_Text(pDX, IDC_DIRECTION, m_direction);
	DDX_Text(pDX, IDC_MAXNORM, m_max_norm);
	DDX_Text(pDX, IDC_STARTX, m_start_x);
	DDX_Text(pDX, IDC_STARTY, m_start_y);
	DDX_Text(pDX, IDC_ENDX, m_end_x);
	DDX_Text(pDX, IDC_ENDY, m_end_y);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CMy2DMD_SIMLATEDlg, CDialog)
	//{{AFX_MSG_MAP(CMy2DMD_SIMLATEDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_EN_CHANGE(IDC_InFilename, OnChangeInFilename)
	ON_EN_CHANGE(IDC_InFirstN, OnChangeInFirstN)
	ON_EN_CHANGE(IDC_InLastN, OnChangeInLastN)
	ON_EN_CHANGE(IDC_InGainEMD, OnChangeInGainEMD)
	ON_EN_CHANGE(IDC_InGainDOG, OnChangeInGainDOG)
	ON_EN_CHANGE(IDC_InTimcEMD, OnChangeInTimcEMD)
	ON_EN_CHANGE(IDC_InDistEMD, OnChangeInDistEMD)
	ON_EN_CHANGE(IDC_InTimcEMD2, OnChangeInTimcEMD)
	ON_EN_CHANGE(IDC_InDistEMD2, OnChangeInDistEMD)
	ON_EN_CHANGE(IDC_InTimcEMD3, OnChangeInTimcEMD)
	ON_EN_CHANGE(IDC_InDistEMD3, OnChangeInDistEMD)
	ON_EN_CHANGE(IDC_InTimcEMD4, OnChangeInTimcEMD)
	ON_EN_CHANGE(IDC_InDistEMD4, OnChangeInDistEMD)
	ON_EN_CHANGE(IDC_Inedge_thr, OnChangeInTimcEMD)
	ON_EN_CHANGE(IDC_InPreproNL, OnChangeInPreproNL)
	ON_EN_CHANGE(IDC_InSizeDOG, OnChangeInSizeDOG)
	ON_EN_CHANGE(IDC_InCentAmpDOG, OnChangeInCentAmpDOG)
	ON_EN_CHANGE(IDC_InBalanEMD, OnChangeInBalanEMD)
	ON_EN_CHANGE(IDC_DIRECTION, OnChangeInDirection)
	ON_EN_CHANGE(IDC_MAXNORM, OnChangeInMaxNorm)
	ON_EN_CHANGE(IDC_STARTX, OnChangeInStartX)
	ON_EN_CHANGE(IDC_STARTY, OnChangeInStartY)
	ON_EN_CHANGE(IDC_ENDX, OnChangeInEndX)
	ON_EN_CHANGE(IDC_ENDY, OnChangeInEndY)
	ON_BN_CLICKED(IDC_LOADINPUT, OnLoadinput)
	ON_BN_CLICKED(IDC_FILTER, OnFilter)
	ON_BN_CLICKED(IDC_MOTION, OnMotion)
	ON_BN_CLICKED(IDC_SIMULATE, OnSimulate)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_DISPCOMBO, OnDisplay)
	ON_BN_CLICKED(IDC_DISPMCALL, OnDisplay) // to be changed, to show intemediate multi-channels
	END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMy2DMD_SIMLATEDlg message handlers

BOOL CMy2DMD_SIMLATEDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);
	
	delphi[0]=2.0;delphi[1]=4.0;delphi[2]=8.0;delphi[3]=16.0; //initialize when dialogue opens AM08
	timcon[0]=2.0;timcon[1]=2.0;timcon[2]=2.0;timcon[3]=2.0; 
	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	m_pImageObject = NULL;
	n_pImageObject = NULL;
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CMy2DMD_SIMLATEDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CMy2DMD_SIMLATEDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CMy2DMD_SIMLATEDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

/////////////////////// ADDED FUNCTIONALITY /////////////////////////////////////
/////////////////////// ADDED FUNCTIONALITY /////////////////////////////////////
/////////////////////// ADDED FUNCTIONALITY /////////////////////////////////////

void CMy2DMD_SIMLATEDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	//if (gval_heap == 1)			
	//	{ delete ppGvalues;		ppGvalues = 0; }	// free memory and reset pointer
	//if (fval_heap == 1)			
	//	{ delete ppFvalues;		ppFvalues = 0; }	// free memory and reset pointer
	
	CDialog::OnCancel();
}

void CMy2DMD_SIMLATEDlg::OnChangeInDirection() 
{
	UpdateData(TRUE);
	direction = m_direction;
}

void CMy2DMD_SIMLATEDlg::OnChangeInMaxNorm() 
{
	UpdateData(TRUE);
	max_norm = m_max_norm;
}

void CMy2DMD_SIMLATEDlg::OnChangeInStartX() 
{
	UpdateData(TRUE);
	start_x = m_start_x;
}

void CMy2DMD_SIMLATEDlg::OnChangeInStartY() 
{
	UpdateData(TRUE);
	start_y = m_start_y;
}

void CMy2DMD_SIMLATEDlg::OnChangeInEndX() 
{
	UpdateData(TRUE);
	end_x = m_end_x;
}

void CMy2DMD_SIMLATEDlg::OnChangeInEndY() 
{
	UpdateData(TRUE);
	end_y = m_end_y;
}

void CMy2DMD_SIMLATEDlg::OnChangeInFilename() 
{
	// TODO: Add your control notification handler code here
	// input of new filename
	UpdateData(TRUE); sprintf( in_name_str, "%s", m_in_filename); 
	// reset image object and clear screen >> load new file
	if ( m_pImageObject!=NULL )
		{ RedrawWindow();			// erase previous image and messages
		m_pImageObject=NULL; }
	if (gval_heap == 1)			
		{ delete ppGvalues;		ppGvalues = 0; }	// free memory and reset pointer
	if (fval_heap == 1)			
		{ delete ppFvalues;		ppFvalues = 0; }	// free memory and reset pointer
}


void CMy2DMD_SIMLATEDlg::OnChangeInFirstN() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE); n_first = m_in_first_n; 
	// reset image object and clear screen >> load new file
	if ( m_pImageObject!=NULL )
		{ RedrawWindow();			// erase previous image and messages
		m_pImageObject=NULL; }
	if (gval_heap == 1)			
		{ delete ppGvalues;		ppGvalues = 0; }	// free memory and reset pointer
	if (fval_heap == 1)			
		{ delete ppFvalues;		ppFvalues = 0; }	// free memory and reset pointer
}


void CMy2DMD_SIMLATEDlg::OnChangeInLastN() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE); n_last = m_in_last_n; 	
	// reset image object and clear screen >> load new file
	if ( m_pImageObject!=NULL )
		{ RedrawWindow();			// erase previous image and messages
		m_pImageObject=NULL; }
	if (gval_heap == 1)			
		{ delete ppGvalues;		ppGvalues = 0; }	// free memory and reset pointer
	if (fval_heap == 1)			
		{ delete ppFvalues;		ppFvalues = 0; }	// free memory and reset pointer
}


void CMy2DMD_SIMLATEDlg::OnChangeInGainDOG() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE); amplifi = atof(m_in_filtergain); 	
	// reset image object and clear screen >> calculate new filter output
	if ( m_pImageObject!=NULL )
		{ RedrawWindow();			// erase previous image and messages
		m_pImageObject=NULL; }
	if (fval_heap == 1)			
		{ delete ppFvalues;		ppFvalues = 0; }	// free memory and reset pointer
}


void CMy2DMD_SIMLATEDlg::OnChangeInPreproNL() 
{
	// TODO: Add your control notification handler code here
	// TODO: nonlinear preprocessing function still need to be implemented
	// TODO: Add your control notification handler code here
	UpdateData(TRUE); prepro = m_in_preproc; 
	// reset image object and clear screen >> calculate new filter output
	if ( m_pImageObject!=NULL )
		{ RedrawWindow();			// erase previous image and messages
		m_pImageObject=NULL; }
	if (fval_heap == 1)			
		{ delete ppFvalues;		ppFvalues = 0; }	// free memory and reset pointer
}


void CMy2DMD_SIMLATEDlg::OnChangeInTimcEMD() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE); 
	timcon[0] = atof(m_in_emdtimc);
	timcon[1] = atof(m_in_emdtimc2);
	timcon[2] = atof(m_in_emdtimc3);
	timcon[3] = atof(m_in_emdtimc4);
	edge_thresh = atof(m_in_edgethr);
	// erase old image & text
	if ( n_pImageObject!=NULL )
	  RedrawWindow();			// erase previous image and messages
	// reset image object >> calculate new 2DDMD output
	n_pImageObject=NULL;		
}


void CMy2DMD_SIMLATEDlg::OnChangeInDistEMD() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE); 
	delphi[0] = atof(m_in_emddist);
	delphi[1] = atof(m_in_emddist2);
	delphi[2] = atof(m_in_emddist3);
	delphi[3] = atof(m_in_emddist4);
	// erase old image & text
	if ( n_pImageObject!=NULL )
	  RedrawWindow();			// erase previous image and messages
	// reset image object >> calculate new 2DDMD output
	n_pImageObject=NULL;	
}


void CMy2DMD_SIMLATEDlg::OnChangeInGainEMD() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE); outgain = atof(m_in_emdgain); 	
	// erase old image & text
	if ( n_pImageObject!=NULL )
	  RedrawWindow();			// erase previous image and messages
	// reset image object >> calculate new 2DDMD output
	n_pImageObject=NULL;
}


void CMy2DMD_SIMLATEDlg::OnChangeInSizeDOG() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE); sigfac = atof(m_in_sizeratio); 	
	// reset image object and clear screen >> calculate new filter output
	if ( m_pImageObject!=NULL )
		{ RedrawWindow();			// erase previous image and messages
		m_pImageObject=NULL; }
	if (fval_heap == 1)			
		{ delete ppFvalues;		ppFvalues = 0; }	// free memory and reset pointer	
}


void CMy2DMD_SIMLATEDlg::OnChangeInCentAmpDOG() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE); gainctr = atof(m_in_cntramp); 	
	// reset image object and clear screen >> calculate new filter output
	if ( m_pImageObject!=NULL )
		{ RedrawWindow();			// erase previous image and messages
		m_pImageObject=NULL; }
	if (fval_heap == 1)			
		{ delete ppFvalues;		ppFvalues = 0; }	// free memory and reset pointer	
}


void CMy2DMD_SIMLATEDlg::OnChangeInBalanEMD() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE); balanc = atof(m_in_balanc); 	
	// erase old image & text
	if ( n_pImageObject!=NULL )
	  RedrawWindow();			// erase previous image and messages
	// reset image object >> calculate new 2DDMD output
	n_pImageObject=NULL;	
}


void DoCopy3Dto2D(unsigned short int * twoDarray, unsigned short int * threeDarray, int num_pixels, int indexshift)	
{
	// copy sequence array values to image array 
	int ii;

	for (ii=0; ii<num_pixels; ii++ )		// run through pixels
		*(twoDarray + ii) =  *(threeDarray + ii + indexshift);	
		// set image array pixel to sequence array greylevel value 
}


void DoCopy2Dto3D(unsigned short int * twoDarray, unsigned short int * threeDarray, int num_pixels, int indexshift)	
{
	// copy sequence array values to image array 
	int ii;

	for (ii=0; ii<num_pixels; ii++ )		// run through pixels
		*(threeDarray + ii + indexshift) = *(twoDarray + ii);	
		// set image array pixel to sequence array greylevel value 
}


int ImageRegion(int curr_chan, int horpos, int verpos, int imtype)	
{
	// find region of current pixel in image array 
	int region = 0;		// default : NULL region
		
	if (imtype==1)		// filter output images
	{
	// region 1: inside frame determined by half-filter width
	if ((horpos>WIDF) && (horpos<sizx-WIDF-1) && (verpos>WIDF) && (verpos<sizy-WIDF-1))
		region = 1;		
	// region 2: outside frame determined by half-filter width
	if ((horpos<WIDF) || (horpos>sizx-WIDF-1) || (verpos<WIDF) || (verpos>sizy-WIDF-1))
		region = 2;		
	}

	if (imtype==2)		// EMD output images
	{
	int delt_pos1 = (int)(-delphi[curr_chan]/2.0);					// relative left/upper input position
	int delt_pos2 = delt_pos1 + ((int)(delphi[curr_chan]));		// relative right/lower input position
	// region 1: inside frame determined by half-filter width + half detector sampling
	if ((horpos>WIDF-delt_pos1+1) && (horpos<sizx-WIDF+delt_pos1-2) 
					&& (verpos>WIDF-delt_pos1+1) && (verpos<sizy-WIDF+delt_pos1-2))
		region = 1;		
	// region 2: outside frame determined by half-filter width + half detector sampling
	if ((horpos<WIDF-delt_pos1+1) || (horpos>sizx-WIDF+delt_pos1-2) 
					|| (verpos<WIDF-delt_pos1+1) || (verpos>sizy-WIDF+delt_pos1-2))
		region = 2;	
	}

	return (region);
}


void CMy2DMD_SIMLATEDlg::OnLoadinput() 
{
	// TODO: Add your control notification handler code here
	// load ascii greylevel image sequence and convert into set of input image maps
	// use separate function to load the stm files  !!!
	char out_str[80];
	int loopi, dumi;
	int num_pixels = 999;
	long seq_pixels = 0;	
	long indexshift = 0;
	unsigned short int * pGvalues;			// pointer to image greylevels (2D data array)
	char in_filnam[80];
	char biglin[50];

	CClientDC ClientDC( this );

	// open first ascii data file and get image size
	DoCheckinput();
	num_pixels = sizx*sizy;						// number of pixels in single image
	seq_pixels = num_pixels*(n_last-n_first+1);	// number of pixels in sequence

	//prepare memory space etc.
	if (gval_heap == 1)			
		{ gval_heap = 0;						// reset flag that heap space is not allocated
		  delete ppGvalues;	ppGvalues = 0; }	// free memory and reset pointer
	// prepare data arrays for greylevel sequence values 
	ppGvalues = new unsigned short int [seq_pixels];// allocate space for sequence on heap
	gval_heap = 1;									// set flag that heap space is allocated
	if ( ppGvalues==NULL )							// warning if array doesn't get requested heap space
		{ AfxMessageBox( "Could not create sequence heap space !" ); return; }

	sprintf( in_filnam, "stim/%s.char", in_name_str);
	if ( (fo=fopen(in_filnam,"rb")) == NULL )			// test whether ascii file can be opened 
		num_pixels=0;										// reset image size to zero if failed
	if ( num_pixels==0 )			
		{ AfxMessageBox( "cannot load ascii file (name?)!" ); return; }	// exit if input file is anavailable
	fgets( biglin, 33, fo );		// read file header (take sizex, sizey from externals...
	
	// if file is to be read from a frame other than the first, only partially load the frames!
	if(n_first>1)
	{
		for (int dd = 1; dd<n_first; dd++)
		{
			for(int jj=0; jj<sizy; jj++)
			{
				for(int ii=0; ii<sizx; ii++){ fgetc(fo);}
			}
		}
	} // understood June 2013

	// load the image data for the full sequence
	for (loopi=n_first; loopi<=n_last; loopi++)		// run through image sequence loop
		{
		BeginWaitCursor();

		// prepare data arrays for greylevel values 
		pGvalues = new unsigned short int [num_pixels];	// allocate space for image on heap
		if ( pGvalues==NULL )							// warning if array doesn't get requested heap space
			{ AfxMessageBox( "Could not create image heap space !" ); return; }

		indexshift = (loopi-n_first) * num_pixels;

		DoLoadinput(loopi, indexshift);					// load the data 
		DoCopy3Dto2D(pGvalues, ppGvalues, num_pixels, indexshift);	// copy sequence array values to image array 

		// create buffer for display image and transfer data from array into image buffer
		m_pImageObject = new CImageObject( "null.bmp" );	// by loading empty 8-bit greylevel bmp-file
		dumi = m_pImageObject->Stretch( sizx, sizy );		// resize to curent image size
		if ( (m_pImageObject==NULL) || (dumi == 0))			// warning if anything went wrong
			{ AfxMessageBox( "Could not create temporary image !" ); return; }	// when setting up display image
		CImagePointProcesses mImagePointProcess( m_pImageObject );  // make image available for point processing
		mImagePointProcess.Dump8bitData( pGvalues, sizx, sizy );	// convert greylevels in array on heap into image pixels

		// show image and report data load activity
		if ( (m_pImageObject==NULL) || (sizx==0) || (sizy==0) )
			{ m_pImageObject=NULL; AfxMessageBox( "Could not create image !" ); return; }
		else
			{ 
			RedrawWindow();			// erase previous image and messages
			m_pImageObject->Draw( &ClientDC, 10, 120 );
			sprintf( out_str, "%s.char frame %d loaded: %dx%d min|max %d|%d", 
						in_name_str, loopi, sizx,sizy, gmin,gmax );
			ClientDC.TextOut(20, 100, out_str);
			}	
	delete pGvalues;		pGvalues = 0;		// free memory and reset pointer
		
		}	// end running through image sequence loop
fclose(fo);
}


void DoCheckinput()
{	// open first ascii data file of sequence and determine image size
	FILE    *fi;
	char	in_filnam[80];       
	char    biglin[20];			/* input line string: maximum 1000 values */
	char    *smalin;				/* cut input line string */

	// open first ascii data file and get image size
	sprintf( in_filnam, "stim/%s.char", in_name_str );	// generate input filename 
	if ( (fi=fopen(in_filnam,"rb")) == NULL )			// test whether ascii file can be opened 
	{ AfxMessageBox( "cannot load ascii file (name?)!" ); return; } // and exit if not
	fgets( biglin, 20, fi );		// read file header
	sizx = atoi(biglin);			// convert number of columns
    smalin = (biglin+4);			// cut and ...
	sizy = atoi(smalin);			// ... convert number of rows
	fclose(fi);						// close ascii data file
// end of JMZ function:  DoCheckinput()			
}


void DoLoadinput(int loopi, int indexshift) 
{
	// load ascii greylevel data file and convert into input image map for display
	int ii, jj;
	//double dumy;	
	unsigned short int dumi;
	unsigned char outi;
	int revindexshift;

	if(direction == 1)		revindexshift = (n_last - loopi) * (sizx*sizy);
	
	// get & convert data from ascii file and enter into data array
	gmax = 0;				// reset maximum input intensity
	gmin = 255;				// reset minimum input intensity
	

	for (jj=0; jj<sizy; jj++ )			// run through sizy rows
		{
		for (ii=0; ii<sizx; ii++ )		// run through sizx columns
			{
			outi = fgetc(fo);
			dumi = int(outi );	// convert to 0-255 greylevel value // below-11 added as correction
			if(direction == 0)	*(ppGvalues + jj*sizx + (ii-11) + indexshift) = 3*dumi;	// set sequence array greylevel value 
			else				*(ppGvalues + jj*sizx + (ii-11) + revindexshift) = 3*dumi;
			}										// (greylevel assumed as sum of RGB)
			// above is a quick fixwith -11, not the solution to displacement problem
		}	/* end run through rows */

	// cleanup
// end of JMZ function:  DoLoadinput()			
}


void CMy2DMD_SIMLATEDlg::OnFilter() 
{
	// TODO: Add your control notification handler code here
	// filter ascii greylevel image sequence with spatial DOG filter
	char out_str[80];
	int loopi, dumi;
	long indexshift = 0;
	unsigned short int * pFvalues;			// pointer to filtered image values (2D data array)
	char out_filnam[100];

	// prepare filter and outut/display environmant
	double filt_max = MakeFilter(0,1.0, 3.0);	
	int num_pixels = sizx*sizy;							// number of pixels in single image
	long seq_pixels = num_pixels*(n_last-n_first+1);	// number of pixels in sequence
	CClientDC ClientDC( this );
	//prepare memory space etc.
	if ( ppGvalues==NULL )					// warning if filter array is empty
		{ AfxMessageBox( "Could not filter input (no input)!" ); return; }
	if (fval_heap == 1)			
		{ fval_heap = 0;						// reset flag that heap space is not allocated
		  delete ppFvalues;	ppFvalues = 0; }	// free memory and reset pointer
	// prepare data arrays for filtered sequence values 
	ppFvalues = new unsigned short int [seq_pixels];// allocate space for sequence on heap
	fval_heap = 1;									// set flag that heap space is allocated
	if ( ppFvalues==NULL )							// warning if array doesn't get requested heap space
		{ AfxMessageBox( "Could not create sequence heap space !" ); return; }

	// prepare and open asci data file 
	sprintf( out_filnam, "filt\\%s.cfil", in_name_str );	// generate output pattern filename 
	if ( (fo=fopen(out_filnam,"wb")) == NULL )		// test whether ascii file can be opened 
		{ AfxMessageBox( "cannot save ascii file (open?)!" ); return; }	// prevent save without open file

	// save header: size, name, scale
	fprintf(fo, "%4d %4d pattern : %s% ", sizx, sizy, out_filnam );	// save size in ascii file

	// process the image data for the full sequence
	for (loopi=n_first; loopi<=n_last; loopi++)		// run through image sequence loop
		{
		BeginWaitCursor();

		// prepare data arrays for greylevel values 
		pFvalues = new unsigned short int [num_pixels];	// allocate space for image on heap
		if ( pFvalues==NULL )							// warning if array doesn't get requested heap space
			{ AfxMessageBox( "Could not create filtered heap space !" ); return; }

		// filter image array, using 3D greylevel array as input
		DoFilterArray(0,sizx, sizy, loopi, filt_max);	
		// nonlinear processing of image array, if EMD preprocessing is selected
		if (prepro != 0) DoNonlinArray(sizx, sizy, loopi);	
		// save image array in ascii file
		SaveFilteredArray(0, sizx, sizy, loopi);	

		// copy filtered sequence array values to image array 
		indexshift = (loopi-n_first) * num_pixels;
		DoCopy3Dto2D(pFvalues, ppFvalues, num_pixels, indexshift);	// copy sequence array values to image array 
		
		// create buffer for display image and transfer data from array into image buffer
		m_pImageObject = new CImageObject( "null.bmp" );	// by loading empty 8-bit greylevel bmp-file
		dumi = m_pImageObject->Stretch( sizx, sizy );		// resize to curent image size
		if ( (m_pImageObject==NULL) || (dumi == 0))			// warning if anything went wrong
			{ AfxMessageBox( "Could not create temporary image !" ); return; }	// when setting up display image
		CImagePointProcesses mImagePointProcess( m_pImageObject );  // make image available for point processing
		mImagePointProcess.Dump8bitData( pFvalues, sizx, sizy );	// convert greylevels in array on heap into image pixels

		// show image and report data load activity
		if ( (m_pImageObject==NULL) || (sizx==0) || (sizy==0) )
			{ m_pImageObject=NULL; AfxMessageBox( "Could not create image !" ); return; }
		else
			{ 
			RedrawWindow();			// erase previous image and messages
			m_pImageObject->Draw( &ClientDC, 10, 120 );
			sprintf( out_str, "%s.char frame %d filtered: %dx%d min|max %d|%d", 
						in_name_str, loopi, sizx,sizy, fmin/3,fmax/3 );
			ClientDC.TextOut(20, 100, out_str);
			if ((fmin<0) || (fmax>GGG)) 
				ClientDC.TextOut(450, 100, "OUTSIDE GREYLEVEL RANGE !!!" );
			}	

		EndWaitCursor();
		delete pFvalues;		pFvalues = 0;		// free memory and reset pointer
		}	// end running through image sequence loop
	
	double std  = sigfac*delphi[0];   		// standard deviation of centre
	double sstd = surwid*std;			// standard deviation of surround

	fprintf(fo, "\nfiltered output of stim/%s%d.stm   ", in_name_str, loopi );	
	fprintf(fo, "circ.gauss std.dev cnt: %6.3f halfwidth %6.3f pix  ", std, GFC*std );
	fprintf(fo, "circ.gauss std.dev sur: %6.3f halfwidth %6.3f pix  ", sstd, GFC*sstd );
	fprintf(fo, "gain of centre: %f   ", gainctr );
	fprintf(fo, "gain factor of DOG output: %f  ", amplifi );	
	fprintf(fo, "EMD preprocessing: %d (0|1|2|3) none|HWR|FWR|SQR  ", prepro);	
										// save various comments in ascii file
	fclose(fo);							// close ascii file
}


void DoFilterArray(int curr_chan, int sizx, int sizy, int loopi, double filt_max)	
{
	// filter single image of sequence with 2D spatial filter in filter[][]
	int ii, jj, kk, ll;
	long lumi = 0;
	unsigned short sumi = 0;
	 
	int num_pixels = sizx*sizy;// number of pixels in single image
	int seq_pixels = num_pixels * (n_last-n_first+1); // take into account current channel
	long ppG_index = (loopi-n_first) * num_pixels; //index for input before filtering
	long indexshift = (loopi-n_first) * num_pixels
		+ seq_pixels*curr_chan;	// location of first pixel of current image
	int filmax = (int)(((double)SCAF) * filt_max);	// make amplified filter scale factor to allow for integer operation
	if (curr_chan==0){fmax = GGG/2;fmin = GGG/2;}						// reset max & min input intensity
										
	for (ii=0; ii<sizx; ii++ )		// run through columns
	for (jj=0; jj<sizy; jj++ )		// run through rows
		{
		*(ppFvalues + indexshift + ii + jj*sizx) = (unsigned short)GGG;	// set filtered value to max (white)
		if (ImageRegion(curr_chan,ii,jj,1)==2)		// outside boundary (half filter width) area 
			{													// outside boundary (half filter width) area
				*(ppFvalues + indexshift + ii + jj*sizx) = 0;	// set filtered value to min	
			if (ii>sizx/3)										// set filtered value to zero
				*(ppFvalues + indexshift + ii + jj*sizx) = (unsigned short)(GGG/2);		
			if (ii>2*sizx/3)									// set filtered value to max
				*(ppFvalues + indexshift + ii + jj*sizx) = (unsigned short)GGG;		
			}
		if (ImageRegion(curr_chan, ii,jj,1)==1)		// inside boundary (half filter width) area 
			{	
			lumi = 0;
			for (kk=-WIDF; kk<=WIDF; kk++ )		// run through columns
			for (ll=-WIDF; ll<=WIDF; ll++ )		// run through rows
				lumi += (*(ppGvalues + ppG_index + ii + kk + (jj+ll)*sizx) * filter[curr_chan][MIDF+kk][MIDF+ll]);
				// integrate input greylevels, multiplied with filter weights
			lumi /= SCAF;				// re-scale with normalised filter maximum (by definition SCAF)
			lumi *= filmax;				// scale with initial filter maximum (filt_max)
			lumi /= SCAF;				// re-scale with normalised filter maximum (by definition SCAF)

			lumi /= 2;					// compress range -SCAF..+SCAF to half
			lumi += GGG/2;				// and shift by GGG/2: new range 0|GGG/2|GGG << -SCAF|0|SCAF
			if (lumi>fmax) fmax = lumi;
			if (lumi<fmin) fmin = lumi;
			sumi = (unsigned short)lumi;						// convert to array type
			*(ppFvalues + indexshift + ii + jj*sizx) = sumi;	// set filtered image pixel value
			}
		}		// end run through columns & rows
// end of JMZ function:  DoFilterArray() New AM function - Multichannel filtering jun08			
}


void DoNonlinArray(int sizx, int sizy, int loopi)	
{
	// nonlinear processing of inner region of single image of sequence
	int ii, jj, dumi;
	unsigned short sumi;

	int num_pixels = sizx*sizy;						// number of pixels in single image
	long indexshift = (loopi-n_first) * num_pixels;	// location of first pixel of current image
	fmax = GGG/2;									// reset maximum input intensity
	fmin = GGG/2;									// reset minimum input intensity

	for (ii=0; ii<sizx; ii++ )			// run through columns
	for (jj=0; jj<sizy; jj++ )			// run through rows
		{
		if (ImageRegion(0,ii,jj,1)==1)	// inside boundary (half filter width) area 
			{							// pick filtered image pixel & shift: zero midpoint
			dumi = *(ppFvalues + indexshift + ii + jj*sizx) - GGG/2;	

			if (prepro==1) // 1=HWR  half wave rectification
				if (dumi<0) dumi = 0;
			if (prepro==2) // 2=FWR  full wave rectification
				dumi = abs(dumi);
			if (prepro==3) // 3=SQR  squaring
				dumi = dumi * dumi;

			sumi = (unsigned short) (dumi + GGG/2);
			if (sumi>fmax) fmax = sumi;	// find new maximum
			if (sumi<fmin) fmin = sumi;	// find new minimum
			*(ppFvalues + indexshift + ii + jj*sizx) = sumi;	// set nonlin processed image pixel value
			}
		}		// end run through columns & rows
// end of JMZ function:  DoNonlinArray()			
}


void SaveFilteredArray(int curr_chan, int sizx, int sizy, int loopi)	
{
	// save filtered (& nonlinear processed) single image of sequence 
	int ii, jj, dumi, outi;                

	int num_pixels = sizx*sizy;						// number of pixels in single image
	long indexshift = (loopi-n_first) * num_pixels;	// location of first pixel of current image

	// save in cfil files (in filt subdirectory)
	// save image data (pixel greylevel)
	for (jj=0; jj<sizy; jj++)			// run through rows of image
		{
		for (ii=0; ii<sizx; ii++)		// run through columns of row
			{							// pick current pixel's grey value
			dumi = *(ppFvalues + indexshift + jj*sizx + ii);	
			outi = (int)((double)(2*dumi-GGG) / 3.0 );	// scale to -10000...10000
			fprintf(fo, "%1c ", outi);	// save into ascii file
			}							// end of current row 
		}								// end of all rows of image



// end of JMZ function: SaveFilteredArray()			
}


double MakeFilter(int curr_chan, double sigma_centr, double sigma_surnd)	
{
	// copy sequence array values to image array 
	int kk, ll;
	double hdist, vdist, shdist, svdist;
	double fil2d[MM][MM];
	double dum1, dum2, dum3;
	double sum1, sum2, sum3;

	double std  = sigfac*delphi[curr_chan]; // standard deviation of centre ???
	double sstd = surwid*std;				// standard deviation of surround
	double facto = 1.0 / (std*std*2.0*pi);			/* normalizing for circular standard volume (2D) */
	double sfacto = 1.0 / (sstd*sstd*2.0*pi);		/* normalizing for standard volume */

 	for (kk=0; kk<MM; kk++ )		// initialise filter weight matrix
		for (ll=0; ll<MM; ll++ )	filter[curr_chan][kk][ll] = 0;

   /* and fill with normalised values */
    sum1 = sum2 = sum3 = 0.0;
	for (kk=0; kk<MM; kk++ )	
		for (ll=0; ll<MM; ll++ )	
			{
			hdist = ((double)abs(MIDF-kk)) / std;
			vdist = ((double)abs(MIDF-ll)) / std;
 			dum1 = facto * gainctr * exp( (hdist*hdist+vdist*vdist) / -2.0 );
			sum1 += dum1;			/* filter centre integral */
			shdist = ((double)abs(MIDF-kk)) / sstd;
			svdist = ((double)abs(MIDF-ll)) / sstd;
 			dum2 = sfacto * exp( (shdist*shdist+svdist*svdist) / -2.0 );
			sum2 += dum2;			/* filter suround integral */
			fil2d[kk][ll] = amplifi * (dum1 - dum2);
			sum3 += fil2d[kk][ll];	/* complete filter integral */
			}       /* make and convert data value */

	dum3 = fil2d[MIDF][MIDF];	// maximum filter weight in centre
	WIDF = 0;					// minimal radius of active filter area
	for (kk=0; kk<MM; kk++ )	
		for (ll=0; ll<MM; ll++ )	
			{					// normalise to maximum and convert to integers
			filter[curr_chan][kk][ll] = (int)(((double)SCAF) * fil2d[kk][ll] / dum3);
			if ((WIDF==0) && (filter[curr_chan][kk][ll]!=0)) WIDF = MIDF - kk;
			}					// determine radius of active filter area (weights > 0)
		
	// output of filter mask in text file
	FILE    *fo;					// for control printout ////////////////
	fo=fopen("filter.txt","w");
    fprintf(fo, "\ncirc.gauss std.dev cnt: %6.3f halfwidth %6.3f pix", std, GFC*std );
    fprintf(fo, "\ncirc.gauss std.dev sur: %6.3f halfwidth %6.3f pix ", sstd, GFC*sstd );
	for (kk=0; kk<MM; kk++ )
		{
		fprintf(fo, "\n");	
		for (ll=0; ll<MM; ll++ )	
			fprintf(fo, "%6d ", filter[curr_chan][kk][ll]);
		}
	fprintf(fo, "\nintegral centre %f   surround %f   DOG %f", sum1, sum2, sum3);	
	fprintf(fo, "\nscaled from max %f  to %d   >> return filter scale %f", 
				fil2d[MIDF][MIDF], filter[curr_chan][MIDF][MIDF], dum3);	
	long dumi = 0;							// initialise integral of filter weights
	for (kk=MIDF-WIDF; kk<=MIDF+WIDF; kk++ )	
		for (ll=MIDF-WIDF; ll<=MIDF+WIDF; ll++ )	
			dumi += filter[curr_chan][kk][ll];		// integrate weights within active area
	fprintf(fo, "\nactive filter radius %d : inside integral %d", WIDF, dumi);	
	fclose(fo);

	return(dum3); 
// end of JMZ function: MakeFilter()			
}


void CMy2DMD_SIMLATEDlg::OnSimulate() 
{
	// TODO: Add your control notification handler code here
	char out_str[80];
	int loopi, ii;
	long indexshift;
	char in_filnam[30];
	char biglin[100];
	channel = 4; // number of channels used in SaveMotionArrays
	
	clock_t start, finish;
	double duration;

	start = clock();

	if(m_in_status_load != _T("BLANK")){
		m_in_status_load = _T("...OLD DATA!");
		m_in_status_filt = _T("...OLD DATA!");
		m_in_status_mot = _T("...OLD DATA!");
		m_in_status_comb = _T("...OLD DATA!");
		UpdateData(FALSE);


	}
	
	CClientDC ClientDC( this );
	RedrawWindow();				// erase previous image and messages


	// open first ascii data file and get image size
	DoCheckinput();
	int num_pixels = sizx*sizy;						// number of pixels in single image
	long seq_pixels = num_pixels*(n_last-n_first+1);	// number of pixels in sequence

	// prepare data arrays for greylevel input sequence values 
	if (gval_heap == 1)			
		{ gval_heap = 0;						// reset flag that heap space is not allocated
		  delete ppGvalues;	ppGvalues = 0; }	// free memory and reset pointer
	ppGvalues = new unsigned short int [seq_pixels];// allocate space for sequence on heap
	gval_heap = 1;									// set flag that heap space is allocated
	if ( ppGvalues==NULL )							// warning if array doesn't get requested heap space
		{ AfxMessageBox( "Could not create sequence heap space for input!" ); return; }

	m_in_status_load = _T("...LOADING");
	UpdateData(FALSE);
	RedrawWindow();				// erase previous image and messages

	sprintf( in_filnam, "stim/%s.char", in_name_str);
		if ( (fo=fopen(in_filnam,"rb")) == NULL )			// test whether ascii file can be opened 
			num_pixels=0;										// reset image size to zero if failed
		if ( num_pixels==0 )			
			{ AfxMessageBox( "cannot load ascii file (name?)!" ); return; }	// exit if input file is anavailable
	fgets( biglin, 33, fo );		// read file header (take sizex, sizey from externals...

	if(n_first>1)
	{
		for (int dd = 1; dd<n_first; dd++)
		{
			for(int jj=0; jj<sizy; jj++){
				for(int ii=0; ii<sizx; ii++){
					fgetc(fo);
				}
			}
		}
	}

	// load the image data for the full sequence
	for (loopi=n_first; loopi<=n_last; loopi++)		// run through image sequence loop
		{
		BeginWaitCursor();
		if(direction == 0)		indexshift = (loopi-n_first) * num_pixels;
		else					indexshift = (n_last-loopi-n_first) * num_pixels;

		DoLoadinput(loopi, indexshift);					// load the data 

		// show image and report data load activity
		sprintf( out_str, "%s.char Frame %d loaded: %dx%d min|max %d|%d", 
						in_name_str, loopi, sizx,sizy, gmin,gmax );
		ClientDC.TextOut(20, 100, out_str);
		EndWaitCursor();
		}	// end running through image sequence loop
	fclose(fo);

	m_in_status_load = _T("LOADED");
	UpdateData(FALSE);

	m_in_status_filt = _T("...CALCULATING");
	UpdateData(FALSE);
	RedrawWindow();				// erase previous image and messages

	// filter ascii greylevel image sequence with spatial DOG filter: prepare filter stuff 
	double filt_max[chan_no]; //double array to store fliter maximum AM08 	
	for (ii=0; ii<chan_no; ii++)filt_max[ii] = MakeFilter(ii,1.0, 3.0); //generate max, make filters!!

	if (fval_heap == 1)			
		{ fval_heap = 0;						// reset flag that heap space is not allocated
		  delete ppFvalues;	ppFvalues = 0; }	// free memory and reset pointer
	// prepare data arrays for filtered sequence values 
	ppFvalues = new unsigned short int [seq_pixels*chan_no];// allocate space for sequence on heap
	fval_heap = 1;									// set flag that heap space is allocated
	if ( ppFvalues==NULL )							// warning if array doesn't get requested heap space
		{ AfxMessageBox( "Could not create sequence heap space !" ); return; }

	// process the image data for the full sequence (filtered/preprocessed arrays not saved)
	for (loopi=n_first; loopi<=n_last; loopi++)		// run through image sequence loop
		{
		BeginWaitCursor();
		// filter image array, using 3D greylevel array as input
		for (ii=0; ii<chan_no; ii++) // generate multichannel filters
		{DoFilterArray(ii, sizx, sizy, loopi, filt_max[ii]);} //AM08	
		// nonlinear processing of image array, if EMD preprocessing is selected
		if (prepro != 0) DoNonlinArray(sizx, sizy, loopi);	

		sprintf( out_str, "%s.char frame %d Multi-C filtered: %dx%d min|max %d|%d", 
						in_name_str, loopi, sizx,sizy, fmin/3,fmax/3 );
		ClientDC.TextOut(400, 100, out_str);
		if ((fmin<0) || (fmax>GGG)) 
				ClientDC.TextOut(400, 120, "OUTSIDE GREYLEVEL RANGE !!!" );
		EndWaitCursor();
		}	// end running through image sequence loop

		m_in_status_filt = _T("LOADED");
		UpdateData(FALSE);

		m_in_status_mot = _T("...CALCULATING");
		UpdateData(FALSE);

		RedrawWindow();				// erase previous image and messages

	// generate & display motion detector responses
	BeginWaitCursor();
	// prepare memory space etc. for horizontal and vertical response
	int num_frams = (n_last-n_first+1);					// number of frames in sequence

	if ( ppFvalues==NULL )					// warning if filter array is empty
		{ AfxMessageBox( "Could not calculate response (no filter output)!" ); return; }
	
	if(hval_heap == 1)
	{
		hval_heap = 0;
		delete ppHvalues;	ppHvalues = 0;
	}
	ppHvalues = new double [seq_pixels*(chan_no+2)];	// allocate space for horizontal response on heap
	hval_heap = 1;
	if ( ppHvalues==NULL )					// warning if array doesn't get requested heap space
		{ AfxMessageBox( "Could not create hor.resp heap space !" ); return; }
	//the +2 above and below takes into account the combined channels AM Oct08

	if(vval_heap == 1)
	{
		vval_heap = 0;
		delete ppVvalues;	ppVvalues = 0;
	}
	ppVvalues = new double [seq_pixels*(chan_no+2)];	// allocate space for vertical response on heap
	vval_heap = 1;
	if ( ppVvalues==NULL )					// warning if array doesn't get requested heap space
		{ AfxMessageBox( "Could not create ver.resp heap space !" ); return; }
	
	//Pointers ppH, ppV & ppF multiplied by channel number for multichannel sims!!!
	// generate x-y-t motion detector response: *ppFvalues >>> *ppHvalues,*ppVvalues
	
	for (ii=0; ii<chan_no; ii++) // generate multichannel responses
	{DoMotionArrays(ii, sizx, sizy, num_frams);} //AM08

	double resph, respv;
	for( ii=0; ii<(chan_no+2); ii++) // plus 2 for combo channels AM 2009
	{
		resp_max[ii][0] = 0.0;
		resp_max[ii][1] = 0.0;
		
		for (int kk=0; kk<seq_pixels; kk++)
		{
			resph = *(ppHvalues + kk + (ii*seq_pixels));
			if( (resph < 1.0) && (resph > -1.0) && (resph > resp_max[ii][0]) )	resp_max[ii][0] = abs(resph);
		
			respv = *(ppVvalues + kk + (ii*seq_pixels));
			if( (respv < 1.0) && (respv > -1.0) && (respv > resp_max[ii][1]) )	resp_max[ii][1] = abs(respv);
		}
	}										// find response maximum
	
	// save response values in ascii files (in resp subdirectory)
	SaveMotionArrays(sizx, sizy);

	m_in_status_mot = _T("LOADED");
	UpdateData(FALSE);
	RedrawWindow();				// erase previous image and messages
	
	finish = clock();
	duration = (double)(finish - start) / CLOCKS_PER_SEC; 
	sprintf(out_str, "Done. Filter and motion response took %2.1f seconds to calculate", duration);
	ClientDC.TextOut(400, 100, out_str);

	EndWaitCursor();
}


void CMy2DMD_SIMLATEDlg::OnMotion() 
{
	// TODO: Add your control notification handler code here
	// generate motion detector response
	int nfram, dumi;
	int reg_pixels;
	double dumy, tempi;
	double avgHor = 0.0;
	double avgVer = 0.0;
	double avgLen = 0.0;
	int ii, jj, rpos;
	long indexshift;
	channel = 1;
	char out_filnam[100];
	BOOL success = 0;

	FILE *avg;

	BeginWaitCursor();

	// prepare memory space etc. for horizontal and vertical response
	int num_frams = (n_last-n_first+1);					// number of frames in sequence
	int num_pixels = sizx*sizy;							// number of pixels in single image
	long seq_pixels = num_frams * num_pixels;			// number of pixels in sequence
	reg_pixels = (end_x-start_x+1)*(end_y-start_y+1);

	if ( ppFvalues==NULL )					// warning if filter array is empty
		{ AfxMessageBox( "Could not calculate response (no filter output)!" ); return; }
	ppHvalues = new double [seq_pixels];	// allocate space for horizontal response on heap
	if ( ppHvalues==NULL )					// warning if array doesn't get requested heap space
		{ AfxMessageBox( "Could not create hor.resp heap space !" ); return; }
	ppVvalues = new double [seq_pixels];	// allocate space for vertical response on heap
	if ( ppVvalues==NULL )					// warning if array doesn't get requested heap space
		{ AfxMessageBox( "Could not create hor.resp heap space !" ); return; }

	sprintf( out_filnam, "resp\\averages.txt" );
	if( (avg=fopen(out_filnam,"w")) == NULL )
	{ AfxMessageBox( "cannot save averages file!" ); return; }

	fprintf(avg, "Region of interest starting: %4d |%4d ending: %4d |%4d \n", start_x, start_y, end_x, end_y);
	fprintf(avg, "Values shown below are: Frame num, hor average, ver average, average length \n");

	// generate x-y-t motion detector response: *ppFvalues >>> *ppHvalues,*ppVvalues
	DoMotionArrays(0, sizx, sizy, num_frams);

	// display response in 2D colour code
	// prepare data array for horizontal response values 
	unsigned short int * pHvalues;					// pointer to image response values (data array)
	pHvalues = new unsigned short int [num_pixels];	// allocate space for image on heap
	if ( pHvalues==NULL )							// warning if array doesn't get requested heap space
		{ AfxMessageBox( "Could not create heap space !" ); return; }
	unsigned short int * pVvalues;					// pointer to image response values (data array)
	pVvalues = new unsigned short int [num_pixels];	// allocate space for image on heap
	if ( pVvalues==NULL )							// warning if array doesn't get requested heap space
		{ AfxMessageBox( "Could not create heap space !" ); return; }

	// create buffer for display image and transfer data from array into image buffer
	n_pImageObject = new CImageObject( "col_24bit.bmp" );	// by loading empty 24-bit colour bmp-file
	dumi = n_pImageObject->Stretch( sizx, sizy );			// resize to curent image size
	if ( (n_pImageObject==NULL) || (dumi == 0))				// warning if anything went wrong
		{ AfxMessageBox( "Could not create temporary image !" ); return; }	// when setting up display image
	CImagePointProcesses mImagePointProcess( n_pImageObject );  // make image available for point processing
	CClientDC ClientDC( this );
	RedrawWindow();				// erase previous image and messages

    int hmax = 0;				// initialise horizontal plot maximum
	int hmin = 1023;			// initialise horizontal plot minimum
    int vmax = 0;				// initialise vertical plot maximum
	int vmin = 1023;			// initialise vertical plot minimum
	char out_str[80];

	double resph, respv;
	double resp_hmax = 0.0;
	double resp_vmax = 0.0;
		
	for (int kk=0; kk<seq_pixels; kk++)
	{
		resph = *(ppHvalues + kk);
		if( (resph < 1.0) && (resph > -1.0) && (resph > resp_hmax) )	resp_hmax = abs(resph);
		
		respv = *(ppVvalues + kk);
		if( (respv < 1.0) && (respv > -1.0) && (respv > resp_vmax) )	resp_vmax = abs(respv);
	}

	for (nfram=n_first; nfram<=n_last; nfram++ )	// run through image sequence
		{
		indexshift = (nfram-n_first) * num_pixels;		// shift of base index of current frame

		// get & convert response values from 3D data arrays and enter into 2D display array
		for (jj=0; jj<sizy; jj++ )			// run through sizy rows
			{
			for (ii=0; ii<sizx; ii++ )		// run through sizx columns
				{
					rpos = ImageRegion(0,ii,jj,2);			// find image region of current pixel
					// horizontal response component
					dumy = *(ppHvalues + indexshift + jj*sizx + ii);	// grab current response value	

					// code to normalise to maximum horizontal response value if flag set to 1
					if( (resp_hmax != 0) && (rpos == 1) && (max_norm == 1) )	dumy = dumy/resp_hmax;	
	
					dumi = int( 511.0 * (1.0+dumy) );		// convert to 0-511-1024 ver.resp. value
					if (rpos==1)		// restrict min/max search to area inside boundary  
					{ if (dumi>hmax) hmax = dumi;
					if (dumi<hmin) hmin = dumi;
					if (dumi < 0)	 dumi = 0;				// exclude negative levels after shifting range
					if (dumi > 1023) dumi = 1023; }			// cap at 1023

					*(pHvalues + jj*sizx + ii) = dumi;		// set array vertical response value 	
					if((jj>=start_y) && (jj<=end_y) && (ii>=start_x) && (ii<=end_x))
					{
						avgHor += dumi;
						tempi = dumi;
					}

					// vertical response component
					dumy = *(ppVvalues + indexshift + jj*sizx + ii);	// grab current response value	

					// code to normalise to maximum vertical response value if flag set to 1
					if( (resp_vmax !=0) && (rpos == 1) && (max_norm == 1) )		dumy = dumy/resp_vmax;
	
					dumi = int( 511.0 * (1.0+dumy) );		// convert to 0-511-1024 ver.resp. value
					if (rpos==1)		// restrict min/max search to area inside boundary  
					{ if (dumi>vmax) vmax = dumi;
					if (dumi<vmin) vmin = dumi;
					if (dumi < 0)	 dumi = 0;				// exclude negative levels after shifting range
					if (dumi > 1023) dumi = 1023;}			// cap at 1023
					
					*(pVvalues + jj*sizx + ii) = dumi;		// set array vertical response value 
					if((jj>=start_y) && (jj<=end_y) && (ii>=start_x) && (ii<=end_x))
					{
						avgVer += dumi;
						tempi = sqrt((tempi*tempi)+(dumi*dumi));
						avgLen += tempi;
					}
				}				
			}	/* end run through rows */

		avgHor = avgHor/reg_pixels;
		avgVer = avgVer/reg_pixels;
		avgLen = avgLen/reg_pixels;
		fprintf(avg, "%4d    %4f    %4f    %4f \n", nfram, avgHor, avgVer, avgLen);		

		// convert hor and ver values in 2 arrays on heap into response map pixels
		mImagePointProcess.Dump24bitData( pHvalues, pVvalues, sizx, sizy );	

		// check whether image is available, display and report data load activity
		if ( (n_pImageObject==NULL) || (sizx==0) || (sizy==0) )
			{ n_pImageObject=NULL; AfxMessageBox( "Could not create/display image!" ); return; }
		else
			{ 
			n_pImageObject->Draw( &ClientDC, 10, 120 );
			sprintf( out_str, "resp/%s.chah&chav frame %d: %dx%d min|max %d|%d & %d|%d      ", 
							in_name_str, nfram, sizx,sizy, hmin/4, hmax/4, vmin/4, vmax/4 );
			ClientDC.TextOut(20, 100, out_str);
			if ((hmin<0) || (hmax>1023)) 
				ClientDC.TextOut(450, 100, "OUTSIDE COLOUR RANGE !!!" );
			if( nfram == n_last )
			{
				if( max_norm == 0 )		sprintf( out_filnam, "btmp\\%s.bmp", in_name_str );
				if( max_norm == 1 )		sprintf( out_filnam, "btmp\\%s_norm.bmp", in_name_str );
				success = n_pImageObject->Save(out_filnam, -1);
				sprintf( out_str, "saved as: btmp\\%s.bmp %d x %d : %d",
							in_name_str, sizx, sizy, success);
				ClientDC.TextOut(450, 110, out_str);
			}
			Sleep(DELAY);
			}
	
		}				// end run through image sequence

	// save response values in ascii files (in resp subdirectory)
	SaveMotionArrays(sizx, sizy);

	// cleanup dislay stuff, motion signal data arrays
	delete pHvalues;		pHvalues = 0;		// free memory and reset pointer
	delete pVvalues;		pVvalues = 0;		// free memory and reset pointer
	delete ppHvalues;	ppHvalues = 0;	// free memory and reset pointer
	delete ppVvalues;	ppVvalues = 0;	// free memory and reset pointer
	fclose(avg);

	EndWaitCursor();
}


void DoMotionArrays(int curr_chan, int sizx, int sizy, int num_frams)	
{	// generate x-y-t motion detector response from filered/reprocessed input *ppFvalues >>> *ppHvalues,*ppVvalues
	int nfram, nstep, dumi;
	double dumy;
	int ii, jj;
	int xpos, ypos, rpos;
	long indexshift;
	double LhalfDet, RhalfDet, ThalfDet, BhalfDet;
	double * LeftIn, * RigtIn, * ToppIn, * BottIn;		// timecourse input signals
	double * LeftDl, * RigtDl, * ToppDl, * BottDl;		// timecourse temporally lowpassed signals
	double * HorOut, * VerOut;							// timecourse EMD output

	double dGGG = (double)GGG;							// greylevel array scale factor
	int num_steps = num_frams * FRAMFACT;				// number of timesteps in timecourse
	int num_pixels = sizx*sizy;							// number of pixels in single image
	int seq_pixels = num_pixels*(n_last-n_first+1); // responses per channel
	//indexshift = nfram * num_pixels * seq_pixels * curr_chan; //index for pointers
	//double scal = 10000.0/((double)GGG);				// scale factor: greyvalues 0...GGG >> 0...10000
	double tau_corct = timcon[curr_chan] * ((double)FRAMFACT);	//!!! correct timeconst: frames > simulation steps
	int delt_pos1 = (int)(-delphi[curr_chan]/2.0);					// relative left/upper input position
	int delt_pos2 = delt_pos1 + ((int)(delphi[curr_chan]));		// relative right/lower input position

	// prepare array space on heap for various time series
	LeftIn = new double [num_steps];			// allocate space for L input time series 
	LeftDl = new double [num_steps];			// allocate space for L delayed time series
	RigtIn = new double [num_steps];			// allocate space for R input time series 
	RigtDl = new double [num_steps];			// allocate space for R delayed time series 
	HorOut = new double [num_steps];			// allocate space for H response time series 

	ToppIn = new double [num_steps];			// allocate space for T input time series 
	ToppDl = new double [num_steps];			// allocate space for T delayed time series
	BottIn = new double [num_steps];			// allocate space for B input time series 
	BottDl = new double [num_steps];			// allocate space for B delayed time series 
	VerOut = new double [num_steps];			// allocate space for V response time series 

	// run through pixels and frames to do the motion calculations
	for (ii=0; ii<sizx; ii++ )		// run through columns
	for (jj=0; jj<sizy; jj++ )		// run through rows
	{
	// initialise pixel: run through image sequence and set this location to ZERO
	for (nfram=0; nfram<num_frams; nfram++ )
		{ indexshift = nfram * num_pixels + seq_pixels * curr_chan;	// shift of base index of current frame
		*(ppHvalues + indexshift + jj*sizx+ii) = 0.0;	// set horizontal response to zero
		*(ppVvalues + indexshift + jj*sizx+ii) = 0.0; }	// set vertical response to zero
	rpos = ImageRegion(curr_chan,ii,jj,2);						// find image region of current pixel

	// set pixels in ill-defined border region to values reflecting 2DMD colour code
	if (rpos==2)		// outside boundary (half filter width + half detector sampling) area 
		{	
		for (nfram=0; nfram<num_frams; nfram++ )	//run through image sequence
			{
			indexshift = nfram * num_pixels + seq_pixels * curr_chan;// shift of base index of current frame
			if (ii<sizx/3)									// set horizontal response to min at left
				*(ppHvalues + indexshift + ii + jj*sizx) = -1.0;		
			if (ii>2*sizx/3)								// set horizontal response to max at right
				*(ppHvalues + indexshift + ii + jj*sizx) = 1.0;		
			if (jj<sizy/3)									// set vertical response to max at top
				*(ppVvalues + indexshift + ii + jj*sizx) = 1.0;		
			if (jj>2*sizy/3)								// set vertical response to min at bottom
				*(ppVvalues + indexshift + ii + jj*sizx) = -1.0;		
			}
		}	// end outside boudary area

	// calculate response for central area, exclude ill-defined boundary regions
	if (rpos==1)		// inside boundary (half filter width + half detector sampling) area 
		{
		// generate four input sequences from filtered image sequence 
		for (nstep=0; nstep<num_steps; nstep++ )	//run through simulation steps
			{
			nfram = nstep/FRAMFACT;					// current frame index
			indexshift = nfram * num_pixels + seq_pixels * curr_chan;// shift of base index of current frame
			xpos = ii+delt_pos1; ypos = jj;
			dumi = *(ppFvalues + indexshift + ypos*sizx+xpos);	// pick Left input value from 3D array	
			*(LeftIn + nstep) = (((double)(2*dumi))-dGGG)/dGGG;	// and convert to values -1.0...1.0	
			xpos = ii+delt_pos2; ypos = jj;
			dumi = *(ppFvalues + indexshift + ypos*sizx+xpos);	// pick Right input value from 3D array	
			*(RigtIn + nstep) = (((double)(2*dumi))-dGGG)/dGGG;	// and convert to values -1.0...1.0	

			xpos = ii; ypos = jj+delt_pos1;
			dumi = *(ppFvalues + indexshift + ypos*sizx+xpos);	// pick Top input value from 3D array	
			*(ToppIn + nstep) = (((double)(2*dumi))-dGGG)/dGGG;	// and convert to values -1.0...1.0	
			xpos = ii; ypos = jj+delt_pos2;
			dumi = *(ppFvalues + indexshift + ypos*sizx+xpos);	// pick Bottom input value from 3D array	
			*(BottIn + nstep) = (((double)(2*dumi))-dGGG)/dGGG;	// and convert to values -1.0...1.0	
			}

		// lowpass filter 4 input time series
		DoLowPass(LeftIn, LeftDl, num_steps, tau_corct);	
		DoLowPass(RigtIn, RigtDl, num_steps, tau_corct);	
		DoLowPass(ToppIn, ToppDl, num_steps, tau_corct);	
		DoLowPass(BottIn, BottDl, num_steps, tau_corct);	

		// multiply two create output of horizontal and vertical local EMD
		for (nstep=0; nstep<num_steps; nstep++ )	//run through simulation steps
			{
			LhalfDet = (*(LeftDl + nstep)) * (*(RigtIn + nstep));
			RhalfDet = (*(RigtDl + nstep)) * (*(LeftIn + nstep));
			ThalfDet = (*(ToppDl + nstep)) * (*(BottIn + nstep));
			BhalfDet = (*(BottDl + nstep)) * (*(ToppIn + nstep));
			*(HorOut + nstep) = outgain * (LhalfDet - balanc * RhalfDet);
			*(VerOut + nstep) = outgain * (BhalfDet - balanc * ThalfDet);
			}

		// generate frame averages from 2 output sequences and store in 2 response arrays
		double resp_avrg = 0.0;
		for (nfram=0; nfram<num_frams; nfram++ )	//run through image sequence
			{
			indexshift = nfram * num_pixels + seq_pixels * curr_chan;		// shift of base index of current frame
			dumy = 0.0;								// initialise sum of response steps
			// run through FRAMFACT simulation steps between nfram and nfram+1
			for (nstep=nfram*FRAMFACT; nstep<(nfram+1)*FRAMFACT; nstep++ )
				dumy += *(HorOut + nstep);			// increment sum of response steps
			resp_avrg = dumy / ((double)FRAMFACT);	// convert to horizontal average response
			*(ppHvalues + indexshift + jj*sizx+ii) = resp_avrg;	
													// store current horizontal response value in 3D array on heap
			dumy = 0.0;								// initialise sum of response steps
			// run through FRAMFACT simulation steps between nfram and nfram+1
			for (nstep=nfram*FRAMFACT; nstep<(nfram+1)*FRAMFACT; nstep++ )
				dumy += *(VerOut + nstep);			// increment sum of response steps
			resp_avrg = dumy / ((double)FRAMFACT);	// convert to horizontal average response
			*(ppVvalues + indexshift + jj*sizx+ii) = resp_avrg;	
			}										// store current horizontal response value in 3D array on heap
		}	// end inside boudary area
	}	// end run through columns and rows of image	

	// cleanup  array space on heap for time series
	delete LeftIn;		LeftIn = 0;		// free memory and reset pointer
	delete LeftDl;		LeftDl = 0;		// free memory and reset pointer
	delete RigtIn;		RigtIn = 0;		// free memory and reset pointer
	delete RigtDl;		RigtDl = 0;		// free memory and reset pointer
	delete HorOut;		HorOut = 0;		// free memory and reset pointer

	delete ToppIn;		ToppIn = 0;		// free memory and reset pointer
	delete ToppDl;		ToppDl = 0;		// free memory and reset pointer
	delete BottIn;		BottIn = 0;		// free memory and reset pointer
	delete BottDl;		BottDl = 0;		// free memory and reset pointer
	delete VerOut;		VerOut = 0;		// free memory and reset pointer
// end of JMZ function: DoMotionArrays()			
}


void DoLowPass(double * InCourse, double * OutCourse, int num_steps, double tau_corct)	
{
	// temporal low-pass filtering of InCourse with tau_corct >>> result : OutCourse
	int ii;
	double InPrev = *InCourse;		// initialise previous input value
	double OutPrev = 0.0;			// initialise previous output value
	double InCurr = *InCourse;		// initialise current input value
	double OutCurr = 0.0;			// initialise current output value

	for (ii=0; ii<num_steps; ii++ )		// run through simulation steps
		{								// differentiation quotient lowpassing
		InCurr = *(InCourse + ii);
		OutCurr = OutPrev + (InPrev-OutPrev) / tau_corct;
		*(OutCourse + ii) =  OutCurr;
		OutPrev = OutCurr;
		InPrev = InCurr;
		}
// end of JMZ function: DoLowPass()			
}


void SaveMotionArrays(int sizx, int sizy)	
{
	// save sequence of motion signal maps, as hor and ver ascii files
	int ii, jj, kk, nfram, outi;
	double dumy;
	long indexshift;
	

	char out_filnam[100];                
	double scal = 9999.0;	// scale factor: response -1.0...1.0 >> convert to -10000...10000
	int num_pixels = sizx*sizy;						// number of pixels in single image
	int seq_pixels = num_pixels * (n_last - n_first + 1);

	for(kk=0; kk<channel; kk++)
	{
	// prepare and open 2 ascii data files 
		FILE *foh, *fov;	
		if(channel == 2){
			if( direction == 1 )		sprintf( out_filnam, "resp\\%s_rev_combo%d.chah", in_name_str, kk);
			else						sprintf( out_filnam, "resp\\%s_combo%d.chah", in_name_str, kk);
		}
		else{
			if( direction == 1 )		sprintf( out_filnam, "resp\\%s_rev_%d.chah", in_name_str, kk );	// generate output pattern filename 
			else						sprintf( out_filnam, "resp\\%s_%d.chah", in_name_str, kk );	// generate output pattern filename 
		}
		if ( (foh=fopen(out_filnam,"wb")) == NULL )		// test whether ascii file can be opened 
			{ AfxMessageBox( "cannot save chah ascii file (open?)!" ); return; }	// prevent save without open file
		
			
		if(channel ==2){
			if( direction == 1 )		sprintf( out_filnam, "resp\\%s_rev_combo%d.chav", in_name_str, kk);
			else						sprintf( out_filnam, "resp\\%s_combo%d.chav", in_name_str, kk);
		}
		else{
			if( direction == 1 )		sprintf( out_filnam, "resp\\%s_rev_%d.chav", in_name_str, kk );	// generate output pattern filename 
			else						sprintf( out_filnam, "resp\\%s_%d.chav", in_name_str, kk );	// generate output pattern filename 
		}	
		if ( (fov=fopen(out_filnam,"wb")) == NULL )		// test whether ascii file can be opened 
			{ AfxMessageBox( "cannot save chav ascii file (open?)!" ); return; }	// prevent save without open file
		// save header: size, name, scale
		fprintf(foh, "%4d %4d pattern : %s%   ", sizx, sizy, out_filnam );	// save size in ascii file	
		fprintf(fov, "%4d %4d pattern : %s%   ", sizx, sizy, out_filnam );	// save size in ascii file
	


		// save response values in ascii files (in resp subdirectory)
	for (nfram=n_first; nfram<=n_last; nfram++ )	// run through image sequence
		{
			if(channel == 2){
			indexshift = ((nfram-n_first) * num_pixels) + (seq_pixels * (kk+4));		// location of first pixel of current image
			}
			else{
			indexshift = ((nfram-n_first) * num_pixels) + (seq_pixels * kk);		// location of first pixel of current image
			}
		// save image data (pixel greylevel)
		for (jj=0; jj<sizy; jj++)			// run through rows of image
			{
			for (ii=0; ii<sizx; ii++)		// run through columns of row
				{							// pick current pixel's horizontal response value
				// horizontal response component
				dumy = *(ppHvalues + indexshift + jj*sizx + ii);
				dumy += 1.0;
				dumy = dumy / 2.0;
				outi = (int)(dumy * 255.0);	// scale to -10000...10000
				fprintf(foh, "%1c", outi);	// save into ascii file
				// vertical response component
				dumy = *(ppVvalues + indexshift + jj*sizx + ii);	
				dumy += 1.0;
				dumy = dumy / 2.0;
				outi = (int)(dumy * 255.0);	// scale to -10000...10000
				fprintf(fov, "%1c", outi);	// save into ascii file
				}							// end of current row 
			}								// end of all rows of image
		}				// end run through image sequence
		// save various comments in hor ascii file
		fprintf(foh, "horizontal response of frame %d stim/%s.char   ", nfram, in_name_str );	
		fprintf(foh, "EMD sampling distance: %f pix  ", delphi[0]);
		fprintf(foh, "EMD time constant: %f frames  ", timcon[0]);
		fprintf(foh, "EMD balance factor: %f  ", balanc);
		fprintf(foh, "EMD ouput gain factor: %f  ", outgain );	
											// save various comments in ver ascii file
		fprintf(fov, "vertical response of frame %d stim/%s.char   ", nfram, in_name_str );	
		fprintf(fov, "EMD sampling distance: %f pix  ", delphi[0]);
		fprintf(fov, "EMD time constant: %f frames  ", timcon[0]);
		fprintf(fov, "EMD balance factor: %f  ", balanc);
		fprintf(fov, "EMD ouput gain factor: %f  ", outgain );	
											
		fclose(foh);						// close ascii file
		fclose(fov);						// close ascii file
	}
// end of JMZ function: SaveMotionArrays()			
}



void CMy2DMD_SIMLATEDlg::OnDisplay()
{
	// TODO: Add your control notification handler code here
	// this bit of code also does the addition across the detector channels... AM May2011
	char out_str[80];
	int loopi, ii, jj, kk, ll,mm, rpos;
	int nfram, dumi;
	double dumy, dumv, dumh, edgeh, edgev;
	long indexshift;
	char in_filnam[30];
	char biglin[100];
	char 	out_filnam[100];
	
	channel = 2; // set value for SaveMotionArrays to save combinations 2 equals save, 1 is no
	BOOL success = 0;		// to check that image has been saved as bmp successfully

	m_in_status_comb = _T("...CALCULATING");
	UpdateData(FALSE);
	RedrawWindow();

	CClientDC ClientDC( this );

	int num_pixels = sizx*sizy;						// number of pixels in single image
	long seq_pixels = num_pixels*(n_last-n_first+1);	// number of pixels in sequence

	unsigned short int * pGvalues;	// pointer to image input values (AM Oct08)	
	pGvalues = new unsigned short int [num_pixels];	

	// display response in 2D colour code
	// prepare data array for horizontal response values 
	unsigned short int * pHvalues;					// pointer to image response values (data array)
	unsigned short int * disp_pH;					// single frame, single chan response values 
	disp_pH = new unsigned short int [num_pixels];
	pHvalues = new unsigned short int [num_pixels*(chan_no+2)];	// allocate space for image on heap
	//the +2 above and below takes into account the combined channels AM Oct08
	if ( pHvalues==NULL )							// warning if array doesn't get requested heap space
		{ AfxMessageBox( "Could not create heap space !" ); return; }
	unsigned short int * pVvalues;					// pointer to image response values (data array)
	unsigned short int * disp_pV;					// single frame, single chan response values 
	disp_pV = new unsigned short int [num_pixels];
	pVvalues = new unsigned short int [num_pixels*(chan_no+2)];	// allocate space for image on heap
	if ( pVvalues==NULL )							// warning if array doesn't get requested heap space
		{ AfxMessageBox( "Could not create heap space !" ); return; }

	// create buffer for display image and transfer data from array into image buffer
	n_pImageObject = new CImageObject( "col_24bit.bmp" );	// by loading empty 24-bit colour bmp-file
	dumi = n_pImageObject->Stretch( sizx, sizy );			// resize to curent image size
	if ( (n_pImageObject==NULL) || (dumi == 0))				// warning if anything went wrong
		{ AfxMessageBox( "Could not create temporary image !" ); return; }	// when setting up display image
	CImagePointProcesses mImagePointProcess( n_pImageObject );  // make image available for point processing

	m_pImageObject = new CImageObject( "null.bmp" );// buffer for greyscales AM Oct08	
	dumi = m_pImageObject->Stretch( sizx, sizy );		
	if ( (m_pImageObject==NULL) || (dumi == 0))
	{ AfxMessageBox( "Could not create temporary image !" ); return; }
	CImagePointProcesses nImagePointProcess( m_pImageObject );  


    int hmax = 0;				// initialise horizontal plot maximum
	int hmin = 1023;			// initialise horizontal plot minimum
    int vmax = 0;				// initialise vertical plot maximum
	int vmin = 1023;			// initialise vertical plot minimum

	for (nfram=n_first; nfram<=n_last; nfram++ )	// run through image sequence
		{
		indexshift = (nfram-n_first) * num_pixels;		// shift of base index of current frame

		// get & convert response values from 3D data arrays and enter into 2D display array
		for (jj=0; jj<sizy; jj++ )			// run through sizy rows
			{
			for (ii=0; ii<sizx; ii++ )		// run through sizx columns
				{
				
				for (kk=0; kk<chan_no; kk++)  // kk is the channel index for (Multi-C)
				{
					rpos = ImageRegion(kk,ii,jj,2);			// find image region of current pixel
					// horizontal response component
					dumy = *(ppHvalues + indexshift + kk*seq_pixels + jj*sizx + ii);	// grab current response value	
					if((resp_max[kk][0]!= 0) && (rpos == 1)  && (max_norm == 1)) 
					{
						dumy = dumy/resp_max[kk][0];
						*(ppHvalues + indexshift + kk*seq_pixels + jj*sizx + ii) = dumy;
					}
					//else dumy = 0; // AS Dec08 -> normalise current value to max response

					dumi = int( 511.0 * (1.0+dumy) );		// convert to 0-511-1024 ver.resp. value
					if (rpos==1)		// restrict min/max search to area inside boundary  
					{ if (dumi>hmax) hmax = dumi;
					if (dumi<hmin) hmin = dumi;
					if (dumi < 0)	 dumi = 0;				// exclude negative levels after shifting range
					if (dumi > 1023) dumi = 1023; }			// cap at 1023
					*(pHvalues + kk*num_pixels + jj*sizx + ii) = dumi;		// set array vertical response value 

					// vertical response component
					dumy = *(ppVvalues + indexshift + kk*seq_pixels + jj*sizx + ii);	// grab current response value	
					if((resp_max[kk][1]!= 0) && (rpos == 1) && (max_norm == 1)) 
					{
						dumy = dumy/resp_max[kk][1];
						*(ppVvalues + indexshift + kk*seq_pixels + jj*sizx + ii) = dumy;
					}
					//else dumy = 0; // AS Dec08 -> normalise current value to max response

					dumi = int( 511.0 * (1.0+dumy) );		// convert to 0-511-1024 ver.resp. value
					if (rpos==1)		// restrict min/max search to area inside boundary  
					{ if (dumi>vmax) vmax = dumi;
					if (dumi<vmin) vmin = dumi;
					if (dumi < 0)	 dumi = 0;				// exclude negative levels after shifting range
					if (dumi > 1023) dumi = 1023;}			// cap at 1023, exclude big positives
					*(pVvalues + kk*num_pixels + jj*sizx + ii) = dumi;		// set array vertical response value 
				
					//extra code for multichannel combination Am Oct08
					if (kk==(chan_no-1) && rpos==1)	//When all the channels have been populated
					{
						dumh = 0; dumv = 0;
						for (mm=0;mm<chan_no;mm++)//loop actually performing combination
						{	//(SIMPLE ADDITIVE COMBINATION)
							dumh +=  *(ppHvalues + indexshift + mm*seq_pixels + jj*sizx + ii);	
							dumv +=  *(ppVvalues + indexshift + mm*seq_pixels + jj*sizx + ii);
						}
						dumi = int( 511.0 * (1.0+dumh) ); // no longer an average AM OCT08
						*(ppHvalues + indexshift + (kk+1)*seq_pixels + jj*sizx + ii)= dumh;
						//Above line stores added horiz response for saving AMOct08
						*(pHvalues + (kk+1)*num_pixels + jj*sizx + ii) = dumi; //horiz- for display
						
						dumi = int( 511.0 * (1.0+dumv) );
						*(ppVvalues + indexshift + (kk+1)*seq_pixels + jj*sizx + ii)= dumv;
						//Above line stores added vert response for saving AMOct08
						*(pVvalues + (kk+1)*num_pixels + jj*sizx + ii) = dumi;//vert resp.
						//(END ADDITIVE COMBINATION)
						//---------------------------------------

						dumh = 0; dumv = 0; //(Edge-W COMBINATION)
						edgeh =  *(ppHvalues + indexshift + jj*sizx + ii);//hor-edge(high freq chan)
						edgeh =  abs(edgeh);
						edgev = *(ppVvalues + indexshift + jj*sizx + ii);//vert-edge
						edgev = abs(edgev);
						dumi = 0;  //  to reset dumi values from additive combination
						
						for (mm=0;mm<chan_no;mm++)//loop actually performing combination
						{	// edge gating, no longer edge weighted AMApr2009
							if (edgeh>edge_thresh)	// horizontal response threshold
							{dumh +=  *(ppHvalues + indexshift + mm*seq_pixels 
							+ jj*sizx + ii);} else dumh += 0.0;	
							
							if (edgev>edge_thresh) // vertical response threshold
							{dumv = dumv + *(ppVvalues + indexshift + mm*seq_pixels 
							+ jj*sizx + ii);} else dumv += 0.0;
						}
						
						dumi = int( 511.0 * (1.0+dumh) );
						*(ppHvalues + indexshift + (kk+2)*seq_pixels + jj*sizx + ii)= dumh;
						//Above line stores added horiz response for saving AMOct08
						if (dumi < 0) dumi = 0;	// exclude negative levels after shifting range
						if (dumi > 1023) dumi = 1023;	// cap at 1023
						*(pHvalues + (kk+2)*num_pixels + jj*sizx + ii) = dumi; //horiz- for display
						dumi = int( 511.0 * (1.0+dumv) );
						*(ppVvalues + indexshift + (kk+2)*seq_pixels + jj*sizx + ii)= dumv;
						//Above line stores added vert response for saving AMOct08
						if (dumi < 0) dumi = 0;	// exclude negative levels after shifting range
						if (dumi > 1023) dumi = 1023;// cap at 1023
						*(pVvalues + (kk+2)*num_pixels + jj*sizx + ii) = dumi;//vert resp.
						//(END Edge-W COMBINATION)


					} // extra combination steps can be added here....
				
					if (kk==(chan_no-1) && rpos==2)	//in boundary region of combo
					{
						dumh = 0; dumv = 0;
						dumh = *(ppHvalues + indexshift + kk*seq_pixels + jj*sizx + ii);	
						dumv = *(ppVvalues + indexshift + kk*seq_pixels + jj*sizx + ii);
					
						dumi = int( 511.0 * (1.0+dumh));
						*(pHvalues + (kk+1)*num_pixels + jj*sizx + ii) = dumi; //comb1
						*(pHvalues + (kk+2)*num_pixels + jj*sizx + ii) = dumi; //comb2
						dumi = int( 511.0 * (1.0+dumv));
						*(pVvalues + (kk+1)*num_pixels + jj*sizx + ii) = dumi;//comb1
						*(pVvalues + (kk+2)*num_pixels + jj*sizx + ii) = dumi;//comb2
					} // add boundary combination steps here....

				} //end of kk for loop through channels
				
				} //end of column loop through ii				
			}	/* end run through rows */
		
			m_in_status_comb = _T("LOADED");
			UpdateData(FALSE);
			//RedrawWindow();

		int curr_frame = (nfram-n_first); // lines to set up input display AM Oct08 && Nov08
		DoCopy3Dto2D(pGvalues, ppGvalues, num_pixels, curr_frame*num_pixels);
		nImagePointProcess.Dump8bitData( pGvalues, sizx, sizy );
			
		//below: code to simultaneously display motion responses...
		for (kk=0; kk<(chan_no+2);kk++ )
		{
		for (ll=0;ll<num_pixels; ll++)		//populate pointers for individual channels
		{*(disp_pH + ll) = *(pHvalues + kk*num_pixels + ll);
		*(disp_pV + ll) = *(pVvalues + kk*num_pixels + ll);}
		// convert hor and ver values in 2 arrays on heap into response map pixels
		mImagePointProcess.Dump24bitData( disp_pH, disp_pV, sizx, sizy );	
		// check whether image is available, display and report data load activity
		if ( (n_pImageObject==NULL) || (sizx==0) || (sizy==0) )
			{ n_pImageObject=NULL; AfxMessageBox( "Could not create/display image!" ); return; }
		else
			{ 
			//if (kk<chan_no)
			//{
			//	n_pImageObject->Draw( &ClientDC, 5+kk*(sizx+5), 152 ); // position of indiviual displays			
			//	if (kk==0)
			//	{
			//		sprintf( out_str, "resp/%s%d.hor&ver: %dx%d ", 
			//					in_name_str, nfram, sizx,sizy);
			//		ClientDC.TextOut(5, 118, out_str);
			//	}
			//	sprintf( out_str, "Chan no: %d, Dphi: %2.1f", 
			//				kk+1, delphi[kk]);
			//	ClientDC.TextOut(5+kk*(sizx+5), 134, out_str);
			//	if( nfram == n_last )
			//	{
			//		if( n_pImageObject != NULL )
			//		{
			//			if( max_norm == 0)	sprintf( out_filnam, "btmp\\%s_%d.bmp", in_name_str, kk );
			//			if( max_norm == 1)	sprintf( out_filnam, "btmp\\%s_%d_norm.bmp", in_name_str, kk );
			//			success = n_pImageObject->Save( out_filnam, -1 );
			//			sprintf( out_str, "saved as: btmp\\%s_%d.bmp %d x %d : %d",
			//							in_name_str, kk, sizx, sizy, success);
			//			ClientDC.TextOut(450, 110, out_str );
			//		}
			//		else
			//		{
			//			RedrawWindow();
			//			ClientDC.TextOut(550, 110, "an unknown problem has occurred...			");
			//		}
			//	}
			//}

			if (kk==chan_no)		//first combination - Addition
			{
			n_pImageObject->Draw( &ClientDC, 5, 152 ); // position of indiviual displays
			sprintf( out_str, "Combined output: Addition");
			ClientDC.TextOut(5+(chan_no-2)*(sizx+5), 414, out_str);
			}

			if (kk==chan_no+1)		//Second combination - Edge-W
			{
			n_pImageObject->Draw( &ClientDC, 5+(sizx+5), 152 ); // position of indiviual displays
			sprintf( out_str, "Combined output: Edge-W");
			ClientDC.TextOut(5+(chan_no-1)*(sizx+5), 414, out_str);
			
			sprintf( out_str, "INPUT STIMULUS:"); // to display intput
			ClientDC.TextOut(5, 414, out_str);
			m_pImageObject->Draw(&ClientDC, 5, 432); 
			}

			//if ((hmin<0) || (hmax>1023)) //---> Doesnt work for MC!!!
			//	ClientDC.TextOut(450, 155, "OUTSIDE COLOUR RANGE !!!" );
			Sleep(DELAY);
			}
		}//end of loop through second set of kk's, diplaying each channels frame
		}				// end run through FULL image sequence
	SaveMotionArrays(sizx, sizy);  // save combination response in stim subdirectory
	if( max_norm == 1 )
	{
		channel = 4;
		SaveMotionArrays(sizx, sizy);
	}

	//set up histogram...May11, simple direction histogram for display
	int hist[2][360],index, ch, theta;
	double h_val, v_val,cal_hol, max1, max2; //angle before being made into an integer
	for (ii=0;ii<360;ii++) {hist[0][ii]=0; hist[1][ii]=0; }// set up histogram with null values
	//above, 0 is for addition, 1 is for edge weighted....


	//for (kk=0; kk<n_last; kk++) // time in frames
	//for (jj=0; jj<sizy; jj++) // x dimension
	//for (ii=0; ii<sizx; ii++) // y dimension
	//{
	//	index = n_last*sizx*sizy*chan_no + kk*sizx*sizy; //index at position just before combinations
	//	v_val = *(pVvalues + index + jj*sizx + ii);
	//	h_val = *(pHvalues + index + jj*sizx + ii);
	//	cal_hol = v_val/sqrt(v_val*v_val + h_val*h_val);
	//	theta = (int)(360*asin(cal_hol)/(2*pi));	
	//	ch = ImageRegion(3,ii,jj,2);
	//	if (ch==1) hist[0][theta]+=1; // add one to the given angle, frequency count...
	//	
	//	index = n_last*sizx*sizy*(chan_no+1) + kk*sizx*sizy; //index at position just before combinations
	//	v_val = *(pVvalues + index + jj*sizx + ii);
	//	h_val = *(pHvalues + index + jj*sizx + ii);
	//	cal_hol = v_val/sqrt(v_val*v_val + h_val*h_val);
	//	theta = (int)(360*asin(cal_hol)/(2*pi));	
	//	ch = ImageRegion(3,ii,jj,2);
	//	if (ch==1) hist[1][theta]+=1; // add one to the given angle, frequency count...
	//}// histogram with directions should be done by here....

	

}
