//
// Created by Admin on 2024/12/31.
//

#include "Catalog.h"
#include "Log.h"
#include <cstring>

CCatalog::CCatalog()
{
    m_iBufLen = 0;
    m_iFileCount = 0;
}

CCatalog::~CCatalog()
{

}

int CCatalog::AddFile(char* pFileName, LONGLONG llFileLength)
{
    int ret = 0, wrt, Len = strlen(pFileName) + 1;
    if (m_iBufLen + sizeof(Len) + Len + sizeof(llFileLength) >= sizeof(m_Buf))
    {
        ret = m_CatalogFile.Write(m_Buf, m_iBufLen, wrt);
        if (ret || m_iBufLen != wrt)
        {
            Log(ERROR_LEVEL, "Write catalog file %s failed, ret %d", m_CatalogFile.GetName(), ret);
            return ret;
        }

        m_iBufLen = 0;
    }

    memcpy(m_Buf + m_iBufLen, &Len, sizeof(Len));
    m_iBufLen += sizeof(Len);

    memcpy(m_Buf + m_iBufLen, pFileName, Len);
    m_iBufLen += Len;

    memcpy(m_Buf + m_iBufLen, &llFileLength, sizeof(llFileLength));
    m_iBufLen += sizeof(llFileLength);

    return ret;
}

int CCatalog::IsOpen()
{
    return m_CatalogFile.IsOpen();
}

int CCatalog::SetCatalogName(char* pCatalogName)
{
    m_iFileCount = 0;

    m_CatalogFile.Close();

    int ret = m_CatalogFile.Open(pCatalogName, OPEN_EXISTING);
    if (ret)
    {
        Log(ERROR_LEVEL, "Open catalog file %s failed, ret %d", pCatalogName, ret);
        return ret;
    }
    int r;
    ret = m_CatalogFile.Read(&m_Header, sizeof(m_Header), r);
    if (ret || sizeof(m_Header) != r)
    {
        Log(ERROR_LEVEL, "Read catalog file %s failed, ret %d", pCatalogName, ret);
        return ERR_READ_FILE_FAIL;
    }

    return 0;
}

CatalogHeader* CCatalog::GetSessInfo()
{
    return &m_Header;
}

int CCatalog::GetDetailFileCount()
{
    if (m_iFileCount > 0)
    {
        return m_iFileCount;
    }

    FileInfo Info;

    char* p = m_Buf;

    int r, Len;

    LONGLONG llOffset = sizeof(SessionHeader),llNew;

    m_CatalogFile.Seek(sizeof(CatalogHeader), llNew, FILE_BEGIN);

    m_iFileCount = 0;
    m_iBufLen = 0;

    while (1)
    {
        int ret = m_CatalogFile.Read(m_Buf + m_iBufLen, sizeof(m_Buf) - m_iBufLen, r);
        if (ret)
        {
            Log(ERROR_LEVEL, "Read catalog file %s failed, ret %d", m_CatalogFile.GetName(), ret);
            return ERR_READ_FILE_FAIL;
        }

        if (r == 0)
        {
            break;
        }

        m_iBufLen += r;

        while (p - m_Buf + sizeof(Len) < m_iBufLen)
        {
            memcpy(&Len,p,sizeof(Len));
            if (sizeof(Len) + Len + sizeof(Info.llLength) + (p - m_Buf) > m_iBufLen)
            {
                break;
            }

            p += sizeof(Len);

            memcpy(Info.szFileName, p, Len);
            p += Len;

            memcpy(&Info.llLength, p, sizeof(Info.llLength));
            p += sizeof(Info.llLength);

            Info.llOffset = llOffset;

            m_FileList[m_iFileCount] = Info;

            llOffset += sizeof(FileHeader) -1 + m_Header.iAttrLen + Info.llLength;

            m_iFileCount++;
        }

        memmove(m_Buf, p, m_iBufLen - (p - m_Buf));
        m_iBufLen -= (p - m_Buf);

        p = m_Buf;
    }

    return m_iFileCount;
}


FileInfo* CCatalog::GetDetailFile(int iIndex)
{
    if (iIndex > m_iFileCount || iIndex < 0)
    {
        return NULL;
    }

    FILE_LIST::iterator ptr = m_FileList.find(iIndex);
    if (ptr != m_FileList.end())
    {
        return &ptr->second;
    }

    return NULL;
}


int CCatalog::SetCatalogName(char* pCatalogName, CatalogHeader *pHeader)
{
    int ret = m_CatalogFile.Open(pCatalogName, CREATE_ALWAYS);
    if (ret)
    {
        Log(ERROR_LEVEL, "Open catalog file %s failed, ret %d", pCatalogName,ret);
        return ret;
    }

    strncpy(m_sFileName, pCatalogName,sizeof(m_sFileName));

    m_Header = *pHeader;

    int r;
    ret = m_CatalogFile.Write(pHeader, sizeof(CatalogHeader), r);
    if (ret || sizeof(CatalogHeader) != r)
    {
        Log(ERROR_LEVEL, "Write catalog file %s failed, ret %d", pCatalogName, ret);
        return ERR_WRITE_FILE_FAIL;
    }
    return 0;
}

