// Author : lzy
// Email  : liuzhengyong@trio.ai
// Date   : 2017-08-07 17:11
// Description: 会话类节点类

#ifndef _DIALOG_AGENT_H__
#define _DIALOG_AGENT_H__

#include "Robot/TCD/FrameTaskDman/Utils/Common.h"
#include "Robot/TCD/FrameTaskDman/DMCore/Agents/Agent.h"
#include "Robot/TCD/FrameTaskDman/DMCore/Outputs/Output.h"
#include "Robot/TCD/FrameTaskDman/DMCore/Concepts/AllConcepts.h"
#include "Robot/TCD/FrameTaskDman/DMCore/Grounding/Grounding.h"

namespace TrioTDM {

class CDialogAgent;

/**
 * @brief 节点执行的返回值
 */
typedef enum {
    // 继续执行
    dercContinueExecution,
    dercYieldFloor,
    dercTakeFloor,
    // 等待事件
    dercWaitForEvent,
    dercFinishDialog,
    dercFinishDialogAndCloseSession,
    dercRestartDialog,
    dercError,
} TDialogExecuteReturnCode;

/**
 * @brief NLU 解析槽位 到 concep 的赋值方法
 */
typedef enum {
    bmSlotValue,
    bmExplicitValue,
    bmBindingFilter
} TBindMethod;

/**
 * @brief NLU 解析结果对应到 concept 的描述
 */
typedef struct {
	CDialogAgent* pDialogAgent; /*!< 产生该结构的 会话节点 */
	string sConceptName; /*!< 需要赋值的 concept 的名字  */
    std::vector<std::string> vsOtherConceptNames; /*!< 节点隐式需要的其他 concept,
                                                    有时候只需要一个全结构的部分成员 */ 
	string sGrammarExpectation; /*!< 需要的 NLU 解析结果的槽 */
	TBindMethod bmBindMethod; /*!< concept 使用的赋值绑定方法 */
	string sExplicitValue; /*!< 当 bmBindMethod == bmExplicitValue 且
                             NLU 解析符合 sGrammarExpectation 时，concept 取该值 */
    string sBindingFilterName; /*!< 已注册的用户自定义的 NLU 槽值处理函数 */
	bool bDisabled; /*!< 表明该 期望 当前是否被禁用 */
	string sReasonDisabled;	/*!< 并禁用的理由 */
    string sExpectationType; /*!< 期望的类型 */
} TConceptExpectation;

/**
 * @brief 被触发的 会话节点
 */
typedef struct {
    std::string sAgentName;
    bool bClaimDuringGrounding;
} TFocusClaim;

/**
 * @brief 增加子节点的方法
 */
typedef enum {
    asamAsLastChild,
    asamAsFirstChild,
    asamAsLeftSibling,
    asamAsRightSibling,
} TAddSubAgentMethod;

/**
 * @brief 节点完成结果
 */
typedef enum {
    ctSuccess,
    ctFailed,
    ctStartAgain 
} TCompletionType;

/**
 * @brief 节点触发条件值
 */
typedef struct TriggerInfo {
    TriggerInfo() :
        sOperator("=") {
    }
    std::string sValue;
    std::string sOperator;
} TTriggerInfo;

#define WITHIN_TOPIC_ONLY "bind-this-only"
#define MIXED_INITIATIVE "bind-anything"

/**
 * @brief 会话类节点基类
 */
class CDialogAgent : public CAgent {

public:
    CDialogAgent(const std::string& name, const std::string& configuration = "", const std::string& type = "CAgent:CDialogAgent");
    virtual ~CDialogAgent();

