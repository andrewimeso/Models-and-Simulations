// ImagePointProcesses.cpp

#include "stdafx.h"
#include "ImagePointNewProc.h"
#include <math.h>

/**************************************************************************
*                                                                         *
*                               Point Processes                           *
*                                                                         *
**************************************************************************/

CImagePointProcesses::CImagePointProcesses()
{
}

CImagePointProcesses::CImagePointProcesses( CImageObject *pImageObject )
{
	m_pImageObject = pImageObject;
}

BOOL CImagePointProcesses::Dump24bitDataNL( double NLthresh, unsigned short int *hvalues, unsigned short int *vvalues, int nX2, int nY2, CImageObject *pImageObject )
{
// this function has been changed by JMZ : currently only for 24 bit colour images 
	if( pImageObject != NULL ) m_pImageObject = pImageObject;
	if( m_pImageObject == NULL ) return( FALSE );
	if( m_pImageObject->GetNumBits() == 1 ) return( FALSE );

	unsigned char *pBuffer, *pBits, *pTemp;
	BITMAPINFOHEADER *pBIH;
	RGBQUAD *pPalette;
	int nWidthBytes, nNumColors, x, y;
	int img_w, img_h, img_b;

	img_b = m_pImageObject->GetNumBits();
	// always go through all pixels one by one
	img_w = m_pImageObject->GetWidth();
	img_h = m_pImageObject->GetHeight();

	pBuffer = (unsigned char *) m_pImageObject->GetDIBPointer( &nWidthBytes, img_b );
	if( pBuffer == NULL ) return( FALSE );

	pBIH = (BITMAPINFOHEADER *) &pBuffer[sizeof(BITMAPFILEHEADER)];
	nNumColors = m_pImageObject->GetNumColors();
	pPalette = (RGBQUAD *) &pBuffer[sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)];
	pBits = (unsigned char *) &pBuffer[sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+nNumColors*sizeof(RGBQUAD)];

	CPalette *_pPalette = m_pImageObject->GetPalette();

	int nHor, nVer;
	double xpos, ypos, length;
	for( y=0; y<nY2; y++ )
		{
		pTemp = pBits;
		pTemp += ( ( ( img_h - 1 - y ) * nWidthBytes ) );
		for( x=0; x<nX2; x++ )
			{
			nHor = *(hvalues + y*nX2 + x);	// get red-green value from image ascii array (assuming sum of RGB)
			nVer = *(vvalues + y*nX2 + x);	// get blue-yellow image ascii array (assuming sum of RGB)
			xpos = ((double)nHor)/511.0 - 1.0;
			ypos = ((double)nVer)/511.0 - 1.0;
			length = sqrt(xpos*xpos + ypos*ypos);	// normalised length of motion vector
			if (length<NLthresh)			// subthreshold response vector
				{ nHor = nVer = 511; }		// set hue/intensity to zero
			else							// suprathreshold response vector
				{ nHor = (int)(511.0 + 511.0*(xpos/length) );		
				  nVer = (int)(511.0 + 511.0*(ypos/length) ); }

			// generate new RGB values and convert into palette index 
			SetPCol( nHor, nVer, pTemp[2], pTemp[1], pTemp[0], &pTemp[2], &pTemp[1], &pTemp[0] );
			pTemp += 3;							// raise index (3 byte) for next image pixel
			}
		}

	::GlobalUnlock( m_pImageObject->GetDib() );
	m_pImageObject->m_nLastError = IMAGELIB_SUCCESS;
	return( TRUE );
}

BOOL CImagePointProcesses::Dump24bitData( unsigned short int *hvalues, unsigned short int *vvalues, int nX2, int nY2, CImageObject *pImageObject )
{
// this function has been changed by JMZ : currently only for 24 bit colour images 
	if( pImageObject != NULL ) m_pImageObject = pImageObject;
	if( m_pImageObject == NULL ) return( FALSE );
	if( m_pImageObject->GetNumBits() == 1 ) return( FALSE );

	unsigned char *pBuffer, *pBits, *pTemp;
	BITMAPINFOHEADER *pBIH;
	RGBQUAD *pPalette;
	int nWidthBytes, nNumColors, x, y;
	int img_w, img_h, img_b;

	img_b = m_pImageObject->GetNumBits();
	// always go through all pixels one by one
	img_w = m_pImageObject->GetWidth();
	img_h = m_pImageObject->GetHeight();

	pBuffer = (unsigned char *) m_pImageObject->GetDIBPointer( &nWidthBytes, img_b );
	if( pBuffer == NULL ) return( FALSE );

	pBIH = (BITMAPINFOHEADER *) &pBuffer[sizeof(BITMAPFILEHEADER)];
	nNumColors = m_pImageObject->GetNumColors();
	pPalette = (RGBQUAD *) &pBuffer[sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)];
	pBits = (unsigned char *) &pBuffer[sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+nNumColors*sizeof(RGBQUAD)];

	CPalette *_pPalette = m_pImageObject->GetPalette();

	int nHor, nVer;
	for( y=0; y<nY2; y++ )
		{
		pTemp = pBits;
		pTemp += ( ( ( img_h - 1 - y ) * nWidthBytes ) );
		for( x=0; x<nX2; x++ )
			{
			nHor = *(hvalues + y*nX2 + x);	// get red-green value from image ascii array (assuming sum of RGB)
			nVer = *(vvalues + y*nX2 + x);	// get blue-yellow image ascii array (assuming sum of RGB)
			// generate new RGB values and convert into palette index 
			SetPCol( nHor, nVer, pTemp[2], pTemp[1], pTemp[0], &pTemp[2], &pTemp[1], &pTemp[0] );
			pTemp += 3;							// raise index (3 byte) for next image pixel
			}
		}

	::GlobalUnlock( m_pImageObject->GetDib() );
	m_pImageObject->m_nLastError = IMAGELIB_SUCCESS;
	return( TRUE );
}

BOOL CImagePointProcesses::Dump8bitData( unsigned short int *gvalues, int nX2, int nY2, CImageObject *pImageObject )
{
// this function has been changed by JMZ : currently only for 8 bit images 
	if( pImageObject != NULL ) m_pImageObject = pImageObject;
	if( m_pImageObject == NULL ) return( FALSE );
	
	if( m_pImageObject->GetNumBits() == 1 ) return( FALSE );

	unsigned char *pBuffer, *pBits, *pTemp;
	BITMAPINFOHEADER *pBIH;
	RGBQUAD *pPalette;
	int nWidthBytes, nNumColors, x, y;
	int img_w, img_h, img_b;

	// handle 8 bit images only
	img_b = m_pImageObject->GetNumBits();
	if (img_b != 8)	 AfxMessageBox( "warning: illegal dump image depth (8 bit only) ?" ); 
	// always go thorugh all pixels one by one
	img_w = m_pImageObject->GetWidth();
	img_h = m_pImageObject->GetHeight();

	pBuffer = (unsigned char *) m_pImageObject->GetDIBPointer( &nWidthBytes, img_b );
	if( pBuffer == NULL ) return( FALSE );

	pBIH = (BITMAPINFOHEADER *) &pBuffer[sizeof(BITMAPFILEHEADER)];
	nNumColors = m_pImageObject->GetNumColors();
	pPalette = (RGBQUAD *) &pBuffer[sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)];
	pBits = (unsigned char *) &pBuffer[sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+nNumColors*sizeof(RGBQUAD)];

	CPalette *_pPalette = m_pImageObject->GetPalette();

	//FILE    *fo;					// for control printout ////////////////
	//fo=fopen("test.txt","w");
	//fprintf(fo, "%3d|%3d|%5d   ", x, y, nGrey);			// save into ascii file
	//fclose(fo);

	unsigned char ucRed, ucGreen, ucBlue, Data;
	int nGrey;
	for( y=0; y<nY2; y++ )
		{
		pTemp = pBits;
		pTemp += ( ( ( img_h - 1 - y ) * nWidthBytes ) );
		for( x=0; x<nX2; x++ )
			{
			Data = *pTemp;						// get palette index for current image pixel
			nGrey  = *(gvalues + y*nX2 + x)/3;	// get greylevel from image ascii array (assuming sum of RGB)
			// generate new RGB values and convert into palette index 
			SetGrey( nGrey, ucRed, ucGreen, ucBlue, &ucRed, &ucGreen, &ucBlue );
			Data = (unsigned char) m_pImageObject->GetNearestIndex( ucRed, ucGreen, ucBlue, pPalette, nNumColors );
			*pTemp++ = Data;					// set new palette index for this image pixel
			}
		}

	::GlobalUnlock( m_pImageObject->GetDib() );
	m_pImageObject->m_nLastError = IMAGELIB_SUCCESS;
	return( TRUE );
}

