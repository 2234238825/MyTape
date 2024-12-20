//
// Created by Admin on 2024/9/23.
//

#ifndef UNTITLED13_SCSIEXCUTE_H
#define UNTITLED13_SCSIEXCUTE_H
#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <scsi/sg.h>
#include <cstring>
#include <vector>
using namespace std;

#define BLOCK_SIZE 65536 // 要设置的块大小 64KB
#define WRITE_BUFFER_SIZE 65536   // 定义写缓冲区大小
#define SCSI_WRITE_6_COMMAND 0x0A  // SCSI 6字节WRITE命令的操作码
#define SENSE_BUFFER_LEN 32      // Sense Buffer 长度
#define READ_BUFFER_SIZE 65536    // 定义读缓冲区大小
#define SCSI_READ_6_COMMAND 0x08  // SCSI 6字节READ命令的操作码

#define TAPE_DEVICE "/dev/sg22"  // 磁带设备路径
class scsiExcute
{
public:
    scsiExcute()
    {
      //  set_scsi_block_size(BLOCK_SIZE);
    }
    int set_scsi_block_size(int block_size);
    int write_block(char *buffer,int blocks);
    int read_block(char *buffer, int blocks);
    int get_scsi_block_size(unsigned int *block_size);


private:
    void print_sense_buffer(unsigned char* sense_buffer, int len);

private:
    vector<void *> m_drive;
};


#endif //UNTITLED13_SCSIEXCUTE_H
