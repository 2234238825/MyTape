//
// Created by Admin on 2024/10/23.
//

#ifndef UNTITLED13_LIBRARYINTERFACE_H
#define UNTITLED13_LIBRARYINTERFACE_H

#include "scsiLibrary.h"
#include "scsiDrive.h"
#include "tapeGlobal.h"
#include "Catalog.h"
class LibraryInterface
{
public:
    LibraryInterface(int IndexOfLibrary)
    {
        m_Library = GetLibraryIndex(IndexOfLibrary);

        m_iBlockSize = 65536;
        m_statisticsBlocks = 0;
    };
    int WriteTapeHeader();
    int Produce(char* buf, int len);
    int AppendFile(char* pFileName, LONGLONG llFileLength, void* pFileAttr);
    int CreateBackupTask(char *rootPath, int iFileAttrlen);
    int StartFileBackupTask(char *pPath);
    int WriteBlock(char* pBuffer, int pBlocks);
    int SetFileLength(char* pFileName, LONGLONG llFileLength);
private:
    CScsiLibrary *m_Library;
    CScsiDrive *m_Drive;
    int m_iBlockSize;
    int m_ulTapeBlock;
    int m_iDataLen;
    char *m_pBuffer;
    CCatalog m_iCatalog;
    TapeHeader m_TapeHeader;
    TapeInfo *m_tapeInfo;
    int m_iFileAttrLen;
    int m_iTotalLen;
    int m_statisticsBlocks;
};


#endif //UNTITLED13_LIBRARYINTERFACE_H
