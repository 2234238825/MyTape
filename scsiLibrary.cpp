//
// Created by Admin on 2024/10/11.
//

#include "scsiLibrary.h"
int CScsiLibrary::new_buffer(int buffer_size)
{
    if(m_CommandStruct.responseBufferLength > 0)
        delete_buffer();
    m_CommandStruct.responseBuffer = new char[buffer_size];
    memset(m_CommandStruct.responseBuffer,0,buffer_size);
    m_CommandStruct.responseBufferLength = buffer_size;
    return 0;
}

int CScsiLibrary::delete_buffer()
{
    delete[] m_CommandStruct.responseBuffer;
    m_CommandStruct.responseBufferLength = 0;
    return 0;
}

void CScsiLibrary::print_sense_buffer(unsigned char *sense_buffer, int len)
{
    std::cout << "Sense Buffer: sg_decode_sense ";
    for (int i = 0; i < len; ++i) {
        std::cout << std::hex << (int)sense_buffer[i] << " ";
    }
    std::cout << std::dec << std::endl;
}

/*!
 *
 * @param element_type 0x00 All element types reported, 0x01 Medium transport element (accessor), 0x02 Storage element, 0x03 Import/Export element, 0x04 Data transfer element
 * @param starting_element_address
 * @param num_elements
 * @param CDB_byte_6
 * @param voltag_bit
 * @return
 */
int
CScsiLibrary::read_element_status(char element_type, int starting_element_address, int num_elements, char CDB_byte_6, bool voltag_bit)
{
    new_buffer(20000);

    int fd = open(m_CommandStruct.devicePath, O_RDWR);
    if (fd < 0) {
        perror("Failed to open tape device");
        return -1;
    }

    struct sg_io_hdr io_hdr;
    unsigned char cdb[12]; // MODE SELECT(6) 命令格式
    unsigned char sense_buffer[32];

    memset(&io_hdr, 0, sizeof(io_hdr));
    memset(cdb, 0, sizeof(cdb));
    memset(sense_buffer, 0, sizeof(sense_buffer));

    cdb[0] = 0xB8;  // REPORT ELEMENT STATUS 操作码
    cdb[1] = 0;
    if(voltag_bit == true)
        cdb[1] |= RE_VOLTAG_BIT;

    cdb[1] |= element_type;

    cdb[2] = (unsigned char) (starting_element_address >> 8);
    cdb[3] = (unsigned char) (starting_element_address);

    cdb[4] = (unsigned char) (num_elements >> 8);
    cdb[5] = (unsigned char) (num_elements);

    if((CDB_byte_6 & 0x80) != 0x80)
    {
        cdb[6] |= CDB_byte_6;
    }
    cdb[7] = m_CommandStruct.responseBufferLength>>16;
    cdb[8] = m_CommandStruct.responseBufferLength>>8;
    cdb[9] = m_CommandStruct.responseBufferLength;

    io_hdr.interface_id = 'S';
    io_hdr.cmd_len = sizeof(cdb);
    io_hdr.mx_sb_len = sizeof(sense_buffer);
    io_hdr.dxfer_direction = SG_DXFER_FROM_DEV;  // 数据从设备传输到主机
    io_hdr.dxfer_len = m_CommandStruct.responseBufferLength;  // 返回的数据缓冲区长度
    io_hdr.dxferp = m_CommandStruct.responseBuffer;  // 返回数据的缓冲区指针
    io_hdr.cmdp = cdb;  // 指向CDB
    io_hdr.sbp = sense_buffer;  // 传感器缓冲区
    io_hdr.timeout = 3000;  // 设置超时时间，单位为毫秒

    if (ioctl(fd, SG_IO, &io_hdr) < 0) {
        perror("SCSI REPORT ELEMENT STATUS command failed");
        return IO_ERR;
    }

    if ((io_hdr.info & SG_INFO_OK_MASK) != SG_INFO_OK) {
        std::cerr << "SCSI 命令执行错误" << std::endl;
        if (io_hdr.sb_len_wr > 0) {
            std::cerr << "Sense Data 错误码: ";
            print_sense_buffer(sense_buffer, io_hdr.sb_len_wr); // 输出 Sense Buffer
        }
        close(fd);
        return 1;
    }

    m_DeviceStatus.status_header = (ELEMENT_STATUS_HEADER *)m_CommandStruct.responseBuffer;
    m_DeviceStatus.page_header = (ELEMENT_STATUS_PAGE *)(m_CommandStruct.responseBuffer + sizeof(ELEMENT_STATUS_HEADER));
    m_DeviceStatus.descriptor = (ELEMENT_DESCRIPTOR *)(m_CommandStruct.responseBuffer + sizeof(ELEMENT_STATUS_HEADER)+ sizeof(ELEMENT_STATUS_PAGE));

    print_response_data(0xb8);
    return 0;
}


