-// 2DMD_GENPATDlg.cpp : implementation file
//

#include "stdafx.h"
#include "2DMD_GENPAT.h"
#include "2DMD_GENPATDlg.h"
#include "ImagePointNewProc.h"
#include <math.h>
#include ".\2dmd_genpatdlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define GGG 765							/* 3*8 bit expanded greylevel range */
#define PIP 3.1415926535				/* PI */
#define DELAY 1						/* delay after image in msec */

// define external string and image variables
char out_name_str[50] = "test_";		// filename : same initialisation as in Dlg 
int  n_images = 15;						// number of images in sequence : same initialisation as in Dlg
int  n_dots = 256;						// number of dots in frame : same initialisation as in Dlg
int  n_dirs = 2;						// number of motion directions : same initialisation as in Dlg
int  dotsiz = 2;						// size of dots for contour stuff	

// define external image parameters and arrays
int sizx = 300;							// horizontal image size (# columns): same initialisation as in Dlg
int sizy = 300;							// vertical image size (# rows) : same initialisation as in Dlg
int	gmax = 0;							// maximum pattern intensity
int	gmin = 0;							// minimum pattern intensity
int contype = 0;						// select type of contour
double disp = 2.0;						// dot displacement : first componen(left of cont)
double disp2 = 2.0;						// dot displacement : for second component(right)
double lifetime = 5.0;					// stimulus dot lifetime (max)
double dir = 0.0;						// motion direction : same initialisation as in Dlg
double dirdiff = 180.0;					// motion direction difference: same initialisation as in Dlg
double pi = 3.14159265;					// pi

//*************** DEFAULT PARAMS FOR WHITE NOISE STUFF********************
double mod_amp = 1.0;
double mod_freq = 3.0;
double carr_freq = 3.0;
int mod_type = 2;


unsigned short int * pGvalues;			// pointer to image greylevels (2D data array)
unsigned short int * pIvalues;			// pointer to image greylevels (2D data array)
unsigned short int * pHvalues;			// pointer to temporary greylevels (2D data array)

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
// CMy2DMD_GENPATDlg dialog

CMy2DMD_GENPATDlg::CMy2DMD_GENPATDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMy2DMD_GENPATDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMy2DMD_GENPATDlg)
	m_in_sizeX = _T("300");
	m_in_sizeY = _T("300");
	m_in_dotsiz = 2;
	m_in_numFrames = _T("15");
	m_in_numDots = 256;
	m_in_contype = _T("0");
	m_in_disp2 = _T("2.0");
	m_in_numDirs = 2;
	m_in_filename = _T("test_");
	m_in_progress = _T("BLANK");
	m_in_disp = _T("2.0");
	m_in_dir = _T("0.0");
	m_in_dirdiff = _T("180.0");
	m_in_m_amp = _T("1.0");	
	m_in_freq_mod = _T("3.0");
	m_in_freq_carr = _T("3.0");
	m_in_modtype = _T("2");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMy2DMD_GENPATDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMy2DMD_GENPATDlg)
	DDX_Text(pDX, IDC_InSizeX, m_in_sizeX);
	DDX_Text(pDX, IDC_dotsiz, m_in_dotsiz);
	DDX_Text(pDX, IDC_InSizeY, m_in_sizeY);
	DDX_Text(pDX, IDC_InNumFrames, m_in_numFrames);
	DDX_Text(pDX, IDC_InFileName, m_in_filename);
	DDX_Text(pDX, IDC_InDispX, m_in_disp);
	DDX_Text(pDX, IDC_disp2, m_in_disp2);
	DDX_Text(pDX, IDC_InDsipY, m_in_dir);
	DDX_Text(pDX, IDC_NumDots, m_in_numDots);
	DDX_Text(pDX, IDC_NumDirs, m_in_numDirs);
	DDX_Text(pDX, IDC_InDirDiff, m_in_dirdiff);
	DDX_Text(pDX, IDC_contype, m_in_contype);
	DDX_Text(pDX, IDC_progress, m_in_progress);
	DDX_Text(pDX, IDC_m_amp, m_in_m_amp);
	DDX_Text(pDX, IDC_freq_mod, m_in_freq_mod);
	DDX_Text(pDX, IDC_freq_carr, m_in_freq_carr);
	DDX_Text(pDX, IDC_modtype, m_in_modtype);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CMy2DMD_GENPATDlg, CDialog)
	//{{AFX_MSG_MAP(CMy2DMD_GENPATDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_GENDOTS, OnGendots)
	ON_BN_CLICKED(IDC_GENDOTS3, OnGenwnoise)
	ON_BN_CLICKED(IDC_GENDOTS4, OnGensegments)
	ON_BN_CLICKED(IDC_GENDOTS6, OnGenpplaids)
	ON_BN_CLICKED(IDC_GENDOTS7, OnGenreplaids)
	ON_EN_CHANGE(IDC_InSizeX, OnEnChangeInsizex)
	ON_EN_CHANGE(IDC_InSizeY, OnEnChangeInsizey)
	ON_EN_CHANGE(IDC_InNumFrames, OnEnChangeInnumframes)
	ON_EN_CHANGE(IDC_InFileName, OnEnChangeInfilename)
	ON_EN_CHANGE(IDC_InDispX, OnEnChangeIndispx)
	ON_EN_CHANGE(IDC_disp2, OnEnChangeIndispx)
	ON_EN_CHANGE(IDC_InDsipY, OnEnChangeIndsipy)
	ON_EN_CHANGE(IDC_NumDots, OnEnChangeNumdots)
	ON_EN_CHANGE(IDC_NumDirs, OnEnChangeNumdirs)
	ON_EN_CHANGE(IDC_InDirDiff, OnEnChangeIndirdiff)
	ON_EN_CHANGE(IDC_contype, OnEnChangeIndirdiff)
	ON_EN_CHANGE(IDC_m_amp,  OnEnChangeIndirdiff)
	ON_EN_CHANGE(IDC_freq_mod, OnEnChangeIndirdiff)
	ON_EN_CHANGE(IDC_freq_carr, OnEnChangeIndirdiff)
	ON_EN_CHANGE(IDC_modtype,  OnEnChangeIndirdiff)
	ON_EN_CHANGE(IDC_dotsiz, OnEnChangeIndirdiff)
	//}}AFX_MSG_MAP
	
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMy2DMD_GENPATDlg message handlers

BOOL CMy2DMD_GENPATDlg::OnInitDialog()
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
	m_pImageObject = NULL;
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CMy2DMD_GENPATDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CMy2DMD_GENPATDlg::OnPaint() 
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
	//below, added by andrew meso feb2009 to seed random numbers with clock
	srand( (unsigned)time( NULL ) );

}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CMy2DMD_GENPATDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

/////////////////////////////////////////////////////
///////////////// the real stuff starts here ////////
/////////////////////////////////////////////////////

void CMy2DMD_GENPATDlg::OnGendots() 
{
	// TODO: Add your control notification handler code here
	// generate sequence of greylevel images sequence and save as stm files
	char out_str[80];
	int outi, loopi, ii, jj, kk, ll;
	int dumi = 0;
	int xpos, ypos;
	double x0[513][4];
	double y0[513][4];
	double xxpos, yypos, xdisp, ydisp, dir_now;
	int num_pixels = 999;
	double dumy = 0.0;
	BOOL success = 0;

	CClientDC ClientDC( this );
	num_pixels = sizx*sizy;			// number of pixels in single image

	//prepare stuff
	gmax = 0;				// reset maximum input intensity
	gmin = 255;				// reset minimum input intensity
	srand( (unsigned)time( NULL ) );				// initialise random number generator
	//dotsiz = 2;
	
	// save in stm files (in stim subdirectory)
	// prepare and open asci data file 
	FILE    *fo;
	char 	out_filnam[100];                
	
	sprintf( out_filnam, "stim\\trans_rdk_%s.char", out_name_str );	// generate output pattern filename 
		if ( (fo=fopen(out_filnam,"wb")) == NULL )		// test whether ascii file can be opened 
				{ AfxMessageBox( "cannot save ascii file (open?)!" ); return; }	// prevent save without open file

	// save header: size, name, scale
	//double	scal = 9999.0/765.0;	// scale factor: greyvalues vary between 0...765 >> convert to 0...10000
	fprintf(fo, "%4d %4d pattern : %s", sizx, sizy, out_filnam );	// save size in ascii file
	
	for (ll=1; ll<=n_dirs; ll++)						// generate ll sets of dots
		for (kk=1; kk<=n_dots; kk++)					// generate kk random dot locations
			{
			dumi = rand();								// generate random integer in the range 0 to RAND_MAX
			dumy = ((double)dumi) / ((double)RAND_MAX);	// convert to random double in the range 0.0 to 1.0
			x0[kk][ll] = dumy*((double)sizx);
			dumi = rand();								// generate random integer in the range 0 to RAND_MAX
			dumy = ((double)dumi) / ((double)RAND_MAX);	// convert to random double in the range 0.0 to 1.0
			y0[kk][ll] = dumy*((double)sizy);
			}

	// load the image data for the full sequence
	for (loopi=1; loopi<=n_images; loopi++)		// run through image sequence loop
		{
		BeginWaitCursor();

		// prepare data array for greylevel values 
		pGvalues = new unsigned short int [num_pixels];	// allocate space for image on heap
		if ( pGvalues==NULL )							// warning if array doesn't get requested heap space
			{ AfxMessageBox( "Could not create image heap space !" ); return; }

		// make white backround, store data in data array for display and storage in ascii file 
		dumy = 1.0;	
		dumi = GreyVal(dumy);				// convert to 0-255 greylevel value
		for (jj=0; jj<sizy; jj++ )			// run through sizy rows
			{
			for (ii=0; ii<sizx; ii++ )		// run through sizx columns
				*(pGvalues + jj*sizx + ii) = 3*dumi;	// set sequence array greylevel value 
			}	/* end run through rows */

		// add black dots, store data in data array for display and storage in ascii file 
		for (ll=1; ll<=n_dirs; ll++)					// generate ll sets of dots
		{
		dir_now = dir + ((double)(ll-1))*dirdiff;
		xdisp = (cos(PIP*dir_now/180.0))*disp;			// horizontal displacement < direction
		ydisp = (sin(PIP*dir_now/180.0))*disp;			// vertical displacement < direction
		dumy = 0.0;	
		dumi = GreyVal(dumy);							// convert to 0-255 greylevel value

		for (kk=1; kk<=n_dots; kk++)					// generate n_dots random dot locations
			{		
			xxpos = x0[kk][ll] + ((double)loopi) * xdisp;	
			xpos = (int)xxpos;		if (xpos>=sizx-dotsiz) xpos-=sizx-2*dotsiz;	if (xpos<dotsiz) xpos+=sizx-2*dotsiz;
			yypos = y0[kk][ll] - ((double)loopi) * ydisp;	
			ypos = (int)yypos;		if (ypos>=sizy-dotsiz) ypos-=sizy-2*dotsiz;	if (ypos<dotsiz) ypos+=sizy-2*dotsiz;

			for (jj=ypos; jj<ypos+dotsiz; jj++ )					// run through dot size vertical
				for (ii=xpos; ii<xpos+dotsiz; ii++ )				// run through dot size horizontal
					*(pGvalues + jj*sizx + ii) = 3*dumi;	// set sequence array greylevel value 
			}	// end of run through kk dots
		}		// end of run through ll sets of dots

		// save image data (pixel greylevel)
		for (jj=0; jj<sizy; jj++)			// run through rows of image
			{
			for (ii=0; ii<sizx; ii++)		// run through columns of row
				{ 
				dumi = *(pGvalues + jj*sizx + ii);	// pick current pixel's grey value
				outi = (int)((double)dumi/3);	// scale to 0...10000
				fprintf(fo, "%1c", outi);			// save into ascii file	
			}						// end of current row 
			//fprintf(fo, "\n");			// new line at end of row
			}							// end of all rows of image
		
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
			m_pImageObject->Draw( &ClientDC, 10, 155 );
			sprintf( out_str, "%s%d.char generated: %dx%d min|max %d|%d", 
						out_name_str, loopi, sizx,sizy, gmin,gmax );
			ClientDC.TextOut(20, 140, out_str);
			}	

		//	save gif output //////////////////////////////////////////////////////////
		if ( m_pImageObject != NULL )
			{
			sprintf( out_filnam, "giff\\rdkdots_%s%d.gif", out_name_str, loopi+0 );	// generate output pattern filename 
			success = m_pImageObject->Save(out_filnam, -1);			// save image as gif
			sprintf( out_str, "saved as: giff/%s%d.gif %d x %d : %d", 
						out_name_str, loopi, sizx, sizy, success );	// generate output string
			ClientDC.TextOut(450, 140, out_str);						// display string
			}
		else
			{ RedrawWindow();			// erase previous image and messages
			  ClientDC.TextOut(550, 140, " an unknown problem has occured...                     "); }

		Sleep(DELAY);
		EndWaitCursor();
		delete pGvalues;		pGvalues = 0;		// free memory and reset pointer
		}	// end running through image sequence loop
		
		fprintf(fo, " comment 1 " );	fprintf(fo, " comment 2 " );
		fprintf(fo, " comment 3 " );	fprintf(fo, " comment 4 " );
		fprintf(fo, " comment 5 " );	// save various comments in ascii file
		fclose(fo);						// close ascii file

}

