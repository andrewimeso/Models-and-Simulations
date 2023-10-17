// ImageObject.cpp

#include "stdafx.h"
#include "ImageObject.h"

char *CImageObject::szExtensions[] = { ".BMP", ".GIF", ".PCX", ".TGA", ".JPG", ".TIF", "" };

CImageObject::CImageObject()
{

	InitVars();

}

CImageObject::CImageObject( const char *pszFilename, CDC *pDC, int nX, int nY )
{

	InitVars();
	Load( pszFilename, pDC, nX, nY );

}

CImageObject::~CImageObject()
{

	if( m_hDib ) ::GlobalFree( m_hDib );
	if( m_pszFilename != NULL ) delete [] m_pszFilename;

}

void CImageObject::InitVars( BOOL bFlag )
{

	m_nWidth = m_nHeight = m_nBits = m_nColors = m_nImageType = 0;
	m_nX = m_nY = 0;
	m_nLastError = 0;
	m_hDib = NULL;
	m_nPaletteCreationType = FIXED_PALETTE;
	if( !bFlag ){
		m_pszFilename = NULL;
		m_pLogPal = NULL;
		}
	m_nQuality = 50;

}

BOOL CImageObject::Load( const char *pszFilename, CDC *pDC, int nX, int nY )
{

	m_nImageType = FileType( pszFilename );
	if( m_nImageType == 0 ){
		m_nLastError = IMAGELIB_UNSUPPORTED_FILETYPE;
		return( FALSE );
		}

	KillImage();

	m_pszFilename = new char [strlen(pszFilename)+1];
	if( m_pszFilename != NULL ) strcpy( m_pszFilename, pszFilename );

	switch( m_nImageType ){
		case IMAGETYPE_BMP:
			m_hDib = ::LoadBMP( pszFilename );
			if( m_hDib == NULL ){
				m_nLastError = ::GetLastPicLibError();
				return( FALSE );
				}
			break;
		case IMAGETYPE_GIF:
			m_hDib = ::LoadGIF( pszFilename );
			if( m_hDib == NULL ){
				m_nLastError = ::GetLastPicLibError();
				return( FALSE );
				}
			break;
		case IMAGETYPE_JPG:
			m_hDib = ::LoadJPG( pszFilename );
			if( m_hDib == NULL ){
				m_nLastError = ::GetLastPicLibError();
				return( FALSE );
				}
			break;
		case IMAGETYPE_PCX:
			m_hDib = ::LoadPCX( pszFilename );
			if( m_hDib == NULL ){
				m_nLastError = ::GetLastPicLibError();
				return( FALSE );
				}
			break;
		case IMAGETYPE_TGA:
			m_hDib = ::LoadTGA( pszFilename );
			if( m_hDib == NULL ){
				m_nLastError = ::GetLastPicLibError();
				return( FALSE );
				}
			break;
		case IMAGETYPE_TIF:
			m_hDib = ::LoadTIF( pszFilename );
			Beep(880,500);
			if( m_hDib == NULL ){
				m_nLastError = ::GetLastPicLibError();
			Beep(440,500);
				return( FALSE );
				}
			break;
		}

	ProcessImageHeader();
	ProcessPalette();

	if( pDC != NULL ) Draw( pDC, nX, nY );

	return( TRUE );

}

BOOL CImageObject::GetImageInfo( const char *pszFilename, int *pnWidth,
	int *pnHeight, int *pnPlanes, int *pnBitsPerPixel, int *pnNumColors )
{
	int nImageType;

	nImageType = FileType( pszFilename );
	if( nImageType == 0 ){
		m_nLastError = IMAGELIB_UNSUPPORTED_FILETYPE;
		return( FALSE );
		}

	switch( nImageType ){
		case IMAGETYPE_BMP:
			return( ::GetBMPInfo( pszFilename, pnWidth, pnHeight, pnPlanes, pnBitsPerPixel, pnNumColors ) );
		case IMAGETYPE_GIF:
			return( ::GetGIFInfo( pszFilename, pnWidth, pnHeight, pnPlanes, pnBitsPerPixel, pnNumColors ) );
		case IMAGETYPE_JPG:
			return( ::GetJPGInfo( pszFilename, pnWidth, pnHeight, pnPlanes, pnBitsPerPixel, pnNumColors ) );
		case IMAGETYPE_PCX:
			return( ::GetPCXInfo( pszFilename, pnWidth, pnHeight, pnPlanes, pnBitsPerPixel, pnNumColors ) );
		case IMAGETYPE_TGA:
			return( ::GetTGAInfo( pszFilename, pnWidth, pnHeight, pnPlanes, pnBitsPerPixel, pnNumColors ) );
		case IMAGETYPE_TIF:
			return( ::GetTIFInfo( pszFilename, pnWidth, pnHeight, pnPlanes, pnBitsPerPixel, pnNumColors ) );
		}

	return( FALSE );

}

int CImageObject::GetLastError( void )
{

	return( m_nLastError );

}

BOOL CImageObject::Save( const char *pszFilename, int nType )
{

	if( nType == -1 ) nType = ExtensionIndex( pszFilename );

	if( nType < IMAGETYPE_FIRSTTYPE || nType > IMAGETYPE_LASTTYPE ) return( FALSE );
	m_nImageType = nType;
	delete [] m_pszFilename;
	m_pszFilename = new char [strlen(pszFilename)+1];
	if( m_pszFilename != NULL ) strcpy( m_pszFilename, pszFilename );

	switch( m_nImageType ){
		case IMAGETYPE_BMP:
			return( ::SaveBMP( pszFilename, m_hDib ) );
			break;
		case IMAGETYPE_GIF:
			return( ::SaveGIF( pszFilename, m_hDib ) );
			break;
		case IMAGETYPE_JPG:
			return( ::SaveJPG( pszFilename, m_hDib, m_nQuality ) );
			break;
		case IMAGETYPE_PCX:
			return( ::SavePCX( pszFilename, m_hDib ) );
			break;
		case IMAGETYPE_TGA:
			return( ::SaveTGA( pszFilename, m_hDib ) );
			break;
		case IMAGETYPE_TIF:
			return( ::SaveTIF( pszFilename, m_hDib ) );
			break;
		}

	return( TRUE );

}

int CImageObject::ExtensionIndex( const char *pszFilename )
{
	int Index = 0;
	char *pszExtension;

	pszExtension = (char *) &pszFilename[strlen(pszFilename)-4];

	while( szExtensions[Index][0] ){
		if( !stricmp( pszExtension, szExtensions[Index] ) ) return( Index + 1 );
		Index++;
		}

	return( -1 );

}

int CImageObject::GetWidth( void )
{

	return( m_nWidth );

}

int CImageObject::GetHeight( void )
{

	return( m_nHeight );

}

int CImageObject::GetNumBits( void )
{

	return( m_nBits );

}

int CImageObject::GetNumColors( void )
{

	return( m_nColors );

}

BOOL CImageObject::GetPaletteData( RGBQUAD *pRGBPalette )
{

	m_nLastError = IMAGELIB_HDIB_NULL;
	if( m_hDib == NULL ) return( FALSE );

	char *pTemp;
	pTemp = (char *) ::GlobalLock( m_hDib );
	m_nLastError = IMAGELIB_MEMORY_LOCK_ERROR;
	if( pTemp == NULL ) return( FALSE );

	memcpy( pRGBPalette, &pTemp[sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)], m_nPaletteBytes );

	m_nLastError = IMAGELIB_SUCCESS;
	::GlobalUnlock( m_hDib );

	return( TRUE );

}

RGBQUAD *CImageObject::GetPaletteData( void )
{
	m_nLastError = IMAGELIB_HDIB_NULL;
	if( m_hDib == NULL ) return( NULL );

	m_nLastError = IMAGELIB_NO_PALETTE_FOR_HIGH_COLOR;
	if( m_nColors <= 0 || m_nColors > 256 ) return( NULL );

	RGBQUAD *pRGBPalette;
	pRGBPalette = new RGBQUAD [m_nColors];
	if( pRGBPalette == NULL ){
		m_nLastError = IMAGELIB_MEMORY_ALLOCATION_ERROR;
		return( NULL );
		}

	char *pTemp;
	pTemp = (char *) ::GlobalLock( m_hDib );
	m_nLastError = IMAGELIB_MEMORY_LOCK_ERROR;
	if( pTemp == NULL ){
		delete [] pRGBPalette;
		return( NULL );
		}

	memcpy( pRGBPalette, &pTemp[sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)], m_nColors * sizeof( RGBQUAD ) );

	m_nLastError = IMAGELIB_SUCCESS;
	::GlobalUnlock( m_hDib );

	return( pRGBPalette );

}

int CImageObject::GetImageType( const char *pFilename )
{

	return( ::FileType( pFilename ) );

}

int CImageObject::GetImageType( void )
{

	return( m_nImageType );

}

