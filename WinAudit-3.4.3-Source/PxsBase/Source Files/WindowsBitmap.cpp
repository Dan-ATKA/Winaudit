///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Windows Image Class Implementation
//
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Copyright 1987-2022 PARMAVEX SERVICES
//
// Licensed under the European Union Public Licence (EUPL), Version 1.1 or -
// as soon they will be approved by the European Commission - subsequent
// versions of the EUPL (the "Licence"). You may not use this work except in
// compliance with the Licence. You may obtain a copy of the Licence at:
//
// http://ec.europa.eu/idabc/eupl
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the Licence is distributed on an "AS IS" basis,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the Licence for the specific language governing permissions and
// limitations under the Licence. This source code is free software. It
// must not be sold, leased, rented, sub-licensed or used for any form of
// monetary recompense whatsoever. This notice must not be removed or altered
// from this source distribution.
//
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
// Remarks
///////////////////////////////////////////////////////////////////////////////////////////////////

// Bitmap functions such as GetDIBits, SetDIBits and CreateDIBitmap use a BITMAPINFO. However,
// this structure does not specify the number of bytes of colour table (palette) data. To avoid any
// buffer issues will only allow zero for the biClrUsed member of BITMAPINFOHEADER. In this case
// the number bytes used will be the  3 DWORD colour masks for 16 and 32-bit bitmaps when the
// compression is BI_BITFIELDS plus the maximum number of RGBQUAD entries for the given pixel
// depth. See method GetNumColourTableBytes. This restriction should not be an issue as if the
// BITMAPINFOHEADER was read from a device independent file, pallete information is not stored.
// Likewise, if have obtained BITMAPINFOHEADER from a HBITMAP, optimsed pallete colours are only
// for VGA monitors, i.e <= 256 colours.
//
// Any header other than BITMAPINFOHEADER is unsupported, i.e. BITMAPV4HEADER and BITMAPV5HEADER

// BMP
// biBitCount  biCompression   biClrUsed   bmiColors(RGBQUADs/DWORDs)  bitmap bits
// 1           BI_RGB          0           2                           1 bit per pixel
//
// 4           BI_RGB          0           16                          4 bits per pixel
// 4           BI_RGB          >0          biClrUsed                   4 bits per pixel
// 4           BI_RLE4         0           16                          variable
// 4           BI_RLE4         >0          biClrUsed                   variable
//
// 8           BI_RGB          0           256                         1 byte per pixel
// 8           BI_RGB          >0          biClrUsed                   1 byte per pixel
// 8           BI_RLE8         0           256                         variable
// 8           BI_RLE8         >0          biClrUsed                   variable
//
// 16          BI_RGB          0           0                           1 WORD per pixel
// 16          BI_RGB          >0          biClrUsed                   1 WORD per pixel
// 16          BI_BITFIELDS    0           3                           1 WORD per pixel
// 16          BI_BITFIELDS    >0          3+biClrUsed                 1 WORD per pixel
//
// 24          BI_RGB          0           0                           3 bytes per pixel
// 24          BI_RGB          >0          biClrUsed                   3 bytes per pixel
//
// 32          BI_RGB          0           0                           1 DWORD per pixel
// 32          BI_RGB          >0          biClrUsed                   1 DWORD per pixel
// 32          BI_BITFIELDS    0           3                           1 DWORD per pixel
// 32          BI_BITFIELDS    >0          3+biClrUsed                 1 DWORD per pixel

///////////////////////////////////////////////////////////////////////////////////////////////////
// Include Files
///////////////////////////////////////////////////////////////////////////////////////////////////

// 1. Own Interface
#include "PxsBase/Header Files/WindowsBitmap.h"

// 2. C System Files

// 3. C++ System Files

// 4. Other Libraries

// 5. This Project
#include "PxsBase/Header Files/AllocateBytes.h"
#include "PxsBase/Header Files/BoundsException.h"
#include "PxsBase/Header Files/NullException.h"
#include "PxsBase/Header Files/ParameterException.h"
#include "PxsBase/Header Files/SystemException.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
// Construction/Destruction
///////////////////////////////////////////////////////////////////////////////////////////////////

// Default constructor
WindowsBitmap::WindowsBitmap()
              :m_hBitmap( nullptr ),
               m_FileBytes()
{
    try
    {
        Reset();
    }
    catch ( const Exception& e )
    {
        PXSLogException( e, __FUNCTION__ );
    }
}

// Copy constructor - not allowed so no implementation

// Destructor
WindowsBitmap::~WindowsBitmap()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// Operators
///////////////////////////////////////////////////////////////////////////////////////////////////

