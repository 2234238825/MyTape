//
// Created by Admin on 2025/2/21.
//

#ifndef UNTITLED13_CONVERTJSON_H
#define UNTITLED13_CONVERTJSON_H
#include "json/json.h"
#include "CSerialization.h"

#ifdef __WINDOWS__
#include "Util.h"
//#include "WTypes.h"
#else
#include <iconv.h>
#endif
#include <string>
#include <map>
#include <sstream>
using namespace std;
enum
{
    E_CODE_START = 1000000000,
    E_CNVJSON_CODECNV_ICONV_OPEN,
    E_CNVJSON_CODECNV_ICONV,
    E_CNVJSON_CREATE_ELE2JSON_VECTOR,
    E_CNVJSON_CREATE_ELE2JSON_LIST,
    E_CNVJSON_CREATE_ELE2JSON_ARRAY,
    E_CNVJSON_JSON2ELE_JSONOBJECT_SIZE,
    E_CNVJSON_JSON2ELE_ARRAY_SIZE
};
#define  I2DEF_BUFF_SIZE 1024

inline std::string Json2String(const Json::Value &root)
{
    Json::StreamWriterBuilder writerBuilder;
    writerBuilder.settings_["indentation"] = "";
    JSONCPP_STRING  dataStr = Json::writeString(writerBuilder, root);
    return dataStr;
}

inline bool String2Json(Json::Value &Value, const std::string &Strbuf)
{
    bool bRes = false;
    JSONCPP_STRING errs;
    Json::CharReaderBuilder readerBuilder;
    Json::CharReader *pRead = readerBuilder.newCharReader();

    if (pRead)
    {
        bRes = pRead->parse(Strbuf.c_str(), Strbuf.c_str() + Strbuf.length(), &Value, &errs);
        delete pRead;
    }

    return bRes;
}
namespace ConvJson{
    template<typename T>
    int Struct2Json(T &src, Json::Value &dst)
    {
        return src.ConvJson_Struct2Json(dst);
    }

    template<typename T>
    int Json2Struct(T &dst, const Json::Value &src)
    {
        return dst.ConvJson_JSon2Struct(src);
    }
    template <typename T>
    T Json2Struct(Json::Value &src) {
        T dst;
        Json2Struct(dst, src);
        return dst;
    }
    template <typename T>
    T Json2Struct(const std::string& json_str) {
        Json::Value src;
        String2Json(src, json_str);
        return Json2Struct<T>(src);
    }
};

#ifndef __WINDOWS__
inline int code_convert(const char* from_charset,const char* to_charset,char* tmpin, char* tmpout, size_t inlen )
{
    iconv_t cd = iconv_open(to_charset, from_charset);
    if (cd == (iconv_t)-1)
    { return -E_CNVJSON_CODECNV_ICONV_OPEN; }
    size_t outlen = I2DEF_BUFF_SIZE;
#ifdef TYPECONV
    size_t ret = iconv(cd, (const char**)(&tmpin), &inlen, &tmpout, &outlen);
#else
    size_t ret = iconv(cd, &tmpin, &inlen, &tmpout, &outlen);
#endif
    if (ret == (size_t)-1)
    { return -E_CNVJSON_CODECNV_ICONV; }
    iconv_close(cd);
}

inline int unicode_to_utf8(const wchar_t* inbuf, size_t inlen, char *outbuf)
{

    char* tmpin = (char*)inbuf;
    code_convert("UNICODE","UTF-8",tmpin, outbuf, inlen);

    return 0;
}

inline int utf8_to_unicode(const char* inbuf, size_t inlen, wchar_t* outbuf)
{

    char* tmpin = (char*)inbuf;
    char* tmpout = (char*)outbuf;
    code_convert("UTF-8","UNICODE",tmpin, tmpout, inlen);

    return 0;
}
#endif


template<typename T>
inline int ConvjsonAppendElement(T e,Json::Value &lists)
{ Json::Value tmp; e.ConvJson_Struct2Json(tmp); lists.append(tmp); return 0; }
template<>
inline int ConvjsonAppendElement(std::string e,Json::Value &lists)
{ lists.append(e); return 0;}
template<>
inline int ConvjsonAppendElement(int32_t e,Json::Value &lists)
{lists.append(e); return 0;}
template<>
inline int ConvjsonAppendElement(uint32_t e,Json::Value &lists)
{ lists.append(e); return 0;}
template<>
inline int ConvjsonAppendElement(int64_t e,Json::Value &lists)
{ lists.append(e); return 0;}
template<>
inline int ConvjsonAppendElement(uint64_t e,Json::Value &lists)
{ lists.append(e); return 0;}
template<>
inline int ConvjsonAppendElement(int16_t e, Json::Value &lists)
{ lists.append(e); return 0;}
template<>
inline int ConvjsonAppendElement(uint16_t e, Json::Value &lists)
{ lists.append(e); return 0;}
template<>
inline int ConvjsonAppendElement(int8_t &e, Json::Value &lists)
{ lists.append(e); return 0;}
template<>
inline int ConvjsonAppendElement(uint8_t e, Json::Value &lists)
{ lists.append(e); return 0;}
template<>
inline int ConvjsonAppendElement(bool e, Json::Value &lists)
{ lists.append(e); return 0;}
template<>
inline int ConvjsonAppendElement(double e, Json::Value &lists)
{ lists.append(e); return 0;}
template<>
inline int ConvjsonAppendElement(char e,Json::Value &lists)
{ int8_t t = (int8_t)e; lists.append(TransPointer(t)); return 0;}
template<>
inline int ConvjsonAppendElement(char* e,Json::Value &lists)
{ std::string str = e; lists.append(TransPointer(str)); return 0;}
template<>
inline int ConvjsonAppendElement(wchar_t e,Json::Value &lists)
{
#ifdef __WINDOWS__
    int16_t t = (int16_t)e;
#else
    int32_t t = (int32_t)e;
#endif
    lists.append(TransPointer(t));
    return 0;
}
template<>
inline int ConvjsonAppendElement(wchar_t* e,Json::Value &lists)
{
#ifdef  __WINDOWS__
    char* str = NULL;
    str = WIDE_2_MULTI(e,CP_UTF8);
    std::string retStr = str;
#else
    wchar_t inbuf[I2DEF_BUFF_SIZE] = {};
    char outbuf[I2DEF_BUFF_SIZE] = {};
    memcpy(inbuf,e,I2DEF_BUFF_SIZE);
    size_t inlen = wcslen(inbuf)*4;
    unicode_to_utf8(inbuf, inlen, outbuf);
    std::string retStr = outbuf;
#endif
    lists.append(TransPointer(retStr));
    return 0;
}
template<>
inline int ConvjsonAppendElement(wstring e,Json::Value &lists)
{
    wchar_t inbuf[I2DEF_BUFF_SIZE] = {};
    char outbuf[I2DEF_BUFF_SIZE] = {};
    memcpy(inbuf,e.c_str(),I2DEF_BUFF_SIZE);
#ifdef  __WINDOWS__
    char* ret = NULL;
    ret = WIDE_2_MULTI(inbuf,CP_UTF8);
    std::string retStr = ret;
#else
    size_t inlen = wcslen(inbuf)*4;
    unicode_to_utf8(inbuf, inlen, outbuf);
    std::string retStr = outbuf;
#endif
    lists.append(TransPointer(retStr));
    return 0;
}