BOOL CImageObject::Draw( CDC *pDC, int nX, int nY )
{
	if( nX != -1 ) m_nX = nX;
	if( nY != -1 ) m_nY = nY;

	m_nLastError = IMAGELIB_HDIB_NULL;
	if( m_hDib == NULL ) return( FALSE );

	char *pTemp;
	pTemp = (char *) ::GlobalLock( m_hDib );
	m_nLastError = IMAGELIB_MEMORY_LOCK_ERROR;
	if( pTemp == NULL ) return( NULL );
	BITMAPINFOHEADER *pBIH;
	pBIH = (BITMAPINFOHEADER *) &pTemp[sizeof(BITMAPFILEHEADER)];

	int nRet = ::StretchDIBits( pDC->m_hDC, m_nX, m_nY, m_nWidth, m_nHeight, 0, 0, m_nWidth, m_nHeight, (const void FAR *) &pTemp[sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+m_nPaletteBytes], (BITMAPINFO *) pBIH, DIB_RGB_COLORS, SRCCOPY );

	::GlobalUnlock( m_hDib );
	m_nLastError = IMAGELIB_STRETCHDIBITS_ERROR;
	if( nRet != m_nHeight ) return( FALSE );
	m_nLastError = IMAGELIB_SUCCESS;

	return( TRUE );

}

LOGPALETTE *CImageObject::CreateLogPalette( RGBQUAD *pPalette, int nNumColors )
{
	LOGPALETTE *pLogPal;
	int i;

	if( pPalette == NULL ) return( NULL );

	pLogPal = (LOGPALETTE *) new char [sizeof(LOGPALETTE)+nNumColors*sizeof(PALETTEENTRY)];
	if( pLogPal == NULL ) return( NULL );

	pLogPal->palVersion = 0x300;
	pLogPal->palNumEntries = (unsigned short) nNumColors;
	for( i=0; i<nNumColors; i++ ){
		pLogPal->palPalEntry[i].peRed = pPalette[i].rgbRed;
		pLogPal->palPalEntry[i].peGreen = pPalette[i].rgbGreen;
		pLogPal->palPalEntry[i].peBlue = pPalette[i].rgbBlue;
		pLogPal->palPalEntry[i].peFlags = 0;
		}

	return( pLogPal );

}

void CImageObject::ProcessImageHeader( void )
{

	m_nLastError = IMAGELIB_HDIB_NULL;
	if( m_hDib == NULL ) return;

	char *pTemp;
	BITMAPINFOHEADER *pBIH;
	pTemp = (char *) ::GlobalLock( m_hDib );
	m_nLastError = IMAGELIB_MEMORY_LOCK_ERROR;
	if( pTemp == NULL ) return;

	pBIH = (BITMAPINFOHEADER *) &pTemp[sizeof(BITMAPFILEHEADER)];
	m_nWidth = pBIH->biWidth;
	m_nHeight = pBIH->biHeight;
	m_nPlanes = pBIH->biPlanes;
	m_nBits = pBIH->biBitCount;
	m_nColors = 1 << m_nBits;
	if( m_nPlanes > 1 ) m_nColors <<= ( m_nPlanes - 1 );
	if( m_nBits >= 16 ) m_nColors = 0;

	::GlobalUnlock( m_hDib );

	m_nLastError = IMAGELIB_SUCCESS;

}

void CImageObject::ProcessPalette( void )
{

	m_nLastError = IMAGELIB_HDIB_NULL;
	if( m_hDib == NULL ) return;

	CWindowDC WindowDC( NULL );
	m_nScreenPlanes = WindowDC.GetDeviceCaps( PLANES );
	m_nScreenBits = WindowDC.GetDeviceCaps( BITSPIXEL );

	m_nPaletteBytes = 0;
	m_Palette.DeleteObject();

	if( m_nBits <= 8 ) m_nPaletteBytes = m_nColors * sizeof( RGBQUAD );
	if( m_nScreenBits >= 16 ) return;

	char *pTemp;
	pTemp = (char *) ::GlobalLock( m_hDib );
	m_nLastError = IMAGELIB_MEMORY_LOCK_ERROR;
	if( pTemp == NULL ) return;

	if( m_nBits <= 8 ){
		RGBQUAD *pRGBPalette;
		pRGBPalette = (RGBQUAD *) &pTemp[sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)];
		LOGPALETTE *pPalette;
		pPalette = CreateLogPalette( pRGBPalette, m_nColors );
		if( pPalette == NULL ){
			m_nLastError = IMAGELIB_LOGICAL_PALETTE_CREATION_ERROR;
			goto ProcessPaletteExit;
			}
		m_Palette.CreatePalette( pPalette );
		delete [] pPalette;
		}

	m_nLastError = IMAGELIB_SUCCESS;

ProcessPaletteExit:
	::GlobalUnlock( m_hDib );

}

void CImageObject::KillImage( void )
{

	if( m_hDib ) ::GlobalFree( m_hDib );
	m_hDib = NULL;

	if( m_pLogPal != NULL ) delete [] m_pLogPal;
	m_pLogPal = NULL;

	if( m_pszFilename != NULL ) delete [] m_pszFilename;
	m_pszFilename = NULL;

	m_Palette.DeleteObject();

}

BOOL CImageObject::SetPalette( CDC *pDC )
{

	m_nLastError = IMAGELIB_HDIB_NULL;
	if( m_hDib == NULL ) return( FALSE );

	pDC->SelectPalette( &m_Palette, FALSE );
	pDC->RealizePalette();

	m_nLastError = IMAGELIB_SUCCESS;

	return( TRUE );

}

BOOL CImageObject::IsLoaded( void )
{

	return( m_hDib != NULL );

}

void *CImageObject::GetDIBPointer( int *nWidthBytes, int nNewBits, int *nNewWidthBytes, int nNewWidth )
{

	m_nLastError = IMAGELIB_HDIB_NULL;
	if( m_hDib == NULL ) return( NULL );

	void *pTemp;
	pTemp = ::GlobalLock( m_hDib );
	m_nLastError = IMAGELIB_MEMORY_LOCK_ERROR;
	if( pTemp == NULL ) return( NULL );

	if( nWidthBytes != NULL ) *nWidthBytes = WidthBytes( m_nBits, m_nWidth );

	if( nNewWidthBytes != NULL ){
		if( nNewWidth == -1 ) nNewWidth = m_nWidth;
		*nNewWidthBytes = WidthBytes( nNewBits, nNewWidth );
		}

	return( pTemp );

}

int CImageObject::WidthBytes( int nBits, int nWidth )
{
	int nWidthBytes;

	nWidthBytes = nWidth;
	if( nBits == 1 ) nWidthBytes = ( nWidth + 7 ) / 8;
	else if( nBits == 4 ) nWidthBytes = ( nWidth + 1 ) / 2;
	else if( nBits == 16 ) nWidthBytes = nWidth * 2;
	else if( nBits == 24 ) nWidthBytes = nWidth * 3;
	else if( nBits == 32 ) nWidthBytes = nWidth * 4;
	while( ( nWidthBytes & 3 ) != 0 ) nWidthBytes++;

	return( nWidthBytes );

}

BOOL CImageObject::Crop( int nX1, int nY1, int nX2, int nY2 )
{

	NormalizeCoordinates( &nX1, &nY1, &nX2, &nY2 );

	if( nX2 - nX1 + 1 == m_nWidth && nY2 - nY1 + 1 == m_nHeight ) return( TRUE );

	unsigned char *pOldBuffer, *pNewBuffer;
	HGLOBAL hNewMemory;
	DWORD dwNewMemorySize;
	BITMAPFILEHEADER *pOldBFH, *pNewBFH;
	BITMAPINFOHEADER *pOldBIH, *pNewBIH;
	RGBQUAD *pOldPalette, *pNewPalette;
	unsigned char *pOldBits, *pNewBits, *pOldTemp, *pNewTemp;
	int nOldWidthBytes, nNewWidthBytes;
	int nNumColors, y, nPixelBytes;

	pOldBuffer = (unsigned char *) GetDIBPointer( &nOldWidthBytes, m_nBits, &nNewWidthBytes, nX2 - nX1 + 1 );
	if( pOldBuffer == NULL ) return( FALSE );

	pOldBFH = (BITMAPFILEHEADER *) pOldBuffer;
	pOldBIH = (BITMAPINFOHEADER *) &pOldBuffer[sizeof(BITMAPFILEHEADER)];
	nNumColors = m_nColors;
	pOldPalette = (RGBQUAD *) &pOldBuffer[sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)];
	pOldBits = (unsigned char *) &pOldBuffer[sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+nNumColors*sizeof(RGBQUAD)];

	dwNewMemorySize = (DWORD) nNumColors * sizeof( RGBQUAD ) + sizeof( BITMAPFILEHEADER ) + sizeof( BITMAPINFOHEADER ) + nNewWidthBytes * ( nY2 - nY1 + 1 );
	m_nLastError = IMAGELIB_MEMORY_ALLOCATION_ERROR;
	hNewMemory = ::GlobalAlloc( GMEM_MOVEABLE | GMEM_ZEROINIT, dwNewMemorySize );
	if( hNewMemory == NULL ){
		::GlobalUnlock( m_hDib );
		return( FALSE );
		}

	m_nLastError = IMAGELIB_MEMORY_LOCK_ERROR;
	pNewBuffer = (unsigned char *) ::GlobalLock( hNewMemory );
	if( pNewBuffer == NULL ){
		::GlobalFree( hNewMemory );
		::GlobalUnlock( m_hDib );
		return( FALSE );
		}

	pNewBFH = (BITMAPFILEHEADER *) pNewBuffer;
	pNewBIH = (BITMAPINFOHEADER *) &pNewBuffer[sizeof(BITMAPFILEHEADER)];
	pNewPalette = (RGBQUAD *) &pNewBuffer[sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)];
	pNewBits = (unsigned char *) &pNewBuffer[sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+nNumColors*sizeof(RGBQUAD)];

	*pNewBFH = *pOldBFH;
	*pNewBIH = *pOldBIH;
	pNewBIH->biSizeImage = 0;
	pNewBIH->biWidth = nX2 - nX1 + 1;
	pNewBIH->biHeight = nY2 - nY1 + 1;
	if( nNumColors != 0 ) memcpy( pNewPalette, pOldPalette, nNumColors * sizeof( RGBQUAD ) );

	nPixelBytes = 1;
	if( pOldBIH->biBitCount == 16 ) nPixelBytes = 2;
	else if( pOldBIH->biBitCount == 24 ) nPixelBytes = 3;
	else if( pOldBIH->biBitCount == 32 ) nPixelBytes = 4;

	for( y=0; y<pNewBIH->biHeight; y++ ){
		pOldTemp = pOldBits;
		pOldTemp += ( ( pOldBIH->biHeight - 1 - ( y + nY1 ) ) * nOldWidthBytes );
		pOldTemp += ( nPixelBytes *	nX1 );
		pNewTemp = pNewBits;
		pNewTemp += ( ( pNewBIH->biHeight - 1 - y ) * nNewWidthBytes );
		memcpy( pNewTemp, pOldTemp, nNewWidthBytes );
		pNewBIH->biSizeImage += nNewWidthBytes;
		}

	m_nWidth = pNewBIH->biWidth;
	m_nHeight = pNewBIH->biHeight;
	::GlobalUnlock( m_hDib );
	::GlobalUnlock( hNewMemory );
	::GlobalFree( m_hDib );
	m_hDib = hNewMemory;

	m_nLastError = IMAGELIB_SUCCESS;

	return( TRUE );

}

