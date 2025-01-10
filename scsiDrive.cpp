//
// Created by Admin on 2024/10/11.
//

#include "scsiDrive.h"
void CScsiDrive::print_sense_buffer(unsigned char* sense_buffer, int len)
{
    std::cout << "Sense Buffer: sg_decode_sense ";
    for (int i = 0; i < len; ++i) {
        std::cout << std::hex << (int)sense_buffer[i] << " ";
    }
    std::cout << std::dec << std::endl;
}

int CScsiDrive::set_scsi_block_size(unsigned int block_size)
{
    int fd = open(m_CommandStruct.devicePath, O_RDWR);
    if (fd < 0) {
        perror("Failed to open tape device");
        return -1;
    }

    sg_io_hdr io_hdr = {0};
    unsigned char cdb[6] = {0}; // MODE SELECT(6) 命令格式
    unsigned char sense_buffer[32] = {0};
    unsigned char data_out[12] = {0}; // MODE SELECT 数据包，取决于设备文档和支持

    // 设置块大小到数据包中
    data_out[2] = 0x10;
    data_out[3] = 8;  //块描述符的长度 切记别漏掉 否则不报错不成功
    data_out[9] = (block_size >> 16) & 0xFF;
    data_out[10] = (block_size >> 8) & 0xFF;
    data_out[11] = block_size & 0xFF;

    // 填充 MODE SELECT 命令
    cdb[0] = SCSI_MODE_SELECT;  // MODE SELECT 操作码
    cdb[1] = 0x10;             // PF位设为1，表示可更改模式参数
    cdb[4] = sizeof(data_out); // 数据包长度

    // 填充 sg_io_hdr
    io_hdr.interface_id = 'S';
    io_hdr.cmd_len = sizeof(cdb);
    io_hdr.mx_sb_len = sizeof(sense_buffer);
    io_hdr.dxfer_direction = SG_DXFER_TO_DEV; // 方向：写入设备
    io_hdr.dxfer_len = sizeof(data_out);
    io_hdr.dxferp = data_out;
    io_hdr.cmdp = cdb;
    io_hdr.sbp = sense_buffer;
    io_hdr.timeout = 5000; // 5秒超时

    // 发送命令
    int result = ioctl(fd, SG_IO, &io_hdr);
    if (result < 0) {
        perror("SCSI MODE SELECT failed");
        return -1;
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
    std::cout << "SCSI block size set to " << block_size << " bytes." << std::endl;
    return 0;
}

/*!
 *
 * @param buffer
 * @param blocks
 * @warning io_hdr.dxfer_len will limit the data received device(tape), otherwise it could report errors if io_hdr.dxfer_len > length of buffer\n
 * so it's better to make them equal
 */
int CScsiDrive::write_block(char *buffer,int blocks)
{
    // 打开设备
    int fd = open(m_CommandStruct.devicePath, O_RDWR);
    if (fd < 0) {
        std::cerr << "无法打开设备: " << m_CommandStruct.devicePath << " 错误: " << strerror(errno) << std::endl;
        return 1;
    }

    // 要写入的数据

    //memset(write_buffer, 'A', sizeof(write_buffer));  // 填充数据'A'

    // SCSI CDB (6字节写命令)
    unsigned char cdb[6];
    memset(cdb, 0, sizeof(cdb));
    cdb[0] = SCSI_WRITE_6_COMMAND;  // 操作码 (WRITE 6)
    // LBA字段（逻辑块地址），在这个简单示例中使用0地址
    cdb[1] = 0x01;
    cdb[2] = blocks >> 16;  // LBA的高字节
    cdb[3] = blocks >> 8;   // LBA的中间字节
    cdb[4] = blocks;            // LBA的字节
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
    io_hdr.dxfer_direction = SG_DXFER_TO_DEV;  // 数据传输方向：主机到设备
    io_hdr.dxfer_len = WRITE_BUFFER_SIZE*blocks;   // 数据缓冲区长度
    io_hdr.dxferp = buffer;              // 指向数据缓冲区
    io_hdr.cmdp = cdb;                         // 指向CDB
    io_hdr.sbp = sense_buffer;                   // 指向Sense Buffer
    io_hdr.timeout = 5000;                     // 超时时间：5秒

    // 使用 ioctl 发送 SCSI 命令
    int status = ioctl(fd, SG_IO, &io_hdr);
    if (status < 0) {
        std::cerr << "SCSI WRITE 命令失败: " << strerror(errno) << std::endl;
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

    std::cout << "成功写入 " << blocks  << " 个块到磁带设备" << std::endl;

    // 关闭设备
    close(fd);
    return 0;

}

/*!
 *
 * @param buffer
 * @param blocks
 * @return errorCode
 * @warning blocks must be set correctly, otherwise, an error will occur
 */
int CScsiDrive::read_block(char *buffer, int blocks)
{
    // 打开设备
    int fd = open(m_CommandStruct.devicePath, O_RDWR);
    if (fd < 0) {
        std::cerr << "无法打开设备: " << m_CommandStruct.devicePath << " 错误: " << strerror(errno) << std::endl;
        return 1;
    }

    // 准备读取的数据缓冲区
    unsigned char read_buffer[READ_BUFFER_SIZE];
    memset(read_buffer, 0, sizeof(read_buffer));  // 将缓冲区初始化为0

    // SCSI CDB (6字节读命令)
    unsigned char cdb[6];
    memset(cdb, 0, sizeof(cdb));
    cdb[0] = SCSI_READ_6_COMMAND;  // 操作码 (READ 6)
    // LBA字段（逻辑块地址），在这个简单示例中使用0地址
    cdb[1] = 0x01;  // LBA的高字节
    cdb[2] = blocks >> 16;  // LBA的高字节
    cdb[3] = blocks >> 8;   // LBA的中间字节
    cdb[4] = blocks;            // LBA的字节
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
    io_hdr.dxfer_len = WRITE_BUFFER_SIZE*blocks;    // 数据缓冲区长度
    io_hdr.dxferp = buffer;               // 指向数据缓冲区
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

    // 输出读取到的数据
    std::cout << "成功读取 " << READ_BUFFER_SIZE*blocks << " 字节的数据:" << std::endl;


    // 关闭设备
    close(fd);
    return 0;
}

int CScsiDrive::get_query()
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
    cdb[1] = 0x01;
    cdb[2] = 0x80;
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

    print_response_data(0x12,0x80);
    // 关闭设备
    close(fd);
    return 0;
}

void CScsiDrive::set_device_path(char *path)
{
    memcpy(m_CommandStruct.devicePath,path,64);
}

int CScsiDrive::print_response_data(int cdb_code,int page_code)
{
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
            memcpy(m_SerialNumber,m_CommandStruct.responseBuffer+4,SERIAL_NO_LENGTH);
        }

    }
    return 0;
}

