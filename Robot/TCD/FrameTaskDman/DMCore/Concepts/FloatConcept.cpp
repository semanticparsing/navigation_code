#include "FloatConcept.h"

namespace TrioTDM {

CFloatHyp::CFloatHyp() {
    _hyp_type = ctFloat;
    _value = 0.0;
    _confidence = 0.0;
}

CFloatHyp::CFloatHyp(CFloatHyp& hyp) {
    _hyp_type = ctFloat;
    _value = hyp._value;
    _confidence = hyp._confidence;
}

CFloatHyp::CFloatHyp(float value, float confidence) {
    _hyp_type = ctFloat;
    _value = value;
    _confidence = confidence;
}

CHyp& CFloatHyp::operator = (float value) {
    _value = value;
    _confidence = 1.0;
    return *this;
}

CHyp& CFloatHyp::operator = (CHyp& hyp) {
    if ( &hyp != this ) {
        if ( hyp.GetHypType() != ctFloat ) {
            VLOG(WARNING) << "operator = hypType is not ctFloat, type: " << hyp.GetHypType();
            return *this;
        }
        CFloatHyp& f_hyp = (CFloatHyp&)hyp;
        _value = f_hyp._value;
        _confidence = f_hyp._confidence;
    }
    return *this;
}

bool CFloatHyp::operator == (CHyp& hyp) {
    if ( hyp.GetHypType() != ctFloat ) {
        VLOG(WARNING) << "operator == hypType is not ctFloat, type: " << hyp.GetHypType();
        return false;
    }
    return _value == ((CFloatHyp&)hyp)._value;
}

bool CFloatHyp::operator < (CHyp& hyp) {
    if ( hyp.GetHypType() != ctFloat ) {
        VLOG(WARNING) << "operator < hypType is not ctFloat, type: " << hyp.GetHypType();
        return false;
    }
    return _value < ((CFloatHyp&)hyp)._value;
}

bool CFloatHyp::operator > (CHyp& hyp) {
    if ( hyp.GetHypType() != ctFloat ) {
        VLOG(WARNING) << "operator > hypType is not ctFloat, type: " << hyp.GetHypType();
        return false;
    }
    return _value > ((CFloatHyp&)hyp)._value;
}

bool CFloatHyp::operator <= (CHyp& hyp) {
    if ( hyp.GetHypType() != ctFloat ) {
        VLOG(WARNING) << "operator <= hypType is not ctFloat, type: " << hyp.GetHypType();
        return false;
    }
    return _value <= ((CFloatHyp&)hyp)._value;
}

bool CFloatHyp::operator >= (CHyp& hyp) {
    if ( hyp.GetHypType() != ctFloat ) {
        VLOG(WARNING) << "operator >= hypType is not ctFloat, type: " << hyp.GetHypType();
        return false;
    }
    return _value >= ((CFloatHyp&)hyp)._value;
}

CHyp* CFloatHyp::operator [] (const std::string& item) {
    VLOG(WARNING) << "Indexing operator [] called on CFloatHyp.";
    return NULL;
}

std::string CFloatHyp::ValueToString() {
    return FormatString("%.4f", _value);
}

std::string CFloatHyp::ToString() {
    return FormatString("%.4f%s%.2f", _value, VAL_CONF_SEPARATOR,
            _confidence);
}

void CFloatHyp::FromString(const std::string& strg) {
    std::string value;
    std::string confidence;
    SplitOnFirst(strg, VAL_CONF_SEPARATOR, value, confidence);
    value = Trim(value);
    confidence = Trim(confidence);
    if ( sscanf(value.c_str(), "%f", &_value) != 1 ) {
        VLOG(FATAL) << "Cannot perform conversion to CFloatHyp, strg: " << strg;
        return ;
    }
    if ( confidence == "" ) {
        _confidence = 1.0;
    } else {
        if ( sscanf(confidence.c_str(), "%f", &_confidence) != 1 ) {
            VLOG(FATAL) << "Cannot perform conversion to CFloatHyp, strg: " << strg;
            return ;
        }
    }
}

CFloatConcept::CFloatConcept(const std::string& name, TConceptSource source) :
    CConcept(name, source, DEFAULT_FLOAT_CARDINALITY) {
    _concept_type = ctFloat;
}

CFloatConcept::CFloatConcept(const std::string& name, float value, float confidence, TConceptSource source) {
    _concept_type = ctFloat;
    _concept_source = source;
    _name = name;
    _owner_dialog_agent = NULL;
    _owner_concept = NULL;
    _grounding_model = NULL;
    _current_hyps.push_back(new CFloatHyp(value, confidence));
    _num_valid_hyps = 1;
    _cardinality = DEFAULT_FLOAT_CARDINALITY;
    _turn_last_updated = -1;
    _is_waiting_conveyance = false;
    _conveyance = cNotConveyed;
    SetHistoryConcept(false);
    _prev_concept = NULL;
}

CConcept& CFloatConcept::operator = (bool value) {
    if ( _is_history_concept ) {
        VLOG(FATAL) << "Cannot assign to concept history, name: " << _name;
        return NULLConcept;
    }
    ClearCurrentHypSet();
    AddHyp(new CFloatHyp((float)value, 1.0));
    SetGroundedFlag(true);
    return *this;
}

CConcept& CFloatConcept::operator = (int value) {
    if ( _is_history_concept ) {
        VLOG(FATAL) << "Cannot assign to concept history, name: " << _name;
        return NULLConcept;
    }
    ClearCurrentHypSet();
    AddHyp(new CFloatHyp((float)value, 1.0));
    SetGroundedFlag(true);
    return *this;
}

CConcept& CFloatConcept::operator = (float value) {
    if ( _is_history_concept ) {
        VLOG(FATAL) << "Cannot assign to concept history, name: " << _name;
        return NULLConcept;
    }
    ClearCurrentHypSet();
    AddHyp(new CFloatHyp(value, 1.0));
    SetGroundedFlag(true);
    return *this;
}

CConcept& CFloatConcept::operator = (double value) {
    if ( _is_history_concept ) {
        VLOG(FATAL) << "Cannot assign to concept history, name: " << _name;
        return NULLConcept;
    }
    ClearCurrentHypSet();
    AddHyp(new CFloatHyp((float)value, 1.0));
    SetGroundedFlag(true);
    return *this;
}

CFloatConcept::operator bool() {
    return (operator float() != 0);
}

CFloatConcept::operator int() {
    return (int)operator float();
}

CFloatConcept::operator float() {
    if ( IsUpdatedAndGrounded() && GetTopHyp() ) {
        return (((CFloatHyp*)GetTopHyp())->_value);
    } else {
        return 0.0;
    }
}

CConcept* CFloatConcept::EmptyClone() {
    return new CFloatConcept;
}

CHyp* CFloatConcept::HypFactory() {
    return new CFloatHyp();
}

}