BOOL CImagePointProcesses::Grab8bitData( unsigned short int *gvalues, int nX2, int nY2, CImageObject *pImageObject )
{
// this function has been changed by JMZ : currently only for 8 bit images 
	if( pImageObject != NULL ) m_pImageObject = pImageObject;
	if( m_pImageObject == NULL ) return( FALSE );
	
	if( m_pImageObject->GetNumBits() == 1 ) return( FALSE );

	unsigned char *pBuffer, *pBits, *pTemp;
	BITMAPINFOHEADER *pBIH;
	RGBQUAD *pPalette;
	int nWidthBytes, nNumColors, x, y;
	int img_w, img_h, img_b;

	// handle 8 bit images only
	img_b = m_pImageObject->GetNumBits();
	if (img_b != 8)	 AfxMessageBox( "warning: illegal grab image depth (8 bit only) ?" ); 
	// always go thorugh all pixels one by one
	img_w = m_pImageObject->GetWidth();
	img_h = m_pImageObject->GetHeight();

	pBuffer = (unsigned char *) m_pImageObject->GetDIBPointer( &nWidthBytes, img_b );
	if( pBuffer == NULL ) return( FALSE );

	pBIH = (BITMAPINFOHEADER *) &pBuffer[sizeof(BITMAPFILEHEADER)];
	nNumColors = m_pImageObject->GetNumColors();
	pPalette = (RGBQUAD *) &pBuffer[sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)];
	pBits = (unsigned char *) &pBuffer[sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+nNumColors*sizeof(RGBQUAD)];

	CPalette *_pPalette = m_pImageObject->GetPalette();

	unsigned char ucRed, ucGreen, ucBlue, Data;
	for( y=0; y<nY2; y++ )
		{
		pTemp = pBits;
		pTemp += ( ( ( img_h - 1 - y ) * nWidthBytes ) );
		for( x=0; x<nX2; x++ )
			{
			Data = *pTemp;						// get palette index for current image pixel
			ucRed = pPalette[Data].rgbRed;		// separate R value
			ucGreen = pPalette[Data].rgbGreen;	// separate G value
			ucBlue = pPalette[Data].rgbBlue;	// separate B value
			*(gvalues + y*nX2 + x) = ucRed + ucGreen + ucBlue;// store cummulative grey level 0..765
			*pTemp++ = Data;					// set new palette index for this image pixel
			}
		}

	::GlobalUnlock( m_pImageObject->GetDib() );
	m_pImageObject->m_nLastError = IMAGELIB_SUCCESS;
	return( TRUE );
}

BOOL CImagePointProcesses::ChangeIntensity( int nGrey, int nX1, int nY1, int nX2, int nY2, CImageObject *pImageObject )
{
// this function has been changed by JMZ : currently only for 8 bit images 
	if( pImageObject != NULL ) m_pImageObject = pImageObject;
	if( m_pImageObject == NULL ) return( FALSE );
	
	if( m_pImageObject->GetNumBits() == 1 ) return( FALSE );

	BOOL bLessThanHalf, bCompleteImage;
	m_pImageObject->NormalizeCoordinates( &nX1, &nY1, &nX2, &nY2, &bCompleteImage, &bLessThanHalf );

	unsigned char *pBuffer, *pBits, *pTemp;
	BITMAPINFOHEADER *pBIH;
	RGBQUAD *pPalette;
	int nWidthBytes, nNumColors, x, y;
	int img_w, img_h, img_b;

	// always go thorugh all pixels one by one
	img_b = m_pImageObject->GetNumBits();
	img_w = m_pImageObject->GetWidth();
	img_h = m_pImageObject->GetHeight();
	if( bCompleteImage )
		{
		nX1 = nY1 = 0;
		nX2 = img_w;	nY2 = img_h;
		}

	pBuffer = (unsigned char *) m_pImageObject->GetDIBPointer( &nWidthBytes, img_b );
	if( pBuffer == NULL ) return( FALSE );

	pBIH = (BITMAPINFOHEADER *) &pBuffer[sizeof(BITMAPFILEHEADER)];
	nNumColors = m_pImageObject->GetNumColors();
	pPalette = (RGBQUAD *) &pBuffer[sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)];
	pBits = (unsigned char *) &pBuffer[sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+nNumColors*sizeof(RGBQUAD)];

	// grey values between 0 (black) and 255 (white)
	if( nGrey < 0 ) nGrey = 0;
	else if( nGrey > 255 ) nGrey = 255;

	CPalette *_pPalette = m_pImageObject->GetPalette();

	unsigned char ucRed, ucGreen, ucBlue, Data;
	switch( m_pImageObject->GetNumBits() )
	{

	case 4:
	for( y=nY1; y<=nY2; y++ )
		{
		pTemp = pBits;
		pTemp += ( ( ( img_h - 1 - y ) * nWidthBytes ) + nX1 );
		for( x=nX1; x<=nX2; x++ )
			{
			Data = *pTemp;						// get palette index for current image pixel
			if( !( x & 1 ) ) Data >>= 4;
			else Data &= 0x0f;
			ucRed = pPalette[Data].rgbRed;		// separate R value
			ucGreen = pPalette[Data].rgbGreen;	// separate G value
			ucBlue = pPalette[Data].rgbBlue;	// separate B value
			// generate new RGB values and convert into palette index 
			SetGrey( nGrey, ucRed, ucGreen, ucBlue, &ucRed, &ucGreen, &ucBlue );
			Data = (unsigned char) m_pImageObject->GetNearestIndex( ucRed, ucGreen, ucBlue, pPalette, nNumColors );
			if( !( x & 1 ) )	{ Data <<= 4; (*pTemp) &= 0x0f;	(*pTemp) |= Data; }
			else				{ (*pTemp) &= 0xf0;	(*pTemp) |= Data; pTemp++; }
			}
		}
	break;

	case 8:
	for( y=nY1; y<=nY2; y++ )
		{
		pTemp = pBits;
		pTemp += ( ( ( img_h - 1 - y ) * nWidthBytes ) + nX1 );
		for( x=nX1; x<=nX2; x++ )
			{
			Data = *pTemp;						// get palette index for current image pixel
			ucRed = pPalette[Data].rgbRed;		// separate R value
			ucGreen = pPalette[Data].rgbGreen;	// separate G value
			ucBlue = pPalette[Data].rgbBlue;	// separate B value
			// generate new RGB values and convert into palette index 
			SetGrey( nGrey, ucRed, ucGreen, ucBlue, &ucRed, &ucGreen, &ucBlue );
			Data = (unsigned char) m_pImageObject->GetNearestIndex( ucRed, ucGreen, ucBlue, pPalette, nNumColors );
			*pTemp++ = Data;					// set new palette index for this image pixel
			}
		}
	break;

	case 16:
	for( y=nY1; y<=nY2; y++ )
		{
		pTemp = pBits;
		pTemp += ( ( ( img_h - 1 - y ) * nWidthBytes ) + nX1 );
		for( x=nX1; x<=nX2; x++ )
			{			}
		}
	break;

	case 24:
	for( y=nY1; y<=nY2; y++ )
		{
		pTemp = pBits;
		pTemp += ( ( ( img_h - 1 - y ) * nWidthBytes ) + nX1 );
		for( x=nX1; x<=nX2; x++ )
			{
			SetGrey( nGrey, pTemp[0], pTemp[1], pTemp[2], &pTemp[0], &pTemp[1], &pTemp[2] );
			pTemp += 3;					
			}
		}
	break;

	case 32:
	for( y=nY1; y<=nY2; y++ )
		{
		pTemp = pBits;
		pTemp += ( ( ( img_h - 1 - y ) * nWidthBytes ) + nX1 );
		for( x=nX1; x<=nX2; x++ )
			{
			ucRed = pTemp[0];
			ucGreen = pTemp[1];
			ucBlue = pTemp[2];
			*pTemp++ = ucRed;
			*pTemp++ = ucGreen;
			*pTemp++ = ucBlue;
			pTemp++;
			}
		}
	break;

	}

	::GlobalUnlock( m_pImageObject->GetDib() );

	m_pImageObject->m_nLastError = IMAGELIB_SUCCESS;

	return( TRUE );
}

