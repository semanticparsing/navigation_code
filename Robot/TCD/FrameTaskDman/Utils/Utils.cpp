#include "Utils.h"
#include <iostream>
#include "Common/Base/time.h"

namespace TrioTDM {

// D: implements the printf functionality in a string
std::string FormatString(const char *lpszFormat, ...) {
	va_list pArgs;
	va_start(pArgs, lpszFormat);
    char szBuffer[STRING_MAX];
	// print into the buffer
	vsnprintf(szBuffer, STRING_MAX, lpszFormat, pArgs);
	
	// return a string object initialized from that buffer
	return((string)szBuffer);
}

// D: implements the + operator for strings
string operator + (const string sString1, const string sString2) {
	string sResult = sString1;
	sResult += sString2;
	return sResult;
}

// D: conversion from bool to string
string BoolToString(bool bBool) {
    return bBool?"true":"false";
}

// A: conversion from int to string
string IntToString(int iInt) {
    return FormatString("%d", iInt);
}

// D: conversion from float to string
string FloatToString(float fFloat) {
    return FormatString("%f", fFloat);
}

// A: tests if a string contains a number
bool IsANumber(string sStr) {
	size_t i = 0;

	// empty string -> not a number
	if (sStr.size() == 0)
		return false;

	// skip a potential +/- sign
	if ((sStr[0] == '-')||(sStr[0] == '+'))
		i++;

	// string just contains + or - -> not a number
	if (i == sStr.size())
		return false;

	// skip all digits
	while ((i < sStr.size()) && isdigit(sStr[i]))
		i++;

	// traversed the whole string -> number
	if (i == sStr.size())
		return true;
	// reached a decimal point
	else if (sStr[i] == '.') {
		i++;
		// skip digits after the decimal point
		while ((i < sStr.size()) && isdigit(sStr[i]))
			i++;
		// traversed the whole string -> number
		if (i == sStr.size())
			return true;
		// found something else than a digit -> not a number
		else 
			return false;
	} else
		// found something else than a digit or a decimal point -> not a number
		return false;
}

// A: trim specified characters (default spaces) from the string on the left
string TrimLeft(string sString, const char pToTrim[]) {
	unsigned int i = 0;
	while(i < sString.length()) 
	{
		const char * c = pToTrim;
		while ( *c != '\0' && *c != sString[i] )
			c++;
		if ( *c == '\0' )
			break;
		i++;
	}
	string sResult = sString.substr(i, sString.length() - i);
	return sResult;
}

// A: trim specified characters (default spaces) from the string on the right
string TrimRight(string sString, const char pToTrim[]) {
	int i = sString.length()-1;
	while((i >= 0) )
	{
		const char * c = pToTrim;
		while ( *c != '\0' && *c != sString[i] )
			c++;
		if ( *c == '\0' )
			break;
		i--;
	}
	string sResult = sString.substr(0, i+1);
	return sResult;
}

// A: trim specified characters (default space) from the string at both ends
string Trim(string sString, const char pToTrim[]) {
	return TrimLeft(TrimRight(sString, pToTrim), pToTrim);
}

std::string ExtractFirstLine(std::string& strg) {
    std::string result;
    SplitOnFirst(strg, "\n", result, strg);
    return result;
}

// A: splits the string in 2 parts, around and not including the first 
//    occurence of any of a set of specified characters. Returns true on success
bool SplitOnFirst(string sOriginal, const char* pDividers, 
				  string& rsFirstPart, string& rsSecondPart) {

    int iCharPos = sOriginal.find_first_of(pDividers);
    if(iCharPos == -1) {
		// if the character was not found
		rsFirstPart = sOriginal;
		rsSecondPart = "";
		return false;
	} else {
		// if the character was found
		rsFirstPart = sOriginal.substr(0, iCharPos);
		rsSecondPart = sOriginal.substr(iCharPos + 1, 
										sOriginal.length() - iCharPos);
		return true;			
	}
}

// D: function similar to SplitOnFirst. It takes as an extra argument a char
// that act as quote characters, and therefore any occurence of the dividers 
// within that is not considered
bool SplitOnFirst(string sOriginal, const char* pDividers, 
				  string& rsFirstPart, string& rsSecondPart, char cQuote) {

    int i = 0;
    bool bWithinQuotes = false;
    int l = sOriginal.length();
    while(i < l) {
        // if we/re within quotes, just skip over everything until 
        // a new quote character is met
        if(bWithinQuotes) {
            while((sOriginal[i] != cQuote) && (i < l)) 
                i++;
            // check that we didn't reach the end
            if(i == l) {
                rsFirstPart = sOriginal;
                rsSecondPart = "";
                return false;
            }
            // o/w increment i;
            i++;
            // and set ourselves out of quotes
            bWithinQuotes = false;
        } else if(sOriginal[i] == cQuote) {
            // o/w if we just saw a quote, put ourselves in quote mode
            bWithinQuotes = true;
            // and move on
            i++;
        } else if(strchr(pDividers, sOriginal[i]) != NULL) {
            // o/w if we hit on one of the dividers
            rsFirstPart = sOriginal.substr(0, i);
            rsSecondPart = sOriginal.substr(i + 1, sOriginal.length() - i);
            return true;
        } else {
            i++;
        }
    }

    // if we got out of the loop, it means we reached the end without returning, 
    // so then there are no dividers
    rsFirstPart = sOriginal;
    rsSecondPart = "";
    return false;
}

// A: splits the string in 2 parts, around and not including the last
//    occurence of any of a set of specified characters. Returns true on success
bool SplitOnLast(string sOriginal, const char* pDividers, 
				  string& rsFirstPart, string& rsSecondPart) {
    int iCharPos = sOriginal.find_last_of(pDividers);
	if(iCharPos == -1) {
		// if the character was not found
		rsFirstPart = "";
		rsSecondPart = sOriginal;
		return false;
	} else {
		// if the character was found
		rsFirstPart = sOriginal.substr(0, iCharPos);
		rsSecondPart = sOriginal.substr(iCharPos + 1, 
										sOriginal.length() - iCharPos);
		return true;			
	}
}

// A: partitions a string into tokens divided by any of a set of specified
//    characters.
vector<string> PartitionString(const string& sString, const char * pDividers) {
	vector<string> saResult;
	string sTemp;
    std::string strg(sString);

    while (strg != "" ) {
		SplitOnFirst(strg, pDividers, sTemp, strg);
		if ( sTemp != "" )
			saResult.push_back(sTemp);
	}
	return saResult;
} 

// A: partitions a string into tokens divided by any of a set of specified
//    characters.
vector<string> PartitionString(string sString, char * pDividers, char cQuote) {
	vector<string> saResult;
	string sTemp;
	while (sString != "" )
	{
		SplitOnFirst(sString, pDividers, sTemp, sString, cQuote);
		if ( sTemp != "" )
			saResult.push_back(sTemp);
	}
	return saResult;
} 

// D: replaces all the occurences of a substring in a string with another
//    substring
string ReplaceSubString(const std::string& sSource, string sToReplace, string sReplacement) {
    string sResult = sSource;
    int pos = 0;
    while((pos = sResult.find(sToReplace, pos)) >= 0) {
        sResult.replace(pos, sToReplace.length(), sReplacement);
		pos += sReplacement.length();
    }
    return sResult;
}

// D: find the corresponding closing quote
unsigned int FindClosingQuoteChar(string sString, unsigned int iStartPos, 
								  char cOpenQuote, char cCloseQuote) {

	unsigned int iOpenBraces = 1;
	unsigned int iPos = iStartPos;
	while((iOpenBraces > 0) && (iPos < sString.length())) {
		if(sString[iPos] == cOpenQuote) 
			iOpenBraces++;
		else if(sString[iPos] == cCloseQuote)
			iOpenBraces--;
		iPos++;
	}

	// finally return the position
	return iPos;
}

string S2SHashToString(std::map<std::string, std::string> s2sHash, string sSeparator, 
                       string sEquals) {
    // store the string
    string sResult = "";
	if (!s2sHash.empty()) {
		// go through the mapping and find something that matches the focus
		for(std::map<std::string, std::string>::iterator iPtr = s2sHash.begin(); iPtr != s2sHash.end(); iPtr++) {
			if (iPtr != s2sHash.begin()) sResult += sSeparator;
			sResult += iPtr->first + sEquals + iPtr->second;
		}
	}
    // finally, return the string
    return sResult;
}

std::map<std::string, std::string> StringToS2SHash(string sString, string sSeparator, 
                              string sEquals) {
    // extract the pairs
    std::vector<std::string> vsPairs = 
        PartitionString(sString, (char *)sSeparator.c_str());
    // form the hash
    std::map<std::string, std::string> s2s;
    for(unsigned int i = 0; i < vsPairs.size(); i++) {
        string sAttr, sValue;
        SplitOnFirst(vsPairs[i], (char *)sEquals.c_str(), sAttr, sValue);        
        s2s.insert(std::map<std::string, std::string>::value_type(Trim(sAttr), Trim(sValue)));
    }
    // finally, return the constructed hash
    return s2s;
}

void AppendToS2S(std::map<std::string, std::string>& rs2sInto, const std::map<std::string, std::string>& rs2sFrom) {
    for(std::map<std::string, std::string>::const_iterator iPtr = rs2sFrom.begin(); iPtr != rs2sFrom.end(); iPtr++) { 
        rs2sInto.insert(std::map<std::string, std::string>::value_type(iPtr->first, iPtr->second));
    }
}


// D: creates a random integer ID
int GetRandomIntID() {
    return rand();
}

long long GetTime() {
    return common::GetTimeInSecond();
}



}
