//
// Created by Admin on 2025/2/21.
//

#ifndef UNTITLED13_CSERIALIZATION_H
#define UNTITLED13_CSERIALIZATION_H

#include <list>
#include <vector>
#include <assert.h>
#include <iostream>
#include <sstream>
#include <typeinfo>
#include <stdlib.h>
#include <string.h>


using namespace std;

enum {
    ARCHIVE_TYPE_OBJ=0,
    ARCHIVE_TYPE_BASE,
    ARCHIVE_TYPE_STL,
    ARCHIVE_TYPE_ARRAY,
    ARCHIVE_TYPE_STR,
    ARCHIVE_TYPE_WSTR,
    ARCHIVE_TYPE_MAX,
};

enum {
    BASE_SUBTYPE_NULL = 0,
    BASE_SUBTYPE_INT,
    BASE_SUBTYPE_UINT,
    BASE_SUBTYPE_LONG,
    BASE_SUBTYPE_ULONG,
    BASE_SUBTYPE_INT32,
    BASE_SUBTYPE_UINT32,
    BASE_SUBTYPE_INT64,
    BASE_SUBTYPE_UINT64,
    BASE_SUBTYPE_BOOL,
    BASE_SUBTYPE_CHAR,
    BASE_SUBTYPE_UCHAR,
    BASE_SUBTYPE_WCHAR,
    BASE_SUBTYPE_MAX,
};

enum {
    STL_SUBTYPE_NULL = 0,
    STL_SUBTYPE_LIST,
    STL_SUBTYPE_VECTOR,
    STL_SUBTYPE_MAX,
};

#ifdef __WINDOWS__
#pragma warning(push)
#pragma warning(disable:4200)
#endif

struct CBase_Archive_Node_Struct{
    uint32_t   type:3,
            pointer:2,
            subtype:7,
            count:20;
    uint32_t   len;
    char       data[0]; // Flawfinder: ignore
};

#ifdef __WINDOWS__
#pragma warning(pop)
#endif

static int _gNpNothing = 0;

template<typename T>
static list<int>& TransList(T& t)
{ static list<int> _glist; return _glist; };

template<typename T>
static list<T>& TransList(list<T>& t)
{ return t; };

template<typename T>
static vector<int>& TransVector(T& t)
{ static vector<int> _gvector; return _gvector; };

template<typename T>
static vector<T>& TransVector(vector<T>& t)
{ return t; };

template<typename T>
static T& TransArrayNode(T& t)
{ return t; };

template<typename T>
static T& TransArrayNode(T t[])
{ return t[0]; };

template<typename T>
static T& TransPointer(T &t, int &np=_gNpNothing)
{ np=0; return t;};

template<typename T>
static T& TransPointer(T* t, int &np=_gNpNothing)
{ static T tmp; np=1; if (t) return *t; else return tmp; };

template<typename T>
static T& TransPointer(T** t, int &np=_gNpNothing)
{ static T tmp; np=2; if (t) return **t; else return tmp; };

template<typename T>
static T& TransPointer(T*** t, int &np=_gNpNothing)
{ static T tmp; np=3; if (t) return ***t; else return tmp; };

template<typename T>
static T& TransPointer(T**** t, int &np=_gNpNothing)
{ static T tmp; np=4; if (t) return ****t; else return tmp; };

template<typename T>
static T& TransPointer(T***** t, int &np=_gNpNothing)
{ static T tmp; np=5; if (t) return *****t; else return tmp; };

template<typename T>
static T& TransPointer(T****** t, int &np=_gNpNothing)
{ static T tmp; np=6; if (t) return ******t; else return tmp; };

template<typename T>
static T& TransPointer(T******* t, int &np=_gNpNothing)
{ static T tmp; np=7; if (t) return *******t; else return tmp; };

template<typename T>
static T& TransPointer(T******** t, int &np=_gNpNothing)
{ static T tmp; np=8; if (t) return ********t; else return tmp; };

template<typename T>
static T& TransPointer(T********* t, int &np=_gNpNothing)
{ static T tmp; np=9; if (t) return *********t; else return tmp; };

