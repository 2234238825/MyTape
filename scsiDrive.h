//
// Created by Admin on 2024/10/11.
//

#ifndef UNTITLED13_SCSIDRIVE_H
#define UNTITLED13_SCSIDRIVE_H
/*
 * This is used to excute scsi commond and reponse data, other more complex operations in another source file
 *
 * */
#include "scsiStruct.h"
#include "kfcStruct.h"
#include "ErrorCode.h"
#include "Log.h"
using namespace std;

class CScsiDrive
{
public:
    static void print_sense_buffer(unsigned char* sense_buffer, int len);
    CScsiDrive()
    {
        memset(&m_CommandStruct,0,sizeof(m_CommandStruct));
        m_tapeInfo = nullptr;
    }
    int setDeviceCommandStructAndDeviceInfo(SCSI_COMMAND_STRUCT commandStruct,DEVICE_INFO deviceInfo);
    int set_scsi_block_size(unsigned int block_size = 65536);
    int scsi_write_fileMarks();
    int scsi_test_unit_ready();
    int scsi_space_blocks(int ulBlocks);
    int scsi_space_fileMarks(unsigned int ulnum);
    int scsi_read_pos(int &ulposition);
    int get_drive_block_size(int *block_size);
    int write_block(char *buffer, int blocks);
    int read_block(char *buffer, int blocks);
    int setSerialNumber();
    int seek_block();
    int load();
    int unload;
    int rewind();
    void set_device_path(char *path);
    const char * getDriveSerialNumber();
    const char * getDrivePath();
    TapeInfo* getTapeInfo();
    void setTapeInfo(TapeInfo* tapeInfo);
    void setAddr(int addr);
    int getAddr();
private:
    int print_response_data(int cdb_code,int page_code = 0);
    int new_buffer(int buffer_size);
    int delete_buffer();
private:
    SCSI_COMMAND_STRUCT m_CommandStruct;
    DEVICE_INFO m_DeviceInfo;
    TapeInfo* m_tapeInfo;
    char m_SerialNumber[SERIAL_NO_LENGTH];
    char m_DrivePath[DEVICE_PATH];
    int m_DriveAddr;
};


#endif //UNTITLED13_SCSIDRIVE_H
