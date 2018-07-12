// Author : lzy
// Email  : liuzhengyong@trio.ai
// Date   : 2017-09-06 16:24
// Description: 自然语言生成 

#ifndef _NLG_H__
#define _NLG_H__

#include "Robot/TCD/FrameTaskDman/Utils/Common.h"
#include "Robot/TCD/FrameTaskDman/DMCore/Concepts/AllConcepts.h"

namespace TrioTDM {

class CMapNLG;
class CTemplateNLG;

/**
 * @brief 自然语言生成类
 */
class CNLG {

public:
    ~CNLG();

    static CNLG* Instance();
    /**
     * @brief 初始化函数
     * @param[in] template_file nlg 模板文件
     * @param[in] dit_file 关键词到自然语言映射文件
     */
    bool Init(const std::string& template_file, const std::string& dit_file = "");
    /**
     * @brief 自然语言生成
     * @param[in] sentence 包含使用哪种自然语言生成方法和 关键词 或 模板词
     * @param[in] info 模板使用的传入参数
     */
    std::string Generate(const std::string& sentence, CConcept& info);

protected:
    CNLG();

private:
    static CNLG* s_instance;
    CMapNLG* _map_nlg; ///< 使用关键词生成自然语言的类对象
    CTemplateNLG* _template_nlg; ///< 使用模板生成自然语言的类对象
};

}


#endif