int CScsiDrive::new_buffer(int buffer_size)
{
    if(m_CommandStruct.responseBufferLength > 0)
        delete_buffer();
    m_CommandStruct.responseBuffer = new char[buffer_size];
    memset(m_CommandStruct.responseBuffer,0,buffer_size);
    m_CommandStruct.responseBufferLength = buffer_size;
    return 0;
}

int CScsiDrive::delete_buffer()
{
    delete[] m_CommandStruct.responseBuffer;
    m_CommandStruct.responseBufferLength = 0;
    return 0;
}

int CScsiDrive::setDeviceCommandStructAndDeviceInfo(SCSI_COMMAND_STRUCT commandStruct, DEVICE_INFO deviceInfo)
{
    m_CommandStruct = commandStruct;
    m_DeviceInfo = deviceInfo;
    memcpy(m_DrivePath,commandStruct.devicePath,DEVICE_PATH);
    return 0;
}

const char *CScsiDrive::getDriveSerialNumber()
{
    return m_SerialNumber;
}

const char *CScsiDrive::getDrivePath()
{
    return m_DrivePath;
}

int CScsiDrive::rewind()
{
    // 打开设备
    int fd = open(m_CommandStruct.devicePath, O_RDWR);
    if (fd < 0) {
        std::cerr << "无法打开设备: " << m_CommandStruct.devicePath << " 错误: " << strerror(errno) << std::endl;
        return 1;
    }
    // SCSI Command Descriptor Block (CDB) for REWIND
    unsigned char cdb[6] = {0};
    cdb[0] = SCSI_REWIND; // Operation code
    cdb[1] = 0;      // Immediate = 0 (wait until complete)

    // SG_IO structure for SCSI command execution
    sg_io_hdr_t io_hdr;
    unsigned char sense_buffer[32] = {0};
    memset(&io_hdr, 0, sizeof(io_hdr));

    io_hdr.interface_id = 'S';
    io_hdr.cmdp = cdb;
    io_hdr.cmd_len = sizeof(cdb);
    io_hdr.sbp = sense_buffer;
    io_hdr.mx_sb_len = sizeof(sense_buffer);
    io_hdr.dxfer_direction = SG_DXFER_NONE; // No data transfer
    io_hdr.timeout = 5000;                 // 5 seconds timeout

    // 使用 ioctl 发送 SCSI 命令
    int status = ioctl(fd, SG_IO, &io_hdr);
    if (status < 0) {
        std::cerr << "SCSI READ 命令失败: " << strerror(errno) << std::endl;
        close(fd);
        return SCSI_REWIND_ERROR;
    }

    // 检查传输状态
    if ((io_hdr.info & SG_INFO_OK_MASK) != SG_INFO_OK) {
        std::cerr << "SCSI 命令执行错误" << std::endl;
        if (io_hdr.sb_len_wr > 0) {
            std::cerr << "Sense Data 错误码: ";
            print_sense_buffer(sense_buffer, io_hdr.sb_len_wr); // 输出 Sense Buffer
        }
        close(fd);
        return SCSI_REWIND_ERROR;
    }

    return 0;
}