template<typename T>
static T& TransPointer(T********** t, int &np=_gNpNothing)
{ static T tmp; np=9; if (t) return **********t; else return tmp; };

template<typename T>
static int Check_BaseType(T& t, int &stype)
{
    int nType = ARCHIVE_TYPE_BASE;
    if ( typeid(T) == typeid(int) )                stype = BASE_SUBTYPE_INT;
    else if ( typeid(T) == typeid(unsigned int) )  stype = BASE_SUBTYPE_UINT;
    else if ( typeid(T) == typeid(long) )          stype = BASE_SUBTYPE_LONG;
    else if ( typeid(T) == typeid(unsigned long) ) stype = BASE_SUBTYPE_ULONG;
    else if ( typeid(T) == typeid(int32_t) )       stype = BASE_SUBTYPE_INT32;
    else if ( typeid(T) == typeid(uint32_t) )      stype = BASE_SUBTYPE_UINT32;
    else if ( typeid(T) == typeid(int64_t) )       stype = BASE_SUBTYPE_INT64;
    else if ( typeid(T) == typeid(uint64_t) )      stype = BASE_SUBTYPE_UINT64;
    else if ( typeid(T) == typeid(unsigned char))  stype = BASE_SUBTYPE_UCHAR;
    else if ( typeid(T) == typeid(bool) )          stype = BASE_SUBTYPE_BOOL;
    else if ( typeid(T) == typeid(char) )          stype = BASE_SUBTYPE_CHAR;
    else if ( typeid(T) == typeid(wchar_t) )       stype = BASE_SUBTYPE_WCHAR;
    else if ( typeid(t) == typeid(string) )        nType = ARCHIVE_TYPE_STR;
    else if ( typeid(t) == typeid(wstring) )       nType = ARCHIVE_TYPE_WSTR;
    else
        return ARCHIVE_TYPE_MAX;
    return nType;
};