    static CAgent* AgentFactory(const std::string& name, const std::string& configuration);

public:
    // CAgent specific methods ( overwritten )
    virtual void Register();
    /**
     * @brief 调用该节点的所有子节点的注册函数
     */
    virtual void RegisterSubAgent();
    virtual void Create();
    /**
     * @brief 创建子节点
     */
    virtual void CreateSubAgents();
    /**
     * @brief 清除触发条件
     */
    virtual void ClearTriggerConcept();
    /**
     * @brief 清除 轮数 大于  round 的 concept
     * @param[in] source 要清楚的 concept 的源，系统的 或者 用户的
     * @param[in] need_retain_concepts 需要保留的 concept 列表
     * @param[in] round 清除的 concept 的对话轮数限定
     */
    virtual void ClearConcepts(TConceptSource source, const std::set<std::string>& need_retain_concepts, int l_round = 1, int r_round = 9999);
    /**
     * @brief 清除当前 agent 包含的符合条件的 concept
     */
    virtual void ClearCurrentConcepts(TConceptSource source, const std::set<std::string>& need_retain_concepts, int l_round = 1, int r_round = 9999);
    virtual void Initialize();
    virtual void Reset();
    /**
     * @brief 恢复业务流触发条件
     */
    virtual void RecoverTriggerConcept();
    virtual void SetOwnerCoreAgent(CDMCoreAgent* core_agent);
    /**
     * @brief 设置 子节点的 CoreAgenti
     */
    virtual void SetSubAgentOwnerCoreAgent(CDMCoreAgent* core_agent);
    virtual CDMCoreAgent* GetOwnerCoreAgent();
    /**
     * @brief 节点入栈是调用该函数
     */
    virtual void IncomingStack();
    /**
     * @brief 执行策略为 START_AGAIN_LEFT_TO_RIGHT 时，返回值为 true 时，重启该任务
     */
    virtual bool IsStartAgain();
    /**
     * @brief 回滚 concept 最后一次的更新
     */
    virtual void RollbackLastUpdateConcept(bool is_recursion = true);
    /**
     * @brief 反序列化该节点及其子节点和包含的 concept
     */
    virtual bool Load(const std::map<std::string, std::string>& agents, const std::map<std::string, std::string>& concepts);
    /**
     * @brief 序列化该节点及其子节点和包含的 concept
     */
    virtual bool DeclareSaveData(std::map<std::string, std::string>& agents, std::map<std::string, std::string>& concepts);
    /*8
     * @brief 序列化类成员变量
     */
    virtual std::string ToJsonString();
    /**
     * @brief 反序列化类成员变量
     */
    virtual bool FromJsonString(const std::string& strg);
    /**
     * @brief 需要持久化的内容改变
     */
    void NotifyChange();
    /**
     * @brief 是否需要持久化该节点跳转到的节点
     */
    virtual bool IsNeedSaveForTaskSkip();
    std::set<std::string> GetMustConcepts() { return _must_concepts; }
    bool GetIsInitialStatus() { return _is_initial_status;}

public:
    /**
     * @brief 中移动返回数据字段
     */
    virtual int Outaction();
    /**
     * @brief 中移动返回数据字段
     */
    virtual int ModelTypeOperator();
    /**
     * @brief 中移动返回数据字段
     */
    virtual int ModelTypeDTMFLength();
    /**
     * @brief 中移动返回数据字段
     */
    virtual int ModelTypeEnd();
    /**
     * @brief 中移动返回数据字段
     */
    virtual TEOutputType OutputType();
    /**
     * @brief 标志系统反馈给用户的信息是否重要，（重听相关）
     */
    virtual bool IsHelpful();
    
public:
    // Fundamental Dialog Agent methods. Most of these are to be overwritten by derived agent classes 
    /**
     * @brief 创建节点关联的 concept
     */
    virtual void CreateConcepts();
    /**
     * @brief 节点是否可执行
     */
    virtual bool IsExecutable();
    /**
     * @brief 从该节点向上查找返回对应的主话题
     */
    virtual CDialogAgent* GetOwnerMainTopic();
    /**
     * @brief 触发条件
     */
    virtual std::map<std::string, TTriggerInfo>& GetTriggerConcepts();
    /**
     * @brief 错误处理方法标识
     */
    virtual std::string GetErrorHandle();
    /**
     * @brief 节点执行函数
     */
    virtual TDialogExecuteReturnCode Execute(COutput* output);
    /**
     * @brief 节点关联的 concept 更新时，调用该函数
     */
    virtual void OnConceptUpdated();
    /**
     * @brief 是否需要生成期望数据
     */
    virtual bool IsNeedDeclareExpectations();
    /**
     * @brief 生成节点期望 concept 数据
     */
    virtual int DeclareExpectations(std::vector<TConceptExpectation>& expections, bool is_must_not_dts_agent = false);
    /**
     * @brief 生成除 exclude 集合中的所有 concept
     */
    virtual void DeclareConcepts(std::vector<CConcept*>& concepts, std::set<CConcept*>& exclude);
    /**
     * @brief 创建节点 concept 关联的基础模型类
     */
    virtual void CreateGroundingModel(const std::string& grounding_model_spec);
    /**
     * @brief 返回节点 concept 关联的基础模型类
     */
    virtual CGroundingModel* GetGroundingModel();
    /**
     * @brief 生成除 exclude 外，所有的 concept 基础模型
     */
    virtual void DeclareGroundingModels(std::vector<CGroundingModel*>& concepts, std::set<CGroundingModel*>& exclude);
    /**
     * @brief 是否满足期望条件
     */
    virtual bool ExpectCondition();
    /**
     * @brief 节点关联 concept 绑定策略
     */
    virtual std::string DeclareBindingPolicy();
    /**
     * @brief 生成可触发的节点集合信息
     */
    virtual int DeclareFocusClaims(std::vector<TFocusClaim>& focus_claims);
    /**
     * @brief 会话触发条件满足时调用
     */
    virtual void OnFocusclaims();
    /**
     * @brief 会话节点可执行的前置条件
     */
    virtual bool PreconditionsSatisfied();
    /**
     * @brief 会话是否会被触发
     */
    virtual bool ClaimsFocus();
    /**
     * @brief 会话节点入栈的参考条件
     */
    virtual bool ClaimsFocusDuringGrounding();
    /**
     * @brief 会话是否被槽位触发
     */
    virtual std::string TriggeredByCommands();
    /**
     * @brief 创建触发槽位
     */
    void CreateTriggerConcept();
    /**
     * @brief 节点成功执行的条件
     */
    virtual bool SuccessCriteriaSatisfied();
    /**
     * @brief 节点执行失败的判断条件
     */
    virtual bool FailureCriteriaSatisfied();
    /**
     * @brief 节点最大执行次数限制
     */
    virtual int GetMaxExecuteCounter();
    /**
     * @brief 节点被创建后调用的函数
     * @note 用户可以改写改函数，自定义创建后的一些逻辑
     */
    virtual void OnCreation();
    /**
     * @brief 节点被销毁时调用的函数
     * @note 用户可改写函数，自定义一些销毁工作
     */
    virtual void OnDestruction();
    /**
     * @brief 节点初始化时调用的函数
     * @note 用户可改写函数，自定义节点初始化逻辑
     */
    virtual void OnInitialization();
    /**
     * @brief 节点完成后调用的函数
     * @note 用户可改写函数，自定义完成后的一些执行逻辑
     */
    virtual void OnCompletion();
    /**
     * @brief 节点执行失败后调用的函数
     * @note 用户可改写函数，自定义失败处理逻辑，默认失败后触发系统异常业务流
     */
    virtual void OnExecuteFail();
    /**
     * @brief 节点重置，保留历史 concept 数据
     * @note ::CDialogAgent::Reset 会清空历史 concept 数据
     */
    virtual void ReOpen();
    /**
     * @brief 节点管理的 concept ，数据重置
     */
    virtual void ReOpenConcepts();
    /**
     * @brief 重启话题
     */
    virtual void ReOpenTopic();
    /**
     * @brief 节点是否是业务流根节点
     */
    virtual bool IsABusiness();
    /**
     * @brief 节点是否话题根节点
     */
    virtual bool IsAMainTopic();
    /**
     * @brief 节点输出信息
     */
    virtual std::string Prompt();
    /**
     * @brief 超时输出
     */
    virtual std::string TimeoutPrompt();
    /**
     * @brief 详情信息输出
     */
    virtual std::string ExplainMorePrompt();
    /**
     * @brief 版本信息输出
     */
    virtual std::string CreateVersionedPrompt(const std::string& version);
    /**
     * @brief 上文信息输出
     */
    virtual std::string EstablishContextPrompt();
    /**
     * @brief 用户可回答信息提示
     */
    virtual std::string WhatCanISayPrompt();
    /**
     * @brief 配置信息
     */
    virtual std::string InputLineConfigurationInitString();
    /**
     * @brief 节点是否是会话式任务节点
     */
    virtual bool IsDTSAgent();
    virtual bool IsConversationSynchronous();
    virtual bool RequiresFloor();
    virtual void Undo();

public:
    /**
     * @brief 根据 concept 的带路径名取 concept
     */
    virtual CConcept& C(const std::string& concept_path);
    /**
     * @brief 根据 concept 的带路径名取 concept
     */
    virtual CConcept& C(const char* concept_path, ...);
    /**
     * @brief 根据 concept 的名子取 concept
     */
    virtual CConcept& LocalC(const std::string& concept_name);
    /**
     * @brief 根据节点的路径名取节点
     */
    CDialogAgent& A(const std::string& dialog_agent_path);
    /**
     * @brief 根据节点的路径名取节点
     */
    CDialogAgent& A(const char* dialog_agent_path, ...);
    /**
     * @brief 增加子节点
     */
    void AddSubAgent(CDialogAgent* da_who, CDialogAgent* da_where, TAddSubAgentMethod asam_method);
    /**
     * @brief 删除子节点
     */
    void DeleteSubAgent(CDialogAgent* da_who);
    /**
     * @brief 删除动态子节点
     */
    void DeleteDynamicSubAgents();
    /**
     * @brief 设置父节点
     */
    void SetParent(CDialogAgent* da_parent);
    /**
     * @brief 获取父节点
     */
    CDialogAgent* GetParent();
    /**
     * @brief 更新节点路径名
     * @note 父节点变更路径名变更
     */
    void UpdateName();
    /**
     * @brief 从配置文件中更新数据
     */
    void UpdateConfig();
    /**
     * @brief 设置上下文 节点
     */
    void SetContextAgent(CDialogAgent* da_context_agent);
    /**
     * @brief 获取上下文 节点
     */
    CDialogAgent* GetContextAgent();
    /**
     * @brief 获取当前节点所属的话题根节点
     */
    CDialogAgent* GetMainTopic();
    /**
     * @brief 获取当前节点所属的业务流根节点
     */
    CDialogAgent* GetBusiness();
    /**
     * @biref 获取节点名，不带路径
     */
    std::string GetDialogAgentName();
    /**
     * @brief 节点是否完成
     */
    bool HasCompleted();
    /**
     * @brief 设置节点完成类型
     */
    void SetCompleted(TCompletionType completion_type = ctSuccess);
    /**
     * @brief 重置节点完成情况
     */
    void ResetCompleted();
    /**
     * @brief 节点是否失败
     */
    bool HasFailed();
    /**
     * @brief 节点是否成功
     */
    bool HasSucceeded();
    /**
     * @brief 节点所在树是否阻塞
     */
    bool IsAgentPathBlocked();
    /**
     * @brief 节点是否阻塞
     */
    virtual bool IsBlocked();
    /**
     * @brief 阻塞以该节点为根的树
     */
    void Block();
    /**
     * @brief 解阻塞以该节点为根的树
     */
    void UnBlock();
    /**
     * @brief 设置动态节点标识为 true
     */
    void SetDynamicAgent();
    /**
     * @brief 设置节点名
     */
    void SetName(const std::string& name);
    /**
     * @brief 是否是动态节点
     */
    bool IsDynamicAgent();
    /**
     * @brief 设置动态节点 ID
     */
    void SetDynamicAgentID(const std::string& dynamic_agent_id);
    /**
     * @brief 获取动态节点 ID
     */
    std::string GetDynamicAgentID();
    /**
     * @brief 节点执行计数加加
     */
    void IncrementExecuteCounter();
    /**
     * @brief 获取节点执行计数
     */
    int GetExecuteCounter();
    /**
     * @brief 节点是否被重置过
     */
    bool WasReset();
    /**
     * @brief 节点是否被重启过
     */
    bool WasReOpened();
    /**
     * @brief 节点交互轮数加加
     */
    void IncrementTurnsInFocusCounter();
    /**
     * @brief 获取节点交互轮数
     */
    int GetTurnsInFocusCounter();
    /**
     * @brief 设置最后交互索引
     */
    void SetLastInputIndex(int input_index);
    /**
     * @brief 获取最后交互索引
     */
    int GetLastInputInex();
    /**
     * @brief 设置最后执行索引
     */
    void SetLastExecutionIndex(int index);
    /**
     * @brief 获取最后执行索引
     */
    int GetLastExecutionIndex();
    /**
     * @brief 设置最后绑定索引
     */
    void SetLastBindingsIndex(int index);
    /**
     * @brief 获取最后绑定索引
     */
    int GetLastBindingsIndex();
    /**
     * @brief 使用字符串更新配置信息
     */
    void SetInputConfiguration(const std::string& configuration);
    /**
     * @brief 使用槽位值更新配置信息
     */
    void SetInputConfigurationParameter(const std::string& slot, const std::string& value);
    /**
     * @brief 配置是否有改槽位
     */
    bool HasInputConfigurationParameter(const std::string& slot);
    /**
     * @brief 获取槽位对应的值
     */
    std::string GetInputConfigurationParameterValue(const std::string& slot);
    /**
     * @brief 使用父节点的配置更新当前节点配置
     */
    void InheritParentInputConfiguration();
    /**
     * @brief 获取当前节点配置信息
     */
    std::map<std::string, std::string> GetInputLineConfiguration();

protected:
    /**
     * @brief 生成 concept 期望信息
     * @param[in] concept_name 期望的 concept 名字
     * @param[in] grammar_mapping 配置的槽位映射
     * @param[out] expectations concept 期望信息
     */
    void ParseGrammarMapping(const std::string& concept_name, const std::string& grammar_mapping, std::vector<TConceptExpectation>& expectations);
    /**
     * @brief NLG 生成使用的变量
     */
    virtual CConcept& NLGConceptParam();
    /**
     * @brief 填充业务流触发条件
     */
    void FillBusinessTriggerConcept();

public:
    /**
     * @brief 流程执行错误时，NLG 生成错误信息需要的 concept
     */
    virtual CConcept& NLGConceptErrorParam();
    /**
     * @brief 帮助信息输出是，NLG 需要的 concept
     */
    virtual CConcept& NLGConceptHelpParam();

public:
    static CDialogAgent NULLDialogAgent;

protected:
    // 需要持久化的变量
    