void CImagePointProcesses::SetPCol( int nHor, int nVer, unsigned char ucRed, unsigned char ucGreen, unsigned char ucBlue, unsigned char *pucNewRed, unsigned char *pucNewGreen, unsigned char *pucNewBlue )
{	// convert horizontal and vertical response values in 2D-colour code
	if( m_pImageObject == NULL ) return;

	DWORD Red, Green, Blue;
	double xdist, ydist, rdist, angle;
	double cBlu, cRed, cGrn;
	double dBlu, dRed, dGrn;
	double iBlu, iRed, iGrn;
	double jBlu, jRed, jGrn;
	double pi = 3.1415926535;

	// determine relative coordinates, length and angle of response vector
	// NOTE response values are encoded as 0|511|1023 for -1.0|0.0|1.0 !!!!!
	xdist = ((double)(nHor-511)) / 511.0;
	ydist = ((double)(nVer-511)) / 511.0;
	rdist = sqrt(xdist*xdist + ydist*ydist);
	angle = 2.0 * atan2(ydist,xdist)/pi;

	// mix basic colour (hue) in four quadrants : colour wheel
	if (angle<-1.0)	
		{ 
		iRed=255.0; iGrn=0.0; iBlu=0.0;		// left: red
		jRed=0.0; jGrn=0.0; jBlu=255.0;		// down: blue
		dRed = -(angle+1.0)*iRed + (2.0+angle)*jRed;	// mix R component
		dGrn = -(angle+1.0)*iGrn + (2.0+angle)*jGrn;	// mix G component
		dBlu = -(angle+1.0)*iBlu + (2.0+angle)*jBlu;	// mix B component
		}	

	if ((angle>=-1.0)&&(angle<0.0))	
		{ 
		iRed=0.0; iGrn=0.0; iBlu=255.0;		// down: blue
		jRed=0.0; jGrn=255.0; jBlu=0.0;		// right: green
		dRed = -angle*iRed + (1.0+angle)*jRed;	// mix R component
		dGrn = -angle*iGrn + (1.0+angle)*jGrn;	// mix G component
		dBlu = -angle*iBlu + (1.0+angle)*jBlu;	// mix B component
		}	

	if ((angle>=0.0)&&(angle<1.0))	
		{ 
		iRed=0.0; iGrn=255.0; iBlu=0.0;		// right: green
		jRed=255.0; jGrn=255.0; jBlu=0.0;	// up: yellow
		dRed = (1.0-angle)*iRed + angle*jRed;	// mix R component
		dGrn = (1.0-angle)*iGrn + angle*jGrn;	// mix G component
		dBlu = (1.0-angle)*iBlu + angle*jBlu;	// mix B component
		}	

	if (angle>=1.0)	
		{ 
		iRed=255.0; iGrn=255.0; iBlu=0.0;	// up: yellow
		jRed=255.0; jGrn=0.0; jBlu=0.0;		// left: red
		dRed = (2.0-angle)*iRed + (angle-1.0)*jRed;	// mix R component
		dGrn = (2.0-angle)*iGrn + (angle-1.0)*jGrn;	// mix G component
		dBlu = (2.0-angle)*iBlu + (angle-1.0)*jBlu;	// mix B component
		}	

	// reduce intensity of colour according to length of response vector
	if (rdist>1.0) rdist=1.0;	// ensure maximim response intensity
	cRed = 255.0 - rdist * (255.0-dRed);
	cGrn = 255.0 - rdist * (255.0-dGrn);
	cBlu = 255.0 - rdist * (255.0-dBlu);

	// set pixel values
	Red   = (DWORD) cRed;		ucRed   = (unsigned char) Red;
	Green = (DWORD) cGrn;		ucGreen = (unsigned char) Green;
	Blue  = (DWORD) cBlu;		ucBlue  = (unsigned char) Blue;

	*pucNewRed = ucRed;
	*pucNewGreen = ucGreen;
	*pucNewBlue = ucBlue;
}


void CImagePointProcesses::SetGrey( int nGrey, unsigned char ucRed, unsigned char ucGreen, unsigned char ucBlue, unsigned char *pucNewRed, unsigned char *pucNewGreen, unsigned char *pucNewBlue )
{	// set greylevel 
	if( m_pImageObject == NULL ) return;

	DWORD Red, Green, Blue;

	Red   = (DWORD) nGrey;		ucRed   = (unsigned char) Red;
	Green = (DWORD) nGrey;		ucGreen = (unsigned char) Green;
	Blue  = (DWORD) nGrey;		ucBlue  = (unsigned char) Blue;

	*pucNewRed = ucRed;
	*pucNewGreen = ucGreen;
	*pucNewBlue = ucBlue;
}

