#include "Concept.h"

#include "Robot/TCD/FrameTaskDman/DMCore/Agents/DialogAgents/DialogAgent.h"
#include "Robot/TCD/FrameTaskDman/DMCore/Agents/CoreAgents/AllCoreAgents.h"

namespace TrioTDM {

CConcept CConcept::NULLConcept("NULL");

CHyp::CHyp() {
    _hyp_type = ctUnknown;
    _confidence = 0.0;
}

CHyp::CHyp(CHyp& hyp) {
    _hyp_type = hyp._hyp_type;
    _confidence = hyp._confidence;
}

CHyp::~CHyp() {
}

TConceptType CHyp::GetHypType() {
    return _hyp_type;
}

float CHyp::GetConfidence() {
    return _confidence;
}

CHyp& CHyp::operator = (CHyp& hyp) {
    if ( &hyp != this) {
        _hyp_type = hyp._hyp_type;
        _confidence = hyp._confidence;
    }
    return *this;
}

void CHyp::SetConfidence(float confidence) {
    _confidence = confidence;
}

bool CHyp::operator == (CHyp& hyp) {
    VLOG(WARNING) << "Equality operator called on abstract CHyp.";
    return false;
}

bool CHyp::operator < (CHyp& hyp) {
    VLOG(WARNING) << "Comparsion operator < called on abstract CHyp.";
    return false;
}

bool CHyp::operator > (CHyp& hyp) {
    VLOG(WARNING) << "Comparsion operator > called on abstract CHyp.";
    return false;
}

bool CHyp::operator <= (CHyp& hyp) {
    VLOG(WARNING) << "Comparsion operator <= called on abstract CHyp.";
    return false;
}

bool CHyp::operator >= (CHyp& hyp) {
    VLOG(WARNING) << "Comparsion operator >= called on abstract CHyp.";
    return false;
}

CHyp* CHyp::operator [](const std::string& item) {
    VLOG(WARNING) << "Indexing operator [] called on abstract CHyp.";
    return NULL;
}

std::string CHyp::ValueToString() {
    VLOG(WARNING) << "ValueToString called on abstract CHyp.";
    return ABSTRACT_CONCEPT;
}

std::string CHyp::ToString() {
    VLOG(WARNING) << "ToString called on abstract CHyp.";
    return ABSTRACT_CONCEPT;
}

void CHyp::FromString(const std::string& str) {
    VLOG(WARNING) << "FromString called on abstruct CHyp. Call failed.";
}

CConcept::CConcept(const std::string& name, TConceptSource concept_source, int cardinality) {
    _concept_type = ctUnknown;
    _concept_source = concept_source;
    _name = name;
    _owner_dialog_agent = NULL;
    _owner_concept = NULL;
    _grounding_model = NULL;
    _is_grounded = false;
    _is_invalidated = false;
    _is_restored_for_grounding = false;
    _is_sealed = false;
    _is_change_notification = true;
    _num_valid_hyps = 0;
    _cardinality = cardinality;
    _turn_last_updated = -1;
    _conveyance = cNotConveyed;
    _is_waiting_conveyance = false;
    _prev_concept = NULL;
    _is_history_concept = false;
    _explictly_confirmed_hyp = "";
    _explicitly_disconfirmed_hyp = "";
}

CConcept::~CConcept() {
    if ( _prev_concept != NULL ) {
        delete _prev_concept;
    }
    _prev_concept = NULL;

    if ( CGroundingManagerAgent::Instance() != NULL ) {
        CGroundingManagerAgent::Instance()->RemoveConceptGroundingRequest(this);
        delete _grounding_model;
    }
    _grounding_model = NULL;

    if ( _is_waiting_conveyance ) { ClearWaitingConveyance(); }
    ClearConceptNotificationPointer();
}

size_t CConcept::GetItemsSize() {
    VLOG(WARNING) << "Can not call GetItemsSize on CConcept, name: " << _name;
    return 0;
}

CConcept& CConcept::operator = (CConcept& concept) {
    if ( &concept != this ) {
        Update(CU_ASSIGN_FROM_CONCEPT, &concept);
    }
    return *this;
}

CConcept& CConcept::operator = (int value) {
    VLOG(WARNING) << "Assignment operator from integer to " << _name << ConceptTypeAsString[_concept_type].c_str();

    return NULLConcept;
}

CConcept& CConcept::operator = (bool value) {
    VLOG(WARNING) << "Assignment operator from bool to " << _name << ConceptTypeAsString[_concept_type].c_str();
    return NULLConcept;
}

CConcept& CConcept::operator = (float value) {
    VLOG(WARNING) << "Assignment operator from float to " << _name << ConceptTypeAsString[_concept_type].c_str();
    return NULLConcept;
}

CConcept& CConcept::operator = (double value) {
    VLOG(WARNING) << "Assignment operator from double to " << _name << ConceptTypeAsString[_concept_type].c_str();
    return NULLConcept;
}

CConcept& CConcept::operator = (const std::string& value) {
    Update(CU_ASSIGN_FROM_STRING, &value);
    return *this;
}

CConcept& CConcept::operator = (const char* value) {
    return operator=((std::string)value);
}

bool CConcept::operator == (CConcept& concept) {
    CHyp* top_hyp_1 = GetTopHyp();
    CHyp* top_hyp_2 = concept.GetTopHyp();

    if ( (top_hyp_1 == NULL) && (top_hyp_2 == NULL) ) {
        return true;
    } else if ( (top_hyp_1 == NULL) || (top_hyp_2 == NULL) ) {
        return false;
    }
    return *top_hyp_1 == *top_hyp_2;
}

bool CConcept::operator != (CConcept& concept) {
    return !operator==(concept);
}

bool CConcept::operator < (CConcept& concept) {
    CHyp* top_hyp_1 = GetTopHyp();
    if ( top_hyp_1 == NULL ) { return false; }
    CHyp* top_hyp_2 = concept.GetTopHyp();
    if ( top_hyp_2 == NULL ) { return false; }
    return *top_hyp_1 < *top_hyp_2;
}

bool CConcept::operator > (CConcept& concept) {
    CHyp* top_hyp_1 = GetTopHyp();
    if ( top_hyp_1 == NULL ) { return false; }
    CHyp* top_hyp_2 = concept.GetTopHyp();
    if ( top_hyp_2 == NULL ) { return false; }
    return *top_hyp_1 > *top_hyp_2;
}

bool CConcept::operator <= (CConcept& concept) {
    CHyp* top_hyp_1 = GetTopHyp();
    if ( top_hyp_1 == NULL ) { return false; }
    CHyp* top_hyp_2 = concept.GetTopHyp();
    if ( top_hyp_2 == NULL ) { return false; }
    return *top_hyp_1 <= *top_hyp_2;
}

bool CConcept::operator >= (CConcept& concept) {
    CHyp* top_hyp_1 = GetTopHyp();
    if ( top_hyp_1 == NULL ) { return false; }
    CHyp* top_hyp_2 = concept.GetTopHyp();
    if ( top_hyp_2 == NULL ) { return false; }
    return *top_hyp_1 >= *top_hyp_2;
}

CConcept& CConcept::operator [](int index) {
    if ( index == 0 ) {
        return *this;
    }
    if ( (index < 0 ) && (_prev_concept != NULL) ) {
        if ( index == -1 ) {
            return *_prev_concept;
        } else {
            return _prev_concept->operator [](index + 1);
        }
    } else {
        VLOG(WARNING) << "Indexing operator " << index << " on " << _name << " atomic " << ConceptTypeAsString[_concept_type] << " failed.";
        return NULLConcept;
    }
}

CConcept& CConcept::operator [](const std::string& index) {
    if ( index.empty() ) {
        return *this;
    } else {
        std::string first_index, follow_up;
        SplitOnFirst(index, ".", first_index, follow_up);
        int n_first_index;
        if ( sscanf(first_index.c_str(), "%d", &n_first_index) != -1 ) {
            VLOG(WARNING) << "Indexing operator " << index << "on " << _name << " atomic " << ConceptTypeAsString[_concept_type] << "failed.";
            return NULLConcept;
        } else {
            CConcept *concept = &(operator[](n_first_index));
            if ( follow_up.empty() ) {
                return *concept;
            } else {
                return concept->operator[](follow_up);
            }
        }
    }
}


CConcept& CConcept::operator [](const char* index) {
    return operator[]((std::string)index);
}

CConcept::operator int() {
    VLOG(WARNING) << "Integer cast operator on " << _name << ConceptTypeAsString[_concept_type] << " type concept failed.";
    return 0;
}

CConcept::operator float() {
    VLOG(WARNING) << "Float cast operator on " << _name << ConceptTypeAsString[_concept_type] << " type concept failed.";
    return 0.0;
}

CConcept::operator bool() {
    VLOG(WARNING) << "Bool cast operator on " << _name << ConceptTypeAsString[_concept_type] << " type concept failed.";
    return false;
}

CConcept::operator std::string() {
    // TODO  HS
    std::string strg = Trim(GroundedHypToString());
    VLOG(DEBUG) << "Concept: " << _name << ", string: " << strg;
    return strg;
}

void CConcept::Clear() {
    VLOG(DEBUG) << __FUNCTION__ << ", Name: " << _name;
    if ( _is_history_concept ) {
        VLOG(WARNING) << "Cannot perform Clear on concept " << _name << " history.";
    }
    std::string initial_value;
    if ( _grounding_model ) {
        initial_value = TrimRight(HypSetToString());
    }
    if ( _prev_concept != NULL ) {
        delete _prev_concept;
        _prev_concept = NULL;
    }

    ClearCurrentHypSet();
    _explictly_confirmed_hyp = "";
    _explicitly_disconfirmed_hyp = "";
}

void CConcept::ClearCurrentValue() {
    std::string initial_value;
    if ( _grounding_model ) {
        initial_value = TrimRight( HypSetToString() );
    }
    ClearCurrentHypSet();
}

CConcept* CConcept::Clone( bool clone_history) {
    CConcept* concept = EmptyClone();

    concept->SetConceptType(_concept_type);
    concept->SetConceptSource(_concept_source);
    concept->_name = _name;
    concept->_owner_dialog_agent = _owner_dialog_agent;
    concept->SetOwnerConcept(_owner_concept);
    concept->_grounding_model = NULL;
    concept->_is_sealed = _is_sealed;
    concept->DisableChangeNotification();
    concept->CopyCurrentHypSetFrom(*this);
    concept->SetGroundedFlag(_is_grounded);
    concept->SetInvalidatedFlag(_is_invalidated);

    if ( clone_history ) {
        concept->SetRestoredForGroundingFlag(_is_restored_for_grounding);
    } else {
        concept->SetRestoredForGroundingFlag(false);
    }
    concept->_cardinality = _cardinality;
    concept->SetTurnLastUpdated(_turn_last_updated);
    concept->_conveyance = _conveyance;
    concept->_is_waiting_conveyance = false;
    concept->SetHistoryConcept(_is_history_concept);
    if ( clone_history && (_prev_concept != NULL) ) {
        concept->_prev_concept = _prev_concept->Clone();
    } else {
        concept->_prev_concept = NULL;
    }

    concept->_explictly_confirmed_hyp = _explictly_confirmed_hyp;
    concept->_explicitly_disconfirmed_hyp = _explicitly_disconfirmed_hyp;

    return concept;
}

CConcept* CConcept::EmptyClone() {
    return new CConcept;
}

void CConcept::Update(const std::string& update_type, const void* update_data) {
    if ( CGroundingManagerAgent::Instance() == NULL ) {
        VLOG(WARNING) << "CGroundingManagerAgent::Instance() == NULL";
        return ;
    }
    std::string initial_value;
    std::string initial_partial_value;
    if ( _grounding_model ) {
        initial_value = TrimRight(HypSetToString());
    }
    std::string belief_udating_model_name = CGroundingManagerAgent::Instance()->GetBeliefUpdatingModelName();
    VLOG(DEBUG) << "Update, update_type: " << update_type << "\tbelief_udating_model_name: " << belief_udating_model_name << ", Name: " << _name;
    if ( belief_udating_model_name == "npu" ) {
        if ( update_type == CU_ASSIGN_FROM_STRING ) {
            Update_NPU_AssignFromString(update_data);
        } else if ( update_type == CU_ASSIGN_FROM_CONCEPT ) {
            Update_NPU_AssignFromConcept(update_data);
        } else if ( update_type == CU_UPDATE_WITH_CONCEPT ) {
            Update_NPU_UpdateWithConcept(update_data);
        } else if ( update_type == CU_COLLAPSE_TO_MODE ) {
            Update_NPU_CollapseToMode(update_data);
        } else if ( update_type == CU_PARTIAL_FROM_STRING ) {
            Update_PartialFromString(update_data);
        } else {
            VLOG(WARNING) << "UnKnown update type " << update_type << " in updating concept " << GetAgentQualifiedName();
        }
        if ( update_type != CU_PARTIAL_FROM_STRING ) {
            ClearPartialHypSet();
        }
    } else if ( belief_udating_model_name == "calista" ) {
        if ( update_type == CU_ASSIGN_FROM_STRING ) {
            Update_Calista_AssignFromString(update_data);
        } else if ( update_type == CU_ASSIGN_FROM_CONCEPT ) {
            Update_Calista_AssignFromConcept(update_data);
        } else if ( update_type == CU_UPDATE_WITH_CONCEPT ) {
            Update_Calista_UpdateWithConcept(update_data);
        } else if ( update_type == CU_COLLAPSE_TO_MODE ) {
            Update_Calista_CollapseToMode(update_data);
        } else {
            VLOG(WARNING) << "UnKnown update type " << update_type << " in  updating concept " << GetAgentQualifiedName();
        }
    }
}

void CConcept::Update_NPU_AssignFromString(const void* update_data) {
    VLOG(DEBUG) << __FUNCTION__ << ", Name: " << _name << ", UpdateData: " << *(std::string*)update_data;
    if ( _is_history_concept ) {
        VLOG(WARNING) << "_is_history_concept is true. " << _name;
        return ;
    }
    std::string val_conf;
    std::string rest = *(std::string*)update_data;

    ClearCurrentHypSet();

    if ( rest == INVALIDATED_CONCEPT || rest == UNDEFINED_CONCEPT ) {
        return ;
    }
    while( rest.length() > 0 ) {
        SplitOnFirst(rest, HYPS_SEPARATOR, val_conf, rest);
        int index = AddNewHyp();
        _current_hyps[index]->FromString(val_conf);
    }
}

void CConcept::Update_NPU_AssignFromConcept(const void* update_data) {
    if ( _is_history_concept ) {
        VLOG(WARNING) << "_is_history_concept is true, " << _name;
    }
    CopyCurrentHypSetFrom(*((CConcept*)update_data));
}

void CConcept::Update_NPU_UpdateWithConcept(const void* update_data) {
    VLOG(DEBUG) << __FUNCTION__ << ", name: " << _name;
    if ( _owner_dialog_agent == NULL || _owner_dialog_agent->GetOwnerCoreAgent() == NULL ) {
        VLOG(FATAL) <<__FUNCTION__ << ", Can not get CoreAgent, _owner_dialog_agent == NULL: " 
            << (_owner_dialog_agent == NULL) << ", Name: " << _name;
        return ;
    }
    if ( _is_history_concept ) {
        VLOG(WARNING) << __FUNCTION__ << ", _is_history_concept is true, Name: " << _name;
        return ;
    }
    CConcept* concept = (CConcept*)update_data;
    CHyp* old_top_hyp = GetTopHyp();
    std::string s_old_top_hyp = "";
    if ( old_top_hyp ) {
        s_old_top_hyp = old_top_hyp->ValueToString();
    }
    TSystemActionOnConcept action = _owner_dialog_agent->GetOwnerCoreAgent()->GetSystemActionOnConcept(this);
    // 显示确认
    bool is_undergoing_explicit_confirm = ( action.sSystemAction == SA_EXPL_CONF );
    // 隐式确认
    bool is_undergoing_implicit_confirm = ( action.sSystemAction == SA_IMPL_CONF );

    if ( concept && concept->IsUpdated() ) {
        if ( _cardinality < concept->_cardinality ) {
            _cardinality = concept->_cardinality;
        }
        std::vector<float> conf1, conf2;
        // 当前所有取值的可信度的和
        float conf1_sum = 0;
        for (size_t i = 0;i < _current_hyps.size(); ++i ) {
            if ( _current_hyps[i] != NULL ) {
                conf1.push_back(_current_hyps[i]->GetConfidence());
                conf1_sum += conf1[i];
            } else {
                conf1.push_back(0);
            }
        }

        // 新的所有取值的可信度的和
        float conf2_sum = 0;
        for ( size_t i = 0;i < concept->_current_hyps.size(); ++i ) {
            if ( concept->_current_hyps[i] != NULL ) {
                conf2_sum += concept->_current_hyps[i]->GetConfidence();
            }
        }
        float unk_conf1;
        if ( _cardinality == (int)_current_hyps.size() ) {
            unk_conf1 = 0;
        } else {
            unk_conf1 = (1 - conf1_sum) / (_cardinality - _current_hyps.size());
        }
        float unk_conf2;
        if ( _cardinality == (int)concept->_current_hyps.size() ) {
            unk_conf2 = 0;
        } else {
            unk_conf2 = (1 - conf2_sum) / (_cardinality - concept->_current_hyps.size());
        }

        for ( size_t i = 0;i < _current_hyps.size(); ++i ) {
            conf2.push_back(unk_conf2);
        }
        size_t orig_set_size = _current_hyps.size();
        for ( size_t i = 0;i < concept->_current_hyps.size(); ++i ) {
            bool is_found = false;
            size_t j = 0;
            if ( concept->_current_hyps[i] != NULL ) {
                for ( j = 0;j < orig_set_size; ++j ) {
                    if ( *(concept->_current_hyps[i]) == *(_current_hyps[j])) {
                        is_found = true;
                        break;
                    }
                }
            }
            if ( is_found ) {
                conf2[j] = concept->_current_hyps[i]->GetConfidence();
            } else {
                if ( concept->_current_hyps[i] != NULL ) {
                    int index = AddNewHyp();
                    *(_current_hyps[index]) = *(concept->_current_hyps[i]);
                    conf1.push_back(unk_conf1);
                    conf2.push_back(concept->_current_hyps[i]->GetConfidence());
                } else {
                    AddNullHyp();
                    conf1.push_back(unk_conf1);
                    conf2.push_back(0);
                }
            }
        }
        for ( size_t i = 0;i < _current_hyps.size(); ++i ) {
            if ( _current_hyps[i] != NULL) {
                SetHypConfidence(i, conf1[i] * conf2[i]);
            }
        }

        float normalizer = 0;
        for ( size_t i = 0;i < conf1.size(); ++i ) {
            normalizer += conf1[i] * conf2[i];
        }
        normalizer += (_cardinality-conf1.size()) * unk_conf1 * unk_conf2;

        for ( size_t i = 0;i < _current_hyps.size(); ++i ) {
            if ( _current_hyps[i] != NULL ) {
                SetHypConfidence(i, (conf1[i] * conf2[i]) / normalizer);
            }
        }
        normalizer = 0;
        for ( size_t i = 0;i < _current_hyps.size(); ++i ) {
            if ( _current_hyps[i] != NULL ) {
                normalizer += _current_hyps[i]->GetConfidence();
            }
        }
        if ( normalizer > 1 - FREE_PROB_MASS ) {
            for ( size_t i = 0;i < _current_hyps.size(); ++i ) {
                if ( _current_hyps[i] != NULL) {
                    SetHypConfidence(i, _current_hyps[i]->GetConfidence()*(1-FREE_PROB_MASS)/normalizer);
                }
            }
        }
    }
    if ( is_undergoing_explicit_confirm ) {
        CHyp* nw_top_hyp = GetTopHyp();
        std::string s_nw_top_hyp = "";
        if ( nw_top_hyp ) {
            s_nw_top_hyp = nw_top_hyp->ValueToString();
        }
        if ( (s_old_top_hyp == s_nw_top_hyp) && (s_old_top_hyp != "")  && _owner_dialog_agent->GetOwnerCoreAgent()->GetRegistryAgent() != NULL ) {
            std::string agency_name = FormatString("/_ExplicitConfirm[%s]", GetAgentQualifiedName().c_str());
            CDialogAgent* expl_confirm_agency = ((CDialogAgent*)_owner_dialog_agent->GetOwnerCoreAgent()->GetRegistryAgent()->operator[](agency_name));
            std::string request_agent_name = FormatString("_ExplicitConfirm[$s]/RequestConfirm", GetAgentQualifiedName().c_str());
            CDialogAgent* request_confirm_agent = ((CDialogAgent*)_owner_dialog_agent->GetOwnerCoreAgent()->GetRegistryAgent()->operator[](request_agent_name));
            CConcept& confirm_concept = expl_confirm_agency->C("confirm");
            bool is_yes = confirm_concept.IsAvailableAndGrounded() && (bool)(confirm_concept);
            bool is_no = confirm_concept.IsAvailableAndGrounded() && !(bool)(confirm_concept);
            bool is_too_many_non_understandings = false;
            if ( expl_confirm_agency->HasParameter("max_attempts") ) {
                is_too_many_non_understandings = request_confirm_agent->GetTurnsInFocusCounter() >= 
                    request_confirm_agent->GetMaxExecuteCounter() - 1;
            }
            if ( is_yes ) {
                int index = GetTopHypIndex();
                SetHypConfidence(index, 1 - FREE_PROB_MASS);
                NotifyChange();
                SetExplicitlyConfirmedHyp(GetHyp(index));
                for (int h = 0;h < index; ++h ) {
                    DeleteHyp(0);
                }
                while( GetNumHyps() > 1)  DeleteHyp(1);
                SetGroundedFlag(true);
            } else if ( is_no || is_too_many_non_understandings ) {
                int index = GetTopHypIndex();
                DeleteHyp(index);
                if ( is_no ) {
                    SetExplicitlyDisconfirmedHyp(s_old_top_hyp);
                }
                SetInvalidatedFlag(!GetNumValidHyps());
            }
        }
    }

    if ( is_undergoing_implicit_confirm && IsSealed() && IsUpdated() && _owner_dialog_agent->GetOwnerCoreAgent()->GetRegistryAgent() != NULL) {
        std::string agency_name = FormatString("/_ImplicitConfirmExpect[%s]", GetAgentQualifiedName().c_str());
        CConcept& confirm_concept = ((CDialogAgent*)_owner_dialog_agent->GetOwnerCoreAgent()->GetRegistryAgent()->operator[](agency_name))->C("confirm");
        bool is_no = confirm_concept.IsAvailableAndGrounded() && !(bool)(confirm_concept);
        int index = GetTopHypIndex();

        if ( is_no ) {
            DeleteHyp(index);
            SetInvalidatedFlag(!GetNumValidHyps());
        } else {
            SetHypConfidence(index, 1 - FREE_PROB_MASS);
            for (int h = 0;h < index; ++h ) {
                DeleteHyp(0);
            }
            while( GetNumHyps() > 1 ) DeleteHyp(1);

            SetGroundedFlag(true);
        }
    }
}

void CConcept::Update_NPU_CollapseToMode(const void* update_data) {
    if ( _is_history_concept ) {
        VLOG(WARNING) << "_is_history_concept is true, " << _name;
        return ;
    }
    int index = GetTopHypIndex();
    if ( index == -1 ) {
        return ;
    }
    SetHypConfidence(index, 1-FREE_PROB_MASS);

    for ( int i = 0;i < index; ++i ) {
        DeleteHyp(0);
    }
    while( GetNumHyps() > 1) {
        DeleteHyp(1);
    }
    SetGroundedFlag(true);
}

void CConcept::Update_PartialFromString(const void* update_data) {
    if ( _is_history_concept ) {
        VLOG(WARNING) << "_is_history_concept is true, " << _name;
        return ;
    }

    std::string val_conf;
    std::string rest = *(std::string*)update_data;
    ClearPartialHypSet();

    while ( rest.length() > 0 ) {
        SplitOnFirst(rest, HYPS_SEPARATOR, val_conf, rest);
        int index = AddNewPartialHyp();
        _partial_hyps[index]->FromString(val_conf);
    }
}

void CConcept::Update_Calista_AssignFromString(const void* update_data) {
    Update_NPU_AssignFromConcept(update_data);
}

void CConcept::Update_Calista_AssignFromConcept(const void* update_data) {
    Update_NPU_AssignFromConcept(update_data);
}

// TODO
void CConcept::Update_Calista_UpdateWithConcept(const void* update_data) {
    VLOG(FATAL) << "Update_Calista_UpdateWithConcept TODO";
}

void CConcept::Update_Calista_CollapseToMode(const void* update_data) {
    if ( _is_history_concept ) {
        VLOG(WARNING) << "_is_history_concept is true, " << _name;
        return ;
    }
    int index = GetTopHypIndex();
    if ( index == -1 ) {
        return ;
    }
    for ( int i = 0;i < index; ++i ) {
        DeleteHyp(0);
    }
    while( GetNumHyps() > 1 ) {
        DeleteHyp(1);
    }
    SetGroundedFlag(true);
}

bool CConcept::IsUpdated() {
    return GetNumValidHyps() > 0;
}

bool CConcept::IsUpdatedAndGrounded() {
    return IsUpdated() && IsGrounded();
}

bool CConcept::IsAvailable() {
    if ( IsUpdated() ) {
        return true;
    }
    if ( _prev_concept ) {
        return _prev_concept->IsAvailable();
    } else {
        return false;
    }
}

bool CConcept::IsAvailableAndGrounded() {
    if ( IsUpdatedAndGrounded() ) {
        return true;
    }
    if ( _prev_concept ) {
        return _prev_concept->IsAvailableAndGrounded();
    } else {
        return false;
    }
}

bool CConcept::IsGrounded() {
    if ( _is_history_concept ) { return true; }

    if ( _grounding_model == NULL ) {
        if ( _owner_concept != NULL ) {
            return _owner_concept->IsGrounded();
        } else {
            return true;
        }
    } else {
        return _is_grounded;
    }
}

bool CConcept::HasPartialUpdate() {
    return _partial_hyps.size() > 0;
}

bool CConcept::IsUndergoingGrounding() {
    if ( _grounding_model && CGroundingManagerAgent::Instance() != NULL ) {
        return CGroundingManagerAgent::Instance()->GroundingInProgressOnConcept(this);
    } else {
        return false;
    }
}

bool CConcept::IsInvalidated() {
    return GetInvalidatedFlag();
}

bool CConcept::IsRestoredForGrounding() {
    return GetRestoredForGroundingFlag();
}

std::string CConcept::GroundedHypToString() {
    if ( IsUpdatedAndGrounded() ) {
        return TopHypToString();
    } else {
        return IsInvalidated() ? INVALIDATED_CONCEPT : UNDEFINED_CONCEPT;
    }
}

std::string CConcept::TopHypToString() {
    VLOG(DEBUG) << __FUNCTION__ << ", Name: " << _name;
    if ( IsUpdated() ) {
        return GetTopHyp()->ValueToString() + "\n";
    } else {
        return IsInvalidated() ? INVALIDATED_CONCEPT : UNDEFINED_CONCEPT;
    }
}

// TODO RETURN 
std::string CConcept::HypSetToString() {
    std::string result;

    if ( IsUpdated() ) {
        for ( size_t i = 0;i < _current_hyps.size(); ++i ) {
            result += ( _current_hyps[i] ? _current_hyps[i]->ToString() : UNDEFINED_VALUE) + HYPS_SEPARATOR;
        }
        return result.substr(0, result.length()-1) + "\n";
    } else {
        return IsInvalidated() ? INVALIDATED_CONCEPT"\n" : UNDEFINED_CONCEPT"\n";
    }

    if ( HasPartialHyp() ) {
        result += " (PARTIAL:";
        for ( size_t i = 0;i < _current_hyps.size(); ++i ) {
            result += (_partial_hyps[i] ? _partial_hyps[i]->ToString() : UNDEFINED_VALUE) + HYPS_SEPARATOR;
        }
        result = result.substr(0, result.length()-1 ) + ")";
    }
    return result + "\n";
}

TConceptType CConcept::GetConceptType() {
    return _concept_type;
}

void CConcept::SetConceptType(TConceptType concept_type) {
    _concept_type = concept_type;
}

TConceptSource CConcept::GetConceptSource() {
    return _concept_source;
}

void CConcept::SetConceptSource(TConceptSource concept_source ) {
    _concept_source = concept_source;
}

void CConcept::SetName(const std::string& name) {
    _name = name;
}

std::string CConcept::GetName() {
    return _name;
}

std::string CConcept::GetSmallName() {
    std::string foo, small_name;
    SplitOnLast(_name, ".", foo, small_name);
    return small_name;
}

std::string CConcept::GetAgentQualifiedName() {
    if ( _owner_dialog_agent != NULL ) {
        return FormatString("%s/%s", _owner_dialog_agent->GetName().c_str(), GetName().c_str());
    } else {
        return GetName();
    }
}

void CConcept::SetOwnerDialogAgent(CDialogAgent* dialog_agent) {
    _owner_dialog_agent = dialog_agent;
}

CDialogAgent* CConcept::GetOwnerDialogAgent() {
    return _owner_dialog_agent;
}

void CConcept::SetOwnerConcept(CConcept* concept) {
    _owner_concept = concept;
    if ( _prev_concept ) {
        _prev_concept->SetOwnerConcept(concept);
    }
}

CConcept* CConcept::GetOwnerConcept() {
    return _owner_concept;
}

void CConcept::CreateGroundingModel(const std::string& grounding_model_spec) {
    string sThisSpec = "";
	string sDefaultGroundingModelSpec = "";

    std::vector<std::string> vsGMSpecs;
    SplitString(grounding_model_spec, ',', &vsGMSpecs);
	for(size_t i = 0; i < vsGMSpecs.size(); i++) {
		vsGMSpecs[i] = TrimLeft(vsGMSpecs[i]);
		if((sThisSpec == "") && (vsGMSpecs[i].find('=') == -1)) {
			sThisSpec = vsGMSpecs[i];
		}
		else {
            std::string sHead, sTail;
			SplitOnFirst(vsGMSpecs[i], "=", sHead, sTail);
			if (sHead == "@default") {
				sDefaultGroundingModelSpec = sTail;
			}
			else {
				VLOG(WARNING) << "Could not create a grounding model for concept, head: " << sHead;
                return ;
			}
		}
	}
	
	// A: no specification given, fall back on the default one
	if (sThisSpec == "") {
		sThisSpec = sDefaultGroundingModelSpec;
	}
    
	if(!(CGroundingManagerAgent::Instance()->GetConfiguration().bGroundConcepts) ||
		(sThisSpec == "none") || (sThisSpec == "")) {
        _grounding_model = NULL;
    } else {
        _grounding_model = (CGMConcept *)
            CGroundingManagerAgent::Instance()->CreateGroundingModel(
			    CGroundingManagerAgent::Instance()->GetConfiguration().sConceptGM,
			    sThisSpec);
        _grounding_model->Initialize();
        _grounding_model->SetConcept(this);
    }
}

CGMConcept* CConcept::GetGroundingModel() {
    if ( _is_history_concept ) {
        VLOG(WARNING) << "_is_history_concept is true, " << _name;
        return NULL;
    }
    return _grounding_model;
}

void CConcept::SetGroundedFlag(bool grounded) {
    _is_grounded = grounded;
}

bool CConcept::GetGroundedFlag() {
    return _is_grounded;
}

void CConcept::DeclareGroundingModels(
        std::vector<CGroundingModel*>& models,
        std::set<CGroundingModel*>& exclude) {
    if ( _grounding_model &&
            exclude.find(_grounding_model) == exclude.end() ) {
        models.push_back(_grounding_model);
        exclude.insert(_grounding_model);
    }
}

void CConcept::DeclareConcepts(std::vector<CConcept*>& concepts,
        std::set<CConcept*>& exclude) {
    if ( exclude.find(this) == exclude.end() ) {
        concepts.push_back(this);
        exclude.insert(this);
    }
}

void CConcept::SetInvalidatedFlag(bool invalidated) {
    _is_invalidated = invalidated;
    if ( IsRestoredForGrounding() && _prev_concept ) {
        _prev_concept->SetInvalidatedFlag(invalidated);
        if ( invalidated ) {
            SetRestoredForGroundingFlag(false);
        }
    }
    BreakSeal();
}

bool CConcept::GetInvalidatedFlag() {
    return _is_invalidated;
}

void CConcept::SetRestoredForGroundingFlag(bool restored_for_grounding) {
    _is_restored_for_grounding = restored_for_grounding;
}

bool CConcept::GetRestoredForGroundingFlag() {
    return _is_restored_for_grounding;
}

void CConcept::Seal() {
    _is_sealed = true;
}

void CConcept::BreakSeal() {
    _is_sealed = false;
}

bool CConcept::IsSealed() {
    return _is_sealed;
}

void CConcept::DisableChangeNotification() {
    SetChangeNotification(false);
}

void CConcept::EnableChangeNotification() {
    SetChangeNotification(true);
}

void CConcept::SetChangeNotification(bool change_notification) {
    _is_change_notification = change_notification;
}

void CConcept::NotifyChange() {
    SetGroundedFlag(false);
    SetInvalidatedFlag(false);
    BreakSeal();
    MarkTurnLastUpdated();
    _conveyance = cNotConveyed;
    ClearWaitingConveyance();

    if ( _grounding_model && _is_change_notification && CGroundingManagerAgent::Instance() != NULL ) {
        CGroundingManagerAgent::Instance()->RequestConceptGrounding(this);
    }
}

CHyp* CConcept::HypFactory() {
    VLOG(FATAL) << __FUNCTION__ << ", can not call HypFactory on CConcept."; 
    return NULL;
}

int CConcept::AddHyp(CHyp* hyp) {
    _current_hyps.push_back(hyp);
    _num_valid_hyps ++;
    NotifyChange();
    return (int)(_current_hyps.size()-1);
}

int CConcept::AddNewHyp() {
    _current_hyps.push_back(HypFactory());
    _num_valid_hyps += 1;
    NotifyChange();
    return (_current_hyps.size() -1 );
}

int CConcept::AddNullHyp() {
    VLOG(DEBUG) << "WhatWhat...\tName; " << _name;
    _current_hyps.push_back(NULL);
    NotifyChange();
    return (int)(_current_hyps.size() - 1);
}

void CConcept::SetHyp(int index, CHyp* hyp) {
    SetNullHyp(index);
    if ( !hyp ) {
        return ;
    }
    _current_hyps[index] = HypFactory();
    *(_current_hyps[index]) = *hyp;
    _num_valid_hyps += 1;
    NotifyChange();
}

void CConcept::SetNullHyp(int index) {
    if ( _current_hyps[index] != NULL ) {
        delete _current_hyps[index];
        _current_hyps[index] = NULL;
        _num_valid_hyps -= 1;
        NotifyChange();
    }
}

void CConcept::DeleteHyp(int index) {
    if ( _current_hyps[index] != NULL ) {
        delete _current_hyps[index];
        _num_valid_hyps -= 1;
    }
    _current_hyps.erase(_current_hyps.begin() + index);
    NotifyChange();
}

CHyp* CConcept::GetHyp(int index) {
    if ( index >= 0 && index < _current_hyps.size() ) {
        return _current_hyps[index];
    } else {
        VLOG(WARNING) << "index error, " << index;
        return NULL;
    }
}

int CConcept::GetHypIndex(CHyp* hyp) {
    for ( size_t i = 0;i < _current_hyps.size(); ++i ) {
        if ( (_current_hyps[i] != NULL) && (*hyp == *(_current_hyps[i])) ) {
            return i;
        }
    }
    return -1;
}

float CConcept::GetHypConfidence(int index) {
    CHyp* hyp = GetHyp(index);
    return hyp ? hyp->GetConfidence() : 0;
}

void CConcept::SetHypConfidence(int index, float confidence) {
    CHyp* hyp = GetHyp(index);
    if ( hyp ) {
        if ( hyp->GetConfidence() != confidence ) {
            hyp->SetConfidence(confidence);
            NotifyChange();
        }
    } else {
        VLOG(WARNING) << "hyp is NULL, index: " << index;
    }
}

CHyp* CConcept::GetTopHyp() {
    int top_hyp_index = GetTopHypIndex();
    if ( top_hyp_index == -1 ) {
        return NULL;
    } else {
        return GetHyp(top_hyp_index);
    }
}

int CConcept::GetTopHypIndex() {
    if ( _num_valid_hyps == 0 ) {
        return -1;
    }
    float max_confidence = 0;
    int selected = -1;

    for ( size_t i = 0;i < _current_hyps.size(); ++i ) {
        if ( _current_hyps[i] != NULL ) {
            if ( _current_hyps[i]->GetConfidence() > max_confidence ) {
                max_confidence = _current_hyps[i]->GetConfidence();
                selected = i;
            }
        }
    }
    return selected;
}

int CConcept::Get2ndHypIndex() {
    if ( _num_valid_hyps < 2 ) {
        return -1;
    }
    int top_hyp_index = GetTopHypIndex();
    float max_confidence = 0;
    int selected = -1;
    for ( size_t i = 0;i < _current_hyps.size(); ++i ) {
        if ( ( i != top_hyp_index ) && _current_hyps[i] != NULL ) {
            if ( _current_hyps[i]->GetConfidence() > max_confidence ) {
                max_confidence = _current_hyps[i]->GetConfidence();
                selected = i;
            }
        }
    }
    return selected;
}

float CConcept::GetTopHypConfidence() {
    CHyp* top_hyp = GetTopHyp();
    if ( top_hyp == NULL ) {
        return 0.0;
    } else {
        return top_hyp->GetConfidence();
    }
}

bool CConcept::IsValidHyp(int index) {
    return GetHyp(index) != NULL;
}

int CConcept::GetNumHyps() {
    return (int)_current_hyps.size();
}

int CConcept::GetNumValidHyps() {
    return _num_valid_hyps;
}

void CConcept::ClearCurrentHypSet() {
    VLOG(DEBUG) << __FUNCTION__ << ", Name: " << _name;
    if ( _current_hyps.size() == 0 ) {
        return ;
    }
    for ( size_t i = 0;i < _current_hyps.size(); ++i ) {
        if ( _current_hyps[i] != NULL ) {
            delete _current_hyps[i];
            _current_hyps[i] = NULL;
        }
    }
    _current_hyps.clear();
    _num_valid_hyps = 0;
    NotifyChange();
}


void CConcept::CopyCurrentHypSetFrom(CConcept& concept) {
    VLOG(DEBUG) << __FUNCTION__ << ", Name: " << _name;
    ClearCurrentHypSet();
    for ( size_t i = 0;i < concept.GetNumHyps(); ++i ) {
        CHyp* hyp;
        if ( (hyp = concept.GetHyp(i) ) != NULL ) {
            AddNewHyp();
            *(_current_hyps[i]) = *hyp;
        } else {
            AddNullHyp();
        }
    }
    _num_valid_hyps = concept._num_valid_hyps;
    _explictly_confirmed_hyp = concept.GetExplicitlyConfirmedHypAsString();
    _explicitly_disconfirmed_hyp = concept.GetExplicitlyDisconfirmedHypAsString();
}

void CConcept::SetCardinality(int cardinality) {
    _cardinality = cardinality;
}

int CConcept::GetCardinality() {
    return _cardinality;
}

float CConcept::GetPriorForHyp(CHyp* hyp) {
    if ( CGroundingManagerAgent::Instance() != NULL ) {
        float prior = CGroundingManagerAgent::Instance()->GetPriorForConceptHyp(
                GetSmallName(), hyp ? hyp->ValueToString() : "<UNDEFINED>");
        if ( prior != -1 ) {
            return prior;
        }
    } 
    return (float)1.0 / (_cardinality + 1);
}

float CConcept::GetConfusabilityForHyp(CHyp* hyp) {
    if ( CGroundingManagerAgent::Instance() != NULL ) {
        float confusability = CGroundingManagerAgent::Instance()->GetConfusabilityForConceptHyp(
                GetSmallName(), hyp ? hyp->ValueToString() : "<UNDEFINED>");
        if ( confusability != -1 ) {
            return confusability;
        }
    }
    return 0.5;
}

std::string CConcept::GetConceptTypeInfo() {
    if ( CGroundingManagerAgent::Instance() != NULL ) {
        return CGroundingManagerAgent::Instance()->GetConceptTypeInfoForConcept(GetSmallName());
    } else {
        return "";
    }
}

void CConcept::SetExplicitlyConfirmedHyp(CHyp* hyp) {
    SetExplicitlyConfirmedHyp(hyp->ValueToString());
}

void CConcept::SetExplicitlyConfirmedHyp(const std::string& confirmed) {
    _explictly_confirmed_hyp = confirmed;
}

// D: set the explicitly disconfirmed hyp
void CConcept::SetExplicitlyDisconfirmedHyp(CHyp* hyp) {
    SetExplicitlyDisconfirmedHyp(hyp->ValueToString());
}

// D: alternate function for settting the explicitly disconfirmed hyp
void CConcept::SetExplicitlyDisconfirmedHyp(const std::string sAExplicitlyDisconfirmedHyp) {
    _explicitly_disconfirmed_hyp = sAExplicitlyDisconfirmedHyp;
}

// D: return the explicitly confirmed hyp
std::string CConcept::GetExplicitlyConfirmedHypAsString() {
    return _explictly_confirmed_hyp;
}

// D: return the explicitly confirmed hyp
std::string CConcept::GetExplicitlyDisconfirmedHypAsString() {
    return _explicitly_disconfirmed_hyp;
}

// D: clears the explicitly confirmed hyp
void CConcept::ClearExplicitlyConfirmedHyp() {
    _explictly_confirmed_hyp = "";
}

// D: clears the explicitly confirmed hyp
void CConcept::ClearExplicitlyDisconfirmedHyp() {
    _explicitly_disconfirmed_hyp = "";
}

// A: adds a partial hypothesis to the current set of partial hypotheses
int CConcept::AddPartialHyp(CHyp* hyp) {
    _partial_hyps.push_back(hyp);
    _num_valid_partial_hyps += 1;
    return (int)(_partial_hyps.size() - 1);
}

// A: adds a new partial hypothesis to the current set of partial hypotheses
int CConcept::AddNewPartialHyp() {
    _partial_hyps.push_back(HypFactory());
    _num_valid_partial_hyps += 1;
    return (int)(_partial_hyps.size() - 1);
}

// A: adds a null hypothesis to the current set of partial hypotheses
int CConcept::AddNullPartialHyp() {
    _partial_hyps.push_back(NULL);
    return (int)(_partial_hyps.size() - 1);
}

// A: indicates whether a partial hypothesis is currently available 
bool CConcept::HasPartialHyp() {
    return _num_valid_partial_hyps > 0;
}

// A: returns one partial hypothesis
CHyp* CConcept::GetPartialHyp(int index) {
    if ( (index < 0 ) || (index >= (int)_partial_hyps.size()) ) {
        VLOG(WARNING) << "index out of bounds in GetPartialHyp, index: " << index << " _partial_hyps.size(): " << _partial_hyps.size();
        return NULL;
    }
    return _partial_hyps[index];
}

// return the top partial hypothesis
CHyp* CConcept::GetTopPartialHyp() {
    int top_partial_hyp_index = GetTopPartialHypIndex();
    if ( top_partial_hyp_index == -1 ) {
        return NULL;
    } else {
        return GetPartialHyp(top_partial_hyp_index);
    }
}

// A: return the top partial hyp index
int CConcept::GetTopPartialHypIndex() {
    if ( _num_valid_partial_hyps == 0 ) {
        return -1;
    }
    float max_confidence = 0;
    int selected = -1;
    for ( size_t h = 0;h < _partial_hyps.size(); ++h ) {
        if ( _partial_hyps[h] != NULL ) {
            if ( _partial_hyps[h]->GetConfidence() > max_confidence ) {
                max_confidence = _partial_hyps[h]->GetConfidence();
                selected = h;
            }
        }
    }
    return selected;
}

// A: return the confidence score of the top partial hypothesis
float CConcept::GetTopPartialHypConfidence() {
    CHyp* top_partial_hyp = GetTopPartialHyp();
    if ( top_partial_hyp == NULL ) {
        return 0;
    } else {
        return top_partial_hyp->GetConfidence();
    }
}

// A: check if a partial hypothesis is valid (confidence score > 0; 
// valus is not NULL)
bool CConcept::IsValidPartialHyp(int iIndex) {
    return GetPartialHyp(iIndex) != NULL;
}

// A: return the total number of partial hypotheses for a concept 
// (including NULLs)
int CConcept::GetNumPartialHyps() {
    return (int)_partial_hyps.size();
}

// A: return the number of valid partial hypotheses for a concept
int CConcept::GetNumValidPartialHyps() {
    return _num_valid_partial_hyps;
}
// A: clears the current partial value of the concept
void CConcept::ClearPartialHypSet() {
	// reset the partial hyp set
	// go through all the valconfs and deallocate them
	for(int h = 0; h < (int)_partial_hyps.size(); h++) {
		if(_partial_hyps[h] != NULL) 
			delete _partial_hyps[h];
	}
	_partial_hyps.clear();
	// finally, reset the number of valid hypotheses
	_num_valid_partial_hyps = 0;
}


//-----------------------------------------------------------------------------
// Methods providing access to turn last updated information
//-----------------------------------------------------------------------------

// D: Set the turn the concept was last updated
void CConcept::SetTurnLastUpdated(int iTurn) {
    _turn_last_updated = iTurn;
}

// D: Mark now as the turn in which the concept was last updated
void CConcept::MarkTurnLastUpdated() {
    VLOG(DEBUG) << __FUNCTION__ << ", ConceptName: " << _name;
    if ( _owner_dialog_agent == NULL || _owner_dialog_agent->GetOwnerCoreAgent() == NULL ) {
        VLOG(FATAL) << __FUNCTION__ << ", Can not get core_agent, _owner_dialog_agent == NULL: " << (_owner_dialog_agent == NULL);
        return ;
    }
    SetTurnLastUpdated(_owner_dialog_agent->GetOwnerCoreAgent()->GetLastInputTurnNumber());
}

// D: Access to turn in which the concept was last updated information
int CConcept::GetTurnLastUpdated() {
    return _turn_last_updated;
}

// D: returns the number of turns that have elapsed since the concept was 
//    last updated
int CConcept::GetTurnsSinceLastUpdated() {
    if ( _owner_dialog_agent == NULL || _owner_dialog_agent->GetOwnerCoreAgent() == NULL ) {
        VLOG(FATAL) << __FUNCTION__ << ", Can not get core_agent, _owner_dialog_agent == NULL: " << (_owner_dialog_agent == NULL);
        return -1;
    }
    return _owner_dialog_agent->GetOwnerCoreAgent()->GetLastInputTurnNumber() - GetTurnLastUpdated();
}

//-----------------------------------------------------------------------------
// Methods providing access to conveyance information
//-----------------------------------------------------------------------------

// D: sets the waiting for conveyance flag
void CConcept::SetWaitingConveyance() {
    _is_waiting_conveyance = true;
}

// D: clear the waiting for conveyance flag
void CConcept::ClearWaitingConveyance() {
    if ( _is_waiting_conveyance ) {
        _is_waiting_conveyance = false;
        /*
        if ( COutputManagerAgent::Instance() != NULL ) {
            //COutputManagerAgent::Instance()->CancelConceptNotificationRequest(this);
        }
        */
    }
}

// A: set the conveyance information
void CConcept::SetConveyance(TConveyance cAConveyance) {
	_conveyance = cAConveyance;
}

// A: return the conveyance information
TConveyance CConcept::GetConveyance() {
	return _conveyance;
}

// D: clear the concept notification pointer
void CConcept::ClearConceptNotificationPointer() {
    /*
    if ( COutputManagerAgent::Instance() != NULL ) {
        //COutputManagerAgent::Instance()->ChangeConceptNotificationPointer(this, NULL);
    }
    */
}

//-----------------------------------------------------------------------------
// Methods for concept history manipulation
//-----------------------------------------------------------------------------

// D: reopens the concept (i.e. moves current value into history, and starts
//    with a clean new value
void CConcept::ReOpen() {
    if ( _is_history_concept ) {
        VLOG(WARNING) << "Cannot perform ReOen on concept, " << _name;
        return ;
    }
    std::string initial_value;
    if ( _grounding_model ) {
        initial_value = TrimRight(HypSetToString());
    }
    CConcept* concept = Clone(false);
    concept->Update(CU_COLLAPSE_TO_MODE, NULL);
    concept->SetHistoryConcept(true);
    concept->_prev_concept = _prev_concept;
    _prev_concept = concept;
    ClearCurrentHypSet();
}

// D: restores the concept (i.e. restores the concept to a previous incarnation
//    from its history
void CConcept::Restore(int index) {
    if ( _is_history_concept ) {
        VLOG(WARNING) << "Cannot perform Restore on Concept, " << _name;
        return ;
    }
    std::string initial_value;
    if ( _grounding_model ) {
        initial_value = TrimRight(HypSetToString());
    }
    if ( index == 0 ) {
        return ;
    }
    CConcept* concept = &(operator[](index));
    CopyCurrentHypSetFrom(*concept);
    SetGroundedFlag(true);
    ClearHistory();
}

// D: clears the history of the current concept
void CConcept::ClearHistory() {
    if ( _is_history_concept ) {
        VLOG(WARNING) << "Cannot perform ClearHistory on Concept.";
        return ;
    }

    if ( _prev_concept != NULL ) {
        delete _prev_concept;
        _prev_concept = NULL;
    }
}

// D: merges the history on the concept, and returns a new concept containing 
//    that 
CConcept* CConcept::CreateMergedHistoryConcept() {
    if ( IsUpdated() || IsInvalidated() ) {
        CConcept* merged_history = Clone(false);
        merged_history->SetHistoryConcept(false);
        return merged_history;
    } else {
        if ( _prev_concept == NULL ) {
            return NULL;
        } else {
            return _prev_concept->CreateMergedHistoryConcept();
        }
    }
}

// D: merges the history of the concept into the current value
void CConcept::MergeHistory() {
    std::string initial_value;
    if ( _grounding_model ) {
        initial_value = TrimRight(HypSetToString());
    }
    if ( IsUpdated() || IsInvalidated() ) {
        ClearHistory();
        SetInvalidatedFlag(false);
        SetRestoredForGroundingFlag(false);
    } else {
        CConcept* merged_history_concept = CreateMergedHistoryConcept();

        if ( merged_history_concept ) {
            CopyCurrentHypSetFrom(*merged_history_concept);
            SetGroundedFlag(merged_history_concept->GetGroundedFlag());
            delete merged_history_concept;
        }

        ClearHistory();
    }
}

// D: returns the size of the history on the concept
int CConcept::GetHistorySize() {
    if ( _prev_concept == NULL ) {
        return 0;
    } else {
        return _prev_concept->GetHistorySize() + 1;
    }
}

// D: returns a certain historical version of a concept
CConcept& CConcept::GetHistoryVersion(int iIndex) {
    // just redirect through the operator
    return operator [] (iIndex);
}

// D: set the history concept flag
void CConcept::SetHistoryConcept(bool bAHistoryConcept) {
	_is_history_concept = bAHistoryConcept;
}

// D: get the history concept flag
bool CConcept::IsHistoryConcept() {
	return _is_history_concept;
}

// D: getsize method: 
int CConcept::GetSize() {
    VLOG(WARNING) << "GetSize() Cannot be called on concept, name: " << _name;
	return -1;
}

// D: DeleteAt method
void CConcept::DeleteAt(size_t index) {
    VLOG(WARNING) << "DeleteAt Cannot be called on concept.";
}

// J: InsertAt method
void CConcept::InsertAt(size_t index, CConcept &concept) {
    VLOG(WARNING) << "InsertAt Cannot be called on concept.";
}


}