template<typename T>
inline int CreateElementJson(T element, const std::string &name, Json::Value &dest)
{ Json::Value tmp; element.ConvJson_Struct2Json(tmp); dest[name] = tmp; return 0; }
template<typename T>
inline int CreateElementJson(T element[], const std::string &name, Json::Value &dest)
{ return 0; }
template <>
inline int CreateElementJson(std::string element, const std::string &name, Json::Value &dest)
{  dest[name] = TransPointer(element);  return 0; }
template<>
inline int CreateElementJson(int32_t element, const std::string &name, Json::Value &dest)
{  dest[name] = element;  return 0; }
template<>
inline int CreateElementJson(uint32_t element, const std::string &name, Json::Value &dest)
{ dest[name] = element;  return 0;}
template<>
inline int CreateElementJson(int64_t element, const std::string &name, Json::Value &dest)
{ dest[name] = element; return 0;}
template<>
inline int CreateElementJson(uint64_t element, const std::string &name, Json::Value &dest)
{ dest[name] = element; return 0;}
template<>
inline int CreateElementJson(int16_t element, const std::string &name, Json::Value &dest)
{ dest[name] = element; return 0;}
template<>
inline int CreateElementJson(uint16_t element, const std::string &name, Json::Value &dest)
{ dest[name] = element; return 0;}
template<>
inline int CreateElementJson(bool element, const std::string &name, Json::Value &dest)
{  dest[name] = element; return 0;}
template<>
inline int CreateElementJson(int8_t element, const std::string &name, Json::Value &dest)
{ dest[name] = element; return 0;}
template<>
inline int CreateElementJson(double element, const std::string &name, Json::Value &dest)
{  dest[name] = element;  return 0;}
template<>
inline int CreateElementJson(uint8_t element, const std::string &name, Json::Value &dest)
{ dest[name] = element; return 0;}
template<>
inline int CreateElementJson(char element, const std::string &name, Json::Value &dest)
{ int8_t t = (int8_t)element; dest[name] = TransPointer(t);  return 0; }
template<>
inline int CreateElementJson(char* element, const std::string &name, Json::Value &dest)
{ std::string str = element; dest[name] = TransPointer(str); return 0;}
template<>
inline int CreateElementJson(wchar_t element, const std::string &name, Json::Value &dest)
{
#ifdef __WINDOWS__
    int16_t t = (int16_t)element;
#else
    int32_t t = (int32_t)element;
#endif
    dest[name] = TransPointer(t);
    return 0;
}


template<>
inline int CreateElementJson(wchar_t* element, const std::string &name, Json::Value &dest)
{
#ifdef __WINDOWS__
    char* str = NULL;
    str = WIDE_2_MULTI(element,CP_UTF8);
    std::string retStr = str;
#else
    wchar_t inbuf[I2DEF_BUFF_SIZE] = {};
    char outbuf[I2DEF_BUFF_SIZE] = {};

    memcpy(inbuf,element,I2DEF_BUFF_SIZE);
    size_t inlen = wcslen(inbuf)*4;
    unicode_to_utf8(inbuf, inlen, outbuf);
    std::string retStr = outbuf;
#endif
    dest[name] = TransPointer(retStr);
    return 0;
}
template<>
inline int CreateElementJson(wstring element, const std::string &name, Json::Value &dest)
{
    wchar_t inbuf[I2DEF_BUFF_SIZE] = {};
    char outbuf[I2DEF_BUFF_SIZE] = {};
    memcpy(inbuf,element.c_str(),I2DEF_BUFF_SIZE);
#ifdef __WINDOWS__
    char* str = NULL;
    str = WIDE_2_MULTI(inbuf,CP_UTF8);
    std::string retStr = str;
#else
    size_t inlen = wcslen(inbuf)*4;
    unicode_to_utf8(inbuf, inlen, outbuf);
    std::string retStr = outbuf;
#endif
    dest[name] = TransPointer(retStr);
    return 0;
}
template<typename T>
inline int CreateElementJson(std::vector<T> vec, const std::string &name, Json::Value &dest)
{
    Json::Value lst;
    if(vec.empty())
    {
        return -E_CNVJSON_CREATE_ELE2JSON_VECTOR;
    }
    typename std::vector<T>::iterator iter=vec.begin();
    for(; iter!=vec.end(); iter++){
        //T tmp = *iter;
        ConvjsonAppendElement((T)(*iter),lst);
    }
    dest[name] = lst;
    return 0;
}
template<typename T>
inline int CreateElementJson(std::map<std::string,T>& element, const std::string &name, Json::Value &dest)
{
    Json::Value retList;
    if(element.empty())
    {
        return -10108;
    }
    typename std::map<std::string,T>::iterator kv = element.begin();
    for (; kv != element.end(); kv++) {
        Json::Value item;
        item[kv->first] = kv->second;
        retList.append(item);
    }

    dest[name] = retList;
    return 0;
}
template<typename T>
inline int CreateElementJson(std::list<T> element, const std::string &name, Json::Value &dest)
{
    Json::Value retList;
    if(element.empty())
    {
        return -E_CNVJSON_CREATE_ELE2JSON_LIST;
    }
    typename std::list<T>::iterator iter = element.begin();
    for(; iter!=element.end(); iter++){

        ConvjsonAppendElement((T)(*iter),retList);
    }
    dest[name] = retList;
    return 0;
}

template<typename T>
int CreateArray(T element[], int nArrary, Json::Value &lists)
{
    if(nArrary == 0)
    {
        return -E_CNVJSON_CREATE_ELE2JSON_ARRAY;
    }
    for ( int n=0; n<nArrary; ++n ){
        ConvjsonAppendElement(element[n],lists);
    }
    return 0;
}
template<typename T>
int CreateArray(T &element, int nArrary, Json::Value &lists)
{ return 0; }


template<typename T>
int ConvJson_Element2Json(T &element, const std::string &name, Json::Value &dest)
{

    int nPointer = 0;
    int nArrary = 0;
    int nStype = 0;
    int nType = Check_ArchiveType(element, nPointer, nArrary, nStype);

    Json::Value lst;
    if (nType == ARCHIVE_TYPE_ARRAY){
        CreateArray(element,nArrary,lst);
        dest[name] = lst;
    }
    else{
        CreateElementJson(element, name, dest);
    }

    return 0;
};






template<typename T>
inline int _CreateJsonElement(T *val,const Json::Value &listE)
{ return 0; }
template<typename T>
inline int _CreateJsonElement(T &val,const Json::Value &listE)
{ val.ConvJson_JSon2Struct(listE); return 0; }

template<typename T>
inline int CreateJsonElement(T &val,const Json::Value &listE)
{ return _CreateJsonElement(val,listE); }
template<typename T>
inline int CreateJsonElement(std::list<T> &val,const Json::Value &listE)
{  return 0; }
template<typename T>
inline int CreateJsonElement(std::vector<T> &val,const Json::Value &listE)
{  return 0; }
template<typename T>
inline int CreateJsonElement(std::map<std::string,T> &val,const Json::Value &listE)
{  return 0; }
template<>
inline int CreateJsonElement(char* &val, const Json::Value &listE)
{ std::string str = listE.asString(); val = new char[str.length()+1];memcpy(val,str.c_str(),str.length()+1);   return 0; }
template<>
inline int CreateJsonElement(std::string &val, const Json::Value &listE)
{ val = listE.asString();   return 0;	 }
template<>
inline int CreateJsonElement(int32_t &val, const Json::Value &listE)
{ val = listE.asInt();   return 0; }
template<>
inline int CreateJsonElement(uint32_t &val, const Json::Value &listE)
{ val = listE.asUInt(); return 0; }
template<>
inline int CreateJsonElement(int64_t &val, const Json::Value &listE)
{ val = listE.asInt64(); return 0; }
template<>
inline int CreateJsonElement(uint64_t &val, const Json::Value &listE)
{ val = listE.asUInt64();  return 0;	}
template<>
inline int CreateJsonElement(int16_t &val, const Json::Value &listE)
{ val = listE.asInt(); return 0; }
template<>
inline int CreateJsonElement(uint16_t &val, const Json::Value &listE)
{ val = listE.asUInt(); return 0; }
template<>
inline int CreateJsonElement(int8_t &val, const Json::Value &listE)
{ val = listE.asInt(); return 0; }
template<>
inline int CreateJsonElement(uint8_t &val, const Json::Value &listE)
{ val = listE.asUInt(); return 0; }
template<>
inline int CreateJsonElement(bool &val, const Json::Value &listE)
{ val = listE.asBool();   return 0; }
template<>
inline int CreateJsonElement(double &val, const Json::Value &listE)
{ val = listE.asDouble();   return 0; }
template<>
inline int CreateJsonElement(char &val, const Json::Value &listE)
{ int8_t ele = listE.asInt(); val = ele;   return 0; }

