// 2DMD_DISPLAYDlg.cpp : implementation file
//

#include "stdafx.h"
#include "2DMD_DISPLAY.h"
#include "2DMD_DISPLAYDlg.h"
#include "ImagePointNewProc.h"
#include <string>
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define chan_no 4					//pre-defined number of multichannels in use

int sizx = 0;							// horizontal image size
int sizy = 0;							// vertical image size
double scal = 1.0;						// image intensity scale factor
int disp_hist = 0;						// display histogram radio button
int hist_type = 0;						// 0 for full histogram, 1 for hi-resolution histogram
double pi = 3.1415926535;
double alpha;
FILE *avg;

double resp_max[chan_no+2][2];		// why chan_no+2 ??? -> two extra channels for combo responses

FILE *fi;	FILE *fi1;	FILE *fi2;	FILE *fi3;	FILE *fi4; FILE *fi5; FILE *fi6;
FILE *fiv1;	FILE *fiv2;	FILE *fiv3;	FILE *fiv4; FILE *fiv5; FILE *fiv6;
 
// define external string and image variables
char in_name_str[50] = "";			// core filename: same initialisation as in Dlg
double fshift = 0.0;					// display intensity DC shift: same initialisation as in Dlg
double fscale = 1.0;					// display intensity gain (before shift): same initialisation as in Dlg 
double thresh = 255.0;					// threshold for nonlinear scaling (999.9 for linear): same initialisation as in Dlg 
int  n_first = 1;						// num first frame: same initialisation as in Dlg
int  n_last  = 20;						// num last frame: same initialisation as in Dlg
int num_pixels = 0;
long seq_pixels;
int frequ = 1;
int WIDF = 32;
double chan_max[4][2];
int small_histo[256][2][chan_no+1];	//AM02DEC08 - global array to store histogram for display (+1 for all frame histogram)
int DELAY = 10;						//AM02DEC08/ MAY09

int out_circ = 0;					// AS20FEB09 - radius of outer circle for region of interest
int in_circ = 0;					// AS20FEB09 - radius of inner circle for region of interest
//-----------------------
//variables/ simulation info necessary for working histograms....
double delphi[4]; // from standard simulation parameters
#define MM 65
int MIDF = (MM-1)/2; // centre of filter array
	

//--------------------


unsigned short int * pHvalues;			// pointer to image horizontal response values (data array)
unsigned short int * pVvalues;			// pointer to image vertical response values (data array)
unsigned short int * disp_pH;
unsigned short int * disp_pV;
double * ppHvalues;
double * ppVvalues;
int		* histHvalues; 
int		* histVvalues;
int		* smallHhist;
int		* smallVhist;
int		* histAmp;
int		* histDir;
unsigned long int	   * twoDhist;
unsigned long int     * twoDhires;
unsigned short int	   * dispHist;
unsigned short int	   * dispTwohist;
unsigned short int	   * currHist;
unsigned short int	   * currTwohist;

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
// CMy2DMD_DISPLAYDlg dialog

CMy2DMD_DISPLAYDlg::CMy2DMD_DISPLAYDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMy2DMD_DISPLAYDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMy2DMD_DISPLAYDlg)
	m_in_inputname = _T("");
	m_in_intensgain = _T("1.0");
	m_in_intenshift = _T("0.0");
	m_in_firstn = 1;
	m_in_lastn = 20;
	m_in_threshold = _T("999.9");
	m_in_status_hist = _T("EMPTY");
	m_hist_type = 0;
	m_in_circ = 0;
	m_out_circ = 0;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMy2DMD_DISPLAYDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMy2DMD_DISPLAYDlg)
	DDX_Text(pDX, IDC_IN_inputname, m_in_inputname);
	DDX_Text(pDX, IDC_IN_gain, m_in_intensgain);
	DDX_Text(pDX, IDC_IN_shift, m_in_intenshift);
	DDX_Text(pDX, IDC_IN_first, m_in_firstn);
	DDX_Text(pDX, IDC_IN_last, m_in_lastn);
	DDX_Text(pDX, IDC_IN_thresh, m_in_threshold);
	DDX_Text(pDX, IDC_STATUS_HIST, m_in_status_hist);
	DDX_Text(pDX, IDC_HISTTYPE, m_hist_type);
	DDX_Text(pDX, IDC_In_Circ, m_in_circ);
	DDX_Text(pDX, IDC_Out_Circ, m_out_circ);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CMy2DMD_DISPLAYDlg, CDialog)
	//{{AFX_MSG_MAP(CMy2DMD_DISPLAYDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_EN_CHANGE(IDC_IN_inputname, OnChangeINinputname)
	ON_EN_CHANGE(IDC_IN_gain, OnChangeINgain)
	ON_EN_CHANGE(IDC_IN_shift, OnChangeINshift)
	ON_BN_CLICKED(IDC_LOADRESP, OnShowHist)
	ON_BN_CLICKED(IDC_LOADRESP2, OnSegresp)
	ON_BN_CLICKED(IDC_LOADRESP3, OnLoadresp)
	ON_EN_CHANGE(IDC_IN_first, OnChangeINfirst)
	ON_EN_CHANGE(IDC_IN_last, OnChangeINlast)
	ON_BN_CLICKED(IDC_SHOWRESEQ, OnCalcHist)
	ON_BN_CLICKED(IDC_AVERRESP, OnAverresp)
	ON_EN_CHANGE(IDC_IN_thresh, OnChangeINthresh)
	ON_EN_CHANGE(IDC_HISTTYPE, OnChangeINhisttype)
	ON_EN_CHANGE(IDC_In_Circ, OnChangeINincirc)
	ON_EN_CHANGE(IDC_Out_Circ, OnChangeINoutcirc)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_CALC_combH, &CMy2DMD_DISPLAYDlg::OnBnClickedCalccombh)
	ON_BN_CLICKED(IDC_SHOW_combH, &CMy2DMD_DISPLAYDlg::OnBnClickedShowcombh)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMy2DMD_DISPLAYDlg message handlers

BOOL CMy2DMD_DISPLAYDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

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
	l_pImageObject = NULL;
	m_pImageObject = NULL;
	n_pImageObject = NULL;
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CMy2DMD_DISPLAYDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CMy2DMD_DISPLAYDlg::OnPaint() 
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
HCURSOR CMy2DMD_DISPLAYDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

/////////////////////// ADDED FUNCTIONALITY /////////////////////////////////////
/////////////////////// ADDED FUNCTIONALITY /////////////////////////////////////
/////////////////////// ADDED FUNCTIONALITY /////////////////////////////////////

void CMy2DMD_DISPLAYDlg::OnChangeINinputname() 
{
	// TODO: Add your control notification handler code here
	// input of new filename
	UpdateData(TRUE); sprintf( in_name_str, "%s", m_in_inputname); 
	// erase old image & text
	RedrawWindow();			// erase previous image and messages
	// reset image object >> load new file
	l_pImageObject = NULL;
	m_pImageObject = NULL;
	n_pImageObject = NULL;
}


void CMy2DMD_DISPLAYDlg::OnChangeINgain() 
{
	// TODO: Add your control notification handler code here
	// input of new intensity gain factor
	UpdateData(TRUE); fscale = atof(m_in_intensgain); 
	// erase old image & text
	RedrawWindow();			// erase previous image and messages
	// reset image object >> load new file
	l_pImageObject = NULL;
	m_pImageObject = NULL;
	n_pImageObject = NULL;
}


void CMy2DMD_DISPLAYDlg::OnChangeINshift() 
{
	// TODO: Add your control notification handler code here
	// input of new intensity DC shift
	UpdateData(TRUE); fshift = atof(m_in_intenshift); 
	// erase old image & text
	RedrawWindow();			// erase previous image and messages
	// reset image object >> load new file
	l_pImageObject = NULL;
	m_pImageObject = NULL;	
	n_pImageObject = NULL;
}


void CMy2DMD_DISPLAYDlg::OnChangeINthresh() 
{
	// TODO: Add your control notification handler code here
	// input of new threshold for nonlinear colour display
	UpdateData(TRUE); thresh = atof(m_in_threshold); 
	// erase old image & text
	RedrawWindow();			// erase previous image and messages
	// reset image object >> load new file
	l_pImageObject = NULL;
	m_pImageObject = NULL;	
	n_pImageObject = NULL;
	
}

void CMy2DMD_DISPLAYDlg::OnChangeINhisttype() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE); hist_type = m_hist_type; 
}

void CMy2DMD_DISPLAYDlg::OnChangeINfirst() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE); n_first = m_in_firstn; 
}


void CMy2DMD_DISPLAYDlg::OnChangeINlast() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE); n_last = m_in_lastn; 		
}

void CMy2DMD_DISPLAYDlg::OnChangeINincirc()
{
	UpdateData(TRUE); in_circ = m_in_circ;
}

void CMy2DMD_DISPLAYDlg::OnChangeINoutcirc()
{
	UpdateData(TRUE); out_circ = m_out_circ;
}

void CMy2DMD_DISPLAYDlg::OnLoadresp() 
{
	// TODO: Add your control notification handler code here
	// load 2 ascii response data files and convert into response pseudocolour map  
	int dumi, dummi, nfram, ii, jj, kk, ll;
	char out_str[80];
	char in_filnam[100];                
	int tot_frames = n_last-n_first+1;

	int hmax = 0;
	int hmin = 1023;

	// set simulation parameters, but the motion detection has already been done!
	delphi[0] = 2; 
	delphi[1] = 4; 
	delphi[2] = 8; 
	delphi[3] = 16;

    int vmax = 0;
	int vmin = 1023;
	int curr_val = 0; // variable to store current histogram value

	if (m_in_status_hist == _T("LOADED"))
	{
		delete ppHvalues;	ppHvalues = 0;
		delete ppVvalues;	ppVvalues = 0;
		delete histHvalues;	histHvalues = 0;
		delete histVvalues;	histVvalues = 0;
		delete smallHhist;	smallHhist = 0;
		delete smallVhist;	smallVhist = 0;
		m_in_status_hist = _T("...OLD DATA!");
		UpdateData(FALSE);
	}

	CClientDC ClientDC( this );
	BeginWaitCursor();
	RedrawWindow();			// erase previous image and messages

	
		for(kk=0; kk<4; kk++)
		{
			sprintf( in_filnam, "resp/%s_%d.chah", in_name_str, kk );	// generate input filename 
			dumi = DoCheckFile(in_filnam);	// determine first image size
			if (dumi==0)					// and exit if input file not opened 
				{ AfxMessageBox( "cannot load first ascii file (name?)!" ); return; } // and exit if not
		}
	
		for(kk=0; kk<2; kk++) //additional step for checking combined channel outputs May11
		{
			sprintf( in_filnam, "resp/%s_combo%d.chah", in_name_str, kk );	// generate input filename 
			dumi = DoCheckFile(in_filnam);	// determine first image size
			if (dumi==0)					// and exit if input file not opened 
				{ AfxMessageBox( "cannot load first ascii file for combination(name?)!" ); return; } // and exit if not
		}

	num_pixels = sizx*sizy;			// calculate number of pixels
	seq_pixels = num_pixels * (n_last - n_first + 1);

	// prepare data array for horizontal then vertical response values 
	ppHvalues = new double [seq_pixels*(chan_no+2)];
	pHvalues = new unsigned short int [num_pixels*(chan_no+2)];	// allocate space for image on heap
	if ( pHvalues==NULL )							// warning if array doesn't get requested heap space
		{ AfxMessageBox( "Could not create H heap space !" ); return; }
	ppVvalues = new double [seq_pixels*(chan_no+2)];
	pVvalues = new unsigned short int [num_pixels*(chan_no+2)];	// allocate space for image on heap
	if ( pVvalues==NULL )							// warning if array doesn't get requested heap space
		{ AfxMessageBox( "Could not create H heap space !" ); return; }

	disp_pH = new unsigned short int [num_pixels];		// initialise pointer for hor response values in current frame of current channel for display
	if(disp_pH==NULL)
		{ AfxMessageBox( "Could not create image display hor heap !" ); return; }

	disp_pV = new unsigned short int [num_pixels];		// initialise pointer for ver response values in current frame of current channel for display
	if(disp_pV==NULL)
		{ AfxMessageBox( "Could not create image display hor heap !" ); return; }

	n_pImageObject = new CImageObject( "col_24bit.bmp" );
	dumi = n_pImageObject->Stretch( sizx, sizy );
	if ( (n_pImageObject==NULL) || (dumi == 0) )
		{ AfxMessageBox( "Could not create image !" ); return; }

	CImagePointProcesses mImagePointProcess( n_pImageObject );


		for(nfram = n_first; nfram <= n_last; nfram++)
		{
			for(kk = 0; kk < chan_no; kk++) // going through response channels May11
			{	
					sprintf( in_filnam, "resp/%s_%d.chah", in_name_str, kk );	// generate input filename 
					dumi = DoLoadFile(nfram, kk, 0, in_filnam, &hmin, &hmax);
					if (dumi==0)					// and exit if hor response file not opened
						{ AfxMessageBox( "cannot load current chah file (name?)!" ); return; } 
			
					sprintf( in_filnam, "resp/%s_%d.chav", in_name_str, kk );	// generate input filename 
					dumi = DoLoadFile(nfram, kk, 1, in_filnam, &vmin, &vmax);
					if (dumi==0)					// and exit if ver response file not opened
						{ AfxMessageBox( "cannot load current chav file (name?)!" ); return; } 
			}		// end of loop through single response channels 1-4
			
			//RedrawWindow();
			sprintf( out_str, "resp/%s.chah&chav frame %d: %dx%d min|max %d|%d&%d|%d", 
						in_name_str, nfram, sizx,sizy, hmin/4, hmax/4, vmin/4, vmax/4 );
			ClientDC.TextOut(550, 45, out_str);

			for(kk = 0; kk < chan_no; kk++)
			{
				for(jj=0; jj<sizy; jj++)
				{
					for(ii=0; ii<sizx; ii++)
					{
						*(disp_pH + jj*sizx + ii) = *(pHvalues + jj*sizx + ii + (kk*num_pixels));
						*(disp_pV + jj*sizx + ii) = *(pVvalues + jj*sizx + ii + (kk*num_pixels));
					}
				}
				mImagePointProcess.Dump24bitData( disp_pH, disp_pV, sizx, sizy );
				if( thresh != 255.0 )
					mImagePointProcess.Dump24bitDataNL( thresh, disp_pH, disp_pV, sizx, sizy );

				n_pImageObject->Draw( &ClientDC, 2+kk*(sizx+2), 152);
				if(kk==0)
					{
						sprintf( out_str, "resp/%s%d.hor&ver: %dx%d ", in_name_str, nfram, sizx, sizy);
						ClientDC.TextOut(5, 118, out_str);
					}			
				sprintf( out_str, "Chan no: %d", kk+1);
				ClientDC.TextOut(2+kk*(sizx+2), 134, out_str);	
				
			}  // end loop through channels for display
		}	// end loop through image sequence

	fclose(fi1);	fclose(fi2);	fclose(fi3);	fclose(fi4); 
	fclose(fiv1);	fclose(fiv2);	fclose(fiv3);	fclose(fiv4);
	fclose(avg);
	delete pHvalues;	pHvalues = 0;
	delete pVvalues;	pVvalues = 0;
	delete disp_pH;		disp_pH = 0;
	delete disp_pV;		disp_pV = 0;
	EndWaitCursor();	
}


