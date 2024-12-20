//
// Created by Admin on 2024/10/22.
//

#ifndef UNTITLED13_MYFUNTION_H
#define UNTITLED13_MYFUNTION_H
#include <iostream>
#include <random>
#ifdef _WINDOWS
#include <Windows.h>
#include <io.h >
#include <WinIoCtl.h >

#else

#include <wchar.h>
#include <stdarg.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/types.h>
#include <unistd.h>
#include <dlfcn.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <cerrno>
#include <aio.h>
#include <sys/vfs.h>

#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ErrorCode.h"
namespace caps
{

    //均值为 mean、标准差为 stddev 的高斯噪声
    double generateGaussianNoise(double mean, double stddev);


    class CFile
    {
#define PATH_SIZE 2048
        typedef struct _FIND_DATA
        {
            unsigned int dwFileAttributes;
            char cFileName[PATH_SIZE];

        } FIND_DATA, * P_FIND_DATA, * LP_FIND_DATA;
        static const char SLASH = '/';

#define LONGLONG long long
#define WCHAR char

#define FILE_ATTRIBUTE_DIRECTORY 16
#ifndef _WINDOWS

#define CREATE_ALWAYS 1
#define OPEN_ALWAYS  2
#define OPEN_EXISTING 3

#define FILE_SHARE_READ 4
#define FILE_SHARE_WRITE 5

#define FILE_FLAG_NO_BUFFERING 1

#define FILE_BEGIN           SEEK_SET
#define FILE_CURRENT         SEEK_CUR
#define FILE_END             SEEK_END
#define WCHAR char
#define LPCWSTR char*
#define LPCTSTR char*
#define LPCSTR char*
#define WORD unsigned short

#define FARPROC void*
#define HANDLE void*

#define INFINITE -1

#endif
#ifdef _WINDOWS
        HANDLE m_hFile;
#else
        int m_hFile;
#endif

        WCHAR m_sFileName[PATH_SIZE];

    public:
        CFile()
        {
#ifdef _WINDOWS
            m_hFile = INVALID_HANDLE_VALUE;
#else
            m_hFile = -1;
#endif
            m_sFileName[0] = 0;
        }
        ~CFile()
        {
            Close();
        }
        WCHAR* GetName()
        {
            return m_sFileName;
        }

