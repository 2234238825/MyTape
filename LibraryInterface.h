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
    };
    int WriteTapeHeader();
    int Write(int buf, int len);
    int AppendFile(char* pFileName, LONGLONG llFileLength, void* pFileAttr);
    int CreateBackupTask(char *rootPath, int iFileAttrlen);
private:
    CScsiLibrary *m_Library;
    int m_iBlockSize;
    int m_iDataLen;
    char *m_pBuffer;
    CCatalog m_iCatalog;

};


#endif //UNTITLED13_LIBRARYINTERFACE_H
