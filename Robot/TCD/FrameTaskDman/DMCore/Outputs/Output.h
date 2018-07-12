// Author : lzy
// Email  : liuzhengyong@trio.ai
// Date   : 2017-08-23 09:29
// Description: 输出数据 

#ifndef _OUTPUT_H__
#define _OUTPUT_H__

#include "Robot/TCD/FrameTaskDman/Utils/Utils.h"
#include "Robot/TCD/FrameTaskDman/DMCore/Concepts/Concept.h"

// DEFINE_int32(outaction_interactive, 9, "交互");
// DEFINE_int32(outaction_end, 10, "结束");
// DEFINE_int32(outaction_skip, 11, "跳转");

namespace TrioTDM {

/**
 * @brief 输出信息的类型
 */
typedef enum EOutputType {
    O_UNDEFINE,
    O_ERROR,
    O_HELP,
    O_PROMPT,
    O_INFOR
} TEOutputType;

#define BROADCAST   0x4
#define RECOGNITION 0x2
#define DTMF        0x1

// #
#define END_POUND_SIGN     0x1
// *
#define END_ASTERISK       0x2


/**
 * @brief 输出信息的结构体
 */
class COutput {
    friend class COutputHistory;
    friend class COutputManagerAgent;

public:
    COutput();
    virtual ~COutput();
    /**
     * @brief 序列化
     */
    Json::Value ToJson();
    /**
     * @brief 反序列化
     */
    bool FromJson(const Json::Value& j_data);
    /**
     * @brief 设置模式类型
     */
    void SetModelType(const std::string& model_type);
    /**
     * @brief 返回模式类型
     */
    virtual std::string ModelType() const;
    void SetModelTypeOperator(int model_type_operator);
    void SetModelTypeDTMFLength(int model_type_dtmf_length);
    void SetModelTypeEnd(int model_type_end);
    /**
     * @brief 设置信息用处分类
     */
    void SetOutaction(int outaction);
    /**
     * @brief 获取信息用处分类
     */
    int GetOutaction() const;
    /**
     * @brief 设置输出信息类型，错误，帮助。。
     */
    void SetOutputType(TEOutputType output_type);
    /**
     * @brief 获取输出信息类型
     */
    TEOutputType GetOutputType() const;
    /**
     * @brief 设置信息是否有用
     */
    void SetIsHelpful(bool is_helpful);
    /**
     * @brief 返回信息是否有用
     */
    bool GetIsHelpful() const;
    /**
     * @brief 设置生成信息的节点名
     */
    void SetGenAgentName(const std::string& agent_name);
    /**
     * @brief 获取生成信息的节点名
     */
    std::string GetGenAgentName() const;
    /**
     * @brief 设置手机号
     */
    void SetTelephone(const std::string& telephone);
    /**
     * @brief 获取手机号
     */
    std::string GetTelephone() const;
    /**
     * @brief 设置输出语句
     */
    void SetSendMsg(const std::string& msg);
    /**
     * @brief 获取输出语句
     */
    std::string Msg() const;
    /**
     * @brief 序列化
     */
    virtual std::string ToString();
    virtual COutput* Clone(int output_id);
    /**
     * @brief 返回生成该信息的节点名字
     */
    std::string GetGeneratorAgentName();

    bool IsOutput() { return _is_output;}
    void SetIsOutput(bool flag = true) { _is_output = flag;}

    int GetDialogStateIndex();
    void SetDialogStateIndex(int index);
    std::string GetDialogState();
    void SetDialogState(const std::string& state);

    void SetConveyance(TConveyance conveyance);
    TConveyance GetConveyance();

    void SetAct(const std::string& act);
    std::string GetAct();

    /*
    void SetFinalFloorStatus(TFloorStatus floor);
    TFloorStatus GetFinalFloorStatus();
    */
    std::string GetFinalFloorStatusLabel();

    bool CheckFlag(const std::string& flag);
    void NotifyConceptConveyance(const std::string& concept_name, TConveyance conveyance);
    virtual CConcept* GetConceptByName(const std::string& name);
    void CancelConceptNotificationRequest(CConcept* concept);
    void ChangeConceptNotificationPointer(CConcept* old_concept, CConcept* new_concept);
    int GetRepeatCounter();
    void IncrementRepeatCounter();

protected:
    virtual void Clone(COutput* output, int new_output_id);

protected:
    std::string _generator_agent_name; ///< 生成该信息的会话节点名
    std::string _telephone; ///< 手机号
    std::string _send_msg; ///< 生成的信息语句
    std::string _dialog_state; ///< 会话状态
    std::string _act;
    std::string _object;
    std::string _model_type; ///< 模式类型

    /// _model_type_operator _model_type_dtmf_length  _model_type_end 二进制按顺序拼接，组成 model_type
    int _model_type_operator; ///< 二进制的三位数字，分别代表是否支持  放音、识别、按键,   1代表支持
    int _model_type_dtmf_length; ///< 两位二进制数字，代表支持按键的情况下, 最大按键长度是多少
    int _model_type_end; ///< 两位二进制数字，代表 * # 是否为功能键
    int _outaction; ///< 信息用处分类类型
    int _repeat_counter; ///< 重听计数
    int _output_id; ///< 输出信息 ID
    int _execution_index; ///< 执行索引
    bool _is_output; ///< 是否输出信息
    bool _is_helpful; ///< 是否有用信息，主要用于重听逻辑
    
    TConveyance _conveyance; //< 传输结果
    TEOutputType _output_type; ///< 输出信息的类型
};

}

#endif