void CMy2DMD_DISPLAYDlg::OnSegresp() 
{
	// TODO: Add your control notification handler code here
	// load 2 ascii response data files and convert into response pseudocolour map  
	int dumi, dummi, nfram, ii, jj, kk, ll, mm, nn, reg, ps_x, ps_y, max_posx, max_valx;
	int max_posy, max_valy;
	char out_str[80];
	char in_filnam[100];                
	int tot_frames = n_last-n_first+1;

	int hmax = 0;
	int hmin = 1023;
	int curr_index;
	int kern_siz = 31; // size of filtering kernal for segregation filter
	int seg_res = 11;  // segregation of resolution -> should be an odd number!!
	int loc_hist[2][12]; // store for local histogram function....	

	double hold_h, hold_v, calc_h, calc_v; // variables for intermediate calculations... AM MAY2009
	double *kernel;  // filtering kernel

    int vmax = 0;
	int vmin = 1023;
	int curr_val = 0; // variable to store current histogram value

	if (m_in_status_hist != _T("LOADED"))
	{ AfxMessageBox( "do the histograms! how do you expect to segregate? magic?" ); return; }

	CClientDC ClientDC( this );
	BeginWaitCursor();
	RedrawWindow();			// erase previous image and messages

	num_pixels = sizx*sizy;			// calculate number of pixels
	seq_pixels = num_pixels * (n_last - n_first + 1);
	
	kernel = new double [kern_siz*kern_siz]; //set up blurring kernel
	for (ii=0;ii<kern_siz*kern_siz; ii++) *(kernel + ii)= 0.0;

	// prepare data array for horizontal response values 
	pHvalues = new unsigned short int [num_pixels*chan_no];	// allocate space for image on heap
	if ( pHvalues==NULL )							// warning if array doesn't get requested heap space
		{ AfxMessageBox( "Could not create H heap space !" ); return; }
	
	pVvalues = new unsigned short int [num_pixels*chan_no];	// allocate space for image on heap
	if ( pVvalues==NULL )							// warning if array doesn't get requested heap space
		{ AfxMessageBox( "Could not create H heap space !" ); return; }

	disp_pH = new unsigned short int [num_pixels];		// initialise pointer for hor response values in current frame of current channel for display
	if(disp_pH==NULL)
		{ AfxMessageBox( "Could not create image display hor heap !" ); return; }

	disp_pV = new unsigned short int [num_pixels];		// initialise pointer for ver response values in current frame of current channel for display
	if(disp_pV==NULL)
		{ AfxMessageBox( "Could not create image display hor heap !" ); return; }

	n_pImageObject = new CImageObject( "col_24bit.bmp" );
	dumi = n_pImageObject->Stretch( sizx, sizy );
	if ( (n_pImageObject==NULL) || (dumi == 0) )
		{ AfxMessageBox( "Could not create image !" ); return; }

	CImagePointProcesses mImagePointProcess( n_pImageObject );
	reg = 0;

		for(nfram = n_first; nfram <= n_last; nfram++)
		{
			for(kk = 0; kk < chan_no; kk++)
			{
				for(jj=0; jj<sizy; jj++)
				{
					curr_index = num_pixels*(nfram-n_first) + kk*tot_frames*num_pixels; //index along ppH pointer
					for(ii=0; ii<sizx; ii++)
					{
						//------------------------------------------------
						calc_h = *(ppHvalues + jj*sizx + ii + curr_index);
						calc_v = *(ppVvalues + jj*sizx + ii + curr_index);
						//------------------------------------------------
						
						reg = ImageRegion(kk,ii,jj); // identify if within response region... AM May2009 (1 is in)
						if (reg == 1 && nfram != n_last)
						{
							calc_h = seg_res*(int)(calc_h/seg_res); //reducing resolution  
							calc_v = seg_res*(int)(calc_h/seg_res); //reducing resolution	
						} 

						if (reg == 1 && nfram == n_last && kk!= 1 && kk!= 2) // ACTUAL SEGREGATION STEP, last frame.... Am MAY2009
						{
							curr_index = num_pixels*(nfram-n_first-1) + kk*tot_frames*num_pixels; //index along ppH pointer
							// index above goes to second last frame....
							for (ll=0;ll<2;ll++) // resest local histogram holder
							for (mm=0;mm<12;mm++)loc_hist[ll][mm]= 0;

							max_posx = (int)(seg_res*0.5); max_valx = 0; // reset non linear segg. variables
							max_posy = (int)(seg_res*0.5); max_valy = 0;
							//------------------------------------------------
							//calc_h = *(ppHvalues + jj*sizx + ii + curr_index);
							//calc_v = *(ppVvalues + jj*sizx + ii + curr_index);
							//------------------------------------------------
							hold_v = 0.0; hold_h = 0.0; // preset to zero
							
							for (ll=0;ll<kern_siz; ll++) // x index
							for (mm=0;mm<kern_siz; mm++) // y index
							{
								ps_x = (int)(ll-0.5*kern_siz); ps_y = (int)(mm-0.5*kern_siz); 	
								//------------------------------------------------
								hold_h = *(ppHvalues + (jj+ps_y)*sizx + ii + ps_x + curr_index);
								hold_h = (int)(hold_h/(1023/seg_res));
								
								hold_v = *(ppVvalues + jj*sizx + ii + curr_index);
								hold_v = (int)(hold_v/(1023/seg_res));
								
								if (hold_h<seg_res && hold_h != (int)(seg_res*0.5)) loc_hist[0][(int)(hold_h)]+= 1;	
								if (hold_h == (int)(seg_res*0.5) && hold_v != (int)(seg_res*0.5) && hold_h<seg_res)
								{loc_hist[0][(int)(hold_h)]+= 1;}

								if (hold_v<seg_res && hold_v != (int)(seg_res*0.5) || hold_h != (int)(seg_res*0.5)) loc_hist[1][(int)(hold_v)]+= 1;
								if (hold_v == (int)(seg_res*0.5) && hold_h != (int)(seg_res*0.5) && hold_v<seg_res)
								{loc_hist[1][(int)(hold_v)]+= 1;}
								//------------------------------------------------
								// cluster algorithm input goes here, creating local histogram....
								for (nn=0;nn<seg_res;nn++)
								{ 
									if (loc_hist[0][nn]>max_valy) {max_valy = loc_hist[0][nn]; max_posy = nn;} 
									if (loc_hist[1][nn]>max_valx) {max_valx = loc_hist[1][nn]; max_posx = nn;}
								}
								calc_h = max_posy * (1023/seg_res);
								calc_v = max_posx * (1023/seg_res);
							} // end of for loop with ll and mm, i.e. segregation filtering AM May09
						
						} // END OF IF CONDITION FOR LAST FRAME.... AM MAY09
						//------------------------------------------------------------
						*(disp_pH + jj*sizx + ii) = (int)(calc_h);
						*(disp_pV + jj*sizx + ii) = (int)(calc_v);
					}
				}

				mImagePointProcess.Dump24bitData( disp_pH, disp_pV, sizx, sizy );
				if( thresh != 255.0 )
					mImagePointProcess.Dump24bitDataNL( thresh, disp_pH, disp_pV, sizx, sizy );


				n_pImageObject->Draw( &ClientDC, 2+kk*(sizx+2), 152);
				if(kk==0)
					{
						sprintf( out_str, "resp/%s%d.hor&ver: %dx%d ", in_name_str, nfram, sizx, sizy);
						ClientDC.TextOut(5, 118, out_str);
					}			
				sprintf( out_str, "Chan no: %d", kk+1);
				ClientDC.TextOut(2+kk*(sizx+2), 134, out_str);	
				
			}  // end loop through channels for display
		}	// end loop through image sequence

	delete pHvalues;	pHvalues = 0;
	delete pVvalues;	pVvalues = 0;
	delete disp_pH;		disp_pH = 0;
	delete disp_pV;		disp_pV = 0;
	EndWaitCursor();		
}


int DoCheckFile(char * in_filnam)
{	// open first ascii data file of sequence and determine image size
	FILE    *fi;
	char    biglin[1000*6];			/* input line string: maximum 1000 values */
	char    *smalin;				/* cut input line string */

	// open first ascii data file and get image size
	if ( (fi=fopen(in_filnam,"rb")) == NULL )	// test whether ascii file can be opened 
		return(0);								// and exit if not
	fgets( biglin, 20, fi );		// read file header
	sizx = atoi(biglin);			// convert number of columns
    smalin = (biglin+4);			// cut and ...
	sizy = atoi(smalin);			// ... convert number of rows
	fclose(fi);						// close ascii data file
	return(1);
// end of JMZ function:  DoCheckinput()			
}

int DoLoadFile(int loop, int channel, int horver, char * filname, int * mmin, int * mmax) 
{
	// load ascii greylevel data file and convert into input image array
	int ii, jj, border, length;
	double dumy;
	unsigned char outi;
	int dumi;
	char out_filnam[100];
	
	double avgHor = 0.0;
	double avgVer = 0.0;
	double avgLen = 0.0;
	int circpix = 0;

	
	if( channel == 0 && horver == 0)
	{
		sprintf( out_filnam, "resp\\averages.txt" );
		if( (avg=fopen(out_filnam,"w")) == NULL )
		{ AfxMessageBox( "cannot save averages file!" ); return 0; }

		fprintf(avg, "Region of interest outer circle: %d | inner circle: %d \n", out_circ, in_circ);
	}


	char    biglin[10000];			/* input line string: maximum 1000 values */
	
	length = strlen(filname);
	
	length = length + 21;
	
	num_pixels = sizx * sizy;
	seq_pixels = ((n_last - n_first + 1)*num_pixels);


	if(loop == n_first){
	// open ascii data file and get header
		if( horver == 0)
		{
			if ( channel == 0){
					if ( (fi1=fopen(filname,"rb")) == NULL)	return(0);	
					fgets( biglin, length, fi1 );		// read file header
			}
			if ( channel == 1){
					if ( (fi2=fopen(filname,"rb")) == NULL)	return(0);	
					fgets( biglin, length, fi2 );		// read file header
			}
			if ( channel == 2){
					if ( (fi3=fopen(filname,"rb")) == NULL)		return(0);	
					fgets( biglin, length, fi3 );		// read file header
			}
			if ( channel == 3){
					if ( (fi4=fopen(filname,"rb")) == NULL)		return(0);	
					fgets( biglin, length, fi4 );		// read file header
			}
					
		}
		else
		{
			if ( channel == 0){
					if ( (fiv1=fopen(filname,"rb")) == NULL)		return(0);	
					fgets( biglin, length, fiv1 );		// read file header
			}
			if ( channel == 1){
					if ( (fiv2=fopen(filname,"rb")) == NULL)		return(0);	
					fgets( biglin, length, fiv2 );		// read file header
			}
			if ( channel == 2){
					if ( (fiv3=fopen(filname,"rb")) == NULL)		return(0);	
					fgets( biglin, length, fiv3 );		// read file header
			}
			if ( channel == 3){
					if ( (fiv4=fopen(filname,"rb")) == NULL)		return(0);	
					fgets( biglin, length, fiv4 );		// read file header
			}
			
		
		}
			if(loop > 1){
				for(int dd = 1; dd<loop; dd++){
					for(jj=0; jj<sizy; jj++){
						for(ii=0; ii<sizx; ii++){
							if(horver == 0){
								if ( channel == 0)	 fgetc(fi1);
								if ( channel == 1)	 fgetc(fi2);
								if ( channel == 2)	 fgetc(fi3);
								if ( channel == 3)	 fgetc(fi4);}
							else{
								if ( channel == 0)	 fgetc(fiv1);
								if ( channel == 1)	 fgetc(fiv2);
								if ( channel == 2)	 fgetc(fiv3);
								if ( channel == 3)	 fgetc(fiv4);}
						}
					}
				}
			}
	}
	
	// get & convert data from ascii file and enter into data array
	int levi = 1;						// factor for integer array value to be stored
	int maxi = 1023;						// range of integer array values
	double amply = 1023.0;				// range of double array values
	double shifty = 0.0;				// relative DC shift of array values

    *mmax = 0;
	*mmin = maxi;

	long indexshift = num_pixels * channel;
	long indexshift2 = (num_pixels * (loop-n_first)) + (seq_pixels * channel);

	for (jj=0; jj<sizy; jj++ )			// run through sizy rows
		{
		for (ii=0; ii<sizx; ii++ )		// run through sizx columns
			{
				if(horver == 0){
					if ( channel == 0)		outi = fgetc(fi1);
					if ( channel == 1)		outi = fgetc(fi2);
					if ( channel == 2)		outi = fgetc(fi3);
					if ( channel == 3)		outi = fgetc(fi4);}
				else{
					if ( channel == 0)		outi = fgetc(fiv1);
					if ( channel == 1)		outi = fgetc(fiv2);
					if ( channel == 2)		outi = fgetc(fiv3);
					if ( channel == 3)		outi = fgetc(fiv4);}
					dumi = int(outi);

					if((dumi == 0) || (dumi == 255))	border = 1;
					else								border = 0;

					dumy = fshift + fscale * ((double)dumi/255.0);
					dumi = int(amply * (dumy + shifty));
					

					if(dumi < 0)	dumi = 0;
					if(dumi > maxi)	dumi = maxi;

					if((border == 0) && (dumi>*mmax))	*mmax = dumi;
					if((border == 0) && (dumi<*mmin))	*mmin = dumi;
					
					/////////////////////////////////////////////////////
					//// calculate distance from centre of current pixel
					int distance;
					double side1 = 0.0, side2 = 0.0;
					if(ii<127)		side1 = (127-ii)*(127-ii);
					if(ii>127)		side1 = (ii-127)*(ii-127);
					if(jj<127)		side2 = (127-jj)*(127-jj);
					if(jj>127)		side2 = (jj-127)*(jj-127);
						
					distance = (int)sqrt(side1+side2);

					/////////////////////////////////////////////////////
					//// exclude border of image
					if((ii<14) || (ii>241) || (jj<14) || (jj>241))
						distance = out_circ-1;

					/////////////////////////////////////////////////////
					//// if region of interest is defined, set anything
					//// outside the region to appear white
					if(in_circ > 0 && out_circ > 0)
					{
						if((distance>out_circ) || (distance<in_circ))
						{
							dumi = 511;
						}
						else
						{
							circpix += 1;
							if(horver == 0)		avgHor += dumi;
							if(horver == 1)		avgVer += dumi;
						}
					}	

					if(horver == 0){
					*(pHvalues + indexshift + jj*sizx + ii) = levi * dumi;
					*(ppHvalues + indexshift2 + jj*sizx + ii) = levi * dumi;}
					else{
					*(pVvalues + indexshift + jj*sizx + ii) = levi * dumi;
					*(ppVvalues + indexshift2 + jj*sizx + ii) = levi * dumi;}
			}										// (greylevel assumed as sum of RGB)
		}	/* end run through rows */
	if(horver == 0)	
	{
		avgHor = avgHor/circpix;
		fprintf(avg, "Channel %d | Pixels in region: %5d | average hor: %5d ", channel, circpix, avgHor);
	}
	if(horver == 1)
	{
		avgVer = avgVer/circpix;
		fprintf(avg, "| average ver: %5d \n", avgVer);
	}
	// cleanup
	return(1);
// end of JMZ function:  DoLoadinput()			
}
	