void CMy2DMD_GENPATDlg::OnGensegments()   //code to generate segmented motion AM Jan08 
{
	// TODO: Add your control notification handler code here
	// generate sequence of greylevel images sequence and save as stm files
	char out_str[80];
	int outi, loopi, ii, jj, kk, ll;
	int dumi = 0;
	int xpos, ypos;
	double xxpos, yypos, dir_now;
	int num_pixels = 999;
	double dumy = 0.0;
	BOOL success = 0;
	int sqvcont = contype;			// CASE OF SINGLE COUTOUR!!!
	
	FILE	*fo;
	char 	out_filnam[100];                

	CClientDC ClientDC( this );
	num_pixels = sizx*sizy;			// number of pixels in single image
	///----------------- NEW CONTOUR DOT CODE NOV08 -------------------------
	// generate random dot starting coordinates, calculate the dots position for all frames
	// and write the dot coordinates into the array.
	int		i, j, l, index;
	int		path_length;
	int		case_no;
	int		dot_coord_array[513][120]; // array for storing independent dot positions 
	int		dot_ages [513]; // 513 - maximum number of dots, 120 ??
	double	strip_width, dir_rad;
	double  maxg=0.0; //...> maximum width is 256 by 256! 
	double	*Gabor; // AM Apr 2011 using a pointer instead of an array for modulator 
	double	phase, sigma, sf;
	int		gx, gy;
	int		vert;
	double xrand,yrand;
	//	sqvcont = 0;    //0 discrete direction diff, no speed diff (square)(easily altered for no dir diff and disc speed diff)
						//1 discrete direction diff, cont speed diff (orig Gabor)
						//2 no direction diff, cont speed diff (just adding values to the Gabor)
						//3 no direction diff, cont speed diff (using abs(Gabor)) - more copmarable to 1
						//4 alternative version of 1, speed truly cont, dir disc
						//5 cont direction diff (defined by Gabor), no speed difference 
						//6 cont direction diff and cont speed diff (both defined by Gabor)
	int		direction_deg[4];
	double	direction_rad[4];
	double  x_direction_step[4];
	double  y_direction_step[4];

	// **********************************************************************************************
	// derive actual stimulus variables
	while (dir> 360) dir -= 360;	// enforce motion directions between 0 and 360 deg
	// convert the primary movement direction from degrees (int) into radians (double)
	dir_rad = ((2.0 * pi) / 360.0) * ((double)dir);

	// **********************************************************************************************
	// global conversion of the set of movement directions (calculated from the primary movement
	// direction) from degrees (int) into radians (double).
	l=(int)(pow((double)(-1),(rand())));
	for ( i=0; i<n_dirs; i++)			// set up th edifferent directions present within motion
	{
	direction_deg[i] = (int)(dir*l + i * dirdiff);
	while (direction_deg[i] >= 360)	direction_deg[i] -= 360;
	} // make sure directions are between 0-360
	
	for ( i=0; i<n_dirs; i++)
	{
	direction_rad[i] = ((2.0*pi)/360.0) * ((double)direction_deg[i]);
	} //change to radians from degrees

	// (x,y) dot displacements per frame for all 2 movement directions from trigonometry.
	for ( i=0; i<n_dirs; i++)
		{
			if (i==0)
			{
			x_direction_step[i] = (disp * (cos(direction_rad[i]))); //hor speed
			y_direction_step[i] = (disp * (sin(direction_rad[i])));// vert speed
			}
			else
			{
			x_direction_step[i] = (disp2 * (cos(direction_rad[i]))); //hor speed
			y_direction_step[i] = (disp2 * (sin(direction_rad[i]))); // vert speed
			}
		}

	// **********************************************************************************************
	// generate random x-y coordinates in first 2 columns of array i.e. the dot starting positions.
	// rand() % 255 because dot is 2x2 in size, i.e. if 256 half the dot would be missing from the
	// grid. But first, seed the timer.
	// and also generate random ages for the dots
	srand( (unsigned)time( NULL ) );	// initialise random number generator

	for (i=0; i<n_dots; ++i) // assign all random dot start positions
	{
		dot_coord_array[i][0] = rand() % ((int)sizx-1);	// x coordinate start
		dot_coord_array[i][1] = rand() % ((int)sizy-1);// y coordinate start
		// output_file2 << " i = " << i << " j = " << j << " : " << dot_coord_array [i] [j] << "\n";
	}

	// assign initial random ages to dots
	for (i=0; i<n_dots; ++i)
		{
		dot_ages[i] = rand() % (int)(lifetime); // give each dot its own life cycle
		if (lifetime>n_images) dot_ages[i]=1;
		}

	// **********************************************************************************************
	// generate x-y coordinates for consecutive frames in pairs of columns of array 
	// case 'case_no': dots travelling in 'case_no' different directions 
	// separated by 'dot_direction_interval'degrees

	phase = 0;		// starting phase of gabor
	sigma = 30;		// size in pixel of gabor width!!!
	sf=1;			// spatial frequency of envelope
	//strip_width=256/2;
	int jitter=rand() % 10;
	jitter=jitter-5;

	double mid_point = sizx*0.5; // AM Nov08
	vert=1; //1 vertical, 0 horizontal ---> gabor function properties...

	Gabor = new double[sizx*sizy]; //Initialising the gabor pointer  
	for (i=0;i<(sizx*sizy);i++) *(Gabor + i) = 0.0; // set initial values to zero
	
	if (vert==1) //make gabor function AM Nov08 (Vertical modulation)
	{
	for (i=0; i<sizx; ++i)
		{
		for (j=0; j<sizy; ++j)
			{
			index = i + j*sizx; // turn the 2D array position into a pointer index
			dumy = sin((i+(phase/2.0)*sizx)*2*pi*sf/sizx)*(1/(2*pi*pow((double)(sigma),2)))
				*exp(-(pow((double)(i-sizx/2),2)+pow((double)(j-sizy/2),2))/(2.0*pow(sigma,2)));
			*(Gabor + index) = dumy;
			if (dumy>maxg) maxg = dumy;
			}
		}
	}
	else		// horizontal Gabor AM Feb09
	{
	for (i=0; i<sizx; ++i)
		{
		for (j=0; j<sizy; ++j)
			{
			index = i + j*sizx; // turn the 2D array position into a pointer index
			dumy =sin((i+(phase/2.0)*sizx)*2*pi*sf/sizx)*(1/(2*pi*pow((double)(sigma),2)))
				*exp(-(pow((double)(i-sizx/2),2)+pow((double)(j-sizy)/2,2))/(2.0*pow((double)(sigma),2)));
			*(Gabor + index) = dumy;
			if (dumy>maxg) maxg = dumy;
			}
		}
	}


	for (i=0; i<n_dots; ++i)	// not sure...
		{xrand=(rand() % 10000)/10000.0 * x_direction_step[1];
		yrand= (rand() % 10000)/10000.0 * y_direction_step[1];
		l = 1;// + i % case_no;		// alternate between 0 and case_no-1
									// to determine direction between #1 and #case_no
		for (j=1; j<n_images; ++j)		// run through all frames for dot i
			{
			gx=dot_coord_array[i][2*j-2]; //hor position (previous)
			gy=dot_coord_array[i][2*j-1]; // vert position (previous)
			index = (gx+gy*sizx); // the index position of the 2D array frame
			dumy = *(Gabor + index); // calculate the weighting of the current gabor function
			//dot_coord_array[i][2*j]= dot_coord_array[i][2*j-2] + (int)(floor( (x_direction_step[1]) + 0.5));
			
			// IN CODE BELOW, dumy is the Gabor weighting....
			switch (sqvcont) // Code for alternative types of contour types
			{
			case 0: // single solid contour through the centre
				if (dot_coord_array[i][2*j-2]<mid_point) // check previous pos of dot 
				{	dot_coord_array[i][2*j] = dot_coord_array[i][2*j-2]+(int)(floor( (x_direction_step[0]) + 0.5));
				dot_coord_array[i][2*j+1] = dot_coord_array[i][2*j-1] + (int)(floor( (y_direction_step[0]) + 0.5));}
				else  
				{
					dot_coord_array[i][2*j] = dot_coord_array[i][2*j-2]+(int)(floor( (x_direction_step[1]) + 0.5));
					dot_coord_array[i][2*j+1] = dot_coord_array[i][2*j-1]+(int)(floor( (y_direction_step[1]) + 0.5));// - min_speed;
				}

				if  (fmod((double)dot_ages[i],lifetime)==0) // relocate if dot is dead
				{
				dot_coord_array[i][2*j]=rand() % (sizx-1);
				dot_coord_array[i][2*j+1]=rand() % (sizy-1);
				}   // end of lifetime if
				break;
			case 1:  // sigle low freq gabor...
				dot_coord_array[i][2*j]   = floor(dot_coord_array[i][2*j-2]+x_direction_step[0]*(dumy/maxg) + 0.5);
				dot_coord_array[i][2*j+1] = floor(dot_coord_array[i][2*j-1]+y_direction_step[0]*(dumy/maxg)+0.5);
				if (floor(disp*(dumy/maxg)+0.5)==0)
				{
				dot_coord_array[i][2*j]   = floor(dot_coord_array[i][2*j-2]+(xrand-x_direction_step[1]/2) + 0.5);
				dot_coord_array[i][2*j+1] = floor(dot_coord_array[i][2*j-1]+(yrand-y_direction_step[1]/2)+0.5);
				}
				if  (fmod((double)dot_ages[i],lifetime)==0) 
				{
				dot_coord_array[i][2*j]=rand() % (sizx-1);
				dot_coord_array[i][2*j+1]=rand() % (sizy-1);
				xrand=rand() * x_direction_step[1];
				yrand=rand() * y_direction_step[1];
				}   // end of lifetime if
				break;
			case 2:
				dot_coord_array[i][2*j] = floor(dot_coord_array[i][2*j-2]+((x_direction_step[1]-x_direction_step[2])*(dumy/maxg+1)/2+x_direction_step[2])+0.5);
				dot_coord_array[i][2*j+1] = floor(dot_coord_array[i][2*j-1]+((y_direction_step[1]-y_direction_step[2])*(dumy/maxg+1)/2+y_direction_step[2])+0.5);
				if ((fmod((double)dot_ages[i], lifetime)==0) ||  (floor(disp*(dumy/maxg)+0.5)==0))
				{
				dot_coord_array[i][2*j]=rand() % (sizx-1);
				dot_coord_array[i][2*j+1]=rand() % (sizy-1);
				}   // end of lifetime if
				break;
			case 3:
				dot_coord_array[i][2*j+1] = dot_coord_array[i][2*j-1]+floor((y_direction_step[1]-y_direction_step[2])*fabs((float) dumy/maxg)+y_direction_step[2]+0.5);
				if ( (fmod((double)dot_ages[i],lifetime)==0) || (floor(y_direction_step[1]*(dumy/maxg)+0.5)==0) )
				{
				dot_coord_array[i][2*j]=rand() % (sizx-1);
				dot_coord_array[i][2*j+1]=rand() % (sizy-1);
				}   // end of lifetime if
				break;
	
			case 5:
				dot_coord_array[i][2*j]   = floor(dot_coord_array[i][2*j-2] + (disp*cos( (direction_rad[1]-direction_rad[2]) * (dumy/maxg+1)/2 + direction_rad[2] ) + 0.5  ) );//
				dot_coord_array[i][2*j+1] = floor(dot_coord_array[i][2*j-1]+ disp*sin((direction_rad[1]-direction_rad[2])*(dumy/maxg+1)/2+direction_rad[2])+0.5 );
				if ( (fmod((double)dot_ages[i],lifetime)==0) || (floor(disp*(dumy/maxg)+0.5)==0) )
				{
				dot_coord_array[i][2*j]=rand() % (sizx-1);
				dot_coord_array[i][2*j+1]=rand() % (sizy-1);
				}   // end of lifetime if
				break;
			case 6:
				dot_coord_array[i][2*j]   = floor( dot_coord_array[i][2*j-2] + ( ( (dumy/maxg+1)/2*(disp-disp2)+disp2 ) * cos( (direction_rad[1]-direction_rad[2]) * (dumy/maxg+1)/2 + direction_rad[2] ) + 0.5  ) );//
				dot_coord_array[i][2*j+1] = floor(dot_coord_array[i][2*j-1]+ ((dumy/maxg+1)/2*(disp-disp2)+disp2) * sin((direction_rad[1]-direction_rad[2])*(dumy/maxg+1)/2+direction_rad[2])+0.5 );
				if ( (fmod((double)dot_ages[i],lifetime)==0) || (floor(disp*(dumy/maxg)+0.5)==0) )
				{
				dot_coord_array[i][2*j]=rand() % (sizx-1);
				dot_coord_array[i][2*j+1]=rand() % (sizy-1);
				}   // end of lifetime if
				break;

			} 

			dot_ages[i]=dot_ages[i]+1;
			//wrap around 256x256 grid
			while (dot_coord_array [i][2*j] <= -1)// Left.
				dot_coord_array [i][2*j] = dot_coord_array [i][2*j] + (sizx-1);
			while (dot_coord_array [i] [2*j] >= (sizx-1)) // Right.
				dot_coord_array [i] [2*j] = dot_coord_array [i][2*j] - (sizx-1);
			// Finishing with the y-direction.
			while (dot_coord_array [i][2*j+1] <= -1)	// Top.
				dot_coord_array [i][2*j+1] = dot_coord_array [i][2*j+1] + (sizx-1);
			while (dot_coord_array [i] [2*j+1] >= (sizx-1)) // Bottom.	
				dot_coord_array [i][2*j+1] = dot_coord_array [i][2*j+1] - (sizx-1);
				//
			}	// end run through frames
		}		// end run through dots

			
	sprintf( out_filnam, "stim\\rdkseg_%s.char", out_name_str );	// generate output pattern filename 
			if ( (fo=fopen(out_filnam,"wb")) == NULL )		// test whether ascii file can be opened 
					{ AfxMessageBox( "cannot save ascii file (open?)!" ); return; }	// prevent save without open file

		// save header: size, name, scale
		//double	scal = 9999.0/765.0;	// scale factor: greyvalues vary between 0...765 >> convert to 0...10000
		fprintf(fo, "%4d %4d pattern : %s", sizx, sizy, out_filnam );	// save size in ascii file
		//fprintf(fo, "%12.6f      scale factor\n", 10000.0 );				// save scale factor in ascii file
			
	//LOOP Below to start running through images frame by frame, to draw them!!!
	for (loopi=0; loopi<n_images; loopi++)		// run through image sequence loop
		{
		BeginWaitCursor();

		// prepare data array for greylevel values 
		pGvalues = new unsigned short int [num_pixels];	// allocate space for image on heap
		if ( pGvalues==NULL )							// warning if array doesn't get requested heap space
			{ AfxMessageBox( "Could not create image heap space !" ); return; }

		// make white backround, store data in data array for display and storage in ascii file 
		dumy = 1.0;	//for grey backgroud, make this 0.5
		dumi = GreyVal(dumy);		// convert to 0-255 greylevel value
		for (jj=0; jj<sizy; jj++ )	// run through sizy rows - set to white (background)
			{
			for (ii=0; ii<sizx; ii++ )		// run through sizx columns
				*(pGvalues + jj*sizx + ii) = 3*dumi;	// set sequence array greylevel value 
			}	/* end run through rows */

		dumy = 0.0;	
		dumi = GreyVal(dumy);	// convert to 0-255 greylevel value - set to black
		

		for (kk=0; kk<n_dots; kk++)	// generate n_dots random dot locations
			{		
			xxpos = dot_coord_array[kk][2*loopi]; //get x coordinate
			xpos = (int)xxpos;	if(xpos>=sizx-dotsiz) xpos -= sizx-2*dotsiz;	
								if (xpos<dotsiz) xpos += sizx-2*dotsiz;
			yypos = dot_coord_array[kk][2*loopi-1];	// get y coordinate
			ypos = (int)yypos;	if (ypos>=sizy-dotsiz) ypos-=sizy-2*dotsiz;	
								if (ypos<dotsiz) ypos+=sizy-2*dotsiz;

			for (jj=ypos; jj<ypos+dotsiz; jj++ )	// run through dot size vertical
			for (ii=xpos; ii<xpos+dotsiz; ii++ )	// run through dot size horizontal
				{
				if (ii>-1 && ii<sizx && jj>-1 && jj<sizx)
				*(pGvalues + jj*sizx + ii) = 3*dumi;	// set sequence array greylevel value 
				}
			}	// end of run through kk dots
		
		// save in stm files (in stim subdirectory)
		// prepare and open asci data file 
		// save image data (pixel greylevel)
		
		for (jj=0; jj<sizy; jj++)			// run through rows of image
			{
			for (ii=0; ii<sizx; ii++)		// run through columns of row
				{ 
				dumi = *(pGvalues + jj*sizx + ii);	// pick current pixel's grey value
				outi = (int)((double)dumi / 3.0);	// scale to 0...10000
				fprintf(fo, "%1c", outi);			// save into ascii file
				}						// end of current row 
			}							// end of all rows of image

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
			RedrawWindow();	// erase previous image and messages -> contour stim frame
			m_pImageObject->Draw( &ClientDC, 10, 155 );
			sprintf( out_str, "%s.char frame %d generated: %dx%d min|max %d|%d", 
						out_name_str, loopi+1, sizx,sizy, gmin,gmax );
			ClientDC.TextOut(10, 140, out_str);
			}	

		//	save gif output //////////////////////////////////////////////////////////
		if ( m_pImageObject != NULL )
			{
			sprintf( out_filnam, "giff\\rdkseg_%s%d.gif", out_name_str, loopi+1 );	// generate output pattern filename 
			success = m_pImageObject->Save(out_filnam, -1);			// save image as gif
			sprintf( out_str, "saved as: giff/%s%d.gif %d x %d : %d", 
						out_name_str, loopi+1, sizx, sizy, success );	// generate output string
			ClientDC.TextOut(350, 140, out_str);						// display string
			}
		else
			{ RedrawWindow();			// erase previous image and messages
			  ClientDC.TextOut(350, 140, " an unknown problem has occured...                     "); }

		Sleep(DELAY);
		EndWaitCursor();
		delete pGvalues;		pGvalues = 0;		// free memory and reset pointer
		}	// end running through image sequence loop
		fprintf(fo, " comment 1 " );	fprintf(fo, " comment 2 " );
		fprintf(fo, " comment 3 " );		fprintf(fo, " comment 4 " );
		fprintf(fo, " comment 5 " );	// save various comments in ascii file
		fclose(fo);						// close ascii file
}

