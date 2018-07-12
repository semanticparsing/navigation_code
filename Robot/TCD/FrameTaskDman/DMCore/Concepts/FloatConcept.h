// Author : lzy
// Email  : liuzhengyong@trio.ai
// Date   : 2017-09-12 16:18
// Description: 浮点数类数据

#ifndef _FLOAT_CONCEPT_H__
#define _FLOAT_CONCEPT_H__

#include "Concept.h"
#include "Robot/TCD/FrameTaskDman/Utils/Common.h"

namespace TrioTDM {

class CFloatConcept;
/**
 * @brief 浮点数类型值
 */
class CFloatHyp : public CHyp {
    
    friend class CFloatConcept;

public:
    CFloatHyp();
    CFloatHyp(CFloatHyp& hyp);
    CFloatHyp(float value, float confidence = 1.0);

    CHyp& operator = (float value);
    virtual CHyp& operator = (CHyp& hyp);

    virtual bool operator == (CHyp& hyp);
    virtual bool operator < (CHyp& hyp);
    virtual bool operator > (CHyp& hyp);
    virtual bool operator <= (CHyp& hyp);
    virtual bool operator >= (CHyp& hyp);

    virtual CHyp* operator [](const std::string& item);
    virtual std::string ValueToString();
    virtual std::string ToString();
    virtual void FromString(const std::string& strg);

protected:
    float _value; ///< 浮点数值
};

#define DEFAULT_FLOAT_CARDINALITY 1000
#define CFloat(Value) (CConcept&)CFloatConcept("NONAME", Value, 1, csSystem)
/**
 * @brief 浮点型槽值数据
 */
class CFloatConcept: public CConcept {

public:
    CFloatConcept(const std::string& name = "NONAME", TConceptSource source = csUser);
    CFloatConcept(const std::string& name, float value, float confidence = 1.0, TConceptSource source = csUser);

    virtual CConcept& operator = (CConcept& concept) { return CConcept::operator = (concept); };
    virtual CConcept& operator = (int value);
    virtual CConcept& operator = (bool value);
    virtual CConcept& operator = (float value);
    virtual CConcept& operator = (double value);
    virtual CConcept& operator = (const std::string& value) { return CConcept::operator = (value); };
    virtual CConcept& operator = (const char* value) {return CConcept::operator = (value); };

    virtual operator bool();
    virtual operator int();
    virtual operator float();

    virtual CConcept* EmptyClone();
    virtual CHyp* HypFactory();

};


}

#endif