void CImageObject::NormalizeCoordinates( int *nX1, int *nY1, int *nX2, int *nY2, BOOL *bCompleteImage, BOOL *bLessThanHalf )
{

	if( *nX1 == -1 ) *nX1 = 0;
	if( *nY1 == -1 ) *nY1 = 0;
	if( *nX2 == -1 ) *nX2 = m_nWidth - 1;
	if( *nY2 == -1 ) *nY2 = m_nHeight - 1;

	if( *nX1 > *nX2 ){
		int nTemp;
		nTemp = *nX1;
		*nX1 = *nX2;
		*nX2 = nTemp;		
		}

	if( *nY1 > *nY2 ){
		int nTemp;
		nTemp = *nY1;
		*nY1 = *nY2;
		*nY2 = nTemp;		
		}

	if( *nX1 < 0 ) *nX1 = 0;
	if( *nX2 > m_nWidth - 1 ) *nX2 = m_nWidth - 1;

	if( *nX2 < 0 ) *nX2 = 0;
	if( *nX2 > m_nWidth - 1 ) *nX2 = m_nWidth - 1;

	if( *nY1 < 0 ) *nY1 = 0;
	if( *nY1 > m_nHeight - 1 ) *nY1 = m_nHeight - 1;

	if( *nY2 < 0 ) *nY2 = 0;
	if( *nY2 > m_nHeight - 1 ) *nY2 = m_nHeight - 1;

	if( bCompleteImage != NULL ) *bCompleteImage = TRUE;
	if( bLessThanHalf != NULL ) *bLessThanHalf = FALSE;
	if( *nX1 > 0 || *nY1 > 0 || *nX2 < m_nWidth - 1 || *nY2 < m_nHeight - 1 ){
		if( bCompleteImage != NULL ) *bCompleteImage = FALSE;
		DWORD dwTotalPixels, dwThesePixels;
		dwTotalPixels = (DWORD) m_nWidth * m_nHeight;
		dwThesePixels = (DWORD) ( (*nX2) - (*nX1) + 1 ) * (DWORD) ( (*nY2) - (*nY1) + 1 );
		if( bLessThanHalf != NULL && dwThesePixels <= dwTotalPixels / 2 ) *bLessThanHalf = TRUE;
		}

}

BOOL CImageObject::Stretch( int nNewWidth, int nNewHeight )
{

	if( nNewWidth == m_nWidth && nNewHeight == m_nHeight ) return( TRUE );

	unsigned char *pOldBuffer, *pNewBuffer, *pOldTemp, *pNewTemp;
	HGLOBAL hNewMemory;
	DWORD dwNewMemorySize;
	BITMAPFILEHEADER *pOldBFH, *pNewBFH;
	BITMAPINFOHEADER *pOldBIH, *pNewBIH;
	RGBQUAD *pOldPalette, *pNewPalette;
	unsigned char *pOldBits, *pNewBits;
	int nOldWidthBytes, nNewWidthBytes;
	int nNumColors;
	int x, y, xx, yy;
	unsigned char Data;

	pOldBuffer = (unsigned char *) GetDIBPointer( &nOldWidthBytes, m_nBits, &nNewWidthBytes, nNewWidth );
	if( pOldBuffer == NULL ) return( FALSE );

	pOldBFH = (BITMAPFILEHEADER *) pOldBuffer;
	pOldBIH = (BITMAPINFOHEADER *) &pOldBuffer[sizeof(BITMAPFILEHEADER)];
	nNumColors = m_nColors;
	pOldPalette = (RGBQUAD *) &pOldBuffer[sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)];
	pOldBits = (unsigned char *) &pOldBuffer[sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+nNumColors*sizeof(RGBQUAD)];

	dwNewMemorySize = (DWORD) nNumColors * sizeof( RGBQUAD ) + sizeof( BITMAPFILEHEADER ) + sizeof( BITMAPINFOHEADER ) + nNewWidthBytes * nNewHeight;
	m_nLastError = IMAGELIB_MEMORY_ALLOCATION_ERROR;
	hNewMemory = ::GlobalAlloc( GMEM_MOVEABLE | GMEM_ZEROINIT, dwNewMemorySize );
	if( hNewMemory == NULL ){
		::GlobalUnlock( m_hDib );
		return( FALSE );
		}

	m_nLastError = IMAGELIB_MEMORY_LOCK_ERROR;
	pNewBuffer = (unsigned char *) ::GlobalLock( hNewMemory );
	if( pNewBuffer == NULL ){
		::GlobalFree( hNewMemory );
		::GlobalUnlock( m_hDib );
		return( FALSE );
		}

	pNewBFH = (BITMAPFILEHEADER *) pNewBuffer;
	pNewBIH = (BITMAPINFOHEADER *) &pNewBuffer[sizeof(BITMAPFILEHEADER)];
	pNewPalette = (RGBQUAD *) &pNewBuffer[sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)];
	pNewBits = (unsigned char *) &pNewBuffer[sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+nNumColors*sizeof(RGBQUAD)];

	*pNewBFH = *pOldBFH;
	*pNewBIH = *pOldBIH;
	pNewBIH->biSizeImage = nNewWidthBytes * nNewHeight;
	pNewBIH->biWidth = nNewWidth;
	pNewBIH->biHeight = nNewHeight;
	if( nNumColors != 0 ) memcpy( pNewPalette, pOldPalette, nNumColors * sizeof( RGBQUAD ) );

	m_nWidth = nNewWidth;
	m_nHeight = nNewHeight;

	switch( m_nBits ){
		case 1:
			for( y=0; y<nNewHeight; y++ ){
				yy = ( pOldBIH->biHeight * y ) / nNewHeight;
				pOldTemp = pOldBits;
				pOldTemp += ( ( pOldBIH->biHeight - 1 - yy ) * nOldWidthBytes );
				pNewTemp = pNewBits;
				pNewTemp += ( ( nNewHeight - 1 - y ) * nNewWidthBytes );
				for( x=0; x<nNewWidth; x++ ){
					xx = ( pOldBIH->biWidth * x ) / nNewWidth;
					Data = pOldTemp[xx/8];
					Data >>= ( 7 - ( xx & 7 ) );
					Data &= 0x01;
					Data <<= ( 7 - ( x & 7 ) );
					pNewTemp[x/8] |= Data;
					}
				}
			break;
		case 4:
			for( y=0; y<nNewHeight; y++ ){
				yy = ( pOldBIH->biHeight * y ) / nNewHeight;
				pOldTemp = pOldBits;
				pOldTemp += ( ( pOldBIH->biHeight - 1 - yy ) * nOldWidthBytes );
				pNewTemp = pNewBits;
				pNewTemp += ( ( nNewHeight - 1 - y ) * nNewWidthBytes );
				for( x=0; x<nNewWidth; x++ ){
					xx = ( pOldBIH->biWidth * x ) / nNewWidth;
					Data = pOldTemp[xx/2];
					if( !( xx & 1 ) ) Data >>= 4;
					else Data &= 0x0f;
					if( !( x & 1 ) ) Data <<= 4;
					pNewTemp[x/2] |= Data;
					}
				}
			break;
		case 8:
			for( y=0; y<nNewHeight; y++ ){
				yy = ( pOldBIH->biHeight * y ) / nNewHeight;
				pOldTemp = pOldBits;
				pOldTemp += ( ( pOldBIH->biHeight - 1 - yy ) * nOldWidthBytes );
				pNewTemp = pNewBits;
				pNewTemp += ( ( nNewHeight - 1 - y ) * nNewWidthBytes );
				for( x=0; x<nNewWidth; x++ ){
					xx = ( pOldBIH->biWidth * x ) / nNewWidth;
					pNewTemp[x] = pOldTemp[xx];
					}
				}
			break;
		case 16:
			for( y=0; y<nNewHeight; y++ ){
				yy = ( pOldBIH->biHeight * y ) / nNewHeight;
				pOldTemp = pOldBits;
				pOldTemp += ( ( pOldBIH->biHeight - 1 - yy ) * nOldWidthBytes );
				pNewTemp = pNewBits;
				pNewTemp += ( ( nNewHeight - 1 - y ) * nNewWidthBytes );
				for( x=0; x<nNewWidth; x++ ){
					xx = ( pOldBIH->biWidth * x ) / nNewWidth;
					pNewTemp[x*2] = pOldTemp[xx*2];
					pNewTemp[x*2+1] = pOldTemp[xx*2+1];
					}
				}
			break;
		case 24:
			for( y=0; y<nNewHeight; y++ ){
				yy = ( pOldBIH->biHeight * y ) / nNewHeight;
				pOldTemp = pOldBits;
				pOldTemp += ( ( pOldBIH->biHeight - 1 - yy ) * nOldWidthBytes );
				pNewTemp = pNewBits;
				pNewTemp += ( ( nNewHeight - 1 - y ) * nNewWidthBytes );
				for( x=0; x<nNewWidth; x++ ){
					xx = ( pOldBIH->biWidth * x ) / nNewWidth;
					pNewTemp[x*3] = pOldTemp[xx*3];
					pNewTemp[x*3+1] = pOldTemp[xx*3+1];
					pNewTemp[x*3+2] = pOldTemp[xx*3+2];
					}
				}
			break;
		case 32:
			for( y=0; y<nNewHeight; y++ ){
				yy = ( pOldBIH->biHeight * y ) / nNewHeight;
				pOldTemp = pOldBits;
				pOldTemp += ( ( pOldBIH->biHeight - 1 - yy ) * nOldWidthBytes );
				pNewTemp = pNewBits;
				pNewTemp += ( ( nNewHeight - 1 - y ) * nNewWidthBytes );
				for( x=0; x<nNewWidth; x++ ){
					xx = ( pOldBIH->biWidth * x ) / nNewWidth;
					pNewTemp[x*4] = pOldTemp[xx*4];
					pNewTemp[x*4+1] = pOldTemp[xx*4+1];
					pNewTemp[x*4+2] = pOldTemp[xx*4+2];
					pNewTemp[x*4+3] = pOldTemp[xx*4+3];
					}
				}
			break;
		}

	::GlobalUnlock( m_hDib );
	::GlobalUnlock( hNewMemory );
	::GlobalFree( m_hDib );
	m_hDib = hNewMemory;

	m_nLastError = IMAGELIB_SUCCESS;

	return( TRUE );

}

