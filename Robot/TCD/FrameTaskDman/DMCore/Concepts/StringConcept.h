// Author : lzy
// Email  : liuzhengyong@trio.ai
// Date   : 2017-08-28 11:29
// Description: 字符串类型数据

#ifndef _STRING_CONCEPT_H__
#define _STRING_CONCEPT_H__

#include "Robot/TCD/FrameTaskDman/Utils/Common.h"
#include "Concept.h"

namespace TrioTDM {

class CStringConcept;

/**
 * @brief 字符串类型数据
 */
class CStringHyp : public CHyp {

    friend class CStringConcept;

protected:
    std::string _value;

public:
    CStringHyp();
    CStringHyp(const std::string& value, float confidence = 1.0);
    CStringHyp(CStringHyp &hyp);

    CHyp& operator = (const std::string& value);
    CHyp& operator = (char* value);
    virtual CHyp& operator = (CHyp& value);

    virtual bool operator == (CHyp& hyp);
    virtual bool operator < (CHyp& hyp);
    virtual bool operator > (CHyp& hyp);
    virtual bool operator <= (CHyp& hyp);
    virtual bool operator >= (CHyp& hyp);

    virtual CHyp* operator [](const std::string& item);
    virtual std::string ValueToString();
    virtual std::string ToString();
    virtual void FromString(const std::string& strg);
};

#define DEFAULT_STRING_CARDINALITY 1000
#define CString(Value) (CConcept&)CStringConcept("NONAME", Value, 1, csSystem)
/**
 * @brief 字符串类槽值对
 */
class CStringConcept : public CConcept {

public:
    CStringConcept(const std::string& name = "NONAME", TConceptSource source = csUser);
    CStringConcept(CStringConcept& concept);
    CStringConcept(const std::string& name, const std::string& value, float confidence = 1.0, TConceptSource source = csUser);

    virtual CConcept& operator = (CConcept& concept);
    virtual CConcept& operator = (int value);
    virtual CConcept& operator = (bool value);
    virtual CConcept& operator = (float value);
    virtual CConcept& operator = (double value);
    virtual CConcept& operator = (const std::string& value) { return CConcept::operator = (value); };
    virtual CConcept& operator = (const char* value) { return CConcept::operator = (value); };

    virtual CConcept* EmptyClone();
    virtual CHyp* HypFactory();

};

}

#endif