    // 调用成员变量自己的持久化函数
    std::vector<CConcept*>      _concepts; ///< 节点关联的 concept
    std::vector<CDialogAgent*>  _sub_agents; ///< 节点子节点列表

    // 自己持久化
    int _last_persistence_count;
    int _execute_counter; ///< 执行计数
    int _reset_counter; ///< 重置次数
    int _re_open_counter; ///< 重启次数
    int _turns_in_focus_counter; ///< 触发次数
    int _last_input_index; ///< 最后一次输入索引
    int _last_execution_input_index; ///< 最后一次执行输入索引
    int _last_excution_index; ///< 最后一次执行索引
    int _last_bindings_index; ///< 最后一次绑定索引
    bool _is_completed; ///< 完成与否标识
    bool _is_blocked; ///< 阻塞与否标识
    // add to tree at runtime
    bool _is_dynamic_agent; ///< 是否是动态节点
    bool _is_inherited_parent_input_configuration; ///< 是否有继承父节点配置信息
    bool _is_create_sub_agent; ///< 是否已经创建子节点
    bool _is_create_skip_task; ///< 是否创建了跳转节点
    bool _is_initial_status; ///< 是否是初始化状态
    std::string _dynamic_agent_id; ///< 动态 ID
    TCompletionType _completion_type; ///< 节点完成类型