void CImageObject::operator= (const CImageObject &ImageObject)
{

	KillImage();

	m_nWidth = ImageObject.m_nWidth;
	m_nHeight = ImageObject.m_nHeight;
	m_nPlanes = ImageObject.m_nPlanes;
	m_nBits = ImageObject.m_nBits;
	m_nColors = ImageObject.m_nColors;
	m_nImageType = ImageObject.m_nImageType;
	m_nX = ImageObject.m_nX;
	m_nY = ImageObject.m_nY;
	m_nLastError = ImageObject.m_nLastError;
	m_nScreenPlanes = ImageObject.m_nScreenPlanes;
	m_nScreenBits = ImageObject.m_nScreenBits;
	m_nPaletteBytes = ImageObject.m_nPaletteBytes;
	m_nQuality = ImageObject.m_nQuality;
	m_nPaletteCreationType = ImageObject.m_nPaletteCreationType;

	int nNumColors = m_nColors;
	int nWidthBytes = WidthBytes( m_nBits, m_nWidth );

	if( ImageObject.m_hDib != NULL ){

		DWORD dwSize = ::GlobalSize( ImageObject.m_hDib );

		char *pData = (char *) ::GlobalLock( ImageObject.m_hDib );
		if( pData != NULL ){

			HGLOBAL hGlobal = ::GlobalAlloc( GMEM_MOVEABLE | GMEM_ZEROINIT, dwSize );
			if( hGlobal != NULL ){

				char *pDestData = (char *) ::GlobalLock( hGlobal );
				if( pDestData != NULL ){

					memcpy( pDestData, pData, dwSize );

					if( nNumColors != 0 ) CreatePaletteFromDIB( (RGBQUAD *) &pData[sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)], nNumColors );

					else if( ImageObject.m_pLogPal != NULL ){
						m_pLogPal = (LOGPALETTE *) new char [sizeof(LOGPALETTE)+ImageObject.m_pLogPal->palNumEntries*sizeof(PALETTEENTRY)];
						if( m_pLogPal != NULL ){
							for( int i=0; i<ImageObject.m_pLogPal->palNumEntries; i++ ) m_pLogPal[i] = ImageObject.m_pLogPal[i];
							m_Palette.CreatePalette( m_pLogPal );
							}
						}

					::GlobalUnlock( hGlobal );
					m_hDib = hGlobal;
					}
				else ::GlobalFree( hGlobal );
				}
			::GlobalUnlock( ImageObject.m_hDib );
			}
		}

	if( ImageObject.m_pszFilename != NULL ){
		m_pszFilename = new char [strlen(ImageObject.m_pszFilename)+1];
		strcpy( m_pszFilename, ImageObject.m_pszFilename );
		}

}

void CImageObject::CreatePaletteFromDIB( RGBQUAD *pRGBPalette, int nNumColors )
{

	if( pRGBPalette != NULL ){
		LOGPALETTE *pPalette;
		pPalette = CreateLogPalette( pRGBPalette, nNumColors );
		if( pPalette != NULL ){
			m_Palette.CreatePalette( pPalette );
			delete [] pPalette;
			}
		else m_nLastError = IMAGELIB_LOGICAL_PALETTE_CREATION_ERROR;
		}

}

int CImageObject::GetNearestIndex( unsigned char ucRed, unsigned char ucGreen, unsigned char ucBlue, RGBQUAD *pRGBPalette, int nNumColors )
{
	int i, Index = 0;
	DWORD NewDiff, Diff = 100000L;
	DWORD Red, Green, Blue;

	for( i=0; i<nNumColors; i++ ){
		if( ucRed > pRGBPalette[i].rgbRed ) Red = (DWORD) ( pRGBPalette[i].rgbRed - ucRed );
		else Red = (DWORD) ( ucRed - pRGBPalette[i].rgbRed );
		if( ucGreen > pRGBPalette[i].rgbGreen ) Green = (DWORD) ( pRGBPalette[i].rgbGreen - ucGreen );
		else Green = (DWORD) ( ucGreen - pRGBPalette[i].rgbGreen );
		if( ucBlue > pRGBPalette[i].rgbBlue ) Blue = (DWORD) ( pRGBPalette[i].rgbBlue - ucBlue );
		else Blue = (DWORD) ( ucBlue - pRGBPalette[i].rgbBlue );
		NewDiff = ( Red * Red ) + ( Green * Green ) + ( Blue * Blue );
		if( NewDiff < Diff ){
			if( NewDiff <= 1 ) return( i );
			Diff = NewDiff;
			Index = i;
			}
		}

	return( Index );

}

BOOL CImageObject::Rotate( int nDegrees )
{

	m_nLastError = IMAGELIB_HDIB_NULL;
	if( m_hDib == NULL ) return( FALSE );

	m_nLastError = IMAGELIB_ROTATE_ERROR;
	switch( nDegrees ){
		case 90:
			if( !RotateIt( nDegrees ) ) return( FALSE );
			break;
		case 180:
			if( !Invert() ) return( FALSE );
			if( !Reverse() ) return( FALSE );
			break;
		case 270:
			if( !RotateIt( nDegrees ) ) return( FALSE );
			break;
		default:
			m_nLastError = IMAGELIB_ROTATION_VALUE_ERROR;
			return( FALSE );
		}

	m_nLastError = IMAGELIB_SUCCESS;

	return( TRUE );

}