void CMy2DMD_GENPATDlg::OnGenwnoise()   //code to generate segmented motion AM Jan08 
{
	// TODO: Add your control notification handler code here
	// generate sequence of greylevel images sequence and save as stm files
	char out_str[80];
	int outi, loopi, ii, jj, kk, ll;
	int dumi = 0;
	int xpos, ypos;
	double xxpos, yypos, xdisp, ydisp, dir_now;
	int num_pixels = 999;
	double dumy = 0.0;
	BOOL success = 0;
	double *base_wnoise;		//pointer to store white noise stationary image
	double *noise_filt;			//pointer to store 2D DoG filter 
	double *mod_funct;			// pointer containing the nodulating function M1 & M2
	FILE	*fo;
	char 	out_filnam[100];                

	CClientDC ClientDC( this );
	num_pixels = sizx*sizy;			// number of pixels in single image
	///----------------- NEW CONTOUR DOT CODE NOV08 -------------------------
	// generate random dot starting coordinates, calculate the dots position for all frames
	// and write the dot coordinates into the array.
	int		i, j, l;
	double	strip_width, dir_rad;
	double	phase, sigma, sf;
	
	//----------------------------End January 2009, white noise patterns----
	int point_len = sizx*sizy*5; //the number of elements for frame display...
	int filt_kern = 32;	// size of filter kernel (8 times band-pass size)
	int im_x, im_y, disp_1, disp_2, disp_3, disp_4, filt; // pixel positions while filtering...
	double filt_tot1, filt_tot2, time_sigma, window_sigma, t_mod, s_mod, all_mod; // two different filter total to filter two components separately, in case of diff factors 
	double r_len, DoG1, DoG2, d_x1, d_x2, d_y1, d_y2, max_val, min_val;
	double sigma1 = 0.5 * carr_freq; // band-pass filter parameter - i.e. spatial extent in pixels, # to the left 
	double sigma2 = sigma1 * 1.6 /*1.6*/; //where 1.6 is the centre surround ratio (Marr 1982) // was 5, Jan10
	

	base_wnoise = new double [point_len]; // also includes gaussian window, pointer pos 5 
	noise_filt = new double [filt_kern*filt_kern];	
	mod_funct = new double [2*sizy + n_images]; // spatial modulators and Gaussian temporal window		
	
	double hold1, hold2; //dummy variables to see intermediate values
	if (carr_freq<0.5) filt = 0;  // 0 means no bandpass filtering, 1 means bandpass filtering...
	else filt = 1; //---> do not bandpass filter where freq is less than pixel size (pixel limit)
	
	time_sigma = n_images*0.80 /*0.25*/; // time constant for fading image in and out, one quater of interval
	window_sigma = sizx * 0.80/*0.25*/; // space constant for presenting the stimulus
	//Changed 27 Nov
	
	double val1, val2; //intermediate variables holding values during calculations

	for (i=0;i<point_len;i++) *(base_wnoise + i) = 0.0; // zero all pixels in white noise pointer

	for (i=0;i<sizx;i++) // generate random numbers between 0-1 for white noise
	for (j=0;j<sizy;j++)
	{
	val1 = rand()%5000; val2 = rand() % 5000; // to look at generated white noise values
	*(base_wnoise + i + sizx*j) = val1/5000; // return double between 0->1 
	*(base_wnoise + i + sizx*j + sizx*sizy) = val2/5000;} // assign random luminances to each pixel
	
	// preset max and min holders to zero and 1 before starting on experiment
	max_val = 0.0; min_val = 0.0;

	for (i=0;i<sizy;i++) // generate modulator functions M1 and M2
	{
		
		// case of square modulator.....
		if (mod_type==1)
		{
			*(mod_funct + i) = 0.5 * (floor)(0.499999 * sin(2*pi*mod_freq*i/sizy));  // square wave modulator		
			*(mod_funct + i + sizy) = 0.5 * (floor)(0.499999 * -sin(2*pi*mod_freq*i/sizy));
		}
		// case of sine modulator.....
		if (mod_type==0)
		{
			*(mod_funct + i) = 0.5 * (1 + mod_amp * sin(2*pi*mod_freq*i/sizy));  // simple sine wave modulator		
			*(mod_funct + i + sizy) = 0.5 *( 1 - mod_amp * sin(2*pi*mod_freq*i/sizy));
		}
		
		// case of root sine modulator.....
		if (mod_type==2)
		{
			*(mod_funct + i) = sqrt(0.5 * (1 + mod_amp * sin(2*pi*mod_freq*i/sizy)));  // square root for normalising property		
			*(mod_funct + i + sizy) = sqrt(0.5 *( 1 - mod_amp * sin(2*pi*mod_freq*i/sizy)));
		}

		// case of no modulation.....
		if (mod_type==3)
		{
			*(mod_funct + i) = 1 ;  // constant modulator (for comparison...)		
			*(mod_funct + i + sizy) = 1;
		}
	
		// gaussian temporal window here, assumes sizx larger than n_images
		
		if (i<n_images) // populate temporal modulation gaussian
		{
			r_len = (i-n_images*0.5); // when r=0, middle frame has maximum contrast
			DoG1 = 1.0/(time_sigma*sqrt(2.0*pi)) 
				* exp( r_len*r_len/(-2.0*time_sigma*time_sigma)); //not a DoG, just a G!!
			*(mod_funct + i + 2*sizy) = DoG1;
			
		} // end of temporal modulation steps
	}
	
	t_mod = 1.0/(time_sigma*sqrt(2.0*pi)); // this gives the amplitude scaling of the Gaussian	
	s_mod = 1.0/(window_sigma*window_sigma*2.0*pi);  // amplitude scaling of 2D Gaussian

	for (i=0;i<filt_kern;i++)
	for (j=0;j<filt_kern;j++) // generate band-pass filtering Kernel
	{
		r_len = sqrt((double)(i-filt_kern*0.5)*(i-filt_kern*0.5) 
			+ (double)(j-filt_kern*0.5)*(j-filt_kern*0.5)); // find radial length	
		DoG1 = 1.0/(sigma1*sigma1*2.0*pi) * exp( r_len*r_len/(-2.0*sigma1*sigma1)); // centre 
		DoG2 = 1.0/(sigma2*sigma2*2.0*pi) * exp( r_len*r_len / (-2.0*sigma2*sigma2)); // surround
		*(noise_filt + i + j*filt_kern) =  DoG1 - DoG2; // difference of Gaussians
	}// end of band-pass filter generating kernel
	
		// ************SPATIAL GAUSSIAN WINDOW ******************** 
	for (i=0;i<sizx;i++)
	for (j=0;j<sizy;j++) // generate band-pass filtering Kernel
	{
		r_len = sqrt((double)(i-sizx*0.5)*(i-sizx*0.5) 
			+ (double)(j-sizy*0.5)*(j-sizy*0.5)); // find radial length	
		DoG1 = 1.0/(window_sigma*window_sigma*2.0*pi) 
			*exp(r_len*r_len/(-2.0*window_sigma*window_sigma)); //Gaussian 
		*(base_wnoise + i + j*sizx + 4*sizx*sizy ) =  DoG1; // Window, set to 1!
	}// end of spatial window generating kernel
	
	max_val = 0;	// to store maximum value
	min_val = 1;  // to store minimum value
	
	for (i=0;i<sizx;i++)
	for (j=0;j<sizy;j++) // Convolution with band-passing filter 
	{
		
		if (filt !=0)
		{
		for (kk=0;kk<filt_kern;kk++) // run through the filter kernel for each pixel
		for (ll=0;ll<filt_kern;ll++)
		{
			if (kk==0 && ll==0) 
			{filt_tot1 = 0.0;
			filt_tot2 = 0.0;
			if (i==0 && j==0)	//condition to indicate within program than filtering is occuring... AMJan09
			{
				m_in_progress = _T("...FILTERING"); //Indicate...
				UpdateData(FALSE);
				CClientDC ClientDC( this );
				RedrawWindow();	
			}
			} // at the start of a new pix position, reset filter value to 0
			im_x = i + (int)(kk-filt_kern*0.5); //gives the x vector of convolution point 
			im_y = j + (int)(ll-filt_kern*0.5); // gives the y vector of the convolution point
			while (im_x < 0) im_x += (sizx-1);
			while (im_y < 0) im_y += (sizy-1);
			while (im_x >= sizx) im_x += (1-sizx); // wrap around when filtering...
			while (im_y >= sizy) im_y += (1-sizy);

			filt_tot1 += *(noise_filt + kk + ll*filt_kern) * *(base_wnoise + im_x + sizx*im_y ); // image 1
			filt_tot2 += *(noise_filt + kk + ll*filt_kern) * *(base_wnoise + im_x + sizx*im_y 
				+ sizx*sizy); // image 2
			if (kk==filt_kern-1 && ll==filt_kern-1) 
			{
				*(base_wnoise + i + sizx*j + 2*sizx*sizy)= filt_tot1; // representing component 1 (post filtering)
				*(base_wnoise + i + sizx*j + 3*sizx*sizy)= filt_tot2;
				// find maximum and minimum for later scaling
				if ((filt_tot1 + filt_tot2)> max_val) max_val = (filt_tot1 + filt_tot2);
				if ((filt_tot1 + filt_tot2)< min_val) min_val = (filt_tot1 + filt_tot2);
			
				if (i==sizx-1 && j==sizy-1)	//condition to indicate end of filtering... AMJan09
				{
					m_in_progress = _T("BLANK"); //Indicate...
					UpdateData(FALSE);
					CClientDC ClientDC( this );
					RedrawWindow();	
				}
			} // representing component 2
     		
		} // end of loop and convolution doing the filtering 
		} // end of if condition for filt
		
		if (filt == 0) // alternative without filtering...
			{
			hold1 = *(base_wnoise + i + sizx*j); //m1 
			hold2 = *(base_wnoise + i + sizx*j + sizx*sizy); //m2
			*(base_wnoise + i + sizx*j + 2*sizx*sizy)= hold1; // representing comp1 (post filtering)
			*(base_wnoise + i + sizx*j + 3*sizx*sizy)= hold2;
			
			if ((hold1 + hold2)> max_val) max_val = (hold1 + hold2);
			if ((hold1 + hold2)< min_val) min_val = (hold1 + hold2);
			} // representing component 2
	
	} //end of convolution
		
	sprintf( out_filnam, "stim\\Wnsgrad_%s.char", out_name_str );	// generate output pattern filename 
			if ( (fo=fopen(out_filnam,"wb")) == NULL )		// test whether ascii file can be opened 
					{ AfxMessageBox( "cannot save ascii file (open?)!" ); return; }	// prevent save without open file

		// save header: size, name, scale
		//double	scal = 9999.0/765.0;	// scale factor: greyvalues vary between 0...765 >> convert to 0...10000
		fprintf(fo, "%4d %4d pattern : %s", sizx, sizy, out_filnam );	// save size in ascii file
		//fprintf(fo, "%12.6f      scale factor\n", 10000.0 );				// save scale factor in ascii file
	
	// AM feb 09 code to determine translation direction of white noise 
	d_x1 = disp * cos(dir*2*pi/360); // horiz translational displacement 
	d_x2 = disp2 * cos((dir + dirdiff)*2*pi/360);
	d_y1 = disp * sin(dir*2*pi/360); // vertical translational displacement /// 
	d_y2 = disp2 * sin((dir + dirdiff)*2*pi/360); // division by 360 to get radians from deg...
	
	//LOOP Below to start running through images frame by frame, to draw them!!!
	for (loopi=0; loopi<n_images; loopi++)		// run through image sequence loop
		{
		BeginWaitCursor();

		// prepare data array for greylevel values 
		pGvalues = new unsigned short int [num_pixels];	// allocate space for image on heap
		if ( pGvalues==NULL )							// warning if array doesn't get requested heap space
			{ AfxMessageBox( "Could not create image heap space !" ); return; }

		// make white backround, store data in data array for display and storage in ascii file 
		dumy = 1.0;	
		dumi = GreyVal(dumy);		// convert to 0-255 greylevel value
		for (jj=0; jj<sizy; jj++ )	// run through sizy rows - set to white (background)
			{
			for (ii=0; ii<sizx; ii++ )		// run through sizx columns
				*(pGvalues + jj*sizx + ii) = 3*dumi;	// set sequence array greylevel value 
			}	/* end run through rows */

		dumy = 0.0;	
		dumi = GreyVal(dumy);	// convert to 0-255 greylevel value - set to black
	
		// save in stm files (in stim subdirectory)
		// prepare and open asci data file 
			
		
		for (ii=0;ii<sizx; ii++)
		for (jj=0;jj<sizy; jj++)
		{
			disp_1 = (int)(ii + d_x1 * loopi); // component one displaced position
			disp_2 = (int)(ii + d_x2 * loopi); // component 2 diplaced position in frame
			while (disp_1>(sizx-1)) disp_1 -= sizx-1;
			while (disp_1<0) disp_1 += sizx-1;
			while (disp_2>(sizx-1)) disp_2 -= sizx-1;
			while (disp_2<0) disp_2 += sizx-1;  // horizontal motion

			disp_3 = (int)(jj + d_y1 * loopi); // component one displaced position
			disp_4 = (int)(jj + d_y2 * loopi); // component 2 diplaced position in frame
			
			while (disp_3>(sizy-1)) disp_3 -= sizy-1;
			while (disp_3<0) disp_3 += sizy-1;
			while (disp_4>(sizy-1)) disp_4 -= sizy-1;
			while (disp_4<0) disp_4 += sizy-1;  // vertical motion

			if ((max_val - min_val) != 0)
			{
				hold1 = *(base_wnoise + disp_1 + sizx*disp_3 + 2*sizx*sizy)* *(mod_funct + jj);
				hold2 = *(base_wnoise + disp_2 + sizx*disp_4 + 3*sizx*sizy)* *(mod_funct + jj + sizy);
				//uncomment below to reduce to one component... AM Mar2012
				//hold1 = 0;
				all_mod = 1 * 	*(mod_funct + 2*sizx + loopi)/t_mod   // change the no. 1 to modulate Lo 
					* *(base_wnoise + ii + jj*sizx + 4*sizx*sizy )/s_mod;
				
				hold1 = (((hold1 + hold2)*(1+mod_amp*0.5) - min_val)/(max_val-min_val))* all_mod; // both components				
				*(pGvalues + jj*sizx + ii) = GreyVal(hold1 + (1-(all_mod))*0.5);
			} // condition calculating the various 
		}
		
		// save image data (pixel greylevel)
		for (jj=0; jj<sizy; jj++)			// run through rows of image
			{
			for (ii=0; ii<sizx; ii++)		// run through columns of row
				{ 
				dumi = *(pGvalues + jj*sizx + ii);	// pick current pixel's grey value
				outi = (int)((double)dumi / 3.0);	// scale to 0...10000
				fprintf(fo, "%1c", outi);			// save into ascii file
				}						// end of current row 
			}							// end of all rows of image

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
			RedrawWindow();	// erase previous image and messages -> contour stim frame
			m_pImageObject->Draw( &ClientDC, 10, 155 );
			sprintf( out_str, "%s.char frame %d generated: %dx%d min|max %d|%d", 
						out_name_str, loopi+1, sizx,sizy, gmin,gmax );
			ClientDC.TextOut(10, 140, out_str);
			}	

		//	save gif output //////////////////////////////////////////////////////////
		if ( m_pImageObject != NULL )
			{
			sprintf( out_filnam, "giff\\Wnsgrad_%s%d.gif", out_name_str, loopi+1 );	// generate output pattern filename 
			success = m_pImageObject->Save(out_filnam, -1);			// save image as gif
			sprintf( out_str, "saved as: giff/%s%d.gif %d x %d : %d", 
						out_name_str, loopi+1, sizx, sizy, success );	// generate output string
			ClientDC.TextOut(350, 140, out_str);						// display string
			}
		else
			{ RedrawWindow();			// erase previous image and messages
			  ClientDC.TextOut(350, 140, " an unknown problem has occured...                     "); }

		Sleep(DELAY);
		EndWaitCursor();
		delete pGvalues;		pGvalues = 0;		// free memory and reset pointer
		}	// end running through image sequence loop
		
		delete mod_funct;		mod_funct = 0;		// free memory & reset pointer
		delete noise_filt;		noise_filt = 0;		// free memory & reset pointer

		
		fprintf(fo, " comment 1 " );	fprintf(fo, " comment 2 " );
		fprintf(fo, " comment 3 " );		fprintf(fo, " comment 4 " );
		fprintf(fo, " comment 5 " );	// save various comments in ascii file
		fclose(fo);						// close ascii file
}