    // 持久化内容是否有更新
    bool _is_updated; ///< 在最近一次持久化后节点数据是否有更新

    // 不需要持久化的变量，建树时自动生成，且不随执行状态变化
    CDialogAgent*       _da_parent; ///< 指向父节点
    CDialogAgent*       _da_context_agent; ///< 指向上文节点
    CGroundingModel*    _grounding_model; ///< 模型节点
    std::string         _dialog_agent_name; ///< 节点名，不带路径
    std::string         _triggered_by_commands; ///< 触发命令
    std::string         _trigger_commands_grounding_model_spec; ///< 触发命令规格

    std::map<std::string, std::string> _input_line_configuration; ///< 配置信息
    std::map<std::string, TTriggerInfo> _trigger_concepts; ///< 槽位值触发信息
    std::set<std::string> _need_concepts; ///< 需要保留的槽位列表
    std::set<std::string> _must_concepts; ///< 必须保留的槽位列表
};

/**
 * @brief 所有宏定义参照函数
 */
#define PRECONDITION(condition) \
    public:\
        virtual bool PreconditionsSatisfied() {\
            return (condition);\
        }\

#define CAN_TRIGGER_DURING_GROUNDING \
    public:\
        virtual bool ClaimsFocusDuringGrounding() {\
            return true;\
        }\

#define SUCCEEDS_WHEN(condition) \
    public:\
        virtual bool SuccessCriteriaSatisfied() {\
            return (condition); \
        }\

#define FAILS_WHEN(condition) \
    public:\
        virtual bool FailureCriteriaSatisfied() {\
            return (condition);\
        }\

#define MAX_ATTEMPTS(count) \
    public:\
        virtual int GetMaxExecuteCounter() {\
            return (count);\
        }\

#define CONCEPT_BINDING_POLICY(policy) \
    public:\
        virtual std::string DeclareBindingPolicy() {\
            return (policy);\
        }\

#define EXPECT_WHEN(condition) \
    public:\
        virtual bool ExpectCondition() {\
            return (condition);\
        }\

#define ON_CREATION(DO_STUFF) \
    public:\
        virtual void OnCreation() {\
            DO_STUFF;\
        }\

#define ON_DESTRUCTION(DO_STUFF)\
	public:\
	    virtual void OnDestruction() {\
		    DO_STUFF;\
    	}\

// D: macro for specifying initialization code for an agent
#define ON_INITIALIZATION(DO_STUFF)\
	public:\
	    virtual void OnInitialization() {\
		    DO_STUFF;\
    	}\

// D: macro for defining an "effect" for the agent, through the OnCompletion
//    virtual function
#define ON_COMPLETION(DO_STUFF)\
	public:\
	    virtual void OnCompletion() {\
            DO_STUFF;\
    	}\

#define ON_EXECUTE_FAIL( DO_STUFF ) \
    public:\
        virtual void OnExecuteFail() {\
            DO_STUFF;\
        }\

// D: macro for defining reopening behavior for a dialog agent
#define ON_REOPEN(DO_STUFF)\
	public:\
	    virtual void ReOpen() {\
		    DO_STUFF;\
    	}\


// D: macro to be used to reset an agent on completion
#define RESET {Reset();}
// #define CLEAR_TRIGGER_CONCEPT { ClearTriggerConcept(); }
#define CLEAR_TRIGGER_CONCEPT { ; }
#define BLOCK {Block();}
#define REOPEN {ReOpen();}

// D: macro for defining the conditions under which the agent claims focus
#define TRIGGERED_BY(Condition)\
	public:\
        virtual bool ClaimsFocus() {\
		    return (Condition);\
    	}\

// D: macro for definiting the user commands which trigger this agent
#define TRIGGERED_BY_COMMANDS(Commands, GroundingModelSpec)\
	public:\
	virtual string TriggeredByCommands() {\
        _triggered_by_commands = Commands;\
        _trigger_commands_grounding_model_spec = GroundingModelSpec;\
        return (Commands);\
	}\

// A: macro for defining agents which require the floor to be executed
#define REQUIRES_FLOOR()\
	public:\
	virtual bool RequiresFloor() {\
		return true;\
	}\
	
// A: macro for defining agents which do not require the floor in order to be executed
#define DOES_NOT_REQUIRE_FLOOR()\
	public:\
	virtual bool RequiresFloor() {\
		return false;\
	}\
	
#define IS_BUSINESS() \
    public:\
        virtual bool IsABusiness() {\
            return true;\
        }\

// D: macro for defining agents which are main topics
#define IS_MAIN_TOPIC()\
	public:\
	virtual bool IsAMainTopic() {\
		return true;\
	}\

	
// D: macro for defining agents which are main topics
#define IS_NOT_DTS_AGENT()\
	public:\
	virtual bool IsDTSAgent() {\
		return false;\
	}\

// D: macro for defining the prompt
#define PROMPT(PROMPT)\
	public:\
	virtual string Prompt() {\
		return (PROMPT);\
	}\

// D: macro for defining the timeout prompt
#define PROMPT_TIMEOUT(PROMPT)\
	public:\
	virtual string TimeoutPrompt() {\
		return (PROMPT);\
	}\

// D: macro for defining the explain more prompt
#define PROMPT_EXPLAINMORE(PROMPT)\
	public:\
	virtual string ExplainMorePrompt() {\
		return (PROMPT);\
	}\

// D: macro for defining the establish context prompt
#define PROMPT_ESTABLISHCONTEXT(PROMPT)\
	public:\
	virtual string EstablishContextPrompt() {\
		return (PROMPT);\
	}\

// D: macro for defining the what can i say prompt
#define PROMPT_WHATCANISAY(PROMPT)\
	public:\
	virtual string WhatCanISayPrompt() {\
		return (PROMPT);\
	}\

// AR: macro for defining the language model required by the agent
#define INPUT_LINE_CONFIGURATION(CONFIG_LINE)\
	public:\
	virtual string InputLineConfigurationInitString() {\
		return (CONFIG_LINE);\
	}\

// A: macro for defining agents which cannot be executed by anticipation
#define IS_CONVERSATION_SYNCHRONOUS()\
	public:\
	virtual bool IsConversationSynchronous() {\
		return true;\
	}\

#define NEED_DECLARE_EXPECTATIONS() \
    public:\
    virtual bool IsNeedDeclareExpectations () {\
        return true;\
    }\

#define ERROR_HANDLE( Handle ) \
    public:\
        virtual std::string GetErrorHandle() {\
            return Handle;\
        }\

#define NLG_CONCEPT_PARAM( Param )\
    public:\
        virtual CConcept& NLGConceptParam() {\
            return C(#Param);\
        }\

#define NLG_CONCEPT_ERROR_PARAM( Param )\
    public:\
        virtual CConcept& NLGConceptErrorParam() {\
            return C(#Param);\
        }\

#define NLG_CONCEPT_HELP_PARAM( Param )\
    public:\
        virtual CConcept& NLGConceptHelpParam() {\
            return C(#Param);\
        }\

#define OUTACTION( Action ) \
    public:\
        int Outaction() {\
            return Action;\
        }\

#define MODEL_TYPE_OPERATOR( Operator ) \
    public:\
        int ModelTypeOperator() {\
            return Operator;\
        }\

#define MODEL_TYPE_DTMF_LENGTH( Length ) \
    public:\
        int ModelTypeDTMFLength() {\
            return Length;\
        }\

#define MODEL_TYPE_END( End ) \
    public:\
        int ModelTypeEnd() {\
            return End;\
        }\

#define OUTPUT_TYPE( Type ) \
    public:\
        TEOutputType OutputType() {\
            return Type;\
        }\

#define IS_HELPFUL( Flag ) \
    public:\
        bool IsHelpful() {\
            return Flag;\
        }\

#define ON_CONCEPT_UPDATED( DO_STUFF ) \
    public:\
        virtual void OnConceptUpdated() {\
            VLOG(DEBUG) << __FUNCTION__ << ", Name: " << _name;\
            DO_STUFF;\
        }\


#define OUTPUT_SET_INPUT \
    do {\
        output->SetIsOutput(true);\
        output->SetOutaction( Outaction() );\
        output->SetModelTypeOperator( ModelTypeOperator() );\
        output->SetModelTypeDTMFLength( ModelTypeDTMFLength() );\
        output->SetModelTypeEnd( ModelTypeEnd() );\
        output->SetOutputType( OutputType() );\
        output->SetIsHelpful( IsHelpful() );\
        output->SetTelephone( _owner_core_agent->GetTelephone() );\
        output->SetGenAgentName(prompt);\
    } while(0)\

#define DO_CREATE_SUB_AGENTS do{\
    CreateSubAgents();\
    SetSubAgentOwnerCoreAgent( _owner_core_agent );\
    RegisterSubAgent();\
}while(0)\

#define FILL_TRIGGER_CONCEPT \
    {FillBusinessTriggerConcept();}

}

#endif
