//
// Created by Admin on 2024/11/4.
//
#include <cstring>
#include "tapeGlobal.h"

/*!
 * 这个源文件是初始用于初始化磁带库的全局变量，所有的全局变量都应放在这里面。
 * 主要的有两个类：CScsiLibrary 和 CScsiDrive.
 */

vector<CScsiLibrary*> gv_scsiLibrary;
vector<CScsiDrive*> gv_scsiDrive;

int SendScsiInquiry(char *responseBuffer,const char *device_path,unsigned char pageCode)
{
    // 打开设备
    int fd = open(device_path, O_RDWR);
    if (fd < 0)
    {
        std::cerr << "无法打开设备: " << device_path << " 错误: " << strerror(errno) << std::endl;
        return 1;
    }

    char buffer[INQUIRY_BUFFER_LENGTH];
    memset(buffer,0,INQUIRY_BUFFER_LENGTH);

    // SCSI CDB (6字节读命令)
    unsigned char cdb[6];
    memset(cdb, 0, sizeof(cdb));
    cdb[0] = SCSI_INQUIRY;  // 操作码 (READ 6)
    if(pageCode == 0x00)
    {
        cdb[1] = 0x00;
        cdb[2] = 0x00;
    }
    else
    {
        cdb[1] = 0x01;
        cdb[2] = pageCode;
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
    io_hdr.dxfer_len = INQUIRY_BUFFER_LENGTH;     // 数据缓冲区长度
    io_hdr.dxferp = buffer;                     // 指向数据缓冲区
    io_hdr.cmdp = cdb;                            // 指向CDB
    io_hdr.sbp = sense_buffer;                   // 指向Sense Buffer
    io_hdr.timeout = 5000;                      // 超时时间：5秒

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
            CScsiDrive::print_sense_buffer(sense_buffer, io_hdr.sb_len_wr); // 输出 Sense Buffer
        }
        close(fd);
        return 1;
    }
    memcpy(responseBuffer,buffer,sizeof(STANDARD_INQUIRY_RESPONSE));
    close(fd);
    return 0;
}

char* Skip(char* pBuf, char c, int nSkip)
{
    char* p = pBuf;

    for (int i = 0; i < nSkip;i++)
    {
        p = strchr(p, c);

        if (p == nullptr)
        {
            return nullptr;
        }
        p++;
    }
    return p;
}

/*!
 * @param DeviceCommandStruct
 * @return Ercode
 * @note 将设备路径放、ABSL等信息入vector<SCSI_COMMAND_STRUCT>中
 */
int LinuxScanSCSI(vector<SCSI_COMMAND_STRUCT> *DeviceCommandStruct)
{
    FILE* fp = fopen("/proc/scsi/sg/devices", "r");

    if (fp == nullptr)
    {
        return -1;
    }
    int nSG = 0, tmp;
    char buf[256], * p;   //secure code
    while (fgets(buf, 256, fp))
    {
        tmp = nSG;
        nSG++;
        p = Skip(buf, '\t', 4);
        if (p == nullptr)
            continue;

        int nType = atoi(p);

        if (nType != OPTICAL_DEVICE && nType != SEQUENTIAL_ACCESS && nType != SCSI_CHANGER_DEVICE)
             continue;

        SCSI_COMMAND_STRUCT tmpCommandStruct{};
        p = buf;
        tmpCommandStruct.Adapter = atoi(p);
        p = Skip(buf, '\t', 1);
        if (p == nullptr)
            continue;

        tmpCommandStruct.Bus = atoi(p);
        p = Skip(buf, '\t', 2);
        if (p == nullptr)
            continue;

        tmpCommandStruct.scsiId = atoi(p);
        p = Skip(buf, '\t', 3);
        if (p == nullptr)
            continue;

        tmpCommandStruct.scsiId = atoi(p);
        sprintf(tmpCommandStruct.devicePath, "/dev/sg%d", tmp);
        DeviceCommandStruct->emplace_back(tmpCommandStruct);
    }

    fclose(fp);
    return 0;
}

