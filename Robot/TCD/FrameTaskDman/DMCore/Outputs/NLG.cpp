#include "NLG.h"
#include "MapNLG.h"
#include "TemplateNLG.h"

namespace TrioTDM {

CNLG* CNLG::s_instance = NULL;

CNLG::CNLG() :
    _map_nlg(NULL) {
    ;
}

CNLG::~CNLG() {
    if ( _map_nlg != NULL ) {
        delete _map_nlg;
        _map_nlg = NULL;
    }
    if ( _template_nlg != NULL ) {
        delete _template_nlg;
        _template_nlg = NULL;
    }
}

CNLG* CNLG::Instance() {
    if ( s_instance == NULL ) {
        s_instance = new CNLG();
    }
    return s_instance;
}

bool CNLG::Init(const std::string& template_file, const std::string& dit_file) {
    _map_nlg = new CMapNLG();

    if ( _map_nlg == NULL ) {
        VLOG(FATAL) << "new CMapNLG() fail.";
        return false;
    }

    // dit_file is optional
    if ( !dit_file.empty() && !_map_nlg->Load(dit_file) ) {
        VLOG(FATAL) << "_map_nlg Load fail. file: " << dit_file;
        return false;
    }

    _template_nlg = new CTemplateNLG();

    if ( _template_nlg == NULL ) {
        VLOG(FATAL) << "new CTemplateNLG() fail.";
        return false;
    }

    if ( !_template_nlg->Load(template_file) ) {
        VLOG(FATAL) << "_template_nlg load file fail. file: " << template_file;
        return false;
    }

    return true;
}

std::string CNLG::Generate(const std::string& sentence, CConcept& info) {
    std::string type;
    std::string value;
    VLOG(DEBUG) << "NLG, sentence: " << sentence;
    if ( !SplitOnFirst(sentence, " ", type, value) ) {
        VLOG(WARNING) << "sentence form error, " << sentence;
        return sentence;
    }
    type = StringToUpperASCII(Trim(type));
    std::string result;
    if ( type == "MAP" ) {
        if ( !_map_nlg ) {
            VLOG(FATAL) << "_map_nlg is NULL.";
        } else if ( !_map_nlg->Generate(value, result) ) {
            result.clear();
            VLOG(WARNING) << "_map_nlg call Generate error, " << sentence;
        }
    } else if ( type == "TEMPLATE" ) {
        if ( !_template_nlg ) {
            VLOG(FATAL) << "_template_nlg is NULL.";
        } else if ( !_template_nlg->Generate(value, info, result) ) {
            result.clear();
            VLOG(WARNING) << "_template_nlg call Generate error, " << sentence;
        }
    }
    ReplaceSubstringsAfterOffset(&result, 0, "\\n", "\n");
    return result;
}

}