        int Open(LPCWSTR pFileName, int nOpenFlag = OPEN_EXISTING, int nShareFlag= FILE_SHARE_READ, int nAttributsFlags=0)
        {
#ifdef _WINDOWS
            m_hFile = CreateFileW(pFileName, GENERIC_READ | GENERIC_WRITE,nShareFlag,
			NULL,nOpenFlag, nAttributsFlags,NULL);
		if (m_hFile == INVALID_HANDLE_VALUE)
		{
			if (GetLastError() == ERROR_FILE_NOT_FOUND)
				return ERR_FILE_NOT_FOUND;

			return ERR_OPEN_FILE_FAIL;
		}

		wcscpy_s(m_sFileName, _countof(m_sFileName), pFileName);

		return 0;
#else
            int Flag = O_RDWR;
            if (nOpenFlag == CREATE_ALWAYS)
            {
                Flag |= O_CREAT;
                if (FileExist(pFileName))
                {
                    Delete(pFileName);
                }
            }
            else if (nOpenFlag == OPEN_ALWAYS)
            {
                if (!FileExist(pFileName))
                {
                    Flag |= O_CREAT;
                }
            }
            nShareFlag = 0;
            if (nShareFlag == 0)
            {
                //Flag = O_EXCL;
            }

            if (nAttributsFlags & FILE_FLAG_NO_BUFFERING)
            {
                Flag |= O_SYNC;
            }

            m_hFile = open(pFileName, Flag, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
            if (m_hFile < 0)
            {
                return ERR_OPEN_FILE_FAIL;
            }
            strncpy(m_sFileName, pFileName, sizeof(m_sFileName));

            return 0;
#endif
        }
        int IsOpen()
        {
#ifdef _WINDOWS
            return m_hFile != INVALID_HANDLE_VALUE;
#else
            return m_hFile != -1;
#endif

        }
        int Read(void* pBuffer, int nToRead, int& nReaded)
        {
#ifdef _WINDOWS
            BOOL ret = ReadFile(m_hFile, pBuffer, (DWORD)nToRead, (DWORD*)&nReaded, NULL);
		if (!ret)
			return ERR_READ_FILE_FAIL;

		return 0;

#else
            int ret = read(m_hFile, pBuffer, nToRead);
            if (ret == -1)
                return ERR_READ_FILE_FAIL;

            nReaded = ret;
            return 0;
#endif
        }

        int Write(void* pBuffer, int nToWrite, int& nWritten)
        {
#ifdef _WINDOWS
            BOOL ret = WriteFile(m_hFile, pBuffer, (DWORD)nToWrite, (DWORD*)&nWritten, NULL);
		if (!ret)
			return ERR_WRITE_FILE_FAIL;

		return 0;
#else
            int ret = write(m_hFile, pBuffer, nToWrite);
            if (ret == -1)
                return ERR_READ_FILE_FAIL;

            nWritten = ret;
            return 0;
#endif
        }

        int Seek(LONGLONG llOffset, LONGLONG& llNewOffset, int nMethod)
        {
#ifdef _WINDOWS
            LARGE_INTEGER DistanceToMove, NewFilePointer;

		DistanceToMove.QuadPart = llOffset;

		int ret = SetFilePointerEx(m_hFile, DistanceToMove, &NewFilePointer, nMethod);
		if (!ret)
			return ERR_SEEK_FILE_FAIL;

		llNewOffset = NewFilePointer.QuadPart;

		return 0;

#else
            off_t ret = lseek(m_hFile, llOffset, nMethod);
            if(ret == -1)
                return ERR_SEEK_FILE_FAIL;

            llNewOffset = ret;

            return 0;
#endif
        }

        int Close()
        {
#ifdef _WINDOWS
            if (m_hFile != INVALID_HANDLE_VALUE)
		{
			CloseHandle(m_hFile);
			m_hFile = INVALID_HANDLE_VALUE;
		}
		return 0;

#else
            if (m_hFile != -1)
            {
                close(m_hFile);
                m_hFile = -1;
            }
            return 0;
#endif
        }

        int Flush()
        {
#ifdef _WINDOWS
            BOOL ret = FlushFileBuffers(m_hFile);
		if (!ret)
			return ERR_FLUSH_FILE_FAIL;

		return 0;
#else
#ifdef NOT_SUPPORT_SYNCFS
            int ret = fsync(m_hFile);
#else
            int ret = syncfs(m_hFile);
#endif
            if (ret)
                return ERR_FLUSH_FILE_FAIL;

            return 0;
#endif
        }

        int SetSparse()
        {
#ifdef _WINDOWS
            DWORD r;

		BOOL ret = DeviceIoControl(m_hFile,FSCTL_SET_SPARSE,NULL,0,NULL,0,&r,0);
		if (!ret)
		{
			return ERR_OPERATE_FILE_FAIL;
		}

		return 0;
#else
            return -1;
#endif
        }

        int ZeroFile(LONGLONG llOffset, LONGLONG llLength)
        {
#ifdef _WINDOWS
            DWORD r;

		FILE_ZERO_DATA_INFORMATION Info;
		Info.FileOffset.QuadPart = llOffset;
		Info.BeyondFinalZero.QuadPart = llOffset + llLength;

		BOOL ret = DeviceIoControl(m_hFile, FSCTL_SET_ZERO_DATA, &Info, sizeof(Info), NULL, 0, &r, 0);
		if (!ret)
		{
			return ERR_OPERATE_FILE_FAIL;
		}

		return 0;
#else
            return -1;
#endif
        }

        int GetFileSize(LONGLONG& llLen)
        {
#ifdef _WINDOWS

            LARGE_INTEGER FileSize;
		BOOL ret = GetFileSizeEx(m_hFile, &FileSize);

		if (!ret)
			return ERR_OPEN_FILE_FAIL;

		llLen = FileSize.QuadPart;

		return 0;
#else
            struct stat buf;
            if (fstat(m_hFile, &buf) == 0)
            {
                llLen = buf.st_size;

                return 0;
            }
            return ERR_OPEN_FILE_FAIL;
#endif
        }

        int Delete()
        {
            if (!FileExist(m_sFileName))
            {
                return 0;
            }

            Close();

#ifdef _WINDOWS
            if (DeleteFileW(m_sFileName))
		{
			return 0;
		}
#else
            if (remove(m_sFileName) == 0)
            {
                return 0;
            }
#endif

            return ERR_DELETE_FILE_FAIL;
        }

        static int Delete(WCHAR* pFileName)
        {
            if (!FileExist(pFileName))
            {
                return 0;
            }
#ifdef _WINDOWS
            if (DeleteFileW(pFileName))
		{
			return 0;
		}
#else
            if (remove(pFileName) == 0)
            {
                return 0;
            }
#endif

            return ERR_DELETE_FILE_FAIL;
        }

        static int CreateFolder(WCHAR* pFolderName)
        {
#ifdef _WINDOWS

            WCHAR* p = wcschr(pFolderName, SLASH);
		while (p)
		{
			p = wcschr(p+1, SLASH);
			if (!p)
				break;
			WCHAR c = *(p + 1);
			*(p + 1) = 0;

			int ret = CreateDirectoryW(pFolderName, 0);
			if (!ret)
			{
				if (GetLastError() != ERROR_ALREADY_EXISTS)
					return ERR_CREATE_FOLDER_FAIL;
			}

			*(p + 1) = c;
		}

		return 0;
#else
            WCHAR* p = strchr(pFolderName, SLASH);
            while (p)
            {
                WCHAR c = *(p + 1);
                *(p + 1) = 0;

                DIR* pDir = opendir(pFolderName);
                if (pDir == NULL)
                {
                    int ret = mkdir(pFolderName, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
                    if (ret)
                    {
                        if (errno != EEXIST)
                            return ERR_CREATE_FOLDER_FAIL;
                    }
                }
                else
                {
                    closedir(pDir);
                }
                *(p + 1) = c;

                p = strchr(p + 1, SLASH);
                if (!p)
                    break;
            }

            return 0;
#endif
        }

        static int DeleteFolder(WCHAR* pFolderName)
        {
#ifdef _WINDOWS

            int ret = RemoveDirectory(pFolderName, 0);
		if (!ret)
		{
			return ERR_DELETE_FOLDER_FAIL;
		}
		return 0;
#else
            int ret = rmdir(pFolderName);
            if (ret)
            {
                return ERR_DELETE_FOLDER_FAIL;
            }
            return 0;
#endif
        }

        static int GetSectorSize(const WCHAR* pFolderName,int& nSectorSize)
        {
#ifdef _WINDOWS
            DWORD SectorsPerCluster, BytesPerSector, NumberOfFreeClusters, TotalNumberOfClusters;
		if (GetDiskFreeSpaceW(pFolderName,
			&SectorsPerCluster,
			&BytesPerSector,
			&NumberOfFreeClusters,
			&TotalNumberOfClusters))
		{
			nSectorSize = BytesPerSector;
			return 0;
		}

		return ERR_INVALID_PARA;
#else
            return -1;
#endif
        }

        static int GetDiskSize(const WCHAR* pFolderName, LONGLONG& nDiskFreeSize, LONGLONG& nDiskSize)
        {
#ifdef _WINDOWS
            DWORD SectorsPerCluster, BytesPerSector, NumberOfFreeClusters, TotalNumberOfClusters;
		if (GetDiskFreeSpaceW(pFolderName,
			&SectorsPerCluster,
			&BytesPerSector,
			&NumberOfFreeClusters,
			&TotalNumberOfClusters))
		{
			nDiskFreeSize = NumberOfFreeClusters;
			nDiskFreeSize *= BytesPerSector* SectorsPerCluster;

			nDiskSize = TotalNumberOfClusters;
			nDiskSize *= BytesPerSector * SectorsPerCluster;
			return 0;
		}

		return ERR_INVALID_PARA;
#else
            return -1;
#endif
        }

        int GetBinaryPath(WCHAR* pFolderName, int nSize)
        {
#ifdef _WINDOWS
            return GetModuleFileName(NULL,pFolderName, nSize);
#else
            return -1;
#endif
        }

        static int Rename(WCHAR* pFromFileName, WCHAR* pToFileName)
        {
#ifdef _WINDOWS
            if (MoveFileEx(pFromFileName,pToFileName,MOVEFILE_REPLACE_EXISTING))
		{
			return 0;
		}
#else
            if (rename(pFromFileName, pToFileName) == 0)
            {
                return 0;
            }
#endif
            return ERR_INVALID_PARA;
        }

        int FileExist()
        {
#ifdef _WINDOWS
            if (_waccess(m_sFileName, 0) == 0)
		{
			return 1;
		}

		return 0;
#else
            if (access(m_sFileName, 0) == 0)
            {
                return 1;
            }

            return 0;
#endif
        }

        static int FileExist(WCHAR* pFileName)
        {
#ifdef _WINDOWS
            if (_waccess(pFileName, 0) == 0)
		{
			return 1;
		}

		return 0;
#else
            if (access(pFileName, 0) == 0)
            {
                return 1;
            }

            return 0;
#endif
        }

        static HANDLE FindFirstFile(WCHAR* pFolder, FIND_DATA* lpFindFileData)
        {
#ifdef _WINDOWS
            HANDLE hFind = FindFirstFile(pFolder,lpFindFileData);

		return hFind;
#else

            DIR* dp = opendir(pFolder);

            if (dp == NULL)
            {
                return (HANDLE)-1;
            }

            struct dirent* dirp = readdir(dp);
            if (dirp != NULL)
            {
                strncpy(lpFindFileData->cFileName, dirp->d_name, PATH_SIZE);
                lpFindFileData->dwFileAttributes = 0;
                if (dirp->d_type == DT_DIR)
                {
                    lpFindFileData->dwFileAttributes |= FILE_ATTRIBUTE_DIRECTORY;
                }
            }

            return (HANDLE)dp;
#endif
        }

        static int FindNextFile(HANDLE hFind, FIND_DATA* lpFindFileData)
        {
#ifdef _WINDOWS
            if(FindNextFile(pFolder, lpFindFileData))
			return 0;
#else
            struct dirent* dirp = readdir((DIR *)hFind);
            if (dirp != NULL)
            {
                strncpy(lpFindFileData->cFileName, dirp->d_name, PATH_SIZE);
                lpFindFileData->dwFileAttributes = 0;
                if (dirp->d_type == DT_DIR)
                {
                    lpFindFileData->dwFileAttributes |= FILE_ATTRIBUTE_DIRECTORY;
                }

                return 0;
            }
#endif
            return ERR_NO_MORE;
        }

        static int FindClose(HANDLE hFind)
        {
#ifdef _WINDOWS
            if(FindClose(hFind))
			return 0;
#else
            if (closedir((DIR*)hFind) == 0)
                return 0;
#endif
            return ERR_INVALID_PARA;
        }

    };
}


#endif //UNTITLED13_MYFUNTION_H
