// ImagePointProcesses.h

#ifndef __IMAGEPOINTPROCESSES_H__
#define __IMAGEPOINTPROCESSES_H__

#include "ImageObject.h"

class CImagePointProcesses
{

public:
	CImagePointProcesses();
	CImagePointProcesses( CImageObject * );

	BOOL Dump24bitData( unsigned short int *hvalues, unsigned short int *vvalues, int nX2 = -1, int nY2 = -1, CImageObject *pImageObject = NULL );
	BOOL Dump8bitData( unsigned short int *gvalues, int nX2 = -1, int nY2 = -1, CImageObject *pImageObject = NULL );
	BOOL Grab8bitData( unsigned short int *gvalues, int nX2 = -1, int nY2 = -1, CImageObject *pImageObject = NULL );
	BOOL ChangeBrightness( int, int nX1 = -1, int nY1 = -1, int nX2 = -1, int nY2 = -1, CImageObject *pImageObject = NULL );
	BOOL ChangeIntensity( int, int nX1 = -1, int nY1 = -1, int nX2 = -1, int nY2 = -1, CImageObject *pImageObject = NULL );
	BOOL ReverseColors( int nX1 = -1, int nY1 = -1, int nX2 = -1, int nY2 = -1, CImageObject *pImageObject = NULL );
	BOOL MakeGray( BOOL bSetPalette = TRUE, CImageObject *pImageObject = NULL );
	BOOL Colorize( int, int, int, int, COLORREF, CImageObject *pImageObject = NULL );

	int *GetHistogram( int nX1 = -1, int nY1 = -1, int nX2 = -1, int nY2 = -1, CImageObject *pImageObject = NULL );
	void SetImageObjectClass( CImageObject * );
	int *CreateHistogram( int, int, int, int, unsigned char *, RGBQUAD *, int, CImageObject *pImageObject = NULL );

protected:
	void BrightnessValues( int, unsigned char, unsigned char, unsigned char, unsigned char *, unsigned char *, unsigned char * );
	void SetGrey( int, unsigned char, unsigned char, unsigned char, unsigned char *, unsigned char *, unsigned char * );
	void SetPCol( int, int, unsigned char, unsigned char, unsigned char, unsigned char *, unsigned char *, unsigned char * );

	CImageObject *m_pImageObject;		

};

#endif