void CMy2DMD_DISPLAYDlg::OnCalcHist() 
{
	// TODO: Add your control notification handler code here
	// load sequences of ascii response data files, convert into input image map, and display
	int ii, jj, kk, nfram, ll, new_bin, dummy, dummx, dumi, twoD; // declare twoD up here??? AM MAY09
	double dumy, dumx;
	double resph, respv;
	char out_filnam[80];
	int tot_frames = n_last - n_first + 1;
	int hist_thresh = 2; //histogram threshold, nonlinearity to reduce zero responses AM MAY2009
	// IMPORTANT parameter above, now set to 2 from an initial 50, to exclude just the smallest responses around 0
	
	//////////////////////////////////////////////////
	///////// for calculating amplitude histogram ...? why? am june 2013
	double amp;
	double amp_max;
	int hist_amp;
	///////////////////////////////////////////////////
	///////// for calculating direction histogram
	double hordir;
	double verdir;
	int hist_dir;

	FILE *testh;
	sprintf( out_filnam, "resp\\%s_hor.txt",  in_name_str);
	if ( (testh=fopen(out_filnam,"w")) == NULL )		// test whether ascii file can be opened 
		{ AfxMessageBox( "cannot save hi-res hor histogram file (open?)!" ); return; }	// prevent save without open file

	FILE *testv;
	sprintf( out_filnam, "resp\\%s_ver.txt",  in_name_str);
	if ( (testv=fopen(out_filnam,"w")) == NULL )		// test whether ascii file can be opened 
		{ AfxMessageBox( "cannot save hi-res hor histogram file (open?)!" ); return; }	// prevent save without open file


	m_in_status_hist = _T("...CALCULATING");
	UpdateData(FALSE);
	RedrawWindow();
	disp_hist = 1; // display the produced histograms??? maximum responses for scaling...
	// 
	for( kk=0; kk<chan_no; kk++)						// will be in header of hor&ver files
	{													// giving max and min for response values
		resp_max[kk][0] = 0.0; // xx
		resp_max[kk][1] = 0.0; // yy
		
		for (int ii=0; ii<seq_pixels; ii++) // find response maximum values - may be useful for later scaling
		{
			resph = *(ppHvalues + ii + (kk*seq_pixels)); //horizontal responses
			if( (resph > resp_max[kk][0]) && (resph != 0) && (resph != 1023))	
				resp_max[kk][0] = abs(resph);

			respv = *(ppVvalues + ii + (kk*seq_pixels)); // vertical responses
			if( (respv > resp_max[kk][1]) && (respv != 0) && (respv != 1023))	
				resp_max[kk][1] = abs(respv);
		
		}
	} // find response maximum for normalisation later ASNov08

	//************ Initialise arrays for calculating histogram values ************
	histHvalues = new int [(1024 * tot_frames * chan_no) + (1024*chan_no)];
	if(histHvalues==NULL)
		{ AfxMessageBox( "Could not create hor histogram heap space!" ); return; }
	histVvalues = new int [(1024 * tot_frames * chan_no) + (1024*chan_no)];
	if( histVvalues==NULL)
		{ AfxMessageBox( "Could not create ver histogram heap space!" ); return; }
	smallHhist = new int [(256 * tot_frames * chan_no) + (256*chan_no)];
	if( smallHhist==NULL )
		{ AfxMessageBox( "Could not create hor scaled histogram heap space!" ); return; }
	smallVhist = new int [(256 * tot_frames * chan_no) + (256*chan_no)];
	if( smallVhist==NULL )
		{ AfxMessageBox( "Could not create ver scaled histogram heap space!" ); return; }
	twoDhist = new unsigned long int [(256*256*chan_no*tot_frames) + (256*256*chan_no)];
	if( twoDhist==NULL ) // AM MAY2009 - unsigned short too small!!!
		{ AfxMessageBox( "Could not create full 2D histogram heap speace!" ); return; }
	twoDhires = new unsigned long int [(1024*1024*chan_no*tot_frames) + (1024*1024*chan_no)];
	if( twoDhires==NULL ) // AM MAY2009 - unsigned short too small!!!
		{ AfxMessageBox( "Could no create hi-res 2D histogram heap space!" ); return; }
	histAmp = new int [256*tot_frames*chan_no];
	if( histAmp==NULL )
		{ AfxMessageBox( "Could not create amplitude histogram heap space!" ); return; }
	histDir = new int [256*tot_frames*chan_no];
	if( histDir==NULL )
		{ AfxMessageBox( "Could not create direction histogram heap space!" ); return; }
	//*****************************************************************************
	int reg = 0;
	for (ii=0;ii<1024;ii++) // initialize histogram arrays to zero! Am02DEC08
	for (kk=0;kk<chan_no;kk++) // channels
	for (nfram=n_first; nfram<=n_last; nfram++)
	{
		*(histHvalues + ii + tot_frames*1024*kk + (nfram-n_first)*1024) = 0; //AM MAY09... the -1!!!!
		*(histVvalues + ii + tot_frames*1024*kk + (nfram-n_first)*1024) = 0; //AM MAY09... the -1!!!!
		
		for(twoD=0; twoD<1024; twoD++)
		*(twoDhires + ii*1024 + twoD + tot_frames*1024*1024*kk + (nfram-n_first)*1024*1024) = 0;
		
		
		if (nfram==n_last)
		{
			for(twoD=0; twoD<1024; twoD++)
				*(twoDhires + ii*1024 + twoD + tot_frames*1024*1024*chan_no + kk*1024*1024) = 0;
		}


		if (ii<256) // NOTE AM MAY 2009, you need the -1 if using indeces starting from 1!! (nfram)
		{
			*(smallHhist + ii + (tot_frames*256*kk) + ((nfram-n_first)*256)) = 0;
			*(smallVhist + ii + (tot_frames*256*kk) + ((nfram-n_first)*256)) = 0;
			*(histAmp + ii + (tot_frames*256*kk) + ((nfram-n_first)*256)) = 0;
			*(histDir + ii + (tot_frames*256*kk) + ((nfram-n_first)*256)) = 0;
			for(twoD=0; twoD<256; twoD++)
				*(twoDhist + ii*256 + twoD + (tot_frames*256*256*kk) + ((nfram-n_first)*256*256)) = 0;
			if (nfram==n_last) // AM MAY 2009 - for final frame
			{
				*(smallHhist + ii + tot_frames*256*chan_no + kk*256) = 0;
				*(smallVhist + ii + tot_frames*256*chan_no + kk*256) = 0;
				for(twoD=0; twoD<256; twoD++)
				*(twoDhist + ii*256 + twoD + tot_frames*256*256*chan_no + kk*256*256) = 0;
			}
		}
		// This point in the program should set pointers to zero for the realtime histograms
	} // end of initialization

	for(kk=0; kk<chan_no; kk++)		// run through channels, calculate histograms
	{
		amp_max = sqrt((1023.0*1023.0)*2.0); //..... scales to root(2)*1023
		for(nfram=n_first; nfram<=n_last; nfram++)
		{
			long indexshift = (kk*seq_pixels) + ((nfram-n_first) * num_pixels);
			for(jj=0; jj<sizy; jj++)
			{
				for(ii=0; ii<sizx; ii++)
				{
					reg =  ImageRegion(kk,ii,jj); // find whether current location is inside or outside filters
					// above AM May2009
					if (reg==1)
						{
						dumy = *(ppHvalues + jj*sizx + ii + indexshift); // find current hor response
						dumx = *(ppVvalues + jj*sizx + ii + indexshift); // find current ver response

						dummy = (int)dumy;  // get integer version of the responses
						dummx = (int)dumx;

						// this separates the 'speed' from the direction
						//////////////////////////////////////
						///////// find amplitude which is a measure of speed AM June 13
						amp = sqrt((dumy*dumy) + (dumx*dumx));
						hist_amp = ((amp/amp_max)*255.0); // changed AM 1mAY09
						*(histAmp + hist_amp + (tot_frames*256*kk) + ((nfram-n_first)*256)) += 1;
						//////////////////////////////////////
						//////////////////////////////////////


						//////////////////////////////////////
						///////// find direction  // only used for direction histograms!!! AM APR2009
						hordir = (dumx - 127)/127;		// scale values from 0 to 255 to -1 to 1
						verdir = (dumy - 127)/127;
						// changes for testing 01 May09 -> probably not relevant here...	
						//hordir = (dumx - 511)/511;		// scale values from 0 to 255 to -1 to 1
						//verdir = (dumy - 511)/511;
		

						//should use sin or cos & trig rules to avoid discontinuities 

						if( hordir > 0 && verdir > 0 )	alpha = 360 * ((tan(verdir/hordir))/(2*pi));
						if( hordir < 0 && verdir > 0 )	alpha = 360 * ((tan(verdir/hordir)+pi)/(2*pi));
						if( hordir < 0 && verdir <= 0 )	alpha = 360 * ((tan(verdir/hordir)+pi)/(2*pi));
						if( hordir > 0 && verdir <= 0 )	alpha = 360 * ((tan(verdir/hordir)+2*pi)/(2*pi));
						if( hordir == 0 && verdir >0 )	alpha = 360/4;
						if( hordir == 0 && verdir <0 )	alpha = 3*(360/4);
						if( hordir == 0 && verdir == 0)	alpha = 361;
						//hist_dir = (int)abs(alpha);	// scale directions to 0 to 255 for histogram bins
						// is above line correct??? -> my suggestion is below: AMAPR2009
						hist_dir = (int)abs(alpha*255/360);	// scale directions to 0 to 255 for histogram bins

						if( hist_dir > 255 )	hist_dir = 255;				
						*(histDir + hist_dir + (tot_frames*256*kk) + ((nfram-n_first)*256)) += 1;
						//////////////////////////////////////
						//////////////////////////////////////

						fprintf(testh, "| %10d ", hist_dir);

						if( (dummy != 0) && (dummy != 1023) && (dummy != 509) && (dummy != 512) && amp>hist_thresh)
						{
							*(histHvalues + dummy + (tot_frames*1024*kk) + ((nfram-n_first)*1024)) += 1;
							*(histHvalues + dummy + (tot_frames*1024*chan_no) + (kk*1024)) += 1;
						}
						new_bin = dummy/4;  // below are histograms with a resolution of 256...
						if((new_bin != 0) && (new_bin != 127) && (new_bin != 255) && amp>hist_thresh)
						{
							*(smallHhist + new_bin + (tot_frames*256*kk) + ((nfram-n_first)*256)) += 1; // running seq
							*(smallHhist + new_bin + (tot_frames*256*chan_no) + (kk*256)) += 1; // cummulative
						}

						if( (dummx != 0) && (dummx != 1023) && (dummx != 509) && (dummx != 512) && amp>hist_thresh)
						{
							*(histVvalues + dummx + (tot_frames*1024*kk) + ((nfram-n_first)*1024)) += 1;
							*(histVvalues + dummx + (tot_frames*1024*chan_no) + (kk*1024)) += 1;
						}
						new_bin = dummx/4;
						if((new_bin != 0) && (new_bin != 127) && (new_bin != 255) && amp>hist_thresh )
						{
							*(smallVhist + new_bin + (tot_frames*256*kk) + ((nfram-n_first)*256)) += 1; // running seq
							*(smallVhist + new_bin + (tot_frames*256*chan_no) + (kk*256)) += 1; // cummulative
						}

						/////////////////////////////////////////
						// populate pointers for 2D histograms///
						/////////////////////////////////////////
						if(/*(dummx != 511) && (dummx != 512) && (dummy != 511) && (dummy != 512) &&*/ 
							(dummx > 0) && (dummx<1023) && (dummy > 0) && (dummy<1023) && amp>hist_thresh) // AM MAY09
						{
							*(twoDhires + dummx + dummy*1024 + (tot_frames*1024*1024*kk) + ((nfram-n_first)*256*256)) += 1;
							*(twoDhires + dummx + dummy*1024 + (tot_frames*1024*1024*chan_no) + (kk*1024*1024)) += 1;
						}
						
						dummx = (int)(dumx/4);	dummy = (int)(dumy/4); // && amp!=0 condition added to exclude zero
						if(/*(dummx != 127) && (dummx != 128) && (dummy != 127) && (dummy != 128) 
							&&*/(dummx > 0) && (dummx<255) && (dummy > 0) && (dummy<255) && amp>hist_thresh) //AM May09
						{
							*(twoDhist + dummx + dummy*256 + (tot_frames*256*256*kk) + ((nfram-n_first)*256*256)) += 1; // hist for 1 frame
							*(twoDhist + dummx + dummy*256 + (tot_frames*256*256*chan_no) + (kk*256*256)) += 1; // global histogram...
						}
						}//---------> end of condition reg==1
				}	// run through ii, x positions
			} // run through jj, y positions
			fprintf(testh,"\n");
			fprintf(testv,"\n");
		}	// end nfram running through frames
	} //end kk, running through channels
m_in_status_hist = _T("LOADED");
	UpdateData(FALSE);
	
	//---code for k-cluster means component separation...
	//May 2009 AM
	//from *(twoDhist)
	//-----------------------------------------------------------


	//////// Add code to save histogram values to be used on other research
	FILE	*histh;
	FILE	*histv;
	FILE	*fullh;
	FILE	*fullv;

	sprintf( out_filnam, "resp\\%s_histh.txt",  in_name_str);
	if ( (histh=fopen(out_filnam,"w")) == NULL )		// test whether ascii file can be opened 
		{ AfxMessageBox( "cannot save hi-res hor histogram file (open?)!" ); return; }	// prevent save without open file
	
	sprintf( out_filnam, "resp\\%s_histv.txt", in_name_str );
	if ( (histv=fopen(out_filnam,"w")) == NULL )		// test whether ascii file can be opened 
		{ AfxMessageBox( "cannot save hi-res ver histogram file (open?)!" ); return; }	// prevent save without open file

	sprintf( out_filnam, "resp\\%s_fullh.txt", in_name_str );
	if ( (fullh=fopen(out_filnam,"w")) == NULL )
		{ AfxMessageBox( "cannot save full hor histogram file (open?)!"); return; }

	sprintf( out_filnam, "resp\\%s_fullv.txt", in_name_str );
	if ( (fullv=fopen(out_filnam,"w")) == NULL )
		{ AfxMessageBox( "cannot save full ver histogram file (open?)!"); return; }

	for (kk=0;kk<chan_no;kk++) // run through channels
	{
		int file = kk+1;
		fprintf(histh, "Start of Channel: %d \n ", file );
		fprintf(histv, "Start of Channel: %d \n ", file );
		for (nfram=n_first; nfram<=n_last; nfram++)  // run through frames in sequence
		{
			fprintf(histh, "Start of Frame: %d \n ", nfram );
			fprintf(histv, "Start of Frame: %d \n ", nfram );
			fprintf(fullh, "Start of Frame: %d \n ", nfram );
			fprintf(fullv, "Start of Frame: %d \n ", nfram );
			for (ii=0;ii<1024;ii++) // run through histogram bins
			{
				dumx = *(histHvalues + ii + (tot_frames*1024*kk) + ((nfram-n_first)*1024));
				dumy = *(histVvalues + ii + (tot_frames*1024*kk) + ((nfram-n_first)*1024));
				dummx = (int)dumx;
				dummy = (int)dumy;
				fprintf(histh, "Bin %3d:%5d ", ii, dummx);
				fprintf(histv, "Bin %3d:%5d ", ii, dummy);

				if (ii<256)
				{
					dumx = *(smallHhist + ii + (tot_frames*256*kk) + ((nfram-n_first)*256));
					dumy = *(smallVhist + ii + (tot_frames*256*kk) + ((nfram-n_first)*256));
					dummx = (int)dumx;
					dummy = (int)dumy;
					fprintf(fullh, "Bin %3d:%5d ", ii, dummx);
					fprintf(fullv, "Bin %3d:%5d ", ii, dummy);
				}
				if(ii == 1023)
				{
					fprintf(histh, "\n");
					fprintf(histv, "\n");
					fprintf(fullh, "\n");
					fprintf(fullv, "\n");
				}
			}
		}
	} // loop through histograms

	fclose(histh);
	fclose(histv);
	fclose(fullh);
	fclose(fullv);
	fclose(testh);
	fclose(testv);
}


