///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Securable Objects Permission Information Class Header
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

#ifndef WINAUDIT_OBJECT_PERMISSION_INFORMATION_H_
#define WINAUDIT_OBJECT_PERMISSION_INFORMATION_H_

///////////////////////////////////////////////////////////////////////////////////////////////////
// Remarks
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
// Includes
///////////////////////////////////////////////////////////////////////////////////////////////////

// 1. Own Interface
#include "WinAudit/Header Files/WinAudit.h"

// 2. C System Files
#include <AccCtrl.h>

// 3. C++ System Files

// 4. Other Libraries

// 5. This Project

// 6. Forwards
class AuditRecord;
template< class T > class TArray;

///////////////////////////////////////////////////////////////////////////////////////////////////
// Interface
///////////////////////////////////////////////////////////////////////////////////////////////////

class ObjectPermissionInformation
{
    public:
        // Default constructor
        ObjectPermissionInformation();

        // Destructor
        ~ObjectPermissionInformation();

        // Methods
 static void GetAuditRecords( TArray< AuditRecord >* pRecords );

    protected:
        // Methods

        // Data members

    private:
        // Copy constructor - not allowed
        ObjectPermissionInformation( const ObjectPermissionInformation& oInformation );

        // Assignment operator - not allowed
        ObjectPermissionInformation& operator= ( const ObjectPermissionInformation& oInformation );

        // Methods

 static void GetAccountNameFromSid( const PSID pSid, String* pAccountName);
 static void GetDesktopSecurity( PSECURITY_DESCRIPTOR* ppSecurityDescriptor );
 static void GetNonWindowSecurity( const String& ObjectName,
                                   SE_OBJECT_TYPE objectType,
                                   PSECURITY_DESCRIPTOR* ppSecurityDescriptor);
 static void GetObjectPermissionRecords( const String& ObjectName,
                                         SE_OBJECT_TYPE objectType,
                                         TArray< AuditRecord >* pRecords );
 static void GetWorkStationSecurity( PSECURITY_DESCRIPTOR* ppSecurityDescriptor );
 static bool HasAllAccess( ACCESS_MASK accessMask, SE_OBJECT_TYPE type, bool workStation );
 static void MakeObjectPermissionRecords( const String& ObjectName,
                                          SE_OBJECT_TYPE type,
                                          bool workStation,
                                          PSECURITY_DESCRIPTOR pSecurityDescriptor,
                                          TArray< AuditRecord >* pRecords );
 static void TranslateAccessMask( ACCESS_MASK accessMask,
                                  SE_OBJECT_TYPE type,
                                  bool workStation, String* pTranslation );
 static void TranslateAceFlags( LONG aceFlags, String* pTranslation );
 static void TranslateAceType( LONG aceType, String* pTranslation );
 static void TranslateAdsSpecificRights( DWORD specificRights, String* pTranslation );
 static void TranslateDesktopSpecificRights( DWORD specificRights, String* pTranslation );
 static void TranslateFileSpecificRights( DWORD specificRights, String* pTranslation );
 static void TranslateGenericRights( DWORD genericRights, String* pTranslation );
 static void TranslateObjectType( SE_OBJECT_TYPE type, String* pTranslation );
 static void TranslatePrinterSpecificRights( DWORD specificRights, String* pTranslation );
 static void TranslateServiceSpecificRights( DWORD specificRights, String* pTranslation );
 static void TranslateSpecificRights( DWORD specificRights,
                                      SE_OBJECT_TYPE type,
                                      bool workStation, String* pTranslation );
 static void TranslateStandardRights( DWORD standardRights, String* pTranslation );
 static void TranslateWorkstationSpecificRights( DWORD specificRights, String* pTranslation );

        // Data members
};

#endif  // WINAUDIT_OBJECT_PERMISSION_INFORMATION_H_
