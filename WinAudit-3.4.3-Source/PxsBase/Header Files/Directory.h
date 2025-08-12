///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Directory Class Header
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

#ifndef PXSBASE_DIRECTORY_H_
#define PXSBASE_DIRECTORY_H_

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
class FilePathSizeDateList;
class String;
class StringArray;

///////////////////////////////////////////////////////////////////////////////////////////////////
// Interface
///////////////////////////////////////////////////////////////////////////////////////////////////

class Directory
{
    public:
        // Default constructor
        Directory();

        // Destructor
        ~Directory();

        // Methods
 static void    CreateNew( const String& Path );
 static void    Delete( const String& Path );
 static bool    Exists( const String& Path );
 static void    GetSpecialDirectory( int nFolder, String* pDirectoryPath );
 static void    GetTempDirectory( String* pTempDirectory );
 static bool    IsOnLocalDrive( const String& Path );
 static void    ListFiles( const String& Path, const String& DotExt, StringArray* pFileNames );
 static void    SplitPath( const String& Path,
                           String* pDrive, String* pDir, String* pFname, String* pExt );

    protected:
        // Methods

        // Data members

    private:
        // Copy constructor - not allowed
        Directory( const Directory& oDirectory );

        // Assignment operator - not allowed
        Directory& operator= ( const Directory& oDirectory );

        // Methods

        // Data members
};

#endif  // PXSBASE_DIRECTORY_H_