void CMy2DMD_DISPLAYDlg::OnShowHist() 
{
	// TODO: Add your control notification handler code here
	// load 2 ascii response data files and convert into response pseudocolour map  
	int dumi, dummi, nfram, ii, jj, kk;
	char out_str[80];
	char in_filnam[100];                
	int tot_frames = n_last-n_first+1;

	int hmax = 0;
	int hmin = 1023;


    int vmax = 0;
	int vmin = 1023;
	int curr_val = 0; // variable to store current histogram value
	int allhist_h = 0;
	int allhist_v = 0;
	int two_val = 0;

	CClientDC ClientDC( this );
	BeginWaitCursor();
	RedrawWindow();			// erase previous image and messages

	FILE *lores;
	FILE *hires;

	char out_filnam[80];
	sprintf( out_filnam, "resp\\test.txt" );
	if ( (lores=fopen(out_filnam,"w")) == NULL )		// test whether ascii file can be opened 
		{ AfxMessageBox( "cannot save chah ascii file (open?)!" ); return; }	// prevent save without open file

	if ( (hires=fopen(out_filnam,"w")) == NULL )		// test whether ascii file can be opened 
		{ AfxMessageBox( "cannot save chah ascii file (open?)!" ); return; }	// prevent save without open file
	
		
		for(kk=0; kk<4; kk++)
		{
			sprintf( in_filnam, "resp/%s_%d.chah", in_name_str, kk );	// generate input filename 
			dumi = DoCheckFile(in_filnam);	// determine first image size
			if (dumi==0)					// and exit if input file not opened 
				{ AfxMessageBox( "cannot load first ascii file (name?)!" ); return; } // and exit if not
		}
		
	num_pixels = sizx*sizy;			// calculate number of pixels
	seq_pixels = num_pixels * (n_last - n_first + 1);

	// prepare data array for horizontal response values 
	ppHvalues = new double [seq_pixels*chan_no];
	pHvalues = new unsigned short int [num_pixels*chan_no];	// allocate space for image on heap
	if ( pHvalues==NULL )							// warning if array doesn't get requested heap space
		{ AfxMessageBox( "Could not create H heap space !" ); return; }
	ppVvalues = new double [seq_pixels*chan_no];
	pVvalues = new unsigned short int [num_pixels*chan_no];	// allocate space for image on heap
	if ( pVvalues==NULL )							// warning if array doesn't get requested heap space
		{ AfxMessageBox( "Could not create H heap space !" ); return; }

	currTwohist = new unsigned short int [num_pixels];		// initialise pointer for histogram of current frame in current channel for display

	currHist = new unsigned short int [num_pixels];		// initialise pointer for histogram of current frame in current channel for display
	disp_pH = new unsigned short int [num_pixels];		// initialise pointer for hor response values in current frame of current channel for display
	if(disp_pH==NULL)
		{ AfxMessageBox( "Could not create image display hor heap !" ); return; }

	disp_pV = new unsigned short int [num_pixels];		// initialise pointer for ver response values in current frame of current channel for display
	if(disp_pV==NULL)
		{ AfxMessageBox( "Could not create image display hor heap !" ); return; }

	n_pImageObject = new CImageObject( "col_24bit.bmp" );
	dumi = n_pImageObject->Stretch( sizx, sizy );
	if ( (n_pImageObject==NULL) || (dumi == 0) )
		{ AfxMessageBox( "Could not create image !" ); return; }
	
	CImagePointProcesses mImagePointProcess( n_pImageObject );

	
	m_pImageObject = new CImageObject( "col_8bit.bmp" );
	dummi = m_pImageObject->Stretch( sizx, sizy );
	if ( (m_pImageObject==NULL) || (dumi == 0) )
		{ AfxMessageBox( "Could not create blank histogram !" ); return; }

		
	CImagePointProcesses nImagePointProcess( m_pImageObject );  
		
	l_pImageObject = new CImageObject( "col_8bit.bmp" );
	dumi = l_pImageObject->Stretch( sizx, sizy );
	if ( (l_pImageObject==NULL) || (dumi == 0) )
		{ AfxMessageBox( "Could not create blank histogram !" ); return; }

	CImagePointProcesses pImagePointProcess( l_pImageObject );  

		dispHist = new unsigned short int [num_pixels*chan_no*2];		// initialise pointer for histogram values of current frame
		dispTwohist = new unsigned short int [num_pixels*chan_no*2];

		int h_max[chan_no], v_max[chan_no], ll, mm;
		int h_amax[chan_no], v_amax[chan_no];
		int amp_max[chan_no], dir_max[chan_no];
		int two_max, two_amax;
		

		// CODE BELOW CHANGED, AM 30APR2009, see detailed annotations......
		for(nfram = n_first; nfram <= n_last; nfram++)
		{
			for(kk = 0; kk < chan_no; kk++)
			{	
					sprintf( in_filnam, "resp/%s_%d.chah", in_name_str, kk );	// generate input filename 
					dumi = DoLoadFile(nfram, kk, 0, in_filnam, &hmin, &hmax);
					if (dumi==0)					// and exit if hor response file not opened
						{ AfxMessageBox( "cannot load current ascii file (name?)!" ); return; } 
			
					sprintf( in_filnam, "resp/%s_%d.chav", in_name_str, kk );	// generate input filename 
					dumi = DoLoadFile(nfram, kk, 1, in_filnam, &vmin, &vmax);
					if (dumi==0)					// and exit if hor response file not opened
						{ AfxMessageBox( "cannot load current ascii file (name?)!" ); return; } 
				

				if(disp_hist == 1)		// when histogram flag is set to show histograms
				{
					h_max[kk]=0; v_max[kk]=0; //initialize maxd search values 	
					amp_max[kk]=0; dir_max[kk]=0;
					if (nfram == n_first) {h_amax[kk] = 0; v_amax[kk] = 0;} //AM May2009
					two_max = 0; two_amax = 0;
					if(hist_type == 0 || hist_type == 2 || hist_type == 3)
					{
						for (ll=0;ll<256;ll++) //find max of current histogram 
						{		
							// calculate vert and hor max of frame by frame histogram

								if (*(smallHhist + ll + (tot_frames*256*kk) + ((nfram-n_first)*256)) > h_max[kk]) 
									h_max[kk] = *(smallHhist + ll + (tot_frames*256*kk) + ((nfram-n_first)*256));
								if (*(smallVhist + ll + (tot_frames*256*kk) + ((nfram-n_first)*256)) > v_max[kk]) 
									v_max[kk] = *(smallVhist + ll + (tot_frames*256*kk) + ((nfram-n_first)*256));
								if (*(histAmp + ll + (tot_frames*256*kk) + ((nfram-n_first)*256)) > amp_max[kk]);
									amp_max[kk] = *(histAmp + ll + (tot_frames*256*kk) + ((nfram-n_first)*256));
								if (*(histDir + ll + (tot_frames*256*kk) + ((nfram-n_first)*256)) > dir_max[kk]);
									dir_max[kk] = *(histDir + ll + (tot_frames*256*kk) + ((nfram-n_first)*256));
								
								if(nfram==n_last) // condition to caclulate max of cummulative histogram
								{ 
								allhist_h = *(smallHhist + ll + 256*kk + tot_frames*256*chan_no);
								if ( allhist_h > h_amax[kk]) 
									{h_amax[kk] = *(smallHhist + tot_frames*256*chan_no + ll + 256*kk);}
							
								allhist_v = *(smallVhist + ll + 256*kk + tot_frames*256*chan_no);
								if (allhist_v > v_amax[kk]) 
								{v_amax[kk] = *(smallVhist + tot_frames*256*chan_no + ll + 256*kk);}
															
								}
							
						} // end of loop to find current max
					}
					if(hist_type == 1) // not in use for the most part AM June 2013
					{
						for (ll=384;ll<640;ll++) //find max of current histogram 
						{	
							int temp = 0;
							temp = *(histHvalues + ll + (tot_frames*1024*kk) + ((nfram-n_first)*1024));
							fprintf(lores, "%8d ", temp);
							// calculate vert and hor max of frame by frame histogram
							if (*(histHvalues + ll + (tot_frames*1024*kk) + ((nfram-n_first)*1024)) > h_max[kk]) 
								h_max[kk] = *(histHvalues + ll + (tot_frames*1024*kk) + ((nfram-n_first)*1024));
							if (*(histVvalues + ll + (tot_frames*1024*kk) + ((nfram-n_first)*1024)) > v_max[kk]) 
								v_max[kk] = *(histVvalues + ll + (tot_frames*1024*kk) + ((nfram-n_first)*1024));
							
							if(nfram==n_last) // condition to caclulate max of cummulative histogram
							{
								allhist_h = *(histHvalues + ll + 1024*kk + tot_frames*1024*chan_no);
								if ( allhist_h > h_amax[kk]) 
								{h_amax[kk] = *(histHvalues + tot_frames*1024*chan_no + ll + 1024*kk);}
							
								allhist_v = *(histVvalues + ll + 1024*kk + tot_frames*1024*chan_no);
								if (allhist_v > v_amax[kk]) 
								{v_amax[kk] = *(histVvalues + tot_frames*1024*chan_no + ll + 1024*kk);}
							}
						} // end of loop to find current max
					}
					
					for(jj=0; jj<256; jj++)			// code to find maximum value in 2D histogram array
					for(ii=0; ii<256; ii++)
					{
						if( hist_type == 0 )
								{
									if(*(twoDhist + ii + jj*256 + ((tot_frames*256*256*kk) + ((nfram-n_first)*256*256))) > two_max)	
										two_max = *(twoDhist + ii + jj*256 + ((tot_frames*256*256*kk) + ((nfram-n_first)*256*256)));
								}
						if( hist_type == 1 )
								{
									if(*(twoDhires + ii + 384 + ((jj+384)*256) + ((tot_frames*1024*1024*kk) + ((nfram-n_first)*1024*1024))) > two_max)	
										two_max = *(twoDhires + ii + 384 + ((jj+384)*256) + ((tot_frames*1024*1024*kk) + ((nfram-n_first)*1024*1024)));
								}
					
						if (nfram == n_last && hist_type == 0) // new AM MAY2009, again June 2013
						{
								if(*(twoDhist + ii + jj*256 + tot_frames*256*256*chan_no + 
									256*256*kk)> two_amax)	
								two_amax = *(twoDhist + ii + jj*256 + tot_frames*256*256*chan_no + kk*256*256);
						}

						if (nfram == n_last && hist_type == 1) // new AM MAY2009, again June 2013
						{
								if(*(twoDhires + ii + 384 + (jj+384)*256 + tot_frames*1024*1024*chan_no + 
									1024*1024*kk)> two_amax)	
								two_amax = *(twoDhires + ii + 384 + (jj+384)*sizx + tot_frames*1024*1024*chan_no + kk*1024*1024);
						}
					}// end of run through ii and jj
					
					//below, running through histogram pointers
					for(jj=0; jj<sizy; jj++)
					{
						for(ii=0; ii<sizx; ii++) // MAY 2009. i have reversed the ii and jj indeces to make histogram diplay
						{
							if(hist_type == 0 && ii<256 && jj<256) // restrict histogram within pointers...
								two_val = *(twoDhist + jj + ii*256 + (tot_frames*256*256*kk) + ((nfram-n_first)*256*256));
							if(hist_type == 1 && ii<256 && jj<256)
								two_val = *(twoDhires + jj + 384 + ((ii+384)*256) + (tot_frames*1024*1024*kk) + ((nfram-n_first)*1024*1024));
							
							if( two_val == 0 && ii<256 && jj<256 )	
								*(dispTwohist + ii + jj*sizx + (kk*num_pixels)) = 765;
								// above pointer based on spatial extent sizx not 256pix 06/13 AM
							else 
							{
								// ANDY, you have a log compression in this histogram????-> AMAPR2009???
								//two_val = 200-((int)(200*log((double)(two_val)))/log((double)(two_max)));
								//two_val = 1; // testing may09
								// i am trying a non log version!! AM MAy2009
								//two_val = 765-((int)(765*(double)(two_val))/(double)(two_max));
								
								two_val = 765-(0.5+0.5*(int)(765*(double)(two_val))/(double)(two_max));
								*(dispTwohist + ii + jj*sizx + (kk*num_pixels)) = two_val;
								// again above display pointer is bigger than hist calc pointer
							}
							// below is not really used.. no high res histograms...
							if( hist_type == 2)	// code for loading amplitude histogram for display
							{
								if( amp_max[kk] == 0 )	curr_val = (int)(sizx * (*(histAmp + ii + (tot_frames*256*kk) + ((nfram-n_first)*256))));
								else					curr_val = (int)(sizx * (*(histAmp + ii + (tot_frames*256*kk) + ((nfram-n_first)*256)))/amp_max[kk]);
								if( curr_val >= (sizx - jj) )		*(dispHist + (kk*num_pixels) + jj*sizx + ii) = 127;
								else								*(dispHist + (kk*num_pixels) + jj*sizx + ii) = 765;
							}
							if( hist_type == 2) // code for loading direction histogram for display
							{
								if( dir_max[kk] == 0 )	curr_val = (int)(sizx * (*(histDir + ii + (tot_frames*256*kk) + ((nfram-n_first)*256))));
								else					curr_val = (int)(sizx * (*(histDir + ii + (tot_frames*256*kk) + ((nfram-n_first)*256)))/dir_max[kk]);
								if( curr_val >= (sizx - jj) )		*(dispTwohist + (kk*num_pixels) + jj*sizx + ii) = 127;
								else								*(dispTwohist + (kk*num_pixels) + jj*sizx + ii) = 765;
							}
							if( hist_type == 1 || hist_type == 0 )
							{
								if(jj< 128) 
								{
									if( hist_type == 1 && h_max[kk] !=0)
										curr_val = (int)(127 * (*(histHvalues + ii + 384 + (tot_frames*1024*kk) + ((nfram-n_first)*1024)))/h_max[kk]);
									if( hist_type == 0)
									{
										if(h_max[kk] == 0)		curr_val = (int)(127 * (*(smallHhist + ii + (tot_frames*256*kk) + ((nfram-n_first)*256))));
										else					curr_val = (int)(127 * (*(smallHhist + ii + (tot_frames*256*kk) + ((nfram-n_first)*256)))/h_max[kk]);
									}
									if(curr_val >= ((sizx/2) - jj))
									{	
										*(dispHist + (kk*num_pixels) + jj*sizx + ii) = 127;
									}
									else
										{*(dispHist + (kk*num_pixels) + jj*sizx + ii) = 765;}
								}
								if(jj>127)
								{
									if( hist_type == 1 && v_max[kk] !=0)
										curr_val = (int)(127 * (*(histVvalues + ii + 384 + (tot_frames*1024*kk) + ((nfram-n_first)*1024)))/v_max[kk]);
									if( hist_type == 0)
										if(v_max[kk] == 0)		curr_val = (int)(127 * (*(smallVhist + ii + (tot_frames*256*kk) + ((nfram-n_first)*256))));
										else					curr_val = (int)(127 * (*(smallVhist + ii + (tot_frames*256*kk) + ((nfram-n_first)*256)))/v_max[kk]);
									if(curr_val >= (sizx - jj))
									{
										*(dispHist + (kk*num_pixels) + jj*sizx + ii) = 127;
									}
									else
										{*(dispHist + (kk*num_pixels) + jj*sizx + ii) = 765;}
								}
								if((jj<129 && jj>126) || (ii<129 && ii>126) 
									|| ii<3 || ii>253 || jj<3 || jj>253 ) // add boundaries
								{
									*(dispHist + (kk*num_pixels) + jj*sizx + ii) = 0;
									//below commented out because we want to see the axes of the 2d plots
									// i.e. the histogram responses at the axes... AM MAY2009, reintroduced 06/13
									*(dispTwohist + (kk*num_pixels) + jj*sizx + ii) = 127;
								}
							

								//######## 03DEC08################## last updated.... June 2013
								if (nfram==n_last /*am june 13 avoid step*/) // when you get to the last frame AM03Dec08... again APR09
								{
	                                //------------- AM MAY2009_> changing order of ii and jj to correct histogram
									if( hist_type == 0 && ii<256 && jj<256)two_val = *(twoDhist + jj + ii*256 + 
										tot_frames*256*256*chan_no + 256*256*kk); // this referes to histogram
									if( hist_type == 1 )two_val = *(twoDhires + jj + 384 + ((ii+384)*256) 
										+ (tot_frames*1024*1024*chan_no + 256*256*kk)); // not yet working for high res
									if( two_val == 0)	
										*(dispTwohist + ii + jj*sizx + chan_no*num_pixels + kk*num_pixels) = 765;
									else 
									{ // last frame, show histograms
										//two_val = 765-((int)(765*log((double)(two_val)))/log((double)(two_amax)));
										// i am trying a non log version!! AM MAy2009
										//two_val = 765-((int)(765*(double)(two_val))/(double)(two_max));
										// also trying log verson with all non zeros darker!
										//two_val = 200-((int)(200*log((double)(two_val))/log((double)(two_amax))));
										two_val = 765-(0.5+0.5*(int)(765*(double)(two_val))/(double)(two_amax));
										*(dispTwohist + ii + jj*sizx + chan_no*num_pixels + kk*num_pixels) = two_val;
									}
									
									//------------END OF NEWEST CODE AM MAY09 for populating the 
									if(jj<127)
									{
										if( hist_type == 0 && h_amax[kk]!=0)
											curr_val = (int)(127 * (*(smallHhist + ii + tot_frames*256*chan_no + kk*256))/h_amax[kk]);
										if( hist_type == 1 && h_amax[kk]!=0)
											curr_val = (int)(127 * (*(histHvalues + ii + 384 + tot_frames*1024*chan_no + kk*1024))/h_amax[kk]);
										if(curr_val >= ((sizx/2) - jj))
										{
											*(dispHist + chan_no*num_pixels + kk*num_pixels + jj*sizx + ii) = 127;
										}
										else
											{*(dispHist + chan_no*num_pixels + (kk*num_pixels) + jj*sizx + ii) = 765;}
									}
									if(jj>128)
									{
										if( hist_type == 0 && v_amax[kk]!=0)	// for full histogram 
											curr_val = (int)(127 * (*(smallVhist + ii + tot_frames*256*chan_no + kk*256))/v_amax[kk]);
										if( hist_type == 1 && v_amax[kk]!=0) // for hi-res histogram
											curr_val = (int)(127 * (*(histVvalues + ii + 384 + tot_frames*1024*chan_no + kk*1024))/v_amax[kk]);
										if(curr_val >= (sizx - jj))
										{
											*(dispHist + chan_no*num_pixels + kk*num_pixels + jj*sizx + ii) = 127;
										}
										else
											{*(dispHist + chan_no*num_pixels + kk*num_pixels + jj*sizx + ii) = 765;}
									}
									if((jj<129 && jj>126) || (ii<129 && ii>126)
										|| ii<3 || ii>253 || jj<3 || jj>253 )
										{
											*(dispHist + chan_no*num_pixels + kk*num_pixels + jj*sizx + ii) = 0;
											*(dispTwohist + chan_no*num_pixels + (kk*num_pixels) + jj*sizx + ii) = 127;	
										}	
								} //end of last frame condition..... AM03Dec08 
							}
						}		// end of loop through columns
					}		// end of loop through rows
				}		// end of if statement when histogram flag is set
			}		// end of loop through channels
			
			//RedrawWindow();
			sprintf( out_str, "resp/%s.chah&chav frame %d: %dx%d min|max %d|%d&%d|%d", 
						in_name_str, nfram, sizx,sizy, hmin/4, hmax/4, vmin/4, vmax/4 );
			ClientDC.TextOut(550, 45, out_str);

			for(kk = 0; kk < chan_no; kk++)
			{
				for(ll=0; ll<num_pixels; ll++)
				{
					*(disp_pH + ll) = *(pHvalues + ll + (kk*num_pixels));
					*(disp_pV + ll) = *(pVvalues + ll + (kk*num_pixels));
				}
				if( disp_hist == 1)
				{
					for(ll=0; ll<num_pixels; ll++)	
					{
						*(currHist + ll) = *(dispHist + ll + (kk*num_pixels));
						*(currTwohist + ll) = *(dispTwohist + ll + (kk*num_pixels));
					}
					// not sure if this will work....
					nImagePointProcess.Dump8bitData( currHist,sizx, sizy );
					pImagePointProcess.Dump8bitData( currTwohist,sizx, sizy );
				}

				mImagePointProcess.Dump24bitData( disp_pH, disp_pV, sizx, sizy );
				if( thresh != 255.0 )
					mImagePointProcess.Dump24bitDataNL( thresh, disp_pH, disp_pV, sizx, sizy );


				n_pImageObject->Draw( &ClientDC, 2+kk*(sizx+2), 152);
				
				if(disp_hist == 1)
				{
					m_pImageObject->Draw( &ClientDC, 2+kk*(sizx+2), 410);
					l_pImageObject->Draw( &ClientDC, 2+kk*(sizx+2), 670);
				}
				if(kk==0)
					{
						sprintf( out_str, "resp/%s%d.hor&ver: %dx%d ", in_name_str, nfram, sizx, sizy);
						ClientDC.TextOut(5, 118, out_str);
					}			
				sprintf( out_str, "Chan no: %d", kk+1);
				ClientDC.TextOut(2+kk*(sizx+2), 134, out_str);	

				if(disp_hist == 1 && hist_type == 0)	// display H and V labels on histograms and max values
				{
					sprintf( out_str, "H: %d", h_max[kk] );
					ClientDC.TextOut(10+kk*(sizx+2), 420, out_str);
					sprintf( out_str, "V: %d", v_max[kk] );
					ClientDC.TextOut(10+kk*(sizx+2), 636, out_str);
				}
				
				if (nfram==n_last && disp_hist == 1 && hist_type == 0) // do only when on the last fram
				{
						if (kk==0) Sleep (1500); // delay before showing cummulative histogram
						
						for(ll=0; ll<num_pixels; ll++) // populate pointer with cummulative histogram	
						{
						*(currHist + ll) = *(dispHist + chan_no*num_pixels + ll + (kk*num_pixels));
						*(currTwohist + ll) = *(dispTwohist + chan_no*num_pixels + ll + (kk*num_pixels));}
						
						nImagePointProcess.Dump8bitData( currHist, sizx, sizy );
						pImagePointProcess.Dump8bitData( currTwohist, sizx, sizy );
											
						if(disp_hist == 1)
						{
							m_pImageObject->Draw( &ClientDC, 2+kk*(sizx+2), 410);
							l_pImageObject->Draw( &ClientDC, 2+kk*(sizx+2), 670);
						}
						
						// label the cummulative histograms...
						sprintf( out_str, "H - all: %d", h_amax[kk] );
						ClientDC.TextOut(10+kk*(sizx+2), 420, out_str);
						sprintf( out_str, "V - all: %d", v_amax[kk] );
						ClientDC.TextOut(10+kk*(sizx+2), 636, out_str);
						}

			}  // end loop through channels for display
		}	// end loop through image sequence
	
	
	fclose(fi1);	fclose(fi2);	fclose(fi3);	fclose(fi4);
	fclose(fiv1);	fclose(fiv2);	fclose(fiv3);	fclose(fiv4);
	fclose(lores);	fclose(hires);
	delete pHvalues;	pHvalues = 0;
	delete pVvalues;	pVvalues = 0;
	delete dispHist;	dispHist = 0;
	delete disp_pH;		disp_pH = 0;
	delete disp_pV;		disp_pV = 0;
	EndWaitCursor();	
}


