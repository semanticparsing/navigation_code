// Author : lzy
// Email  : liuzhengyong@trio.ai
// Date   : 2017-09-08 10:06
// Description: 事件管理类 

#ifndef _INTERACTION_EVENT_MANAGER_AGENT_H__
#define _INTERACTION_EVENT_MANAGER_AGENT_H__

#include <queue>
#include "Robot/TCD/FrameTaskDman/Utils/Common.h"
#include "Robot/TCD/FrameTaskDman/DMCore/Events/InteractionEvent.h"
#include "Robot/TCD/FrameTaskDman/DMCore/Agents/Agent.h"

#define IET_DIALOG_STATE_CHANGE	"dialog_state_change"
#define IET_USER_UTT_START	"user_utterance_start"
#define IET_USER_UTT_END	"user_utterance_end"
#define IET_PARTIAL_USER_UTT "partial_user_utterance"
#define IET_SYSTEM_UTT_START	"system_utterance_start"
#define IET_SYSTEM_UTT_END	"system_utterance_end"
#define IET_SYSTEM_UTT_CANCELED	"system_utterance_canceled"
#define IET_FLOOR_OWNER_CHANGES "floor_owner_changes"
#define IET_SESSION "session"
#define IET_GUI "gui"

namespace TrioTDM {

/**
 * @brief 事件管理类
 */
class CInteractionEventManagerAgent: public CAgent {

private:
  static CInteractionEventManagerAgent* s_instance; ///< 单例对象指针

    std::queue <CInteractionEvent*, list<CInteractionEvent*> > qpieEventQueue;
    std::vector <CInteractionEvent*> vpieEventHistory;
	CInteractionEvent *pieLastEvent;
	CInteractionEvent *pieLastInput;

public:

	CInteractionEventManagerAgent(const std::string& name, 
					   const std::string& configuration = "",
					   const std::string& type = "CAgent:CInteractionEventManagerAgent");
	virtual ~CInteractionEventManagerAgent();

	static CAgent* AgentFactory(const std::string& name, const std::string& configuration);
    static CInteractionEventManagerAgent* Instance();

	virtual void Reset();	// overwritten Reset
	virtual void Initialize();	// overwritten Initialize

public:

    /**
     * @brief 判断队列是否存在数据
     */
	bool HasEvent();
    /**
     * @brief 从队列里取出事件
     * @note 如果队列为空，返回 NULL
     */
	CInteractionEvent *GetNextEvent();
    /**
     * @brief 返回最后的时间
     */
	CInteractionEvent *GetLastEvent();
    /**
     * @brief 返回最后的输入事件
     */
	CInteractionEvent *GetLastInput();
    /**
     * @brief 最后的事件是否和给定的 grammar 匹配
     */
	bool LastEventMatches(const string& sGrammarExpectation);
    /**
     * @brief 最后的输入事件是否和给定的 grammar 匹配
     */
	bool LastInputMatches(const std::string& sGrammarExpectation);
    /**
     * @brief 最后的时间是否是完成状态
     */
    bool LastEventIsComplete();
    /**
     * @brief 返回最后的事件的可信度
     */
    float GetLastEventConfidence();
    /**
     * @brief 返回最后的事件的匹配给定 grammar 的值
     */
    std::string GetValueForExpectation(const std::string& sGrammarExpectation);

	// Waits for an interaction event to arrive from the Interaction Manager
	void WaitForEvent();

	// Used by the Galaxy Bridge to signal that a new event has arrived
	void SignalInteractionEventArrived();

};


}

#endif