void CMy2DMD_GENPATDlg::OnGenpplaids()   //code to generate segmented motion AM Jan08 
{
	// TODO: Add your control notification handler code here
	// generate sequence of greylevel images sequence and save as stm files
	char out_str[80];
	int outi, loopi, ii, jj, kk, ll, mm, nn,pp, dumi, harms, c_index;
	double r, theta, psi, epsilon, hypo, dumy, c_val, speed, max, min, xx,yy, fix_rad; // variables for trig
	//AM APRIL 2009!!!
	double *grat_holder; // pointer to hold the grating value
	grat_holder = new double [2*sizx*sizy*n_images]; // store separate sine waves
	
	BOOL success = 0;
	FILE	*fo;
	char 	out_filnam[100];  
	
	harms = dotsiz; // number of harmonics
	CClientDC ClientDC( this );

	for (ii=0; ii<(2*sizx*sizy*n_images); ii++) *(grat_holder + ii) = 0.0; // preset to zero
	
	// generate the two separate plaids below... Am Sept2009
	for (ii=0; ii<sizx; ii++) // x position
	for (jj=0; jj<sizx; jj++)	// y position
	for (kk=0; kk<2; kk++) // component, 1 or 2
	for (ll=0; ll<n_images; ll++) // number of frames...
	{ //loop generating  
		c_index = ii + jj*sizx + ll*sizx*sizy + kk*sizx*sizy*n_images;
		theta = (dir + kk*dirdiff)* 2*pi/360; // calculate orientation angle in radians
		if (kk==0) speed = disp;
		if (kk==1) speed = disp2;
		
		hypo = sqrt((double)(ii*ii + jj*jj)); // hypotenuse (spelling??)
		if (hypo!=0) epsilon = asin((double)(jj/hypo)); else epsilon = asin(jj/(hypo+0.001)); // avoid division by 0 
		psi = abs(theta - epsilon); // angle 
		
		r =  cos(psi)* hypo; // calculate phase of current location, based on orienated grat

		for (mm=0; mm<harms;mm++)	// run through no of harmonics, create harmonic grating stimuli
		{
			*(grat_holder + c_index) += (sin(2*pi/carr_freq*(r+speed*ll)*(2*(mm+1)-1)))/(2*(mm+1)-1); 
			// values about are between 0->1 and then scaled appropriately by harmonic scaler
		}

	} // end of generating both grating components
	
	max=-1; min=+1; // set max and min levels at the extremes... AMAPR09

	for (ii=0;ii<2*sizx*sizy*n_images; ii++)
	{
		c_val = *(grat_holder + ii);
		if (c_val>max) max= c_val;
		if (c_val<min) min= c_val;
	} // end of loop to identify minimum and maximum values for scaling of responses....
	
	
	// generate pseudo-plaid stimulus for display...
	unsigned int *aperture;  // to store aperture template
	double *stim_disp; // to store full generated pseudo-plaids before display
	unsigned int *rand_pos; // to store jitter and component assignement of each aperture
	int num_aper, jitter, aper_siz;
	double aper_ratio;

	//--------VARIABLES-------------------------------
	num_aper = 8; // number of apertures in each direction, total = num_aper * num_aper 
	if (mod_freq>1)
	{ AfxMessageBox( "Could not create create apertures with ap_rat>1!" ); return; } // give a warning that apaerture cannot be created
	aper_ratio = mod_freq; // the ratio of length of aperture compared to full stim region 
	fix_rad = 2; // radius of fixation spot... pre-defined... AMsept09
	//--------FIXED--------------------------------------
	aper_siz = (int)(aper_ratio * sizx/num_aper); // this is the size of each aperture
	jitter = (int)(sizx/num_aper-aper_siz); // this is how much each stim can be jittered
	
	aperture = new unsigned int[aper_siz*aper_siz];
	for (ii=0;ii<aper_siz;ii++) // not sure of this step!!!
	for (jj=0;jj<aper_siz;jj++)
	{
		c_index = ii + jj*aper_siz; // index for aperture pointer position
		r = sqrt((ii-0.5*aper_siz)*(ii-0.5*aper_siz) + (jj-0.5*aper_siz)*(jj-0.5*aper_siz)); 
		if (r<(0.5*aper_siz)) *(aperture + c_index)= 1; else *(aperture + c_index)=0; 
	} // default aperture created...

	rand_pos = new unsigned int [3*num_aper*num_aper]; // to store jitter 
	
	for (ii=0;ii<3*num_aper*num_aper;ii++) *(rand_pos + ii)=0;

	for (ii=0;ii<num_aper;ii++)
	for (jj=0;jj<num_aper;jj++)
	{
		ll = rand() % jitter; // x jitter
		mm = rand() % jitter; // y jitter
		c_index = ii + jj*num_aper; // index for current 

		*(rand_pos + c_index) = ll; // x jitter
		*(rand_pos + c_index + (num_aper*num_aper)) = mm; // y jitter
	
	   	if (ii==0 && jj==0) // assign the component to be displayed in each aperture here... 1 or 2
		{
			c_val = 0.0; // counter... goes up to number of apertures.....
			while (c_val<num_aper*num_aper*0.5) // populate half the apertures wiht one direction
			{
			nn = rand()%(num_aper*num_aper); // show comp 1 or 2...
			dumy = *(rand_pos + nn + 2*num_aper*num_aper); // check if current aperture is assigned
			
			if (dumy==0.0)
				{
					*(rand_pos + nn + 2*num_aper*num_aper) = 1; //assign opp direction
					c_val += 1.0; //increment counter....
				} //end of if, checking current position
			} // end of while loop, should not go on forever
		} // end of if condition for assigning component positions.
	} // end of stochastic processes...

	stim_disp = new double [sizx*sizy*n_images];
	
	for (ii=0;ii<sizx*sizy*n_images;ii++) *(stim_disp + ii) = 0.0; //initialise with values zero 
	
	//generate plaid stimuli with fixation and fovea exclusion
	for (mm=0;mm<n_images;mm++)  // number of frames....
	for (kk=0; kk<num_aper; kk++)
	for (ll=0; ll<num_aper; ll++)// to run through aperture positions... 
	for (ii=0; ii<aper_siz; ii++)
	for (jj=0; jj<aper_siz; jj++)// to construct each individual aperture
	{
		nn = *(rand_pos + kk + num_aper*ll); // x jitter
		pp = *(rand_pos + kk + num_aper*ll + num_aper*num_aper); // y jitter
		
		xx = kk*sizx/8 + nn + ii;//x position in full image
		yy = ll*sizx/8 + pp + jj;//y position in full image

		r = sqrt((xx-0.5*sizx)*(xx-0.5*sizx) + (yy-0.5*sizy)*(yy-0.5*sizy)); // find radial dist...

		dumy = *(rand_pos + kk + ll*num_aper + 2*num_aper*num_aper); // component assignment
		
		if (dumy==0.0) // show left component in aperture
		{
			c_index = xx + yy*sizx + sizx*sizy*mm ; // index for stored vals
            c_val = *(grat_holder + c_index); // different index position from next if() 
			*(stim_disp + c_index) = c_val * *(aperture + ii + jj*aper_siz); 
		}
	
		if (dumy==1.0) // show right component in aperture
		{
			c_index = xx + yy*sizx + sizx*sizy*mm ; // index for stored vals
            c_val = *(grat_holder + c_index + sizx*sizy*n_images);
			*(stim_disp + c_index) = c_val * *(aperture + ii + jj*aper_siz); 
			// New addition: to remove half of the component signal
			if (disp2==0) *(stim_disp + c_index) = 0;		
		}

		if (r<aper_siz*mod_amp) //exclude fovea region AM Apr11 (changeable, now use m)
		{
			c_index = xx + yy*sizx + sizx*sizy*mm ; // index for stored vals
            *(stim_disp + c_index) = 0; // exclusion region spot, set to near local black... 
		}
	
	
	} // end of nested loops going through apertures

	//loop to put in fixation spot... AM sept09
	for (mm=0;mm<n_images;mm++)  // number of frames....
	for (kk=0; kk<fix_rad*2; kk++)
	for (ll=0; ll<fix_rad*2; ll++)//
	{
		r = sqrt((kk-fix_rad)*(kk-fix_rad)+ (ll-fix_rad)*(ll-fix_rad));// radial coor
		xx = (kk-fix_rad)+0.5*sizx; yy = (ll-fix_rad)+0.5*sizy; // find x & y coords!!! amsept09 

		if (r<=fix_rad)
		{
		c_index = xx + yy*sizx + sizx*sizy*mm ; // index for stored vals
        *(stim_disp + c_index) = 0.95*min; // dark almost circular fixation spot...
		}
	}

	sprintf( out_filnam, "stim\\psplaid_%s.char", out_name_str );	// generate output pattern filename 
		if ( (fo=fopen(out_filnam,"wb")) == NULL )		// test whether ascii file can be opened 
			{ AfxMessageBox( "cannot save ascii file (open?)!" ); return; }	// prevent save without open file

	fprintf(fo, "%4d %4d pattern : %s", sizx, sizy, out_filnam );	// save size in ascii file
	//LOOP Below to start running through images frame by frame, to draw them!!!
	for (loopi=0; loopi<n_images; loopi++)		// run through image sequence loop
		{
		BeginWaitCursor();

		// prepare data array for greylevel values 
		pGvalues = new unsigned short int [sizx*sizy*3];	// allocate space for image on heap
		if ( pGvalues==NULL )							// warning if array doesn't get requested heap space
			{ AfxMessageBox( "Could not create display image heap space !" ); return; }

		pIvalues = new unsigned short int [sizx*sizy];	// allocate space for image on heap
		if ( pIvalues==NULL )							// warning if array doesn't get requested heap space
			{ AfxMessageBox( "Could not create saving image heap space !" ); return; }

			// make white backround, store data in data array for display and storage in ascii file 
		dumy = 1.0;	
		dumi = GreyVal(dumy);		// convert to 0-255 greylevel value
		for (jj=0; jj<sizy; jj++ )	// run through sizy rows - set to white (background)
			{
			for (ii=0; ii<sizx*3; ii++ )		// run through sizx columns
				*(pGvalues + jj*sizx*3 + ii) = 3*dumi;	// set sequence array greylevel value 
				if (ii<sizx) *(pIvalues + jj*sizx + ii) = 3*dumi; // set stimulus array greylevel value
			}	/* end run through rows */

		dumy = 0.0;	
		dumi = GreyVal(dumy);	// convert to 0-255 greylevel value - set to black
	
		// save in stm files (in stim subdirectory)
		// prepare and open asci data file 
		
		for (ii=0;ii<sizx*2; ii++)
		for (jj=0;jj<sizy; jj++)
		{	
			if (ii<sizx)kk=0; else kk=1; // kk component number currently being used (first or second)
			if (kk==0) ll=ii; else ll = ii - sizx; // use ll instead of ii to index components   
			c_index = ll + jj*sizx + sizx*sizy*loopi + sizx*sizy*n_images*kk; // index for stored vals (kk)
            c_val = *(grat_holder + c_index);
			c_val = (c_val-min)/(max-min); // produce normalised pixel value, range 0 to 1
			
			if (c_val>1.0) c_val = 1.0; if (c_val<0.0) c_val = 0.0; // cap upper and lower values
			*(pGvalues + jj*sizx*3 + ii) = 255*3*c_val;

			if (kk==0) // fill the aperture stimulus positions for the plaids....
			{
				c_val = *(stim_disp + c_index - sizx*sizy*n_images*kk); // 
				c_val = (c_val-min)/(max-min); // produce
				*(pGvalues + jj*sizx*3 + ii + sizx*2) = 255*3*c_val;
				*(pIvalues + jj*sizx + ii) = 255*3*c_val; // image used for storing just stim
			
			}

		}
		
		int sav_arr = 1; // array to be saved, full, comp+stim:0 or stimulus:1
		
		// save image data (pixel greylevel), either full comp + stim or just stim
		if (sav_arr == 0) // to save the components and the multiaperture plaid
		{
		for (jj=0; jj<sizy; jj++)			// run through rows of image
			{
			for (ii=0; ii<sizx*3; ii++)		// run through columns of row
				{ 
				dumi = *(pGvalues + jj*sizx*3 + ii);	// pick current pixel's grey value
				outi = (int)((double)dumi / 3.0);	// scale to 0...10000
				fprintf(fo, "%1c", outi);			// save into ascii file
				}						// end of current row 
			}							// end of all rows of image
		}

		if (sav_arr == 1) // to save just the stimulus
		{
		for (jj=0; jj<sizy; jj++)			// run through rows of image
			{
			for (ii=0; ii<sizx; ii++)		// run through columns of row
				{ 
				dumi = *(pIvalues + jj*sizx + ii);	// pick current pixel's grey value
				outi = (int)((double)dumi / 3.0);	// scale to 0...10000
				fprintf(fo, "%1c", outi);			// save into ascii file
				}						// end of current row 
			}							// end of all rows of image
		}
		

		// create buffer for display image and transfer data from array into image buffer
		m_pImageObject = new CImageObject( "null.bmp" );	// by loading empty 8-bit greylevel bmp-file
		dumi = m_pImageObject->Stretch( sizx*3, sizy );		// resize to curent image size
		if ( (m_pImageObject==NULL) || (dumi == 0))			// warning if anything went wrong
			{ AfxMessageBox( "Could not create temporary image !" ); return; }	// when setting up display image
		CImagePointProcesses mImagePointProcess( m_pImageObject );  // make image available for point processing
		mImagePointProcess.Dump8bitData( pGvalues, sizx*3, sizy );	// convert greylevels in array on heap into image pixels
		 

		// show image and report data load activity
		if ( (m_pImageObject==NULL) || (sizx==0) || (sizy==0) )
			{ m_pImageObject=NULL; AfxMessageBox( "Could not create image !" ); return; }
		else
			{ 
			RedrawWindow();	// erase previous image and messages -> contour stim frame
			m_pImageObject->Draw( &ClientDC, 10, 155 );
			sprintf( out_str, "%s.char frame %d generated: %dx%d min|max %d|%d", 
						out_name_str, loopi+1, sizx*3,sizy, gmin,gmax );
			ClientDC.TextOut(10, 140, out_str);
			}	

		//	save gif output //////////////////////////////////////////////////////////
		
		if (sav_arr==0) // to save full thing, components + stimuli
		{
		if ( m_pImageObject != NULL )
			{
			sprintf( out_filnam, "giff\\psplaid_%s%d.gif", out_name_str, loopi+1 );	// generate output pattern filename 
			success = m_pImageObject->Save(out_filnam, -1);			// save image as gif
			sprintf( out_str, "saved as: giff/%s%d.gif %d x %d : %d", 
						out_name_str, loopi+1, sizx*3, sizy, success );	// generate output string
			ClientDC.TextOut(350, 140, out_str);						// display string
			}
		else
			{ RedrawWindow();			// erase previous image and messages
			  ClientDC.TextOut(350, 140, " an unknown problem has occured...                     "); }
		}

		if (sav_arr==1) // to save just multiaperture stimuli
		{
			dumi = m_pImageObject->Stretch( sizx, sizy );		// resize to curent image size
			CImagePointProcesses mImagePointProcess( m_pImageObject );  // make image available for point processing
			mImagePointProcess.Dump8bitData( pIvalues, sizx, sizy );	// convert greylevels in array on heap into image pixels
			if ( m_pImageObject != NULL )
				{
				sprintf( out_filnam, "giff\\psplaid_%s%d.gif", out_name_str, loopi+1 );	// generate output pattern filename 
				success = m_pImageObject->Save(out_filnam, -1);			// save image as gif
				sprintf( out_str, "saved as: giff/%s%d.gif %d x %d : %d", 
							out_name_str, loopi+1, sizx, sizy, success );	// generate output string
				ClientDC.TextOut(350, 140, out_str);						// display string
				}
			else
				{ RedrawWindow();			// erase previous image and messages
				ClientDC.TextOut(350, 140, " an unknown problem has occured...                     "); }
		}
			
		Sleep(DELAY);
		EndWaitCursor();
		delete pGvalues;		pGvalues = 0;		// free memory and reset pointer
		delete pIvalues;		pIvalues = 0;	
}	

		
}


