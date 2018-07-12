// Author : lzy
// Email  : liuzhengyong@trio.ai
// Date   : 2017-08-18 17:35
// Description: 任务树执行管理类

#ifndef _DM_CORE_AGENT_H__
#define _DM_CORE_AGENT_H__

#include "Robot/TCD/FrameTaskDman/Utils/Common.h"
#include "Common/ThirdParty/jsoncpp-0.5.0/include/json/json.h"
#include "Robot/TCD/FrameTaskDman/DMCore/Agents/Agent.h"
#include "Robot/TCD/FrameTaskDman/DMCore/Agents/DialogAgents/DialogAgent.h"
#include "Robot/TCD/FrameTaskDman/DMCore/Events/InteractionEvent.h"

namespace ChinaMobileProvince {

class ChinaMobileSession;

} 

namespace TrioTDM {

/** 类前置声明 **/
class CInteractionEventManagerAgent;
class CDTTManagerAgent;
class CStateManagerAgent;
class COutputManagerAgent;
class CGainConceptApi;
class CExecuteApi;
class CRegistryAgent;

typedef enum {
    fsUnknown,
    fsUser,
    fsSystem,
    fsFree,
} TFloorStatus;

/**
 * @brief 任务执行的返回结果枚举值
 */
typedef enum { 
    dmFail = 0,
    dmSuccuss,
    dmCloseSession
} TDMStatus;

/**
 * @brief 相应等级的期望 concept
 */
typedef struct CCompiledExpectationLevel {
    CCompiledExpectationLevel() {
        pdaGenerator = NULL;
    }
    virtual ~CCompiledExpectationLevel() {
    }
    std::map<std::string, std::vector<int> > mapCE;
    CDialogAgent* pdaGenerator;
} TCompiledExpectationLevel;

/**
 * @brief 所有等级的期望 concept
 */
typedef struct {
    std::vector<TConceptExpectation> celSystemExpectations;
    std::vector<TCompiledExpectationLevel> vCompiledExpectations;
} TExpectationAgenda;

/**
 * @brief 执行栈中的元素
 */
typedef struct CExecutionStackItem {
    CDialogAgent* pdaAgent;
    int iEHIndex;
    
    Json::Value ToJson() const;
    bool FromJson(const Json::Value& j_data, CRegistryAgent* registry_agent);
} TExecutionStackItem;

/**
 * @brief 执行栈
 */
typedef std::list<TExecutionStackItem> TExecutionStack;

Json::Value ExecutionStackToJson(const TExecutionStack& stack);
bool ExecutionStackFromJson(const Json::Value& j_data, TExecutionStack& stack, CRegistryAgent* registry_agent);

/**
 * @brief 历史执行 agent 信息保存项
 */
typedef struct CExecutionHistoryItem {
    /**
     * @brief 当前 Agent 路径名
     */
    std::string sCurrentAgent;
    /**
     * @brief 当前 Agent 的类型
     */
    std::string sCurrentAgentType;
    /**
     * @brief 将当前 Agent 入栈的 Agent
     */
    std::string sScheduledBy;
    bool bScheduled;
    /**
     * @brief 是否已经执行
     */
    bool bExecuted;
    bool bCommitted;
    bool bCanceled;
    /**
     * @brief Agent 执行时间
     */
    std::vector<UInt>   vtExecutionTimes;
    /**
     * @brief Agent 入栈的时间
     */
    UInt timeScheduled;
    /**
     * @brief Agent 出栈顶时间
     */
    UInt timeTerminated;
    
    int iStateHistoryIndex;

    Json::Value ToJson() const;
    bool FromJson(const Json::Value& j_data);
} TExecutionHistoryItem;

/**
 * @brief 历史执行 agent 信息保存
 */
class CExecutionHistory: public std::vector<TExecutionHistoryItem> {
};

/**
 * @brief 用户输入信息单个槽值到 concept 的关联信息
 */
typedef struct CBinding {
    bool bBlocked;
    std::string sGrammarExpectation;
    std::string sValue;
    float fConfidence;
    int iLevel;
    std::string sAgentName;
    std::string sConceptName;
    std::string sReasonDisabled;
    Json::Value ToJson() const;
    bool FromJson(const Json::Value& j_data);
} TBinding;

#define FCU_EXPLICIT_CONFIRM 1
#define FCU_IMPLICIT_CONFIRM 2
#define FCU_UNPLANNED_IMPLICIT_CONFIRM 3

/**
 * @brief 用户输入信息槽位所更新的 concept 信息
 */
typedef struct CForcedConceptUpdate {
    std::string sConceptName;
    int iType;
    bool bUnderstanding;
    Json::Value ToJson() const;
    bool FromJson(const Json::Value& j_data);
} TForcedConceptUpdate;

/**
 * @brief 用户输入信息所有槽位到 concept 关联信息
 */
typedef struct CBindingsDescr {
//    int iTurnNumber;
    std::string sEventType;
    bool bNonUnderstanding;
    int iConceptsBound;
    int iConceptsBlocked;
    int iSlotsMatched;
    int iSlotsBlocked;
    std::vector<TBinding> vbBindings;
    std::vector<TForcedConceptUpdate> vfcuForcedUpdates;

    Json::Value ToJson() const;
    bool FromJson(const Json::Value& j_data);
} TBindingsDescr;

/**
 * @brief 历史用户输入信息所有槽位到 concept 关联信息
 */
typedef std::vector<TBindingsDescr> TBindingHistory;

/**
 * @brief 槽位值和 concept 匹配时可调用的 槽值处理函数
 * @return 返回应该填入 concept 的值
 */
typedef std::string (*TBindingFilterFunct)(const std::string& slot_name, const std::string& slot_value);

/**
 * @brief 当前填充的 concept，按类型分类
 */
typedef struct CSystemAction {
    std::set<CConcept *> setcpRequests;
    std::set<CConcept *> setcpExplicitConfirms;
    std::set<CConcept *> setcpImplicitConfirms;
    std::set<CConcept *> setcpUnplannedImplicitConfirms;

    Json::Value ToJson() const;
    bool FromJson(const Json::Value& j_data, CDialogAgent* root);
} TSystemAction;

#define SA_REQUEST "REQ"
#define SA_EXPL_CONF "EC"
#define SA_IMPL_CONF "IC"
#define SA_UNPLANNED_IMPL_CONF "UIC"
#define SA_OTHER "OTH"

/**
 * @brief concept 对应的类型
 */
typedef struct CSystemActionOnConcept {
    CSystemActionOnConcept() {
        sSystemAction = "";
    }
    virtual ~CSystemActionOnConcept() {
    }
    std::string sSystemAction;
} TSystemActionOnConcept;

/**
 * @brief 用户自定义的流程启动调用函数
 */
typedef void (*TCustomStartOverFunct)();

/**
 * @brief 任务树执行管理类
 */
class CDMCoreAgent : public CAgent {

public:
    CDMCoreAgent(const std::string& name, const std::string& configuration = "", const std::string& type = "CAgent:CDMCoreAgent");
    virtual ~CDMCoreAgent();
    virtual void OnDestruction();

