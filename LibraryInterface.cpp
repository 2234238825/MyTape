//
// Created by Admin on 2024/10/23.
//

#include "LibraryInterface.h"


int LibraryInterface::Produce(char *buf, int len)
{

    int ret, iCopied = 0, iToCopy;

    m_iTotalLen += len;
    while (iCopied < len)
    {
        if (m_iDataLen + len > BUF_SIZE)
        {
            iToCopy = BUF_SIZE - m_iDataLen;
            if (iToCopy > 0)
            {
                memcpy(m_pBuffer + m_iDataLen, buf + iCopied, iToCopy);

                m_iDataLen += iToCopy;

                iCopied += iToCopy;
            }

            int iBlocks = m_iDataLen / m_iBlockSize;
            ret = WriteBlock(m_pBuffer, iBlocks);
            if (ret)
            {
                Log(ERROR_LEVEL, "Write data block to tape failed, ret %d", ret);
                return ret;
            }

            iBlocks *= m_iBlockSize;
            if (m_iDataLen - iBlocks > 0)
            {
                memmove(m_pBuffer, m_pBuffer + iBlocks, m_iDataLen - iBlocks);
            }

            m_iDataLen -= iBlocks;
        }

        int iToCopy = len - iCopied;
        if (iToCopy > BUF_SIZE - m_iDataLen)
        {
            iToCopy = BUF_SIZE - m_iDataLen;
        }

        memcpy(m_pBuffer + m_iDataLen, buf + iCopied, iToCopy);

        m_iDataLen += iToCopy;

        iCopied += iToCopy;
    }
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
    int ret;

    if (m_iDataLen + sizeof(FileHeader) + m_iFileAttrLen - 1 > BUF_SIZE)
    {
        int iBlocks = m_iDataLen / m_iBlockSize;
        ret = WriteBlock(m_pBuffer, iBlocks);
        if (ret)
        {
            Log(ERROR_LEVEL, "Write data block to tape failed, ret %d", ret);
            return ret;
        }

        iBlocks *= m_iBlockSize;
        memmove(m_pBuffer, m_pBuffer + iBlocks, m_iDataLen - iBlocks);

        m_iDataLen -= iBlocks;
    }

    FileHeader* pFileHeader = (FileHeader*)(m_pBuffer + m_iDataLen);

    strncpy(pFileHeader->szFileName, pFileName, FILE_PATH_LEN);
    pFileHeader->llFileLength = llFileLength;

    memcpy(pFileHeader->Attributs, pFileAttr, m_iFileAttrLen);

    m_iDataLen += sizeof(FileHeader) + m_iFileAttrLen - 1;

    return 0;
}

/*!
 * @note 如果是空白磁带，倒带，写磁带头, 写磁带文件。 \n
 *       如果不是空白磁带，seekToTapeEnd,首先看看有没正常的结束标记，没有就加上。\n
 *       将SessionHeader传入m_pBuffer。                     \n
 */

int LibraryInterface::
CreateBackupTask(char *rootPath, int iFileAttrlen)
{

    m_Drive = m_Library->getDrive(0);
    TapeInfo *iTapeInfo = m_Drive->getTapeInfo();

    m_Drive->rewind();
    iTapeInfo->iSequence = 0;


    m_pBuffer = new char[BUF_SIZE];

    SessionHeader* pSessHeader = (SessionHeader * )m_pBuffer;
    memcpy(pSessHeader->szRootpath,rootPath,FILE_PATH_LEN);


    m_Drive->scsi_read_pos(m_ulTapeBlock);

    char szPath[FILE_PATH_LEN];
    sprintf(szPath, "meta%c%s%cCatalog_%u_%u", SLASH,iTapeInfo->szBarCode, SLASH, m_TapeHeader.iSequence, m_ulTapeBlock);
    int ret = CFile::CreateFolder(szPath);
    if(ret)
    {
        Log(ERROR_LEVEL,"create folder failed:%s",szPath);
    }

    CatalogHeader catalogHeader{};
    memcpy(catalogHeader.szRootpath,rootPath,FILE_PATH_LEN);
    catalogHeader.iSequence = 0;
    catalogHeader.iSignature = 0013156;
    m_iCatalog.SetCatalogName(szPath, &catalogHeader);
    StartFileBackupTask(rootPath);
    return 0;
}