////
template<>
inline int CreateJsonElement(wchar_t &val, const Json::Value &listE)
{
#ifdef __WINDOWS__
    int16_t ele = listE.asUInt();
#else
    int32_t ele = listE.asUInt();
#endif
    val = ele;

    return 0;
}
template<>
inline int CreateJsonElement(wchar_t* &val, const Json::Value &listE)
{
    std::string str = listE.asString();
    char inbuf[I2DEF_BUFF_SIZE] = {0};

    memcpy(inbuf,str.c_str(),sizeof(inbuf));
#ifdef __WINDOWS__
    wchar_t* retStr = NULL;
    retStr = MULTI_2_WIDE(inbuf,CP_UTF8);
    val = retStr;
#else
    wchar_t outbuf[I2DEF_BUFF_SIZE] = {};
    size_t inlen = strlen(inbuf)*2;
    utf8_to_unicode(inbuf, inlen, outbuf );
    val = outbuf;
#endif

    return 0;
}
template<>
inline int CreateJsonElement(wstring &val, const Json::Value &listE)
{
    std::string str = listE.asString();
    char inbuf[I2DEF_BUFF_SIZE] = {0};

    memcpy(inbuf,str.c_str(),sizeof(inbuf));
#ifdef __WINDOWS__
    wchar_t* retStr = NULL;
    retStr = MULTI_2_WIDE(inbuf,CP_UTF8);
    val = retStr;
#else
    wchar_t outbuf[I2DEF_BUFF_SIZE] = {};
    size_t inlen = strlen(inbuf)*2;
    utf8_to_unicode(inbuf, inlen, outbuf );
    val = outbuf;
#endif
    return 0;
}

template<typename T>
inline int CreateJsonListVec(std::vector<T> &element,const std::string &name, const Json::Value &src)
{

    int size = src[name].size();
    if (size == 0)
    {
        return -E_CNVJSON_JSON2ELE_JSONOBJECT_SIZE;
    }

    const Json::Value &listE = src[name];
    element.clear();
    for (int i=0; i<size;i++)
    {
        T val;
        CreateJsonElement(val, listE[i]);
        element.push_back(val);
    }

    return 0;
}
template<typename T>
inline int CreateJsonListVec(std::list<T> &element,const std::string &name, const Json::Value &src)
{

    int size = src[name].size();
    if (size == 0)
    {
        return -E_CNVJSON_JSON2ELE_JSONOBJECT_SIZE;
    }

    const Json::Value &listE = src[name];
    element.clear();
    for (int i=0; i<size;i++)
    {
        T val;
        CreateJsonElement(val, listE[i]);
        element.push_back(val);
    }
    return 0;
}
template<typename T>
inline int CreateJsonListVec( T &element,const std::string &name, const Json::Value &src)
{ return 0; }

template<typename T>
inline int CreateJsonMap(std::map<std::string,T> &element,const std::string &name, const Json::Value &src)
{

    int size = src[name].size();
    if (size == 0)
    {
        return -10108;
    }

    const Json::Value &listE = src[name];

    Json::Value::Members members;
    members = listE.getMemberNames();
    element.clear();
    for(Json::Value::Members::iterator itemr= members.begin(); itemr != members.end(); itemr++) {
        std::string strKey = *itemr;
        std::string strValue; ;
        if (listE[strKey.c_str()].isArray() || listE[strKey.c_str()].isObject()) {
            return -10108;
        } else {
            strValue = listE[strKey.c_str()].isNull() ? "" : listE[strKey.c_str()].asString();
            element[strKey] = strValue;
        }
    }

    return 0;
}
template<typename T>
inline int CreateJsonMap( T &element,const std::string &name, const Json::Value &src)
{ return 0; }


template<typename T>
inline int CreateJsonArray(T element[], const std::string &name, int nArrary, const Json::Value &src)
{
    if(nArrary == 0){
        return -E_CNVJSON_JSON2ELE_ARRAY_SIZE;
    }
    int size = src[name].size();
    const Json::Value &listE = src[name];
    for ( int i=0; i < nArrary && i < size; ++i ){
        CreateJsonElement(element[i],listE[i]);
    }
    return 0;
}
template<typename T>
inline int CreateJsonArray(T &element, const std::string &name, int nArrary,const Json::Value lists)
{ return 0; }



template<typename T>
int ConvJson_Json2Element(T &element, const std::string &name, const Json::Value &src)
{
    int nPointer = 0;
    int nArrary = 0;
    int nStype = 0;
    int nType = Check_ArchiveType(element, nPointer, nArrary, nStype);

    Json::Value::Members members;
    members = src.getMemberNames();
    Json::Value::Members::iterator it = members.begin();
    for (; it!=members.end();++it){
        //std::string str = *it;
        if(name == *it){
            try{
                if (nType == ARCHIVE_TYPE_ARRAY){
                    CreateJsonArray(element, name, nArrary, src);
                }
                else if(nType == ARCHIVE_TYPE_STL){
                    if (nStype == 3){
                        CreateJsonMap(element, name, src);
                    }else{
                        CreateJsonListVec(element, name, src);
                    }
                }
                else{
                    CreateJsonElement(element, src[name]);
                }
            }
            catch(...)
            {
                cout<<"Json object parse failed"<<endl;
            }
        }
    }

    return 0;
}