int CScsiDrive::get_drive_block_size(int *block_size)
{

    return 0;
}

int CScsiDrive::scsi_space_blocks(int ulBlocks)
{
    int fd = open(m_CommandStruct.devicePath, O_RDWR);
    if (fd < 0)
    {
        return -1;
    }

    // SCSI SPACE command (CDB = 0x11)
    unsigned char cdb[6] = {0x11, 0x00, 0x00, 0x00, 0x01, 0x00};
    // 0x00 in cdb[1] means SPACE by blocks, cdb[4:2] specifies the number of blocks to skip.
    cdb[2] = ulBlocks >> 16;
    cdb[3] = ulBlocks >> 8;
    cdb[4] = ulBlocks;
    // SCSI generic (SG) I/O structure
    sg_io_hdr_t io_hdr;
    memset(&io_hdr, 0, sizeof(sg_io_hdr_t));

    unsigned char sense_buffer[32]; // Buffer to store sense data

    // Configure the SCSI command
    io_hdr.interface_id = 'S'; // Always 'S'
    io_hdr.cmd_len = sizeof(cdb); // Command length
    io_hdr.mx_sb_len = sizeof(sense_buffer); // Maximum sense buffer length
    io_hdr.dxfer_direction = SG_DXFER_NONE; // No data transfer for SPACE command
    io_hdr.cmdp = cdb; // Pointer to the CDB
    io_hdr.sbp = sense_buffer; // Pointer to sense buffer
    io_hdr.timeout = 5000; // Timeout in milliseconds

    if (ioctl(fd, SG_IO, &io_hdr) < 0)
    {
        perror("SCSI REPORT ELEMENT STATUS command failed");
        return IO_ERR;
    }

    if ((io_hdr.info & SG_INFO_OK_MASK) != SG_INFO_OK)
    {
        std::cerr << "SCSI 命令执行错误" << std::endl;
        if (io_hdr.sb_len_wr > 0)
        {
            std::cerr << "Sense Data 错误码: ";
            print_sense_buffer(sense_buffer, io_hdr.sb_len_wr); // 输出 Sense Buffer
        }
        close(fd);
        return 1;
    }
    close(fd);
    return 0;
}

