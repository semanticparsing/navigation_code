#include "TemplateNLG.h"
#include <algorithm>
#include "Common/Util/File/simpleLineReader.h"

namespace TrioTDM {
 
static bool SelectRandomSentence(const std::vector<std::string>& sentences, string& sentence) {
    if ( sentences.empty() ) {
        VLOG(WARNING) << "sentences is empty.";
        return false;
    }
    srand((unsigned)time(NULL));
    int random_num=rand()%(sentences.size());
    sentence = sentences.at(random_num);
    return true;
}

static bool TemplateSort(const STemplate& a, const STemplate& b) {
    return a.variableCount < b.variableCount;
}

void STemplate::Clear() {
    field2parameter.clear();
    sentences.clear();
    variableCount = 0;
}

bool STemplate::Empty() {
    return sentences.empty() || nlgName.empty();
}

CTemplateNLG::CTemplateNLG() {
    ;
}

CTemplateNLG::~CTemplateNLG() {
    ;
}

bool CTemplateNLG::Load(const std::string& nlg_file) {
    VLOG(DATA) << __FUNCTION__ << ", file: " << nlg_file;
    file::SimpleLineReader reader;
    
    if ( !reader.Open(nlg_file) ) {
        VLOG(FATAL) << "CMapNLG Load can not read file, " << nlg_file;
        return false;
    }
    
    std::vector<std::string> lines;
    std::string key;
    std::string sentence;
    bool is_start = true;

    reader.ReadLines(&lines);
    STemplate templ;

    for ( size_t i = 0;i < lines.size(); ++i ) {
        std::string line = Trim(lines[i]);
        if ( line.empty() || line[0] == '#' || line[0] == '-' ) {
            continue;
        }
        if ( line == ";" ) {
            if ( is_start == false ) {
                is_start = true;
                if ( !templ.Empty() ) {
                    _name2template[templ.nlgName].push_back(templ);
                }
                templ.Clear();
                continue;
            } else {
                VLOG(FATAL) << __FUNCTION__ << ", format error, not ;...";
                return false;
            }
        }
        if ( line[line.length()-1] != ')' ) {
            VLOG(FATAL) << __FUNCTION__ << ", format error, line: " << line;
            return false;
        }
        if ( is_start ) {
            size_t st_pos = line.find_first_of("(");
            if ( st_pos == std::string::npos || st_pos == 0 ) {
                VLOG(FATAL) << __FUNCTION__ << ", format error, line: " << line;
                return false;
            }
            templ.nlgName = StringToLowerASCII(line.substr(0, st_pos));
            is_start = false;
            templ.is_check_field = true;
            std::string params = Trim(line.substr(st_pos+1, (int)line.size()-st_pos-2));

            if ( templ.nlgName[0] == '@' ) {
                if ( !params.empty() ) {
                    if ( params[0] == '$' ) {
                        templ.variableCount += 1;
                    }
                    templ.field2parameter.insert(std::make_pair(params, params) );
                }
            } else {
                if ( !params.empty() ) {
                    if ( params == "NONE" ) {
                        templ.is_check_field = false;
                    } else {
                        std::vector<std::string> pairs;
                        SplitString(params, ',', &pairs);
                        for ( size_t j = 0;j < pairs.size(); ++j ) {
                            std::string field;
                            std::string value;
                            if ( SplitOnFirst(pairs[j], "=", field, value) ) {
                                field = Trim(field);
                                value = Trim(value);
                                if ( !field.empty() && !value.empty() ) {
                                    if ( value[0] == '$' ) {
                                        templ.variableCount += 1;
                                    }
                                    templ.field2parameter.insert(std::make_pair(field, value));
                                } else {
                                    VLOG(FATAL) << __FUNCTION__ << ", format error, line: " << line;
                                    return false;
                                }
                            } else {
                                VLOG(FATAL) << __FUNCTION__ << ", format error, line: " << line;
                                return false;
                            }
                        }
                    }
                }
            }
        } else {
            if ( line.size() < 2 || line[0] != '(' || line[line.size()-1] != ')' ) {
                VLOG(FATAL) << __FUNCTION__ << ", format error, line: " << line;
                return false;
            }
            templ.sentences.push_back(line.substr(1, (int)line.size()-2));
        }
    }
    if ( !is_start ) {
        VLOG(FATAL) << __FUNCTION__ << ", format error, file end..";
        return false;
    }

    for ( std::map<std::string, std::vector<STemplate> >::iterator iter = _name2template.begin(); 
            iter != _name2template.end(); ++iter ) {
        std::sort(iter->second.begin(), iter->second.end(), TemplateSort);
    }

    /*
    for (std::map<std::string, std::vector<STemplate> >::iterator iter = _name2template.begin(); iter != _name2template.end(); ++iter) {
        VLOG(DEBUG) << "NlgTemplate: " << iter->first;
        for ( size_t i= 0;i < iter->second.size(); ++i ) {
            VLOG(DEBUG) << "Sentences: " << VectorToString<std::string>(iter->second[i].sentences, "|");
            VLOG(DEBUG) << "Field2Parameter: " << MapToString<std::string, std::string>(iter->second[i].field2parameter, "|");
            VLOG(DEBUG) << "VariableCount: " << iter->second[i].variableCount;
        }
    }
    */
    return true;
}

bool CTemplateNLG::Generate(const std::string& nlg_template, CConcept& concept, std::string& result) {
    VLOG(DEBUG) << __FUNCTION__ << ", nlg_template: " << nlg_template << 
        ", concept: " << concept.HypSetToString();
    std::map<std::string, std::vector<STemplate> >::const_iterator f_iter = _name2template.find(StringToLowerASCII(nlg_template));
    if ( f_iter == _name2template.end() ) {
        VLOG(WARNING) << __FUNCTION__ << ", not nlg_template, " << nlg_template;
        return false;
    }
    for ( size_t i = 0;i < f_iter->second.size(); ++i ) {
        const STemplate& templ = f_iter->second[i];
        std::map<std::string, CConcept&> parameter2value;
        if ( IsMatch(templ, concept, parameter2value) ) {
            if ( Generate(parameter2value, templ, result) ) {
                return true;
            }
        }
    }
    VLOG(WARNING) << __FUNCTION__ << ", Can not find match templ..";
    return false;
}

bool CTemplateNLG::IsMatch(const STemplate& templ, CConcept& concept, std::map<std::string, CConcept&>& parameter2value) {
    VLOG(DEBUG) << __FUNCTION__ << ", templ: " << templ.nlgName;

    if ( !templ.is_check_field || (templ.field2parameter.empty() && concept == CConcept::NULLConcept) ) {
        return true;
    }
    if ( templ.nlgName[0] == '@' ) {
        if ( templ.field2parameter.empty() ) {
            return false;
        }
        const std::string& value = templ.field2parameter.begin()->second;
        if ( value.empty() ) {
            return false;
        }
        if ( value[0] == '$' ) {
            parameter2value.insert( std::map<std::string, CConcept&>::value_type(value, concept) );
            return true;
        } else {
            if ( value != (std::string)(concept) ) {
                return false;
            } else {
                return true;
            }
        }
    } else {
        if ( concept.GetConceptType() != ctFrame || concept.GetItemsSize() != templ.field2parameter.size() ) {
            VLOG(WARNING) << __FUNCTION__ << ", concept.GetConceptType() != ctFrame: " << (concept.GetConceptType() != ctFrame);
            return false;
        }
        for (std::map<std::string, std::string>::const_iterator c_iter = templ.field2parameter.begin();
                c_iter != templ.field2parameter.end(); ++c_iter ) {
            const std::string& field = c_iter->first;
            const std::string& parameter = c_iter->second;
            if ( field.empty() || parameter.empty() ) {
                VLOG(WARNING) << __FUNCTION__ << ", field and parameter can not is empty.";
                return false;
            }
            VLOG(DEBUG) << __FUNCTION__ << ", field: " << field << ", parameter: " << parameter;
            CConcept& value = concept[field];
            if ( value.GetName() == CConcept::NULLConcept.GetName() ) {
                VLOG(WARNING) << __FUNCTION__ << ", concept not field, " << field;
                return false;
            }
            if ( parameter[0] == '$' ) {
                parameter2value.insert(std::map<std::string, CConcept&>::value_type(parameter, value));
            } else if (parameter != (std::string)(value) ) {
                VLOG(DATA) << __FUNCTION__ << ", parameter: " << parameter << ", value: " << (std::string)value;
                return false;
            }
        }
    }
    return true;
}

bool CTemplateNLG::Generate(const std::map<std::string, CConcept&>& parameter2value, const STemplate& templ, std::string& result) {
    result.clear();
    std::string sentence;
    if ( !SelectRandomSentence(templ.sentences, sentence) ) {
        VLOG(FATAL) << __FUNCTION__ << ", SelectRandomSentence error, nlgName: " << templ.nlgName;
        return false;
    }
    int flag = 0;
    std::string field = "";

    for (size_t i = 0;i < sentence.size(); ++i ) {
        if ( sentence[i] == '{' ) {
            flag += 1;
        } else if ( sentence[i] == '}' ) {
            flag -= 1;
            if ( flag == 0 ) {
                if ( field.empty() ) {
                    VLOG(WARNING) << __FUNCTION__ << ", field can not is emptyl..";
                    return false;
                }
                if ( field[0] == '$' ) {
                    std::map<std::string, CConcept&>::const_iterator f_iter = parameter2value.find(field);
                    if ( f_iter == parameter2value.end() ) {
                        VLOG(WARNING) << __FUNCTION__ << ", not field, " << field;
                        return false;
                    }
                    result += (std::string)(f_iter->second);
                } else if (field[0] == '&') {
                    size_t pos = field.find("(");
                    if ( pos == std::string::npos || field[field.length()-1] != ')' ) {
                        VLOG(WARNING) << __FUNCTION__ << ", field error, " << field;
                        return false;
                    }
                    std::string nlg_template = field.substr(1, pos-1);
                    std::string param = field.substr(pos+1, field.length()-pos-2);
                    if ( param.length() >= 2 && param[0] == '[' && param[param.length()-1] == ']' ) {
                        param = param.substr(1, param.length()-2);
                        std::string value;
                        std::string sep;
                        // $businesses, ，
                        if ( SplitOnFirst(param, ",", value, sep) ) {
                            std::map<std::string, CConcept&>::const_iterator f_iter = parameter2value.find(value);
                            if (f_iter == parameter2value.end() ) {
                                VLOG(WARNING) << __FUNCTION__ << ", not field, " << value;
                                return false;
                            }
                            CConcept& concept = f_iter->second;
                            if ( concept.GetConceptType() != ctArray ) {
                                VLOG(WARNING) << __FUNCTION__ << ", concept not ctArray, Type: " << concept.GetConceptType();
                                return false;
                            }
                            for ( size_t j = 0;j < concept.GetSize(); ++j ) {
                                std::string tmp;
                                if ( Generate(nlg_template, concept[j], tmp) ) {
                                    if ( j > 0 ) {
                                        result += sep;
                                    }
                                    result += tmp;
                                } else {
                                    VLOG(WARNING) << __FUNCTION__ << "Generate(nlg_template, concept, tmp) error, " << nlg_template;
                                    return false;
                                }
                            }
                        } else {
                            VLOG(WARNING) << __FUNCTION__ << ", field error. " << field << ", param: " << param;
                            return false;
                        }
                    } else if ( !param.empty() && param[0] == '$' ) {
                        std::map<std::string, CConcept&>::const_iterator f_iter = parameter2value.find(param);
                        if (f_iter == parameter2value.end() ) { 
                            VLOG(WARNING) << __FUNCTION__ << ", not field, " << param;
                            return false;
                        }
                        CConcept& concept = f_iter->second;
                        std::string tmp;
                        if ( Generate(nlg_template, concept, tmp) ) {
                            result += tmp;
                        } else {
                            VLOG(WARNING) << __FUNCTION__ << "Generate(nlg_template, concept, tmp) error, " << nlg_template;
                            return false;
                        }
                    }
                } else {
                    VLOG(WARNING) << __FUNCTION__ << ", sentence is error.";
                    return false;
                }
                field = "";
            } else {
                VLOG(WARNING) << __FUNCTION__ << ", {} not match..";
                return false;
            }
        } else {
            if (flag == 1) {
                field += sentence[i];
            } else if ( flag == 0 ) {
                result += sentence[i];
            } else {
                VLOG(WARNING) << __FUNCTION__ << ", {} not match..";
                return false;
            }
        }
    }
    return true;
}

}