BOOL CImageObject::RotateIt( int nDegrees )
{

	int nNewWidth, nNewHeight;
	nNewWidth = m_nHeight;
	nNewHeight = m_nWidth;

	unsigned char *pOldBuffer, *pNewBuffer;
	HGLOBAL hNewMemory;
	DWORD dwNewMemorySize;
	BITMAPFILEHEADER *pOldBFH, *pNewBFH;
	BITMAPINFOHEADER *pOldBIH, *pNewBIH;
	RGBQUAD *pOldPalette, *pNewPalette;
	unsigned char *pOldBits, *pNewBits;
	int nOldWidthBytes, nNewWidthBytes;
	int nNumColors;

	pOldBuffer = (unsigned char *) GetDIBPointer( &nOldWidthBytes, m_nBits, &nNewWidthBytes, nNewWidth );
	if( pOldBuffer == NULL ) return( FALSE );

	pOldBFH = (BITMAPFILEHEADER *) pOldBuffer;
	pOldBIH = (BITMAPINFOHEADER *) &pOldBuffer[sizeof(BITMAPFILEHEADER)];
	nNumColors = m_nColors;
	pOldPalette = (RGBQUAD *) &pOldBuffer[sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)];
	pOldBits = (unsigned char *) &pOldBuffer[sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+nNumColors*sizeof(RGBQUAD)];

	dwNewMemorySize = (DWORD) nNumColors * sizeof( RGBQUAD ) + sizeof( BITMAPFILEHEADER ) + sizeof( BITMAPINFOHEADER ) + nNewWidthBytes * nNewHeight;
	m_nLastError = IMAGELIB_MEMORY_ALLOCATION_ERROR;
	hNewMemory = ::GlobalAlloc( GMEM_MOVEABLE | GMEM_ZEROINIT, dwNewMemorySize );
	if( hNewMemory == NULL ){
		::GlobalUnlock( m_hDib );
		return( FALSE );
		}

	m_nLastError = IMAGELIB_MEMORY_LOCK_ERROR;
	pNewBuffer = (unsigned char *) ::GlobalLock( hNewMemory );
	if( pNewBuffer == NULL ){
		::GlobalFree( hNewMemory );
		::GlobalUnlock( m_hDib );
		return( FALSE );
		}

	pNewBFH = (BITMAPFILEHEADER *) pNewBuffer;
	pNewBIH = (BITMAPINFOHEADER *) &pNewBuffer[sizeof(BITMAPFILEHEADER)];
	pNewPalette = (RGBQUAD *) &pNewBuffer[sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)];
	pNewBits = (unsigned char *) &pNewBuffer[sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+nNumColors*sizeof(RGBQUAD)];

	*pNewBFH = *pOldBFH;
	*pNewBIH = *pOldBIH;
	pNewBIH->biSizeImage = nNewWidthBytes * nNewHeight;
	pNewBIH->biWidth = nNewWidth;
	pNewBIH->biHeight = nNewHeight;
	if( nNumColors != 0 ) memcpy( pNewPalette, pOldPalette, nNumColors * sizeof( RGBQUAD ) );

	m_nWidth = nNewWidth;
	m_nHeight = nNewHeight;

	if( nDegrees == 90 ) Rotate90( pNewBits, pOldBits );
	else Rotate270( pNewBits, pOldBits );

	::GlobalUnlock( m_hDib );
	::GlobalUnlock( hNewMemory );
	::GlobalFree( m_hDib );
	m_hDib = hNewMemory;

	m_nLastError = IMAGELIB_SUCCESS;

	return( TRUE );

}

void CImageObject::Rotate90( unsigned char *pNewBits, unsigned char *pOldBits )
{

	int nOldWidth, nOldHeight, nOldWidthBytes, nNewWidthBytes;
	nOldWidth = m_nHeight;
	nOldHeight = m_nWidth;

	unsigned char *pTemp;
	unsigned char Data, Data1, Data2, Data3, Data4, Mask, Shift;
	int x, y;

	switch( m_nBits ){
		case 1:
			nOldWidthBytes = ( nOldWidth + 7 ) / 8;
			while( ( nOldWidthBytes & 3 ) != 0 ) nOldWidthBytes++;
			nNewWidthBytes = ( m_nWidth + 7 ) / 8;
			while( ( nNewWidthBytes & 3 ) != 0 ) nNewWidthBytes++;
			for( y=0; y<m_nHeight; y++ ){
				pTemp = pNewBits;
				pTemp += ( ( m_nHeight - 1 - y ) * nNewWidthBytes );
				for( x=0; x<m_nWidth; x++ ){
					Data = pOldBits[(m_nWidth-1-x)*nOldWidthBytes+(y/8)];
					Data >>= ( 7 - ( y & 7 ) );
					Data &= 0x01;
					Shift = ( 7 - ( ( m_nWidth - 1 - x ) & 7 ) );
					Mask = 1 << Shift;
					Data <<= Shift;
					pTemp[(m_nWidth-1-x)/8] &= ( Mask ^ 0xff );
					pTemp[(m_nWidth-1-x)/8] |= Data;
					}
				}
			break;
		case 4:
			nOldWidthBytes = ( nOldWidth + 1 ) / 2;
			while( ( nOldWidthBytes & 3 ) != 0 ) nOldWidthBytes++;
			nNewWidthBytes = ( m_nWidth + 1 ) / 2;
			while( ( nNewWidthBytes & 3 ) != 0 ) nNewWidthBytes++;
			for( y=0; y<m_nHeight; y++ ){
				pTemp = pNewBits;
				pTemp += ( ( m_nHeight - 1 - y ) * nNewWidthBytes );
				for( x=0; x<m_nWidth; x++ ){
					Data = pOldBits[(m_nWidth-1-x)*nOldWidthBytes+(y/2)];
					if( !( y & 1 ) ) Data >>= 4;
					else Data &= 0x0f;
					if( ( x & 1 ) ){
						Data <<= 4;
						pTemp[(m_nWidth-1-x)/2] &= 0x0f;
						pTemp[(m_nWidth-1-x)/2] |= Data;
						}
					else{
						pTemp[(m_nWidth-1-x)/2] &= 0xf0;
						pTemp[(m_nWidth-1-x)/2] |= Data;
						}
					}
				}
			break;
		case 8:
			nOldWidthBytes = nOldWidth;
			while( ( nOldWidthBytes & 3 ) != 0 ) nOldWidthBytes++;
			nNewWidthBytes = m_nWidth;
			while( ( nNewWidthBytes & 3 ) != 0 ) nNewWidthBytes++;
			for( y=0; y<m_nHeight; y++ ){
				pTemp = pNewBits;
				pTemp += ( ( m_nHeight - 1 - y ) * nNewWidthBytes );
				for( x=0; x<m_nWidth; x++ ){
					Data = pOldBits[(m_nWidth-1-x)*nOldWidthBytes+y];
					pTemp[m_nWidth-1-x] = Data;
					}
				}
			break;
		case 16:
			nOldWidthBytes = nOldWidth * 2;
			while( ( nOldWidthBytes & 3 ) != 0 ) nOldWidthBytes++;
			nNewWidthBytes = m_nWidth * 2;
			while( ( nNewWidthBytes & 3 ) != 0 ) nNewWidthBytes++;
			for( y=0; y<m_nHeight; y++ ){
				pTemp = pNewBits;
				pTemp += ( ( m_nHeight - 1 - y ) * nNewWidthBytes );
				for( x=0; x<m_nWidth; x++ ){
					Data1 = pOldBits[(m_nWidth-1-x)*nOldWidthBytes+y*2];
					Data2 = pOldBits[(m_nWidth-1-x)*nOldWidthBytes+y*2+1];
					pTemp[(m_nWidth-1-x)*2] = Data1;
					pTemp[(m_nWidth-1-x)*2+1] = Data2;
					}
				}
			break;
		case 24:
			nOldWidthBytes = nOldWidth * 3;
			while( ( nOldWidthBytes & 3 ) != 0 ) nOldWidthBytes++;
			nNewWidthBytes = m_nWidth * 3;
			while( ( nNewWidthBytes & 3 ) != 0 ) nNewWidthBytes++;
			for( y=0; y<m_nHeight; y++ ){
				pTemp = pNewBits;
				pTemp += ( ( m_nHeight - 1 - y ) * nNewWidthBytes );
				for( x=0; x<m_nWidth; x++ ){
					Data1 = pOldBits[(m_nWidth-1-x)*nOldWidthBytes+y*3];
					Data2 = pOldBits[(m_nWidth-1-x)*nOldWidthBytes+y*3+1];
					Data3 = pOldBits[(m_nWidth-1-x)*nOldWidthBytes+y*3+2];
					pTemp[(m_nWidth-1-x)*3] = Data1;
					pTemp[(m_nWidth-1-x)*3+1] = Data2;
					pTemp[(m_nWidth-1-x)*3+2] = Data3;
					}
				}
			break;
		case 32:
			nOldWidthBytes = nOldWidth * 4;
			while( ( nOldWidthBytes & 3 ) != 0 ) nOldWidthBytes++;
			nNewWidthBytes = m_nWidth * 4;
			while( ( nNewWidthBytes & 3 ) != 0 ) nNewWidthBytes++;
			for( y=0; y<m_nHeight; y++ ){
				pTemp = pNewBits;
				pTemp += ( ( m_nHeight - 1 - y ) * nNewWidthBytes );
				for( x=0; x<m_nWidth; x++ ){
					Data1 = pOldBits[(m_nWidth-1-x)*nOldWidthBytes+y*4];
					Data2 = pOldBits[(m_nWidth-1-x)*nOldWidthBytes+y*4+1];
					Data3 = pOldBits[(m_nWidth-1-x)*nOldWidthBytes+y*4+2];
					Data4 = pOldBits[(m_nWidth-1-x)*nOldWidthBytes+y*4+3];
					pTemp[(m_nWidth-1-x)*4] = Data1;
					pTemp[(m_nWidth-1-x)*4+1] = Data2;
					pTemp[(m_nWidth-1-x)*4+2] = Data3;
					pTemp[(m_nWidth-1-x)*4+3] = Data4;
					}
				}
			break;
		}

}