#define CONVJSON_STRUCT2JSON_BEGIN \
public:\
    int ConvJson_Struct2Json(Json::Value &jObject)\
{\
    int nRet = 0;
//nRet = ConvJson_Element2Json(element,"element",jObject);
//if(nRet!=0)
//   return nRet;
//return nRet;
//};
#define CONVJSON_STRUCT2JSON_END return nRet; };

#define CONVJSON_JSON2STRUCT_BEGIN \
    int ConvJson_JSon2Struct(const Json::Value &jObject)\
{                                  \
    int nRet = 0;

#define CONVJSON_JSON2STRUCT_END return nRet; };

#define CONVJSON_DO_E2J(ELM) { nRet = ConvJson_Element2Json(ELM,#ELM,jObject); if(nRet!=0) return nRet; };
#define CONVJSON_DO_J2E(ELM) { nRet = ConvJson_Json2Element(ELM,#ELM,jObject); if(nRet!=0) return nRet; };

#ifdef __WINDOWS__

#define CONVJSON_FUN_ARGS(x, y) x y

#define CONVJSON_CALL_LOOP_1(ACT,M)        ACT(M)
#define CONVJSON_CALL_LOOP_2(ACT,M,...)    ACT(M) CONVJSON_FUN_ARGS(CONVJSON_CALL_LOOP_1, (ACT, __VA_ARGS__))
#define CONVJSON_CALL_LOOP_3(ACT,M,...)    ACT(M) CONVJSON_FUN_ARGS(CONVJSON_CALL_LOOP_2, (ACT, __VA_ARGS__))
#define CONVJSON_CALL_LOOP_4(ACT,M,...)    ACT(M) CONVJSON_FUN_ARGS(CONVJSON_CALL_LOOP_3, (ACT, __VA_ARGS__))
#define CONVJSON_CALL_LOOP_5(ACT,M,...)    ACT(M) CONVJSON_FUN_ARGS(CONVJSON_CALL_LOOP_4, (ACT, __VA_ARGS__))
#define CONVJSON_CALL_LOOP_6(ACT,M,...)    ACT(M) CONVJSON_FUN_ARGS(CONVJSON_CALL_LOOP_5, (ACT, __VA_ARGS__))
#define CONVJSON_CALL_LOOP_7(ACT,M,...)    ACT(M) CONVJSON_FUN_ARGS(CONVJSON_CALL_LOOP_6, (ACT, __VA_ARGS__))
#define CONVJSON_CALL_LOOP_8(ACT,M,...)    ACT(M) CONVJSON_FUN_ARGS(CONVJSON_CALL_LOOP_7, (ACT, __VA_ARGS__))
#define CONVJSON_CALL_LOOP_9(ACT,M,...)    ACT(M) CONVJSON_FUN_ARGS(CONVJSON_CALL_LOOP_8, (ACT, __VA_ARGS__))
#define CONVJSON_CALL_LOOP_10(ACT,M,...)   ACT(M) CONVJSON_FUN_ARGS(CONVJSON_CALL_LOOP_9, (ACT, __VA_ARGS__))
#define CONVJSON_CALL_LOOP_11(ACT,M,...)   ACT(M) CONVJSON_FUN_ARGS(CONVJSON_CALL_LOOP_10, (ACT, __VA_ARGS__))
#define CONVJSON_CALL_LOOP_12(ACT,M,...)   ACT(M) CONVJSON_FUN_ARGS(CONVJSON_CALL_LOOP_11, (ACT, __VA_ARGS__))
#define CONVJSON_CALL_LOOP_13(ACT,M,...)   ACT(M) CONVJSON_FUN_ARGS(CONVJSON_CALL_LOOP_12, (ACT, __VA_ARGS__))
#define CONVJSON_CALL_LOOP_14(ACT,M,...)   ACT(M) CONVJSON_FUN_ARGS(CONVJSON_CALL_LOOP_13, (ACT, __VA_ARGS__))
#define CONVJSON_CALL_LOOP_15(ACT,M,...)   ACT(M) CONVJSON_FUN_ARGS(CONVJSON_CALL_LOOP_14, (ACT, __VA_ARGS__))
#define CONVJSON_CALL_LOOP_16(ACT,M,...)   ACT(M) CONVJSON_FUN_ARGS(CONVJSON_CALL_LOOP_15, (ACT, __VA_ARGS__))
#define CONVJSON_CALL_LOOP_17(ACT,M,...)   ACT(M) CONVJSON_FUN_ARGS(CONVJSON_CALL_LOOP_16, (ACT, __VA_ARGS__))
#define CONVJSON_CALL_LOOP_18(ACT,M,...)   ACT(M) CONVJSON_FUN_ARGS(CONVJSON_CALL_LOOP_17, (ACT, __VA_ARGS__))
#define CONVJSON_CALL_LOOP_19(ACT,M,...)   ACT(M) CONVJSON_FUN_ARGS(CONVJSON_CALL_LOOP_18, (ACT, __VA_ARGS__))
#define CONVJSON_CALL_LOOP_20(ACT,M,...)   ACT(M) CONVJSON_FUN_ARGS(CONVJSON_CALL_LOOP_19, (ACT, __VA_ARGS__))
#define CONVJSON_CALL_LOOP_21(ACT,M,...)   ACT(M) CONVJSON_FUN_ARGS(CONVJSON_CALL_LOOP_20, (ACT, __VA_ARGS__))
#define CONVJSON_CALL_LOOP_22(ACT,M,...)   ACT(M) CONVJSON_FUN_ARGS(CONVJSON_CALL_LOOP_21, (ACT, __VA_ARGS__))
#define CONVJSON_CALL_LOOP_23(ACT,M,...)   ACT(M) CONVJSON_FUN_ARGS(CONVJSON_CALL_LOOP_22, (ACT, __VA_ARGS__))
#define CONVJSON_CALL_LOOP_24(ACT,M,...)   ACT(M) CONVJSON_FUN_ARGS(CONVJSON_CALL_LOOP_23, (ACT, __VA_ARGS__))
#define CONVJSON_CALL_LOOP_25(ACT,M,...)   ACT(M) CONVJSON_FUN_ARGS(CONVJSON_CALL_LOOP_24, (ACT, __VA_ARGS__))
#define CONVJSON_CALL_LOOP_26(ACT,M,...)   ACT(M) CONVJSON_FUN_ARGS(CONVJSON_CALL_LOOP_25, (ACT, __VA_ARGS__))
#define CONVJSON_CALL_LOOP_27(ACT,M,...)   ACT(M) CONVJSON_FUN_ARGS(CONVJSON_CALL_LOOP_26, (ACT, __VA_ARGS__))
#define CONVJSON_CALL_LOOP_28(ACT,M,...)   ACT(M) CONVJSON_FUN_ARGS(CONVJSON_CALL_LOOP_27, (ACT, __VA_ARGS__))
#define CONVJSON_CALL_LOOP_29(ACT,M,...)   ACT(M) CONVJSON_FUN_ARGS(CONVJSON_CALL_LOOP_28, (ACT, __VA_ARGS__))
#define CONVJSON_CALL_LOOP_30(ACT,M,...)   ACT(M) CONVJSON_FUN_ARGS(CONVJSON_CALL_LOOP_29, (ACT, __VA_ARGS__))
#define CONVJSON_CALL_LOOP_31(ACT,M,...)   ACT(M) CONVJSON_FUN_ARGS(CONVJSON_CALL_LOOP_30, (ACT, __VA_ARGS__))
#define CONVJSON_CALL_LOOP_32(ACT,M,...)   ACT(M) CONVJSON_FUN_ARGS(CONVJSON_CALL_LOOP_31, (ACT, __VA_ARGS__))
#define CONVJSON_CALL_LOOP_33(ACT,M,...)   ACT(M) CONVJSON_FUN_ARGS(CONVJSON_CALL_LOOP_32, (ACT, __VA_ARGS__))
#define CONVJSON_CALL_LOOP_34(ACT,M,...)   ACT(M) CONVJSON_FUN_ARGS(CONVJSON_CALL_LOOP_33, (ACT, __VA_ARGS__))
#define CONVJSON_CALL_LOOP_35(ACT,M,...)   ACT(M) CONVJSON_FUN_ARGS(CONVJSON_CALL_LOOP_34, (ACT, __VA_ARGS__))
#define CONVJSON_CALL_LOOP_36(ACT,M,...)   ACT(M) CONVJSON_FUN_ARGS(CONVJSON_CALL_LOOP_35, (ACT, __VA_ARGS__))
#define CONVJSON_CALL_LOOP_37(ACT,M,...)   ACT(M) CONVJSON_FUN_ARGS(CONVJSON_CALL_LOOP_36, (ACT, __VA_ARGS__))
#define CONVJSON_CALL_LOOP_38(ACT,M,...)   ACT(M) CONVJSON_FUN_ARGS(CONVJSON_CALL_LOOP_37, (ACT, __VA_ARGS__))
#define CONVJSON_CALL_LOOP_39(ACT,M,...)   ACT(M) CONVJSON_FUN_ARGS(CONVJSON_CALL_LOOP_38, (ACT, __VA_ARGS__))
#define CONVJSON_CALL_LOOP_40(ACT,M,...)   ACT(M) CONVJSON_FUN_ARGS(CONVJSON_CALL_LOOP_39, (ACT, __VA_ARGS__))
#define CONVJSON_CALL_LOOP_41(ACT,M,...)   ACT(M) CONVJSON_FUN_ARGS(CONVJSON_CALL_LOOP_40, (ACT, __VA_ARGS__))
#define CONVJSON_CALL_LOOP_42(ACT,M,...)   ACT(M) CONVJSON_FUN_ARGS(CONVJSON_CALL_LOOP_41, (ACT, __VA_ARGS__))
#define CONVJSON_CALL_LOOP_43(ACT,M,...)   ACT(M) CONVJSON_FUN_ARGS(CONVJSON_CALL_LOOP_42, (ACT, __VA_ARGS__))
#define CONVJSON_CALL_LOOP_44(ACT,M,...)   ACT(M) CONVJSON_FUN_ARGS(CONVJSON_CALL_LOOP_43, (ACT, __VA_ARGS__))
#define CONVJSON_CALL_LOOP_45(ACT,M,...)   ACT(M) CONVJSON_FUN_ARGS(CONVJSON_CALL_LOOP_44, (ACT, __VA_ARGS__))
#define CONVJSON_CALL_LOOP_46(ACT,M,...)   ACT(M) CONVJSON_FUN_ARGS(CONVJSON_CALL_LOOP_45, (ACT, __VA_ARGS__))
#define CONVJSON_CALL_LOOP_47(ACT,M,...)   ACT(M) CONVJSON_FUN_ARGS(CONVJSON_CALL_LOOP_46, (ACT, __VA_ARGS__))
#define CONVJSON_CALL_LOOP_48(ACT,M,...)   ACT(M) CONVJSON_FUN_ARGS(CONVJSON_CALL_LOOP_47, (ACT, __VA_ARGS__))
#define CONVJSON_CALL_LOOP_49(ACT,M,...)   ACT(M) CONVJSON_FUN_ARGS(CONVJSON_CALL_LOOP_48, (ACT, __VA_ARGS__))
#define CONVJSON_CALL_LOOP_50(ACT,M,...)   ACT(M) CONVJSON_FUN_ARGS(CONVJSON_CALL_LOOP_49, (ACT, __VA_ARGS__))
#define CONVJSON_CALL_LOOP_51(ACT,M,...)   ACT(M) CONVJSON_FUN_ARGS(CONVJSON_CALL_LOOP_50, (ACT, __VA_ARGS__))
#define CONVJSON_CALL_LOOP_52(ACT,M,...)   ACT(M) CONVJSON_FUN_ARGS(CONVJSON_CALL_LOOP_51, (ACT, __VA_ARGS__))
#define CONVJSON_CALL_LOOP_53(ACT,M,...)   ACT(M) CONVJSON_FUN_ARGS(CONVJSON_CALL_LOOP_52, (ACT, __VA_ARGS__))
#define CONVJSON_CALL_LOOP_54(ACT,M,...)   ACT(M) CONVJSON_FUN_ARGS(CONVJSON_CALL_LOOP_53, (ACT, __VA_ARGS__))
#define CONVJSON_CALL_LOOP_55(ACT,M,...)   ACT(M) CONVJSON_FUN_ARGS(CONVJSON_CALL_LOOP_54, (ACT, __VA_ARGS__))
#define CONVJSON_CALL_LOOP_56(ACT,M,...)   ACT(M) CONVJSON_FUN_ARGS(CONVJSON_CALL_LOOP_55, (ACT, __VA_ARGS__))
#define CONVJSON_CALL_LOOP_57(ACT,M,...)   ACT(M) CONVJSON_FUN_ARGS(CONVJSON_CALL_LOOP_56, (ACT, __VA_ARGS__))
#define CONVJSON_CALL_LOOP_58(ACT,M,...)   ACT(M) CONVJSON_FUN_ARGS(CONVJSON_CALL_LOOP_57, (ACT, __VA_ARGS__))
#define CONVJSON_CALL_LOOP_59(ACT,M,...)   ACT(M) CONVJSON_FUN_ARGS(CONVJSON_CALL_LOOP_58, (ACT, __VA_ARGS__))
#define CONVJSON_CALL_LOOP_60(ACT,M,...)   ACT(M) CONVJSON_FUN_ARGS(CONVJSON_CALL_LOOP_59, (ACT, __VA_ARGS__))
#define CONVJSON_CALL_LOOP_61(ACT,M,...)   ACT(M) CONVJSON_FUN_ARGS(CONVJSON_CALL_LOOP_60, (ACT, __VA_ARGS__))
#define CONVJSON_CALL_LOOP_62(ACT,M,...)   ACT(M) CONVJSON_FUN_ARGS(CONVJSON_CALL_LOOP_61, (ACT, __VA_ARGS__))
#define CONVJSON_CALL_LOOP_63(ACT,M,...)   ACT(M) CONVJSON_FUN_ARGS(CONVJSON_CALL_LOOP_62, (ACT, __VA_ARGS__))
#define CONVJSON_CALL_LOOP_64(ACT,M,...)   ACT(M) CONVJSON_FUN_ARGS(CONVJSON_CALL_LOOP_63, (ACT, __VA_ARGS__))
#define CONVJSON_CALL_LOOP_65(ACT,M,...)   ACT(M) CONVJSON_FUN_ARGS(CONVJSON_CALL_LOOP_64, (ACT, __VA_ARGS__))
#define CONVJSON_CALL_LOOP_66(ACT,M,...)   ACT(M) CONVJSON_FUN_ARGS(CONVJSON_CALL_LOOP_65, (ACT, __VA_ARGS__))
#define CONVJSON_CALL_LOOP_67(ACT,M,...)   ACT(M) CONVJSON_FUN_ARGS(CONVJSON_CALL_LOOP_66, (ACT, __VA_ARGS__))
#define CONVJSON_CALL_LOOP_68(ACT,M,...)   ACT(M) CONVJSON_FUN_ARGS(CONVJSON_CALL_LOOP_67, (ACT, __VA_ARGS__))
#define CONVJSON_CALL_LOOP_69(ACT,M,...)   ACT(M) CONVJSON_FUN_ARGS(CONVJSON_CALL_LOOP_68, (ACT, __VA_ARGS__))
#define CONVJSON_CALL_LOOP_70(ACT,M,...)   ACT(M) CONVJSON_FUN_ARGS(CONVJSON_CALL_LOOP_69, (ACT, __VA_ARGS__))
#define CONVJSON_CALL_LOOP_71(ACT,M,...)   ACT(M) CONVJSON_FUN_ARGS(CONVJSON_CALL_LOOP_70, (ACT, __VA_ARGS__))
#define CONVJSON_CALL_LOOP_72(ACT,M,...)   ACT(M) CONVJSON_FUN_ARGS(CONVJSON_CALL_LOOP_71, (ACT, __VA_ARGS__))
#define CONVJSON_CALL_LOOP_73(ACT,M,...)   ACT(M) CONVJSON_FUN_ARGS(CONVJSON_CALL_LOOP_72, (ACT, __VA_ARGS__))
#define CONVJSON_CALL_LOOP_74(ACT,M,...)   ACT(M) CONVJSON_FUN_ARGS(CONVJSON_CALL_LOOP_73, (ACT, __VA_ARGS__))
#define CONVJSON_CALL_LOOP_75(ACT,M,...)   ACT(M) CONVJSON_FUN_ARGS(CONVJSON_CALL_LOOP_74, (ACT, __VA_ARGS__))
#define CONVJSON_CALL_LOOP_76(ACT,M,...)   ACT(M) CONVJSON_FUN_ARGS(CONVJSON_CALL_LOOP_75, (ACT, __VA_ARGS__))
#define CONVJSON_CALL_LOOP_77(ACT,M,...)   ACT(M) CONVJSON_FUN_ARGS(CONVJSON_CALL_LOOP_76, (ACT, __VA_ARGS__))
#define CONVJSON_CALL_LOOP_78(ACT,M,...)   ACT(M) CONVJSON_FUN_ARGS(CONVJSON_CALL_LOOP_77, (ACT, __VA_ARGS__))
#define CONVJSON_CALL_LOOP_79(ACT,M,...)   ACT(M) CONVJSON_FUN_ARGS(CONVJSON_CALL_LOOP_78, (ACT, __VA_ARGS__))
#define CONVJSON_CALL_LOOP_80(ACT,M,...)   ACT(M) CONVJSON_FUN_ARGS(CONVJSON_CALL_LOOP_79, (ACT, __VA_ARGS__))
#define CONVJSON_CALL_LOOP_81(ACT,M,...)   ACT(M) CONVJSON_FUN_ARGS(CONVJSON_CALL_LOOP_80, (ACT, __VA_ARGS__))
#define CONVJSON_CALL_LOOP_82(ACT,M,...)   ACT(M) CONVJSON_FUN_ARGS(CONVJSON_CALL_LOOP_81, (ACT, __VA_ARGS__))
#define CONVJSON_CALL_LOOP_83(ACT,M,...)   ACT(M) CONVJSON_FUN_ARGS(CONVJSON_CALL_LOOP_82, (ACT, __VA_ARGS__))
#define CONVJSON_CALL_LOOP_84(ACT,M,...)   ACT(M) CONVJSON_FUN_ARGS(CONVJSON_CALL_LOOP_83, (ACT, __VA_ARGS__))
#define CONVJSON_CALL_LOOP_85(ACT,M,...)   ACT(M) CONVJSON_FUN_ARGS(CONVJSON_CALL_LOOP_84, (ACT, __VA_ARGS__))
#define CONVJSON_CALL_LOOP_86(ACT,M,...)   ACT(M) CONVJSON_FUN_ARGS(CONVJSON_CALL_LOOP_85, (ACT, __VA_ARGS__))
#define CONVJSON_CALL_LOOP_87(ACT,M,...)   ACT(M) CONVJSON_FUN_ARGS(CONVJSON_CALL_LOOP_86, (ACT, __VA_ARGS__))
#define CONVJSON_CALL_LOOP_88(ACT,M,...)   ACT(M) CONVJSON_FUN_ARGS(CONVJSON_CALL_LOOP_87, (ACT, __VA_ARGS__))
#define CONVJSON_CALL_LOOP_89(ACT,M,...)   ACT(M) CONVJSON_FUN_ARGS(CONVJSON_CALL_LOOP_88, (ACT, __VA_ARGS__))
#define CONVJSON_CALL_LOOP_90(ACT,M,...)   ACT(M) CONVJSON_FUN_ARGS(CONVJSON_CALL_LOOP_89, (ACT, __VA_ARGS__))
#define CONVJSON_CALL_LOOP_91(ACT,M,...)   ACT(M) CONVJSON_FUN_ARGS(CONVJSON_CALL_LOOP_90, (ACT, __VA_ARGS__))
#define CONVJSON_CALL_LOOP_92(ACT,M,...)   ACT(M) CONVJSON_FUN_ARGS(CONVJSON_CALL_LOOP_91, (ACT, __VA_ARGS__))
#define CONVJSON_CALL_LOOP_93(ACT,M,...)   ACT(M) CONVJSON_FUN_ARGS(CONVJSON_CALL_LOOP_92, (ACT, __VA_ARGS__))
#define CONVJSON_CALL_LOOP_94(ACT,M,...)   ACT(M) CONVJSON_FUN_ARGS(CONVJSON_CALL_LOOP_93, (ACT, __VA_ARGS__))
#define CONVJSON_CALL_LOOP_95(ACT,M,...)   ACT(M) CONVJSON_FUN_ARGS(CONVJSON_CALL_LOOP_94, (ACT, __VA_ARGS__))
#define CONVJSON_CALL_LOOP_96(ACT,M,...)   ACT(M) CONVJSON_FUN_ARGS(CONVJSON_CALL_LOOP_95, (ACT, __VA_ARGS__))
#define CONVJSON_CALL_LOOP_97(ACT,M,...)   ACT(M) CONVJSON_FUN_ARGS(CONVJSON_CALL_LOOP_96, (ACT, __VA_ARGS__))
#define CONVJSON_CALL_LOOP_98(ACT,M,...)   ACT(M) CONVJSON_FUN_ARGS(CONVJSON_CALL_LOOP_97, (ACT, __VA_ARGS__))
#define CONVJSON_CALL_LOOP_99(ACT,M,...)   ACT(M) CONVJSON_FUN_ARGS(CONVJSON_CALL_LOOP_98, (ACT, __VA_ARGS__))

#else

#define CONVJSON_CALL_LOOP_1(ACT,M)        ACT(M)
#define CONVJSON_CALL_LOOP_2(ACT,M,...)    ACT(M) CONVJSON_CALL_LOOP_1(ACT, __VA_ARGS__)
#define CONVJSON_CALL_LOOP_3(ACT,M,...)    ACT(M) CONVJSON_CALL_LOOP_2(ACT, __VA_ARGS__)
#define CONVJSON_CALL_LOOP_4(ACT,M,...)    ACT(M) CONVJSON_CALL_LOOP_3(ACT, __VA_ARGS__)
#define CONVJSON_CALL_LOOP_5(ACT,M,...)    ACT(M) CONVJSON_CALL_LOOP_4(ACT, __VA_ARGS__)
#define CONVJSON_CALL_LOOP_6(ACT,M,...)    ACT(M) CONVJSON_CALL_LOOP_5(ACT, __VA_ARGS__)
#define CONVJSON_CALL_LOOP_7(ACT,M,...)    ACT(M) CONVJSON_CALL_LOOP_6(ACT, __VA_ARGS__)
#define CONVJSON_CALL_LOOP_8(ACT,M,...)    ACT(M) CONVJSON_CALL_LOOP_7(ACT, __VA_ARGS__)
#define CONVJSON_CALL_LOOP_9(ACT,M,...)    ACT(M) CONVJSON_CALL_LOOP_8(ACT, __VA_ARGS__)
#define CONVJSON_CALL_LOOP_10(ACT,M,...)   ACT(M) CONVJSON_CALL_LOOP_9(ACT, __VA_ARGS__)
#define CONVJSON_CALL_LOOP_11(ACT,M,...)   ACT(M) CONVJSON_CALL_LOOP_10(ACT, __VA_ARGS__)
#define CONVJSON_CALL_LOOP_12(ACT,M,...)   ACT(M) CONVJSON_CALL_LOOP_11(ACT, __VA_ARGS__)
#define CONVJSON_CALL_LOOP_13(ACT,M,...)   ACT(M) CONVJSON_CALL_LOOP_12(ACT, __VA_ARGS__)
#define CONVJSON_CALL_LOOP_14(ACT,M,...)   ACT(M) CONVJSON_CALL_LOOP_13(ACT, __VA_ARGS__)
#define CONVJSON_CALL_LOOP_15(ACT,M,...)   ACT(M) CONVJSON_CALL_LOOP_14(ACT, __VA_ARGS__)
#define CONVJSON_CALL_LOOP_16(ACT,M,...)   ACT(M) CONVJSON_CALL_LOOP_15(ACT, __VA_ARGS__)
#define CONVJSON_CALL_LOOP_17(ACT,M,...)   ACT(M) CONVJSON_CALL_LOOP_16(ACT, __VA_ARGS__)
#define CONVJSON_CALL_LOOP_18(ACT,M,...)   ACT(M) CONVJSON_CALL_LOOP_17(ACT, __VA_ARGS__)
#define CONVJSON_CALL_LOOP_19(ACT,M,...)   ACT(M) CONVJSON_CALL_LOOP_18(ACT, __VA_ARGS__)
#define CONVJSON_CALL_LOOP_20(ACT,M,...)   ACT(M) CONVJSON_CALL_LOOP_19(ACT, __VA_ARGS__)
#define CONVJSON_CALL_LOOP_21(ACT,M,...)   ACT(M) CONVJSON_CALL_LOOP_20(ACT, __VA_ARGS__)
#define CONVJSON_CALL_LOOP_22(ACT,M,...)   ACT(M) CONVJSON_CALL_LOOP_21(ACT, __VA_ARGS__)
#define CONVJSON_CALL_LOOP_23(ACT,M,...)   ACT(M) CONVJSON_CALL_LOOP_22(ACT, __VA_ARGS__)
#define CONVJSON_CALL_LOOP_24(ACT,M,...)   ACT(M) CONVJSON_CALL_LOOP_23(ACT, __VA_ARGS__)
#define CONVJSON_CALL_LOOP_25(ACT,M,...)   ACT(M) CONVJSON_CALL_LOOP_24(ACT, __VA_ARGS__)
#define CONVJSON_CALL_LOOP_26(ACT,M,...)   ACT(M) CONVJSON_CALL_LOOP_25(ACT, __VA_ARGS__)
#define CONVJSON_CALL_LOOP_27(ACT,M,...)   ACT(M) CONVJSON_CALL_LOOP_26(ACT, __VA_ARGS__)
#define CONVJSON_CALL_LOOP_28(ACT,M,...)   ACT(M) CONVJSON_CALL_LOOP_27(ACT, __VA_ARGS__)
#define CONVJSON_CALL_LOOP_29(ACT,M,...)   ACT(M) CONVJSON_CALL_LOOP_28(ACT, __VA_ARGS__)
#define CONVJSON_CALL_LOOP_30(ACT,M,...)   ACT(M) CONVJSON_CALL_LOOP_29(ACT, __VA_ARGS__)
#define CONVJSON_CALL_LOOP_31(ACT,M,...)   ACT(M) CONVJSON_CALL_LOOP_30(ACT, __VA_ARGS__)
#define CONVJSON_CALL_LOOP_32(ACT,M,...)   ACT(M) CONVJSON_CALL_LOOP_31(ACT, __VA_ARGS__)
#define CONVJSON_CALL_LOOP_33(ACT,M,...)   ACT(M) CONVJSON_CALL_LOOP_32(ACT, __VA_ARGS__)
#define CONVJSON_CALL_LOOP_34(ACT,M,...)   ACT(M) CONVJSON_CALL_LOOP_33(ACT, __VA_ARGS__)
#define CONVJSON_CALL_LOOP_35(ACT,M,...)   ACT(M) CONVJSON_CALL_LOOP_34(ACT, __VA_ARGS__)
#define CONVJSON_CALL_LOOP_36(ACT,M,...)   ACT(M) CONVJSON_CALL_LOOP_35(ACT, __VA_ARGS__)
#define CONVJSON_CALL_LOOP_37(ACT,M,...)   ACT(M) CONVJSON_CALL_LOOP_36(ACT, __VA_ARGS__)
#define CONVJSON_CALL_LOOP_38(ACT,M,...)   ACT(M) CONVJSON_CALL_LOOP_37(ACT, __VA_ARGS__)
#define CONVJSON_CALL_LOOP_39(ACT,M,...)   ACT(M) CONVJSON_CALL_LOOP_38(ACT, __VA_ARGS__)
#define CONVJSON_CALL_LOOP_40(ACT,M,...)   ACT(M) CONVJSON_CALL_LOOP_39(ACT, __VA_ARGS__)
#define CONVJSON_CALL_LOOP_41(ACT,M,...)   ACT(M) CONVJSON_CALL_LOOP_40(ACT, __VA_ARGS__)
#define CONVJSON_CALL_LOOP_42(ACT,M,...)   ACT(M) CONVJSON_CALL_LOOP_41(ACT, __VA_ARGS__)
#define CONVJSON_CALL_LOOP_43(ACT,M,...)   ACT(M) CONVJSON_CALL_LOOP_42(ACT, __VA_ARGS__)
#define CONVJSON_CALL_LOOP_44(ACT,M,...)   ACT(M) CONVJSON_CALL_LOOP_43(ACT, __VA_ARGS__)
#define CONVJSON_CALL_LOOP_45(ACT,M,...)   ACT(M) CONVJSON_CALL_LOOP_44(ACT, __VA_ARGS__)
#define CONVJSON_CALL_LOOP_46(ACT,M,...)   ACT(M) CONVJSON_CALL_LOOP_45(ACT, __VA_ARGS__)
#define CONVJSON_CALL_LOOP_47(ACT,M,...)   ACT(M) CONVJSON_CALL_LOOP_46(ACT, __VA_ARGS__)
#define CONVJSON_CALL_LOOP_48(ACT,M,...)   ACT(M) CONVJSON_CALL_LOOP_47(ACT, __VA_ARGS__)
#define CONVJSON_CALL_LOOP_49(ACT,M,...)   ACT(M) CONVJSON_CALL_LOOP_48(ACT, __VA_ARGS__)
#define CONVJSON_CALL_LOOP_50(ACT,M,...)   ACT(M) CONVJSON_CALL_LOOP_49(ACT, __VA_ARGS__)
#define CONVJSON_CALL_LOOP_51(ACT,M,...)   ACT(M) CONVJSON_CALL_LOOP_50(ACT, __VA_ARGS__)
#define CONVJSON_CALL_LOOP_52(ACT,M,...)   ACT(M) CONVJSON_CALL_LOOP_51(ACT, __VA_ARGS__)
#define CONVJSON_CALL_LOOP_53(ACT,M,...)   ACT(M) CONVJSON_CALL_LOOP_52(ACT, __VA_ARGS__)
#define CONVJSON_CALL_LOOP_54(ACT,M,...)   ACT(M) CONVJSON_CALL_LOOP_53(ACT, __VA_ARGS__)
#define CONVJSON_CALL_LOOP_55(ACT,M,...)   ACT(M) CONVJSON_CALL_LOOP_54(ACT, __VA_ARGS__)
#define CONVJSON_CALL_LOOP_56(ACT,M,...)   ACT(M) CONVJSON_CALL_LOOP_55(ACT, __VA_ARGS__)
#define CONVJSON_CALL_LOOP_57(ACT,M,...)   ACT(M) CONVJSON_CALL_LOOP_56(ACT, __VA_ARGS__)
#define CONVJSON_CALL_LOOP_58(ACT,M,...)   ACT(M) CONVJSON_CALL_LOOP_57(ACT, __VA_ARGS__)
#define CONVJSON_CALL_LOOP_59(ACT,M,...)   ACT(M) CONVJSON_CALL_LOOP_58(ACT, __VA_ARGS__)
#define CONVJSON_CALL_LOOP_60(ACT,M,...)   ACT(M) CONVJSON_CALL_LOOP_59(ACT, __VA_ARGS__)
#define CONVJSON_CALL_LOOP_61(ACT,M,...)   ACT(M) CONVJSON_CALL_LOOP_60(ACT, __VA_ARGS__)
#define CONVJSON_CALL_LOOP_62(ACT,M,...)   ACT(M) CONVJSON_CALL_LOOP_61(ACT, __VA_ARGS__)
#define CONVJSON_CALL_LOOP_63(ACT,M,...)   ACT(M) CONVJSON_CALL_LOOP_62(ACT, __VA_ARGS__)
#define CONVJSON_CALL_LOOP_64(ACT,M,...)   ACT(M) CONVJSON_CALL_LOOP_63(ACT, __VA_ARGS__)
#define CONVJSON_CALL_LOOP_65(ACT,M,...)   ACT(M) CONVJSON_CALL_LOOP_64(ACT, __VA_ARGS__)
#define CONVJSON_CALL_LOOP_66(ACT,M,...)   ACT(M) CONVJSON_CALL_LOOP_65(ACT, __VA_ARGS__)
#define CONVJSON_CALL_LOOP_67(ACT,M,...)   ACT(M) CONVJSON_CALL_LOOP_66(ACT, __VA_ARGS__)
#define CONVJSON_CALL_LOOP_68(ACT,M,...)   ACT(M) CONVJSON_CALL_LOOP_67(ACT, __VA_ARGS__)
#define CONVJSON_CALL_LOOP_69(ACT,M,...)   ACT(M) CONVJSON_CALL_LOOP_68(ACT, __VA_ARGS__)
#define CONVJSON_CALL_LOOP_70(ACT,M,...)   ACT(M) CONVJSON_CALL_LOOP_69(ACT, __VA_ARGS__)
#define CONVJSON_CALL_LOOP_71(ACT,M,...)   ACT(M) CONVJSON_CALL_LOOP_70(ACT, __VA_ARGS__)
#define CONVJSON_CALL_LOOP_72(ACT,M,...)   ACT(M) CONVJSON_CALL_LOOP_71(ACT, __VA_ARGS__)
#define CONVJSON_CALL_LOOP_73(ACT,M,...)   ACT(M) CONVJSON_CALL_LOOP_72(ACT, __VA_ARGS__)
#define CONVJSON_CALL_LOOP_74(ACT,M,...)   ACT(M) CONVJSON_CALL_LOOP_73(ACT, __VA_ARGS__)
#define CONVJSON_CALL_LOOP_75(ACT,M,...)   ACT(M) CONVJSON_CALL_LOOP_74(ACT, __VA_ARGS__)
#define CONVJSON_CALL_LOOP_76(ACT,M,...)   ACT(M) CONVJSON_CALL_LOOP_75(ACT, __VA_ARGS__)
#define CONVJSON_CALL_LOOP_77(ACT,M,...)   ACT(M) CONVJSON_CALL_LOOP_76(ACT, __VA_ARGS__)
#define CONVJSON_CALL_LOOP_78(ACT,M,...)   ACT(M) CONVJSON_CALL_LOOP_77(ACT, __VA_ARGS__)
#define CONVJSON_CALL_LOOP_79(ACT,M,...)   ACT(M) CONVJSON_CALL_LOOP_78(ACT, __VA_ARGS__)
#define CONVJSON_CALL_LOOP_80(ACT,M,...)   ACT(M) CONVJSON_CALL_LOOP_79(ACT, __VA_ARGS__)
#define CONVJSON_CALL_LOOP_81(ACT,M,...)   ACT(M) CONVJSON_CALL_LOOP_80(ACT, __VA_ARGS__)
#define CONVJSON_CALL_LOOP_82(ACT,M,...)   ACT(M) CONVJSON_CALL_LOOP_81(ACT, __VA_ARGS__)
#define CONVJSON_CALL_LOOP_83(ACT,M,...)   ACT(M) CONVJSON_CALL_LOOP_82(ACT, __VA_ARGS__)
#define CONVJSON_CALL_LOOP_84(ACT,M,...)   ACT(M) CONVJSON_CALL_LOOP_83(ACT, __VA_ARGS__)
#define CONVJSON_CALL_LOOP_85(ACT,M,...)   ACT(M) CONVJSON_CALL_LOOP_84(ACT, __VA_ARGS__)
#define CONVJSON_CALL_LOOP_86(ACT,M,...)   ACT(M) CONVJSON_CALL_LOOP_85(ACT, __VA_ARGS__)
#define CONVJSON_CALL_LOOP_87(ACT,M,...)   ACT(M) CONVJSON_CALL_LOOP_86(ACT, __VA_ARGS__)
#define CONVJSON_CALL_LOOP_88(ACT,M,...)   ACT(M) CONVJSON_CALL_LOOP_87(ACT, __VA_ARGS__)
#define CONVJSON_CALL_LOOP_89(ACT,M,...)   ACT(M) CONVJSON_CALL_LOOP_88(ACT, __VA_ARGS__)
#define CONVJSON_CALL_LOOP_90(ACT,M,...)   ACT(M) CONVJSON_CALL_LOOP_89(ACT, __VA_ARGS__)
#define CONVJSON_CALL_LOOP_91(ACT,M,...)   ACT(M) CONVJSON_CALL_LOOP_90(ACT, __VA_ARGS__)
#define CONVJSON_CALL_LOOP_92(ACT,M,...)   ACT(M) CONVJSON_CALL_LOOP_91(ACT, __VA_ARGS__)
#define CONVJSON_CALL_LOOP_93(ACT,M,...)   ACT(M) CONVJSON_CALL_LOOP_92(ACT, __VA_ARGS__)
#define CONVJSON_CALL_LOOP_94(ACT,M,...)   ACT(M) CONVJSON_CALL_LOOP_93(ACT, __VA_ARGS__)
#define CONVJSON_CALL_LOOP_95(ACT,M,...)   ACT(M) CONVJSON_CALL_LOOP_94(ACT, __VA_ARGS__)
#define CONVJSON_CALL_LOOP_96(ACT,M,...)   ACT(M) CONVJSON_CALL_LOOP_95(ACT, __VA_ARGS__)
#define CONVJSON_CALL_LOOP_97(ACT,M,...)   ACT(M) CONVJSON_CALL_LOOP_96(ACT, __VA_ARGS__)
#define CONVJSON_CALL_LOOP_98(ACT,M,...)   ACT(M) CONVJSON_CALL_LOOP_97(ACT, __VA_ARGS__)
#define CONVJSON_CALL_LOOP_99(ACT,M,...)   ACT(M) CONVJSON_CALL_LOOP_98(ACT, __VA_ARGS__)

#endif

#define CONVJSON_VAR_LIST                  99,98,97,\
    96,95,94,93,92,91,90,89,88,87,86,85,84,83,82,81,\
    80,79,78,77,76,75,74,73,72,71,70,69,68,67,66,65,\
    64,63,62,61,60,59,58,57,56,55,54,53,52,51,50,49,\
    48,47,46,45,44,43,42,41,40,39,38,37,36,35,34,33,\
    32,31,30,29,28,27,26,25,24,23,22,21,20,19,18,17,\
    16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1

#define CONVJSON_VAR_COUNT_XTEMP_N(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,\
    _17,_18,_19,_20,_21,_22,_23,_24,_25,_26,_27,_28,_29,_30,_31,_32,\
    _33,_34,_35,_36,_37,_38,_39,_40,_41,_42,_43,_44,_45,_46,_47,_48,\
    _49,_50,_51,_52,_53,_54,_55,_56,_57,_58,_59,_60,_61,_62,_63,_64,\
    _65,_66,_67,_68,_69,_70,_71,_72,_73,_74,_75,_76,_77,_78,_79,_80,\
    _81,_82,_83,_84,_85,_86,_87,_88,_89,_90,_91,_92,_93,_94,_95,_96,\
    _97,_98,_99,N,...) N
#define CONVJSON_VAR_COUNT_XTEMP(arg) CONVJSON_VAR_COUNT_XTEMP_N arg
#define CONVJSON_VAR_COUNT(...) CONVJSON_VAR_COUNT_XTEMP((__VA_ARGS__,CONVJSON_VAR_LIST))

#define CONVJSON_CHOOSE_HELPER3(M,count)  M##count
#define CONVJSON_CHOOSE_HELPER2(M,count) CONVJSON_CHOOSE_HELPER3(M,count)
#define CONVJSON_CHOOSE_HELPER1(M,count) CONVJSON_CHOOSE_HELPER2(M,count)
#define CONVJSON_CHOOSE_HELPER(M,count)   CONVJSON_CHOOSE_HELPER1(M,count)

#ifdef __WINDOWS__

#define CONVJSON_ACTCALL_EXT(ACT,...) CONVJSON_FUN_ARGS(CONVJSON_CHOOSE_HELPER(CONVJSON_CALL_LOOP_,CONVJSON_VAR_COUNT(__VA_ARGS__)), ( ACT,__VA_ARGS__ ) )

#else

#define CONVJSON_ACTCALL_EXT(ACT,...) CONVJSON_CHOOSE_HELPER(CONVJSON_CALL_LOOP_,CONVJSON_VAR_COUNT(__VA_ARGS__))( ACT,__VA_ARGS__ )

#endif



#define CONVJSON_INIT(...) \
    CONVJSON_STRUCT2JSON_BEGIN \
    CONVJSON_ACTCALL_EXT(CONVJSON_DO_E2J,__VA_ARGS__) \
    CONVJSON_STRUCT2JSON_END \
    CONVJSON_JSON2STRUCT_BEGIN \
    CONVJSON_ACTCALL_EXT(CONVJSON_DO_J2E,__VA_ARGS__) \
    CONVJSON_JSON2STRUCT_END \


#endif //UNTITLED13_CONVERTJSON_H
