//
// Created by Admin on 2024/10/23.
//

#include "LibraryInterface.h"

int LibraryInterface::Write(int buf, int len)
{
    return 0;
}

int LibraryInterface::WriteTapeHeader()
{
    CScsiDrive *iDrive = m_Library->getDrive(0);
    char buf[WRITE_BUFFER_SIZE] = {};

    TapeHeader tapeHeader{"huhh", 0, 0x1234567, 20241226, 0};
    memcpy(buf, &tapeHeader, sizeof(TapeHeader));
    iDrive->rewind();
    iDrive->write_block(buf,1);
    iDrive->rewind();

    char readbuffer[READ_BUFFER_SIZE];
    iDrive->read_block(readbuffer,1);

    cout<<((TapeHeader*)readbuffer)->toString()<<endl;
    return 0;
}

int LibraryInterface::AppendFile(char *pFileName, long long int llFileLength, void *pFileAttr)
{

    return 0;
}

int LibraryInterface::CreateBackupTask(char *rootPath, int iFileAttrlen)
{

    SessionHeader* pSessHeader = (SessionHeader * )m_pBuffer;
    memcpy(pSessHeader->szRootpath,rootPath,FILE_PATH_LEN);

    CatalogHeader catalogHeader;
   // sprintf(szPath, "Library_metaData%c%s%cCatalog_%u_%u", SLASH, m_TapeHeader.szTapeName, SLASH, m_TapeHeader.iSequence, m_ulTapeBlock);
    return 0;
}

