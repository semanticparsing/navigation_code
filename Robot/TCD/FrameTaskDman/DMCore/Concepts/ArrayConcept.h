// Author : lzy
// Email  : liuzhengyong@trio.ai
// Date   : 2017-09-18 19:39
// Description: 数组类型数据

#ifndef _ARRAY_CONCEPT_H__
#define _ARRAY_CONCEPT_H__

#include "Concept.h"
#include "Robot/TCD/FrameTaskDman/Utils/Common.h"

namespace TrioTDM {

/**
 * @brief 数组型数据类
 */
class CArrayConcept: public CConcept {

public:
    CArrayConcept(const std::string& name = "NONAME",
            TConceptSource source = csUser);
    virtual ~CArrayConcept();
    /**
     * @brief 创建一个数组成员元素
     */
    virtual CConcept* CreateElement() = 0;

    virtual bool operator == (CConcept& concept);
    virtual bool operator < (CConcept& concept);
    virtual bool operator > (CConcept& concept);
    virtual bool operator <= (CConcept& concept);
    virtual bool operator >= (CConcept& concept);

    virtual CConcept& operator [](int index);
    virtual CConcept& operator [](const std::string& index);
	virtual CConcept& operator [] (const char* index) { return operator[](std::string(index));};
    
    virtual CConcept& operator = (CConcept& concept) { return CConcept::operator = (concept); };
    virtual CConcept& operator = (int value) { return CConcept::operator = (value); };
    virtual CConcept& operator = (bool value){ return CConcept::operator = (value); };
    virtual CConcept& operator = (float value){ return CConcept::operator = (value); };
    virtual CConcept& operator = (double value){ return CConcept::operator = (value); };
    virtual CConcept& operator = (const std::string& value) { return CConcept::operator = (value); };
    virtual CConcept& operator = (const char* value) {return CConcept::operator = (value); };
    /**
     * @brief 从当前数据克隆数据
     * @param[in] is_clone_history 是否克隆历史数据
     */
    virtual CConcept* Clone(bool is_clone_history = true);
    /**
     * @brief 使用序列化的字符串更新数据
     */
    virtual void Update_NPU_AssignFromString(const void* update_data);
    virtual void Update_NPU_UpdateWithConcept(const void* update_data);
    virtual void Update_NPU_CollapseToMode(const void* update_data);
    virtual void Update_Calista_UpdateWithConcept(const void* update_data);
    virtual void Update_Calista_CollapseToMode(const void* update_data);
    /**
     * @brief 获取数据是否更新
     */
    virtual bool IsUpdated();
    virtual bool IsUndergoingGrounding();
    /**
     * @brief 基础数据序列化
     */
    virtual std::string GroundedHypToString();
    /**
     * @brief 有用数据序列化
     */
    virtual std::string TopHypToString();
    /**
     * @brief 所有数据序列化
     */
    virtual std::string HypSetToString();
    /**
     * @brief 设置数据名字
     */
    virtual void SetName(const std::string& name);
    /**
     * @brief 设置数据所属会话节点
     */
    virtual void SetOwnerDialogAgent(CDialogAgent* agent);
    /**
     * @brief 创建基础数据模型
     */
    virtual void CreateGroundingModel(const std::string& grounding_model_spec);
    /**
     * @brief 生成基础数据模型集合
     */
    virtual void DeclareGroundingModels(
            std::vector<CGroundingModel*>& models,
            std::set<CGroundingModel*>& exclude);
    /**
     * @brief 生成数据集合
     */
    virtual void DeclareConcepts(
            std::vector<CConcept*>& concepts,
            std::set<CConcept*>& exclude);
    /**
     * @brief 设置数据变化是否发布通知
     */
    virtual void SetChangeNotification(bool flag = true);
    /**
     * @brief 生成一个数据元素
     * @note 数组不支持该方法
     */
    virtual CHyp* HypFactory();
    /**
     * @brief 增加一个指定的数据
     * @note 数组不支持该方法
     */
    virtual int AddHyp(CHyp* hyp);
    /**
     * @brief 增加一个新的数据
     * @note 数组不支持该方法
     */
    virtual int AddNewHyp();
    /**
     * @brief 增加一个空数据
     * @note 数组不支持该方法
     */
    virtual int AddNullHyp();
    /**
     * @brief 使用指定数据设置相应索引位的数据
     * @note 数组不支持该方法
     */
    virtual void SetHyp(int index, CHyp* hyp);
    /**
     * @brief 设置指定索引位的数据为 NULL
     * @note 数组不支持该方法
     */
    virtual void SetNullHyp(int index);
    /**
     * @brief 删除指定索引位的数据
     * @note 数组不支持该方法
     */
    virtual void DeleteHyp(int index);
    /**
     * @brief 获取指定索引位的数据
     * @note 数组不支持该方法
     */
    virtual CHyp* GetHyp(int index);
    /**
     * @brief 获取指定数据的索引
     * @note 数组不支持该方法
     */
    virtual int GetHypIndex(CHyp* hyp);
    /**
     * @brief 获取指定索引位数据的可信度
     * @note 数组不支持该方法
     */
    virtual float GetHypConfidence(int index);
    /***
     * @brief 获取可信度最高的数据
     * @note 数组不支持该方法
     */
    virtual CHyp* GetTopHyp();
    /**
     * @brief 获取可信度最高的数据的索引
     * @note 数组不支持该方法
     */
    virtual int GetTopHypIndex();
    virtual float GetTopHypConfidence();
    virtual bool IsValidHyp(int index);
    virtual int GetNumHyps();
    virtual int GetNumValidHyps();
    virtual void ClearCurrentHypSet();
    virtual void CopyCurrentHypSetFrom(CConcept& concept);
    virtual int GetTurnLastUpdated();
    virtual void SetHistoryConcept(bool flag = true);
    virtual int GetSize();
    virtual void DeleteAt(size_t index);
    virtual void InsertAt(size_t index, CConcept& concept);

protected:
    std::vector<CConcept*> _concept_array; ///< 数组数据集合
    std::string _element_grounding_model_spec; ///< 基础数据模型的配置
};

#define DEFINE_ARRAY_CONCEPT_TYPE(ElementaryConceptType, ArrayConceptTypeName) \
	class ArrayConceptTypeName: public CArrayConcept {\
	public:\
		ArrayConceptTypeName(string aName = "NONAME",\
							 TConceptSource csAConceptSource = csUser): \
            CArrayConcept(aName, csAConceptSource) {\
		}\
		virtual CConcept* CreateElement() {\
            CConcept* pConcept;\
            pConcept = new ElementaryConceptType("NONAME", _concept_source);\
            pConcept->CreateGroundingModel(_element_grounding_model_spec);\
            pConcept->SetOwnerDialogAgent(_owner_dialog_agent);\
            pConcept->SetOwnerConcept(this);\
            return pConcept;\
		}\
        virtual CConcept* EmptyClone() {\
            return new ArrayConceptTypeName;\
        }\
        virtual CConcept& operator = (CConcept& concept) { return CArrayConcept::operator = (concept); };\
        virtual CConcept& operator = (int value) { return CArrayConcept::operator = (value); };\
        virtual CConcept& operator = (bool value){ return CArrayConcept::operator = (value); };\
        virtual CConcept& operator = (float value){ return CArrayConcept::operator = (value); };\
        virtual CConcept& operator = (double value){ return CArrayConcept::operator = (value); };\
        virtual CConcept& operator = (const std::string& value) { return CArrayConcept::operator = (value); };\
        virtual CConcept& operator = (const char* value) {return CArrayConcept::operator = (value); };\
	};\

}

#endif
