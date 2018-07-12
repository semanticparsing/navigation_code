// Author : lzy
// Email  : liuzhengyong@trio.ai
// Date   : 2017-09-05 19:26
// Description: 结构体类型数据 

#ifndef _STRUCT_CONCEPT_H__
#define _STRUCT_CONCEPT_H__

#include "Robot/TCD/FrameTaskDman/Utils/Common.h"
#include "Concept.h"

namespace TrioTDM {

class CStructConcept;

/**
 * @brief 结构体数据值
 */
class CStructHyp : public CHyp {

    friend class CStructConcept;

protected:
	std::map<std::string, CConcept*>* _item_map; ///< 指向 concept 的类成员变量 _item_map
	std::vector<std::string>* _items; ///< 指向 concept 的类成员变量 _items
	int	_hyp_index; ///< 结构体中各字段值的索引

public:
	CStructHyp(std::map<std::string, CConcept*>* pAItemMap = NULL, std::vector<std::string>* psvAItems = NULL, 
		int iAHypIndex = -1, bool bComplete = true);
    CStructHyp(CStructHyp& rAStructHyp);

    void SetHypIndex(int iAHypIndex);

	virtual CHyp& operator = (CHyp& rAHyp);

    virtual void SetConfidence(float fAConfidence);

    virtual bool operator == (CHyp& rAHyp);
    virtual bool operator < (CHyp& rAHyp);
    virtual bool operator > (CHyp& rAHyp);
    virtual bool operator <= (CHyp& rAHyp);
    virtual bool operator >= (CHyp& rAHyp);
    
    virtual CHyp* operator [](const std::string& sItem);
	
	virtual string ValueToString();
	virtual string ToString();
	virtual void FromString(const std::string& sString);
};

#define DEFAULT_STRUCT_CARDINALITY 1000
/**
 * @brief 结构体类 concept 数据
 */
class CStructConcept: public CConcept {

protected:
	std::map<std::string, CConcept*> _item_map; ///< 结构体字段名 和 其对应的数据的 映射
	std::vector<std::string> _items; ///< 结构体字段名集合

public:
	
	CStructConcept(const std::string& name = "NONAME",
		TConceptSource source = csUser);
    ~CStructConcept();

    virtual size_t GetItemsSize();
    /**
     * @brief 创建结构体，即初始化 _item_map 和 _items
     */
	virtual void CreateStructure();

	virtual void DestroyStructure();

    virtual bool operator < (CConcept& rAConcept);
    virtual bool operator > (CConcept& rAConcept);
    virtual bool operator <= (CConcept& rAConcept);
    virtual bool operator >= (CConcept& rAConcept);
	virtual CConcept& operator [] (const std::string& sAItemName);
	virtual CConcept& operator [] (int index) { return CConcept::operator[](index); };
	virtual CConcept& operator [] (const char* index) { return operator[](std::string(index));};

    virtual CConcept& operator = (CConcept& concept) { return CConcept::operator = (concept); };
    virtual CConcept& operator = (int value) { return CConcept::operator = (value); };
    virtual CConcept& operator = (bool value){ return CConcept::operator = (value); };
    virtual CConcept& operator = (float value){ return CConcept::operator = (value); };
    virtual CConcept& operator = (double value){ return CConcept::operator = (value); };
    virtual CConcept& operator = (const std::string& value) { return CConcept::operator = (value); };
    virtual CConcept& operator = (const char* value) {return CConcept::operator = (value); };
	virtual CConcept* Clone(bool bCloneHistory = true);

	virtual void Update_NPU_AssignFromString(const void* pUpdateData);

	virtual void Update_PartialFromString(const void* pUpdateData);

	virtual std::string HypSetToString();

	virtual void SetName(const std::string& sAName);

	virtual void SetOwnerDialogAgent(CDialogAgent* pADialogAgent);

	virtual void CreateGroundingModel(const std::string& sGroundingModelSpec);

    virtual void DeclareGroundingModels(
        std::vector<CGroundingModel*>& rgmpvModels,
        std::set<CGroundingModel*>& rgmpsExclude);

    virtual void DeclareConcepts(
	    std::vector<CConcept*>& rcpvConcepts, 
		std::set<CConcept*>& rcpsExclude);

    virtual void SetChangeNotification(
		bool bAChangeNotification = true);

	virtual CHyp* HypFactory();

    virtual int AddNewHyp();

    virtual int AddNullHyp();

    virtual void SetHyp(int iIndex, CHyp* pHyp);
    virtual void SetNullHyp(int iIndex);

	virtual void DeleteHyp(int iIndex);

    virtual void ClearCurrentHypSet();

	virtual int AddNewPartialHyp();

    virtual int AddNullPartialHyp();
	virtual void SetHistoryConcept(bool bAHistoryConcept = true);

protected:
	virtual void UpdateFromString(const void* pUpdateData, const std::string& sUpdateType);

};

#define DEFINE_STRUCT_CONCEPT_TYPE(StructConceptTypeName, OTHER_CONTENTS) \
    class StructConceptTypeName : public CStructConcept {\
    public:\
        StructConceptTypeName(const std::string& name = "NONAME", TConceptSource source = csUser) : \
            CStructConcept(name, source) {\
        }\
        virtual CConcept* EmptyClone() {\
            return new StructConceptTypeName;\
        }\
        OTHER_CONTENTS \
    };\

#define ITEMS(ITEM_DEFINITIONS) \
    virtual void CreateStructure() {\
        ITEM_DEFINITIONS \
    }\

#define ITEM(Name, ConceptType) \
    _item_map.insert(std::make_pair(#Name,\
        new ConceptType(_name + "." + #Name, _concept_source)));\
    _item_map[#Name]->SetOwnerDialogAgent(_owner_dialog_agent);\
    _item_map[#Name]->SetOwnerConcept(this);\
    _items.push_back(#Name);\

}

#endif