BOOL CImagePointProcesses::ChangeBrightness( int nBrightness, int nX1, int nY1, int nX2, int nY2, CImageObject *pImageObject )
{
	if( pImageObject != NULL ) m_pImageObject = pImageObject;
	if( m_pImageObject == NULL ) return( FALSE );
	
	if( m_pImageObject->GetNumBits() == 1 ) return( FALSE );
	if( nBrightness == 100 ) return( TRUE );

	BOOL bLessThanHalf, bCompleteImage;
	m_pImageObject->NormalizeCoordinates( &nX1, &nY1, &nX2, &nY2, &bCompleteImage, &bLessThanHalf );

	unsigned char *pBuffer, *pBits, *pTemp;
	BITMAPINFOHEADER *pBIH;
	RGBQUAD *pPalette;
	int nWidthBytes, nNumColors, x, y, i;

	pBuffer = (unsigned char *) m_pImageObject->GetDIBPointer( &nWidthBytes, m_pImageObject->GetNumBits() );
	if( pBuffer == NULL ) return( FALSE );

	pBIH = (BITMAPINFOHEADER *) &pBuffer[sizeof(BITMAPFILEHEADER)];
	nNumColors = m_pImageObject->GetNumColors();
	pPalette = (RGBQUAD *) &pBuffer[sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)];
	pBits = (unsigned char *) &pBuffer[sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+nNumColors*sizeof(RGBQUAD)];

	if( nBrightness < 0 ) nBrightness = 0;
	else if( nBrightness > 200 ) nBrightness = 200;

	LOGPALETTE *pLogPal;
	CPalette *_pPalette = m_pImageObject->GetPalette();
	if( m_pImageObject->GetNumBits() > 8 && ( pLogPal = m_pImageObject->GetLogPal() ) != NULL && ( bCompleteImage || !bLessThanHalf ) ){
		_pPalette->DeleteObject();
		for( i=0; i<nNumColors; i++ )
			BrightnessValues( nBrightness,
				pLogPal->palPalEntry[i].peRed,
				pLogPal->palPalEntry[i].peGreen,
				pLogPal->palPalEntry[i].peBlue,
				&pLogPal->palPalEntry[i].peRed,
				&pLogPal->palPalEntry[i].peGreen,
				&pLogPal->palPalEntry[i].peBlue );
		_pPalette->CreatePalette( pLogPal );
		}

	switch( m_pImageObject->GetNumBits() ){
		case 4:
			if( bCompleteImage ){
				_pPalette->DeleteObject();
				for( i=0; i<nNumColors; i++ ) BrightnessValues( nBrightness, pPalette[i].rgbRed, pPalette[i].rgbGreen, pPalette[i].rgbBlue, &pPalette[i].rgbRed, &pPalette[i].rgbGreen, &pPalette[i].rgbBlue );
				{
				LOGPALETTE *pLogPal = m_pImageObject->CreateLogPalette( pPalette, nNumColors );
				if( pLogPal != NULL ){
					_pPalette->CreatePalette( pLogPal );
					delete [] pLogPal;
					}
				}
				}
			else{
				unsigned char ucRed, ucGreen, ucBlue, Data;
				for( y=nY1; y<=nY2; y++ ){
					pTemp = pBits;
					pTemp += ( ( ( m_pImageObject->GetHeight() - 1 - y ) * nWidthBytes ) + ( nX1 / 2 ) );
					for( x=nX1; x<=nX2; x++ ){
						Data = *pTemp;
						if( !( x & 1 ) ) Data >>= 4;
						else Data &= 0x0f;
						ucRed = pPalette[Data].rgbRed;
						ucGreen = pPalette[Data].rgbGreen;
						ucBlue = pPalette[Data].rgbBlue;
						BrightnessValues( nBrightness, ucRed, ucGreen, ucBlue, &ucRed, &ucGreen, &ucBlue );
						Data = (unsigned char) m_pImageObject->GetNearestIndex( ucRed, ucGreen, ucBlue, pPalette, nNumColors );
						if( !( x & 1 ) ){
							Data <<= 4;
							(*pTemp) &= 0x0f;
							(*pTemp) |= Data;
							}
						else{
							(*pTemp) &= 0xf0;
							(*pTemp) |= Data;
							pTemp++;
							}
						}
					}
				}
			break;
		case 8:
			if( bCompleteImage ){
				_pPalette->DeleteObject();
				for( i=0; i<nNumColors; i++ ) BrightnessValues( nBrightness, pPalette[i].rgbRed, pPalette[i].rgbGreen, pPalette[i].rgbBlue, &pPalette[i].rgbRed, &pPalette[i].rgbGreen, &pPalette[i].rgbBlue );
				{
				LOGPALETTE *pLogPal = m_pImageObject->CreateLogPalette( pPalette, nNumColors );
				if( pLogPal != NULL ){
					_pPalette->CreatePalette( pLogPal );
					delete [] pLogPal;
					}
				}
				}
			else{
				unsigned char ucRed, ucGreen, ucBlue, Data;
				for( y=nY1; y<=nY2; y++ ){
					pTemp = pBits;
					pTemp += ( ( ( m_pImageObject->GetHeight() - 1 - y ) * nWidthBytes ) + nX1 );
					for( x=nX1; x<=nX2; x++ ){
						Data = *pTemp;
						ucRed = pPalette[Data].rgbRed;
						ucGreen = pPalette[Data].rgbGreen;
						ucBlue = pPalette[Data].rgbBlue;
						BrightnessValues( nBrightness, ucRed, ucGreen, ucBlue, &ucRed, &ucGreen, &ucBlue );
						Data = (unsigned char) m_pImageObject->GetNearestIndex( ucRed, ucGreen, ucBlue, pPalette, nNumColors );
						*pTemp++ = Data;
						}
					}
				}
			break;
		case 16:
			for( y=nY1; y<=nY2; y++ ){
				pTemp = pBits;
				pTemp += ( ( ( m_pImageObject->GetHeight() - 1 - y ) * nWidthBytes ) + ( nX1 * 2 ) );
				for( x=nX1; x<=nX2; x++ ){					}
				}
			break;
		case 24:
			for( y=nY1; y<=nY2; y++ ){
				pTemp = pBits;
				pTemp += ( ( ( m_pImageObject->GetHeight() - 1 - y ) * nWidthBytes ) + ( nX1 * 3 ) );
				for( x=nX1; x<=nX2; x++ ){
					BrightnessValues( nBrightness, pTemp[0], pTemp[1], pTemp[2], &pTemp[0], &pTemp[1], &pTemp[2] );
					pTemp += 3;
					}
				}
			break;
		case 32:
			for( y=nY1; y<=nY2; y++ ){
				unsigned char ucRed, ucGreen, ucBlue;
				pTemp = pBits;
				pTemp += ( ( ( m_pImageObject->GetHeight() - 1 - y ) * nWidthBytes ) + ( nX1 * 4 ) );
				for( x=nX1; x<=nX2; x++ ){
					ucRed = pTemp[0];
					ucGreen = pTemp[1];
					ucBlue = pTemp[2];


					*pTemp++ = ucRed;
					*pTemp++ = ucGreen;
					*pTemp++ = ucBlue;
					pTemp++;
					}
				}
			break;
		}

	::GlobalUnlock( m_pImageObject->GetDib() );

	m_pImageObject->m_nLastError = IMAGELIB_SUCCESS;

	return( TRUE );
}