int CScsiLibrary::mode_sense(int page_code, int DBD_bit)
{
    int fd = open(m_CommandStruct.devicePath, O_RDWR);
    if (fd < 0) {
        perror("Failed to open tape device");
        return -1;
    }

    struct sg_io_hdr io_hdr;
    unsigned char cdb[6];
    unsigned char sense_buffer[32];

    memset(&io_hdr, 0, sizeof(io_hdr));
    memset(cdb, 0, sizeof(cdb));
    memset(sense_buffer, 0, sizeof(sense_buffer));

    cdb[0] = SCSI_MODE_SENSE;
    cdb[1] = 0x00;
    if(DBD_bit)
    {
        cdb[1] |= 0x08;
    }
    cdb[2] = page_code;
    cdb[3] = 0x00;
    cdb[4] = MODE_SENSE_BUFFER_LENGTH;
    cdb[5] = 0x00;

    io_hdr.interface_id = 'S';
    io_hdr.cmd_len = sizeof(cdb);
    io_hdr.mx_sb_len = sizeof(sense_buffer);
    io_hdr.dxfer_direction = SG_DXFER_FROM_DEV;  // 数据从设备传输到主机
    io_hdr.dxfer_len = m_CommandStruct.responseBufferLength;  // 返回的数据缓冲区长度
    io_hdr.dxferp = m_CommandStruct.responseBuffer;  // 返回数据的缓冲区指针
    io_hdr.cmdp = cdb;  // 指向CDB
    io_hdr.sbp = sense_buffer;  // 传感器缓冲区
    io_hdr.timeout = 3000;  // 设置超时时间，单位为毫秒

    if (ioctl(fd, SG_IO, &io_hdr) < 0) {
        perror("SCSI REPORT ELEMENT STATUS command failed");
        return IO_ERR;
    }

    if ((io_hdr.info & SG_INFO_OK_MASK) != SG_INFO_OK) {
        std::cerr << "SCSI 命令执行错误" << std::endl;
        if (io_hdr.sb_len_wr > 0) {
            std::cerr << "Sense Data 错误码: ";
            print_sense_buffer(sense_buffer, io_hdr.sb_len_wr); // 输出 Sense Buffer
        }
        close(fd);
        return 1;
    }
    return 0;
}

int CScsiLibrary::mode_select()
{
    return 0;
}

int CScsiLibrary::get_library_all_elements()
{
    int ret,retry=5;
    new_buffer(MODE_SENSE_BUFFER_LENGTH);

    while(retry--&&ret != 0)
        ret = mode_sense(0x1d,0);

    if(ret)
    {
        return READ_DEV_INFO_ERROR;
    }
    else
    {
        if(m_CommandStruct.responseBuffer[3]!=0)
        {
            return SCSI_STRUCT_ERROR;
        }

        if((m_CommandStruct.responseBuffer[4] & 0x1d)!=0x1d)
        {
            ret = mode_sense(0x1d,1);
            if(ret)
                return READ_DEV_INFO_ERROR;
        }
    }

    char *ptr2 = (char*)&m_CommandStruct.responseBuffer[4];;
    unsigned short *ptr = (unsigned short *)&m_CommandStruct.responseBuffer[4];
    /* The function swab copies len bytes from the location referenced by src to the location referenced by dst, swapping adjacent bytes.
     * This is used to copy data from one machine to another with a different byte order.*/
    swab(ptr2,ptr2,m_CommandStruct.responseBuffer[5]);

    m_ElementStruct.addr_trans = *(ptr + 1);
    m_ElementStruct.num_trans = *(ptr + 2);

    m_ElementStruct.addr_slot = *(ptr + 3);
    m_ElementStruct.num_slot = *(ptr + 4);

    m_ElementStruct.addr_ie = *(ptr + 5);
    m_ElementStruct.num_ie = *(ptr + 6);

    m_ElementStruct.addr_drive = *(ptr + 7);
    m_ElementStruct.num_drive = *(ptr + 8);

    print_response_data(0x1d);
    return 0;
}

