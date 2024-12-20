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
using namespace std;
#define MODE_SELECT_CMD 0x15    // SCSI MODE SELECT 命令操作码
#define WRITE_BUFFER_SIZE 65536   // 定义写缓冲区大小
#define SCSI_WRITE_6_COMMAND 0x0A  // SCSI 6字节WRITE命令的操作码
#define SENSE_BUFFER_LEN 32      // Sense Buffer 长度
#define READ_BUFFER_SIZE 65536    // 定义读缓冲区大小
#define SCSI_READ_6_COMMAND 0x08  // SCSI 6字节READ命令的操作码


class CScsiDrive
{
public:
    static void print_sense_buffer(unsigned char* sense_buffer, int len);
    CScsiDrive()
    {
        memset(&m_CommandStruct,0,sizeof(m_CommandStruct));
    }
    int setDeviceCommandStructAndDeviceInfo(SCSI_COMMAND_STRUCT commandStruct,DEVICE_INFO deviceInfo);
    int set_scsi_block_size(unsigned int block_size = 65536);
    int get_drive_block_size(int *block_size);
    int write_block(char *buffer,int blocks);
    int read_block(char *buffer, int blocks);
    int get_query();
    int read_positon();
    int seek_block();
    int load();
    int unload;
    int rewind();
    void set_device_path(char *path);
    const char * getDriveSerialNumber();
    const char * getDrivePath();
private:
    int print_response_data(int cdb_code,int page_code = 0);
    int new_buffer(int buffer_size);
    int delete_buffer();
private:
    SCSI_COMMAND_STRUCT m_CommandStruct;
    DEVICE_INFO m_DeviceInfo;
    vector<void *> mDrive;
    char m_SerialNumber[SERIAL_NO_LENGTH];
    char m_DrivePath[DEVICE_PATH];
};


#endif //UNTITLED13_SCSIDRIVE_H