// Assignment operator - not allowed so no implementation

///////////////////////////////////////////////////////////////////////////////////////////////////
// Public Methods
///////////////////////////////////////////////////////////////////////////////////////////////////

//===============================================================================================//
//  Description:
//      Draw this bitmap on the specified device context
//
//  Parameters:
//      hdc - the device context
//
//  Returns:
//      void
//===============================================================================================//
void WindowsBitmap::Draw( HDC hdc )
{
    if ( ( hdc == nullptr ) || ( m_hBitmap == nullptr ) )
    {
        return;     // Nothing to do
    }
    PXSDrawBitmap( hdc, m_hBitmap, 0, 0 );
}

//===============================================================================================//
//  Description:
//      Get this bitmap as bytes to store as a file (.bmp)
//
//  Parameters:
//      hdc        - the device context
//      pFileBytes - receives the bytes
//
//  Returns:
//      void
//===============================================================================================//
void WindowsBitmap::GetFileBytes( HDC hdc, ByteArray* pFileBytes )
{
    if ( ( hdc == nullptr ) || ( pFileBytes == nullptr ) )
    {
        throw NullException( L"hdc/pFileBytes", __FUNCTION__ );
    }
    pFileBytes->Zero();

    if ( m_FileBytes.GetSize() == 0 )
    {
        ToFileBytes( hdc );
    }
    *pFileBytes = m_FileBytes;
}

//===============================================================================================//
//  Description:
//      Get the pixel size of this bitmap
//
//  Parameters:
//      pSize - receives the size
//
//  Returns:
//      void
//===============================================================================================//
void WindowsBitmap::GetSize( SIZE* pSize ) const
{
    BITMAP bm;

    if ( pSize == nullptr )
    {
        throw NullException( L"pSize", __FUNCTION__ );
    }
    pSize->cx = 0;
    pSize->cy = 0;

    if ( m_hBitmap == nullptr )
    {
        return;     // no bitmap;
    }

    memset( &bm , 0 , sizeof ( bm ) );
    if ( GetObject( (HGDIOBJ)m_hBitmap, sizeof ( bm ), &bm ) == 0 )
    {
        throw SystemException( GetLastError(), L"GetObject", __FUNCTION__ );
    }
    pSize->cx = bm.bmWidth;
    pSize->cy = bm.bmHeight;
}

//===============================================================================================//
//  Description:
//      Reset this bitmap
//
//  Parameters:
//      none
//
//  Returns:
//      void
//===============================================================================================//
void WindowsBitmap::Reset()
{
    if ( m_hBitmap )
    {
        DeleteObject( m_hBitmap );
        m_hBitmap= nullptr;
    }
    m_FileBytes.Zero();
}

//===============================================================================================//
//  Description:
//      Set this bitmap from the specified input bitmaps
//
//  Parameters:
//      hBitmap - the handle to the bitmap
//
//  Returns:
//      void
//===============================================================================================//
void WindowsBitmap::Set( HBITMAP hBitmap )
{
    Reset();
    if ( hBitmap == nullptr )
    {
        return;     // nothing to do
    }

    m_hBitmap = (HBITMAP)CopyImage( hBitmap, IMAGE_BITMAP, 0, 0, 0 );
    if ( m_hBitmap == nullptr )
    {
        throw SystemException( GetLastError(), L"CopyImage", __FUNCTION__ );
    }
}