void CImagePointProcesses::BrightnessValues( int nBrightness, unsigned char ucRed, unsigned char ucGreen, unsigned char ucBlue, unsigned char *pucNewRed, unsigned char *pucNewGreen, unsigned char *pucNewBlue )
{
	if( m_pImageObject == NULL ) return;

	DWORD Red, Green, Blue;

	Red = (DWORD) ( ( (DWORD) ucRed * (DWORD) nBrightness ) / (DWORD) 100 );
	if( Red > 255 ) Red = 255;
	else if( Red == 0 && nBrightness > 100 ) Red = (DWORD) ( ( nBrightness * 255 ) / 100 );
	ucRed = (unsigned char) Red;
	Green = (DWORD) ( ( (DWORD) ucGreen * (DWORD) nBrightness ) / (DWORD) 100 );
	if( Green > 255 ) Green = 255;
	else if( Green == 0 && nBrightness > 100 ) Green = (DWORD) ( ( nBrightness * 255 ) / 100 );
	ucGreen = (unsigned char) Green;
	Blue = (DWORD) ( ( (DWORD) ucBlue * (DWORD) nBrightness ) / (DWORD) 100 );
	if( Blue > 255 ) Blue = 255;
	else if( Blue == 0 && nBrightness > 100 ) Blue = (DWORD) ( ( nBrightness * 255 ) / 100 );
	ucBlue = (unsigned char) Blue;

	*pucNewRed = ucRed;
	*pucNewGreen = ucGreen;
	*pucNewBlue = ucBlue;
}

BOOL CImagePointProcesses::ReverseColors( int nX1, int nY1, int nX2, int nY2, CImageObject *pImageObject )
{
	if( pImageObject != NULL ) m_pImageObject = pImageObject;
	if( m_pImageObject == NULL ) return( FALSE );

	BOOL bLessThanHalf, bCompleteImage;
	m_pImageObject->NormalizeCoordinates( &nX1, &nY1, &nX2, &nY2, &bCompleteImage, &bLessThanHalf );

	unsigned char *pBuffer, *pBits, *pTemp;
	BITMAPINFOHEADER *pBIH;
	RGBQUAD *pPalette;
	int nWidthBytes, nNumColors, x, y, i;

	pBuffer = (unsigned char *) m_pImageObject->GetDIBPointer( &nWidthBytes, m_pImageObject->GetNumBits() );
	if( pBuffer == NULL ) return( FALSE );

	pBIH = (BITMAPINFOHEADER *) &pBuffer[sizeof(BITMAPFILEHEADER)];
	nNumColors = m_pImageObject->GetNumColors();
	pPalette = (RGBQUAD *) &pBuffer[sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)];
	pBits = (unsigned char *) &pBuffer[sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+nNumColors*sizeof(RGBQUAD)];

	LOGPALETTE *pLogPal = m_pImageObject->GetLogPal();
	CPalette *_pPalette = m_pImageObject->GetPalette();
	if( m_pImageObject->GetNumBits() > 8 && pLogPal != NULL && ( bCompleteImage || !bLessThanHalf ) ){
		_pPalette->DeleteObject();
		for( i=0; i<nNumColors; i++ ){
			pLogPal->palPalEntry[i].peRed = 255 - pLogPal->palPalEntry[i].peRed;
			pLogPal->palPalEntry[i].peGreen = 255 - pLogPal->palPalEntry[i].peGreen;
			pLogPal->palPalEntry[i].peBlue = 255 - pLogPal->palPalEntry[i].peBlue;
			}
		_pPalette->CreatePalette( pLogPal );
		}

	switch( m_pImageObject->GetNumBits() ){
		case 1:
			for( y=nY1; y<=nY2; y++ ){
				unsigned char Mask;
				pTemp = pBits;
				pTemp += ( ( ( m_pImageObject->GetHeight() - 1 - y ) * nWidthBytes ) + ( nX1 / 8 ) );
				for( x=nX1; x<=nX2; x++ ){
					Mask = 0x80 >> ( x & 7 );
					if( pTemp[0] & Mask ) pTemp[0] &= ( Mask ^ 0xff );
					else pTemp[0] |= Mask;
					if( ( x & 7 ) == 7 ) pTemp++;
					}
				}
			break;
		case 4:
 			if( bCompleteImage ){
				_pPalette->DeleteObject();
				for( i=0; i<nNumColors; i++ ){
					pPalette[i].rgbRed = 255 - pPalette[i].rgbRed;
					pPalette[i].rgbGreen = 255 - pPalette[i].rgbGreen;
					pPalette[i].rgbBlue = 255 - pPalette[i].rgbBlue;
					}
				{
					LOGPALETTE *pLogPal = m_pImageObject->CreateLogPalette( pPalette, nNumColors );
					if( pLogPal != NULL ){
						_pPalette->CreatePalette( pLogPal );
						delete [] pLogPal;
						}
				}
				}
			else{
				unsigned char ucRed, ucGreen, ucBlue, Data;
				for( y=nY1; y<=nY2; y++ ){
					pTemp = pBits;
					pTemp += ( ( ( m_pImageObject->GetHeight() - 1 - y ) * nWidthBytes ) + ( nX1 / 2 ) );
					for( x=nX1; x<=nX2; x++ ){
						Data = *pTemp;
						if( !( x & 1 ) ) Data >>= 4;
						else Data &= 0x0f;
						ucRed = pPalette[Data].rgbRed;
						ucGreen = pPalette[Data].rgbGreen;
						ucBlue = pPalette[Data].rgbBlue;
						ucRed = 255 - ucRed;
						ucGreen = 255 - ucGreen;
						ucBlue = 255 - ucBlue;
						Data = (unsigned char) m_pImageObject->GetNearestIndex( ucRed, ucGreen, ucBlue, pPalette, nNumColors );
						if( !( x & 1 ) ){
							Data <<= 4;
							(*pTemp) &= 0x0f;
							(*pTemp) |= Data;
							}
						else{
							(*pTemp) &= 0xf0;
							(*pTemp) |= Data;
							pTemp++;
							}
						}
					}
				}
			break;
		case 8:
			if( bCompleteImage ){
				_pPalette->DeleteObject();
				for( i=0; i<nNumColors; i++ ){
					pPalette[i].rgbRed = 255 - pPalette[i].rgbRed;
					pPalette[i].rgbGreen = 255 - pPalette[i].rgbGreen;
					pPalette[i].rgbBlue = 255 - pPalette[i].rgbBlue;
					}
				{
				LOGPALETTE *pLogPal = m_pImageObject->CreateLogPalette( pPalette, nNumColors );
				if( pLogPal != NULL ){
					_pPalette->CreatePalette( pLogPal );
					delete [] pLogPal;
					}
				}
				}
			else{
				unsigned char ucRed, ucGreen, ucBlue, Data;
				for( y=nY1; y<=nY2; y++ ){
					pTemp = pBits;
					pTemp += ( ( ( m_pImageObject->GetHeight() - 1 - y ) * nWidthBytes ) + nX1 );
					for( x=nX1; x<=nX2; x++ ){
						Data = *pTemp;
						ucRed = pPalette[Data].rgbRed;
						ucGreen = pPalette[Data].rgbGreen;
						ucBlue = pPalette[Data].rgbBlue;
						ucRed = 255 - ucRed;
						ucGreen = 255 - ucGreen;
						ucBlue = 255 - ucBlue;
						Data = (unsigned char) m_pImageObject->GetNearestIndex( ucRed, ucGreen, ucBlue, pPalette, nNumColors );
						*pTemp++ = Data;
						}
					}
				}
			break;
		case 16:
			for( y=nY1; y<=nY2; y++ ){
				pTemp = pBits;
				pTemp += ( ( ( m_pImageObject->GetHeight() - 1 - y ) * nWidthBytes ) + ( nX1 * 2 ) );
				for( x=nX1; x<=nX2; x++ ){
					}
				}
			break;
		case 24:
			for( y=nY1; y<=nY2; y++ ){
				pTemp = pBits;
				pTemp += ( ( ( m_pImageObject->GetHeight() - 1 - y ) * nWidthBytes ) + ( nX1 * 3 ) );
				for( x=nX1; x<=nX2; x++ ){
					pTemp[0] = 255 - pTemp[0];
					pTemp[1] = 255 - pTemp[1];
					pTemp[2] = 255 - pTemp[2];
					pTemp += 3;
					}
				}
			break;
		case 32:
			for( y=nY1; y<=nY2; y++ ){
				pTemp = pBits;
				pTemp += ( ( ( m_pImageObject->GetHeight() - 1 - y ) * nWidthBytes ) + ( nX1 * 4 ) );
				for( x=nX1; x<=nX2; x++ ){
					pTemp[0] = 255 - pTemp[0];
					pTemp[1] = 255 - pTemp[1];
					pTemp[2] = 255 - pTemp[2];
					pTemp[3] = 255 - pTemp[3];
					pTemp += 4;
					}
				}
			break;
		}

	::GlobalUnlock( m_pImageObject->GetDib() );

	m_pImageObject->m_nLastError = IMAGELIB_SUCCESS;

	return( TRUE );
}