    static CAgent* AgentFactory(const std::string& name, const std::string& configuration);
    static CDMCoreAgent* Instance();

    virtual void Reset();
    
public:
    /**
     * @brief 初始化函数
     */
    virtual void Initialize();
    /**
     * @brief 初始化对话状态
     */
    bool InitDialogState(const std::string& session, const std::string& telephone);
    /**
     * @brief 系列化类成员成可保存的字符串数据
     */
    bool DeclareSaveData(std::map<std::string, std::string>& agents);
    /**
     * @brief 反序列化类成员
     */
    bool Load(const std::map<std::string, std::string>& agents);
    /**
     * @brief 设置改管理类的 session 值
     */
    void SetSession(const std::string& session);
    /**
     * @brief 获取改管理类的 session 值
     */
    std::string GetSession();
    /**
     * @brief 当前 session 对应的手机号码
     */
    std::string GetTelephone();
    /**
     * @brief 获取任务流程树的根节点
     */
    CDialogAgent* GetTaskRoot();
    /**
     * @brief 获取当前 session 的数据 mobile session
     */
    ChinaMobileProvince::ChinaMobileSession*    GetMobileSession() { return _mobile_session;}
    /**
     * @brief 获取输出管理类
     */
    COutputManagerAgent* GetOutputManagerAgent();
    /**
     * @brief 获取注册 agent 类
     */
    CRegistryAgent* GetRegistryAgent();
    /**
     * @brief 获取系统异常处理任务流程的根节点
     */
    CDialogAgent* GetSystemExpectDialog();
    /**
     * @brief 获取从 csf 数据到 concept 的接口类
     */
    CGainConceptApi* GetGainConceptApi();
    /**
     * @brief 获取执行类
     */
    CExecuteApi* GetExecuteApi();
    /**
     * @brief 当前用户输入是否触发新的业务流
     */
    bool IsTriggerNewBusiness();
    /**
     * @brief 持久化 session 信息
     */
    bool SaveSession();
    /**
     * @brief 加载 session
     */
    bool LoadSession(const std::string& inter_idx);
    /**
     * @brief 获取任务流程树的根节点名称
     */
    std::string GenRootName(const std::string& session);

public:
    /**
     * @brief 执行任务流程树
     * @param [out] 执行过程中任务节点的输出信息
     */
    TDialogExecuteReturnCode Execute(std::vector<COutput>& outputs);
    /**
     * @brief 接受用户输入信息并处理
     */
    void AcquireNextEvent(const CInteractionEvent& event);
    /**
     * @brief 注册槽值过滤函数
     */
    void RegisterBindingFilter(const std::string& name, TBindingFilterFunct bff_filter);
    /**
     * @brief 获取 session id
     */
    int GetIntSessionID();
    /**
     * @brief 设置用户反馈等待超时时间
     */
    void SetTimeoutPeriod(int time);
    /**
     * @brief 获取用户反馈等待超时时间
     */
    int GetTimeoutPeriod();
    /**
     * @brief 设置默认用户反馈等待超时时间
     */
    void SetDefaultTimeoutPeriod(int value);
    /**
     * @brief 获取默认用户反馈等待超时时间
     */
    int GetDefaultTimeoutPeriod();
    /**
     * @brief 设置拒识的可信度阈值
     */
    void SetNonunderstandingThreshold(float value);
    /**
     * @brief 获取拒识的可信度阈值
     */
    float GetNonunderstandingThreshold();
     /**
     * @brief 设置默认拒识的可信度阈值
     */
    void SetDefaultNonunderstandingThreshold(float value);
    /**
     * @brief 获取默认拒识的可信度阈值
     */
    float GetDefaultNonunderstandingThreshold();
    /**
     * @brief 标注当前状态是否可触发新的任务流程
     */
    void SignalFocusClaimsPhase(bool value = true);
    /**
     * @brief 使用枚举值设置状态值
     */
    void SetFloorStatus(TFloorStatus value);
    /**
     * @brief 获取枚举值状态值
     */
    TFloorStatus GetFloorStatus();
    /**
     * @brief 使用字符串设置状态值
     */
    void SetFloorStatus(const std::string& value);
    /**
     * @brief 枚举状态值到字符串的转换
     */
    std::string FloorStatusToString(TFloorStatus floor);
    /**
     * @brief 字符串到枚举状态值的转换
     */
    TFloorStatus StringToFloorStatus(const std::string& floot);
    /**
     * @brief 最后一次会话的 concept 绑定序号
     */
    int LastTurnGetConceptsBound();
    /**
     * @brief 最后一次会话是否被拒识
     */
    bool LastTurnNonUnderstanding();
    /**
     * @brief 会话连续拒识次数
     */
    int GetNumberNonUnderstandings();
    /**
     * @brief 会话被拒识的总次数
     */
    int GetTotalNumberNonUnderstandings();
    /**
     * @brief 将 agent 压入执行栈
     * @param [in] pusher 使 agent 入栈的主导着
     * @param [in] agent 被压入栈者
     */
    void ContinueWith(CAgent* pusher, CDialogAgent* agent);
    /**
     * @brief 重置 agent，将其压入栈顶
     */
    void RestartTopic(CDialogAgent* agent);
    /**
     * @brief 注册用户指定义的会话重启执行函数
     */
    void RegisterCustomStartOver(TCustomStartOverFunct funct);
    /**
     * @brief 重启会话
     */
    void StartOver();
    /**
     * @brief 获取距离栈顶最近的可执行的agent
     */
    CDialogAgent* GetAgentInFocus();
    /**
     * @brief 获取距离栈顶最近的会话类的agent
     */
    CDialogAgent* GetDTSAgentInFocus();
    /**
     * @brief 判断 agent 是否是距离栈顶最近的可执行的
     */
    bool AgentIsInFocus(CDialogAgent* agent);
    /**
     * @brief 获取执行栈里距离 agent 最近的可执行的 CDialogAgent
     */
    CDialogAgent* GetAgentPreviouslyInFocus(CDialogAgent* agent = NULL);
    /**
     * @brief 获取执行栈里距离 agent 最近的可执行的会话类的 CDialogAgent
     */
    CDialogAgent* GetDTSAgentPreviouslyInFocus(CDialogAgent* agent = NULL);
    /**
     * @brief 获取执行栈里距离栈顶近的主话题 CDialogAgent
     */
    CDialogAgent* GetCurrentMainTopicAgent();
    /**
     * @brief 获取执行栈里距离栈顶近的业务根节点
     */
    CDialogAgent* GetCurrentBusinessAgent();
    /**
     * @brief 判断 agent 是否在执行栈里
     */
    bool AgentIsActive(CDialogAgent* agent);
    /**
     * @brief 将 agent 从执行栈里移出
     */
    void PopAgentFromExecutionStack(CDialogAgent* agent);
    /**
     * @brief 将 agent 从执行栈里移出
     * @note 会将 agent 主导入执行栈的节点一并从执行栈里移出
     */
    void PopTopicFromExecutionStack(CDialogAgent* agent);
    /**
     * @brief 将基础类 Agent 移出执行栈
     */
    void PopGroundingAgentsFromExecutionStack();
    /**
     * @brief 历史会话绑定大小
     */
    int GetBindingHistorySize();
    /**
     * @brief 从历史会话绑定中获取第 index 位的 TBindingsDescr
     */
    const TBindingsDescr& GetBindingResult(int index);
    /**
     * @brief 会话轮数
     */
    int GetLastInputTurnNumber();
    /**
     * @brief 获取 concept 的系统状态
     */
    TSystemActionOnConcept GetSystemActionOnConcept(CConcept* concept);
    /**
     * @brief concept 的显示确认
     */
    void SignalExplicitConfirmOnConcept(CConcept* concept);
    /**
     * @brief concept 的隐式确认
     */
    void SignalImplicitConfirmOnConcept(CConcept* concept);
    /**
     * @brief 不需要隐式确认的 concept
     */
    void SignalUnplannedImplicitConfirmOnConcept(int state, CConcept* concept);
    /**
     * @brief 从显示确认列表中移除 concept
     */
    void SignalAcceptOnConcept(CConcept* concept);
    /**
     * @brief 移除执行栈里除根节点外地所有 Agent
     */
    void ResetExecutionStack();

private:
    // TDialogExecuteReturnCode ActualExecute(std::vector<COutput>& outputs);
    int PopCompletedFromExecutionStack();
    void PopAgentFromExecutionStack(CDialogAgent* agent, std::vector<std::string>& agents_eliminated);
    void PopTopicFromExecutionStack(CDialogAgent* agent, std::vector<std::string>& agents_eliminated);
    void PopGroundingAgentsFromExecutionStack(std::vector<std::string>& agents_eliminated);