template<typename T>
static int Check_StlType(T& t, int &stype)
{
    int nType = Check_BaseType(t,stype);
    if ( nType != ARCHIVE_TYPE_MAX )
        return nType;
    string tn = typeid(t).name();
#ifdef __WINDOWS__
    if ( tn.find("class std::list<") == 0 ){
#else
    if ( tn.find("St4list") == 0 ){
#endif
        nType = ARCHIVE_TYPE_STL;
        stype = STL_SUBTYPE_LIST;
    }
#ifdef __WINDOWS__
        else if ( tn.find("class std::vector<") == 0 ){
#else
    else if ( tn.find("St6vector") == 0 ){
#endif
        nType = ARCHIVE_TYPE_STL;
        stype = STL_SUBTYPE_VECTOR;
    }
#ifdef __WINDOWS__
        else if ( ( tn.find("class") == 0 && tn.find("class std::") != 0 ) || tn.find("struct") == 0 )
#else
    else if ( tn.find("1") == 0 || tn.find("2") == 0 )
#endif
        nType = ARCHIVE_TYPE_OBJ;
    else
        nType = ARCHIVE_TYPE_MAX;

    return nType;
};

template<typename T>
static int Check_ArchiveType(T& t, int &nPointer, int &nArraySize, int &nsType)
{
    nPointer = 0;
    nArraySize = 0;
    nsType = 0;
    string tn = typeid(t).name();
#ifdef __WINDOWS__
    if ( tn[tn.length()-1] == ']' ){
        size_t pos = tn.find_last_of('[');
        string s = tn.substr(pos+1, tn.length()-pos-2);
#else
    if ( tn[0] == 'A' ){
        size_t pos = tn.find_first_of('_');
        string s = tn.substr(1,pos-1);
#endif
        if ( s == "" )
            return ARCHIVE_TYPE_MAX;
        else
            nArraySize = atoi(s.c_str()); // Flawfinder: ignore
        return ARCHIVE_TYPE_ARRAY;
    }
    return Check_StlType(TransPointer(t, nPointer), nsType);
};

template<typename T>
static T* NewValue(T* t)
{ return new T; };

template<typename T>
static bool SetValue(T* t, void* ptr, int nPointer, int len)
{
    if ( nPointer == 0 ){
        if ( ptr != NULL )
            memcpy(t, ptr, len); // Flawfinder: ignore
    }
    else if ( nPointer >= 1 ){
        void* p = NewValue(&TransPointer(*t)); assert(p!=NULL);
        if ( ptr != NULL )
            memcpy(p, ptr, len); // Flawfinder: ignore
        while( nPointer -- > 1 ){
            void **pt = new void*;
            *pt = p;
            p = (void *)pt;
        }
        memcpy(t,&p,sizeof(p)); // Flawfinder: ignore
    }
    else
        return false;
    return true;
};

class CBase_Archive_Node
{
public:
    CBase_Archive_Node(){ m_Data = NULL; m_pSub = NULL; m_pNext = NULL;};
    virtual ~CBase_Archive_Node()
    { if ( m_pNext )
            delete m_pNext;
        if ( m_pSub )
            delete m_pSub;
        if ( m_Data )
            delete m_Data;
    };
    void MallocData(size_t len)
    { FreeData(); m_Data = (CBase_Archive_Node_Struct*) new char[len]; assert(m_Data != NULL); memset(m_Data,0,len); };
    void FreeData()
    { if(m_Data) { delete[] (char*)m_Data; m_Data = NULL; } };
    int GetSize()
    { return m_Data == NULL ? 0 : (int)sizeof(CBase_Archive_Node_Struct) + (int)m_Data->len; };
    void SetDataBuff(CBase_Archive_Node_Struct* ptr)
    { int len = sizeof(CBase_Archive_Node_Struct) + ptr->len; MallocData(len); memcpy(m_Data,ptr,len);}; // Flawfinder: ignore

public:
    CBase_Archive_Node_Struct *m_Data;
    CBase_Archive_Node *m_pSub;
    CBase_Archive_Node *m_pNext;
};

template<typename T>
CBase_Archive_Node *Create_Archive_Tree(T &t);

class CBase_Archive_Node_Base : public CBase_Archive_Node
{
public:
    template<typename T>
    CBase_Archive_Node_Base(T &t, int nPointer, int sType)
    {
        size_t len = sizeof(T);
        MallocData(len + sizeof(CBase_Archive_Node_Struct));
        m_Data->type = ARCHIVE_TYPE_BASE;
        m_Data->pointer = nPointer;
        m_Data->subtype = sType;
        m_Data->len = (int)len;
        memcpy(m_Data->data, &t, len); // Flawfinder: ignore; Destination can always hold the source data
    };

    template<typename T>
    bool RealDeSerialization(T &t)
    {
        return SetValue(&t, m_Data->data, m_Data->pointer, m_Data->len);
    };
};

class CBase_Archive_Node_String : public CBase_Archive_Node
{
public:
    CBase_Archive_Node_String(void *ptr, int nPointer)
    {
        string& str = *(string*)ptr;
        size_t len = str.length() + 1;
        MallocData( len + sizeof(CBase_Archive_Node_Struct) );
        m_Data->type = ARCHIVE_TYPE_STR;
        m_Data->pointer = nPointer;
        m_Data->len = (int)len;
        memcpy(m_Data->data, str.c_str(), len); // Flawfinder: ignore; Destination can always hold the source data
    };

    template<typename T>
    bool RealDeSerialization(T &t)
    {
        if ( m_Data->pointer == 0 ){
            string & str = *(string*)((void*)&t);
            str = m_Data->data;
            return true;
        }
        else{
            string *ps = new string(m_Data->data);
            return SetValue(&t, &ps, m_Data->pointer-1, sizeof(string*));
        }
    };
};

class CBase_Archive_Node_WString : public CBase_Archive_Node
{
public:
    CBase_Archive_Node_WString(void *ptr, int nPointer)
    {
        wstring& wstr = *(wstring*)ptr;
        size_t len = (wstr.length()+1)*sizeof(wchar_t);
        MallocData( len + sizeof(CBase_Archive_Node_Struct) );
        m_Data->type = ARCHIVE_TYPE_WSTR;
        m_Data->pointer = nPointer;
        m_Data->len = (int)len;
        memcpy(m_Data->data, wstr.c_str(), len); // Flawfinder: ignore; Destination can always hold the source data
    };
    template<typename T>
    bool RealDeSerialization(T &t)
    {
        if ( m_Data->pointer == 0 ){
            wstring& wstr = *(wstring*)((void*)&t);
            wstr = (wchar_t*)m_Data->data;
            return true;
        }
        else{
            wstring *ps = new wstring((wchar_t*)m_Data->data);
            return SetValue(&t, &ps, m_Data->pointer-1, sizeof(wstring*));
        }
    };
};

class CBase_Archive_Node_Array : public CBase_Archive_Node
{
public:
    template<typename T>
    CBase_Archive_Node_Array(T &t, int nSize)
    {
        int nPointer =0, nArray = 0, nStype = 0;
        int nType = Check_ArchiveType(TransArrayNode(t), nPointer, nArray, nStype);

        if ( nType == ARCHIVE_TYPE_BASE ){
            size_t len = sizeof(TransPointer(TransArrayNode(t)));
            MallocData( len*nSize + sizeof(CBase_Archive_Node_Struct) );
            m_Data->len = (int)len*nSize;
            for ( int i = 0; i < nSize; ++i)
                memcpy(m_Data->data + i*len, &TransPointer(&TransArrayNode(t) + i), len); // Flawfinder: ignore
        }
        else{
            MallocData( sizeof(CBase_Archive_Node_Struct) );
            CBase_Archive_Node **pp = &m_pSub;
            for( int i = 0; i < nSize; ++i ){
                CBase_Archive_Node *ptr = Create_Archive_Tree(*(&TransArrayNode(t) + i));
                assert( ptr != NULL );
                *pp = ptr;
                pp = &ptr->m_pNext;
            }
        }
        m_Data->type = ARCHIVE_TYPE_ARRAY;
        m_Data->pointer = nPointer;
        m_Data->subtype = nStype;
        m_Data->count = nSize;
    };

    template<typename T>
    bool RealDeSerialization(T &t)
    {
        int nPointer =0, nArray = 0, nStype = 0;
        int nType = Check_ArchiveType(TransArrayNode(t), nPointer, nArray, nStype);

        if ( nType == ARCHIVE_TYPE_BASE ){
            size_t len = sizeof(TransPointer(TransArrayNode(t)));
            for ( int i = 0; i < (int)m_Data->count; ++i ){
                SetValue( &TransArrayNode(t) + i, m_Data->data + i*len, m_Data->pointer, (int)len);
            }
        }
        else{
            CBase_Archive_Node *ptr = m_pSub;
            for ( int i = 0; i < (int)m_Data->count; ++i ){
                DeSerialization(*(&TransArrayNode(t) + i), ptr);
                ptr = ptr->m_pNext;
            }
        }
        return false;
    };
};

class CBase_Archive_Node_List : public CBase_Archive_Node
{
public:
    template<typename T>
    CBase_Archive_Node_List(T& t, int nPointer)
    {
        MallocData( sizeof(CBase_Archive_Node_Struct) );
        m_Data->type = ARCHIVE_TYPE_STL;
        m_Data->pointer = nPointer;
        m_Data->subtype = STL_SUBTYPE_LIST;
        m_Data->len = 0;
        uint32_t nCount = 0;
        m_pSub = CreateList(t, nCount);
        m_Data->count = nCount;
    };
    template<typename T>
    CBase_Archive_Node *CreateList(T& list, uint32_t& count)
    {
        count = (uint32_t)list.size();
        CBase_Archive_Node *proot = NULL, *pp = NULL;
        for ( typename T::iterator it = list.begin(); it != list.end(); ++it ){
            CBase_Archive_Node *ptr = Create_Archive_Tree(*it);
            assert( ptr != NULL );
            if ( proot == NULL )
                pp = proot = ptr;
            else
                pp->m_pNext = ptr;
            pp = ptr;
        }
        return proot;
    };

    template<typename TC>
    bool  DeSerializationList(list<TC> &t, CBase_Archive_Node *ptr)
    {
        if ( ptr == NULL )
            return false;
        TC tmp;
        bool bRet = DeSerialization(tmp, ptr);
        t.push_back(tmp);
        return bRet;
    };

    template<typename TC>
    bool  DeSerializationList(list<TC*> &t, CBase_Archive_Node *ptr)
    {
        if ( ptr == NULL )
            return false;
        TC* tmp = NULL;
        bool bRet = DeSerialization(tmp, ptr);
        t.push_back(tmp);
        return bRet;
    };

    template<typename T>
    bool RealDeSerialization(T &t)
    {
        if ( false == SetValue(&t, NULL, m_Data->pointer, sizeof(TransPointer(t))) ){
            return false;
        }
        CBase_Archive_Node *ptr = m_pSub;
        for( int i=0; i < (int)m_Data->count; ++i ){
            if ( this->DeSerializationList(TransList(TransPointer(t)),ptr) )
                ptr = ptr->m_pNext;
            else
                return false;
        }
        return true;
    };
};

class CBase_Archive_Node_Vector : public CBase_Archive_Node
{
public:
    template<typename T>
    CBase_Archive_Node_Vector(T &t, int nPointer)
    {
        MallocData( sizeof(CBase_Archive_Node_Struct) );
        m_Data->type = ARCHIVE_TYPE_STL;
        m_Data->pointer = nPointer;
        m_Data->subtype = STL_SUBTYPE_VECTOR;
        m_Data->len = 0;
        uint32_t nCount = 0;
        m_pSub = CreateVector(t, nCount);
        m_Data->count = nCount;
    };
    template<typename T>
    CBase_Archive_Node *CreateVector(T& vector, uint32_t& count)
    {
        count = (uint32_t)vector.size();
        CBase_Archive_Node *proot = NULL, *pp = NULL;
        for ( typename T::iterator it = vector.begin(); it != vector.end(); ++it ){
            CBase_Archive_Node *ptr = Create_Archive_Tree(*it);
            assert( ptr != NULL );
            if ( proot == NULL )
                pp = proot = ptr;
            else
                pp->m_pNext = ptr;
            pp = ptr;
        }
        return proot;
    };

    template<typename TC>
    bool  DeSerializationVector(vector<TC> &t, CBase_Archive_Node *ptr)
    {
        if ( ptr == NULL )
            return false;
        TC tmp;
        bool bRet = DeSerialization(tmp, ptr);
        t.push_back(tmp);
        return bRet;
    };

    template<typename TC>
    bool  DeSerializationVector(vector<TC*> &t, CBase_Archive_Node *ptr)
    {
        if ( ptr == NULL )
            return false;
        TC* tmp = NULL;
        bool bRet = DeSerialization(tmp, ptr);
        t.push_back(tmp);
        return bRet;
    };

    template<typename T>
    bool RealDeSerialization(T &t)
    {
        if ( false == SetValue(&t, NULL, m_Data->pointer, sizeof(TransPointer(t))) ){
            return false;
        }
        CBase_Archive_Node *ptr = m_pSub;
        for( int i=0; i < (int)m_Data->count; ++i ){
            if ( DeSerializationVector(TransVector(TransPointer(t)),ptr) )
                ptr = ptr->m_pNext;
            else
                return false;
        }
        return true;
    };
};

class CBase_Archive
{
public:
    CBase_Archive(){ m_Root = NULL; m_Pos = NULL; m_nRootCount = 0;};
    virtual ~CBase_Archive() { ClearRoot(); };

    char* GetBuff(int &len);
    bool  BuidFromBuff(char *ptr, int len);

    CBase_Archive_Node *Pop_Root(int &count) ;
    void Set_Root(CBase_Archive_Node *ptr, int count);

protected:
    inline void ClearRoot(){ if (m_Root) delete m_Root; m_Pos = m_Root = NULL; m_nRootCount = 0; };

protected:
    CBase_Archive_Node  *m_Root;
    CBase_Archive_Node  *m_Pos;
    int  m_nRootCount;
};

class CBinary_OutArchive;
class CBinary_InArchive;
class CBaseSerialization
{
public:
    virtual ~CBaseSerialization() {};
    virtual void Serialization(CBinary_OutArchive &oa){ cout<<"CBinary_OutArchive"<<endl; };
    virtual void Serialization(CBinary_InArchive &ia){ cout<<"CBinary_InArchive"<<endl; };
};




#endif //UNTITLED13_CSERIALIZATION_H
