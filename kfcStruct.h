//
// Created by Admin on 2024/10/15.
//

#ifndef UNTITLED13_KFCSTRUCT_H
#define UNTITLED13_KFCSTRUCT_H
#include "scsiStruct.h"

struct SCSI_COMMAND_STRUCT
{
    char devicePath[DEVICE_PATH];
    int deviceType;
    char *responseBuffer;
    int responseBufferLength;
    int logicalUnitNumber;
    int scsiId;
    int Bus;
    int Adapter;
};

struct DEVICE_INFO
{
    char    vendorID[VENDOR_ID_LENGTH];
    char    productID[PRODUCT_ID_LENGTH ];
    char    productRevision[FIRMWARE_LENGTH];

    DEVICE_INFO()
    {
        vendorID[0] = 0;
        productID[0] = 0;
        productRevision[0] = 0;
    };
} ;

struct STANDARD_INQUIRY_RESPONSE
{
char    deviceType;
char    deviceInfo;
char    compliance;
char    reserved_1;
char    additionalLength;
char    reserved_2[3];
char   vendorID[ VENDOR_ID_LENGTH ];
char   productID[ PRODUCT_ID_LENGTH ];
char   firmwareRevisionLevel[ FIRMWARE_LENGTH ];
char		reserved_3[ 5 ];
char		personality;
char    reserved[ 26 ];
} ;

struct DEV_DIS_INFO
{
    char          szDeviceSerialNumber[MAX_SERIAL_NUM_LENGTH];
    int           iDeviceType;             //Tape, Changer,etc
    DEVICE_INFO   primaryInquiryData;
    DEVICE_INFO   secondaryInquiryData;
    char          szDevicePath[DEVICE_PATH];
    char          szStPath[DEVICE_PATH];
    int           ucAdapter;
    int           ucBus;
    int           ucSCSIID;
    int           ucLun;
    int 		  devIdAcslsACS;
    int			  devId3494;
    int			  connectivity;
    char          ucCompliance;
    char			  bLibrary;//this drive belong library or standalone
    char			  bRegisted;
} ;

struct TapeInfo
{
    char iVersion;
    char szBarCode[BARCODE_LENGTH];
    char szTapeName[TAPENAME_LEN];
    char Owner[OWNER_NAME_LEN];
    int iSequence;
    long long llExpireTime;
    int iFlag;
    long long llDataSize;
    char bFreeze;
    char bWriteProtected;
    char bFull;
    int iMoveTimes;
    long long llLastWriteTime;
    char bReserved;
    int iCatalogId;
    char MediumType[OWNER_NAME_LEN];
};

union TapeInfoFile
{
    TapeInfo Info;

    char Revserve[8 * 1024];
};

union stDriveInfo
{
    struct
    {
        int iStatus;//active, up, down

        bool bEmpty;

        int iError;
        long long llLastWriteTime;
        char szSerialNo[MAX_SERIAL_NUM_LENGTH];
        char szPath[DEVICE_PATH];
        char NodeId[NODE_ID_LEN];
        bool bVirtual;
    };
    char Revserve[8 * 1024];
};

#endif //UNTITLED13_KFCSTRUCT_H