    void ClearCurrentSystemAction();
    void ComputeCurrentSystemAction();
    std::string SystemActionToString(const TSystemAction& action);
    std::string CurrentSystemActionToString();

    void AssembleExpectationAgenda();
    void AssembleExpectationAgenda(CDialogAgent* agent);
    void CompileExpectationAgenda();
    void CompileExpectationAgenda(CDialogAgent* agent);
    void EnforceBindingPolicies();
    void BroadcastExpectationAgenda();
    std::string ExpectationAgendaToString();
    std::string ExpectationAgendaToBroadcastString();
    std::string ExpectationAgendaToBroadcastString(const TExpectationAgenda& agenda);
    
    std::string BindingsDescrToString(TBindingsDescr& bindings);
    void UpdateInputLineConfiguration();
    int AssembleFocusClaims();
    void BindConcepts(const std::string& dman, TBindingsDescr& bindings, const CInteractionEvent& event);

    void PerformConceptBinding(const std::string& name, const std::string& value, float confidence, int index, const std::vector<int>& indices, bool is_complete);

    void PerformCustomConceptBinding(int index);
    void PerformForcedConceptUpdates(TBindingsDescr& bindings);
    void ProcessNonUnderstanding();
    void ResolveFocusShift();
    void RollBackDialogState(int state);
    bool DestroyDialogTree();

public:
    static std::vector<std::string> s_floor_status_lables;

private:
    friend class CGroundingManagerAgent;
    friend class CStateManagerAgent;

private:
    /**
     * @brief 任务流程根节点
     */
    CDialogAgent*                               _root;
    /**
     * @brief 输出管理类对象
     */
    COutputManagerAgent*                        _output_manager_agent;
    /**
     * @brief DialogTask 管理类对象
     */
    CDTTManagerAgent*                           _dtt_manager_agent;
    /**
     * @brief agent 注册管理类对象
     */
    CRegistryAgent*                             _registry_agent;
    /**
     * @brief 手机用户 session 信息管理类对象
     */
    ChinaMobileProvince::ChinaMobileSession*    _mobile_session;
    /**
     * @brief 从 csf 获取 concept 类对象
     */
    CGainConceptApi*                            _gain_concept_api;
    /**
     * @brief 调用 csf 执行类对象
     */
    CExecuteApi*                                _execute_api;

private:
    // 需要持久化
    /**
     * @brief 用户反馈等待超时时间
     */
    int                 _timeout_period;
    /**
     * @brief 用户交互轮数
     */
    int                 _turn_number;
    /**
     * @brief 标志是否可触发新流程的标志位
     */
    bool                _is_focus_claims_phase_flag;
    /**
     * @brief 执行栈中的 agent 是否已经变化
     */
    bool                _is_agenda_modified_flag;
    /**
     * @brief 是否需要更改 Expection 数据
     */
    bool                _is_need_update_state;
    /**
     * @brief 是否有新业务被触发
     */
    bool                _is_trigger_new_business;
    /**
     * @brief 拒识可信度阈值
     */
    float               _nonunderstanding_threshold;
    /**
     * @brief 手机号码
     */
    std::string         _telephone;
    /**
     * @brief 根节点名称
     */
    std::string         _root_name;
    /**
     * @brief 执行栈
     */
    TExecutionStack     _execution_stack;
    /**
     * @brief 执行历史
     */
    CExecutionHistory   _execution_history;
    /**
     * @brief 用户槽位和系统 concept 绑定信息
     */
    TBindingHistory     _binding_history;
    /**
     * @brief 分系统类存储 concept
     */
    TSystemAction       _system_action;
    /**
     * @brief 执行状态枚举值
     */
    TFloorStatus        _floor_status;

