// Author : lzy
// Email  : liuzhengyong@trio.ai
// Date   : 2017-09-14 15:57
// Description: 模板类自然语言生成 

#ifndef _TEMPLATE_NLG_H__
#define _TEMPLATE_NLG_H__

#include "Robot/TCD/FrameTaskDman/Utils/Common.h"
#include "Robot/TCD/FrameTaskDman/DMCore/Concepts/AllConcepts.h"

namespace TrioTDM {

/**
 * @brief 模板信息保存
 */
typedef struct Template {
    Template() : 
        variableCount(0){
        ;
    }
    std::map<std::string, std::string> field2parameter;
    std::vector<std::string> sentences;
    std::string nlgName;
    int variableCount;
    bool is_check_field;
    void Clear();
    bool Empty();
} STemplate;

/**
 * @brief 由模板生成自然语言的类
 */
class CTemplateNLG {

public:
    CTemplateNLG();
    ~CTemplateNLG();
    /**
     * @brief 加载模板类文件
     */
    bool Load(const std::string& nlg_file);
    /**
     * @brief 生成自然语言
     * @param[in] nlg_template 模板标识
     * @param[in] concept 传入模板的参数
     * @param[out] result 生成自然语言
     */
    bool Generate(const std::string& nlg_template, CConcept& concept, std::string& result);

private:
    /**
     * @brief 模板是否和入参匹配
     */
    bool IsMatch(const STemplate& templ, CConcept& concept, std::map<std::string, CConcept&>& parameter2value);
    /**
     * @brief 生成自然语言
     * @param[in] parameter2value 模板中参数对应的值
     * @param[in] templ 模板信息
     * @param[out] result 生成的自然语言
     */
    bool Generate(const std::map<std::string, CConcept&>& parameter2value, const STemplate& templ, std::string& result);

private:
    std::map<std::string, std::vector<STemplate> > _name2template; ///< 模板的标识对应的模板信息
};

}

#endif