void CMy2DMD_DISPLAYDlg::OnBnClickedCalccombh()
{
	// TODO: Add your control notification handler code here
	// AM May2011 simple histogram calculation for the combined local responses... not MC
int ii, jj, kk, nfram, ll, new_bin, dummy, dummx, dumi, twoD; // declare twoD up here??? AM MAY09
	double dumy, dumx;
	double resph, respv;
	char out_filnam[80];
	int tot_frames = n_last - n_first + 1;
	int hist_thresh = 50; //histogram threshold, nonlinearity to reduce zero responses AM MAY2009
	//////////////////////////////////////////////////
	///////// for calculating amplitude histogram
	double amp;
	double amp_max;
	int hist_amp;
	///////////////////////////////////////////////////
	///////// for calculating direction histogram
	double hordir;
	double verdir;
	int hist_dir;

	FILE *testh;
	sprintf( out_filnam, "resp\\%s_hor.txt",  in_name_str);
	if ( (testh=fopen(out_filnam,"w")) == NULL )		// test whether ascii file can be opened 
		{ AfxMessageBox( "cannot save hi-res hor histogram file (open?)!" ); return; }	// prevent save without open file

	FILE *testv;
	sprintf( out_filnam, "resp\\%s_ver.txt",  in_name_str);
	if ( (testv=fopen(out_filnam,"w")) == NULL )		// test whether ascii file can be opened 
		{ AfxMessageBox( "cannot save hi-res hor histogram file (open?)!" ); return; }	// prevent save without open file


	m_in_status_hist = _T("...CALCULATING");
	UpdateData(FALSE);
	RedrawWindow();
	disp_hist = 1;
	
	//use the array below just to store the values of the maximum of the combinations MAY11
	resp_max[0][0] = 0.0; /*addition*/ 
	resp_max[0][1] = 0.0; /*addition*/ 
	
	for (int ii=0; ii<seq_pixels; ii++) // find response maximum values - may be useful for later scaling
	{
		// two below: addition of channels
		resph = *(ppHvalues + ii); //horizontal responses
		if( (resph > resp_max[0][0]) && (resph != 0) && (resph != 1023))	
			resp_max[0][0] = abs(resph);

		respv = *(ppVvalues + ii); // vertical responses
		if( (respv > resp_max[0][1]) && (respv != 0) && (respv != 1023))	
			resp_max[0][1] = abs(respv);
		
	}
	

	//************ Initialise arrays for calculating histogram values ************
	//2 used for 2 types of combination, simple addition and edge weighted
	histHvalues = new int [(1024 * tot_frames) + (1024)];
	if(histHvalues==NULL)
		{ AfxMessageBox( "Could not create hor histogram heap space!" ); return; }
	histVvalues = new int [(1024 * tot_frames) + (1024)];
	if( histVvalues==NULL)
		{ AfxMessageBox( "Could not create ver histogram heap space!" ); return; }
	smallHhist = new int [(256 * tot_frames) + (256)];
	if( smallHhist==NULL )
		{ AfxMessageBox( "Could not create hor scaled histogram heap space!" ); return; }
	smallVhist = new int [(256 * tot_frames) + (256)];
	if( smallVhist==NULL )
		{ AfxMessageBox( "Could not create ver scaled histogram heap space!" ); return; }
	twoDhist = new unsigned long int [(256*256*tot_frames) + (256*256)];
	if( twoDhist==NULL ) // AM MAY2011 - unsigned short too small - ANDY!!!
		{ AfxMessageBox( "Could not create full 2D histogram heap speace!" ); return; }
	twoDhires = new unsigned long int [(1024*1024*tot_frames) + (1024*1024)];
	if( twoDhires==NULL ) // AM MAY2011 - unsigned short too small!!!
		{ AfxMessageBox( "Could no create hi-res 2D histogram heap space!" ); return; }
	histAmp = new int [256*tot_frames];
	if( histAmp==NULL )
		{ AfxMessageBox( "Could not create amplitude histogram heap space!" ); return; }
	histDir = new int [256*tot_frames];
	if( histDir==NULL )
		{ AfxMessageBox( "Could not create direction histogram heap space!" ); return; }
	//*****************************************************************************
	int reg = 0;
	for (ii=0;ii<1024;ii++) // initialize histogram arrays to zero! Am02DEC08
	for (kk=0;kk<1;kk++) // either addition or edge weighted....
	for (nfram=n_first; nfram<=n_last; nfram++)
	{
		*(histHvalues + ii + tot_frames*1024*kk + (nfram-n_first)*1024) = 0;  
		*(histVvalues + ii + tot_frames*1024*kk + (nfram-n_first)*1024) = 0;  
		
		for(twoD=0; twoD<1024; twoD++)
		*(twoDhires + ii*1024 + twoD + tot_frames*1024*1024*kk + (nfram-n_first)*1024*1024) = 0;
		
		
		if (nfram==n_last)
		{
			for(twoD=0; twoD<1024; twoD++)
				*(twoDhires + ii*1024 + twoD + tot_frames*1024*1024*1 + kk*1024*1024) = 0;
		}


		if (ii<256) 
		{
			*(smallHhist + ii + (tot_frames*256*kk) + ((nfram-n_first)*256)) = 0;
			*(smallVhist + ii + (tot_frames*256*kk) + ((nfram-n_first)*256)) = 0;
			*(histAmp + ii + (tot_frames*256*kk) + ((nfram-n_first)*256)) = 0;
			*(histDir + ii + (tot_frames*256*kk) + ((nfram-n_first)*256)) = 0;
			for(twoD=0; twoD<256; twoD++)
				*(twoDhist + ii*256 + twoD + (tot_frames*256*256*kk) + ((nfram-n_first)*256*256)) = 0;
			if (nfram==n_last) // AM MAY 2009 - for final frame
			{
				*(smallHhist + ii + tot_frames*256*1 + kk*256) = 0;
				*(smallVhist + ii + tot_frames*256*1 + kk*256) = 0;
				for(twoD=0; twoD<256; twoD++)
				*(twoDhist + ii*256 + twoD + tot_frames*256*256*1 + kk*256*256) = 0;
			}
		}
		
		// This point in the program should set pointers to zero for the realtime histograms
		
	} // end of initialization

	for(kk=0; kk<1; kk++)	//2 June13?	// run through both combination types, calculate histograms May11
	{
		amp_max = sqrt((1023.0*1023.0)*2.0);
		for(nfram=n_first; nfram<=n_last; nfram++)
		{
			long indexshift = ((kk*seq_pixels) + (nfram-n_first) * num_pixels);
			for(jj=0; jj<sizy; jj++)
			{
				for(ii=0; ii<sizx; ii++)
				{
					reg =  ImageRegion(kk,ii,jj); // find whether current location is inside or outside filters
					// above AM May2009
					if (reg==1)
						{
						dumy = *(ppHvalues + jj*sizx + ii + indexshift); // find current hor response
						dumx = *(ppVvalues + jj*sizx + ii + indexshift); // find current ver response

						dummy = (int)dumy;
						dummx = (int)dumx;

						//////////////////////////////////////
						///////// find amplitude
						amp = sqrt((dumy*dumy) + (dumx*dumx));
						hist_amp = ((amp/amp_max)*255.0); // AM MAY11
						*(histAmp + hist_amp + (tot_frames*256*kk) + ((nfram-n_first)*256)) += 1;
						//////////////////////////////////////
						//////////////////////////////////////


						//////////////////////////////////////
						///////// find direction  // only used for direction histograms!!! AM APR2009
						hordir = (dumx - 127)/127;		// scale values from 0 to 255 to -1 to 1
						verdir = (dumy - 127)/127;
					
						//should use sin or cos & trig rules to avoid discontinuities 
						if( hordir > 0 && verdir > 0 )	alpha = 360 * ((tan(verdir/hordir))/(2*pi));
						if( hordir < 0 && verdir > 0 )	alpha = 360 * ((tan(verdir/hordir)+pi)/(2*pi));
						if( hordir < 0 && verdir <= 0 )	alpha = 360 * ((tan(verdir/hordir)+pi)/(2*pi));
						if( hordir > 0 && verdir <= 0 )	alpha = 360 * ((tan(verdir/hordir)+2*pi)/(2*pi));
						if( hordir == 0 && verdir >0 )	alpha = 360/4;
						if( hordir == 0 && verdir <0 )	alpha = 3*(360/4);
						if( hordir == 0 && verdir == 0)	alpha = 361;
						//hist_dir = (int)abs(alpha);	// scale directions to 0 to 255 for histogram bins
						// is above line correct??? -> my suggestion is below: AMAPR2009
						hist_dir = (int)abs(alpha*255/360);	// scale directions to 0 to 255 for histogram bins

						if( hist_dir > 255 )	hist_dir = 255;				
						*(histDir + hist_dir + (tot_frames*256*kk) + ((nfram-n_first)*256)) += 1;
						//////////////////////////////////////
						//////////////////////////////////////

						fprintf(testh, "| %10d ", hist_dir);

						if( (dummy != 0) && (dummy != 1023) && (dummy != 509) && (dummy != 512) && amp>hist_thresh)
						{
							*(histHvalues + dummy + (tot_frames*1024*kk) + ((nfram-n_first)*1024)) += 1;
							*(histHvalues + dummy + (tot_frames*1024) + (kk*1024)) += 1;
						}
						new_bin = dummy/4;
						if((new_bin != 0) && (new_bin != 127) && (new_bin != 255) && amp>hist_thresh)
						{
							*(smallHhist + new_bin + (tot_frames*256*kk) + ((nfram-n_first)*256)) += 1; // running seq
							*(smallHhist + new_bin + (tot_frames*256) + (kk*256)) += 1; // cummulative at end
						}

						if( (dummx != 0) && (dummx != 1023) && (dummx != 509) && (dummx != 512) && amp>hist_thresh)
						{
							*(histVvalues + dummx + (tot_frames*1024*kk) + ((nfram-n_first)*1024)) += 1;
							*(histVvalues + dummx + (tot_frames*1024) + (kk*1024)) += 1;
						}
						new_bin = dummx/4;
						if((new_bin != 0) && (new_bin != 127) && (new_bin != 255) && amp>hist_thresh )
						{
							*(smallVhist + new_bin + (tot_frames*256*kk) + ((nfram-n_first)*256)) += 1; // running seq
							*(smallVhist + new_bin + (tot_frames*256) + (kk*256)) += 1; // cummulative
						}

						///////////////////////////////////////
						// populate pointers for 2D histograms
						///////////////////////////////////////
						if(/*(dummx != 511) && (dummx != 512) && (dummy != 511) && (dummy != 512) &&*/ 
							(dummx > 0) && (dummx<1023) && (dummy > 0) && (dummy<1023) && amp>hist_thresh) // AM MAY09
						{
							*(twoDhires + dummx + dummy*1024 + (tot_frames*1024*1024*kk) + ((nfram-n_first)*256*256)) += 1;
							*(twoDhires + dummx + dummy*1024 + (tot_frames*1024*1024) + (kk*1024*1024)) += 1;
						}
						dummx = (int)(dumx/4);	dummy = (int)(dumy/4); // && amp!=0 condition added to exclude zero
						if(/*(dummx != 127) && (dummx != 128) && (dummy != 127) && (dummy != 128) 
							&&*/(dummx > 0) && (dummx<255) && (dummy > 0) && (dummy<255) && amp>hist_thresh) //AM May09
						{
							*(twoDhist + dummx + dummy*256 + (tot_frames*256*256*kk) + ((nfram-n_first)*256*256)) += 1;
							*(twoDhist + dummx + dummy*256 + (tot_frames*256*256) + (kk*256*256)) += 1;
						}
						}//---------> end of condition reg==1
				}	// run through ii, x positions
			} // run through jj, y positions
			fprintf(testh,"\n");
			fprintf(testv,"\n");
		}	// end nfram running through frames
	} //end kk, running through channels
m_in_status_hist = _T("LOADED");
	UpdateData(FALSE);
	
	//---code for k-cluster means component separation...
	//May 2009 AM, and May 2011 - removed, not ready yet...
	//from *(twoDhist)
	//-----------------------------------------------------------


	//////// Add code to save histogram values to be used on other research
	FILE	*histh;
	FILE	*histv;
	FILE	*fullh;
	FILE	*fullv;

	sprintf( out_filnam, "resp\\%s_histh.txt",  in_name_str);
	if ( (histh=fopen(out_filnam,"w")) == NULL )		// test whether ascii file can be opened 
		{ AfxMessageBox( "cannot save hi-res hor histogram file (open?)!" ); return; }	// prevent save without open file
	
	sprintf( out_filnam, "resp\\%s_histv.txt", in_name_str );
	if ( (histv=fopen(out_filnam,"w")) == NULL )		// test whether ascii file can be opened 
		{ AfxMessageBox( "cannot save hi-res ver histogram file (open?)!" ); return; }	// prevent save without open file

	sprintf( out_filnam, "resp\\%s_fullh.txt", in_name_str );
	if ( (fullh=fopen(out_filnam,"w")) == NULL )
		{ AfxMessageBox( "cannot save full hor histogram file (open?)!"); return; }

	sprintf( out_filnam, "resp\\%s_fullv.txt", in_name_str );
	if ( (fullv=fopen(out_filnam,"w")) == NULL )
		{ AfxMessageBox( "cannot save full ver histogram file (open?)!"); return; }

	for (kk=0;kk<1;kk++) // run through channels
	{
		int file = kk+1;
		fprintf(histh, "Start of Channel: %d \n ", file );
		fprintf(histv, "Start of Channel: %d \n ", file );
		for (nfram=n_first; nfram<=n_last; nfram++)  // run through frames in sequence
		{
			fprintf(histh, "Start of Frame: %d \n ", nfram );
			fprintf(histv, "Start of Frame: %d \n ", nfram );
			fprintf(fullh, "Start of Frame: %d \n ", nfram );
			fprintf(fullv, "Start of Frame: %d \n ", nfram );
			for (ii=0;ii<1024;ii++) // run through histogram bins
			{
				dumx = *(histHvalues + ii + (tot_frames*1024*kk) + ((nfram-n_first)*1024));
				dumy = *(histVvalues + ii + (tot_frames*1024*kk) + ((nfram-n_first)*1024));
				dummx = (int)dumx;
				dummy = (int)dumy;
				fprintf(histh, "Bin %3d:%5d ", ii, dummx);
				fprintf(histv, "Bin %3d:%5d ", ii, dummy);

				if (ii<256)
				{
					dumx = *(smallHhist + ii + (tot_frames*256*kk) + ((nfram-n_first)*256));
					dumy = *(smallVhist + ii + (tot_frames*256*kk) + ((nfram-n_first)*256));
					dummx = (int)dumx;
					dummy = (int)dumy;
					fprintf(fullh, "Bin %3d:%5d ", ii, dummx);
					fprintf(fullv, "Bin %3d:%5d ", ii, dummy);
				}
				if(ii == 1023)
				{
					fprintf(histh, "\n");
					fprintf(histv, "\n");
					fprintf(fullh, "\n");
					fprintf(fullv, "\n");
				}
			}
		}
	} // loop through histograms

	fclose(histh);
	fclose(histv);
	fclose(fullh);
	fclose(fullv);
	fclose(testh);
	fclose(testv);

}

