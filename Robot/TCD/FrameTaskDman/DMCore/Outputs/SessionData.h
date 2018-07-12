// Author : lzy
// Email  : liuzhengyong@trio.ai
// Date   : 2017-10-25 14:33
// Description: 

#ifndef _SESSION_DATA_H__
#define _SESSION_DATA_H__

namespace TrioTDM {

class CSessionData {
public:
    static int s_outaction_interactive;
    // 结束
    static int s_outaction_end;
    // 跳转
    static int s_outaction_skip;

    // session 初始化
    static int s_inaction_begin;
    // 返回语音播报及其他结果
    static int s_inaction_call_ret;
    // 呼叫转移结果
    static int s_inaction_transfer_ret;
    // 挂机
    static std::string s_input_hangup;
    // 系统异常
    static std::string s_input_sys_err;
    // 播放完成
    static std::string s_input_playover;
};

}

#endif