int CScsiDrive::scsi_space_fileMarks(int ulnum)
{
    int fd = open(m_CommandStruct.devicePath, O_RDWR);
    if (fd < 0)
    {
        return -1;
    }

    // SCSI SPACE command (CDB = 0x11)
    unsigned char cdb[6] = {0x11, 0x01, 0x00, 0x00, 0x01, 0x00};
    // 0x01 in cdb[1] means SPACE by fileMarks, cdb[4:2] specifies the number of blocks to skip.
    cdb[2] = ulnum >> 16;
    cdb[3] = ulnum >> 8;
    cdb[4] = ulnum;
    // SCSI generic (SG) I/O structure
    sg_io_hdr_t io_hdr;
    memset(&io_hdr, 0, sizeof(sg_io_hdr_t));

    unsigned char sense_buffer[32]; // Buffer to store sense data

    // Configure the SCSI command
    io_hdr.interface_id = 'S'; // Always 'S'
    io_hdr.cmd_len = sizeof(cdb); // Command length
    io_hdr.mx_sb_len = sizeof(sense_buffer); // Maximum sense buffer length
    io_hdr.dxfer_direction = SG_DXFER_NONE; // No data transfer for SPACE command
    io_hdr.cmdp = cdb; // Pointer to the CDB
    io_hdr.sbp = sense_buffer; // Pointer to sense buffer
    io_hdr.timeout = 5000; // Timeout in milliseconds

    if (ioctl(fd, SG_IO, &io_hdr) < 0)
    {
        perror("SCSI REPORT ELEMENT STATUS command failed");
        return IO_ERR;
    }

    if ((io_hdr.info & SG_INFO_OK_MASK) != SG_INFO_OK)
    {
        std::cerr << "SCSI 命令执行错误" << std::endl;
        if (io_hdr.sb_len_wr > 0)
        {
            std::cerr << "Sense Data 错误码: ";
            CScsiDrive::print_sense_buffer(sense_buffer, io_hdr.sb_len_wr); // 输出 Sense Buffer
        }
        close(fd);
        return 1;
    }
    close(fd);
    return 0;
}

int CScsiDrive::scsi_read_pos(int &ulposition)
{
    int fd = open(m_CommandStruct.devicePath, O_RDWR);
    if (fd < 0)
    {
        perror("Failed to open tape device");
        return -1;
    }

    struct sg_io_hdr io_hdr;
    unsigned char cdb[10] = {};
    unsigned char sense_buffer[32] = {};
    unsigned char response_buffer[80] = {};

    memset(&io_hdr, 0, sizeof(io_hdr));
    memset(cdb, 0, sizeof(cdb));
    memset(sense_buffer, 0, sizeof(sense_buffer));

    cdb[0] = 0x34;
    cdb[1] = 0x00;
    cdb[2] = 0x00;
    cdb[3] = 0x00;
    cdb[4] = 0x00;
    cdb[5] = 0x00;

    io_hdr.interface_id = 'S';
    io_hdr.cmd_len = sizeof(cdb);
    io_hdr.mx_sb_len = sizeof(sense_buffer);
    io_hdr.dxfer_direction = SG_DXFER_FROM_DEV;  // 数据从设备传输到主机
    io_hdr.dxfer_len = 80;  // 返回的数据缓冲区长度
    io_hdr.dxferp = response_buffer;  // 返回数据的缓冲区指针
    io_hdr.cmdp = cdb;  // 指向CDB
    io_hdr.sbp = sense_buffer;  // 传感器缓冲区
    io_hdr.timeout = 3000;  // 设置超时时间，单位为毫秒

    if (ioctl(fd, SG_IO, &io_hdr) < 0)
    {
        perror("SCSI REPORT ELEMENT STATUS command failed");
        return IO_ERR;
    }

    if ((io_hdr.info & SG_INFO_OK_MASK) != SG_INFO_OK)
    {
        std::cerr << "SCSI 命令执行错误" << std::endl;
        if (io_hdr.sb_len_wr > 0)
        {
            std::cerr << "Sense Data 错误码: ";
            CScsiDrive::print_sense_buffer(sense_buffer, io_hdr.sb_len_wr); // 输出 Sense Buffer
        }
        close(fd);
        return 1;
    }
    unsigned int ulBlockAddress = 0, ulTemp = 0;
    using ULONG = unsigned long;
    ulBlockAddress = (ULONG) response_buffer[4];
    ulBlockAddress <<= 24;

    ulTemp = (ULONG) response_buffer[5];
    ulTemp <<= 16;
    ulBlockAddress |= ulTemp;

    ulTemp = (ULONG) response_buffer[6];
    ulTemp <<= 8;
    ulBlockAddress |= ulTemp;

    ulBlockAddress |= (ULONG) response_buffer[7];
    ulposition = ulBlockAddress;

    close(fd);
    return 0;
}

