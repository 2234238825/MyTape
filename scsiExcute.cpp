//
// Created by Admin on 2024/9/23.
//

#include "scsiExcute.h"
#include "scsiStruct.h"
void scsiExcute::print_sense_buffer(unsigned char* sense_buffer, int len)
{
    std::cout << "Sense Buffer: sg_decode_sense ";
    for (int i = 0; i < len; ++i) {
        std::cout << std::hex << (int)sense_buffer[i] << " ";
    }
    std::cout << std::dec << std::endl;
}
int scsiExcute::set_scsi_block_size(int block_size)
{
    int fd = open(TAPE_DEVICE, O_RDWR);
    if (fd < 0) {
        perror("Failed to open tape device");
        return -1;
    }

    sg_io_hdr io_hdr = {0};
    unsigned char cdb[6] = {SCSI_MODE_SELECT,0x10,0,0,12,0}; // MODE SELECT(6) 命令格式
    unsigned char sense_buffer[32] = {0};
    unsigned char data_out[12] = {0}; // MODE SELECT 数据包，取决于设备文档和支持



    // 设置块大小到数据包中
    data_out[0] = 0;
    data_out[1] = 0;
    data_out[2] = 0x10;
    data_out[3] = 8;  //块描述符的长度 切记别漏掉 否则不报错不成功
    data_out[5] = 0;
    data_out[6] = 0;
    data_out[7] = 0;
    data_out[9] = (block_size >> 16)& 0xFF ;
    data_out[10] = (block_size >> 8)& 0xFF ;
    data_out[11] = block_size& 0xFF ;



    // 填充 sg_io_hdr
    io_hdr.interface_id = 'S';
    io_hdr.cmd_len = sizeof(cdb);
    io_hdr.mx_sb_len = 32;
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

int scsiExcute::write_block(char *buffer,int blocks)
{
    // 打开设备
    int fd = open(TAPE_DEVICE, O_RDWR);
    if (fd < 0) {
        std::cerr << "无法打开设备: " << TAPE_DEVICE << " 错误: " << strerror(errno) << std::endl;
        return 1;
    }

    // 要写入的数据
    unsigned char write_buffer[WRITE_BUFFER_SIZE];
    for(int i = 0;i<WRITE_BUFFER_SIZE;i++)
        write_buffer[i] = i%26 + 'a';
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
    io_hdr.dxfer_len = WRITE_BUFFER_SIZE;   // 数据缓冲区长度
    io_hdr.dxferp = write_buffer;              // 指向数据缓冲区
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

    std::cout << "成功写入 " << WRITE_BUFFER_SIZE << " 字节到磁带设备" << std::endl;

    // 关闭设备
    close(fd);
    return 0;

}

int scsiExcute::read_block(char *buffer, int blocks)
{
    // 打开设备
    int fd = open(TAPE_DEVICE, O_RDWR);
    if (fd < 0) {
        std::cerr << "无法打开设备: " << TAPE_DEVICE << " 错误: " << strerror(errno) << std::endl;
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
    io_hdr.dxfer_len = sizeof(read_buffer);    // 数据缓冲区长度
    io_hdr.dxferp = read_buffer;               // 指向数据缓冲区
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
    std::cout << "成功读取 " << READ_BUFFER_SIZE << " 字节的数据:" << std::endl;

    std::cout.write(reinterpret_cast<char*>(read_buffer), READ_BUFFER_SIZE);
    std::cout << std::endl;

    // 关闭设备
    close(fd);
    return 0;
}

int scsiExcute::get_scsi_block_size(unsigned int *block_size)
{
    int fd = open(TAPE_DEVICE, O_RDWR);
    if (fd < 0) {
        std::cerr << "无法打开设备: " << TAPE_DEVICE << " 错误: " << strerror(errno) << std::endl;
        return 1;
    }

    unsigned char data_out[92]={};
    unsigned char sense_buffer[SENSE_BUFFER_LEN] = {};


    // SCSI CDB (6字节读命令)
    unsigned char cdb[6] = {};

    cdb[0] = SCSI_MODE_SENSE;
    cdb[1] = 0x00;
    cdb[2] = 0x00;
    cdb[3] =0x00;
    cdb[4] =92;
    cdb[5] =0x00;

    sg_io_hdr_t io_hdr = {0};

    io_hdr.interface_id = 'S';                 // 使用SCSI通用接口
    io_hdr.cmd_len = sizeof(cdb);              // CDB的长度
    io_hdr.mx_sb_len = sizeof(sense_buffer);    // 没有错误返回信息缓冲区
    io_hdr.dxfer_direction = SG_DXFER_FROM_DEV;  // 数据传输方向：设备到主机
    io_hdr.dxfer_len = 92;    // 数据缓冲区长度
    io_hdr.dxferp = data_out;               // 指向数据缓冲区
    io_hdr.cmdp = cdb;                         // 指向CDB
    io_hdr.sbp = sense_buffer;                   // 指向Sense Buffer
    io_hdr.timeout = 5000;

    int status = ioctl(fd, SG_IO, &io_hdr);
    if (status < 0) {
        std::cerr << "SCSI READ 命令失败: " << strerror(errno) << std::endl;
        close(fd);
        return 1;
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

    unsigned int iBlockSize= 0 ;
    iBlockSize = (int) data_out[9]<<16;
    iBlockSize |= (int) data_out[10]<<8;
    iBlockSize |= (int) data_out[11];
    *block_size = iBlockSize;
    return 0;
}


