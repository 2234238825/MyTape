/**
* @file     	main.cpp
* @author   	huhh
* @email   	2234238825@qq.com
* @version	V1.0
* @date    	24-NOV-2014
* @license  	GNU General Public License (GPL)
* @brief   	Universal Synchronous/Asynchronous Receiver/Transmitter
* @detail		detail
* @attention
*  This file is part of OST.                                                    \n
*  This program is free software; you can redistribute it and/or modify 		\n
*  it under the terms of the GNU General Public License version 3 as 		    \n
*  published by the Free Software Foundation.                               	\n
*  You should have received a copy of the GNU General Public License   		    \n
*  along with OST. If not, see <http://www.gnu.org/licenses/>.       			\n
*  Unless required by applicable law or agreed to in writing, software       	\n
*  distributed under the License is distributed on an "AS IS" BASIS,         	\n
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  	\n
*  See the License for the specific language governing permissions and     	    \n
*  limitations under the License.   											\n
*   																			\n
* @htmlonly
* <span style="font-weight: bold">History</span>
* @endhtmlonly
* Version|Auther|Date|Describe
* ------|----|------|--------
* V3.3|Jones Lee|07-DEC-2017|Create File
* <h2><center>&copy;COPYRIGHT 2017 WELLCASA All Rights Reserved.</center></h2>
*/


#include "scsiExcute.h"
#include "tapeGlobal.h"
#include "LibraryInterface.h"
#include <thread>
#include "BytePacker.h"
#include <iostream>
#include <functional>
#include <queue>

using namespace std;
#define path "/dev/sg4"
CScsiLibrary *LibManger;

struct UsIoOption
{
    void SetFromRule(const std::string &ruleUuid,
                     const std::string &type,
                     int a,
                     std::string tapePoolName,
                     const std::string &library_sn,
                     std::vector<std::string> &driver_sn,
                     std::vector<int> &tape_for_use,
                     const std::string &storageIp, int32_t dedupEnd,
                     int tape_read_type = 1)
    {
        return;
    }
};

int main()
{
    UsIoOption io;
    string s;
    vector<string>ss;
    vector<string>null1, null2;
    vector<int>s1;
    io.SetFromRule(s, s, 2, NULL, NULL,ss,s1,NULL,NULL);
    /*CCatalog Catalog;
    char catalogPath[] = "meta/ttt100L1/Catalog_0_0";
    Catalog.SetCatalogName(catalogPath);
    cout<<Catalog.IsOpen()<<endl;
    cout<<Catalog.GetDetailFileCount()<<endl;
    CatalogHeader *catalogHeader = Catalog.GetSessInfo();
    cout<<catalogHeader->szRootpath<<endl;

    return 0; */


    int p = 10;
    const char *str = "hello, world";

    int driveIndex = 0, slotIndex = 0;
    //LibManger = GetLibraryIndex(0);

    LibraryInterface libraryInterface(0);
    char pPath[64] = "/data";
    libraryInterface.CreateBackupTask(pPath, 0);

    /* thread th1(backup, driveIndex, slotIndex);
     *
       th1.join();
     */

    return 0;
}

int backup(int driveIndex, int slotIndex)
{
    int ret, pos = 0;
    CScsiDrive *iDrive = LibManger->getDrive(driveIndex);
    if (!iDrive)
    {
        cout << "get null drive " << driveIndex << endl;
        return -1;
    }
    //   ret = LibManger->move_medium(slotIndex, driveIndex, MOVE_FROM_SLOT_TO_DRIVE);
    if (ret)
    {
        Log(ERROR_LEVEL, "move tape failed from slot %d to drive %d", slotIndex, driveIndex);
    }
    char buf[WRITE_BUFFER_SIZE] = {};
    TapeHeader tapeHeader{"huhh", 0, 0x1234567, 20241226, 0};
    memcpy(buf, &tapeHeader, sizeof(TapeHeader));
    ret = iDrive->rewind();
    if (ret)
    {
        Log(ERROR_LEVEL, "rewind failed");
        return ret;
    }
    ret = iDrive->write_block(buf, 1);
    if (ret)
    {
        Log(ERROR_LEVEL, "write block failed");
        return ret;
    }
    ret = iDrive->scsi_write_fileMarks();
    if (ret)
    {
        Log(ERROR_LEVEL, "write fileMark failed");
        return ret;
    }
    ret = iDrive->scsi_read_pos(pos);
    if (ret)
    {
        Log(ERROR_LEVEL, "read tape position failed");
        return ret;
    }
    cout << pos << endl;
    return 0;
}

/**
* @brief		can send the message
* @param[in]	canx : The Number of CAN
* @param[in]	id : the can id
* @param[in]	p : the data will be sent
* @param[in]	size : the data size
* @param[in]	is_check_send_time : is need check out the time out
* @note	Notice that the size of the size is smaller than the size of the buffer.
* @return
*	+1 Send successfully \n
*	-1 input parameter error \n
*	-2 canx initialize error \n
*	-3 canx time out error \n
* @par Sample
* @code
*	u8 p[8] = {0};
*	res_ res = 0;
* 	res = can_send_msg(CAN1,1,p,0x11,8,1);
* @endcode
*/
int space_blocks(int ulBlocks)
{
    int fd = open(path, O_RDWR);
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
            CScsiDrive::print_sense_buffer(sense_buffer, io_hdr.sb_len_wr); // 输出 Sense Buffer
        }
        close(fd);
        return 1;
    }
    return 0;
}

void f()
{
    CBytePacker packer;
    packer.PackInt(42);
    packer.PackFloat(3.14f);
    packer.PackString("Hello, World!");

    // Retrieve the packed buffer
    const auto &buffer = packer.GetBuffer();

    // Create a byte unpacker with the packed data
    CByteUnpacker unpacker(buffer);

    // Unpack the data
    int intValue = unpacker.UnpackInt();
    float floatValue = unpacker.UnpackFloat();
    std::string strValue = unpacker.UnpackString();

    // Print unpacked values
    std::cout << "Unpacked Integer: " << intValue << std::endl;
    std::cout << "Unpacked Float: " << floatValue << std::endl;
    std::cout << "Unpacked String: " << strValue << std::endl;
}
/*******************命名规则********************/
// 类 的名称以大写字母“C”开头，表明定义的是类，后跟一个或多个单词。为便于界定，每个单词的首字母要大写。类的命名推荐用"名词"或"形容词＋名词"的形式。
// 结构体 大写+下划线表示。
// 函数 下划线命名法 与驼峰命名法相似，通过一种方式将不同单词区分开，方便读懂变量含义。与驼峰命名法不同的是，驼峰命名法采用的是首字母大写区分，下划线命名法是在不同单词之间添加下划线。
// 类变量 标识符的名字以一个或者多个小写字母开头作为前缀；前缀之后的是首字母大写的驼峰命名法。
// 常量、宏定义和枚举 一般都是所有字母大写，特别是宏定义，在不同字母之间，可以加上下划线，也可以不加。
// 全局变量 g_ 开头 + 驼峰命名
//