void CMy2DMD_DISPLAYDlg::OnBnClickedShowcombh()
{
	// TODO: Add your control notification handler code here
	// AM May2011 simple display of the histograms for the combined local responses... not MC
	int dumi, dummi, nfram, ii, jj, kk;
	char out_str[80];
	char in_filnam[100];                
	int tot_frames = n_last-n_first+1;

	int hmax = 0;
	int hmin = 1023;


    int vmax = 0;
	int vmin = 1023;
	int curr_val = 0; // variable to store current histogram value
	int allhist_h = 0;
	int allhist_v = 0;
	int two_val = 0;

	CClientDC ClientDC( this );
	BeginWaitCursor();
	RedrawWindow();			// erase previous image and messages

	FILE *lores;
	FILE *hires;

	char out_filnam[80];
	sprintf( out_filnam, "resp\\test.txt" );
	if ( (lores=fopen(out_filnam,"w")) == NULL )		// test whether ascii file can be opened 
		{ AfxMessageBox( "cannot save chah ascii file (open?)!" ); return; }	// prevent save without open file

	if ( (hires=fopen(out_filnam,"w")) == NULL )		// test whether ascii file can be opened 
		{ AfxMessageBox( "cannot save chah ascii file (open?)!" ); return; }	// prevent save without open file
	
		
		for(kk=0; kk<1; kk++)
		{
			sprintf( in_filnam, "resp/%s_%d.chah", in_name_str, kk );	// generate input filename 
			dumi = DoCheckFile(in_filnam);	// determine first image size
			if (dumi==0)					// and exit if input file not opened 
				{ AfxMessageBox( "cannot load first ascii file (name?)!" ); return; } // and exit if not
		}
		
	num_pixels = sizx*sizy;			// calculate number of pixels
	seq_pixels = num_pixels * (n_last - n_first + 1);

	// prepare data array for horizontal response values 
	ppHvalues = new double [seq_pixels*1];
	pHvalues = new unsigned short int [num_pixels*1];	// allocate space for image on heap
	if ( pHvalues==NULL )							// warning if array doesn't get requested heap space
		{ AfxMessageBox( "Could not create H heap space !" ); return; }
	ppVvalues = new double [seq_pixels*1];
	pVvalues = new unsigned short int [num_pixels*1];	// allocate space for image on heap
	if ( pVvalues==NULL )							// warning if array doesn't get requested heap space
		{ AfxMessageBox( "Could not create H heap space !" ); return; }

	currTwohist = new unsigned short int [num_pixels];		// initialise pointer for histogram of current frame in current channel for display

	currHist = new unsigned short int [num_pixels];		// initialise pointer for histogram of current frame in current channel for display
	disp_pH = new unsigned short int [num_pixels];		// initialise pointer for hor response values in current frame of current channel for display
	if(disp_pH==NULL)
		{ AfxMessageBox( "Could not create image display hor heap !" ); return; }

	disp_pV = new unsigned short int [num_pixels];		// initialise pointer for ver response values in current frame of current channel for display
	if(disp_pV==NULL)
		{ AfxMessageBox( "Could not create image display hor heap !" ); return; }

	n_pImageObject = new CImageObject( "col_24bit.bmp" );
	dumi = n_pImageObject->Stretch( sizx, sizy );
	if ( (n_pImageObject==NULL) || (dumi == 0) )
		{ AfxMessageBox( "Could not create image !" ); return; }
	
	CImagePointProcesses mImagePointProcess( n_pImageObject );

	
	m_pImageObject = new CImageObject( "col_8bit.bmp" );
	dummi = m_pImageObject->Stretch( sizx, sizy );
	if ( (m_pImageObject==NULL) || (dumi == 0) )
		{ AfxMessageBox( "Could not create blank histogram !" ); return; }

		
	CImagePointProcesses nImagePointProcess( m_pImageObject );  
		
	l_pImageObject = new CImageObject( "col_8bit.bmp" );
	dumi = l_pImageObject->Stretch( sizx, sizy );
	if ( (l_pImageObject==NULL) || (dumi == 0) )
		{ AfxMessageBox( "Could not create blank histogram !" ); return; }

	CImagePointProcesses pImagePointProcess( l_pImageObject );  

		dispHist = new unsigned short int [num_pixels*2];		// initialise pointer for histogram values of current frame
		dispTwohist = new unsigned short int [num_pixels*2];

		int h_max[2], v_max[2], ll, mm;
		int h_amax[2], v_amax[2];
		int amp_max[2], dir_max[2];
		int two_max, two_amax;
		
		// CODE BELOW CHANGED, AM 30APR2009, see detailed annotations......
		for(nfram = n_first; nfram <= n_last; nfram++)
		{
			for(kk = 0; kk <1; kk++) //kk? shouldnt it be 2? 
			{	
					sprintf( in_filnam, "resp/%s_%d.chah", in_name_str, kk );	// generate input filename 
					dumi = DoLoadFile(nfram, kk, 0, in_filnam, &hmin, &hmax);
					if (dumi==0)					// and exit if hor response file not opened
						{ AfxMessageBox( "cannot load current ascii file (name?)!" ); return; } 
			
					sprintf( in_filnam, "resp/%s_%d.chav", in_name_str, kk );	// generate input filename 
					dumi = DoLoadFile(nfram, kk, 1, in_filnam, &vmin, &vmax);
					if (dumi==0)					// and exit if hor response file not opened
						{ AfxMessageBox( "cannot load current ascii file (name?)!" ); return; } 
				

				if(disp_hist == 1)	// when histogram flag is set to show histograms
				{
					h_max[kk]=0; v_max[kk]=0; //initialize maxd search values 	
					amp_max[kk]=0; dir_max[kk]=0;
					if (nfram == n_first) {h_amax[kk] = 0; v_amax[kk] = 0;} //AM May2009
					two_max = 0; two_amax = 0;
					if(hist_type == 0 || hist_type == 2 || hist_type == 3)
					{
						for (ll=0;ll<256;ll++) //find max of current histogram 
						{		
							// calculate vert and hor max of frame by frame histogram

								if (*(smallHhist + ll + (tot_frames*256*kk) + ((nfram-n_first)*256)) > h_max[kk]) 
									h_max[kk] = *(smallHhist + ll + (tot_frames*256*kk) + ((nfram-n_first)*256));
								if (*(smallVhist + ll + (tot_frames*256*kk) + ((nfram-n_first)*256)) > v_max[kk]) 
									v_max[kk] = *(smallVhist + ll + (tot_frames*256*kk) + ((nfram-n_first)*256));
								if (*(histAmp + ll + (tot_frames*256*kk) + ((nfram-n_first)*256)) > amp_max[kk]);
									amp_max[kk] = *(histAmp + ll + (tot_frames*256*kk) + ((nfram-n_first)*256));
								if (*(histDir + ll + (tot_frames*256*kk) + ((nfram-n_first)*256)) > dir_max[kk]);
									dir_max[kk] = *(histDir + ll + (tot_frames*256*kk) + ((nfram-n_first)*256));
								
								if(nfram==n_last) // condition to caclulate max of cummulative histogram
								{ 
								allhist_h = *(smallHhist + ll + 256*kk + tot_frames*256*1);
								if ( allhist_h > h_amax[kk]) 
									{h_amax[kk] = *(smallHhist + tot_frames*256*1 + ll + 256*kk);}
							
								allhist_v = *(smallVhist + ll + 256*kk + tot_frames*256*1);
								if (allhist_v > v_amax[kk]) 
								{v_amax[kk] = *(smallVhist + tot_frames*256*1 + ll + 256*kk);}
															
								}
							
						} // end of loop to find current max
					}
					if(hist_type == 1)
					{
						for (ll=384;ll<640;ll++) //find max of current histogram 
						{	
							int temp = 0;
							temp = *(histHvalues + ll + (tot_frames*1024*kk) + ((nfram-n_first)*1024));
							fprintf(lores, "%8d ", temp);
							// calculate vert and hor max of frame by frame histogram
							if (*(histHvalues + ll + (tot_frames*1024*kk) + ((nfram-n_first)*1024)) > h_max[kk]) 
								h_max[kk] = *(histHvalues + ll + (tot_frames*1024*kk) + ((nfram-n_first)*1024));
							if (*(histVvalues + ll + (tot_frames*1024*kk) + ((nfram-n_first)*1024)) > v_max[kk]) 
								v_max[kk] = *(histVvalues + ll + (tot_frames*1024*kk) + ((nfram-n_first)*1024));
							if(nfram==n_last) // condition to caclulate max of cummulative histogram
							{
								allhist_h = *(histHvalues + ll + 1024*kk + tot_frames*1024*1);
								if ( allhist_h > h_amax[kk]) 
								{h_amax[kk] = *(histHvalues + tot_frames*1024*1 + ll + 1024*kk);}
							
								allhist_v = *(histVvalues + ll + 1024*kk + tot_frames*1024*1);
								if (allhist_v > v_amax[kk]) 
								{v_amax[kk] = *(histVvalues + tot_frames*1024*1 + ll + 1024*kk);}
							}
						} // end of loop to find current max
					}
					for(jj=0; jj<sizy; jj++)			// code to find maximum value in 2D histogram array
					for(ii=0; ii<sizx; ii++)
					{
						if( hist_type == 0 && ii<256 && jj <256) // histograms have 256*256 dimensions
								{
									if(*(twoDhist + ii + jj*256 + ((tot_frames*256*256*kk) + ((nfram-n_first)*256*256))) > two_max)	
										two_max = *(twoDhist + ii + jj*256 + ((tot_frames*256*256*kk) + ((nfram-n_first)*256*256)));
								}
						if( hist_type == 1 && ii<256 && jj <256 )
								{
									if(*(twoDhires + ii + 384 + ((jj+384)*sizx) + ((tot_frames*1024*1024*kk) + ((nfram-n_first)*1024*1024))) > two_max)	
										two_max = *(twoDhires + ii + 384 + ((jj+384)*sizx) + ((tot_frames*1024*1024*kk) + ((nfram-n_first)*1024*1024)));
								}
					
						if (nfram == n_last && hist_type == 0 && ii<256 && jj <256 ) // new AM MAY2009
						{
								if(*(twoDhist + ii + jj*256 + tot_frames*256*256*1 + 
									256*256*kk)> two_amax) // the 1 means this is looking at the second combo type	
								two_amax = *(twoDhist + ii + jj*256 + tot_frames*256*256*1 + kk*256*256);
						}

						if (nfram == n_last && hist_type == 1 && ii<256 && jj <256) // new AM MAY2009
						{
								if(*(twoDhires + ii + 384 + (jj+384)*sizx + tot_frames*1024*1024*1 + 
									1024*1024*kk)> two_amax)	
								two_amax = *(twoDhires + ii + 384 + (jj+384)*sizx + tot_frames*1024*1024*1 + kk*1024*1024);
						}
					}

					for(jj=0; jj<sizy; jj++)
					{
						for(ii=0; ii<sizx; ii++) // <AY 2009. i have reversed the ii and jj indeces to make histogram diplay
						{
							if(hist_type == 0 && ii<256 && jj <256)
								two_val = *(twoDhist + jj + ii*256 + (tot_frames*256*256*kk) + ((nfram-n_first)*256*256));
							if(hist_type == 1 && ii<256 && jj <256)
								two_val = *(twoDhires + jj + 384 + ((ii+384)*sizx) + (tot_frames*1024*1024*kk) + ((nfram-n_first)*1024*1024));
							
							if( two_val == 0 && ii<256 && jj <256)	
								*(dispTwohist + ii + jj*sizx + (kk*num_pixels)) = 765;
							else 
							{
								// ANDY, you have a log compression in this histogram????-> AMAPR2009???
								//two_val = 200-((int)(200*log((double)(two_val)))/log((double)(two_max)));
								//two_val = 1; // testing may09
								// i am trying a non log version!! AM MAy2009
								
								// below everything is linear, not logarithmic compression though may do this again if needed
								two_val = 765-(0.5+0.5*(int)(765*(double)(two_val))/(double)(two_max));
								*(dispTwohist + ii + jj*sizx + (kk*num_pixels)) = two_val;
							}
							if( hist_type == 2)	// code for loading amplitude histogram for display
							{
								if( amp_max[kk] == 0 )	curr_val = (int)(sizx * (*(histAmp + ii + (tot_frames*256*kk) + ((nfram-n_first)*256))));
								else					curr_val = (int)(sizx * (*(histAmp + ii + (tot_frames*256*kk) + ((nfram-n_first)*256)))/amp_max[kk]);
								if( curr_val >= (sizx - jj) )		*(dispHist + (kk*num_pixels) + jj*sizx + ii) = 127;
								else								*(dispHist + (kk*num_pixels) + jj*sizx + ii) = 765;
							}
							if( hist_type == 2) // code for loading direction histogram for display
							{
								if( dir_max[kk] == 0 )	curr_val = (int)(sizx * (*(histDir + ii + (tot_frames*256*kk) + ((nfram-n_first)*256))));
								else					curr_val = (int)(sizx * (*(histDir + ii + (tot_frames*256*kk) + ((nfram-n_first)*256)))/dir_max[kk]);
								if( curr_val >= (sizx - jj) )		*(dispTwohist + (kk*num_pixels) + jj*sizx + ii) = 127;
								else								*(dispTwohist + (kk*num_pixels) + jj*sizx + ii) = 765;
							}
							if( hist_type == 1 || hist_type == 0 )
							{
								if(jj< 128) 
								{
									if( hist_type == 1 && h_max[kk] !=0)
										curr_val = (int)(127 * (*(histHvalues + ii + 384 + (tot_frames*1024*kk) + ((nfram-n_first)*1024)))/h_max[kk]);
									if( hist_type == 0)
									{
										if(h_max[kk] == 0)		curr_val = (int)(127 * (*(smallHhist + ii + (tot_frames*256*kk) + ((nfram-n_first)*256))));
										else					curr_val = (int)(127 * (*(smallHhist + ii + (tot_frames*256*kk) + ((nfram-n_first)*256)))/h_max[kk]);
									}
									if(curr_val >= ((sizx/2) - jj))
									{	
										*(dispHist + (kk*num_pixels) + jj*sizx + ii) = 127;
									}
									else
										{*(dispHist + (kk*num_pixels) + jj*sizx + ii) = 765;}
								}
								if(jj>127)
								{
									if( hist_type == 1 && v_max[kk] !=0)
										curr_val = (int)(127 * (*(histVvalues + ii + 384 + (tot_frames*1024*kk) + ((nfram-n_first)*1024)))/v_max[kk]);
									if( hist_type == 0)
										if(v_max[kk] == 0)		curr_val = (int)(127 * (*(smallVhist + ii + (tot_frames*256*kk) + ((nfram-n_first)*256))));
										else					curr_val = (int)(127 * (*(smallVhist + ii + (tot_frames*256*kk) + ((nfram-n_first)*256)))/v_max[kk]);
									if(curr_val >= (sizx - jj))
									{
										*(dispHist + (kk*num_pixels) + jj*sizx + ii) = 127;
									}
									else
										{*(dispHist + (kk*num_pixels) + jj*sizx + ii) = 765;}
								}
								if((jj<129 && jj>126) || (ii<129 && ii>126)
									|| ii<3 || ii>253 || jj<3 || jj>253) // create borders!
								{
									*(dispHist + (kk*num_pixels) + jj*sizx + ii) = 0;
									//below commented out because we want to see the axes of the 2d plots
									// i.e. the histogram responses at the axes... AM MAY2009
									*(dispTwohist + (kk*num_pixels) + jj*sizx + ii) = 127;
								}
							

								//######## 03DEC08################## further updated June 2013
								if (nfram==n_last) // when you get to the last frame AM03Dec08... again APR09
								{
	                                //------------- AM MAY2009_> changing order of ii and jj to correct histogram (?)
									if( hist_type == 0 && ii<256 && jj<256) two_val = *(twoDhist + jj + ii*256 + 
										tot_frames*256*256*1 + 256*256*kk);
									if( hist_type == 1 && ii<256 && jj<256) two_val = *(twoDhires + jj + 384 + ((ii+384)*sizx) 
										+ (tot_frames*1024*1024*1 + 256*256*kk)); // not yet working for high res
									if( two_val == 0 )	
										*(dispTwohist + ii + jj*sizx + 1*num_pixels + kk*num_pixels) = 765;
									else 
									{ // last frame, show histograms
										//two_val = 765-((int)(765*log((double)(two_val)))/log((double)(two_amax)));
										// i am trying a non log version!! AM MAy2009
										//two_val = 765-((int)(765*(double)(two_val))/(double)(two_max));
										// also trying log verson with all non zeros darker!
										two_val = 765-(0.5+0.5*(int)(765*(double)(two_val))/(double)(two_amax));;
										*(dispTwohist + ii + jj*sizx + 1*num_pixels + kk*num_pixels) = two_val;
									}
									
									//------------END OF CODE AM MAY09
									if(jj<127)
									{
										if( hist_type == 0 && h_amax[kk]!=0)
											curr_val = (int)(127 * (*(smallHhist + ii + tot_frames*256*1 + kk*256))/h_amax[kk]);
										if( hist_type == 1 && h_amax[kk]!=0)
											curr_val = (int)(127 * (*(histHvalues + ii + 384 + tot_frames*1024*1 + kk*1024))/h_amax[kk]);
										if(curr_val >= ((sizx/2) - jj))
										{
											*(dispHist + 1*num_pixels + kk*num_pixels + jj*sizx + ii) = 127;
										}
										else
											{*(dispHist + 1*num_pixels + (kk*num_pixels) + jj*sizx + ii) = 765;}
									}
									if(jj>128)
									{
										if( hist_type == 0 && v_amax[kk]!=0)	// for full histogram 
											curr_val = (int)(127 * (*(smallVhist + ii + tot_frames*256*1 + kk*256))/v_amax[kk]);
										if( hist_type == 1 && v_amax[kk]!=0) // for hi-res histogram
											curr_val = (int)(127 * (*(histVvalues + ii + 384 + tot_frames*1024*1 + kk*1024))/v_amax[kk]);
										if(curr_val >= (sizx - jj))
										{
											*(dispHist + 1*num_pixels + kk*num_pixels + jj*sizx + ii) = 127;
										}
										else
											{*(dispHist + 1*num_pixels + kk*num_pixels + jj*sizx + ii) = 765;}
									}
									if((jj<129 && jj>126) || (ii<129 && ii>126)
										|| ii<3 || ii>253 || jj<3 || jj>253)
									{
										*(dispHist + 1*num_pixels + kk*num_pixels + jj*sizx + ii) = 0;
										*(dispTwohist + ii + jj*sizx + 1*num_pixels + kk*num_pixels) = 127;	
									}	
								} //end of last frame condition..... AM03Dec08 
							}
						}		// end of loop through columns
					}		// end of loop through rows
				}		// end of if statement when histogram flag is set
			}		// end of loop through channels
			
			//RedrawWindow();
			sprintf( out_str, "resp/%s.chah&chav frame %d: %dx%d min|max %d|%d&%d|%d", 
						in_name_str, nfram, sizx,sizy, hmin/4, hmax/4, vmin/4, vmax/4 );
			ClientDC.TextOut(550, 45, out_str);

			for(kk = 0; kk < 1/*2*/; kk++) // maybe this too should be 1 if only one chan used
			{
				for(ll=0; ll<num_pixels; ll++)
				{
					*(disp_pH + ll) = *(pHvalues + ll + (kk*num_pixels));
					*(disp_pV + ll) = *(pVvalues + ll + (kk*num_pixels));
				}
				if( disp_hist == 1)
				{
					for(ll=0; ll<num_pixels; ll++)	
					{
						*(currHist + ll) = *(dispHist + ll + (kk*num_pixels));
						*(currTwohist + ll) = *(dispTwohist + ll + (kk*num_pixels));
					}
					nImagePointProcess.Dump8bitData( currHist,sizx, sizy );
					pImagePointProcess.Dump8bitData( currTwohist,sizx, sizy );
				}

				mImagePointProcess.Dump24bitData( disp_pH, disp_pV, sizx, sizy );
				if( thresh != 255.0 )
					mImagePointProcess.Dump24bitDataNL( thresh, disp_pH, disp_pV, sizx, sizy );


				n_pImageObject->Draw( &ClientDC, 2+kk*(sizx+2), 152);
				
				if(disp_hist == 1)
				{
					m_pImageObject->Draw( &ClientDC, 2+kk*(sizx+2), 410);
					l_pImageObject->Draw( &ClientDC, 2+kk*(sizx+2), 670);
				}
				if(kk==0)
					{
						sprintf( out_str, "resp/%s%d.hor&ver: %dx%d ", in_name_str, nfram, sizx, sizy);
						ClientDC.TextOut(5, 118, out_str);
					}			
				sprintf( out_str, "Combination no: %d", kk+1);
				ClientDC.TextOut(2+kk*(sizx+2), 134, out_str);	

				if(disp_hist == 1 && hist_type == 0)	// display H and V labels on histograms and max values
				{
					sprintf( out_str, "H: %d", h_max[kk] );
					ClientDC.TextOut(10+kk*(sizx+2), 420, out_str);
					sprintf( out_str, "V: %d", v_max[kk] );
					ClientDC.TextOut(10+kk*(sizx+2), 636, out_str);
				}
				
				if (nfram==n_last && disp_hist == 1 && hist_type == 0) // do only when on the last fram
				{
						if (kk==0) Sleep (1500); // delay before showing cummulative histogram
						
						for(ll=0; ll<num_pixels; ll++) // populate pointer with cummulative histogram	
						{
						*(currHist + ll) = *(dispHist + 1*num_pixels + ll + (kk*num_pixels));
						*(currTwohist + ll) = *(dispTwohist + 1*num_pixels + ll + (kk*num_pixels));}
						
						nImagePointProcess.Dump8bitData( currHist, sizx, sizy );
						pImagePointProcess.Dump8bitData( currTwohist, sizx, sizy );
											
						if(disp_hist == 1)
						{
							m_pImageObject->Draw( &ClientDC, 2+kk*(sizx+2), 410);
							l_pImageObject->Draw( &ClientDC, 2+kk*(sizx+2), 670);
						}
						
						// label the cummulative histograms...
						sprintf( out_str, "H - all: %d", h_amax[kk] );
						ClientDC.TextOut(10+kk*(sizx+2), 420, out_str);
						sprintf( out_str, "V - all: %d", v_amax[kk] );
						ClientDC.TextOut(10+kk*(sizx+2), 636, out_str);
						}

			}  // end loop through channels for display
		}	// end loop through image sequence
	
	
	fclose(fi1);	fclose(fi2);	fclose(fi3);	fclose(fi4);
	fclose(fiv1);	fclose(fiv2);	fclose(fiv3);	fclose(fiv4);
	fclose(lores);	fclose(hires);
	delete pHvalues;	pHvalues = 0;
	delete pVvalues;	pVvalues = 0;
	delete dispHist;	dispHist = 0;
	delete disp_pH;		disp_pH = 0;
	delete disp_pV;		disp_pV = 0;
	EndWaitCursor();	


}


