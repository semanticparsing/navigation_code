// Author : lzy
// Email  : liuzhengyong@trio.ai
// Date   : 2017-08-28 11:13
// Description: 整数类型数据

#ifndef _INT_CONCEPT_H__
#define _INT_CONCEPT_H__

#include "Concept.h"
#include "Robot/TCD/FrameTaskDman/Utils/Common.h"

namespace TrioTDM {

class CIntConcept;
/**
 * @brief 整形数据值
 */
class CIntHyp : public CHyp {

    friend class CIntConcept;

public:
    CIntHyp();
    CIntHyp(CIntHyp& hyp);
    CIntHyp(int value, float confidence = 1.0);

    CHyp& operator = (int value);
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
    int _value;

};

#define DEFAULT_INT_CARDINALITY 1000
#define CInt(Value) (CConcept&)CIntConcept((std::string)"NONAME", Value, 1, csSystem)
/**
 * @brief 整形 concept 数据
 */
class CIntConcept: public CConcept {

public:
    CIntConcept(const std::string& name = "NONAME", TConceptSource sorce = csUser);
    CIntConcept(const std::string& name, int value, float confidence = 1.0, TConceptSource source = csUser);

    virtual CConcept& operator = (CConcept& concept) { return CConcept::operator = (concept); };
    virtual CConcept& operator = (int value);
    virtual CConcept& operator = (bool value);
    virtual CConcept& operator = (float value);
    virtual CConcept& operator = (double value);
    virtual CConcept& operator = (const std::string& value) { return CConcept::operator = (value); };
    virtual CConcept& operator = (const char* value) {return CConcept::operator = (value); };
    virtual operator bool();
    virtual operator int();

    virtual CConcept* EmptyClone();
    virtual CHyp* HypFactory();

};

}

#endif
