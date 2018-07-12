// Author : lzy
// Email  : liuzhengyong@trio.ai
// Date   : 2017-08-18 10:27
// Description: bool 类数据

#ifndef _BOOL_CONCEPT_H__
#define _BOOL_CONCEPT_H__

#include "Concept.h"

namespace TrioTDM {

class CBoolConcept;

/**
 * @brief bool 型数据的值
 */
class CBoolHyp : public CHyp {

    friend class CBoolConcept;

public:
    CBoolHyp();
    CBoolHyp(CBoolHyp& value);
    CBoolHyp(bool value, float confidence = 1.0);

    CHyp& operator = (bool value);
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
    bool _value; ///< 保存数据的变量
};

#define DEFAULT_BOOL_CARDINALITY 2
#define CBool(value) (CConcept &)CBoolConcept((std::string)"NONAME", value, 1, csSystem);
/**
 * @brief bool 型数据槽值对
 */
class CBoolConcept: public CConcept {

public:
    CBoolConcept(const std::string& name = "NONAME", TConceptSource source = csUser);
    CBoolConcept(const std::string& name, bool value, float confidence = 1.0, TConceptSource source = csUser);

    virtual CConcept& operator = (CConcept& concept) { return CConcept::operator=(concept);};
    virtual CConcept& operator = (int value);
    virtual CConcept& operator = (bool value);
    virtual CConcept& operator = (float value) { return CConcept::operator=(value); };
    virtual CConcept& operator = (double value) { return CConcept::operator=(value); };
    virtual CConcept& operator = (const std::string& value) { return CConcept::operator=(value); };
    virtual CConcept& operator = (const char* value) { return CConcept::operator=(value); };
    virtual operator bool();
    virtual operator int();

    virtual CConcept* EmptyClone();
    virtual CHyp* HypFactory();
};

}

#endif
