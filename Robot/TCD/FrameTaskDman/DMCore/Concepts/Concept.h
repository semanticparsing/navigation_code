// Author : lzy
// Email  : liuzhengyong@trio.ai
// Date   : 2017-08-09 16:08
// Description: 槽值类数据存储结构 

#ifndef _CONCET_H__
#define _CONCET_H__

#include "Robot/TCD/FrameTaskDman/Utils/Common.h"
#include "Robot/TCD/FrameTaskDman/DMCore/Grounding/Grounding.h"

namespace TrioTDM {

class CDialogAgent;

/**
 * @brief 数据类型枚举值
 * @see ::ConceptTypeAsString
 */
typedef enum { ctUnknown, ctInt, ctBool, ctString, ctFloat, ctStruct,
            ctFrame, ctArray } TConceptType;

/**
 * @brief 数据类型字符串类型
 * @see ::TConceptType
 */
static std::string ConceptTypeAsString[] = {"UnKnown", "Int", "Bool", "String", "Float", "Struct", "Frame", "Array"};

/**
 * @brief 数据来源
 */
typedef enum { csUnKnow, csUser, csSystem} TConceptSource;
/**
 * @brief 数据传输结果，枚举值
 */
typedef enum { cConveyed, cNotConveyed, cFailed} TConveyance;
/**
 * @brief 数据传输结果，字符串结果
 */
static std::string ConveyanceAsString[] = {"Conveyed", "Not conveyed", "Failed"};

#define VAL_CONF_SEPARATOR "|"
#define HYPS_SEPARATOR ";"
#define CONCEPT_EQUALS_SIGN "= \t"

#define ABSTRACT_CONCEPT "<ABSTRACT>\n"
#define UNDEFINED_CONCEPT "<UNDEFINED>\n"
#define INVALIDATED_CONCEPT "<INVALIDATED>\n"
#define UNDEFINED_VALUE "<UNDEF_VAL>"

/**
 * @brief 槽位所对应的值的基类
 */
class CHyp {

public:
    CHyp();
    CHyp(CHyp& hyp);
    virtual ~CHyp();

    TConceptType GetHypType();

    float GetConfidence();

    virtual CHyp& operator = (CHyp& hyp);
    virtual void SetConfidence(float confidence);

    virtual bool operator == (CHyp& hyp);
    virtual bool operator < (CHyp& hyp);
    virtual bool operator > (CHyp& hyp);
    virtual bool operator <= (CHyp& hyp);
    virtual bool operator >= (CHyp& hyp);

    virtual CHyp* operator [](const std::string& item);
    /**
     * @brief 数据值序列化
     */
    virtual std::string ValueToString();
    /**
     * @brief 数据序列化
     * @note 包含数据值 和 可信度
     */
    virtual std::string ToString();
    /**
     * @brief 字符串反序列化到数据结构
     */
    virtual void FromString(const std::string& str);

protected:
    TConceptType _hyp_type; ///< 值类型
    float _confidence; ///< 值可信度
};

#define DEFAULT_HYPSET_CARDINALITY 1000 ///< 定义默认的基数

#define FREE_PROB_MASS ((float)0.05)

// concept 的更新类型
#define CU_ASSIGN_FROM_STRING   "assign_from_string"
#define CU_ASSIGN_FROM_CONCEPT  "assign_from_concept"
#define CU_UPDATE_WITH_CONCEPT  "update_with_concept"
#define CU_COLLAPSE_TO_MODE     "collapse_to_mode"
#define CU_PARTIAL_FROM_STRING  "partial_from_string"

/**
 * @brief 所有槽值数据类的基类
 */
class CConcept {

public:
    CConcept(const std::string& name = "NONAME",
            TConceptSource concept_scorce = csUser,
            int cardinality = DEFAULT_HYPSET_CARDINALITY);

    virtual ~CConcept();
    /**
     * @brief 结构体成员数
     */
    virtual size_t GetItemsSize();