    // 不需持久化
    /**
     * @brief session id
     * @note 每次请求由客户端传入
     */
    std::string         _session;
    /**
     * @brief 期望 concept 集合
     * @note 每次接收事件时生成
     */
    TExpectationAgenda  _agenda;
    /**
     * @brief 可触发的 agent
     */
    std::vector<TFocusClaim> _focus_claims;

    /**
     * @brief 默认等待用户反馈的超时时间
     */
    int     _default_timeout_period;
    /**
     * @brief 默认拒识可信度阈值
     */
    float   _default_nonunderstanding_threshold;
    /**
     * @brief 重启会话时调用的用户自定义函数
     */
    TCustomStartOverFunct                       _start_over_funct;
    /**
     * @brief 用户槽位到 concept 转换时可能的调用函数
     */
    std::map<std::string, TBindingFilterFunct>  _str2bff_filters;

    static CDMCoreAgent* s_instance;
};

#define SYSTEM_EXPECT_DIALOG( SystemExpect ) \
    CDialogAgent* CDMCoreAgent::GetSystemExpectDialog() {\
        std::string agent_name = "/" + _root_name + "/" + #SystemExpect;\
        if ( _registry_agent != NULL ) {\
            return (CDialogAgent*)_registry_agent->operator[](agent_name);\
        } else {\
            VLOG(WARNING) << __FUNCTION__ << ", _registry_agent is NULL, AgentName: " << _name;\
        }\
        return NULL;\
    }\


}

#endif
