#include "StringConcept.h"

namespace TrioTDM {

CStringHyp::CStringHyp() {
    _hyp_type = ctString;
    _value = "";
    _confidence = 0.0;
}

CStringHyp::CStringHyp(CStringHyp& hyp) {
    _hyp_type = ctString;
    _value = hyp._value;
    _confidence = hyp._confidence;
}

CStringHyp::CStringHyp(const std::string& value, float confidence) {
    _hyp_type = ctString;
    _value = value;
    _confidence = confidence;
}

CHyp& CStringHyp::operator = (const std::string& value) {
    _value = value;
    _confidence = 1.0;
    return *this;
}

CHyp& CStringHyp::operator = (char* value) {
    _value = value;
    _confidence = 1.0;
    return *this;
}

CHyp& CStringHyp::operator = (CHyp& value) {
    if (&value != this ) {
        if ( value.GetHypType() != ctString ) {
            VLOG(WARNING) << "not ctString, " << value.GetHypType();
            return *this;
        }
        CStringHyp& hyp = (CStringHyp&)value;
        _value = hyp._value;
        _confidence = hyp._confidence;
    }
    return *this;
}

bool CStringHyp::operator == (CHyp& hyp) {
    if ( hyp.GetHypType() != ctString ) {
        VLOG(WARNING) << "not ctString, " << hyp.GetHypType();
        return false;
    }
    return _value == ((CStringHyp&)hyp)._value;
}

bool CStringHyp::operator < (CHyp& hyp) {
    VLOG(WARNING) << "Comparison operator < called on CStringHyp.";
    return false;
}

bool CStringHyp::operator > (CHyp& hyp) {
    VLOG(WARNING) << "Comparison operator > called on CStringHyp.";
    return false;
}

bool CStringHyp::operator <= (CHyp& hyp) {
    VLOG(WARNING) << "Comparison operator <= called on CStringHyp.";
    return false;
}

bool CStringHyp::operator >= (CHyp& hyp) {
    VLOG(WARNING) << "Comparison operator >= called on CStringHyp.";
    return false;
}

CHyp* CStringHyp::operator [] (const std::string& item) {
    VLOG(WARNING) << "Indexing operator [] called on CStringHyp," << item;
    return NULL;
}

std::string CStringHyp::ValueToString() {
    return _value;
}

std::string CStringHyp::ToString() {
    return _value + FormatString("%s%.2f", VAL_CONF_SEPARATOR, _confidence);
}

void CStringHyp::FromString(const std::string& strg) {
    std::string confidence;
    SplitOnFirst(strg, VAL_CONF_SEPARATOR, _value, confidence);
    _value = Trim(_value);
    confidence = Trim(confidence);

    if ( _value.length() > 1 && _value[0] == '"' && _value[_value.length()-1] == '"' ) {
        _value = _value.substr(1, _value.length()-2);
    }
    if ( confidence == "" ) {
        _confidence = 1.0;
    } else {
        if ( sscanf(confidence.c_str(), "%f", &_confidence) != 1) {
            VLOG(WARNING) << "Cannot perform conversion to CStringHyp from " << strg;
        }
    }
}

CStringConcept::CStringConcept(const std::string& name, TConceptSource source) :
    CConcept(name, source, DEFAULT_STRING_CARDINALITY) {
        _concept_type = ctString;
}

CStringConcept::CStringConcept(const std::string& name, const std::string& value, float confidence, TConceptSource source) {
    _concept_type = ctString;
    _concept_source = source;
    _name = name;
    _owner_dialog_agent = NULL;
    _owner_concept = NULL;
    _grounding_model = NULL;
    _current_hyps.push_back(new CStringHyp(value, confidence));
    _num_valid_hyps = 1;
    _cardinality = DEFAULT_STRING_CARDINALITY;
    _turn_last_updated = -1;
    _is_waiting_conveyance = false;
    _conveyance = cNotConveyed;
    SetHistoryConcept(false);
    _prev_concept = NULL;
}

CConcept& CStringConcept::operator = (CConcept& concept) {
    return CConcept::operator = (concept);
}

CConcept& CStringConcept::operator = (bool value) {
    if ( _is_history_concept ) {
        VLOG(WARNING) << "Cannot assign to concept history, " << _name;
        return *this;
    }
    ClearCurrentHypSet();
    if ( value ) {
        AddHyp(new CStringHyp("true", 1.0));
    } else {
        AddHyp(new CStringHyp("false", 1.0));
    }
    SetGroundedFlag(true);
    return *this;
}

CConcept& CStringConcept::operator = (int value) {
    if ( _is_history_concept ) {
        VLOG(WARNING) << "Cannot assign to concept history, " << _name;
        return *this;
    }
    ClearCurrentHypSet();
    char buffer[256];
    sprintf(buffer, "%d", value);
    AddHyp(new CStringHyp(std::string(buffer), 1.0));
    SetGroundedFlag(true);
    return *this;
}

CConcept& CStringConcept::operator = (float value) {
    if ( _is_history_concept ) {
        VLOG(WARNING) << "Cannot assign to concept history, " << _name;
        return *this;
    }
    ClearCurrentHypSet();
    char buffer[256];
    sprintf(buffer, "%.4f", value);
    AddHyp(new CStringHyp(std::string(buffer), 1.0));
    SetGroundedFlag(true);
    return *this;
}

CConcept& CStringConcept::operator = (double value) {
    return operator = ((float)value);
}

CConcept* CStringConcept::EmptyClone() {
    return new CStringConcept;
}

CHyp* CStringConcept::HypFactory() {
    VLOG(DEBUG) << "call HypFactory on CStringConcept.";
    return new CStringHyp();
}

}
