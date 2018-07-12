/*******************************************************************************
 * Copyright (C) 2016 by Zhuoran Wang, Tricorn (Beijing) Technology Co,. Ltd.  *
 *                                                                             *
 * This file is part of Trio AI dialogue manager.                              *
 *                                                                             *
 *******************************************************************************/

/**
 *  @author Zhuoran Wang
 *  @date   23 May 2016
 */

#ifndef __TrioDM__Common__
#define __TrioDM__Common__

#include "Log.h"
#include "Common/Base/stringUtil.h"
#include "Common/Base/time.h"
#include "Utils.h"
#include "Config.h"

typedef int INT;
typedef double REAL;


#define EPSILON ((REAL)1.0E-9)
#define BELIEF_THRES ((REAL)1.0E-6)
#define BELIEF_TRUST ((REAL)0.5)

#define ST_TIME int64 st_time = common::GetTimeInMs();
#define CAL_TIME VLOG(DATA) << __FUNCTION__ << ", HS: " << common::GetTimeInMs() - st_time;

typedef long long TLLong;
typedef unsigned int UInt;

// Json 处理
#define JSON_ADD_ITEM(j_data, item) \
    do { \
        /* j_data[#item] = Json::Value(item);\*/\
        j_data[#item] = item;\
    } while(0)\

#define JSON_ITEM_TO_INT(j_data, item) \
    do {\
        if ( !j_data[#item].isNull() && j_data[#item].isInt() ) {\
            item = j_data[#item].asInt();\
        } else {\
            VLOG(WARNING) << __FUNCTION__ << ", Fail Int Item: " << #item;\
            return false;\
        }\
    } while(0)\

#define JSON_ITEM_TO_DOUBLE(j_data, item) \
    do {\
        if ( !j_data[#item].isNull() && j_data[#item].isDouble() ) {\
            item = j_data[#item].asDouble();\
        } else {\
            VLOG(WARNING) << __FUNCTION__ << ", Fail Double Item: " << #item;\
            return false;\
        }\
    } while(0)\


#define JSON_ITEM_TO_UINT(j_data, item) \
    do {\
        if ( !j_data[#item].isNull() && ( j_data[#item].isUInt() || j_data[#item].isInt() ) ) {\
            item = j_data[#item].asUInt();\
        } else {\
            VLOG(WARNING) << __FUNCTION__ << ", Fail UInt Item: " << #item;\
            return false;\
        }\
    } while(0)\

#define JSON_ITEM_TO_ENUM(j_data, item, Enum) \
    do {\
        if ( !j_data[#item].isNull() && j_data[#item].isInt() ) {\
            item = (Enum)j_data[#item].asInt();\
        } else {\
            VLOG(WARNING) << __FUNCTION__ << ", Fail Enum Item: " << #item;\
            return false;\
        }\
    } while(0)\

#define JSON_ITEM_TO_STRING(j_data, item) \
    do {\
        if ( !j_data[#item].isNull() && j_data[#item].isString() ) {\
            item = j_data[#item].asString();\
        } else {\
            VLOG(WARNING) << __FUNCTION__ << ", Fail String Item: " << #item;\
            return false;\
        }\
    } while(0)\

#define JSON_ITEM_TO_BOOL(j_data, item) \
    do {\
        if ( !j_data[#item].isNull() && j_data[#item].isBool() ) {\
            item = j_data[#item].asBool();\
        } else {\
            VLOG(WARNING) << __FUNCTION__ << ", Fail Bool Item: " << #item;\
            return false;\
        }\
    } while(0)\

#define JSON_ITEM_TO_ARRAY(j_data, item) \
    do {\
        if ( !j_data[#item].isNull() && j_data[#item].isArray() ) {\
            item = j_data[#item];\
        } else {\
            VLOG(WARNING) << __FUNCTION__ << ", Fail Array Item: " << #item;\
            return false;\
        }\
    } while(0)\

#define JSON_ITEM_TO_OBJECT(j_data, item) \
    do {\
        if ( !j_data[#item].isNull() && j_data[#item].isObject() ) {\
            item = j_data[#item];\
        } else {\
            VLOG(WARNING) << __FUNCTION__ << ", Fail Object Item: " << #item;\
            return false;\
        }\
    } while(0)\

#endif /* defined(__TrioDM__Common__) */