BOOL CImagePointProcesses::MakeGray( BOOL bSetPalette, CImageObject *pImageObject )
{
	if( pImageObject != NULL ) m_pImageObject = pImageObject;
	if( m_pImageObject == NULL ) return( FALSE );

	unsigned char *pBuffer, *pBits, *pTemp;
	BITMAPINFOHEADER *pBIH;
	RGBQUAD *pPalette;
	int nWidthBytes, nNumColors, x, y, i;
	WORD wColorBits, *pColorBits;

	pBuffer = (unsigned char *) m_pImageObject->GetDIBPointer( &nWidthBytes, m_pImageObject->GetNumBits() );
	if( pBuffer == NULL ) return( FALSE );

	pBIH = (BITMAPINFOHEADER *) &pBuffer[sizeof(BITMAPFILEHEADER)];
	nNumColors = m_pImageObject->GetNumColors();
	pPalette = (RGBQUAD *) &pBuffer[sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)];
	pBits = (unsigned char *) &pBuffer[sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+nNumColors*sizeof(RGBQUAD)];

	unsigned char *pLookup;
	DWORD dwGray;

	switch( m_pImageObject->GetNumBits() ){
		case 1:
			break;
		case 4:
			pLookup = new unsigned char [16];
			if( pLookup == NULL ) break;
			memset( pLookup, 0, 16 );
			for( i=0; i<16; i++ ){
				dwGray = ( (DWORD) pPalette[i].rgbRed * 30 +
					(DWORD) pPalette[i].rgbGreen * 59 +
					(DWORD) pPalette[i].rgbBlue * 11 ) / 100;
				pLookup[i] = (unsigned char) dwGray;
				}
			for( i=0; i<16; i++ ){
				pPalette[i].rgbRed = pLookup[i];
				pPalette[i].rgbGreen = pLookup[i];
				pPalette[i].rgbBlue = pLookup[i];
				}
			delete [] pLookup;
			m_pImageObject->ProcessPalette();
			if( bSetPalette && m_pImageObject->m_nPaletteBytes > 0 ){
				CWindowDC WindowDC( NULL );
				m_pImageObject->SetPalette( &WindowDC );
				}
			break;
		case 8:
			pLookup = new unsigned char [256];
			if( pLookup == NULL ) break;
			memset( pLookup, 0, 256 );
			for( i=0; i<256; i++ ){
				dwGray = ( (DWORD) pPalette[i].rgbRed * 30 +
					(DWORD) pPalette[i].rgbGreen * 59 +
					(DWORD) pPalette[i].rgbBlue * 11 ) / 100;
				pLookup[i] = (unsigned char) dwGray;
				}
			for( y=0; y<pBIH->biHeight; y++ ){
				pTemp = pBits;
				pTemp += y * nWidthBytes;
				for( x=0; x<pBIH->biWidth; x++ ) pTemp[x] = pLookup[pTemp[x]];
				}
			delete [] pLookup;
			for( i=0; i<256; i++ ){
				pPalette[i].rgbRed = i;
				pPalette[i].rgbGreen = i;
				pPalette[i].rgbBlue = i;
				}
			m_pImageObject->ProcessPalette();
			if( bSetPalette && m_pImageObject->m_nPaletteBytes > 0 ){
				CWindowDC WindowDC( NULL );
				m_pImageObject->SetPalette( &WindowDC );
				}
			break;
		case 16:
			for( y=0; y<pBIH->biHeight; y++ ){
				pTemp = pBits;
				pTemp += y * nWidthBytes;
				for( x=0; x<pBIH->biWidth; x++ ){
					pColorBits = (WORD *) pTemp[x*2];
					wColorBits = *pColorBits;
					dwGray = ( (DWORD) ( ( wColorBits & 0x7c00 ) >> 10 ) * 30 +
						(DWORD) ( ( wColorBits & 0x03e0 ) >> 5 ) * 59 +
						(DWORD) ( wColorBits & 0x001f ) * 11 ) / 100;
					*pColorBits = ( ( (WORD) dwGray << 10 ) | ( (WORD) dwGray << 5 ) | (WORD) dwGray);
					}
				}
			m_pImageObject->ProcessPalette();
			if( bSetPalette && m_pImageObject->m_nPaletteBytes > 0 ){
				CWindowDC WindowDC( NULL );
				m_pImageObject->SetPalette( &WindowDC );
				}
			break;
		case 24:
			for( y=0; y<pBIH->biHeight; y++ ){
				pTemp = pBits;
				pTemp += y * nWidthBytes;
				for( x=0; x<pBIH->biWidth; x++ ){
					dwGray = ( (DWORD) pTemp[x*3+2] * 30 +
						(DWORD) pTemp[x*3+1] * 59 +
						(DWORD) pTemp[x*3] * 11 ) / 100;
					pTemp[x*3] = (unsigned char) dwGray;
					pTemp[x*3+1] = (unsigned char) dwGray;
					pTemp[x*3+2] = (unsigned char) dwGray;
					}
				}
			m_pImageObject->ProcessPalette();
			if( bSetPalette && m_pImageObject->m_nPaletteBytes > 0 ){
				CWindowDC WindowDC( NULL );
				m_pImageObject->SetPalette( &WindowDC );
				}
			break;
		case 32:
			for( y=0; y<pBIH->biHeight; y++ ){
				pTemp = pBits;
				pTemp += y * nWidthBytes;
				for( x=0; x<pBIH->biWidth; x++ ){
					dwGray = ( (DWORD) pTemp[x*4] * 30 +
						(DWORD) pTemp[x*4+1] * 59 +
						(DWORD) pTemp[x*4+2] * 11 ) / 100;
					pTemp[x*4] = (unsigned char) dwGray;
					pTemp[x*4+1] = (unsigned char) dwGray;
					pTemp[x*4+2] = (unsigned char) dwGray;
					}
				}
			m_pImageObject->ProcessPalette();
			if( bSetPalette && m_pImageObject->m_nPaletteBytes > 0 ){
				CWindowDC WindowDC( NULL );
				m_pImageObject->SetPalette( &WindowDC );
				}
			break;
		}

	::GlobalUnlock( m_pImageObject->GetDib() );

	m_pImageObject->m_nLastError = IMAGELIB_SUCCESS;

	return( TRUE );
}