void CMy2DMD_GENPATDlg::OnGenreplaids()   //code to generate segmented motion AM Jan08 
{
	// TODO: Add your control notification handler code here
	// generate sequence of greylevel images sequence and save as stm files
	char out_str[80];
	int outi, loopi, ii, jj, kk, ll, mm, dumi, harms, c_index;
	double r, theta, psi, epsilon, hypo, dumy, c_val, speed, max, min, xx,yy, fix_rad; // variables for trig
	//AM APRIL 2009!!!
	double *grat_holder; // pointer to hold the grating value
	grat_holder = new double [2*sizx*sizy*n_images]; // store separate sine waves
	
	BOOL success = 0;
	FILE	*fo;
	char 	out_filnam[100];  
	
	harms = dotsiz; // number of harmonics
	CClientDC ClientDC( this );

	for (ii=0; ii<(2*sizx*sizy*n_images); ii++) *(grat_holder + ii) = 0.0; // preset to zero
	
	// generate the two separate plaids below... Am Sept2009
	for (ii=0; ii<sizx; ii++) // x position
	for (jj=0; jj<sizx; jj++)	// y position
	for (kk=0; kk<2; kk++) // component, 1 or 2
	for (ll=0; ll<n_images; ll++) // number of frames...
	{ //loop generating  
		c_index = ii + jj*sizx + ll*sizx*sizy + kk*sizx*sizy*n_images;
		theta = (dir + kk*dirdiff)* 2*pi/360; // calculate orientation angle in radians
		if (kk==0) speed = disp;
		if (kk==1) speed = disp2;
		
		hypo = sqrt((double)(ii*ii + jj*jj)); // hypotenuse (spelling??)
		if (hypo!=0) epsilon = asin((double)(jj/hypo)); else epsilon = asin(jj/(hypo+0.001)); // avoid division by 0 
		psi = abs(theta - epsilon); // angle 
		
		r =  cos(psi)* hypo; // calculate phase of current location, based on orienated grat

		for (mm=0; mm<harms;mm++)	// run through no of harmonics, create harmonic grating stimuli
		{
			*(grat_holder + c_index) += (sin(2*pi/carr_freq*(r+speed*ll)*(2*(mm+1)-1)))/(2*(mm+1)-1); 
			// values about are between 0->1 and then scaled appropriately by harmonic scaler
		}

	} // end of generating both grating components
	
	max=-1; min=+1; // set max and min levels at the extremes... AMAPR09

	for (ii=0;ii<2*sizx*sizy*n_images; ii++)
	{
		c_val = *(grat_holder + ii);
		if (c_val>max) max= c_val;
		if (c_val<min) min= c_val;
	} // end of loop to identify minimum and maximum values for scaling of responses....
	
	
	// generate pseudo-plaid stimulus for display...
	unsigned int *aperture;  // to store aperture template
	double *stim_disp; // to store full generated pseudo-plaids before display
	unsigned int *rand_pos; // to store jitter and component assignement of each aperture
	int num_aper, jitter, aper_siz;
	double aper_ratio;

	//--------VARIABLES-------------------------------
	num_aper = 8; // number of apertures in each direction, total = num_aper * num_aper 
	aper_ratio = mod_freq; // the ratio of length of aperture compared to full stim region 
	fix_rad = 2; // radius of fixation spot... pre-defined... AMsept09
	//--------FIXED--------------------------------------
	aper_siz = (int)(sizx/num_aper); // this is the size of each aperture
	jitter = 0; // this is how much each stim can be jittered
	
	// this is not necessary for real plaids.... 24 Mar2010
	aperture = new unsigned int[aper_siz*aper_siz];
	for (ii=0;ii<aper_siz;ii++) // not sure of this step!!!
	for (jj=0;jj<aper_siz;jj++)
	{
		c_index = ii + jj*aper_siz; // index for aperture pointer position
		r = sqrt((ii-0.5*aper_siz)*(ii-0.5*aper_siz) + (jj-0.5*aper_siz)*(jj-0.5*aper_siz)); 
		if (r<(0.5*aper_siz)) *(aperture + c_index)= 1; else *(aperture + c_index)=1; 
	} // default aperture created...

	stim_disp = new double [sizx*sizy*n_images];
	
	for (ii=0;ii<sizx*sizy*n_images;ii++) *(stim_disp + ii) = 0.0; //initialise with values zero 
	
	//generate plaid stimuli with fixation and fovea exclusion
	// 24 MAR this step my not be necessary in the new version of this.....
	//for (mm=0;mm<n_images;mm++)  // number of frames....
	//for (kk=0; kk<num_aper; kk++)
	//for (ll=0; ll<num_aper; ll++)// to run through aperture positions... 
	//for (ii=0; ii<aper_siz; ii++)
	//for (jj=0; jj<aper_siz; jj++)// to construct each individual aperture
	//{
	//	xx = kk*sizx/8 + ii;//x position in full image
	//	yy = ll*sizx/8 + jj;//y position in full image

	//	r = sqrt((xx-0.5*sizx)*(xx-0.5*sizx) + (yy-0.5*sizy)*(yy-0.5*sizy)); // find radial dist...

	//	//dumy = *(rand_pos + kk + ll*num_aper + 2*num_aper*num_aper); // component assignment
	//	//Comp1
	//	c_index = xx + yy*sizx + sizx*sizy*mm ; // index for stored vals
 //       c_val = *(grat_holder + c_index); // different index position from next if() 
	//	*(stim_disp + c_index) = c_val*0.5; 
	//	//Comp2
 //       c_val = *(grat_holder + c_index + sizx*sizy*n_images);
	//	*(stim_disp + c_index) += c_val*0.5; 
	//	
	//	if (r<aper_siz*1.6) //exclude fovea region AM sept09 (changeable, now 1.25 aps)
	//	{
	//		c_index = xx + yy*sizx + sizx*sizy*mm ; // index for stored vals
 //           *(stim_disp + c_index) = 0; // exclusion region spot, set to near local black... 
	//	}
	//} // end of nested loops going through apertures // above needs to be changes June 2013...
	
	// new code for generating real plaids - not pseudo-plaids... 10 June 2013
	for (mm=0;mm<n_images;mm++)  // number of frames....
	//for (kk=0; kk<num_aper; kk++)
	//for (ll=0; ll<num_aper; ll++)// to run through aperture positions... 
	for (xx=0; xx<sizx; xx++)
	for (yy=0; yy<sizy; yy++)// to construct each individual aperture
	{
		r = sqrt((xx-0.5*sizx)*(xx-0.5*sizx) + (yy-0.5*sizy)*(yy-0.5*sizy)); // find radial dist...

		//Component1
		c_index = xx + yy*sizx + sizx*sizy*mm ; // index for stored vals
        c_val = *(grat_holder + c_index); // different index position from next if() 
		*(stim_disp + c_index) = c_val*0.5; 
		//Component2 ...
        c_val = *(grat_holder + c_index + sizx*sizy*n_images);
		*(stim_disp + c_index) += c_val*0.5; 
		
		if (r<aper_siz*mod_amp) //exclude fovea region AM sept09 (changeable, now 1.25 aps)
		{
			c_index = xx + yy*sizx + sizx*sizy*mm ; // index for stored vals
            *(stim_disp + c_index) = 0; // exclusion region spot, set to near local black... 
		}
	} // end of nested loops going through apertures 


	//loop to put in fixation spot... AM sept09
	for (mm=0;mm<n_images;mm++)  // number of frames....
	for (kk=0; kk<fix_rad*2; kk++)
	for (ll=0; ll<fix_rad*2; ll++)//
	{
		r = sqrt((kk-fix_rad)*(kk-fix_rad)+ (ll-fix_rad)*(ll-fix_rad));// radial coor
		xx = (kk-fix_rad)+0.5*sizx; yy = (ll-fix_rad)+0.5*sizy; // find x & y coords!!! amsept09 

		if (r<=fix_rad)
		{
		c_index = xx + yy*sizx + sizx*sizy*mm ; // index for stored vals
        *(stim_disp + c_index) = 0.95*min; // dark almost circular fixation spot...
		}
	}

	sprintf( out_filnam, "stim\\replaids_%s.char", out_name_str );	// generate output pattern filename 
		if ( (fo=fopen(out_filnam,"wb")) == NULL )		// test whether ascii file can be opened 
			{ AfxMessageBox( "cannot save ascii file (open?)!" ); return; }	// prevent save without open file

	fprintf(fo, "%4d %4d pattern : %s", sizx, sizy, out_filnam );	// save size in ascii file
	//LOOP Below to start running through images frame by frame, to draw them!!!
	for (loopi=0; loopi<n_images; loopi++)		// run through image sequence loop
		{
		BeginWaitCursor();

		// prepare data array for greylevel values 
		pGvalues = new unsigned short int [sizx*sizy*3];	// allocate space for image on heap
		if ( pGvalues==NULL )							// warning if array doesn't get requested heap space
			{ AfxMessageBox( "Could not create display image heap space !" ); return; }

		pIvalues = new unsigned short int [sizx*sizy];	// allocate space for image on heap
		if ( pIvalues==NULL )							// warning if array doesn't get requested heap space
			{ AfxMessageBox( "Could not create saving image heap space !" ); return; }

			// make white backround, store data in data array for display and storage in ascii file 
		dumy = 1.0;	
		dumi = GreyVal(dumy);		// convert to 0-255 greylevel value
		for (jj=0; jj<sizy; jj++ )	// run through sizy rows - set to white (background)
			{
			for (ii=0; ii<sizx*3; ii++ )		// run through sizx columns
				*(pGvalues + jj*sizx*3 + ii) = 3*dumi;	// set sequence array greylevel value 
				if (ii<sizx) *(pIvalues + jj*sizx + ii) = 3*dumi; // set stimulus array greylevel value
			}	/* end run through rows */

		dumy = 0.0;	
		dumi = GreyVal(dumy);	// convert to 0-255 greylevel value - set to black
	
		// save in stm files (in stim subdirectory)
		// prepare and open asci data file 
		
		for (ii=0;ii<sizx*2; ii++)
		for (jj=0;jj<sizy; jj++)
		{	
			if (ii<sizx)kk=0; else kk=1; // kk component number currently being used (first or second)
			if (kk==0) ll=ii; else ll = ii - sizx; // use ll instead of ii to index components   
			c_index = ll + jj*sizx + sizx*sizy*loopi + sizx*sizy*n_images*kk; // index for stored vals (kk)
            c_val = *(grat_holder + c_index);
			c_val = (c_val-min)/(max-min); // produce normalised pixel value, range 0 to 1
			
			if (c_val>1.0) c_val = 1.0; if (c_val<0.0) c_val = 0.0; // cap upper and lower values
			*(pGvalues + jj*sizx*3 + ii) = 255*3*c_val;

			if (kk==0) // fill the aperture stimulus positions for the plaids....
			{
				c_val = *(stim_disp + c_index - sizx*sizy*n_images*kk); // 
				c_val = (c_val-min)/(max-min); // produce
				*(pGvalues + jj*sizx*3 + ii + sizx*2) = 255*3*c_val;
				*(pIvalues + jj*sizx + ii) = 255*3*c_val; // image used for storing just stim
			
			}

		}
		
		int sav_arr = 1; // array to be saved, full, comp+stim:0 or stimulus:1
		
		// save image data (pixel greylevel), either full comp + stim or just stim
		if (sav_arr == 0) // to save the components and the multiaperture plaid
		{
		for (jj=0; jj<sizy; jj++)			// run through rows of image
			{
			for (ii=0; ii<sizx*3; ii++)		// run through columns of row
				{ 
				dumi = *(pGvalues + jj*sizx*3 + ii);	// pick current pixel's grey value
				outi = (int)((double)dumi / 3.0);	// scale to 0...10000
				fprintf(fo, "%1c", outi);			// save into ascii file
				}						// end of current row 
			}							// end of all rows of image
		}

		if (sav_arr == 1) // to save just the stimulus
		{
		for (jj=0; jj<sizy; jj++)			// run through rows of image
			{
			for (ii=0; ii<sizx; ii++)		// run through columns of row
				{ 
				dumi = *(pIvalues + jj*sizx + ii);	// pick current pixel's grey value
				outi = (int)((double)dumi / 3.0);	// scale to 0...10000
				fprintf(fo, "%1c", outi);			// save into ascii file
				}						// end of current row 
			}							// end of all rows of image
		}
		

		// create buffer for display image and transfer data from array into image buffer
		m_pImageObject = new CImageObject( "null.bmp" );	// by loading empty 8-bit greylevel bmp-file
		dumi = m_pImageObject->Stretch( sizx*3, sizy );		// resize to curent image size
		if ( (m_pImageObject==NULL) || (dumi == 0))			// warning if anything went wrong
			{ AfxMessageBox( "Could not create temporary image !" ); return; }	// when setting up display image
		CImagePointProcesses mImagePointProcess( m_pImageObject );  // make image available for point processing
		mImagePointProcess.Dump8bitData( pGvalues, sizx*3, sizy );	// convert greylevels in array on heap into image pixels
		 

		// show image and report data load activity
		if ( (m_pImageObject==NULL) || (sizx==0) || (sizy==0) )
			{ m_pImageObject=NULL; AfxMessageBox( "Could not create image !" ); return; }
		else
			{ 
			RedrawWindow();	// erase previous image and messages -> contour stim frame
			m_pImageObject->Draw( &ClientDC, 10, 155 );
			sprintf( out_str, "%s.char frame %d generated: %dx%d min|max %d|%d", 
						out_name_str, loopi+1, sizx*3,sizy, gmin,gmax );
			ClientDC.TextOut(10, 140, out_str);
			}	

		//	save gif output //////////////////////////////////////////////////////////
		
		if (sav_arr==0) // to save full thing, components + stimuli
		{
		if ( m_pImageObject != NULL )
			{
			sprintf( out_filnam, "giff\\replaids_%s%d.gif", out_name_str, loopi+1 );	// generate output pattern filename 
			success = m_pImageObject->Save(out_filnam, -1);			// save image as gif
			sprintf( out_str, "saved as: giff/%s%d.gif %d x %d : %d", 
						out_name_str, loopi+1, sizx*3, sizy, success );	// generate output string
			ClientDC.TextOut(350, 140, out_str);						// display string
			}
		else
			{ RedrawWindow();			// erase previous image and messages
			  ClientDC.TextOut(350, 140, " an unknown problem has occured...                     "); }
		}

		if (sav_arr==1) // to save just multiaperture stimuli
		{
			dumi = m_pImageObject->Stretch( sizx, sizy );		// resize to curent image size
			CImagePointProcesses mImagePointProcess( m_pImageObject );  // make image available for point processing
			mImagePointProcess.Dump8bitData( pIvalues, sizx, sizy );	// convert greylevels in array on heap into image pixels
			if ( m_pImageObject != NULL )
				{
				sprintf( out_filnam, "giff\\replaids_%s%d.gif", out_name_str, loopi+1 );	// generate output pattern filename 
				success = m_pImageObject->Save(out_filnam, -1);			// save image as gif
				sprintf( out_str, "saved as: giff/%s%d.gif %d x %d : %d", 
							out_name_str, loopi+1, sizx, sizy, success );	// generate output string
				ClientDC.TextOut(350, 140, out_str);						// display string
				}
			else
				{ RedrawWindow();			// erase previous image and messages
				ClientDC.TextOut(350, 140, " an unknown problem has occured...                     "); }
		}
			
		Sleep(DELAY);
		EndWaitCursor();
		delete pGvalues;		pGvalues = 0;		// free memory and reset pointer
		delete pIvalues;		pIvalues = 0;	
}	

		
}

