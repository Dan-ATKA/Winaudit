///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Disk Information Class Header
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

#ifndef WINAUDIT_DISK_INFORMATION_H_
#define WINAUDIT_DISK_INFORMATION_H_

///////////////////////////////////////////////////////////////////////////////////////////////////
// Remarks
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
// Includes
///////////////////////////////////////////////////////////////////////////////////////////////////

// 1. Own Interface
#include "WinAudit/Header Files/WinAudit.h"

// 2. C System Files

// 3. C++ System Files

// 4. Other Libraries

// 5. This Project
#include "WinAudit/Header Files/Ddk.h"

// 6. Forwards
class AuditRecord;
class String;
template< class T > class TArray;

///////////////////////////////////////////////////////////////////////////////////////////////////
// Interface
///////////////////////////////////////////////////////////////////////////////////////////////////

class DiskInformation
{
    public:
        // Default constructor
        DiskInformation();

        // Destructor
        ~DiskInformation();

        // Methods
        void    GetDiagnostics( String* pDiagnostics );
        void    GetAuditRecords( TArray< AuditRecord >* pRecords );

    protected:
        // Data members

        // Methods

    private:
        // Copy constructor - not allowed
        DiskInformation( const DiskInformation& oDiskInformation );

        // Assignment operator - not allowed
        DiskInformation& operator= ( const DiskInformation& oDiskInformation);

        // Methods
        HANDLE CreateDiskHandle( const String& DiskName );
 static void   DiskSerialHexToSerialNumber( const String& SerialHex, String* pSerialNumber );
 static bool   ExtractIddStringValue( const BYTE* pbData,
                                      size_t numBytes, String* pDataString );
 static void   FormatDiskFirmwareDataBlock( const PXSDDK::IDENTIFY_DEVICE_DATA* pIDD,
                                            String* pIddString );
 static void   FormatIddStructureAsString( const PXSDDK::IDENTIFY_DEVICE_DATA* pIDD,
                                           String* pFormattedIdd );
 static void   GetConfigManagerDiskData( String* pConfigManagerDiskData );
        bool   GetDataIdd( BYTE    physicalDrive,
                           WORD*   pBufferKB,
                           String* pSerialNumber,
                           String* pFirmwareRevision, bool* pSmartSupported, bool* pSmartEnabled );
        void   GetDataIoctl( BYTE physicalDrive, String* pSerialNumber, String* pFirmwareRevision );
 static bool   GetDataWMI( BYTE physicalDrive,
                           UINT64* pDiskSizeBytes,
                           UINT64* pTotalCylinders,
                           DWORD* pTotalHeads,
                           DWORD* pSectorsPerTrack,
                           String* pMediaType,
                           String* pManufacturerName,
                           String* pModel, String* pSerialNumber, String* pFirmwareRevision );
 static void   GetDiskPartionInfo( HANDLE hDisk, String* pDiskPartionInfo );
        void   GetDisksAndDevicesData( String* pDisksAndDevicesData );
 static void   GetFixedDiskControllerPosition( BYTE physicalDrive,
                                               String* pRank, String* pMasterSlave );
        bool   GetIDDScsiMiniPort( BYTE physicalDrive, PXSDDK::IDENTIFY_DEVICE_DATA* pIDD );
        bool   GetIDDSmart( BYTE physicalDrive, PXSDDK::IDENTIFY_DEVICE_DATA* pIDD );
        bool   GetIdentifyDeviceData( BYTE physicalDrive, PXSDDK::IDENTIFY_DEVICE_DATA* pIDD );
        void   GetRawDiskData( String* pRawDiskData );
 static void   GetScsiAddressData( HANDLE hDisk, String* pAddressData );
 static void   GetScsiCapabilities( HANDLE hDisk, String* pScsiCapabilities );
 static void   GetScsiInquiryData( HANDLE hDisk, String* pScsiInquiryData );
        bool   GetSmartTestResult( BYTE physicalDrive, String* pSmartTestResult );
 static void   GetStorageAdapterProperty( HANDLE hDisk, String* pAdapterProperty );
 static bool   GetStorageDeviceProperties( HANDLE hDisk,
                                           String* pDeviceType,
                                           String* pDeviceTypeModifier,
                                           String* pRemovableMedia,
                                           String* pVendorId,
                                           String* pProductId,
                                           String* pProductRevision,
                                           String* pSerialNumber,
                                           String* pBusType, String* pErrorMessage );
 static void   GetWmiDiskDriveData( String* pWmiDiskDriveData );
 static bool   PhysDriveNumToDiskNum( BYTE physicalDrive, BYTE* pDiskNumber );
 static void   ResolveManufacturer( const String& Model, String* pManufacturerName );
 static void   TranslateDeviceTypeQualifier( BYTE deviceTypeQualifier, String* pTranslation );
 static void   TranslateIdeDeviceType( UCHAR DeviceType, String* pTranslation );
 static void   TranslatePartitionType( BYTE PartitionType, String* pTranslation );
 static void   TranslateStorageBusType( BYTE BusType, String* pTranslation );

        // Data members
};

#endif  // WINAUDIT_DISK_INFORMATION_H_