int FindBackupDevice()
{
    vector<SCSI_COMMAND_STRUCT> scsiCommandStruct;
    LinuxScanSCSI(&scsiCommandStruct);
    DEVICE_INFO deviceInfo;
    unsigned char pageCode ;

    for(int i = 0;i<scsiCommandStruct.size();i++)
    {
        STANDARD_INQUIRY_RESPONSE standardInquiryResponse{};
        pageCode = 0x00;

        if(SendScsiInquiry((char *)&standardInquiryResponse,scsiCommandStruct[i].devicePath,pageCode))
            return SEND_INQUIRY_ERROR;

        if(standardInquiryResponse.deviceType!=SEQUENTIAL_ACCESS&&standardInquiryResponse.deviceType!=SCSI_CHANGER_DEVICE)
            continue;

        scsiCommandStruct[i].deviceType = (unsigned char)standardInquiryResponse.deviceType;

        memcpy(deviceInfo.productID,standardInquiryResponse.productID,PRODUCT_ID_LENGTH);
        memcpy(deviceInfo.vendorID,standardInquiryResponse.vendorID,VENDOR_ID_LENGTH);
        memcpy(deviceInfo.productRevision,standardInquiryResponse.firmwareRevisionLevel,FIRMWARE_LENGTH);

        if((standardInquiryResponse.personality&0x0f) == COMPAQ_INQUIRY_BIT)
        {
            pageCode = 0xcc;
        }

        if((standardInquiryResponse.personality&0x1f) == DELL_INQUIRY_BIT)
        {
            pageCode = 0xdc;
        }

        if(pageCode != 0x00)
        {
            printf("Error %s",__FUNCTION__);
            char responseBuffer[INQUIRY_BUFFER_LENGTH];
            if(!SendScsiInquiry(responseBuffer,scsiCommandStruct[i].devicePath,pageCode))
                return SEND_INQUIRY_ERROR;
        }

        if(scsiCommandStruct[i].deviceType == SCSI_CHANGER_DEVICE)
        {
            CScsiLibrary *tmpScsiLibrary= new CScsiLibrary;
            tmpScsiLibrary->setDeviceCommandStructAndDeviceInfo(scsiCommandStruct[i],deviceInfo);
            gv_scsiLibrary.emplace_back(tmpScsiLibrary);
        }
        else if(scsiCommandStruct[i].deviceType == SEQUENTIAL_ACCESS)
        {
            CScsiDrive *globalScsiDrive = new CScsiDrive;
            globalScsiDrive->setDeviceCommandStructAndDeviceInfo(scsiCommandStruct[i],deviceInfo);
            globalScsiDrive->get_query();  //获取序列号

            gv_scsiDrive.emplace_back(globalScsiDrive);
        }
    }
    return 0;
}
/*!
 *  @note 将磁带库对应的驱动器放进去，将磁带库，slot数量，tapes条形码和设备序列号都初始化完成。
 */
CScsiLibrary* GetLibraryIndex(int LibraryIndex)
{
    int ret = FindBackupDevice();
    if(ret)
        return nullptr;

    int iNumberOfDrives = gv_scsiDrive.size();
    int iDrive = 0;

    if(LibraryIndex>=gv_scsiLibrary.size()||LibraryIndex<0)
        return nullptr;

    gv_scsiLibrary[LibraryIndex]->get_library_all_elements();
    ELEMENT_STRUCT iElement = gv_scsiLibrary[LibraryIndex]->get_element_struct();

    int iDriveElementAddr = iElement.addr_drive;

    while(iDrive++ < iElement.num_drive)
    {
        char szBitArray[10] = {};   //初始化为全0

        szBitArray[0] = 0x01;  ////ADIC needs this bit=1 for serial #
        szBitArray[1] = 0x81;  ////Exabyte requires Serialization bit=1 for serial #
        szBitArray[2] = 0x80;  ////Primary voltag bit

        int iBitCount = 2;

        gv_scsiLibrary[LibraryIndex]->read_element_status(RE_DT_ELEMENT,iDriveElementAddr,1,0x01,true);

        const char * responseBufferPtr = gv_scsiLibrary[LibraryIndex]->get_response_buffer();


        for(int i = 0;i<iNumberOfDrives;i++)
        {
            const char *iSerialNumberPtr = responseBufferPtr;
            int iFlag = 0;
            while(true)
            {
                iSerialNumberPtr = (char*)memchr(iSerialNumberPtr,gv_scsiDrive[i]->getDriveSerialNumber()[0],100);//长度可以乱填，反正能匹配到就行，大一点就好。
                if(iSerialNumberPtr)
                {
                    if(strcmp(iSerialNumberPtr,gv_scsiDrive[i]->getDriveSerialNumber()) == 0) //驱动器和带库匹配成功
                    {
                        gv_scsiLibrary[LibraryIndex]->setCScsiDrive(gv_scsiDrive[i]);
                        iDriveElementAddr++;
                        iFlag = 1;
                        break;
                    }
                    iSerialNumberPtr++;
                }
                else break;
            }
            if(iFlag)
                break;
        }
    }
    return gv_scsiLibrary[LibraryIndex];
}