BOOL CImagePointProcesses::Colorize( int nX1, int nY1, int nX2, int nY2, COLORREF Color, CImageObject *pImageObject )
{
	if( pImageObject != NULL ) m_pImageObject = pImageObject;
	if( m_pImageObject == NULL ) return( FALSE );

	if( m_pImageObject->GetNumBits() == 1 ) return( FALSE );

	BOOL bLessThanHalf, bCompleteImage;
	m_pImageObject->NormalizeCoordinates( &nX1, &nY1, &nX2, &nY2, &bCompleteImage, &bLessThanHalf );

	unsigned char *pBuffer, *pBits, *pTemp;
	BITMAPINFOHEADER *pBIH;
	RGBQUAD *pPalette;
	int nWidthBytes, nNumColors, x, y, i;

	pBuffer = (unsigned char *) m_pImageObject->GetDIBPointer( &nWidthBytes, m_pImageObject->GetNumBits() );
	if( pBuffer == NULL ) return( FALSE );

	pBIH = (BITMAPINFOHEADER *) &pBuffer[sizeof(BITMAPFILEHEADER)];
	nNumColors = m_pImageObject->GetNumColors();
	pPalette = (RGBQUAD *) &pBuffer[sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)];
	pBits = (unsigned char *) &pBuffer[sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+nNumColors*sizeof(RGBQUAD)];

	DWORD dwRed, dwGreen, dwBlue, dwGray;
	dwBlue = Color >> 16;
	dwGreen = ( Color >> 8 ) & 0x000000ff;
	dwRed = Color & 0x000000ff;

	LOGPALETTE *pLogPal = m_pImageObject->GetLogPal();
	CPalette *_pPalette = m_pImageObject->GetPalette();
	if( m_pImageObject->GetNumBits() > 8 && pLogPal != NULL && ( bCompleteImage || !bLessThanHalf ) ){
		_pPalette->DeleteObject();
		for( i=0; i<nNumColors; i++ ){
			dwGray = ( (DWORD) pLogPal->palPalEntry[i].peRed * 30 +
				(DWORD) pLogPal->palPalEntry[i].peGreen * 59 +
				(DWORD) pLogPal->palPalEntry[i].peBlue * 11 ) / 100;
			pLogPal->palPalEntry[i].peRed = (unsigned char) ( ( dwRed * dwGray ) / (DWORD) 255 );
			pLogPal->palPalEntry[i].peGreen = (unsigned char) ( ( dwGreen * dwGray ) / (DWORD) 255 );
			pLogPal->palPalEntry[i].peBlue = (unsigned char) ( ( dwBlue * dwGray ) / (DWORD) 255 );
			}
		_pPalette->CreatePalette( pLogPal );
		}

	switch( m_pImageObject->GetNumBits() ){
		case 4:
 			if( bCompleteImage ){
				_pPalette->DeleteObject();
				for( i=0; i<nNumColors; i++ ){
					dwGray = ( (DWORD) pPalette[i].rgbRed * 30 +
						(DWORD) pPalette[i].rgbGreen * 59 +
						(DWORD) pPalette[i].rgbBlue * 11 ) / 100;
					pPalette[i].rgbRed = (unsigned char) ( ( dwRed * dwGray ) / (DWORD) 255 );
					pPalette[i].rgbGreen = (unsigned char) ( ( dwGreen * dwGray ) / (DWORD) 255 );
					pPalette[i].rgbBlue = (unsigned char) ( ( dwBlue * dwGray ) / (DWORD) 255 );
					}
				{
				LOGPALETTE *pLogPal = m_pImageObject->CreateLogPalette( pPalette, nNumColors );
				if( pLogPal != NULL ){
					_pPalette->CreatePalette( pLogPal );
					delete [] pLogPal;
					}
				}
				}
			else{
				unsigned char ucRed, ucGreen, ucBlue, Data;
				for( y=nY1; y<=nY2; y++ ){
					pTemp = pBits;
					pTemp += ( ( ( m_pImageObject->GetHeight() - 1 - y ) * nWidthBytes ) + ( nX1 / 2 ) );
					for( x=nX1; x<=nX2; x++ ){
						Data = *pTemp;
						if( !( x & 1 ) ) Data >>= 4;
						else Data &= 0x0f;
						ucRed = pPalette[Data].rgbRed;
						ucGreen = pPalette[Data].rgbGreen;
						ucBlue = pPalette[Data].rgbBlue;
						ucRed = 255 - ucRed;
						ucGreen = 255 - ucGreen;
						ucBlue = 255 - ucBlue;
						Data = (unsigned char) m_pImageObject->GetNearestIndex( ucRed, ucGreen, ucBlue, pPalette, nNumColors );
						if( !( x & 1 ) ){
							Data <<= 4;
							(*pTemp) &= 0x0f;
							(*pTemp) |= Data;
							}
						else{
							(*pTemp) &= 0xf0;
							(*pTemp) |= Data;
							pTemp++;
							}
						}
					}
				}
			break;
		case 8:
			if( bCompleteImage ){
				_pPalette->DeleteObject();
				for( i=0; i<nNumColors; i++ ){
					dwGray = ( (DWORD) pPalette[i].rgbRed * 30 +
						(DWORD) pPalette[i].rgbGreen * 59 +
						(DWORD) pPalette[i].rgbBlue * 11 ) / 100;
					pPalette[i].rgbRed = (unsigned char) ( ( dwRed * dwGray ) / (DWORD) 255 );
					pPalette[i].rgbGreen = (unsigned char) ( ( dwGreen * dwGray ) / (DWORD) 255 );
					pPalette[i].rgbBlue = (unsigned char) ( ( dwBlue * dwGray ) / (DWORD) 255 );
					}
				{
				LOGPALETTE *pLogPal = m_pImageObject->CreateLogPalette( pPalette, nNumColors );
				if( pLogPal != NULL ){
					_pPalette->CreatePalette( pLogPal );
					delete [] pLogPal;
					}
				}
				}
			else{
				unsigned char ucRed, ucGreen, ucBlue, Data;
				for( y=nY1; y<=nY2; y++ ){
					pTemp = pBits;
					pTemp += ( ( ( m_pImageObject->GetHeight() - 1 - y ) * nWidthBytes ) + nX1 );
					for( x=nX1; x<=nX2; x++ ){
						Data = *pTemp;
						dwGray = ( (DWORD) pPalette[Data].rgbRed * 30 +
							(DWORD) pPalette[Data].rgbGreen * 59 +
							(DWORD) pPalette[Data].rgbBlue * 11 ) / 100;
						ucRed = (unsigned char) ( ( dwRed * dwGray ) / (DWORD) 255 );
						ucGreen = (unsigned char) ( ( dwGreen * dwGray ) / (DWORD) 255 );
						ucBlue = (unsigned char) ( ( dwBlue * dwGray ) / (DWORD) 255 );
						Data = (unsigned char) m_pImageObject->GetNearestIndex( ucRed, ucGreen, ucBlue, pPalette, nNumColors );
						*pTemp++ = Data;
						}
					}
				}
			break;
		case 16:
			for( y=nY1; y<=nY2; y++ ){
				pTemp = pBits;
				pTemp += ( ( ( m_pImageObject->GetHeight() - 1 - y ) * nWidthBytes ) + ( nX1 * 2 ) );
				for( x=nX1; x<=nX2; x++ ){
					}
				}
			break;
		case 24:
			for( y=nY1; y<=nY2; y++ ){
				pTemp = pBits;
				pTemp += ( ( ( m_pImageObject->GetHeight() - 1 - y ) * nWidthBytes ) + ( nX1 * 3 ) );
				for( x=nX1; x<=nX2; x++ ){
					dwGray = ( (DWORD) pTemp[2] * 30 +
						(DWORD) pTemp[1] * 59 +
						(DWORD) pTemp[0] * 11 ) / 100;
					pTemp[2] = (unsigned char) ( ( dwRed * dwGray ) / (DWORD) 255 );
					pTemp[1] = (unsigned char) ( ( dwGreen * dwGray ) / (DWORD) 255 );
					pTemp[0] = (unsigned char) ( ( dwBlue * dwGray ) / (DWORD) 255 );
					pTemp += 3;
					}
				}
			break;
		case 32:
			for( y=nY1; y<=nY2; y++ ){
				pTemp = pBits;
				pTemp += ( ( ( m_pImageObject->GetHeight() - 1 - y ) * nWidthBytes ) + ( nX1 * 4 ) );
				for( x=nX1; x<=nX2; x++ ){
					pTemp[0] = 255 - pTemp[0];
					pTemp[1] = 255 - pTemp[1];
					pTemp[2] = 255 - pTemp[2];
					pTemp[3] = 255 - pTemp[3];
					pTemp += 4;
					}
				}
			break;
		}

	::GlobalUnlock( m_pImageObject->GetDib() );

	m_pImageObject->m_nLastError = IMAGELIB_SUCCESS;

	return( TRUE );
}