//===============================================================================================//
//  Description:
//      Set this bitmap from the specified input file bytes (.bmp)
//
//  Parameters:
//      FileBytes - the bitmap's file bytes
//
//  Returns:
//      void
//===============================================================================================//
void WindowsBitmap::SetFileBytes( HDC hdc, const ByteArray& FileBytes )
{
    const BYTE*  pBytes;
    BYTE    b14[ SIZE_OF_BITMAPFILEHEADER ] = { 0 };
    BYTE    b40[ SIZE_OF_BITMAPINFOHEADER ] = { 0 };
    size_t  numBytes, numTableBytes, offsetTable, offsetIndex;
    AllocateBytes    Alloc;     // Auto delete[]
    BITMAPINFOHEADER bmih;
    BITMAPFILEHEADER bmfh;

    Reset();
    m_FileBytes = FileBytes;

    if ( hdc == nullptr )
    {
        throw ParameterException( L"hdc", __FUNCTION__ );
    }

    // Input must be big enough for BITMAPFILEHEADER + BITMAPINFOHEADER
    pBytes   = FileBytes.GetPtr();
    numBytes = FileBytes.GetSize();
    if ( ( pBytes == nullptr ) || ( PXSAddSizeT( sizeof ( b14 ), sizeof ( b40 ) ) > numBytes ) )
    {
        throw SystemException( ERROR_INSUFFICIENT_BUFFER,
                               L"BITMAPFILEHEADER/BITMAPINFOHEADER", __FUNCTION__ );
    }

    // Extract BITMAPFILEHEADER
    FileBytes.Get( 0, b14, sizeof ( b14 ) );
    memset( &bmfh, 0, sizeof ( bmfh ) );
    bmfh.bfType      = PXSMakeUInt16( b14[  0 ], b14[  1 ]                       );
    bmfh.bfSize      = PXSMakeUInt32( b14[  2 ], b14[  3 ], b14[  4 ], b14[  5 ] );
    bmfh.bfReserved1 = PXSMakeUInt16( b14[  6 ], b14[  7 ]                       );
    bmfh.bfReserved2 = PXSMakeUInt16( b14[  8 ], b14[  9 ]                       );
    bmfh.bfOffBits   = PXSMakeUInt32( b14[ 10 ], b14[ 11 ], b14[ 12 ], b14[ 13 ] );
    if ( ( bmfh.bfType    != 0x4d42   ) ||  // Magic 'BM'
         ( bmfh.bfSize    >  numBytes ) ||
         ( bmfh.bfOffBits >  numBytes )  )
    {
        PXSLogAppError( L"Invalid bitmap BITMAPFILEHEADER data." );
        throw SystemException( ERROR_INVALID_DATA, L"BITMAPFILEHEADER", __FUNCTION__ );
    }

    // Extract BITMAPINFOHEADER
    FileBytes.Get( SIZE_OF_BITMAPFILEHEADER, b40, sizeof ( b40 ) );
    memset( &bmih, 0, sizeof ( bmih ) );
    FillBitmapInfoHeader( b40, sizeof( b40 ), &bmih );
    if ( ( bmih.biSize    != SIZE_OF_BITMAPINFOHEADER ) ||
         ( bmih.biClrUsed != 0 ) ||
         ( bmih.biPlanes  != 1 )  )
    {
        PXSLogAppErrorUInt32_2( L"Unsupported BITMAPINFOHEADER: biSize=%%1, biClrUsed=%%2.",
                                bmih.biSize, bmih.biClrUsed );
        throw ParameterException( L"biSize/biClrUsed", __FUNCTION__ );
    }
    numTableBytes = GetNumColourTableBytes( bmih );
    offsetTable   = PXSAddSizeT( sizeof ( b14 ), sizeof ( b40 ) );      // Start of colour table
    offsetIndex   = PXSAddSizeT( offsetTable, numTableBytes );          // start of colour indices
    if ( offsetIndex != bmfh.bfOffBits )
    {
        PXSLogAppErrorSizeT_2( L"Offset values mismatch: offsetIndex = %%1, bfOffBits = %%2",
                               offsetIndex, bmfh.bfOffBits );
        throw BoundsException( L"bmfh.bfOffBits", __FUNCTION__ );
    }
    m_hBitmap = CreateDIBitmap( hdc,
                                &bmih,
                                CBM_INIT,
                                pBytes + bmfh.bfOffBits,
                                reinterpret_cast< const BITMAPINFO* >( pBytes + sizeof ( b14 ) ),
                                DIB_RGB_COLORS );
    if ( m_hBitmap == nullptr )
    {
        throw SystemException( GetLastError(), L"CreateDIBitmap", __FUNCTION__ );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// Protected Methods
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
// Private Methods
///////////////////////////////////////////////////////////////////////////////////////////////////

//===============================================================================================//
//  Description:
//      Fill the BITMAPINFOHEADER using the specified on-disk bytes
//
//  Parameters:
//      pB40     - the buffer
//      numBytes - size of the buffer must be the size of BITMAPINFOHEADER
//      pBmih    - receives the members values
//
//  Remarks:
//      Only supports BITMAPINFOHEADER not  BITMAPV4HEADER nor BITMAPV5HEADER
//
//  Returns:
//      void
//===============================================================================================//
void WindowsBitmap::FillBitmapInfoHeader( const BYTE* pB40,
                                          size_t numBytes, BITMAPINFOHEADER* pBmih )
{
    if ( ( pB40 == nullptr ) || ( numBytes != SIZE_OF_BITMAPINFOHEADER ) || ( pBmih == nullptr ) )
    {
        throw ParameterException( L"pB40/numBytes/pBmih", __FUNCTION__ );
    }
    memset( pBmih, 0, sizeof ( *pBmih) );

    pBmih->biSize          = PXSMakeUInt32( pB40[  0 ], pB40[  1 ], pB40[  2 ], pB40[  3 ] );
    pBmih->biWidth         = PXSMakeInt32 ( pB40[  4 ], pB40[  5 ], pB40[  6 ], pB40[  7 ] );
    pBmih->biHeight        = PXSMakeInt32 ( pB40[  8 ], pB40[  9 ], pB40[ 10 ], pB40[ 11 ] );
    pBmih->biPlanes        = PXSMakeUInt16( pB40[ 12 ], pB40[ 13 ]                         );
    pBmih->biBitCount      = PXSMakeUInt16( pB40[ 14 ], pB40[ 15 ]                         );
    pBmih->biCompression   = PXSMakeUInt32( pB40[ 16 ], pB40[ 17 ], pB40[ 18 ], pB40[ 19 ] );
    pBmih->biSizeImage     = PXSMakeUInt32( pB40[ 20 ], pB40[ 21 ], pB40[ 22 ], pB40[ 23 ] );
    pBmih->biXPelsPerMeter = PXSMakeInt32 ( pB40[ 24 ], pB40[ 25 ], pB40[ 26 ], pB40[ 27 ] );
    pBmih->biYPelsPerMeter = PXSMakeInt32 ( pB40[ 28 ], pB40[ 29 ], pB40[ 30 ], pB40[ 31 ] );
    pBmih->biClrUsed       = PXSMakeUInt32( pB40[ 32 ], pB40[ 33 ], pB40[ 34 ], pB40[ 35 ] );
    pBmih->biClrImportant  = PXSMakeUInt32( pB40[ 36 ], pB40[ 37 ], pB40[ 38 ], pB40[ 39 ] );
}

//===============================================================================================//
//  Description:
//      Determine the number of bytes of colour table data from the specified image header
//
//  Parameters:
//      bmih - the bitmap image header
//
//  Remarks:
//      The number of bytes in the colour table depends on the biBitCount, biClrUsed and
//      biCompression. For our purposes, biClrUsed should be zero as if the BITMAPINFOHEADER was
//      read from a device independent file, pallete information is not stored. Likewise, if have
//      obtained BITMAPINFOHEADER from a HBITMAP, optimsed pallete colours are only for VGA
//      monitors, i.e <= 256 colours.
//
//  Returns:
//      Number of bytes of data
//===============================================================================================//
DWORD WindowsBitmap::GetNumColourTableBytes( const BITMAPINFOHEADER& bmih )
{
    DWORD numBytes = 0;

    if ( ( bmih.biSize != SIZE_OF_BITMAPINFOHEADER ) || ( bmih.biClrUsed != 0 ) )
    {
        PXSLogAppErrorUInt32_2( L"Unsupported BITMAPINFOHEADER: biSize=%%1, biClrUsed=%%2.",
                                bmih.biSize, bmih.biClrUsed );
        throw ParameterException( L"biSize/biClrUsed", __FUNCTION__ );
    }

    // Because biClrUsed = 0, the number of colours in the colour table is the maximum allowed
    // for the pixel depth and compression
    switch ( bmih.biBitCount )
    {
        default:
            throw ParameterException( L"bmih.biBitCount", __FUNCTION__ );

        case 1:     // Monochrome
            if ( bmih.biCompression == BI_RGB )
            {
                numBytes = 2 * sizeof ( RGBQUAD );
            }
            else
            {
                throw ParameterException( L"bmih.biCompression", __FUNCTION__ );
            }
            break;

        case 4:     // 16-colour
            if ( ( bmih.biCompression == BI_RGB ) || ( bmih.biCompression == BI_RLE4 ) )
            {
                numBytes = 16 * sizeof ( RGBQUAD );
            }
            else
            {
                throw ParameterException( L"bmih.biCompression", __FUNCTION__ );
            }
            break;

        case 8:     // 256-colour
            if ( ( bmih.biCompression == BI_RGB ) || ( bmih.biCompression == BI_RLE8 ) )
            {
                numBytes = 256 * sizeof ( RGBQUAD );
            }
            else
            {
                throw ParameterException( L"bmih.biCompression", __FUNCTION__ );
            }
            break;

        // Fall through
        case 16:    // 64K-colour
        case 32:
            if ( bmih.biCompression == BI_RGB )
            {
                numBytes = 0;                       // no colour table
            }
            else if ( bmih.biCompression == BI_BITFIELDS )
            {
                numBytes = 3 * sizeof ( DWORD );    // 3 DWORD colour mask
            }
            else
            {
                throw ParameterException( L"bmih.biCompression", __FUNCTION__ );
            }
            break;

        case 24:
            if ( bmih.biCompression == BI_RGB )
            {
                numBytes = 0;       // no colour table
            }
            else
            {
                throw ParameterException( L"bmih.biCompression", __FUNCTION__ );
            }
            break;
    }

    return numBytes;
}

//===============================================================================================//
//  Description:
//      Get this bitmap as bytes to store to a file (.bmp) using the colour depth of
//      the specified device context
//
//  Parameters:
//      hdc - the device context
//
//  Remarks:
//      Bitmap file structure is as follows:
//      BITMAPFILEHEADER
//      BITMAPINFO
//          BITMAPINFOHEADER
//          Colour Table when biClrUsed = 0
///             3 DWORD mask for 16 and 32-bit bitmaps and BI_BITFIELDS
//              RGBQUAD pallet for 1, 4 and 8 bit bitmaps
//      Color-index array
//
//  Returns:
//      void
//===============================================================================================//
void WindowsBitmap::ToFileBytes( HDC hdc )
{
    BYTE*      pFileBytes;
    DWORD      numFileBytes, numTableBytes;
    BITMAPINFO bmInfo;
    AllocateBytes      Alloc;
    BITMAPFILEHEADER*  pBmfh;

    // Reset
    m_FileBytes.Zero();
    if ( m_hBitmap == nullptr )
    {
        return;     // Nothing to do
    }

    // Get the image's info by calling GetDIBits() with a NULL buffer. Want biClrUsed
    // to be zero as should not store the colour table in a file
    memset( &bmInfo , 0 , sizeof ( bmInfo ) );
    bmInfo.bmiHeader.biSize = sizeof ( BITMAPINFOHEADER );
    GetDIBits( hdc, m_hBitmap, 0, 0, nullptr, &bmInfo, DIB_RGB_COLORS );
    if ( ( bmInfo.bmiHeader.biSize    != SIZE_OF_BITMAPINFOHEADER ) ||
         ( bmInfo.bmiHeader.biClrUsed != 0 ) )
    {
        PXSLogAppErrorUInt32_2( L"Unsupported BITMAPINFOHEADER: biSize=%%1, biClrUsed=%%2.",
                                bmInfo.bmiHeader.biSize, bmInfo.bmiHeader.biClrUsed );
        throw ParameterException( L"biSize/biClrUsed", __FUNCTION__ );
    }

    // Alocate bytes for the file buffer
    if ( bmInfo.bmiHeader.biSizeImage == 0 )
    {
        return;     // Nothing to do
    }
    numTableBytes = GetNumColourTableBytes( bmInfo.bmiHeader );
    numFileBytes  = PXSAddUInt32_4( SIZE_OF_BITMAPFILEHEADER,
                                    SIZE_OF_BITMAPINFOHEADER,
                                    numTableBytes,
                                    bmInfo.bmiHeader.biSizeImage );
    pFileBytes = Alloc.New( numFileBytes );

    // Fill the BITMAPFILEHEADER structure
    pBmfh = reinterpret_cast< BITMAPFILEHEADER* >( pFileBytes );
    pBmfh->bfType      = 0x4d42;      // 0x42 = "B" 0x4d = "M"
    pBmfh->bfSize      = numFileBytes;
    pBmfh->bfReserved1 = 0;
    pBmfh->bfReserved2 = 0;
    pBmfh->bfOffBits   = PXSAddUInt32_3( SIZE_OF_BITMAPFILEHEADER,
                                         SIZE_OF_BITMAPINFOHEADER,
                                         numTableBytes );
    // The BITMAPINFOHEADER goes immediately after BITMAPFILEHEADER
    memcpy( pFileBytes + SIZE_OF_BITMAPFILEHEADER, &bmInfo.bmiHeader, SIZE_OF_BITMAPINFOHEADER );

    // Second call to GetDIBits to get the colour index bits and the color table
    if ( GetDIBits( hdc,
                    m_hBitmap,
                    0,
                    static_cast< UINT >( bmInfo.bmiHeader.biHeight ),
                    pFileBytes + pBmfh->bfOffBits,
                    reinterpret_cast< BITMAPINFO* >( pFileBytes + SIZE_OF_BITMAPFILEHEADER ),
                    DIB_RGB_COLORS ) == 0 )
    {
        throw SystemException( GetLastError(), L"GetDIBits", __FUNCTION__ );
    }
    m_FileBytes.Append( pFileBytes, numFileBytes );
}