int CScsiLibrary::move_medium(int source_element, int destination_element, int iFlag)
{
    int fd = open(m_CommandStruct.devicePath, O_RDWR);
    if (fd < 0) {
        perror("Failed to open tape device");
        return -1;
    }

    // SCSI Command Descriptor Block (CDB) for MOVE MEDIUM
    unsigned char cdb[12] = {0};
    cdb[0] = SCSI_MOVE_MEDIUM;
    if(iFlag == MOVE_FROM_SLOT_TO_DRIVE)
    {
        cdb[2] = (m_ElementStruct.addr_trans >> 8) & 0xFF;
        cdb[3] = m_ElementStruct.addr_trans & 0xFF;
        cdb[4] = ((m_ElementStruct.addr_slot + source_element) >> 8) & 0xFF;
        cdb[5] = (m_ElementStruct.addr_slot + source_element) & 0xFF;
        cdb[6] = ((m_ElementStruct.addr_drive + destination_element) >> 8) & 0xFF;
        cdb[7] = (m_ElementStruct.addr_drive + destination_element) & 0xFF;
    }
    else if(iFlag == MOVE_FROM_DRIVE_TO_SLOT)
    {
        cdb[2] = (m_ElementStruct.addr_trans >> 8) & 0xFF;
        cdb[3] = m_ElementStruct.addr_trans & 0xFF;
        cdb[4] = ((m_ElementStruct.addr_drive + source_element) >> 8) & 0xFF;
        cdb[5] = (m_ElementStruct.addr_drive + source_element) & 0xFF;
        cdb[6] = ((m_ElementStruct.addr_slot + destination_element) >> 8) & 0xFF;
        cdb[7] = (m_ElementStruct.addr_slot + destination_element) & 0xFF;
    }
    // SG_IO structure for SCSI command execution
    sg_io_hdr_t io_hdr;
    unsigned char sense_buffer[32] = {0};
    memset(&io_hdr, 0, sizeof(io_hdr));

    io_hdr.interface_id = 'S';
    io_hdr.cmdp = cdb;
    io_hdr.cmd_len = sizeof(cdb);
    io_hdr.sbp = sense_buffer;
    io_hdr.mx_sb_len = sizeof(sense_buffer);
    io_hdr.dxfer_direction = SG_DXFER_NONE;
    io_hdr.timeout = 5000; // 5 seconds

    // 使用 ioctl 发送 SCSI 命令
    int status = ioctl(fd, SG_IO, &io_hdr);
    if (status < 0) {
        std::cerr << "SCSI READ 命令失败: " << strerror(errno) << std::endl;
        close(fd);
        return MOVE_MEDIUM_ERROR;
    }

    // 检查传输状态
    if ((io_hdr.info & SG_INFO_OK_MASK) != SG_INFO_OK) {
        std::cerr << "SCSI 命令执行错误" << std::endl;
        if (io_hdr.sb_len_wr > 0) {
            std::cerr << "Sense Data 错误码: ";
            print_sense_buffer(sense_buffer, io_hdr.sb_len_wr); // 输出 Sense Buffer
        }
        close(fd);
        return MOVE_MEDIUM_ERROR;
    }
    return 0;
}

/*!
 * @warning 响应数据是堆内存，但是随时会被清空，不能将设备信息的指针指向它。
 */
