#include "BoolConcept.h"

namespace TrioTDM {

CBoolHyp::CBoolHyp() {
    _hyp_type = ctBool;
    _value = false;
    _confidence = 0.0;
}

CBoolHyp::CBoolHyp(CBoolHyp& hyp) {
    _hyp_type = ctBool;
    _value = hyp._value;
    _confidence = hyp._confidence;
}

CBoolHyp::CBoolHyp(bool value, float confidence) {
    _hyp_type = ctBool;
    _value = value;
    _confidence = confidence;
}

CHyp& CBoolHyp::operator = (bool value) {
    _value = value;
    _confidence = 1.0;
    return *this;
}

CHyp& CBoolHyp::operator = (CHyp& hyp) {
    if ( &hyp != this ) {
        if ( hyp.GetHypType() != ctBool ) {
            VLOG(WARNING) << "not ctBool";
            return *this;
        }

        CBoolHyp& b_hyp = (CBoolHyp&)hyp;
        _value = b_hyp._value;
        _confidence = b_hyp._confidence;
    }
    return *this;
}

bool CBoolHyp::operator == (CHyp& hyp) {
    if ( hyp.GetHypType() != ctBool ) {
        VLOG(WARNING) << "not ctBool";
        return false;
    }
    return _value == ((CBoolHyp&)hyp)._value;
}

bool CBoolHyp::operator < (CHyp& hyp) {
    VLOG(WARNING) << "conparison < on ctBool";
    return false;
}

bool CBoolHyp::operator > (CHyp& hyp) {
    VLOG(WARNING) << "conparison > on ctBool";
    return false;
}

bool CBoolHyp::operator <= (CHyp& hyp) {
    VLOG(WARNING) << "conparison <= on ctBool";
    return false;
}

bool CBoolHyp::operator >= (CHyp& hyp) {
    VLOG(WARNING) << "conparison >= on ctBool";
    return false;
}

CHyp* CBoolHyp::operator [](const std::string& item) {
    VLOG(WARNING) << "operator [] on ctBool";
    return NULL;
}

std::string CBoolHyp::ValueToString() {
    return _value ? "true" : "false";
}

std::string CBoolHyp::ToString() {
    std::string result = FormatString("%s%.2f", VAL_CONF_SEPARATOR, _confidence);
    result = (_value ? "true" : "false") + result;
    return result;
}

void CBoolHyp::FromString(const std::string& strg) {
    VLOG(DEBUG) << __FUNCTION__;
    std::string upper_value, confidence;
    SplitOnFirst(strg, VAL_CONF_SEPARATOR, upper_value, confidence);
    upper_value = StringToUpperASCII(Trim(upper_value));
    confidence = Trim(confidence);
    if ( upper_value == "TRUE" || upper_value == "1" ) {
        _value = true;
    } else if ( upper_value == "FALSE" || upper_value == "0" ) {
        _value = false;
    } else {
        VLOG(WARNING) << "upper_value not bool, " << upper_value;
        return ;
    }
    if ( confidence.empty() ) {
        _confidence = 1.0;
    } else {
        if ( sscanf(confidence.c_str(), "%f", &_confidence) != 1 ) {
            VLOG(WARNING) << __FUNCTION__ << "can not to double, " << confidence;
            return ;
        }
    }
}

CBoolConcept::CBoolConcept(const std::string& name, TConceptSource source) : 
    CConcept(name, source, DEFAULT_BOOL_CARDINALITY) {
    _concept_type = ctBool;
}

CBoolConcept::CBoolConcept(const std::string& name, bool value, float confidence, TConceptSource source) {
    _concept_type = ctBool;
    _concept_source = source;
    _name = name;
    _owner_dialog_agent = NULL;
    _owner_concept = NULL;
    _current_hyps.push_back(new CBoolHyp(value, confidence));
    _num_valid_partial_hyps = 1;
    _cardinality = DEFAULT_BOOL_CARDINALITY;
    _turn_last_updated = -1;
    _is_waiting_conveyance = false;
    _conveyance = cNotConveyed;
    SetHistoryConcept(false);
    _prev_concept = NULL;
}

CConcept& CBoolConcept::operator = (bool value) {
    if ( _is_history_concept ) {
        VLOG(WARNING) << "Cannot assign to concept " << _name << " history.";
        return *this;
    }
    ClearCurrentHypSet();
    AddHyp(new CBoolHyp(value, 1.0));
    SetGroundedFlag(true);
    return *this;
}

CConcept& CBoolConcept::operator = (int value) {
    if ( _is_history_concept ) {
        VLOG(WARNING) << "Cannot assign to concept " << _name << " history.";
        return *this;
    }
    ClearCurrentHypSet();
    AddHyp(new CBoolHyp(value, 1.0));
    SetGroundedFlag(true);
    return *this;
}

CBoolConcept::operator bool() {
    if ( IsUpdatedAndGrounded() && GetTopHyp() ) {
        return ((CBoolHyp *)GetTopHyp())->_value;
    } else {
        return false;
    }
}

CBoolConcept::operator int() {
    return (int)operator bool();
}

CConcept* CBoolConcept::EmptyClone() {
    return new CBoolConcept;
}

CHyp* CBoolConcept::HypFactory() {
    VLOG(DEBUG) << "call HypFactory on CBoolConcept.";
    return new CBoolHyp();
}

}
