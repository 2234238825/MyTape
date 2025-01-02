//
// Created by Admin on 2024/12/31.
//

#ifndef UNTITLED13_CATALOG_H
#define UNTITLED13_CATALOG_H

#include "OSAPI.h"
#include "kfcStruct.h"
#include <map>
using namespace std;


typedef map<int, FileInfo> FILE_LIST;

class CCatalog
{
    CFile m_CatalogFile;
    char m_Buf[512 * 1024];
    int m_iBufLen;
    CatalogHeader m_Header;
    char m_sFileName[PATH_SIZE];

    FILE_LIST m_FileList;
    int m_iFileCount;

public:
    CCatalog();
    ~CCatalog();
    int AddFile(char* pFileName, LONGLONG llFileLength);
    int SetCatalogName(char* pCatalogName);
    int SetCatalogName(char* pCatalogName, CatalogHeader* pHeader);
    int ReadCatalog(char* pBuffer, int iToRead, int& iReaded);
    int Seek(LONGLONG llPos, int iFlag);
    int Close();
    int Flush();
    CatalogHeader* GetSessInfo();
    int GetDetailFileCount();
    int GetDetailFileCount(LONGLONG llIndexPos, int inum);
    FileInfo* GetDetailFile(int iIndex);
    int IsOpen();
    int WriteResult(const char* resultStr, int writeSz);
    int DelCatalog();
};

#endif //UNTITLED13_CATALOG_H