void CImageObject::Rotate270( unsigned char *pNewBits, unsigned char *pOldBits )
{

	int nOldWidth, nOldHeight, nOldWidthBytes, nNewWidthBytes;
	nOldWidth = m_nHeight;
	nOldHeight = m_nWidth;

	unsigned char *pTemp;
	unsigned char Data, Data1, Data2, Data3, Data4, Mask, Shift;
	int x, y, yy;

	switch( m_nBits ){
		case 1:
			nOldWidthBytes = ( nOldWidth + 7 ) / 8;
			while( ( nOldWidthBytes & 3 ) != 0 ) nOldWidthBytes++;
			nNewWidthBytes = ( m_nWidth + 7 ) / 8;
			while( ( nNewWidthBytes & 3 ) != 0 ) nNewWidthBytes++;
			for( y=0; y<m_nHeight; y++ ){
				yy = m_nHeight - 1 - y;
				pTemp = pNewBits;
				pTemp += ( ( m_nHeight - 1 - y ) * nNewWidthBytes );
				for( x=0; x<m_nWidth; x++ ){
					Data = pOldBits[(m_nWidth-1-x)*nOldWidthBytes+(yy/8)];
					Data >>= ( 7 - ( yy & 7 ) );
					Data &= 0x01;
					Shift = ( 7 - ( x & 7 ) );
					Mask = 1 << Shift;
					Data <<= Shift;
					pTemp[x/8] &= ( Mask ^ 0xff );
					pTemp[x/8] |= Data;
					}
				}
			break;
		case 4:
			nOldWidthBytes = ( nOldWidth + 1 ) / 2;
			while( ( nOldWidthBytes & 3 ) != 0 ) nOldWidthBytes++;
			nNewWidthBytes = ( m_nWidth + 1 ) / 2;
			while( ( nNewWidthBytes & 3 ) != 0 ) nNewWidthBytes++;
			for( y=0; y<m_nHeight; y++ ){
				yy = m_nHeight - 1 - y;
				pTemp = pNewBits;
				pTemp += ( ( m_nHeight - 1 - y ) * nNewWidthBytes );
				for( x=0; x<m_nWidth; x++ ){
					Data = pOldBits[(m_nWidth-1-x)*nOldWidthBytes+(yy/2)];
					if( !( yy & 1 ) ) Data >>= 4;
					else Data &= 0x0f;
					if( !( x & 1 ) ){
						Data <<= 4;
						pTemp[x/2] &= 0x0f;
						pTemp[x/2] |= Data;
						}
					else{
						pTemp[x/2] &= 0xf0;
						pTemp[x/2] |= Data;
						}
					}
				}
			break;
		case 8:
			nOldWidthBytes = nOldWidth;
			while( ( nOldWidthBytes & 3 ) != 0 ) nOldWidthBytes++;
			nNewWidthBytes = m_nWidth;
			while( ( nNewWidthBytes & 3 ) != 0 ) nNewWidthBytes++;
			for( y=0; y<m_nHeight; y++ ){
				yy = m_nHeight - 1 - y;
				pTemp = pNewBits;
				pTemp += ( ( m_nHeight - 1 - y ) * nNewWidthBytes );
				for( x=0; x<m_nWidth; x++ ){
					Data = pOldBits[(m_nWidth-1-x)*nOldWidthBytes+yy];
					pTemp[x] = Data;
					}
				}
			break;
		case 16:
			nOldWidthBytes = nOldWidth * 2;
			while( ( nOldWidthBytes & 3 ) != 0 ) nOldWidthBytes++;
			nNewWidthBytes = m_nWidth * 2;
			while( ( nNewWidthBytes & 3 ) != 0 ) nNewWidthBytes++;
			for( y=0; y<m_nHeight; y++ ){
				yy = m_nHeight - 1 - y;
				pTemp = pNewBits;
				pTemp += ( ( m_nHeight - 1 - y ) * nNewWidthBytes );
				for( x=0; x<m_nWidth; x++ ){
					Data1 = pOldBits[(m_nWidth-1-x)*nOldWidthBytes+yy*2];
					Data2 = pOldBits[(m_nWidth-1-x)*nOldWidthBytes+yy*2+1];
					pTemp[x*2] = Data1;
					pTemp[x*2+1] = Data2;
					}
				}
			break;
		case 24:
			nOldWidthBytes = nOldWidth * 3;
			while( ( nOldWidthBytes & 3 ) != 0 ) nOldWidthBytes++;
			nNewWidthBytes = m_nWidth * 3;
			while( ( nNewWidthBytes & 3 ) != 0 ) nNewWidthBytes++;
			for( y=0; y<m_nHeight; y++ ){
				yy = m_nHeight - 1 - y;
				pTemp = pNewBits;
				pTemp += ( ( m_nHeight - 1 - y ) * nNewWidthBytes );
				for( x=0; x<m_nWidth; x++ ){
					Data1 = pOldBits[(m_nWidth-1-x)*nOldWidthBytes+yy*3];
					Data2 = pOldBits[(m_nWidth-1-x)*nOldWidthBytes+yy*3+1];
					Data3 = pOldBits[(m_nWidth-1-x)*nOldWidthBytes+yy*3+2];
					pTemp[x*3] = Data1;
					pTemp[x*3+1] = Data2;
					pTemp[x*3+2] = Data3;
					}
				}
			break;
		case 32:
			nOldWidthBytes = nOldWidth * 4;
			while( ( nOldWidthBytes & 3 ) != 0 ) nOldWidthBytes++;
			nNewWidthBytes = m_nWidth * 4;
			while( ( nNewWidthBytes & 3 ) != 0 ) nNewWidthBytes++;
			for( y=0; y<m_nHeight; y++ ){
				yy = m_nHeight - 1 - y;
				pTemp = pNewBits;
				pTemp += ( ( m_nHeight - 1 - y ) * nNewWidthBytes );
				for( x=0; x<m_nWidth; x++ ){
					Data1 = pOldBits[(m_nWidth-1-x)*nOldWidthBytes+yy*4];
					Data2 = pOldBits[(m_nWidth-1-x)*nOldWidthBytes+yy*4+1];
					Data3 = pOldBits[(m_nWidth-1-x)*nOldWidthBytes+yy*4+2];
					Data4 = pOldBits[(m_nWidth-1-x)*nOldWidthBytes+yy*4+3];
					pTemp[x*4] = Data1;
					pTemp[x*4+1] = Data2;
					pTemp[x*4+2] = Data3;
					pTemp[x*4+3] = Data4;
					}
				}
			break;
		}

}

BOOL CImageObject::Invert( void )
{

	unsigned char *pBuffer, *pTemp1, *pTemp2;
	BITMAPINFOHEADER *pBIH;
	RGBQUAD *pPalette;
	unsigned char *pBits;
	int nWidthBytes;
	int nNumColors;
	int y, yy;
	unsigned char *pTemp;

	pBuffer = (unsigned char *) GetDIBPointer( &nWidthBytes );
	if( pBuffer == NULL ) return( FALSE );

	pBIH = (BITMAPINFOHEADER *) &pBuffer[sizeof(BITMAPFILEHEADER)];
	nNumColors = m_nColors;
	pPalette = (RGBQUAD *) &pBuffer[sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)];
	pBits = (unsigned char *) &pBuffer[sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+nNumColors*sizeof(RGBQUAD)];

	pTemp = new unsigned char [nWidthBytes];
	if( pTemp != NULL ){
		yy = pBIH->biHeight;
		if( !( yy & 1 ) ) yy /= 2;
		else yy = ( yy / 2 ) + 1;
		for( y=0; y<yy; y++ ){
			pTemp1 = pBits;
			pTemp1 += ( ( pBIH->biHeight - 1 - y ) * nWidthBytes );
			pTemp2 = pBits;
			pTemp2 += y * nWidthBytes;
			memcpy( pTemp, pTemp1, nWidthBytes );
			memcpy( pTemp1, pTemp2, nWidthBytes );
			memcpy( pTemp2, pTemp, nWidthBytes );
			}
		delete [] pTemp;
		}

	::GlobalUnlock( m_hDib );

	m_nLastError = IMAGELIB_SUCCESS;

	return( TRUE );

}