void CMy2DMD_DISPLAYDlg::OnAverresp() 
{
	// TODO: Add your control notification handler code here
	// load sequences of ascii response data files, convert into average input image map, and display


	int dumi, nfram, kk, i, j;
	int num_pixels = 0;
	char out_str[80];
	char in_filnam[100];                

	int hmax = 0;
	int hmin = 1023;
	double * pDHvalues;

    int vmax = 0;
	int vmin = 1023;
	double * pDVvalues;

	CClientDC ClientDC( this );
	BeginWaitCursor();
	RedrawWindow();			// erase previous image and messages

	
		
		for(kk=0; kk<4; kk++)
		{
			sprintf( in_filnam, "resp/%s_%d.chah", in_name_str, kk );	// generate input filename 
			dumi = DoCheckFile(in_filnam);	// determine first image size
			if (dumi==0)					// and exit if input file not opened 
				{ AfxMessageBox( "cannot load first ascii file (name?)!" ); return; } // and exit if not
		}
		
	num_pixels = sizx*sizy;			// calculate number of pixels
	//seq_pixels = num_pixels * (n_last - n_first + 1);

	// prepare data array for horizontal response values 
	pHvalues = new unsigned short int [num_pixels*4];	// allocate space for image on heap
	if ( pHvalues==NULL )							// warning if array doesn't get requested heap space
		{ AfxMessageBox( "Could not create H heap space !" ); return; }
	pDHvalues = new double [num_pixels*4];
	if ( pDHvalues==NULL )							// warning if array doesn't get requested heap space
		{ AfxMessageBox( "Could not create H heap space !" ); return; }

	for ( j = 0; j<sizy; j++)	for( i = 0; i<sizx; i++)
		*(pDHvalues + j*sizx + i) = 0.0;

	pVvalues = new unsigned short int [num_pixels*4];	// allocate space for image on heap
	if ( pVvalues==NULL )							// warning if array doesn't get requested heap space
		{ AfxMessageBox( "Could not create V heap space !" ); return; }
	pDVvalues = new double [num_pixels*4];
	if ( pDVvalues==NULL )							// warning if array doesn't get requested heap space
		{ AfxMessageBox( "Could not create V heap space !" ); return; }

	for ( j = 0; j<sizy; j++)	for( i = 0; i<sizx; i++)
		*(pDVvalues + j*sizx + i) = 0.0;


	n_pImageObject = new CImageObject( "col_24bit.bmp" );
	dumi = n_pImageObject->Stretch( sizx, sizy );
	if ( (n_pImageObject==NULL) || (dumi == 0) )
		{ AfxMessageBox( "Could not create image !" ); return; }

	CImagePointProcesses mImagePointProcess( n_pImageObject );

	RedrawWindow();

		for(nfram = n_first; nfram <= n_last; nfram++){
			for(kk = 0; kk < 4; kk++){
				sprintf( in_filnam, "resp/%s_%d.chah", in_name_str, kk );	// generate input filename 
				dumi = DoLoadFile(nfram, kk, 0, in_filnam, &hmin, &hmax);
				if (dumi==0)					// and exit if hor response file not opened
					{ AfxMessageBox( "cannot load current ascii file (name?)!" ); return; } 
			
				sprintf( in_filnam, "resp/%s_%d.chav", in_name_str, kk );	// generate input filename 
				dumi = DoLoadFile(nfram, kk, 1, in_filnam, &vmin, &vmax);
				if (dumi==0)					// and exit if hor response file not opened
					{ AfxMessageBox( "cannot load current ascii file (name?)!" ); return; } 
			}
			mImagePointProcess.Dump24bitData( pHvalues, pVvalues, sizx, sizy );
			if( thresh != 255.0 )
				mImagePointProcess.Dump24bitDataNL( thresh, pHvalues, pVvalues, sizx, sizy );
			RedrawWindow();
			sprintf( out_str, "resp/%s.chah&chav frame %d: %dx%d min|max %d|%d&%d|%d", 
						in_name_str, nfram, sizx,sizy, hmin/4, hmax/4, vmin/4, vmax/4 );
			ClientDC.TextOut(550, 45, out_str);

			for(kk = 0; kk < 4; kk++){
				n_pImageObject->Draw( &ClientDC, 2+kk*(sizx+2), 152);
				if(kk==0)
					{
						sprintf( out_str, "resp/%s%d.hor&ver: %dx%d ", in_name_str, nfram, sizx, sizy);
						ClientDC.TextOut(5, 118, out_str);
					}			
				sprintf( out_str, "Chan no: %d", kk+1);
				ClientDC.TextOut(2+kk*(sizx+2), 134, out_str);			

			}

			for ( j = 0; j<sizy; j++ )	for( i = 0; i<sizx; i++ )
				*(pDHvalues + j*sizx + i) += (double)*(pHvalues + j*sizx + i);

			for ( j = 0; j<sizy; j++ )	for( i = 0; i<sizx; i++ )
				*(pDVvalues + j*sizx + i) += (double)*(pVvalues + j*sizx + i);

		}

double numfram = (double)(n_last - n_first + 1);

for ( j = 0; j<sizy; j++ )	for( i = 0; i<sizx; i++ )
	*(pHvalues + j*sizx + i) = (int)(*(pDHvalues + j*sizx + i)/numfram);
for ( j = 0; j<sizy; j++ )	for( i = 0; i<sizx; i++ )
	*(pVvalues + j*sizx + i) = (int)(*(pDVvalues + j*sizx + i)/numfram);

mImagePointProcess.Dump24bitData( pHvalues, pVvalues, sizx, sizy );
			if( thresh != 255.0 )
				mImagePointProcess.Dump24bitDataNL( thresh, pHvalues, pVvalues, sizx, sizy );
			sprintf( out_str, "Average response: %dx%d min|max %d|%d&%d|%d", 
						sizx,sizy, hmin/4, hmax/4, vmin/4, vmax/4 );
			ClientDC.TextOut(550, 45, out_str);

			for(kk = 0; kk < 4; kk++){
				n_pImageObject->Draw( &ClientDC, 2+kk*(sizx+2), 152);
				if(kk==0)
					{
						sprintf( out_str, "resp/%s%d.hor&ver: %dx%d ", in_name_str, nfram, sizx, sizy);
						ClientDC.TextOut(5, 118, out_str);
					}			
				sprintf( out_str, "Chan no: %d", kk+1);
				ClientDC.TextOut(2+kk*(sizx+2), 134, out_str);			

			}

	fclose(fi1);	fclose(fi2);	fclose(fi3);	fclose(fi4);
	fclose(fiv1);	fclose(fiv2);	fclose(fiv3);	fclose(fiv4);
	delete pHvalues;	pHvalues = 0;
	delete pVvalues;	pVvalues = 0;
	EndWaitCursor();	

}