int LibraryInterface::StartFileBackupTask(char *pPath)
{
    long long tm;
    m_iFileAttrLen = sizeof(struct stat);
    FIND_DATA stFind;

    char *p;

    p = pPath + strlen(pPath);

    if (*(p - 1) != SLASH)
    {
        *p = SLASH;
        p++;
        *p = 0;
    }

    int ret = 0;
    HANDLE hFind = CFile::FindFirstFile(pPath, &stFind);
    if (hFind == (HANDLE)(-1))
        return 0;

    do
    {
        if (stFind.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            if(strcmp(stFind.cFileName,".") == 0 ||
               strcmp(stFind.cFileName, "..") == 0)
            {
                continue;
            }
            sprintf(p, "%s%c", stFind.cFileName, SLASH);

            ret = StartFileBackupTask(pPath);
            if (ret)
            {
                printf("BackupSrcFiles failed, ret %d\n", ret);
                break;
            }
        }
        else
        {

            sprintf(p, "%s", stFind.cFileName);

            CFile file;

            ret = file.Open(pPath, OPEN_EXISTING);
            if (ret)
            {
                printf("file.Open %s failed, ret %d\n", pPath, ret);
                break;
            }
            LONGLONG llFileLength;
            ret = file.GetFileSize(llFileLength);
            if (ret)
            {
                printf("GetFileSize %s failed, ret %d\n", pPath, ret);
                break;
            }
            struct stat file_stat;
            ret = stat(pPath, &file_stat);
            if (ret)
            {
                printf("stat %s failed, ret %d\n", pPath, ret);
                break;
            }


            //fprintf(fpLog,"backup file %s, Offset %lld,Length %lld\n", pRelativePath, Offset, llFileLength);
            //fflush(fpLog);

            ret = AppendFile(pPath,llFileLength, &file_stat);
            if (ret)
            {
                Log(ERROR_LEVEL,"AppendFile failed, %d",ret);
                break;
            }

            while (1)
            {
                char buf[512 * 1024];
                int r;
                LONGLONG t = CTime::GetCurTime();

                ret = file.Read(buf, sizeof(buf), r);
                if (ret)
                {
                    printf("file.Read %s failed, ret %d\n", pPath, ret);
                    break;
                }

                tm += CTime::GetCurTime() -t;

                if (r == 0)
                {
                    break;
                }

                ret = Produce(buf, r);
                if (ret)
                {
                    printf("pLibrary->Write failed, ret %d\n", ret);
                    break;
                }
            }

            file.Close();

            if (ret)
            {
                break;
            }

            ret = SetFileLength(pPath, llFileLength);
            if (ret)
            {
                Log(ERROR_LEVEL,"SetFileLength %s failed, ret %d\n", pPath, ret);
                break;
            }
        }

    } while (CFile::FindNextFile(hFind, &stFind) == 0);

    CFile::FindClose(hFind);

    *p = 0;

    return ret;
}

int LibraryInterface::WriteBlock(char *pBuffer, int pBlocks)
{
    m_statisticsBlocks+=pBlocks;
    return m_Drive->write_block(pBuffer,pBlocks);
}

int LibraryInterface::SetFileLength(char *pFileName, LONGLONG llFileLength)
{
    int ret = m_iCatalog.AddFile(pFileName, llFileLength);
    if (ret)
    {
        Log(ERROR_LEVEL, "Add catalog file failed, ret %d", ret);
        return ret;
    }
    return 0;
}


