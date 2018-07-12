#ifndef _UTILS_H__
#define _UTILS_H__

#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include <sstream>
#include <string>
#include <vector>
#include <set>
#include <map>

using namespace std;

#define STRING_MAX 65535

namespace TrioTDM {

// D: implements the printf functionality in a string
/**
 * @brief 实现 printf 的功能
 */
string FormatString(const char *lpszFormat, ...);

// D: implements the + operator for strings
/**
 * @brief 字符串连接
 */
string operator + (const string sString1, const string sString2);

// D: conversion from bool to string
/**
 * @brief bool 值到 字符串的转换
 */
string BoolToString(bool bBool);

// A: conversion from int to string
/**
 * @brief 整形到字符串的转换
 */
string IntToString(int iInt);

// D: conversion from float to string
/**
 * @brief float 到 字符串的转换
 */
string FloatToString(float fFloat);

// A: tests if a string contains a number
/**
 * @brief 判断字符串是否是数字
 */
bool IsANumber(string sStr);

// D: trim specified characters (default spaces) from the string on the left
/**
 * @brief 去除字符串左边的特殊字符
 */
string TrimLeft(string sString, const char pToTrim[] = " \n\t\r");

// D: trim specified characters (default spaces) from the string on the right
/**
 * @brief 去除字符串右边的特殊字符
 */
string TrimRight(string sString, const char pToTrim[] = " \n\t\r");

// D: trim specified characters (default space) from the string at both ends
/**
 * @brief 去除字符串两端的特殊字符
 */
string Trim(string sString, const char pToTrim[] = " \n\t\r");
/**
 * @brief 字符串的第一行，以 \n  分割
 */
string ExtractFirstLine(string& rString);

// D: splits the string in 2 parts, around and not including the first 
// occurence of any of a set of specified characters. Returns true on success
bool SplitOnFirst(string sOriginal, const char* pDividers, 
				  string& rsFirstPart, string& rsSecondPart);

// D: splits the string in 2 parts, around and not including the first 
// occurence of any of a set of specified characters, also taking into account
// a quote character. Returns true on success
bool SplitOnFirst(string sOriginal, const char* pDividers, 
				  string& rsFirstPart, string& rsSecondPart, char cQuote);

// D: splits the string in 2 parts, around and not including the last
// occurence of any of a set of specified characters. Returns true on success
bool SplitOnLast(string sOriginal, const char* pDividers, 
				 string& rsFirstPart, string& rsSecondPart);

// D: partitions a string into tokens divided by any of a set of specified
//    characters.
/**
 * @brief 字符串分割
 */
vector<string> PartitionString(const string& sString, const char * pDividers);

// D: partitions a string into tokens divided by any of a set of specified
//    characters. This takes into account a quote character.
vector<string> PartitionString(string sString, char * pDividers, char cQuote);

// D: replaces a substring ocurring in a string with another string
string ReplaceSubString(const std::string& sSource, string sToReplace, string sReplacement=string());

// D: find the corresponding closing quote
unsigned int FindClosingQuoteChar(string sString, unsigned int iStartPos, 
								  char cOpenQuote, char cCloseQuote);
								  
// D: convert a std::map<std::string, std::string> hash of attribute values into a string 
//    representation								  
string S2SHashToString(std::map<std::string, std::string> s2sHash, 
                       string sSeparator = ", ", string sEquals = " = ");

// D: convert a string list of attribute values into a std::map<std::string, std::string> 
//    representation
std::map<std::string, std::string> StringToS2SHash(string sString, string sSeparator = ",", 
                              string sEquals = "=");
                              
// D: add to a S2S hash from a string description
void AppendToS2S(std::map<std::string, std::string>& rs2sInto, const std::map<std::string, std::string>& rs2sFrom);

// D: creates a random integer ID
int GetRandomIntID();

long long GetTime();

template<typename T>
std::string VectorToString(const std::vector<T>& vet, const std::string& seq) {
    std::ostringstream ostr;
    for ( size_t i = 0;i < vet.size(); ++i ) {
        if ( i > 0 ) {
            ostr << seq;
        }
        ostr << vet[i];
    }
    return ostr.str();
}

template<typename K, typename V>
std::string MapToString(const std::map<K, V>& mp, const std::string& seq) {
    std::ostringstream ostr;
    //template<typename K, typename V>
    for (typename std::map<K, V>::const_iterator c_iter = mp.begin();c_iter != mp.end(); ++c_iter ) {
        if ( c_iter != mp.begin() ) {
            ostr << seq;
        }
        ostr << c_iter->first << ":" << c_iter->second;
    }
    return ostr.str();
}

}

#endif // __UTILS_H__