    virtual CConcept& operator = (CConcept& concept);
    virtual CConcept& operator = (int value);
    virtual CConcept& operator = (bool value);
    virtual CConcept& operator = (float value);
    virtual CConcept& operator = (double value);
    virtual CConcept& operator = (const std::string& value);
    virtual CConcept& operator = (const char* value);
    virtual bool operator == (CConcept& concept);
    virtual bool operator != (CConcept& concept);
    virtual bool operator < (CConcept& concept);
    virtual bool operator > (CConcept& concept);
    virtual bool operator <= (CConcept& concept);
    virtual bool operator >= (CConcept& concept);
    virtual CConcept& operator [](int index);
    virtual CConcept& operator [](const std::string& index);
    virtual CConcept& operator [](const char* index);

    virtual operator int();
    virtual operator float();
    virtual operator bool();
    virtual operator std::string();

    virtual void Clear();

    virtual void ClearCurrentValue();
    virtual CConcept* Clone(bool cone_history = true);

    virtual CConcept* EmptyClone();
    /**
     * @brief 更新 concept
     */
    virtual void Update(const std::string& update_type, const void* update_data);
    /**
     * @brief 使用序列化的字符串赋值
     */
    virtual void Update_NPU_AssignFromString(const void* update_data);
    /**
     * @brief 使用 concept 赋值
     */
    virtual void Update_NPU_AssignFromConcept(const void* update_data);
    /**
     * @brief 使用给定的 concept 更新数据
     */
    virtual void Update_NPU_UpdateWithConcept(const void* update_data);
    virtual void Update_NPU_CollapseToMode(const void* update_data);
    /**
     * @brief 使用序列化的字符串给不完整数据赋值
     */
    virtual void Update_PartialFromString(const void* update_data);
    virtual void Update_Calista_AssignFromString(const void* update_data);
    virtual void Update_Calista_AssignFromConcept(const void* update_data);
    virtual void Update_Calista_UpdateWithConcept(const void* update_data);
    virtual void Update_Calista_CollapseToMode(const void* update_data);
    /**
     * @brief 数据是否更新，当前数据不为空
     */
    virtual bool IsUpdated();
    /**
     * @brief 是否更新，且是 基础类
     */
    virtual bool IsUpdatedAndGrounded();
    /**
     * @brief 是否存在有效数据，是否存在当前数据或者历史数据
     */
    virtual bool IsAvailable();
    /**
     * @brief 是否存在有效且基础数据
     */
    virtual bool IsAvailableAndGrounded();
    /**
     * @brief 是否有不完整数据
     */
    virtual bool HasPartialUpdate();
    /**
     * @brief 是否基础数据
     */
    virtual bool IsGrounded(); 
    virtual bool IsUndergoingGrounding();
    /**
     * @brief 数据是否有效
     */
    virtual bool IsInvalidated();
    /**
     * @brief 是否恢复到基础模型数据
     */
    virtual bool IsRestoredForGrounding();
    /**
     * @brief 将基础数据序列化
     */
    virtual std::string GroundedHypToString();
    /**
     * @brief 最新的数据序列化
     */
    virtual std::string TopHypToString();
    /**
     * @brief 所有的数据序列化
     */
    virtual std::string HypSetToString();
    /**
     * @brief 获得数据类型
     */
    virtual TConceptType GetConceptType();
    /**
     * @brief 设置数据类型
     */
    virtual void SetConceptType(TConceptType concept_type);
    /**
     * @brief 获取数据源
     */
    virtual TConceptSource GetConceptSource();
    /**
     * @brief 设置数据源
     */
    virtual void SetConceptSource(TConceptSource concept_source);
    /**
     * @brief 设置数据名，可以理解成槽位
     */
    virtual void SetName(const std::string& name);
    /**
     * @brief 获取数据名
     */
    std::string GetName();
    /**
     * @brief 数据名以 . 号做分割取最后一个
     */
    std::string GetSmallName();
    /**
     * @brief 使用关联给数据的节点路径名连接名字
     */
    std::string GetAgentQualifiedName();
    /**
     * @brief 设置所属的节点
     */
    virtual void SetOwnerDialogAgent(CDialogAgent* dialog_agent);
    /**
     * @brief 获取所属的节点
     */
    CDialogAgent* GetOwnerDialogAgent();
    /**
     * @brief 设置所属的 concept 
     */
    virtual void SetOwnerConcept(CConcept* concept);
    /**
     * @brief 获取所属的 concept
     */
    CConcept* GetOwnerConcept();
    /**
     * @brief 创建数据模型
     */
    virtual void CreateGroundingModel(const std::string& grounding_mondle_spec);
    /**
     * @brief 获取数据模型
     */
    CGMConcept* GetGroundingModel();
    /**
     * @brief 设置基础标识
     */
    virtual void SetGroundedFlag(bool grounded = true);
    /**
     * @brief 获取基础标识
     */
    virtual bool GetGroundedFlag();
    /**
     * @brief 生成关联的基础模型
     */
    virtual void DeclareGroundingModels (
            std::vector<CGroundingModel*>& models,
            std::set<CGroundingModel*>& exclude);
    /**
     * @brief 生成该数据集合
     */
    virtual void DeclareConcepts(
            std::vector<CConcept*>& concepts,
            std::set<CConcept*>& exclude);
    /**
     * @brief 设置数据是否有效
     */
    virtual void SetInvalidatedFlag(bool invalidated = true);
    /**
     * @brief 获取数据是否有效
     */
    virtual bool GetInvalidatedFlag();
    /**
     * @brief 设置是否恢复到基础数据
     */
    virtual void SetRestoredForGroundingFlag(
            bool restored_for_grounding = true);
    /**
     * @brief 获取是否恢复到基础数据
     */
    virtual bool GetRestoredForGroundingFlag();
    /**
     * @brief 设置数据独立
     */
    virtual void Seal();
    /**
     * @brief 设置数据不独立
     */
    virtual void BreakSeal();
    /**
     * @brief 数据是否独立
     */
    virtual bool IsSealed();
    /**
     * @brief 数据更新不需要发布通知
     */
    virtual void DisableChangeNotification();
    /**
     * @brief 数据更新需要发布通知
     */
    virtual void EnableChangeNotification();
    /**
     * @brief 设置数据更改是否需要发布通知
     */
    virtual void SetChangeNotification(bool change_notification = true);
    /**
     * @brief 数据更新通知
     */
    virtual void NotifyChange();
    /**
     * @brief 产生一个数据对象
     */
    virtual CHyp* HypFactory();
    /**
     * @brief 增加指定数据
     */
    virtual int AddHyp(CHyp* hyp);
    /**
     * @brief 增加一个新数据
     */
    virtual int AddNewHyp();
    /**
     * @brief 增加一个新的空数据
     */
    virtual int AddNullHyp();
    /**
     * @brief 设置相应索引位的数据
     */
    virtual void SetHyp(int index, CHyp* hyp);
    /**
     * @brief 设置相应索引位的数据为空
     */
    virtual void SetNullHyp(int index);
    /**
     * @brief 删除相应索引位的数据
     */
    virtual void DeleteHyp(int index);
    /**
     * @brief 获取相应索引位的数据
     */
    virtual CHyp* GetHyp(int index);
    /**
     * @brief 获取相应数据的索引位
     */
    virtual int GetHypIndex(CHyp* hyp);
    /**
     * @brief 获取相应索引位的数据的可信度
     */
    virtual float GetHypConfidence(int index);
    /**
     * @brief 设置相应索引位数据的可信度
     */
    virtual void SetHypConfidence(int index, float confidence);
    /**
     * @brief 获取可信度最高的索引位的数据
     */
    virtual CHyp* GetTopHyp();
    /**
     * @brief 获取可信度最高的数据对应的索引
     */
    virtual int GetTopHypIndex();
    /**
     * @brief 获取可信度第二高度数据对应的索引
     */
    virtual int Get2ndHypIndex();
    /**
     * @brief 获取数据的可信度最大值
     */
    virtual float GetTopHypConfidence();
    /**
     * @brief 判断相应索引位的数据是否有效
     */
    virtual bool IsValidHyp(int index);
    /**
     * @brief 获取当前数据集合大小
     */
    virtual int GetNumHyps();
    /**
     * @brief 获取数据集合中有效数据的个数
     */
    virtual int GetNumValidHyps();
    /**
     * @brief 清空当前数据集合
     */
    virtual void ClearCurrentHypSet();
    /**
     * @brief 从指定数据中拷贝当前数据集合
     */
    virtual void CopyCurrentHypSetFrom(CConcept& concept);
    /**
     * @brief 设置数据基数
     */
    virtual void SetCardinality(int cardinality);
    /**
     * @brief 获取数据基数
     */
    virtual int GetCardinality();
    /**
     * @brief 获取指定数据的优先级
     */
    virtual float GetPriorForHyp(CHyp* hyp);
    /**
     * @brief 获取指定数据的混淆值
     */
    virtual float GetConfusabilityForHyp(CHyp* hyp);
    /**
     * @brief 获取数据类型信息
     */
    virtual std::string GetConceptTypeInfo();
    /**
     * @brief 设置显示确认的数据
     */
    virtual void SetExplicitlyConfirmedHyp(CHyp* hyp);
    /**
     * @brief 设置显示确认的数据
     */
    virtual void SetExplicitlyConfirmedHyp(const std::string& hyp);
    /**
     * @brief 设置显示否认的数据
     */
    virtual void SetExplicitlyDisconfirmedHyp(CHyp* hyp);
    /**
     * @brief 设置显示否认的数据
     */
    virtual void SetExplicitlyDisconfirmedHyp(const std::string hyp);
    /**
     * @brief 获取显示确认的数据
     */
    virtual std::string GetExplicitlyConfirmedHypAsString();
    /**
     * @brief 获取显示否认的数据
     */
    virtual std::string GetExplicitlyDisconfirmedHypAsString();
    /**
     * @brief 清空显示确认数据
     */
    virtual void ClearExplicitlyConfirmedHyp();
    /**
     * @brief 清空显示否认数据
     */
    virtual void ClearExplicitlyDisconfirmedHyp();
    /**
     * @brief 增加指定的不完整数据
     */
    virtual int AddPartialHyp(CHyp* hyp);
    /**
     * @brief 增加一个新生成的不完整数据
     */
    virtual int AddNewPartialHyp();
    /**
     * @brief 增加一个空的不完整数据
     */
    virtual int AddNullPartialHyp();
    /**
     * @brief 是否存在有效的不完整数据
     */
    virtual bool HasPartialHyp();
    /**
     * @brief 获取指定索引位置的不完整数据
     */
    virtual CHyp* GetPartialHyp(int index);
    /**
     * @brief 获取可信度最高的不完整数据
     */
    virtual CHyp* GetTopPartialHyp();
    /**
     * @brief 获取可信度最高的不完整数据的索引
     */
    virtual int GetTopPartialHypIndex();
    /**
     * @brief 获取可信度最高的不完整数据的可信度
     */
    virtual float GetTopPartialHypConfidence();
    /**
     * @brief 指定位置的不完整数据是否有效
     */
    bool IsValidPartialHyp(int index);
    /**
     * @brief 获取不完整数据的集合大小
     */
    virtual int GetNumPartialHyps();
    /**
     * @brief 获取有效的不完整数据的数量
     */
    virtual int GetNumValidPartialHyps();
    /**
     * @brief 清空不完整数据集合
     */
    virtual void ClearPartialHypSet();
    /**
     * @brief 设置最后的交互轮数
     */
    virtual void SetTurnLastUpdated(int turn);
    /**
     * @brief 获取最后的交互轮数并设置
     */
    virtual void MarkTurnLastUpdated();
    /**
     * @brief 获取导致数据更新的最后一次交互轮数
     */
    virtual int GetTurnLastUpdated();
    /**
     * @brief 获取最后一次更新的交互轮数与当前交互轮数的差值
     */
    int GetTurnsSinceLastUpdated();
    /**
     * @brief 设置等待传输标识为 true
     */
    void SetWaitingConveyance();
    /**
     * @brief 取消等待传输的数据
     */
    void ClearWaitingConveyance();
    /**
     * @brief 设置传输状态
     */
    virtual void SetConveyance(TConveyance conveyance);
    /**
     * @brief 获取传输状态
     */
    TConveyance GetConveyance();
    virtual void ClearConceptNotificationPointer();
    /**
     * @brief 重启 concept，将当前数据保存为历史数据
     */
    virtual void ReOpen();
    /**
     * @brief 使用指定索引位的数据重置当前数据
     */
    virtual void Restore(int index = -1);
    /**
     * @brief 清空历史数据
     */
    virtual void ClearHistory();
    /**
     * @brief 将所有历史数据合并成一个 concept 并返回
     */
    virtual CConcept* CreateMergedHistoryConcept();
    /**
     * @brief 如果当前数据无效，则使用历史数据重置当前数据
     */
    virtual void MergeHistory();
    /**
     * @brief 获取保存的历史数据的数量
     */
    int GetHistorySize();
    /**
     * @brief 获取指定位置的历史数据
     */
    virtual CConcept& GetHistoryVersion(int index);
    /**
     * @brief 设置是否存在历史数据
     */
    virtual void SetHistoryConcept(bool history_concept = true);
    /**
     * @brief 返回是否存在历史数据
     */
    virtual bool IsHistoryConcept();
    /**
     * @brief 获取数据量大小
     */
    virtual int GetSize();
    /**
     * @brief 删除指定索引位的数据
     */
    virtual void DeleteAt(size_t index);
    /**
     * @brief 在指定索引位插入数据
     */
    virtual void InsertAt(size_t index, CConcept& concept);

public:
    static CConcept NULLConcept; ///< NULL concept

protected:
    TConceptType _concept_type; ///< 数据类型
    TConceptSource _concept_source; ///< 数据源

