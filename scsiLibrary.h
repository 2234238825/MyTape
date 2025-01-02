//
// Created by Admin on 2024/10/11.
//

#ifndef UNTITLED13_SCSILIBRARY_H
#define UNTITLED13_SCSILIBRARY_H
#include "myFuntion.h"
#include <cstdlib>
#include "scsiStruct.h"
#include "kfcStruct.h"
#include <cstring>
#include "ErrorCode.h"
#include "Log.h"
#include "scsiDrive.h"
using namespace std;


struct ELEMENT_STRUCT
{
    int addr_drive;
    int num_drive;
    int addr_slot;
    int num_slot;
    int addr_ie;
    int num_ie;
    int addr_trans;
    int num_trans;
};

struct DEVICE_STATUS
{
    ELEMENT_STATUS_HEADER *status_header;
    ELEMENT_STATUS_PAGE *page_header;
    ELEMENT_DESCRIPTOR *descriptor;
};

class CScsiLibrary
{
public:
    CScsiLibrary()
    {
        memset(&m_CommandStruct,0,sizeof(m_CommandStruct));
        //memcpy(m_CommandStruct.devicePath, "/dev/sg2",260);
    }

    int setDeviceCommandStructAndDeviceInfo(SCSI_COMMAND_STRUCT commandStruct,DEVICE_INFO deviceInfo);

    int read_element_status(char element_type,int starting_element_address,int num_elements, char CDB_byte_6, bool voltag_bit);
    int get_library_all_elements();
    int move_medium(int source_element,int destination_element, int iFlag);
    ELEMENT_STRUCT get_element_struct(){return m_ElementStruct;}
    const char* get_response_buffer();
    int get_query(unsigned char page_code);
    void setCScsiDrive(CScsiDrive* drive);

    int get_slot_count();
    int get_drive_count();
    int get_IE_slot_count();
    stDriveInfo *getDriveInfo(int iIndexDrive);
    CScsiDrive * getDrive(int iIndexDrive);
    const char* GetLibrarySerialNumber();

    int initAllSlot();
private:
    void print_sense_buffer(unsigned char* sense_buffer, int len);
    int new_buffer(int buffer_size);
    int delete_buffer();

    int print_response_data(int cdb_code,int page_code = 0);
    int mode_sense(int page_code,int DBD_bit);
    int mode_select();
private:
    SCSI_COMMAND_STRUCT m_CommandStruct;
    ELEMENT_STRUCT m_ElementStruct;
    DEVICE_STATUS m_DeviceStatus;
    vector<CScsiDrive *> m_Drives;

    DEVICE_INFO m_DeviceInfo;
    char m_LibrarySerialNumber[SERIAL_NO_LENGTH];
};


#endif //UNTITLED13_SCSILIBRARY_H