int CScsiDrive::scsi_write_fileMarks()
{
    int fd = open(m_CommandStruct.devicePath, O_RDWR);
    if (fd < 0)
    {
        perror("Failed to open tape device");
        return -1;
    }

    struct sg_io_hdr io_hdr;
    unsigned char cdb[6];
    unsigned char sense_buffer[32];
    unsigned char response_buffer[80] = {};


    memset(&io_hdr, 0, sizeof(io_hdr));
    memset(cdb, 0, sizeof(cdb));
    memset(sense_buffer, 0, sizeof(sense_buffer));

    cdb[0] = 0x10;
    cdb[1] = 0x00;
    cdb[2] = 0x00;
    cdb[3] = 0x00;
    cdb[4] = 0x01;
    cdb[5] = 0x00;

    io_hdr.interface_id = 'S';
    io_hdr.cmd_len = sizeof(cdb);
    io_hdr.mx_sb_len = sizeof(sense_buffer);
    io_hdr.dxfer_direction = SG_DXFER_FROM_DEV;  // 数据从设备传输到主机
    io_hdr.dxfer_len = 80;  // 返回的数据缓冲区长度
    io_hdr.dxferp = response_buffer;  // 返回数据的缓冲区指针
    io_hdr.cmdp = cdb;  // 指向CDB
    io_hdr.sbp = sense_buffer;  // 传感器缓冲区
    io_hdr.timeout = 3000;  // 设置超时时间，单位为毫秒

    if (ioctl(fd, SG_IO, &io_hdr) < 0)
    {
        perror("SCSI REPORT ELEMENT STATUS command failed");
        return IO_ERR;
    }

    if ((io_hdr.info & SG_INFO_OK_MASK) != SG_INFO_OK)
    {
        std::cerr << "SCSI 命令执行错误" << std::endl;
        if (io_hdr.sb_len_wr > 0)
        {
            std::cerr << "Sense Data 错误码: ";
            CScsiDrive::print_sense_buffer(sense_buffer, io_hdr.sb_len_wr); // 输出 Sense Buffer
        }
        close(fd);
        return 1;
    }
    close(fd);
    return 0;
}

int CScsiDrive::scsi_test_unit_raedy()
{
    int fd = open(m_CommandStruct.devicePath, O_RDWR);
    if (fd < 0)
    {
        return -1;
    }
    unsigned char cdb[6];
    cdb[0] = SCSI_TUR;
    cdb[1] = 0x00;
    cdb[2] = 0x00;
    cdb[3] = 0x00;
    cdb[4] = 0x00;
    cdb[5] = 0x00;

    sg_io_hdr_t io_hdr;
    memset(&io_hdr, 0, sizeof(sg_io_hdr_t));

    unsigned char sense_buffer[32]; // Buffer to store sense data

    // Configure the SCSI command
    io_hdr.interface_id = 'S'; // Always 'S'
    io_hdr.cmd_len = sizeof(cdb); // Command length
    io_hdr.mx_sb_len = sizeof(sense_buffer); // Maximum sense buffer length
    io_hdr.dxfer_direction = SG_DXFER_NONE; // No data transfer for SPACE command
    io_hdr.cmdp = cdb; // Pointer to the CDB
    io_hdr.sbp = sense_buffer; // Pointer to sense buffer
    io_hdr.timeout = 5000; // Timeout in milliseconds

    if (ioctl(fd, SG_IO, &io_hdr) < 0)
    {
        perror("SCSI REPORT ELEMENT STATUS command failed");
        return IO_ERR;
    }

    if ((io_hdr.info & SG_INFO_OK_MASK) != SG_INFO_OK)
    {
        std::cerr << "SCSI 命令执行错误" << std::endl;
        if (io_hdr.sb_len_wr > 0)
        {
            std::cerr << "Sense Data 错误码: ";
            CScsiDrive::print_sense_buffer(sense_buffer, io_hdr.sb_len_wr); // 输出 Sense Buffer
        }
        close(fd);
        return 1;
    }
    close(fd);
    return 0;
}