    std::string _name; ///< 数据名字，可理解为槽位
    CDialogAgent* _owner_dialog_agent; ///< 所属的节点
    CConcept* _owner_concept; ///< 所属的数据
    CGMConcept* _grounding_model; ///< 数据对应的模型

    bool _is_grounded; ///< 是否是基础数据

    bool _is_invalidated; ///< 数据是否有效

    bool _is_restored_for_grounding; ///< 数据是否恢复到基础数据

    bool _is_sealed; ///< 数据是否已经改变

    bool _is_change_notification; ///< 数据改变是否应该发布通知

    std::vector<CHyp*> _current_hyps; ///< 对应的数据集合
    int _num_valid_hyps; ///< 有效数据数量

    std::vector<CHyp*> _partial_hyps; ///< 不完整的数据集合
    int _num_valid_partial_hyps; ///< 有效的不完整数据数量

    int _cardinality; ///< 基数
    int _turn_last_updated; ///< 数据最后更新的对话轮数
    TConveyance _conveyance; ///< 数据传输结果类型
    bool _is_waiting_conveyance; ///< 数据是否在等待传输

    CConcept* _prev_concept; ///< 数据链接的前一个数据
    bool _is_history_concept; ///< 是否存在历史数据

    std::string _explictly_confirmed_hyp; ///< 显示确实的数据
    std::string _explicitly_disconfirmed_hyp; ///< 显示否认的数据

};

#define DEFINE_CONCEPT_TYPE(NewConceptTypeName, BaseConceptTypeName, OTHER_CONTENTS) \
    class NewConceptTypeName : public BaseConceptTypeName {\
    public:\
        NewConceptTypeName(const std::string name, TConceptSource concept_source = csUser): \
        BaseConceptTypeName(name, concept_source) {};\
        virtual CConcept* EmptyClone() {\
            return new NewConceptTypeName;\
        };\
        OTHER_CONTENTS;\
    };

#define DEFINE_PRIOR(CODE) \
public:\
virtual float GetPriorForHyp(CHyp* hyp) {\
    CODE;\
}

#define DEFINE_CONFUSABILITY(CODE) \
public:\
virtual float GetConfusabilityForHyp(CHyp* hyp) { \
    CODE;\
}

}

#endif
