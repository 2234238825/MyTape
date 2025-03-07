//
// Created by Admin on 2025/2/21.
//
#ifdef __WINDOWS__
#include <winsock2.h>
#include <windows.h>
#endif
#include <iterator>
#include "CSerialization.h"
char* CBase_Archive::GetBuff(int &len)
{
    len = 0;
    char *ptr = NULL;
    CBase_Archive_Node *cur;
    list<CBase_Archive_Node *> stack;
    cur = m_Root;
    stack.clear();
    while ( cur != NULL ){
        len += cur->GetSize();
        if ( cur->m_pSub ){
            if ( cur->m_pNext != NULL )
                stack.push_back(cur);
            cur = cur->m_pSub;
        }
        else if( cur->m_pNext ){
            cur = cur->m_pNext;
        }
        else{
            cur = NULL;
        }
        if ( cur == NULL ){
            if ( !stack.empty() ){
                cur = stack.back();
                cur = cur->m_pNext;
                stack.pop_back();
            }
        }
    }
    if ( len != 0 )
        ptr = new char[len];
    if ( ptr == NULL )
        return ptr;

    int pos=0;
    cur = m_Root;
    stack.clear();
    while ( cur != NULL ){
        memcpy(ptr+pos,cur->m_Data, cur->GetSize());  // Flawfinder: ignore
        pos += cur->GetSize();
        if ( cur->m_pSub ){
            if ( cur->m_pNext != NULL )
                stack.push_back(cur);
            cur = cur->m_pSub;
        }
        else if( cur->m_pNext ){
            cur = cur->m_pNext;
        }
        else{
            cur = NULL;
        }
        if ( cur == NULL ){
            if ( !stack.empty() ){
                cur = stack.back();
                cur = cur->m_pNext;
                stack.pop_back();
            }
        }
    }

    return ptr;
}

bool  CBase_Archive::BuidFromBuff(char *ptr, int len)
{
    bool bRet = true;
    if ( m_Root != NULL || m_Pos != NULL || ptr == NULL || len == 0 )
        return false;
    int pos = 0;
    int count = 0;
    CBase_Archive_Node *cur=NULL;
    list< pair<CBase_Archive_Node *,int> > stack;
    stack.clear();
    bool bSub = false;
    while ( (len-pos) >= (int)sizeof(CBase_Archive_Node_Struct) ){
        CBase_Archive_Node_Struct *pb = (CBase_Archive_Node_Struct *)(ptr + pos);
        CBase_Archive_Node *par = new CBase_Archive_Node; assert( par != NULL );
        par->SetDataBuff(pb);
        pos += par->GetSize();
        if ( m_Root == NULL ){
            cur = m_Pos = m_Root = par;
        }
        else{
            if ( bSub ){
                cur->m_pSub = par;
                bSub = false;
            }
            else{
                if ( count == 0 ){
                    if ( !stack.empty() ){
                        pair<CBase_Archive_Node *,int> p2 = stack.back();
                        stack.pop_back();
                        cur = p2.first;
                        count = p2.second;
                    }
                    else{
                        cur = m_Pos;
                        m_Pos = par;
                        ++ m_nRootCount;
                    }
                }
                cur->m_pNext = par;
            }
            cur = par;
            --count;
        }

        switch(pb->type){
            case ARCHIVE_TYPE_OBJ:
            case ARCHIVE_TYPE_STL:
            case ARCHIVE_TYPE_ARRAY:
                if ( pb->count > 0 && pb->len == 0 ){
                    if ( count > 0 )
                        stack.push_back(pair<CBase_Archive_Node *,int>(cur,count));
                    count = pb->count;
                    bSub = true;
                }
                break;
            case ARCHIVE_TYPE_BASE:
            case ARCHIVE_TYPE_STR:
            case ARCHIVE_TYPE_WSTR:
                break;
            default:
                return false;
                break;
        }
    }
    m_Pos = m_Root;
    return bRet;
}

CBase_Archive_Node *CBase_Archive::Pop_Root(int &count)
{
    CBase_Archive_Node *ptr = m_Root;
    count = m_nRootCount;
    m_Root = NULL;
    ClearRoot();
    return ptr;
}

void CBase_Archive::Set_Root(CBase_Archive_Node *ptr, const int count)
{
    m_Pos = m_Root = ptr;
    m_nRootCount = count;
}

template<typename T>
static void TypeInfo(T& t)
{

    cout << typeid(T).name() << endl;
};

