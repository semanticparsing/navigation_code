#include "Output.h"

namespace TrioTDM {

COutput::COutput() : 
    _model_type_operator(0),
    _model_type_dtmf_length(0),
    _model_type_end(0),
    _is_output(false),
    _is_helpful(false),
    _output_type(O_UNDEFINE) {
}

COutput::~COutput() {
    ;
}

Json::Value COutput::ToJson() {
    Json::Value j_data;

    JSON_ADD_ITEM(j_data, _generator_agent_name);
    JSON_ADD_ITEM(j_data, _telephone);
    JSON_ADD_ITEM(j_data, _send_msg);
    JSON_ADD_ITEM(j_data, _dialog_state);
    JSON_ADD_ITEM(j_data, _act);
    JSON_ADD_ITEM(j_data, _object);
    JSON_ADD_ITEM(j_data, _model_type_operator);
    JSON_ADD_ITEM(j_data, _model_type_dtmf_length);
    JSON_ADD_ITEM(j_data, _model_type_end);
    JSON_ADD_ITEM(j_data, _outaction);
    JSON_ADD_ITEM(j_data, _repeat_counter);
    JSON_ADD_ITEM(j_data, _output_id);
    JSON_ADD_ITEM(j_data, _execution_index);
    JSON_ADD_ITEM(j_data, _is_output);
    JSON_ADD_ITEM(j_data, _conveyance);
    JSON_ADD_ITEM(j_data, _output_type);

    return j_data;
}

bool COutput::FromJson(const Json::Value& j_data) {
    
    JSON_ITEM_TO_STRING(j_data, _generator_agent_name);
    JSON_ITEM_TO_STRING(j_data, _telephone);
    JSON_ITEM_TO_STRING(j_data, _send_msg);
    JSON_ITEM_TO_STRING(j_data, _dialog_state);
    JSON_ITEM_TO_STRING(j_data, _act);
    JSON_ITEM_TO_STRING(j_data, _object);

    JSON_ITEM_TO_INT(j_data, _model_type_operator);
    JSON_ITEM_TO_INT(j_data, _model_type_dtmf_length);
    JSON_ITEM_TO_INT(j_data, _model_type_end);
    JSON_ITEM_TO_INT(j_data, _outaction);
    JSON_ITEM_TO_INT(j_data, _repeat_counter);
    JSON_ITEM_TO_INT(j_data, _output_id);
    JSON_ITEM_TO_INT(j_data, _execution_index);

    JSON_ITEM_TO_BOOL(j_data, _is_output);

    JSON_ITEM_TO_ENUM(j_data, _conveyance, TConveyance);
    JSON_ITEM_TO_ENUM(j_data, _output_type, TEOutputType);

    return true;
}

void COutput::SetModelType(const std::string& model_type) {
    _model_type = model_type;
}

std::string COutput::ModelType() const {
    if ( !_model_type.empty() ) {
        return _model_type;
    }
    char model_type[8];
    memset(model_type, '0', sizeof(model_type));
    model_type[7] = '\0';
    if ( _model_type_operator & BROADCAST ) {
        model_type[0] = '1';
    }
    if ( _model_type_operator & RECOGNITION ) {
        model_type[1] = '1';
    }
    if ( _model_type_operator & DTMF ) {
        model_type[2] = '1';
    }
    if ( _model_type_dtmf_length >= 0 && _model_type_dtmf_length < 99 ) {
        sprintf(&model_type[3], "%02d", _model_type_dtmf_length);
        model_type[5] = '0';
    } else {
        VLOG(WARNING) << __FUNCTION__ << ", _model_type_dtmf_length not in Range, Value: " << _model_type_dtmf_length;
    }
    if ( _model_type_end & END_ASTERISK ) {
        model_type[5] = '1';
    }
    if ( _model_type_end & END_POUND_SIGN ) {
        model_type[6] = '1';
    }

    VLOG(DEBUG) << __FUNCTION__ << ", " << model_type;
    
    return model_type;
}

void COutput::SetModelTypeOperator(int model_type_operator) {
    _model_type_operator = model_type_operator;
}

void COutput::SetModelTypeDTMFLength(int model_type_dtmf_length) {
    _model_type_dtmf_length = model_type_dtmf_length;
}

void COutput::SetModelTypeEnd(int model_type_end) {
    _model_type_end = model_type_end;
}

void COutput::SetOutaction(int outaction) {
    _outaction = outaction;
}

int COutput::GetOutaction() const {
    VLOG(DEBUG) << __FUNCTION__ << ", Outaction: " << _outaction;
    return _outaction;
}

void COutput::SetOutputType(TEOutputType output_type) {
    _output_type = output_type;
}

TEOutputType COutput::GetOutputType() const {
    return _output_type;
}

void COutput::SetIsHelpful(bool is_helpful) {
    _is_helpful = is_helpful;
}

bool COutput::GetIsHelpful() const {
    return _is_helpful;
}

void COutput::SetGenAgentName(const std::string& agent_name) {
    _generator_agent_name = agent_name;
}

std::string COutput::GetGenAgentName() const {
    return _generator_agent_name;
}

void COutput::SetTelephone(const std::string& telephone) {
    _telephone = telephone;
}

void COutput::SetSendMsg(const std::string& msg) {
    std::vector<std::string> tokens;
    SplitString(msg, '&', &tokens);

    for ( size_t i = 0;i < tokens.size(); ++i ) {
        std::string param;
        std::string value;

        if ( SplitOnFirst(tokens[i], "=", param, value) ) {
            if ( param == "is_helpful" ) {
                if ( value == "1" || StringToUpperASCII(value) == "TRUE" ) {
                    _is_helpful = true;
                } else {
                    _is_helpful = false;
                }
            }
        }
    }

    _send_msg = msg;
}

std::string COutput::Msg() const {
    return _send_msg;
}

std::string COutput::GetTelephone() const {
    return _telephone;
}

std::string COutput::ToString() {
    return "";
}

COutput* COutput::Clone(int output_id) {
    return NULL;
}

std::string COutput::GetGeneratorAgentName() {
    return "";
}

int COutput::GetDialogStateIndex() {
    return 0;
}

void COutput::SetDialogStateIndex(int index) {
}

std::string COutput::GetDialogState() {
    return "";
}
    
void COutput::SetDialogState(const std::string& state) {
}

    
void COutput::SetConveyance(TConveyance conveyance) {
}

TConveyance COutput::GetConveyance() {
    return cFailed;
}

 
void COutput::SetAct(const std::string& act) {
}
 
std::string COutput::GetAct() {
    return "";
}

/*
void COutput::SetFinalFloorStatus(TFloorStatus floor) {

}
 
TFloorStatus COutput::GetFinalFloorStatus() {
    return fsUnknown;
}
*/

std::string COutput::GetFinalFloorStatusLabel() {
    return "";
}

 
bool COutput::CheckFlag(const std::string& flag) {
    return false;
}
 
void COutput::NotifyConceptConveyance(const std::string& concept_name, TConveyance conveyance) {
}

CConcept* COutput::GetConceptByName(const std::string& name) {
    return NULL;
}
 
void COutput::CancelConceptNotificationRequest(CConcept* concept) {
}

void COutput::ChangeConceptNotificationPointer(CConcept* old_concept, CConcept* new_concept) {

}

int COutput::GetRepeatCounter() {
    return 0;
}

void COutput::IncrementRepeatCounter() {
}

void COutput::Clone(COutput* output, int new_output_id) {
}

}