BOOL CImageObject::Reverse( void )
{

	unsigned char *pBuffer, *pTemp;
	BITMAPINFOHEADER *pBIH;
	RGBQUAD *pPalette;
	unsigned char *pBits;
	int nWidthBytes;
	int nNumColors;
	int y, x, xx, xt, nWidth;
	unsigned char Data, Tmp, Data1, Tmp1;

	pBuffer = (unsigned char *) GetDIBPointer( &nWidthBytes );
	if( pBuffer == NULL ) return( FALSE );

	pBIH = (BITMAPINFOHEADER *) &pBuffer[sizeof(BITMAPFILEHEADER)];
	nNumColors = m_nColors;
	pPalette = (RGBQUAD *) &pBuffer[sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)];
	pBits = (unsigned char *) &pBuffer[sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+nNumColors*sizeof(RGBQUAD)];

	xx = pBIH->biWidth;
	if( !( xx & 1 ) ) xx /= 2;
	else xx = ( xx / 2 ) + 1;
	nWidth = pBIH->biWidth;
	if( m_nBits == 4 ){
		xx = pBIH->biWidth;
		xx = ( xx + 3 ) / 4;
		nWidth = ( pBIH->biWidth + 1 ) / 2;
		}
	else if( m_nBits == 1 ){
		xx = pBIH->biWidth;
		xx = ( xx + 15 ) / 16;
		nWidth = ( pBIH->biWidth + 7 ) / 8;
		}

	for( y=0; y<pBIH->biHeight; y++ ){
		pTemp = pBits;
		pTemp += y * nWidthBytes;
		switch( m_nBits ){
			case 1:
				for( x=0; x<xx; x++ ){
					xt = nWidth - 1 - x;
					Data = pTemp[x];
					Data1 = Data << 7;
					Data1 |= ( ( Data & 0x02 ) << 5 );
					Data1 |= ( ( Data & 0x04 ) << 3 );
					Data1 |= ( ( Data & 0x08 ) << 1 );
					Data1 |= ( ( Data & 0x10 ) >> 1 );
					Data1 |= ( ( Data & 0x20 ) >> 3 );
					Data1 |= ( ( Data & 0x40 ) >> 5 );
					Data1 |= ( ( Data & 0x80 ) >> 7 );

					Tmp = pTemp[xt];
					Tmp1 = Tmp << 7;
					Tmp1 |= ( ( Tmp & 0x02 ) << 5 );
					Tmp1 |= ( ( Tmp & 0x04 ) << 3 );
					Tmp1 |= ( ( Tmp & 0x08 ) << 1 );
					Tmp1 |= ( ( Tmp & 0x10 ) >> 1 );
					Tmp1 |= ( ( Tmp & 0x20 ) >> 3 );
					Tmp1 |= ( ( Tmp & 0x40 ) >> 5 );
					Tmp1 |= ( ( Tmp & 0x80 ) >> 7 );

					pTemp[x] = Tmp1;
					pTemp[xt] = Data1;
					}
				break;
			case 4:
				for( x=0; x<xx; x++ ){
					xt = nWidth - 1 - x;
					Data = pTemp[x];
					Tmp = pTemp[xt];
					Data = ( Data << 4 ) | ( Data >> 4 );
					Tmp = ( Tmp << 4 ) | ( Tmp >> 4 );
					pTemp[x] = Tmp;
					pTemp[xt] = Data;
					}
				break;
			case 8:
				for( x=0; x<xx; x++ ){
					xt = nWidth - 1 - x;
					Data = pTemp[x];
					pTemp[x] = pTemp[xt];
					pTemp[xt] = Data;
					}
				break;
			case 16:
				for( x=0; x<xx; x++ ){
					xt = nWidth - 1 - x;
					Data = pTemp[x*2];
					pTemp[x*2] = pTemp[xt*2];
					pTemp[xt*2] = Data;
					Data = pTemp[x*2+1];
					pTemp[x*2+1] = pTemp[xt*2+1];
					pTemp[xt*2+1] = Data;
					}
				break;
			case 24:
				for( x=0; x<xx; x++ ){
					xt = nWidth - 1 - x;
					Data = pTemp[x*3];
					pTemp[x*3] = pTemp[xt*3];
					pTemp[xt*3] = Data;
					Data = pTemp[x*3+1];
					pTemp[x*3+1] = pTemp[xt*3+1];
					pTemp[xt*3+1] = Data;
					Data = pTemp[x*3+2];
					pTemp[x*3+2] = pTemp[xt*3+2];
					pTemp[xt*3+2] = Data;
					}
				break;
			case 32:
				for( x=0; x<xx; x++ ){
					xt = nWidth - 1 - x;
					Data = pTemp[x*4];
					pTemp[x*4] = pTemp[xt*4];
					pTemp[xt*4] = Data;
					Data = pTemp[x*4+1];
					pTemp[x*4+1] = pTemp[xt*4+1];
					pTemp[xt*4+1] = Data;
					Data = pTemp[x*4+2];
					pTemp[x*4+2] = pTemp[xt*4+2];
					pTemp[xt*4+2] = Data;
					Data = pTemp[x*4+3];
					pTemp[x*4+3] = pTemp[xt*4+3];
					pTemp[xt*4+3] = Data;
					}
				break;
			}
		}

	::GlobalUnlock( m_hDib );

	m_nLastError = IMAGELIB_SUCCESS;

	return( TRUE );

}

BOOL CImageObject::ChangeFormat( int nNewBitsPerPixel )
{

	m_nLastError = IMAGELIB_SUCCESS;
	if( nNewBitsPerPixel == m_nBits ) return( TRUE );

	int nOldWidthBytes, nNewWidthBytes;
	char *pBuffer = (char *) GetDIBPointer( &nOldWidthBytes, nNewBitsPerPixel, &nNewWidthBytes );
	if( pBuffer == NULL ) return( FALSE );

	BITMAPINFOHEADER *pOldBIH, *pNewBIH;
	BITMAPFILEHEADER *pOldBFH, *pNewBFH;
	RGBQUAD *pOldRGBPalette, *pNewRGBPalette;
	unsigned char *pOldBits, *pNewBits;
	int nNumColors, nNumNewColors;

	pOldBFH = (BITMAPFILEHEADER *) pBuffer;
	pOldBIH = (BITMAPINFOHEADER *) &pBuffer[sizeof(BITMAPFILEHEADER)];
	pOldRGBPalette = (RGBQUAD *) &pBuffer[sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)];
	nNumColors = m_nColors;
	nNumNewColors = 1 << nNewBitsPerPixel;
	if( nNewBitsPerPixel > 8 ) nNumNewColors = 0;
	pOldBits = (unsigned char *) &pBuffer[sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+nNumColors*sizeof(RGBQUAD)];

	if( m_nBits >= 16 && nNewBitsPerPixel < 16 ){
		if( m_pLogPal != NULL ) delete [] m_pLogPal;
		m_pLogPal = CreatePaletteFromBitmap( nNumNewColors, pOldBits, m_nBits, m_nWidth, m_nHeight );
		}

	HGLOBAL hGlobal;
	DWORD dwSize;
	dwSize = sizeof( BITMAPFILEHEADER ) + sizeof( BITMAPINFOHEADER ) + nNumNewColors * sizeof( RGBQUAD ) + m_nHeight * nNewWidthBytes;
	hGlobal = ::GlobalAlloc( GMEM_MOVEABLE | GMEM_ZEROINIT, dwSize );
	if( hGlobal == NULL ){
		::GlobalUnlock( m_hDib );
		return( FALSE );
		}

	pBuffer = (char *) ::GlobalLock( hGlobal );
	if( pBuffer == NULL ){
		::GlobalFree( hGlobal );
		::GlobalUnlock( m_hDib );
		return( FALSE );
		}

	pNewBFH = (BITMAPFILEHEADER *) pBuffer;
	pNewBIH = (BITMAPINFOHEADER *) &pBuffer[sizeof(BITMAPFILEHEADER)];
	pNewRGBPalette = (RGBQUAD *) &pBuffer[sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)];
	*pNewBFH = *pOldBFH;
	*pNewBIH = *pOldBIH;
	int i, j = nNumNewColors;

	if( m_nBits < 16 && nNewBitsPerPixel < 16 ){
		for( i=0; i<j; i++ ) pNewRGBPalette[i] = pOldRGBPalette[i];
		}
	else if( m_nBits >= 16 ){
 		for( i=0; i<j; i++ ){
			pNewRGBPalette[i].rgbRed = m_pLogPal->palPalEntry[i].peRed;
			pNewRGBPalette[i].rgbGreen = m_pLogPal->palPalEntry[i].peGreen;
			pNewRGBPalette[i].rgbBlue = m_pLogPal->palPalEntry[i].peBlue;
			}
		}
	pNewBIH->biBitCount = nNewBitsPerPixel;
	pNewBIH->biSizeImage = nNewWidthBytes * m_nHeight;
	pNewBIH->biClrUsed = nNumNewColors;
	pNewBFH->bfSize = sizeof( BITMAPFILEHEADER ) + sizeof( BITMAPINFOHEADER ) + nNumNewColors * sizeof( RGBQUAD ) + pNewBIH->biSizeImage;
	pNewBFH->bfOffBits = sizeof( BITMAPFILEHEADER ) + sizeof( BITMAPINFOHEADER ) + nNumNewColors * sizeof( RGBQUAD );
	pNewBits = (unsigned char *) &pBuffer[sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+nNumNewColors*sizeof(RGBQUAD)];
	m_nPaletteBytes = nNumNewColors * sizeof( RGBQUAD );

	for( int y=0; y<m_nHeight; y++ ){
		unsigned char ucRed, ucGreen, ucBlue;
		unsigned char *pSrc, *pDest;
		pSrc = pOldBits;
		pSrc += ( nOldWidthBytes * y );
		pDest = pNewBits;
		pDest += ( nNewWidthBytes * y );
		for( int x=0; x<m_nWidth; x++ ){
			switch( m_nBits ){
				case 1:
					if( pSrc[x/8] & ( 0x80 >> ( x & 7 ) ) )
						ucRed = ucGreen = ucBlue = 0xff;
					else
						ucRed = ucGreen = ucBlue = 0x00;
					break;
				case 4:
					if( !( x & 1 ) ){
						ucRed = pOldRGBPalette[pSrc[x/2]>>4].rgbRed;
						ucGreen = pOldRGBPalette[pSrc[x/2]>>4].rgbGreen;
						ucBlue = pOldRGBPalette[pSrc[x/2]>>4].rgbBlue;
						}
					else{
						ucRed = pOldRGBPalette[pSrc[x/2]&15].rgbRed;
						ucGreen = pOldRGBPalette[pSrc[x/2]&15].rgbGreen;
						ucBlue = pOldRGBPalette[pSrc[x/2]&15].rgbBlue;
						}
					break;
				case 8:
					ucRed = pOldRGBPalette[pSrc[x]].rgbRed;
					ucGreen = pOldRGBPalette[pSrc[x]].rgbGreen;
					ucBlue = pOldRGBPalette[pSrc[x]].rgbBlue;
					break;
				case 16:
					GETRGB555( ucRed, ucGreen, ucBlue, &pSrc[x*2] );
					break;
				case 24:
					ucRed = pSrc[x*3+2];
					ucGreen = pSrc[x*3+1];
					ucBlue = pSrc[x*3];
					break;
				case 32:
					GETRGB888( ucRed, ucGreen, ucBlue, &pSrc[x*4] );
					break;
				}

			switch( nNewBitsPerPixel ){
				case 1:
					if( !( x & 7 ) ) pDest[x/8] = 0;
					pDest[x/8] |= (unsigned char) GetNearestIndex( ucRed, ucGreen, ucBlue, pNewRGBPalette, nNumNewColors ) << ( x & 7 );
					break;
				case 4:
					if( !( x & 1 ) )
						pDest[x/2] = (unsigned char) GetNearestIndex( ucRed, ucGreen, ucBlue, pNewRGBPalette, nNumNewColors ) << 4;
					else
						pDest[x/2] |= (unsigned char) GetNearestIndex( ucRed, ucGreen, ucBlue, pNewRGBPalette, nNumNewColors );
					break;
				case 8:
					pDest[x] = (unsigned char) GetNearestIndex( ucRed, ucGreen, ucBlue, pNewRGBPalette, nNumNewColors );
					break;
				case 16:
					PUTRGB555( ucRed, ucGreen, ucBlue, &pDest[x*2] );
					break;
				case 24:
					pDest[x*3+2] = ucRed;
					pDest[x*3+1] = ucGreen;
					pDest[x*3] = ucBlue;
					break;
				case 32:
					PUTRGB888( ucRed, ucGreen, ucBlue, &pDest[x*4] );
					break;
				}
			}
		}

	::GlobalUnlock( m_hDib );
	::GlobalFree( m_hDib );
	::GlobalUnlock( hGlobal );
	m_hDib = hGlobal;

	ProcessImageHeader();

	return( TRUE );

}

