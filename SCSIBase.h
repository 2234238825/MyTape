//
// Created by Admin on 2024/12/4.
//

#ifndef UNTITLED13_SCSIBASE_H
#define UNTITLED13_SCSIBASE_H


/**
 * @brief 这个类用来执行最基本的scsi命令以及打印错误信息, 所有的成员函数都是为了执行相对应的cdb命令存在.
 *
 */
class SCSIBase
{
public:
    int SCSI_MODE_SENSE();
    int SCSI_MODE_SELECT();
    int SCSI_WRITE_FILE_MASKS();
};


#endif //UNTITLED13_SCSIBASE_H
