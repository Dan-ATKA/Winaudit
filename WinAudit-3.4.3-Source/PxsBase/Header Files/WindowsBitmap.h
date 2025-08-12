///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Windows Bitmap Class Header
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

#ifndef PXSBASE_WINDOWS_BITMAP_H_
#define PXSBASE_WINDOWS_BITMAP_H_

///////////////////////////////////////////////////////////////////////////////////////////////////
// Remarks
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
// Includes
///////////////////////////////////////////////////////////////////////////////////////////////////

// 1. Own Interface
#include "PxsBase/Header Files/PxsBase.h"

// 2. C System Files

// 3. C++ System Files

// 4. Other Libraries

// 5. This Project
#include "PxsBase/Header Files/ByteArray.h"

// 6. Forwards

///////////////////////////////////////////////////////////////////////////////////////////////////
// Interface
///////////////////////////////////////////////////////////////////////////////////////////////////

class WindowsBitmap
{
    public:
        // Default constructor
        WindowsBitmap();

        // Destructor
        ~WindowsBitmap();

        // Methods
        void Draw( HDC hdc );
        void GetFileBytes(  HDC hdc, ByteArray* pFileBytes );
        void GetSize( SIZE* pSize ) const;
        void Reset();
        void Set( HBITMAP hBitmap );
        void SetFileBytes( HDC hdc, const ByteArray& FileBytes );

    protected:
        // Methods

        // Data members

    private:
        // Copy constructor - not allowed
        WindowsBitmap( const WindowsBitmap& oWindowsBitmap );

        // Assignment operator - not allowed
        WindowsBitmap& operator= ( const WindowsBitmap& oWindowsBitmap );

        // Methods
 static void  FillBitmapInfoHeader( const BYTE* pB40, size_t numBytes, BITMAPINFOHEADER* pBmih );
 static DWORD GetNumColourTableBytes( const BITMAPINFOHEADER& bmih );
        void  ToFileBytes( HDC hdc );

        // Data members
        static const DWORD SIZE_OF_BITMAPFILEHEADER = 14;  // On disk bytes for BITMAPFILEHEADER
        static const DWORD SIZE_OF_BITMAPINFOHEADER = 40;  // On disk bytes for BITMAPINFOHEADER
        HBITMAP            m_hBitmap;
        ByteArray          m_FileBytes;
};

#endif  // PXSBASE_WINDOWS_BITMAP_H_