void CImagePointProcesses::SetImageObjectClass( CImageObject *pImageObject )
{
	m_pImageObject = pImageObject;
}

int *CImagePointProcesses::CreateHistogram( int nX1, int nY1, int nX2, int nY2, unsigned char *pData, RGBQUAD *pPalette, int nWidthBytes, CImageObject *pImageObject )
{
	if( pImageObject != NULL ) m_pImageObject = pImageObject;
	if( m_pImageObject == NULL ) return( FALSE );

	m_pImageObject->NormalizeCoordinates( &nX1, &nY1, &nX2, &nY2 );

	int *pBuffer = new int [256*4+4];
	if( pBuffer == NULL ) return( NULL );
	memset( pBuffer, 0, ( 256 * 4 + 4 ) * sizeof( int ) );

	DWORD dwGray;
	int x, y;
	unsigned char *pTemp, ucRed, ucGreen, ucBlue;
	int Pixels = 0;
	int nHeight = m_pImageObject->GetHeight();

	switch( m_pImageObject->GetNumBits() ){
		case 1:
			break;
		case 4:
			break;
		case 8:

			for( y=0; y<nHeight; y++ ){
				pTemp = pData;
				pTemp += ( ( nHeight - 1 - y ) * nWidthBytes );
				pTemp += nX1;
				for( x=nX1; x<=nX2; x++ ){
					ucRed = pPalette[pTemp[x]].rgbRed;
					ucGreen = pPalette[pTemp[x]].rgbGreen;
					ucBlue = pPalette[pTemp[x]].rgbBlue;
					dwGray = ( (DWORD) ucRed * 30 +
						(DWORD) ucGreen * 59 +
						(DWORD) ucBlue * 11 ) / 100;
					dwGray &= 0x000000ff;
					pBuffer[dwGray]++;
					pBuffer[256+ucRed]++;
					pBuffer[512+ucBlue]++;
					pBuffer[768+ucGreen]++;
					Pixels++;
					}
				}
			break;
		case 16:
			for( y=0; y<nHeight; y++ ){
				pTemp = pData;
				pTemp += ( ( nHeight - 1 - y ) * nWidthBytes );
				pTemp += ( nX1 * 2 );
				for( x=nX1; x<=nX2; x++ ){
					GETRGB555( ucRed, ucGreen, ucBlue, pTemp );
					dwGray = ( (DWORD) ucRed * 30 +
						(DWORD) ucGreen * 59 +
						(DWORD) ucBlue * 11 ) / 100;
					dwGray &= 0x000000ff;
					pBuffer[dwGray]++;
					pBuffer[256+pTemp[x*3]]++;
					pBuffer[512+pTemp[x*3+1]]++;
					pBuffer[768+pTemp[x*3+2]]++;
					Pixels++;
					pTemp += 2;
					}
				}
			break;
		case 24:
			for( y=0; y<nHeight; y++ ){
				pTemp = pData;
				pTemp += ( ( nHeight - 1 - y ) * nWidthBytes );
				pTemp += ( nX1 * 3 );
				for( x=nX1; x<=nX2; x++ ){
					dwGray = ( (DWORD) pTemp[x*3+2] * 30 +
						(DWORD) pTemp[x*3+1] * 59 +
						(DWORD) pTemp[x*3] * 11 ) / 100;
					dwGray &= 0x000000ff;
					pBuffer[dwGray]++;
					pBuffer[256+pTemp[x*3]]++;
					pBuffer[512+pTemp[x*3+1]]++;
					pBuffer[768+pTemp[x*3+2]]++;
					Pixels++;
					}
				}
			break;
		case 32:
			for( y=0; y<nHeight; y++ ){
				pTemp = pData;
				pTemp += ( ( nHeight - 1 - y ) * nWidthBytes );
				pTemp += ( nX1 * 4 );
				for( x=nX1; x<=nX2; x++ ){
					GETRGB888( ucRed, ucGreen, ucBlue, pTemp );
					dwGray = ( (DWORD) ucRed * 30 +
						(DWORD) ucGreen * 59 +
						(DWORD) ucBlue * 11 ) / 100;
					dwGray &= 0x000000ff;
					pBuffer[dwGray]++;
					pBuffer[256+pTemp[x*3]]++;
					pBuffer[512+pTemp[x*3+1]]++;
					pBuffer[768+pTemp[x*3+2]]++;
					Pixels++;
					pTemp += 4;
					}
				}
			break;
		}

	pBuffer[0] = pBuffer[256] = pBuffer[512] = pBuffer[768] = 0;
	int Highest = 0, i, j;
	for( i=0; i<256*4; i++ ){
		if( pBuffer[i] > Highest ) Highest = pBuffer[i];
		}
	for( j=0; j<4; j++ ){
		for( i=0; i<256; i++ ) pBuffer[j*256+i] = ( pBuffer[j*256+i] * 255 ) / Highest;
		}
	Highest = 0;
	for( i=0; i<256*4; i++ ){
		if( pBuffer[i] > Highest ) Highest = pBuffer[i];
		}
	pBuffer[256*4] = Highest;
	pBuffer[256*4+1] = Highest / 25;

	for( i=0; i<256; i++ ){
		if( pBuffer[i] >= pBuffer[256*4+1] ) break;
		}
	pBuffer[256*4+2] = i;
	for( i=255; i>=0; i-- ){
		if( pBuffer[i] >= pBuffer[256*4+1] ) break;
		}
	pBuffer[256*4+3] = i;
	if( pBuffer[256*4+2] > 255 ) pBuffer[256*4+2] = 255;
	if( pBuffer[256*4+3] < 0 ) pBuffer[256*4+3] = 0;

	return( pBuffer );
}

