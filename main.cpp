

#include <fstream>
#include "scsiExcute.h"
#include "tapeGlobal.h"

#include <iostream>
#include <set>
#include <functional>
using namespace std;


namespace i2soft
{

// CMapSingleton: A general singleton implementation with keys
// i.e. given an identical key, there is only one instance
    template<typename KeyType, typename ValueType>
    class CMapSingleton
    {
    };
}

namespace i2soft
{
    using DbManager = CMapSingleton<std::string, int>;
}

int main()
{
    CScsiLibrary *LibManger = initLibraryList(0);
    LibManger->move_medium(1,1,MOVE_FROM_SLOT_TO_DRIVE);

    CScsiDrive *iDrive = LibManger->getDrive(1);
    cout<<iDrive->getDrivePath()<<endl;

    iDrive->set_scsi_block_size();
    iDrive->rewind();


    iDrive->write_block(nullptr,1);
    iDrive->rewind();

    iDrive->read_block(nullptr,1);
    LibManger->move_medium(1,1,MOVE_FROM_DRIVE_TO_SLOT);
    return 0;
}

/*******************命名规则********************/
// 类 的名称以大写字母“C”开头，表明定义的是类，后跟一个或多个单词。为便于界定，每个单词的首字母要大写。类的命名推荐用"名词"或"形容词＋名词"的形式。
// 结构体 大写+下划线表示。
// 函数 下划线命名法 与驼峰命名法相似，通过一种方式将不同单词区分开，方便读懂变量含义。与驼峰命名法不同的是，驼峰命名法采用的是首字母大写区分，下划线命名法是在不同单词之间添加下划线。
// 类变量 标识符的名字以一个或者多个小写字母开头作为前缀；前缀之后的是首字母大写的驼峰命名法。
// 常量、宏定义和枚举 一般都是所有字母大写，特别是宏定义，在不同字母之间，可以加上下划线，也可以不加。
// 全局变量 g_ 开头 + 驼峰命名
//

