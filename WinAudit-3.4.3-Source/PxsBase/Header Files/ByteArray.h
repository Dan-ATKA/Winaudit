///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Byte Array Class Header
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

#ifndef PXSBASE_BYTE_ARRAY_H_
#define PXSBASE_BYTE_ARRAY_H_

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

// 6. Forwards
class CharArray;

///////////////////////////////////////////////////////////////////////////////////////////////////
// Interface
///////////////////////////////////////////////////////////////////////////////////////////////////

class ByteArray
{
    public:
        // Default constructor
        ByteArray();

        // Copy constructor
        ByteArray( const ByteArray& oByteArray );

        // Assignment operator
        ByteArray& operator= ( const ByteArray& oByteArray );

        // Destructor
        ~ByteArray();

        // Methods
        void    Allocate( size_t numBytes );
        void    Append( const ByteArray& Bytes );
        void    Append( const BYTE* pBuffer, size_t numBytes );
        void    AppendByte( BYTE byte );
        void    AppendChars( const CharArray& Chars );
        void    AppendChars( const char* pChars, size_t numChars );
        bool    BeginsWith( const BYTE* pBuffer, size_t numBytes ) const;
        int     Compare( size_t offset, const ByteArray& Bytes ) const;
        int     Compare( size_t offset, const BYTE* pBuffer, size_t numBytes ) const;
        int     Compare( size_t offset, const char* pChars, size_t numChars ) const;
        bool    EndsWith( BYTE b ) const;
        bool    EndsWith( const BYTE* pBuffer, size_t numBytes ) const;
        void    Free();
        BYTE    Get( size_t idx ) const;
        size_t  Get( size_t offset, BYTE* pBuffer, size_t bufBytes ) const;
        void    Get( size_t offset, ByteArray* pBytes ) const;
        void    Get( size_t offset, size_t numBytes, ByteArray* pBytes ) const;
        size_t  GetNumAllocated() const;
    const BYTE* GetPtr() const;
        size_t  GetSize() const;
        size_t  IndexOf( size_t offset, const BYTE* pSearch, size_t numBytes ) const;
        size_t  IndexOf( size_t offset,
                         const char* pSearch, size_t numChars, bool caseSensitive ) const;
        bool    IsAt( size_t index, BYTE b ) const;
        void    LeftShift( size_t count );
        void    Prepend( BYTE* pBuffer, size_t numBytes );
        void    Truncate( size_t size );
        void    Zero();

    protected:
        // Methods

        // Data members

    private:
        // Methods

        // Data members
        size_t   GROW_BY;           // psuedo-constant
        size_t   m_uSize;
        size_t   m_uAllocated;
        BYTE*    m_pBytes;
};

#endif  // PXSBASE_BYTE_ARRAY_H_