int CScsiLibrary::print_response_data(int cdb_code,int page_code)
{
  //  printf("[cdb_code:%d,page_code:%d] responseBuffer:\n",cdb_code,page_code);
    if(cdb_code == 0xb8)
    {
        /*for (int i = 0; i < m_CommandStruct.responseBufferLength; ++i)
            std::cout <<  hex<<(int)m_CommandStruct.responseBuffer[i] << " ";*/
        std::cout<<"Primary Volume Tag Information:"<<endl;
        std::cout<<m_DeviceStatus.descriptor->szVolTag<<endl;
    }
    if(cdb_code == 0x1d)
    {
        std::cout<<"Drive numbers:"<<m_ElementStruct.num_drive<<" Drive address:"<<m_ElementStruct.addr_drive<<" Slot numbers:"<<m_ElementStruct.num_slot<<" Slot address:"<<m_ElementStruct.addr_slot<<'\n'<<endl;
    }

    if(cdb_code == 0x12)
    {
        if(page_code == 0x00)
        {
            char *inquiry_response = m_CommandStruct.responseBuffer;

            // 解析设备类型
            int device_type = inquiry_response[0] & 0x1F;
            std::cout << "Device Type: " << device_type << std::endl;

            // 解析制造商
            char vendor[9];
            memset(vendor, 0, sizeof(vendor));
            memcpy(vendor, &inquiry_response[8], 8);
            std::cout << "Vendor: " << vendor << std::endl;

            // 解析产品标识
            char product[17];
            memset(product, 0, sizeof(product));
            memcpy(product, &inquiry_response[16], 16);
            std::cout << "Product: " << product << std::endl;

            // 解析产品修订号
            char revision[5];
            memset(revision, 0, sizeof(revision));
            memcpy(revision, &inquiry_response[32], 4);
            std::cout << "Revision: " << revision << std::endl;
        }
        else if(page_code == 0x80)
        {
            std::cout<<m_CommandStruct.devicePath<<" Data Trans Device Serial Number:"<<m_CommandStruct.responseBuffer+4<<std::endl;
        }

    }
    return 0;
}


/*!
 *
 * @param page_code 00h - Supported Vital Product Data pages (this list) \n
 *                  80h - Unit Serial Number page                        \n
 *                  83h – Device Identification page                     \n
 *                  85h - Management Network Addresses page              \n
 *                  C8h - Vendor Specific Device Capabilities page       \n
 * @return
 */
int CScsiLibrary::get_query(unsigned char page_code)
{
    // 打开设备
    int fd = open(m_CommandStruct.devicePath, O_RDWR);
    if (fd < 0) {
        std::cerr << "无法打开设备: " << m_CommandStruct.devicePath << " 错误: " << strerror(errno) << std::endl;
        return 1;
    }
    new_buffer(INQUIRY_BUFFER_LENGTH);

    // SCSI CDB (6字节读命令)
    unsigned char cdb[6];
    memset(cdb, 0, sizeof(cdb));
    cdb[0] = SCSI_INQUIRY;  // 操作码 (READ 6)
    if(page_code == 0x00)
    {
        cdb[1] = 0x00;
        cdb[2] = 0x00;
    }
    else
    {
        cdb[1] = 0x01;
        cdb[2] = 0x80;
    }
    cdb[3] = 0x00;
    cdb[4] = INQUIRY_BUFFER_LENGTH;
    cdb[5] = 0x00;

    // Sense Buffer（请求检测数据缓冲区）
    unsigned char sense_buffer[SENSE_BUFFER_LEN];
    memset(sense_buffer, 0, sizeof(sense_buffer));

    // 创建 SG_IO 结构
    sg_io_hdr_t io_hdr;
    memset(&io_hdr, 0, sizeof(sg_io_hdr_t));
    io_hdr.interface_id = 'S';                 // 使用SCSI通用接口
    io_hdr.cmd_len = sizeof(cdb);              // CDB的长度
    io_hdr.mx_sb_len = sizeof(sense_buffer);    // 没有错误返回信息缓冲区
    io_hdr.dxfer_direction = SG_DXFER_FROM_DEV;  // 数据传输方向：设备到主机
    io_hdr.dxfer_len = INQUIRY_BUFFER_LENGTH;    // 数据缓冲区长度
    io_hdr.dxferp = m_CommandStruct.responseBuffer;               // 指向数据缓冲区
    io_hdr.cmdp = cdb;                         // 指向CDB
    io_hdr.sbp = sense_buffer;                   // 指向Sense Buffer
    io_hdr.timeout = 5000;                     // 超时时间：5秒

    // 使用 ioctl 发送 SCSI 命令
    int status = ioctl(fd, SG_IO, &io_hdr);
    if (status < 0) {
        std::cerr << "SCSI READ 命令失败: " << strerror(errno) << std::endl;
        close(fd);
        return 1;
    }

    // 检查传输状态
    if ((io_hdr.info & SG_INFO_OK_MASK) != SG_INFO_OK) {
        std::cerr << "SCSI 命令执行错误" << std::endl;
        if (io_hdr.sb_len_wr > 0) {
            std::cerr << "Sense Data 错误码: ";
            print_sense_buffer(sense_buffer, io_hdr.sb_len_wr); // 输出 Sense Buffer
        }
        close(fd);
        return 1;
    }

    print_response_data(0x12,page_code);
    // 关闭设备
    close(fd);
    return 0;
}