int ImageRegion(int curr_chan, int horpos, int verpos)	
{
	// find region of current pixel in image array 
	int delt_pos1, delt_pos2; // define delta position vatiables
	int region = 0;
	
	if (curr_chan<4) // conditions for channel checking, AM MAY11
	{
	delt_pos1 = (int)(-delphi[curr_chan]/2.0);					// relative left/upper input position
	delt_pos2 = delt_pos1 + ((int)(delphi[curr_chan]));		// relative right/lower input position
	}
	
	if (curr_chan>3) // conditions for channel checking, AM MAY11
	{
	delt_pos1 = (int)(-delphi[3]/2.0);					// relative left/upper input position
	delt_pos2 = delt_pos1 + ((int)(delphi[3]));		// relative right/lower input position
	}

	// region 1: inside frame determined by half-filter width + half detector sampling
	if ((horpos>WIDF-delt_pos1+1) && (horpos<sizx-WIDF+delt_pos1-2) 
					&& (verpos>WIDF-delt_pos1+1) && (verpos<sizy-WIDF+delt_pos1-2))
		region = 1;		
	// region 2: outside frame determined by half-filter width + half detector sampling
	if ((horpos<WIDF-delt_pos1+1) || (horpos>sizx-WIDF+delt_pos1-2) 
					|| (verpos<WIDF-delt_pos1+1) || (verpos>sizy-WIDF+delt_pos1-2))
		region = 2;	
	
	return (region);
}


