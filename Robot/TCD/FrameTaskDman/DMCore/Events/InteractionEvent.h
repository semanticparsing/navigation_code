// Author : lzy
// Email  : liuzhengyong@trio.ai
// Date   : 2017-09-08 10:13
// Description: 

#ifndef _INTERACTION_EVENT_H__
#define _INTERACTION_EVENT_H__

#include "Robot/TCD/FrameTaskDman/Utils/Common.h"

namespace TrioTDM {

/**
 * @biref NLU 解析后的结果信息
 */
class CInteractionEvent {

public:
    CInteractionEvent();
    CInteractionEvent(const std::string& type);

    virtual ~CInteractionEvent();

public:
    /**
     * @brief 返回事件类型
     */
    std::string GetType() const;
    /**
     * @brief 返回事件 ID
     */
    int GetID() const;
    /**
     * @brief 设置完成状态
     */
    void SetComplete(bool flag);
    /**
     * @brief 返回是否完成
     */
    bool IsComplete() const;
    /**
     * @brief 返回可信度
     */
    float GetConfidence() const;
    /**
     * @brief 返回槽位对应的字符串值
     */
    std::string GetStringProperty(const std::string& slot) const;
    /**
     * @brief 返回相应话题下槽位对应的值
     */
    std::string GetStringProperty(const std::string& dman, const std::string& slot) const;
    /**
     * @brief 返回槽位对应的整形值
     */
    int GetIntProperty(const std::string& slot) const;
    /**
     * @brief 返回相应话题下槽位对应的整形值
     */
    int GetIntProperty(const std::string& dman, const std::string& slot) const;
    /**
     * @brief 返回槽位对应的浮点数值
     */
    float GetFloatProperty(const std::string& slot) const;
    /**
     * @brief 返回相应话题下槽位对应当浮点数值
     */
    float GetFloatProperty(const std::string& dman, const std::string& slot) const;
    /**
     * @brief 判断是否存在槽位值
     */
    bool HasProperty(const std::string& slot) const;
    /**
     * @brief 判断相应话题下是否存在槽位值
     */
    bool HasProperty(const std::string& dman, const std::string& slot) const;
    /**
     * @brief 设置槽位值
     */
    void SetProperty(const std::string& slot, const std::string& value);
    /**
     * @brief 设置相应话题下的槽位值
     */
    void SetProperty(const std::string& dman, const std::string& slot, const std::string& value);
    /**
     * @brief 获取默认的槽位值集合
     */
    std::map<std::string, std::string>& GetProperties();
    /**
     * @brief 类序列化
     */
    std::string ToString() const;
    /**
     * @brief 是否有匹配的 grammar
     */
    bool Matches(const std::string& grammar_expectation) const;
    /**
     * @brief 在相应话题下是否有匹配的 grammar 
     */
    bool Matches(const std::string& dman, const std::string& grammar_expectation) const;
    /**
     * @brief 返回匹配给定 grammar 的槽对应的值
     */
    std::string GetValueForExpectation(const std::string& grammar_expectation) const;
    /**
     * @brief 返回相应话题下匹配给定 grammar 的槽对应的值
     */
    std::string GetValueForExpectation(const std::string& dman, const std::string& grammar_expectation) const;

private:
    /**
     * @brief 模板是否和槽匹配
     */
    bool MatchesSlot(const std::string& slot, const std::string& pattern) const;
    /**
     * @brief properties 是否存在和 grammar 匹配的 key 值
     */
    bool Matches(const std::string& grammar_expectation, const std::map<std::string, std::string>& properties) const;
    /**
     * @brief 返回 properties 和 grammar 匹配的 key 对应的值
     */
    std::string GetValueForExpectation(const std::string& grammar_expectation, const std::map<std::string, std::string>& properties) const;

protected:
    std::string _type; ///< 事件类型
    int _id; ///< 事件 ID
    bool _is_complete; ///< 事件是否处于完成状态
    float _confidence; ///< 事件的可信度
    std::map<std::string, std::string> _properties; ///< 解析结果的默认槽值对
    std::map<std::string, std::map<std::string, std::string> > _dman2properties; ///< 解析结果的相应话题下的槽值对
};

}


#endif