int CScsiLibrary::setDeviceCommandStructAndDeviceInfo(SCSI_COMMAND_STRUCT commandStruct, DEVICE_INFO deviceInfo)
{
    m_CommandStruct = commandStruct;
    m_DeviceInfo = deviceInfo;
    return 0;
}

const char *CScsiLibrary::get_response_buffer()
{
    return  m_CommandStruct.responseBuffer;
}

void CScsiLibrary::setCScsiDrive(CScsiDrive *drive)
{
    int i = 0;
    while(m_Drives.find(i) != m_Drives.end())
    {
        i++;
    }
    m_Drives.insert({i,drive});
}

CScsiDrive *CScsiLibrary::getDrive(int iIndexDrive)
{
    return m_Drives[iIndexDrive];
}

const char *CScsiLibrary::GetLibrarySerialNumber()
{
    return m_LibrarySerialNumber;
}

int CScsiLibrary::initSlotTape()
{
    for(int i = 0;i<m_ElementStruct.num_slot;i++)
    {
        read_element_status(0x02,m_ElementStruct.addr_slot + i,m_ElementStruct.num_slot,0,true);
        TapeInfo *tapeInfo = new TapeInfo;
        memcpy(tapeInfo->szBarCode,m_DeviceStatus.descriptor->szVolTag,BARCODE_LENGTH);
        strtok(tapeInfo->szBarCode," "); //scsi命令读出来的时候末尾补 32(space), 此处将32替换为 '\0'
        m_slotInfo[i] = (tapeInfo);
    }
    cout<<m_slotInfo[0]->szBarCode<<endl;

    return 0;
}

TapeInfo* CScsiLibrary::getSlotInfo(int slotIndex)
{
    if(m_slotInfo.find(slotIndex)  != m_slotInfo.end())
    {
        return m_slotInfo[slotIndex];
    }
    else return nullptr;
}

int CScsiLibrary::initDriveTape()
{
    for(int i = 0;i<m_Drives.size();i++)
    {
        read_element_status(RE_DT_ELEMENT,m_Drives[i]->getAddr(),1,0,true);

        if(m_DeviceStatus.descriptor->szVolTag[0] != 0)
        {
            TapeInfo *tapeInfo = new TapeInfo;
            memcpy(tapeInfo->szBarCode,m_DeviceStatus.descriptor->szVolTag,BARCODE_LENGTH);
            strtok(tapeInfo->szBarCode," "); //scsi命令读出来的时候末尾补 32(space), 此处将32替换为 '\0'
            m_Drives[i]->setTapeInfo(tapeInfo);
        }
    }
    return 0;
}

