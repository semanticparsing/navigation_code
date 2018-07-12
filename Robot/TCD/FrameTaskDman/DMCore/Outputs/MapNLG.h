// Author : lzy
// Email  : liuzhengyong@trio.ai
// Date   : 2017-09-06 15:54
// Description: 关键字类型自然语言生成 

#ifndef _MAP_NLG_H__
#define _MAP_NLG_H__

#include "Robot/TCD/FrameTaskDman/Utils/Common.h"

namespace TrioTDM {

/**
 * @brief 关键字类型自然语言生成
 */
class CMapNLG {

public:
    CMapNLG();
    ~CMapNLG();
    /**
     * @brief 通过关键字生成自然语言
     */
    bool Generate(const std::string& key, std::string& result) const;
    /**
     * @brief 加载关键字 到 自然语言 的映射文件
     */
    bool Load(const std::string& dit_file);

private:
    std::map<std::string, std::string> _nlg_map; ///< 关键字到自然语言的映射
};

}

#endif