int GreyVal(double dumy)
{	// convert double greylevel value (0.0 - 1.0) to 0 - 255 greylevel value
	// return -1 for background
	int dumi = 0;

	dumi = (int)( 255.0 * dumy);		// convert to 0-255 greylevel value
	if (dumi < 0)	dumi = 0;			// exclude negative levels
	if (dumi > 255)	dumi = 255;			// cap at 255

	if (dumi>gmax) gmax = dumi;			// find maximum
	if (dumi<gmin) gmin = dumi;			// find minimum

	if (dumy == -1.0)	return(-1);		// return background condition
	else				return(3*dumi);	// return within disk condition

// end of JMZ function:  GreyVal()			
}


void CMy2DMD_GENPATDlg::OnEnChangeInsizex()
{
	// TODO:  Add your control notification handler code here
	UpdateData(TRUE); sizx = atoi(m_in_sizeX);
}

void CMy2DMD_GENPATDlg::OnEnChangeInsizey()
{
	// TODO:  Add your control notification handler code here
	UpdateData(TRUE); sizy = atoi(m_in_sizeY);
}

void CMy2DMD_GENPATDlg::OnEnChangeInnumframes()
{
	// TODO:  Add your control notification handler code here
	UpdateData(TRUE); n_images = atoi(m_in_numFrames);
}

