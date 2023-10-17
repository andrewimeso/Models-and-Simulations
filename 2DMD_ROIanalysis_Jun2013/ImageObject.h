// ImageObject.h

#ifndef __IMAGEOBJECT_H__
#define __IMAGEOBJECT_H__

#include "Errors.h"
#include "ImageLoad.h"

#define POPULARITY_PALETTE 0
#define MEDIAN_CUT_PALETTE 1
#define FIXED_PALETTE 2

#pragma pack(1)
typedef struct{
	unsigned short m_nBlueList[256];
} GREENLIST;

typedef struct{
	GREENLIST *m_pGreenList[256];
} REDLIST;

typedef struct{
	unsigned short nCount;
	unsigned char ucRed;
	unsigned char ucGreen;
	unsigned char ucBlue;
} COUNTLIST;
#pragma pack()

#define GETRGB555( a, b, c, d ) { WORD *wData = (WORD *) d; a = (unsigned char) ( ( (*wData) & 0x7c00 ) >> 7 ); b = (unsigned char) ( ( (*wData) & 0x03e0 ) >> 2 ); c = (unsigned char) ( ( (*wData) & 0x001f ) << 3 ); }
#define GETRGB565( a, b, c, d ) { WORD *wData = (WORD *) d; a = (unsigned char) ( ( (*wData) & 0xf800 ) >> 8 ); b = (unsigned char) ( ( (*wData) & 0x07e0 ) >> 3 ); c = (unsigned char) ( ( (*wData) & 0x001f ) << 3 ); }
#define GETRGB888( a, b, c, d ) { DWORD *dwData = (DWORD *) d; a = (unsigned char) ( (*dwData) >> 16 ); b = (unsigned char) ( ( (*dwData) & 0x0000ff00 ) >> 8 ); c = (unsigned char) ( (*dwData) & 0x000000ff ); }
#define PUTRGB555( a, b, c, d ) { WORD *wData = (WORD *) d; *wData = ( ( ( (WORD) a & 0x00f8 ) << 7 ) | ( ( (WORD) b & 0x00f8 ) << 2 ) | ( (WORD) c >> 3 ) ); }
#define PUTRGB565( a, b, c, d ) { WORD *wData = (WORD *) d; *wData = ( ( ( (WORD) a & 0x00f8 ) << 8 ) | ( ( (WORD) b & 0x00fc ) << 3 ) | ( (WORD) c >> 3 ) ); }
#define PUTRGB888( a, b, c, d ) { DWORD *dwData = (DWORD *) d; *dwData = ( (DWORD) a << 16 ) | ( (DWORD) b << 8 ) | (DWORD) c; }

#define _RGB(r,g,b) (WORD)(((b)&~7)<<7)|(((g)&~7)<<2)|((r)>>3)

class CImageObject
{

public:
	CImageObject();
	CImageObject( const char *, CDC *pDC = NULL, int nX = -1, int nY = -1 );
	~CImageObject();

	BOOL Load( const char *, CDC *pDC = NULL, int nX = -1, int nY = -1 );
	int GetLastError( void );
	BOOL Save( const char *, int nType = -1 );
	int GetWidth( void );
	int GetHeight( void );
	int GetNumBits( void );
	int GetNumColors( void );
	BOOL GetPaletteData( RGBQUAD * );
	RGBQUAD *GetPaletteData( void );
	int GetImageType( const char * );
	int GetImageType( void );
	BOOL GetImageInfo( const char *, int *pnWidth = NULL,
		int *pnHeight = NULL, int *pnPlanes = NULL,
		int *pnBitsPerPixel = NULL, int *pnNumColors = NULL );
	BOOL Draw( CDC *, int x = -1, int y = -1 );
	BOOL SetPalette( CDC * );
	void SetPaletteCreationType( int );
	int GetPaletteCreationType( void );

	BOOL IsLoaded( void );
	int ExtensionIndex( const char * );

	BOOL Crop( int, int, int, int );
	BOOL Stretch( int, int );
	BOOL Rotate( int );
	BOOL Invert( void );
	BOOL Reverse( void );
	BOOL RotateIt( int );
	void Rotate90( unsigned char *, unsigned char * );
	void Rotate270( unsigned char *, unsigned char * );
	BOOL ChangeFormat( int );

	int GetPaletteBytes( void );
	HGLOBAL GetDib( void );
	CPalette *GetPalette( void );

	void operator= (const CImageObject &ImageObject);
	void NormalizeCoordinates( int *, int *, int *, int *, BOOL *bCompleteImage = NULL, BOOL *bLessThanHalf = NULL );
	void *GetDIBPointer( int *nWidthBytes = NULL, int nNewBits = 0, int *nNewWidthBytes = NULL, int nNewWidth = -1 );
	LOGPALETTE *GetLogPal( void );
	LOGPALETTE *CreateLogPalette( RGBQUAD *Palette, int NumColors );
	int GetNearestIndex( unsigned char, unsigned char, unsigned char, RGBQUAD *, int );

	void SetQuality( int nQual ){ m_nQuality = nQual; }

	static char *szExtensions[7];
	int m_nLastError;
	int m_nScreenPlanes, m_nScreenBits, m_nPaletteBytes;
	void ProcessPalette( void );
	int WidthBytes( int, int );
	void SetDib( HGLOBAL hDib ){ m_hDib = hDib; }
	void SetLogPal( LOGPALETTE *pLogPal ){ m_pLogPal = pLogPal; }

protected:
	void InitVars( BOOL bFlag = FALSE );
	void ProcessImageHeader( void );
	void KillImage( void );
	void CreatePaletteFromDIB( RGBQUAD *, int );
	LOGPALETTE *CreatePaletteFromBitmap( int, unsigned char *, int, int, int );

	RGBQUAD *MakePopularityPalette( int, unsigned char *, int, int, int );
	RGBQUAD *MakeMedianCutPalette( int, unsigned char *, int, int, int );
	RGBQUAD *MakeFixedPalette( int );

	int m_nWidth, m_nHeight, m_nPlanes, m_nBits, m_nColors, m_nImageType;
	int m_nX, m_nY;
	int m_nQuality;
	int m_nPaletteCreationType;
	CPalette m_Palette;
	HGLOBAL m_hDib;
	char *m_pszFilename;
	LOGPALETTE *m_pLogPal;


};

#endif

