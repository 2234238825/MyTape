//
// Created by Admin on 2024/10/15.
//

#ifndef UNTITLED13_KFCSTRUCT_H
#define UNTITLED13_KFCSTRUCT_H
#include <cstring>
#include <sys/stat.h>
#include <unistd.h>
#include "OSAPI.h"
#include "scsiStruct.h"
using namespace std;
#define WRITE_BUFFER_SIZE 65536   // 定义写缓冲区大小
#define SCSI_WRITE_6_COMMAND 0x0A  // SCSI 6字节WRITE命令的操作码
#define SENSE_BUFFER_LEN 32      // Sense Buffer 长度
#define READ_BUFFER_SIZE 65536    // 定义读缓冲区大小
#define SCSI_READ_6_COMMAND 0x08  // SCSI 6字节READ命令的操作码

#define HEADER_LEN (64*1024)
#define APP_DEF_LEN (32*1024)
#define FILE_PATH_LEN 1024

#define BUF_SIZE (2*1024*1024)

/*!
 *  @note union 的所有成员共享同一段内存，大小由其最大的成员决定。cDummy[HEADER_LEN] 提供了一种固定大小的表示方式，便于在网络通信、文件存储或内存映射时对齐结构体。\n
 *  保持头部信息的固定大小，无论其字段具体定义如何。
 *
 */
union TapeHeader
{

    struct
    {
        char szTapeName[TAPENAME_LEN];
        int iSequence;
        int iSignature;
        long long llWriteTime;
        long long llExpireTime;
    };
    char cDummy[HEADER_LEN];
    string toString() const
    {
        return "MyStruct { szTapeName: " + std::string(szTapeName) +
        ", iSequence: " + std::to_string(iSequence) +
        ", iSignature: " + std::to_string(iSignature) +
        ", llWriteTime: " + std::to_string(llWriteTime)+
        ", llExpireTime: " + std::to_string(llExpireTime)+
        " }";
    }
    friend  ostream& operator<<(std::ostream& os, const TapeHeader& s) {
        os << "MyStruct { szTapeName: " << s.szTapeName << ", iSequence: " << s.iSequence << ", iSignature: " << s.iSignature << ", llWriteTime: " << s.llWriteTime<<" }";
        return os;
    }

};

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

struct FileHeader
{
    int iSignature;
    char szFileName[FILE_PATH_LEN];
    LONGLONG llFileLength;
    char Attributs[1];
};

struct DEVICE_INFO
{
    char vendorID[VENDOR_ID_LENGTH];
    char productID[PRODUCT_ID_LENGTH];
    char productRevision[FIRMWARE_LENGTH];

    DEVICE_INFO()
    {
        vendorID[0] = 0;
        productID[0] = 0;
        productRevision[0] = 0;
    };
};

union SessionHeader
{
    struct
    {
        int iSignature;
        char szRootpath[FILE_PATH_LEN];
        LONGLONG llBackupTime;
        int iFileAttrLen;
        char cAppDef[APP_DEF_LEN];//upper application define
    };

    char cDummy[HEADER_LEN];
};

struct STANDARD_INQUIRY_RESPONSE
{
    char deviceType;
    char deviceInfo;
    char compliance;
    char reserved_1;
    char additionalLength;
    char reserved_2[3];
    char vendorID[VENDOR_ID_LENGTH];
    char productID[PRODUCT_ID_LENGTH];
    char firmwareRevisionLevel[FIRMWARE_LENGTH];
    char reserved_3[5];
    char personality;
    char reserved[26];
};

struct CatalogHeader
{
    int iSignature;
    char szRootpath[FILE_PATH_LEN];
    LONGLONG llBackupTime;
    int iSequence;
    int iAttrLen;
    int ulStartBlock;
    char cAppDef[APP_DEF_LEN];//upper application define
};

struct FileInfo
{
    char szFileName[FILE_PATH_LEN];
    LONGLONG llLength;
    LONGLONG llOffset;
};

struct DEV_DIS_INFO
{
    char szDeviceSerialNumber[MAX_SERIAL_NUM_LENGTH];
    int iDeviceType;             //Tape, Changer,etc
    DEVICE_INFO primaryInquiryData;
    DEVICE_INFO secondaryInquiryData;
    char szDevicePath[DEVICE_PATH];
    char szStPath[DEVICE_PATH];
    int ucAdapter;
    int ucBus;
    int ucSCSIID;
    int ucLun;
    int devIdAcslsACS;
    int devId3494;
    int connectivity;
    char ucCompliance;
    char bLibrary;//this drive belong library or standalone
    char bRegisted;
};

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
