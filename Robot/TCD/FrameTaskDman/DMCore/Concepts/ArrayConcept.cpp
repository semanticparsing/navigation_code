#include "ArrayConcept.h"
#include "Robot/TCD/FrameTaskDman/DMCore/DMCore.h"

namespace TrioTDM {

CArrayConcept::CArrayConcept(const std::string& name, TConceptSource source): 
	CConcept(name, source) {
	_concept_type = ctArray;
	_is_grounded = true;
	_cardinality = -1;
	_element_grounding_model_spec = "";
}

// D: destructor
CArrayConcept::~CArrayConcept() {
	for(size_t i = 0; i < _concept_array.size(); i++) {
		if(_concept_array[i] != NULL) 
			delete _concept_array[i];
	}
    _concept_array.clear();

    if(_prev_concept != NULL)
        delete _prev_concept;
	_prev_concept = NULL;
	if(_grounding_model != NULL) {
		CGroundingManagerAgent::Instance()->RemoveConceptGroundingRequest(this);
        delete _grounding_model;
	}
	_grounding_model = NULL;
    if(_is_waiting_conveyance) ClearWaitingConveyance();
    ClearConceptNotificationPointer();
}


bool CArrayConcept::operator == (CConcept& rAConcept) {

	if (_concept_array.size() != (unsigned)rAConcept.GetSize())
		return false;
	
	for(size_t i=0; i<_concept_array.size(); i++)
		if(*_concept_array[i] != rAConcept[i])
			return false;

	return true;
}

// D: Comparison operator
bool CArrayConcept::operator < (CConcept& rAConcept) {
    VLOG(WARNING) << "Comparison operator < called on array concept.";
    return false;
}

// D: Comparison operator
bool CArrayConcept::operator > (CConcept& rAConcept) {
    VLOG(WARNING) << "Comparison operator > called on array concept.";
    return false;
}

// D: Comparison operator
bool CArrayConcept::operator <= (CConcept& rAConcept) {
    VLOG(WARNING) << "Comparison operator <= called on array concept.";
    return false;
}

// D: Comparison operator
bool CArrayConcept::operator >= (CConcept& rAConcept) {
    VLOG(WARNING) << "Comparison operator >= called on array concept.";
    return false;
}


CConcept& CArrayConcept::operator [](int index) {
    if( index < 0 ) {
        if( _prev_concept != NULL ) {
            if(index == -1) {
                return *_prev_concept;
            } else {
                return _prev_concept->operator [](index + 1);
            }
        } else {
            VLOG(WARNING) << __FUNCTION__ << ", _prev_concept is NULL, Index: " << index << ", Name: " << _name;
	        return NULLConcept;	
        }
    } 

	if(index < (int)_concept_array.size()) {
		return (*_concept_array[index]);
	} 
       
    if( _is_history_concept ) {
        VLOG(WARNING) << __FUNCTION__ << ", Cannot perform, _is_history_concept is true, Name: " << _name;
        return NULLConcept;
    }

	int len = _concept_array.size();
	for(int i=0; i < index - len + 1; i++) {
		CConcept* pAConcept = CreateElement();
		pAConcept->SetChangeNotification(_is_change_notification);
		pAConcept->SetName(FormatString("%s.%d", _name.c_str(), i + len));
		_concept_array.push_back(pAConcept);
	}
	return (*_concept_array[index]);
}

CConcept& CArrayConcept::operator [](const std::string& s_index) {
	if( s_index == "" ) {
		return *this;
	} else {
        std::string base_concept, rest;
		SplitOnFirst(s_index, ".", base_concept, rest);

		int index;
		if( sscanf(base_concept.c_str(), "%d", &index) != 1 ) {
            VLOG(WARNING) << __FUNCTION__ << ", Not Int, s_index: " << s_index;
			return NULLConcept;
		}

		return operator[](index).operator [](rest);
	}
}

// D: Clones the array concept
CConcept* CArrayConcept::Clone(bool bCloneHistory) {

	// start with an empty clone
	CArrayConcept* pConcept = (CArrayConcept *)EmptyClone();

	// set the other members
    pConcept->SetConceptType(_concept_type);
    pConcept->SetConceptSource(_concept_source);
    pConcept->_name = _name;
    pConcept->_owner_dialog_agent = _owner_dialog_agent;
	pConcept->SetOwnerConcept(_owner_concept);
    // a clone (and arrays for that matter) has no grounding model
	pConcept->_grounding_model = NULL;
	pConcept->_element_grounding_model_spec = "";
	pConcept->_is_sealed = _is_sealed;
	// a clone does not notify of changes
	pConcept->DisableChangeNotification();
	// now clone the contents of the array
	for(size_t i = 0; i < _concept_array.size(); i++) {
		pConcept->_concept_array.push_back(_concept_array[i]->Clone());
		// and reassign the owner concept 
		pConcept->_concept_array.back()->SetOwnerConcept(pConcept);
	}
	// and set the grounded flag
	pConcept->SetGroundedFlag(_is_grounded);
    pConcept->SetTurnLastUpdated(_turn_last_updated);
    pConcept->_conveyance = _conveyance;
	// a clone does not wait for conveyance
    pConcept->_is_waiting_conveyance = false;
    pConcept->SetHistoryConcept(_is_history_concept);
	if(bCloneHistory && (_prev_concept != NULL)) 
		pConcept->_prev_concept = _prev_concept->Clone();
	else
        pConcept->_prev_concept = NULL;

	// finally, return the clone
    return pConcept;
}

//-----------------------------------------------------------------------------
// Overwritten protected virtual methods implementing various types of updates
//-----------------------------------------------------------------------------

// D: Naive probabilistic update scheme - assign from string
void CArrayConcept::Update_NPU_AssignFromString(const void* pUpdateData) {

	// check that it's not a history concept
    if(_is_history_concept)
        VLOG(WARNING) << (FormatString("Cannot perform (AssignFromString) update on concept "\
            "(%s) history.", _name.c_str()));

	// o/w 
    string sString = *(string *)pUpdateData;
    string sWorkingString = TrimLeft(sString);

	// first clear the current contents of the array
    ClearCurrentHypSet();

	// the array concept should start with ":size {"
	string sStart;
	if(!SplitOnFirst(sWorkingString, "{", sStart, sWorkingString)) {
		// fail the whole process
		VLOG(WARNING) << (FormatString(
            "Cannot perform conversion to <CArrayConcept>"\
			" %s from string (dump below).\n%s",
			_name.c_str(), sString.c_str()));
	}

	// the sStart string should contain ":size"
	size_t uiSize;
	if(sscanf(sStart.c_str(), ":%lu", &uiSize) != 1) {
		// fail the whole process
		VLOG(WARNING) << (FormatString(
            "Cannot perform conversion to <CArrayConcept>"\
			" %s from string (dump below).\n%s",
			_name.c_str(), sString.c_str()));
	}

	// find the corresponding closing bracket
	size_t uiPos = FindClosingQuoteChar(sWorkingString, 0, '{', '}');

	// skip tabs/spaces/newlines between the concept
	// and the confidence separator
	while((uiPos < sWorkingString.length() - 1)&&
			((sWorkingString[uiPos] == '\n')||
			(sWorkingString[uiPos] == '\t')||
			(sWorkingString[uiPos] == ' '))) {
		uiPos++;	
	}

	// check if there's anything left after iCount (that should be the 
    // confidence score)
	string sConfidence;
	if(uiPos < sWorkingString.length() - 1) {
    	sConfidence = sWorkingString.substr(uiPos + 1, 
			sWorkingString.length() - uiPos - 1);
		float fConfidence = (float)atof(sConfidence.c_str());
        ((void)fConfidence);
		if(errno == ERANGE) {
			// fail the whole process if we can't find a valid number for confidence
			VLOG(WARNING) << (FormatString(
                "Cannot perform update to <CArrayConcept> "\
				"%s from string (dump below).\n%s", _name.c_str(),
				sString.c_str()));
		}
	}

	// now go through all the items and push them into the array
	size_t uiElementCount = 0;
	string sElementValue;
	sWorkingString = TrimLeft(sWorkingString);
	while((sWorkingString.length() > 0)&&(sWorkingString[0] != '}')) {
		if (sWorkingString[0] != '{') {
			// the element is atomic get the whole line
			string sStart;
			
			// if we cannot find a newline, signal an error
			if(!SplitOnFirst(sWorkingString, "\n", sStart, sWorkingString)) {
				VLOG(WARNING) << (FormatString(
                    "Cannot perform conversion to <CArrayConcept>"\
					" %s from string (dump below).\n%s",
					_name.c_str(), sString.c_str()));
			}

			// get the string corresponding to one element
			sElementValue = Trim( sStart);
		
			// and set the rest of the working string
			sWorkingString = TrimLeft(sWorkingString);
		} else {			
			// the element is a structure, it starts with a '{'

			// identify where the corresponding closing brace is 
			size_t iCount = 
                FindClosingQuoteChar(sWorkingString, 1, '{', '}');
	    
			// if we ran out of the string, signal an error
			if(iCount >= sWorkingString.length()) {
				VLOG(WARNING) << (FormatString(
                    "Cannot perform conversion to <CArrayConcept>"\
					" %s from string (dump below).\n%s",
					_name.c_str(), sString.c_str()));
			}

			// get the string corresponding to one element
			sElementValue = sWorkingString.substr(1, iCount - 2);
		
			// and set the rest of the working string
			sWorkingString = TrimLeft(sWorkingString.substr(iCount + 1, 
                sWorkingString.length() - iCount - 1));
		}

		// create an element of the array
		CConcept* pElement = CreateElement();
		// set the notifications flag
		pElement->SetChangeNotification(_is_change_notification);

        // set its value accordingly 
		if((pElement->GetConceptType() == ctStruct) ||
            (pElement->GetConceptType() == ctFrame)) {
            sElementValue = "{" + sElementValue + "}";
            if(sConfidence != "") 
                sElementValue = sElementValue +
                    VAL_CONF_SEPARATOR + sConfidence;
			pElement->Update(CU_ASSIGN_FROM_STRING, &sElementValue);
		} else {
            if(sConfidence != "") 
                sElementValue = sElementValue + 
                    VAL_CONF_SEPARATOR + sConfidence;
			pElement->Update(CU_ASSIGN_FROM_STRING, &sElementValue);
		}

		// set its name accordingly
		pElement->SetName(FormatString("%s.%d", _name.c_str(), 
            uiElementCount));
        // set its owner dialog agent
        pElement->SetOwnerDialogAgent(_owner_dialog_agent);
        // set its owner concept
        pElement->SetOwnerConcept(_owner_concept);

        // and push it into the array
		_concept_array.push_back(pElement);

        // finally, increase the element count
		uiElementCount++;
	}

	// and all we have left now should be the '}'
	if(sWorkingString[0] != '}') {
		// fail the whole process
		VLOG(WARNING) << (FormatString(
            "Cannot perform conversion to <CArrayConcept>"\
			" %s from string (dump below).\n%s",
			_name.c_str(), sString.c_str()));
	}

	// and the number of elements should be equal to size
	if(uiSize != uiElementCount) {
		// fail the conversion, but issue a warning
		VLOG(WARNING) << (FormatString(
            "Specified array size (%d) does not match number of"\
			" existing elements (%d) in conversion to <CArrayConcept> "\
			"%s from string (dump below).\n%s",
			uiSize, uiElementCount, _name.c_str(), sString.c_str()));
	}

    // notify the change
    NotifyChange();
}

// D: Naive probabilistic update scheme - perform update with another concept
void CArrayConcept::Update_NPU_UpdateWithConcept(const void* pUpdateData) {
	VLOG(WARNING) << (FormatString(
		"UpdateWithConcept call on array concept %s failed (array concepts "
        "do not support hypsets).", _name.c_str()));
}

// D: Update the value of a concept by collapsing it to the mode
void CArrayConcept::Update_NPU_CollapseToMode(const void* pUpdateData) {
    // call it for all the elements of the array
    for(int i = 0; i < GetSize(); i++) 
        _concept_array[i]->Update(CU_COLLAPSE_TO_MODE, pUpdateData);
}

//-----------------------------------------------------------------------------
// Overwritten protected virtual methods implementing various types of updates
// for the calista belief updating scheme
//-----------------------------------------------------------------------------

// D: Naive probabilistic update scheme - perform update with another concept
void CArrayConcept::Update_Calista_UpdateWithConcept(const void* pUpdateData) {
	VLOG(WARNING) << (FormatString(
		"UpdateWithConcept call on array concept %s failed (array concepts "
        "do not support hypsets).", _name.c_str()));
}

// D: Update the value of a concept by collapsing it to the mode
void CArrayConcept::Update_Calista_CollapseToMode(const void* pUpdateData) {
    // call it for all the elements of the array
    for(int i = 0; i < GetSize(); i++) 
        _concept_array[i]->Update(CU_COLLAPSE_TO_MODE, pUpdateData);
}

//-----------------------------------------------------------------------------
// Overwritten methods implementing various flags (state information) on the 
// concept
//-----------------------------------------------------------------------------

// D: returns true if the array is updated, i.e. its size is > 0
bool CArrayConcept::IsUpdated() {
	return GetSize() > 0;
}

bool CArrayConcept::IsUndergoingGrounding() {
	for(size_t i=0; i<_concept_array.size(); i++) {
		if(_concept_array[i]->IsUndergoingGrounding()) {
            return true;
        }
	}
	return false;
}

string CArrayConcept::GroundedHypToString() {
    if ( IsUpdatedAndGrounded() ) {
    	string sResult = FormatString(":%d\n{\n", _concept_array.size());
    	for(size_t i=0; i < _concept_array.size(); i++) {
	    	sResult += _concept_array[i]->GroundedHypToString();
    	}
	    sResult += "}\n";
    	return sResult;
    } else {
        return IsInvalidated() ? INVALIDATED_CONCEPT : UNDEFINED_CONCEPT;
    }
}

string CArrayConcept::TopHypToString() {
	string sResult = FormatString(":%d\n{\n", _concept_array.size());
	for(size_t i=0; i < _concept_array.size(); i++) {
		sResult += _concept_array[i]->TopHypToString();
	}
	sResult += "}\n";
	return sResult;
}

// D: conversion to value/conf;value/conf... format
string CArrayConcept::HypSetToString() {
	// the string holding the result
	string sResult = FormatString(":%d\n{\n", _concept_array.size());

	// go through the items and add them to the string
	for(size_t i=0; i < _concept_array.size(); i++) {
		sResult += _concept_array[i]->HypSetToString();
	}

	// and finally add the closing braces and return 
	sResult += "}\n";
	return sResult;
}

//-----------------------------------------------------------------------------
// Overwritten methods providing access to concept name
//-----------------------------------------------------------------------------

// D: set the concept name
void CArrayConcept::SetName(const std::string& sAName) {
    // call the inherited
    CConcept::SetName(sAName);
	// then set the name for all subitems
    for(size_t i=0; i < _concept_array.size(); i++) {
        _concept_array[i]->SetName(FormatString("%s.%d", 
            _name.c_str(), i));
    }
}

//-----------------------------------------------------------------------------
// Overwritten methods providing access to the owner dialog agent
//-----------------------------------------------------------------------------
    
// set the owner dialog agent
void CArrayConcept::SetOwnerDialogAgent(CDialogAgent* pADialogAgent) {
	// call the inherited
	CConcept::SetOwnerDialogAgent(pADialogAgent);
	// then set the owner an all subitems
    for(size_t i=0; i < _concept_array.size(); i++) {
        _concept_array[i]->SetOwnerDialogAgent(pADialogAgent);
    }
}

// D: Create a grounding model
void CArrayConcept::CreateGroundingModel(const std::string& sGroundingModelSpec) {
	// partition the spec by commas
    std::vector<std::string> vsGMSpecs;
    SplitString(sGroundingModelSpec, ',', &vsGMSpecs);
	// and trim it
	string sThisSpec = "";
	for(size_t i = 0; i < vsGMSpecs.size(); i++) {
		vsGMSpecs[i] = TrimLeft(vsGMSpecs[i]);
		// check if it's for the current item
		if((sThisSpec == "") && (vsGMSpecs[i].find('=') == -1)) {
			sThisSpec = vsGMSpecs[i];
		}
	}

	// create the grounding model for this array
	if(!(CGroundingManagerAgent::Instance()->GetConfiguration().bGroundConcepts) || 
		(sThisSpec == "none") || (sThisSpec == "")) {
        _grounding_model = NULL;
    } else {
        VLOG(WARNING) << (FormatString(
			"Could not create a grounding model for array concept"
			" %s. Array concepts cannot undergo grounding.",
			GetAgentQualifiedName().c_str()));
		return;
    }

	// A: look for a default element grounding model for this frame
	//    if several @default appear, use the last one (this happens
	//    when an array inherits the default grounding model of its 
	//    parent frame/array but the user also defines the child's 
	//    default)
	string sDefaultGroundingModelSpec = "";
	for(size_t i = 0; i < vsGMSpecs.size(); i++) {
		string sHead, sRest;
		SplitOnFirst(vsGMSpecs[i], " =.", sHead, sRest);
		if(sHead == "@default") {
			sRest = TrimLeft(sRest);
			if ((sRest.length()>0)&&(sRest[0]=='=')) {
				sDefaultGroundingModelSpec = sRest.substr(1,sRest.length()-1);
			}
		}
	}

	// now go through each of the specs
	string _element_grounding_model_spec = "";

	// A: propagate the default grounding model to the elements
	if (sDefaultGroundingModelSpec != "") {
		_element_grounding_model_spec = "@default=" + sDefaultGroundingModelSpec + ", ";
	}

	for(size_t i = 0; i < vsGMSpecs.size(); i++) {
		if((vsGMSpecs[i] != "") && (vsGMSpecs[i][0] == '#')) {
			string sReminder = 
				vsGMSpecs[i].substr(1, vsGMSpecs[i].length() - 1);
			if((sReminder[0] == '.') || (sReminder[0] == '=')) {
				_element_grounding_model_spec += 
					sReminder.substr(1, sReminder.length() - 1) + ", ";
			} else {
				VLOG(WARNING) << (FormatString(
					"Cannot create grounding model on concept %s. Specification (%s)"\
					" cannot be parsed.", GetAgentQualifiedName().c_str(), 
					sGroundingModelSpec.c_str()));
			}
		}
	}

	// finally, trim the end of the string
	_element_grounding_model_spec = TrimRight(_element_grounding_model_spec, ", ");

	// and create the grounding models on all subelements
	for(size_t i = 0; i < _concept_array.size(); i++)
		_concept_array[i]->CreateGroundingModel(_element_grounding_model_spec);
}

// D: Declare the grounding models subsumed by this concept
void CArrayConcept::DeclareGroundingModels(
    std::vector<CGroundingModel*>& rgmpvModels,
    std::set<CGroundingModel*>& rgmpsExclude) {

    // first call the inherited method
    CConcept::DeclareGroundingModels(rgmpvModels, rgmpsExclude);

    // then add the grounding models for the subsumed concepts
    for(int i = 0; i < GetSize(); i++) 
        _concept_array[i]->DeclareGroundingModels(rgmpvModels, rgmpsExclude);
}

// D: Declare the subsumed concepts
void CArrayConcept::DeclareConcepts(
    std::vector<CConcept*>& rcpvConcepts, 
    std::set<CConcept*>& rcpsExclude) {

    // just add the current concept
	CConcept::DeclareConcepts(rcpvConcepts, rcpsExclude);

    // then go through all the items and have them declare the concepts, too
    for(int i = 0; i < GetSize(); i++) 
        _concept_array[i]->DeclareConcepts(rcpvConcepts, rcpsExclude);
}

//-----------------------------------------------------------------------------
// Methods related to signaling concept changes
//-----------------------------------------------------------------------------

// D: Set the concept change notifications flag
void CArrayConcept::SetChangeNotification(
	bool bAChangeNotification) {
	_is_change_notification = bAChangeNotification;
	// and set it for all children
	for(int i = 0; i < GetSize(); i++) {
		_concept_array[i]->SetChangeNotification(
			bAChangeNotification);
	}
}

//-----------------------------------------------------------------------------
// Overwritten methods related to the current hypotheses set and belief 
// manipulation
//-----------------------------------------------------------------------------

// D: factory method for hypotheses
CHyp* CArrayConcept::HypFactory() {
	VLOG(WARNING) << (FormatString(
		"HypFactory call on array concept %s failed (frame concepts do not "\
        "support hypsets).", _name.c_str()));
	return NULL;
}

// D: adds a hypothesis to the current set of hypotheses
int CArrayConcept::AddHyp(CHyp* pAHyp) {
	VLOG(WARNING) << (FormatString(
		"AddHyp call on array concept %s failed (array concepts do not "\
        "support hypsets).", _name.c_str()));
	return -1;
}

// D: adds a new hypothesis to the current set of hypotheses
int CArrayConcept::AddNewHyp() {
	VLOG(WARNING) << (FormatString(
		"AddNewHyp call on array concept %s failed (array concepts do not "\
        "support hypsets).", _name.c_str()));
	return -1;
}

// D: adds a null hypothesis to the current set of hypotheses
int CArrayConcept::AddNullHyp() {
	VLOG(WARNING) << (FormatString(
		"AddNullHyp call on array concept %s failed (array concepts do not "\
        "support hypsets).", _name.c_str()));
	return -1;
}

// D: sets a hypothesis into a location
void CArrayConcept::SetHyp(int index, CHyp* pHyp) {
	VLOG(WARNING) << (FormatString(
		"SetHyp call on frame concept %s failed (array concepts do not "\
        "support hypsets).", _name.c_str()));
}

// D: sets a null hypothesis into a location
void CArrayConcept::SetNullHyp(int index) {
	VLOG(WARNING) << (FormatString(
		"SetNullHyp call on frame concept %s failed (array concepts do not "\
        "support hypsets).", _name.c_str()));
}

// D: deletes a hypothesis
void CArrayConcept::DeleteHyp(int index) {
	VLOG(WARNING) << (FormatString(
		"DeleteHyp call on array concept %s failed (array concepts do not "\
        "support hypsets).", _name.c_str()));
}

// D: return the hypothesis at a given index
CHyp* CArrayConcept::GetHyp(int index) {
	VLOG(WARNING) << (FormatString(
		"GetHyp call on array concept %s failed (array concepts do not "\
        "support hypsets).", _name.c_str()));
    return NULL;
}

// D: finds the index of a given hypothesis
int CArrayConcept::GetHypIndex(CHyp* pHypIndex) {
	VLOG(WARNING) << (FormatString(
		"GetHypIndex call on array concept %s failed (array concepts do not "\
        "support hypsets).", _name.c_str()));
    return -1;
}

// D: return the confidence of a certain hypothesis (specified by index)
float CArrayConcept::GetHypConfidence(int index) {
	VLOG(WARNING) << (FormatString(
		"GetHypConfidence call on array concept %s failed (array concepts "
        "do not support hypsets).", _name.c_str()));
    return 0;
}

// D: return the top hypothesis
CHyp* CArrayConcept::GetTopHyp() {
	VLOG(WARNING) << (FormatString(
		"GetTopHyp call on array concept %s failed (array concepts "
        "do not support hypsets).", _name.c_str()));
    return NULL;
}

// D: return the top hyp index
int CArrayConcept::GetTopHypIndex() {
	VLOG(WARNING) << (FormatString(
		"GetTopHypIndex call on array concept %s failed (array concepts "
        "do not support hypsets).", _name.c_str()));
    return -1;
}

// D: return the confidence score of the top hypothesis
float CArrayConcept::GetTopHypConfidence() {
	VLOG(WARNING) << (FormatString(
		"GetTopHypConfidence call on array concept %s failed (array concepts "
        "do not support hypsets).", _name.c_str()));
    return 0;
}

// D: check if a hypothesis is valid (confidence score > 0; valus is not 
// NULL)
bool CArrayConcept::IsValidHyp(int index) {
	VLOG(WARNING) << (FormatString(
		"IsValidHyp call on array concept %s failed (array concepts "
        "do not support hypsets).", _name.c_str()));
    return false;
}

// D: return the total number of hypotheses for a concept (including 
// NULLs)
int CArrayConcept::GetNumHyps() {
	VLOG(WARNING) << (FormatString(
		"GetNumHyps call on array concept %s failed (array concepts "
        "do not support hypsets).", _name.c_str()));
    return -1;
}

// D: return the number of valid hypotheses for a concept
int CArrayConcept::GetNumValidHyps() {
	VLOG(WARNING) << (FormatString(
		"GetNumValidHyps call on array concept %s failed (array concepts "
        "do not support hypsets).", _name.c_str()));
    return -1;
}

void CArrayConcept::ClearCurrentHypSet() {

    if(_concept_array.size() == 0) return;

	for(size_t i = 0; i < _concept_array.size(); i++) {
		if(_concept_array[i] != NULL) 
			delete _concept_array[i];
	}

    _concept_array.clear();

    NotifyChange();
}

void CArrayConcept::CopyCurrentHypSetFrom(CConcept& rAConcept) {
    
    ClearCurrentHypSet();

    if( rAConcept.GetConceptType() != ctArray ) {
        VLOG(WARNING) << __FUNCTION__ << ", Not Array, Name: " << _name;
        return ;
    }

    CArrayConcept& array_concept = (CArrayConcept &)rAConcept;

	if(array_concept.GetSize() == 0)
		return;

	for(int i = 0; i < array_concept.GetSize(); i++) {
		CConcept* pAConcept = CreateElement();	
		pAConcept->SetChangeNotification(_is_change_notification);
		pAConcept->SetName(FormatString("%s.%d", _name.c_str(), i));
        pAConcept->SetOwnerDialogAgent(_owner_dialog_agent);
        pAConcept->SetOwnerConcept(this);
		pAConcept->CopyCurrentHypSetFrom(array_concept[i]);
		_concept_array.push_back(pAConcept);
	}

    NotifyChange();
}

int CArrayConcept::GetTurnLastUpdated() {
    int turn = -1;
    for(size_t i = 0; i < _concept_array.size(); i++) {
		if (turn < _concept_array[i]->GetTurnLastUpdated()) {
			turn = _concept_array[i]->GetTurnLastUpdated();
		}
	}
	
    return turn;
}

void CArrayConcept::SetHistoryConcept(bool is_history_concept) {
	_is_history_concept = is_history_concept;

	for(size_t i = 0; i < _concept_array.size(); i++) {
		_concept_array[i]->SetHistoryConcept( is_history_concept );
    }
}



int CArrayConcept::GetSize() {
	return _concept_array.size();
}

void CArrayConcept::DeleteAt(size_t index) {
    if(_is_history_concept) {
        VLOG(WARNING) << __FUNCTION__ << ", Cannot perform, _is_history_concept is true, Name: " << _name;
        return ;
    }

	if( index >= _concept_array.size() ) {
        VLOG(WARNING) << __FUNCTION__ << ", Out of bounds, Index: " << index << ", Name: " << _name;
        return ;
    }

	delete (_concept_array[index]);

    _concept_array.erase(_concept_array.begin() + index);

	for (index++; index < _concept_array.size(); index++) {
		_concept_array[index]->SetName(FormatString("%s.%d", _name.c_str(), index));
    }

    NotifyChange();
}

void CArrayConcept::InsertAt(size_t index, CConcept &rAConcept) {

    if(_is_history_concept) {
        VLOG(WARNING) << __FUNCTION__ << ", Can not perform, Name: " << _name;
        return ;
    }

	if( index > _concept_array.size() ) {
        VLOG(WARNING) << __FUNCTION__ << ", Out of bounds, Index: " << index << ", Name: " << _name;
        return ;
    }

	CConcept* pAConcept = CreateElement();
	pAConcept->SetChangeNotification(_is_change_notification);
    *pAConcept = rAConcept;
	pAConcept->SetName(FormatString("%s.%d", _name.c_str(), index));
    pAConcept->SetOwnerDialogAgent(_owner_dialog_agent);
    pAConcept->SetOwnerConcept(_owner_concept);

	if ( index == _concept_array.size() ) {
		_concept_array.push_back(pAConcept);
    } else {
		_concept_array.insert(_concept_array.begin() + index, pAConcept);
    }

	for (index++; index < _concept_array.size(); index++) {
		_concept_array[index]->SetName( FormatString("%s.%d", _name.c_str(), index) );
    }
    NotifyChange();
}

}