int CCatalog::Flush()
{
    int wrt;
    int ret = m_CatalogFile.Write(m_Buf, m_iBufLen, wrt);
    if (ret || m_iBufLen != wrt)
    {
        Log(ERROR_LEVEL, "Write catalog file %s failed, ret %d", m_CatalogFile.GetName(), ret);
        return ret;
    }

    m_CatalogFile.Flush();

    m_iBufLen = 0;
    return 0;
}

int CCatalog::Seek(LONGLONG llPos, int iFlag)
{
    LONGLONG llNewOffset;
    return m_CatalogFile.Seek(llPos, llNewOffset, iFlag);
}

int CCatalog::Close()
{
    return m_CatalogFile.Close();
}

int CCatalog::ReadCatalog(char* pBuffer, int iToRead, int& iReaded)
{
    int ret = m_CatalogFile.Read(pBuffer, iToRead, iReaded);
    return ret;
}

int CCatalog::GetDetailFileCount(LONGLONG llIndexPos, int inum)
{
    FileInfo Info;

    char* p = m_Buf;

    int r, Len;

    LONGLONG llOffset = sizeof(SessionHeader),llNew;

    m_CatalogFile.Seek(llIndexPos, llNew, FILE_BEGIN);

    m_iFileCount = 0;
    m_iBufLen = 0;
    bool skip = false;

    while (!skip)
    {
        int ret = m_CatalogFile.Read(m_Buf + m_iBufLen, sizeof(m_Buf) - m_iBufLen, r);
        if (ret)
        {
            Log(ERROR_LEVEL, "Read catalog file %s failed, ret %d", m_CatalogFile.GetName(), ret);
            return ERR_READ_FILE_FAIL;
        }

        if (r == 0)
        {
            break;
        }

        m_iBufLen += r;

        while (p - m_Buf + sizeof(Len) < m_iBufLen)
        {
            memcpy(&Len,p,sizeof(Len));
            if (sizeof(Len) + Len + sizeof(Info.llLength) + (p - m_Buf) > m_iBufLen)
            {
                break;
            }

            p += sizeof(Len);

            memcpy(Info.szFileName, p, Len);
            p += Len;

            memcpy(&Info.llLength, p, sizeof(Info.llLength));
            p += sizeof(Info.llLength);

            Info.llOffset = llOffset;

            m_FileList[m_iFileCount] = Info;

            llOffset += sizeof(FileHeader) -1 + m_Header.iAttrLen + Info.llLength;

            m_iFileCount++;
            if (m_iFileCount >= inum)
            {
                skip = true;
                break;
            }
        }

        memmove(m_Buf, p, m_iBufLen - (p - m_Buf));
        m_iBufLen -= (p - m_Buf);

        p = m_Buf;
    }

    return m_iFileCount;
}

int CCatalog::WriteResult(const char *resultStr, int writeSz)
{
    LONGLONG llNew, llCur;
    int ret = m_CatalogFile.Seek(0, llCur, FILE_CURRENT);
    if (ret)
    {
        Log(ERROR_LEVEL, "Write Result to Catalog, Read Seek File Current failed, file:%s, ret:%d", m_CatalogFile.GetName(), ret);
        return ERR_SEEK_FILE_FAIL;
    }

    ret = m_CatalogFile.Seek(0, llNew, FILE_BEGIN);
    if (ret)
    {
        Log(ERROR_LEVEL, "Write Result to Catalog, Read Seek File Begin failed, file:%s, ret:%d", m_CatalogFile.GetName(), ret);
        return ERR_SEEK_FILE_FAIL;
    }

    int iReaded;
    CatalogHeader Header;
    ret = m_CatalogFile.Read(&Header, sizeof(CatalogHeader), iReaded);
    if (ret || iReaded != sizeof(CatalogHeader))
    {
        Log(ERROR_LEVEL, "Write Result to Catalog, Read Catalog Header failed, file:%s, ret:%d", m_CatalogFile.GetName(), ret);
        return ERR_READ_FILE_FAIL;
    }

    char *pCh = Header.cAppDef;
    memcpy(pCh, resultStr, writeSz);
    *(pCh+writeSz) = '\0';
    char *pmCh = m_Header.cAppDef;
    memcpy(pmCh, resultStr, writeSz);
    *(pmCh+writeSz) = '\0';

    ret = m_CatalogFile.Seek(0, llNew, FILE_BEGIN);
    if (ret)
    {
        Log(ERROR_LEVEL, "Write Result to Catalog, Write Seek File Begin failed, file:%s, ret:%d", m_CatalogFile.GetName(), ret);
        return ERR_SEEK_FILE_FAIL;
    }

    ret = m_CatalogFile.Write(&Header, sizeof(CatalogHeader), iReaded);
    if (ret || iReaded != sizeof(CatalogHeader))
    {
        Log(ERROR_LEVEL, "Write Result to Catalog, Write Catalog Header failed failed, file:%s, ret:%d", m_CatalogFile.GetName(), ret);
        return ERR_WRITE_FILE_FAIL;
    }

    ret = m_CatalogFile.Seek(llCur, llNew, FILE_BEGIN);
    if (ret)
    {
        Log(ERROR_LEVEL, "Write Result to Catalog, Seek File Current offset failed, file:%s, ret:%d", m_CatalogFile.GetName(), ret);
        return ERR_SEEK_FILE_FAIL;
    }

    return 0;
}

int CCatalog::DelCatalog()
{
    CFile::Delete(m_sFileName);
    return 0;
}