LOGPALETTE *CImageObject::CreatePaletteFromBitmap( int nNumColors, unsigned char *pBits, int nBits, int nWidth, int nHeight )
{

	RGBQUAD *pRGBPalette;
	if( nBits != 8 && m_nPaletteCreationType == POPULARITY_PALETTE ) pRGBPalette = MakePopularityPalette( nNumColors, pBits, nBits, nWidth, nHeight );
	else if( nBits != 8 && m_nPaletteCreationType == MEDIAN_CUT_PALETTE ) pRGBPalette = MakeMedianCutPalette( nNumColors, pBits, nBits, nWidth, nHeight );
	else if( m_nPaletteCreationType == FIXED_PALETTE ) pRGBPalette = MakeFixedPalette( nNumColors );
	if( pRGBPalette == NULL ) return( NULL );

	LOGPALETTE *pLogPal = CreateLogPalette( pRGBPalette, nNumColors );

	delete [] pRGBPalette;

	return( pLogPal );

}

void CImageObject::SetPaletteCreationType( int nType )
{

	m_nPaletteCreationType = nType;

}

int CImageObject::GetPaletteCreationType( void )
{

	return( m_nPaletteCreationType );

}

RGBQUAD *CImageObject::MakePopularityPalette( int nNumColors, unsigned char *pBits, int nBits, int nWidth, int nHeight )
{
	RGBQUAD *pRGBQuad = new RGBQUAD[nNumColors];
	if( pRGBQuad == NULL ) return( MakeFixedPalette( nNumColors ) );
	memset( pRGBQuad, 0, nNumColors * sizeof( RGBQUAD ) );

	BYTE ColMap[256][3];
	if( !Popularity( pBits, nBits, nWidth, nHeight, ColMap ) ){
		delete [] pRGBQuad;
		return( MakeFixedPalette( nNumColors ) );
		}

	for( int i=0; i<nNumColors; i++ ){
		pRGBQuad[i].rgbRed = ColMap[i][0];
		pRGBQuad[i].rgbGreen = ColMap[i][1];
		pRGBQuad[i].rgbBlue = ColMap[i][2];
		}

	return( pRGBQuad );

}

RGBQUAD *CImageObject::MakeMedianCutPalette( int nNumColors, unsigned char *pBits, int nBits, int nWidth, int nHeight )
{
	RGBQUAD *pRGBQuad = new RGBQUAD[nNumColors];
	if( pRGBQuad == NULL ) return( MakeFixedPalette( nNumColors ) );
	memset( pRGBQuad, 0, nNumColors * sizeof( RGBQUAD ) );

	BYTE ColMap[256][3];
	WORD *Hist = new WORD[32768];
	if( Hist == NULL ){
		delete [] pRGBQuad;
		return( MakeFixedPalette( nNumColors ) );
		}
	memset( Hist, 0, 32768 * sizeof( WORD ) );

	int nWidthBytes = WidthBytes( nBits, nWidth );

	for( int y=0; y<nHeight; y++ ){
		unsigned char *pData = pBits;
		unsigned char ucRed, ucGreen, ucBlue;
		WORD color;
		pData += ( y * nWidthBytes );
		for( int x=0; x<nWidth; x++ ){
			switch( nBits ){
				case 16:
					GETRGB555( ucRed, ucGreen, ucBlue, &pData[x*2] );
					break;
				case 24:
					ucRed = pData[x*3+2];
					ucGreen = pData[x*3+1];
					ucBlue = pData[x*3];
					break;
				case 32:
					GETRGB888( ucRed, ucGreen, ucBlue, &pData[x*4] );
					break;
				}
			color = _RGB( ucRed, ucGreen, ucBlue );
			if( Hist[color] < 65535 ) Hist[color]++;
			}
		}

	MedianCut( Hist, ColMap, (int) 256 );
	for( int i=0; i<nNumColors; i++ ){
		pRGBQuad[i].rgbRed = ColMap[i][0];
		pRGBQuad[i].rgbGreen = ColMap[i][1];
		pRGBQuad[i].rgbBlue = ColMap[i][2];
		}
	delete [] Hist;

	return( pRGBQuad );

}

RGBQUAD *CImageObject::MakeFixedPalette( int nNumColors )
{
	RGBQUAD *pRGBQuad = new RGBQUAD[nNumColors];
	if( pRGBQuad == NULL ) return( NULL );

	static int colors[] = {
		255, 255, 255,
		0, 0, 0,
		255, 0, 0,
		0, 0, 255,
		0, 255, 0,
		150, 150, 150,
		255, 255, 0,
		0, 150, 150,
		150, 0, 150,
		150, 150, 0,
		0, 255, 255,
		255, 0, 255,
		255, 120, 120,
		120, 255, 120,
		120, 120, 255,
		90, 90, 90 };

	int nSteps = ( ( nNumColors + 15 ) / 16 );

	for( int i=0; i<nSteps; i++ ){
		for( int j=0; j<16; j++ ){
			if( i * 16 + j < nNumColors ){
				int r, g, b;
				r = colors[j*3];
				g = colors[j*3+1];
				b = colors[j*3+2];
				r = ( ( nSteps - i ) * r ) / nSteps;
				g = ( ( nSteps - i ) * g ) / nSteps;
				b = ( ( nSteps - i ) * b ) / nSteps;
				pRGBQuad[i*16+j].rgbRed = (unsigned char) r;
				pRGBQuad[i*16+j].rgbGreen = (unsigned char) g;
				pRGBQuad[i*16+j].rgbBlue = (unsigned char) b;
				}
			}
		}

	return( pRGBQuad );

}

int CImageObject::GetPaletteBytes( void )
{

	return( m_nPaletteBytes );

}

HGLOBAL CImageObject::GetDib( void )
{

	return( m_hDib );

}

CPalette *CImageObject::GetPalette( void )
{

	return( &m_Palette );

}

LOGPALETTE *CImageObject::GetLogPal( void )
{

	return( m_pLogPal );

}