void CMy2DMD_GENPATDlg::OnEnChangeInfilename()
{
	// TODO:  Add your control notification handler code here
	UpdateData(TRUE); sprintf( out_name_str, "%s", m_in_filename); 	
}

void CMy2DMD_GENPATDlg::OnEnChangeIndispx()
{
	// TODO:  Add your control notification handler code here
	UpdateData(TRUE); disp = atof(m_in_disp); 
	UpdateData(TRUE); disp2 = atof(m_in_disp2);
}

void CMy2DMD_GENPATDlg::OnEnChangeIndsipy()
{
	// TODO:  Add your control notification handler code here
	UpdateData(TRUE); dir = atof(m_in_dir);
}


void CMy2DMD_GENPATDlg::OnEnChangeNumdots()
{
	// TODO:  Add your control notification handler code here
	UpdateData(TRUE); n_dots = m_in_numDots;
}

void CMy2DMD_GENPATDlg::OnEnChangeNumdirs()
{
	// TODO:  Add your control notification handler code here
	UpdateData(TRUE); n_dirs = m_in_numDirs;
}

void CMy2DMD_GENPATDlg::OnEnChangeIndirdiff()
{
	// TODO:  Add your control notification handler code here
	UpdateData(TRUE); dirdiff = atof(m_in_dirdiff);
	UpdateData(TRUE); dotsiz = m_in_dotsiz; 
	UpdateData(TRUE); contype = atoi(m_in_contype);
	UpdateData(TRUE); mod_amp = atof(m_in_m_amp);
	UpdateData(TRUE); mod_freq = atof(m_in_freq_mod);
	UpdateData(TRUE); carr_freq = atof(m_in_freq_carr);
	UpdateData(TRUE); mod_type = atoi(m_in_modtype);
}