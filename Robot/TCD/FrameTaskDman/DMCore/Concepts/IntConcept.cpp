#include "IntConcept.h"

namespace TrioTDM {

CIntHyp::CIntHyp() {
    _hyp_type = ctInt;
    _value = 0;
    _confidence = 0.0;
}

CIntHyp::CIntHyp(CIntHyp& hyp) {
    _hyp_type = ctInt;
    _value = hyp._value;
    _confidence = hyp._confidence;
}

CIntHyp::CIntHyp(int value, float confidence) {
    _hyp_type = ctInt;
    _value = value;
    _confidence = confidence;
}

CHyp& CIntHyp::operator = (int value) {
    _value = value;
    _confidence = 1.0;
    return *this;
}

CHyp& CIntHyp::operator = (CHyp& hyp) {
    if ( &hyp != this ) {
        if ( hyp.GetHypType() != ctInt ) {
            VLOG(WARNING) << "not ctInt, " << hyp.GetHypType();
            return *this;
        }
        CIntHyp& t_hyp = (CIntHyp&)hyp;
        _value = t_hyp._value;
        _confidence = t_hyp._confidence;
    }
    return *this;
}

bool CIntHyp::operator == (CHyp& hyp) {
    if ( hyp.GetHypType() != ctInt ) {
        VLOG(WARNING) << "not ctInt";
        return false;
    }
    return _value == ((CIntHyp&)hyp)._value;
}

bool CIntHyp::operator < (CHyp& hyp) {
    if ( hyp.GetHypType() != ctInt ) {
        VLOG(WARNING) << "not ctInt";
        return false;
    }
    return _value < ((CIntHyp&)hyp)._value;
}

bool CIntHyp::operator > (CHyp& hyp) {
    if ( hyp.GetHypType() != ctInt ) {
        VLOG(WARNING) << "not ctInt";
        return false;
    }
    return _value > ((CIntHyp&)hyp)._value;
}

bool CIntHyp::operator <= (CHyp& hyp) {
    if ( hyp.GetHypType() != ctInt ) {
        VLOG(WARNING) << "not ctInt";
        return false;
    }
    return _value <= ((CIntHyp&)hyp)._value;
}

bool CIntHyp::operator >= (CHyp& hyp) {
    if ( hyp.GetHypType() != ctInt ) {
        VLOG(WARNING) << "not ctInt";
        return false;
    }
    return _value >= ((CIntHyp&)hyp)._value;
}

CHyp* CIntHyp::operator [] (const std::string& item) {
    VLOG(WARNING) << "Indexing operator [] called on CIntHyp.";
    return NULL;
}

std::string CIntHyp::ValueToString() {
    return FormatString("%d", _value);
}

std::string CIntHyp::ToString() {
    return FormatString("%d%s%.2f", _value, VAL_CONF_SEPARATOR, _confidence);
}

void CIntHyp::FromString(const std::string& strg) {
    std::string value, confidence;
    SplitOnFirst(strg, VAL_CONF_SEPARATOR, value, confidence);
    value = Trim(value);
    confidence = Trim(value);
    if ( sscanf(value.c_str(), "%d", &_value) != 1 ) {
        VLOG(WARNING) << "Cannot conversion to int, " << value;
    }
    if ( confidence == "" ) {
        _confidence = 1.0;
    } else {
        if ( sscanf(confidence.c_str(), "%f", &_confidence) != 1 ) {
            VLOG(WARNING) << "Cannot conversion to float, " << confidence;
        }
    }
}

CIntConcept::CIntConcept(const std::string& name, TConceptSource source) :
    CConcept(name, source, DEFAULT_INT_CARDINALITY) {
        _concept_type = ctInt;
}

CIntConcept::CIntConcept(const std::string& name, int value, float confidence, TConceptSource source) {
    _concept_type = ctInt;
    _concept_source = source;
    _name = name;
    _owner_dialog_agent = NULL;
    _owner_concept = NULL;
    _grounding_model = NULL;
    _current_hyps.push_back(new CIntHyp(value, confidence));
    _num_valid_hyps = 1;
    _cardinality = DEFAULT_INT_CARDINALITY;
    _turn_last_updated = -1;
    _is_waiting_conveyance = false;
    _conveyance = cNotConveyed;
    SetHistoryConcept(false);
    _prev_concept = NULL;
}

CConcept& CIntConcept::operator = (bool value) {
    if ( _is_history_concept ) {
        VLOG(WARNING) << "Cannot assign to concept history, " << _name;
        return *this;
    }
    ClearCurrentHypSet();
    AddHyp(new CIntHyp((int)value, 1.0));
    SetGroundedFlag(true);
    return *this;
}

CConcept& CIntConcept::operator = (int value) {
    VLOG(DEBUG) << "operator =, this: " << this;
    if ( _is_history_concept ) {
        VLOG(WARNING) << "Cannot assign to concept history, " << _name;
        return *this;
    }
    ClearCurrentHypSet();
    AddHyp(new CIntHyp(value, 1.0));
    SetGroundedFlag(true);
    return *this;
}

CConcept& CIntConcept::operator = (float value) {
    if ( _is_history_concept ) {
        VLOG(WARNING) << "Cannot assign to concept history, " << _name;
        return *this;
    }
    ClearCurrentHypSet();
    AddHyp(new CIntHyp((int)value, 1.0));
    SetGroundedFlag(true);
    return *this;
}

CConcept& CIntConcept::operator = (double value) {
    if ( _is_history_concept ) {
        VLOG(WARNING) << "Cannot assign to concept history, " << _name;
        return *this;
    }
    ClearCurrentHypSet();
    AddHyp(new CIntHyp((int)value, 1.0));
    SetGroundedFlag(true);
    return *this;
}

CIntConcept::operator bool() {
    return (operator int() != 0);
}

CIntConcept::operator int() {
    if ( IsUpdatedAndGrounded() && GetTopHyp() ) {
        return ((CIntHyp*)GetTopHyp())->_value;
    } else {
        return 0;
    }
}

CConcept* CIntConcept::EmptyClone() {
    return new CIntConcept;
}

CHyp* CIntConcept::HypFactory() {
    VLOG(DEBUG) << "call HypFactory on CIntConcept.";
    return new CIntHyp();
}

}
