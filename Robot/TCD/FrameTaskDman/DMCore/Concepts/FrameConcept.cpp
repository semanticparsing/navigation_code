#include "FrameConcept.h"
#include "Robot/TCD/FrameTaskDman/DMCore/DMCore.h"

namespace TrioTDM {
// D: constructor
CFrameConcept::CFrameConcept(const std::string& name, TConceptSource source): 
	CStructConcept(name, source) {
	_concept_type = ctFrame;
	_is_grounded = true;
	_cardinality = -1;
}
	
// D: destructor
CFrameConcept::~CFrameConcept() {
    // if this is not a history concept
    if(!_is_history_concept) 
	    DestroyStructure();
	else {
	    _item_map.clear();
	    _items.clear();
	}
	if(_prev_concept != NULL)
        delete _prev_concept;
	_prev_concept = NULL;
	if(_grounding_model != NULL) {
        CGroundingManagerAgent::Instance()->RemoveConceptGroundingRequest(this);
        delete _grounding_model;
	}
	_grounding_model = NULL;
    if( _is_waiting_conveyance ) ClearWaitingConveyance();
    ClearConceptNotificationPointer();
}

//-----------------------------------------------------------------------------
// CConcept: Methods for overall concept manipulation
//-----------------------------------------------------------------------------

// D: equality operator
bool CFrameConcept::operator == (CConcept& concept) {
    // check equality amongst all items
    std::map<std::string, CConcept*>::iterator iPtr;
	for(iPtr = _item_map.begin(); iPtr != _item_map.end(); iPtr++) { 
		if(*(iPtr->second) != concept[iPtr->first]) {
			return false;
		}
    }
	return true;
}

// D: Comparison operator
bool CFrameConcept::operator < (CConcept& concept) {
    VLOG(WARNING) << "Comparison operator < called on frame concept.";
    return false;
}

// D: Comparison operator
bool CFrameConcept::operator > (CConcept& concept) {
    VLOG(WARNING) << "Comparison operator > called on frame concept.";
    return false;
}

// D: Comparison operator
bool CFrameConcept::operator <= (CConcept& concept) {
    VLOG(WARNING) << "Comparison operator <= called on frame concept.";
    return false;
}

// D: Comparison operator
bool CFrameConcept::operator >= (CConcept& concept) {
    VLOG(WARNING) << "Comparison operator >= called on frame concept.";
    return false;
}

// D: clears the value of the concept
void CFrameConcept::Clear() {
    // first check that it's not a history copy
    if(_is_history_concept) {
        VLOG(FATAL) << "Cannot perform ClearCurrentValue on concept history. " << _name;
        return ;
    }

	// o/w delete all it's history
    if(_prev_concept != NULL) {
        delete _prev_concept;
        _prev_concept = NULL;
    }

	// and clear all the concepts in the _item_map    
	for(size_t i = 0; i < _items.size(); i++) {
        _item_map[_items[i]]->Clear();
    }
    // don't log the update since frames don't have grounding models
    // notify the change
    NotifyChange();
}

// D: clears the current value of the concept
void CFrameConcept::ClearCurrentValue() {
    // first check that it's not a history copy
    if(_is_history_concept) {
        VLOG(FATAL) << "Canno perform ClearCurrentValue on concept history." << _name;
        return ;
    }

	// o/w clear all the concepts in the _item_map    
    for(size_t i = 0; i < _items.size(); i++) {
        _item_map[_items[i]]->ClearCurrentValue();
    }

    // don't log the update since frames don't have grounding models

    // notify the change
    NotifyChange();
}

//-----------------------------------------------------------------------------
// Overwritten methods implementing various types of updates in the 
// naive probabilistic update scheme
//-----------------------------------------------------------------------------

// D: Naive probabilistic update scheme - assign from std::string
void CFrameConcept::Update_NPU_AssignFromString(const void* update_data) {

	// first, check that it's not a history concept
    if(_is_history_concept) {
        VLOG(FATAL) << "Cannot perform (AssignFromString) update on concept history." << _name;
        return ;
    }
        
	// o/w 
    std::string sString = *(string *)update_data;
	std::string sWorkingString = TrimLeft(sString);

	// the first character has to be "{"
	if(sWorkingString[0] != '{') {
		// fail the whole process
		VLOG(FATAL) << 
            "Cannot perform conversion to <CFrameConcept> from std::string "
            "(dump below). name: " << _name << "\tstrg: " << sString;
	}

	// advance over the "{"
	sWorkingString = TrimLeft(
        sWorkingString.substr(1, sWorkingString.length()-1));

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
	std::string sConfidence;
	if(uiPos < sWorkingString.length() - 1) {
		sConfidence = sWorkingString.substr(uiPos + 1, 
			sWorkingString.length() - uiPos - 1);
		float fConfidence = (float)atof(sConfidence.c_str());
        ((void) fConfidence);
		if(errno == ERANGE) {
			// fail the whole process if we can't find a valid number for confidence
			VLOG(FATAL) <<
                "Cannot perform update to <CFrameConcept>  from std::string (dump below)." << "name: " << _name << "\tstrg: " << sString;
		}
	}

	// now go through all the lines and convert the items 
	std::string sLine;
	while(((sLine = Trim(ExtractFirstLine(sWorkingString))) != "}") && 
		(sLine != "") && (sLine[0] != '}')) {

		// split at the equals sign
		std::string sItem, sValue;
		if(SplitOnFirst(sLine, CONCEPT_EQUALS_SIGN, sItem, sValue)) {
			// if we successfully made the split
			// check if sValue starts with {. 
			if((sValue[0] == '{') || (sValue[0] == ':')) {
				// in this case, we are dealing with a nested structure (or an 
				// array), so identify where it ends, and correct the sValue;

				// basically go forward counting the matching {}s and
				// terminate when the matching one is found
				sWorkingString = TrimLeft(sValue + sWorkingString);
				size_t iCount = FindClosingQuoteChar(sWorkingString, 
					sWorkingString.find('{') + 1, '{', '}');

				// if we ran out of the std::string, signal an error
				if(iCount >= sWorkingString.length()) {
					VLOG(FATAL) << "Cannot perform conversion to "\
						"<CFrameConcept> from std::string (dump below)." <<
                        _name << "working: " <<  sWorkingString;
				}

				// set the value to the enclosed std::string
				sValue = sWorkingString.substr(0, iCount);
				// and the working std::string to whatever was left
				sWorkingString = TrimLeft(sWorkingString.substr(iCount + 1, 
					sWorkingString.length() - iCount - 1));
			}

			// look for that item in the structure
			std::map<std::string, CConcept*>::iterator iPtr;
			iPtr = _item_map.find(sItem);
			if(iPtr == _item_map.end()) {
				VLOG(FATAL) << "Item not found in structured concept "\
					" while converting from std::string. Ignoring." <<
					sItem << "\tname: " <<  _name;
			} else {
				// and if found, update its value accordingly
				if(sConfidence != "") 
					sValue += VAL_CONF_SEPARATOR + sConfidence;
				iPtr->second->Update(CU_ASSIGN_FROM_STRING, &sValue);
			}
		} else {
			// if no equals sign (split unsuccessful), fail the whole process
			VLOG(FATAL) << 
                "Cannot perform conversion to <CFrameConcept> from std::string (dump below)." <<
                _name << "\tstrg: " << sString;
		} 
	}

	// at this point, we should get out of the loop 
	if(sLine[0] != '}') {
		// fail the whole process
		VLOG(FATAL) << "Cannot perform conversion to <CFrameConcept> from" <<  sString;
	}

    // notify the change
    NotifyChange();
}

// D: Naive probabilistic update scheme - perform update with another concept
void CFrameConcept::Update_NPU_UpdateWithConcept(const void* update_data) {

	// first, check that it's not a history concept
    if(_is_history_concept) {
        VLOG(FATAL) << "Cannot perform (UpdateWithConcept) update on "
			"concept  history." << _name;
        return ;
    }

	CConcept *pConcept = (CConcept *)update_data;

	// check that pConcept is the right type
    if(pConcept->GetConceptType() != ctFrame) {
        VLOG(FATAL) << "type isnot ctFrme, " << pConcept->GetConceptType();
        return ;
    }

    // apply the UpdateWithConcept operator on all the items
    CFrameConcept* pFrameConcept = (CFrameConcept *)pConcept;
    for(size_t i = 0; i < _items.size(); i++) {
        if(pFrameConcept->_item_map.find(_items[i]) == 
            pFrameConcept->_item_map.end()) {
            VLOG(FATAL) << "Item not found in concept during += operator." <<
                _items[i] << "\t"  << pFrameConcept->GetName();
        } else 
            _item_map[_items[i]]->Update(CU_UPDATE_WITH_CONCEPT,
				pFrameConcept->_item_map[_items[i]]);
    }

    // notify the change
    NotifyChange();
}

// D: Update the value of a concept by collapsing it to the mode
void CFrameConcept::Update_NPU_CollapseToMode(const void* update_data) {
    // basically calls collapse to mode on all its subitems
    for(size_t i = 0; i < _items.size(); i++) {
        _item_map[_items[i]]->Update(CU_COLLAPSE_TO_MODE, update_data);
    }
}

// A: Update the partial value of a concept
void CFrameConcept::Update_PartialFromString(const void* update_data) {
	// traverses the frame and updates each sub-concept
	UpdateFromString(update_data, CU_PARTIAL_FROM_STRING);
}

void CFrameConcept::UpdateFromString(const void* update_data, const std::string& sUpdateType) {
    if(_is_history_concept) {
        VLOG(FATAL) << "Cannot perform (AssignFromString) update on "
			"concept history." << _name;
        return ;
    }
        
    std::string sString = *(string *)update_data;
	std::string sWorkingString = TrimLeft(sString);

	if(sWorkingString[0] != '{') {
		VLOG(FATAL) << "Cannot perform conversion to <CFrameConcept> from std::string (dump below)" << _name << "\t" << sString;
        return ;
	}

	sWorkingString = TrimLeft(
        sWorkingString.substr(1, sWorkingString.length()-1));

	size_t uiPos = FindClosingQuoteChar(sWorkingString, 0, '{', '}');

	while((uiPos < sWorkingString.length() - 1)&&
			((sWorkingString[uiPos] == '\n')||
			(sWorkingString[uiPos] == '\t')||
			(sWorkingString[uiPos] == ' '))) {
		uiPos++;	
	}

	std::string sConfidence;
	if(uiPos < sWorkingString.length() - 1) {
		sConfidence = sWorkingString.substr(uiPos + 1, 
			sWorkingString.length() - uiPos - 1);
		float fConfidence = (float)atof(sConfidence.c_str());
        ((void) fConfidence);
		if( errno == ERANGE ) {
			VLOG(FATAL) << "Cannot perform update to <CFrameConcept>  from std::string (dump below)." 
                << _name << "\t" <<  sString;
            return ;
		}
	}

	// now go through all the lines and convert the items 
	std::string sLine;
	while(((sLine = Trim(ExtractFirstLine(sWorkingString))) != "}") && 
		(sLine != "") && (sLine[0] != '}')) {

		// split at the equals sign
		std::string sItem, sValue;
		if(SplitOnFirst(sLine, CONCEPT_EQUALS_SIGN, sItem, sValue)) {
			// if we successfully made the split
			// check if sValue starts with {. 
			if((sValue[0] == '{') || (sValue[0] == ':')) {
				// in this case, we are dealing with a nested structure (or an 
				// array), so identify where it ends, and correct the sValue;

				// basically go forward counting the matching {}s and
				// terminate when the matching one is found
				sWorkingString = TrimLeft(sValue + sWorkingString);
				size_t iCount = FindClosingQuoteChar(sWorkingString, 
					sWorkingString.find('{') + 1, '{', '}');

				// if we ran out of the std::string, signal an error
				if(iCount >= sWorkingString.length()) {
					VLOG(FATAL) << "Cannot perform conversion to <CFrameConcept> from std::string (dump below)." << _name << "\t" << sWorkingString;
                    return ;
				}

				// set the value to the enclosed std::string
				sValue = sWorkingString.substr(0, iCount);
				// and the working std::string to whatever was left
				sWorkingString = TrimLeft(sWorkingString.substr(iCount + 1, 
					sWorkingString.length() - iCount - 1));
			}

			// look for that item in the structure
			std::map<std::string, CConcept*>::iterator iPtr;
			iPtr = _item_map.find(sItem);
			if(iPtr == _item_map.end()) {
				VLOG(FATAL) << "Item not found in structured concept  while converting from std::string. Ignoring." << sItem << "\t" << _name;
                return ;
			} else {
				// and if found, update its value accordingly
				if(sConfidence != "") 
					sValue += VAL_CONF_SEPARATOR + sConfidence;
				iPtr->second->Update(sUpdateType, &sValue);
			}
		} else {
			// if no equals sign (split unsuccessful), fail the whole process
			VLOG(FATAL) << "Cannot perform conversion to <CFrameConcept> from std::string (dump below)" << _name << "\t" <<  sString;
            return ;
		} 
	}

	if(sLine[0] != '}') {
		// fail the whole process
		VLOG(FATAL) << "Cannot perform conversion to <CFrameConcept> froms." << sString;
        return ;
	}
}

// D: Calista belief updating scheme - perform update with another concept
void CFrameConcept::Update_Calista_UpdateWithConcept(const void* update_data) {

	// first, check that it's not a history concept
    if(_is_history_concept) {
        VLOG(FATAL) << "Cannot perform (UpdateWithConcept) update on concept history." << _name;
        return ;
    }

	CConcept *pConcept = (CConcept *)update_data;

	// check that pConcept is the right type
    if(pConcept->GetConceptType() != ctFrame) {
        VLOG(FATAL) << "UpdateWithConcept called with different type on CFrameConcept." << pConcept->GetConceptType();
        return ;
    }

    // apply the UpdateWithConcept operator on all the items
    CFrameConcept* pFrameConcept = (CFrameConcept *)pConcept;
    for(size_t i = 0; i < _items.size(); i++) {
        if(pFrameConcept->_item_map.find(_items[i]) == 
            pFrameConcept->_item_map.end()) {
            VLOG(FATAL) << "Item not found in concept during += operator." << _items[i] << pFrameConcept->GetName();
            return ;
        } else { 
            _item_map[_items[i]]->Update(CU_UPDATE_WITH_CONCEPT,
				pFrameConcept->_item_map[_items[i]]);
        }
    }
    // notify the change
    NotifyChange();
}

// D: Update the value of a concept by collapsing it to the mode
void CFrameConcept::Update_Calista_CollapseToMode(const void* update_data) {
    // basically calls collapse to mode on all its subitems
    for(size_t i = 0; i < _items.size(); i++) {
        _item_map[_items[i]]->Update(CU_COLLAPSE_TO_MODE, update_data);
    }
}

//-----------------------------------------------------------------------------
// Overwritten methods implementing various flags (state information) on the 
// concept
//-----------------------------------------------------------------------------

// D: updatedness: so far a frame concept is updated if a least one of 
//    its items is updated
bool CFrameConcept::IsUpdated() {	
	for(size_t i = 0; i < _items.size(); i++) {
		if(_item_map[_items[i]]->IsUpdated()) {
			return true;
		}
    }
	return false;
}

// D: returns true if the concept is updated and grounded (at least one of its
//    items is updated and grounded)
bool CFrameConcept::IsUpdatedAndGrounded() {
    // if it's not updated, return false
    if(!IsUpdated()) return false;
    
    // o/w if it's updated, check that all updated items are also grounded
    for(size_t i = 0; i < _items.size(); i++) {
        if(_item_map[_items[i]]->IsUpdated() && !_item_map[_items[i]]->IsUpdatedAndGrounded()) {
            return false;
        }
    }

    // if it all checks out, return true
	return true;
}

// D: returns true if the concept is available, (at least one of its items is
//    available)
bool CFrameConcept::IsAvailable() {
	for(size_t i = 0; i < _items.size(); i++) {
		if(_item_map[_items[i]]->IsAvailable()) {
			return true;
		}
    }
	return false;
}

// D: returns true if the concept is available and grounded. (at least one of its 
bool CFrameConcept::IsAvailableAndGrounded() {

    if(IsUndergoingGrounding()) return false;

	for(size_t i = 0; i < _items.size(); i++) {
		if(_item_map[_items[i]]->IsAvailableAndGrounded()) {
			return true;
		}
    }
	return false;
}

bool CFrameConcept::IsUndergoingGrounding() { 	
	for(size_t i = 0; i < _items.size(); i++) {
		if(_item_map[_items[i]]->IsUndergoingGrounding()) {
			return true;
		}
    }
	return false;
}

std::string CFrameConcept::GroundedHypToString() {
    if ( IsUpdatedAndGrounded() ) {
        std::string sResult = "{\n";
        for(size_t i = 0; i < _items.size(); i++) {
		    if(_item_map[_items[i]]->IsUpdatedAndGrounded()) {
			    sResult += _items[i] + "\t" + _item_map[_items[i]]->GroundedHypToString();
    		}
        }
    	sResult += "}\n";
	    return sResult;
    } else {
        return IsInvalidated() ? INVALIDATED_CONCEPT : UNDEFINED_CONCEPT;
    }
}

// A: conversion to "mode_value" format
std::string CFrameConcept::TopHypToString() {
	std::string sResult = "{\n";
	for(size_t i = 0; i < _items.size(); i++) {
		if(_item_map[_items[i]]->IsUpdated()) {
			sResult += _items[i] + "\t" + _item_map[_items[i]]->TopHypToString();
		}
    }
	sResult += "}\n";
	return sResult;
}

// D: conversion to value/conf;value/conf... format
std::string CFrameConcept::HypSetToString() {
	// the std::string holding the result
	std::string sResult = "{\n";

	// go through the items and add them to the std::string
	for(size_t i = 0; i < _items.size(); i++) {
		if(_item_map[_items[i]]->IsUpdated()) {
			sResult += _items[i] + "\t" + 
					   _item_map[_items[i]]->HypSetToString();
		}
    }

	// and finally add the closing braces and return 
	sResult += "}\n";
	return sResult;
}

//-----------------------------------------------------------------------------
// Overwritten methods related to the grounding model
//-----------------------------------------------------------------------------

// D: Create a grounding model
void CFrameConcept::CreateGroundingModel(const std::string& sGroundingModelSpec) {
	// partition the spec by commas
    std::vector<std::string> vsGMSpecs;
    SplitString(sGroundingModelSpec, ',', &vsGMSpecs);
	// and trim it
	std::string sThisSpec = "";
	for(size_t i = 0; i < vsGMSpecs.size(); i++) {
		vsGMSpecs[i] = TrimLeft(vsGMSpecs[i]);
		// check if it's for the current item
		if((sThisSpec == "") && (vsGMSpecs[i].find('=') == -1)) {
			sThisSpec = vsGMSpecs[i];
		}
	}

	// create the grounding model for this frame
    if(!(CGroundingManagerAgent::Instance()->GetConfiguration().bGroundConcepts) ||
		(sThisSpec == "none") || (sThisSpec == "")) {
        _grounding_model = NULL;
    } else {
        VLOG(FATAL) << "Could not create a grounding model for frame concept Frame concepts cannot undergo grounding." << GetAgentQualifiedName();
		return ;
    }

	// A: look for a default element grounding model for this frame
	//    if several @default appear, use the last one (this happens
	//    when a frame inherits the default grounding model of its 
	//    parent frame but the user also defines the child's default)
	std::string sDefaultGroundingModelSpec = "";
	for(size_t i = 0; i < vsGMSpecs.size(); i++) {
		std::string sHead, sRest;
		SplitOnFirst(vsGMSpecs[i], " =.", sHead, sRest);
		if(sHead == "@default") {
			sRest = TrimLeft(sRest);
			if (sRest.length()>0) {
				sDefaultGroundingModelSpec = sRest;
			}
		}
	}

	// now go through each of the elements of the structure
	for(size_t ni = 0; ni < _items.size(); ni++) {
		// get the item name
		std::string sItemName = _items[ni];

		// A: propagate the default grounding model to the items
		std::string sItemGroundingModelSpec = "";
		if (sDefaultGroundingModelSpec != "") {
			sItemGroundingModelSpec = "@default=" + sDefaultGroundingModelSpec + ", ";
		}

		// now check it against each of the specs
		// and construct the spec for that item
		for(size_t i = 0; i < vsGMSpecs.size(); i++) {
			std::string sHead, sRest;
			SplitOnFirst(vsGMSpecs[i], " =.", sHead, sRest);
			if(sHead == sItemName) {
				sRest = TrimLeft(sRest);
				if(sRest.length() > 1) { 
					sItemGroundingModelSpec += sRest + ", ";
				}
			}
		}

		// now create the grounding model on the item
		_item_map[_items[ni]]->CreateGroundingModel(
			TrimRight(sItemGroundingModelSpec, ", "));
	}
}

// D: set the invalidated flag
void CFrameConcept::SetInvalidatedFlag(bool bAInvalidated) {
    _is_invalidated = bAInvalidated;
	for(size_t i = 0; i < _items.size(); i++) {
		_item_map[_items[i]]->SetInvalidatedFlag(_is_invalidated);
    }
    BreakSeal();
}

// D: return the invalidated flag
bool CFrameConcept::GetInvalidatedFlag() {
    _is_invalidated = false;
	for(size_t i = 0; i < _items.size(); i++) {
		if(_item_map[_items[i]]->IsInvalidated()) {
		    _is_invalidated = true;
		    break;
		}
    }
    return _is_invalidated;
}

// Methods related to the restored for grounding flag
void CFrameConcept::SetRestoredForGroundingFlag(bool bARestoredForGrounding) {
    _is_restored_for_grounding = bARestoredForGrounding;
	for(size_t i = 0; i < _items.size(); i++) {
		_item_map[_items[i]]->SetRestoredForGroundingFlag(
		    bARestoredForGrounding);
    }
}

// D: return the restored for grounding flag
bool CFrameConcept::GetRestoredForGroundingFlag() {
    _is_restored_for_grounding = false;
	for(size_t i = 0; i < _items.size(); i++) {
		if(_item_map[_items[i]]->GetRestoredForGroundingFlag()) {
		    _is_restored_for_grounding = true;
		    break;
		}
    }
    return _is_restored_for_grounding;
}

// D: factory method for hypotheses
CHyp* CFrameConcept::HypFactory() {
	VLOG(FATAL) << "HypFactory call on frame concept failed (frame concepts do not support hypsets)." << _name;
	return NULL;
}

// D: adds a hypothesis to the current set of hypotheses
int CFrameConcept::AddHyp(CHyp* pAHyp) {
	VLOG(FATAL) << "AddHyp call on frame concept failed (frame concepts do not support hypsets)." << _name;
	return -1;
}

// D: adds a new hypothesis to the current set of hypotheses
int CFrameConcept::AddNewHyp() {
	VLOG(FATAL) << "AddNewHyp call on frame concept failed (frame concepts do not support hypsets)." << _name;
	return -1;
}

// D: adds a null hypothesis to the current set of hypotheses
int CFrameConcept::AddNullHyp() {
	VLOG(FATAL) << "AddNullHyp call on frame concept failed (frame concepts do not support hypsets)." << _name;
	return -1;
}

// D: sets a hypothesis into a location
void CFrameConcept::SetHyp(int index, CHyp* pHyp) {
	VLOG(FATAL) << "SetHyp call on frame concept failed (frame concepts do not support hypsets)." << _name;
}

// D: sets a null hypothesis into a location
void CFrameConcept::SetNullHyp(int index) {
	VLOG(FATAL) << "SetNullHyp call on frame concept failed (frame concepts do not support hypsets)." << _name;
}

// D: deletes a hypothesis
void CFrameConcept::DeleteHyp(int index) {
	VLOG(FATAL) << "DeleteHyp call on frame concept failed (frame concepts do not support hypsets)." << _name;
}

// D: return the hypothesis at a given index
CHyp* CFrameConcept::GetHyp(int index) {
	VLOG(FATAL) << "GetHyp call on frame concept failed (frame concepts do not support hypsets)." << _name;
    return NULL;
}

// D: finds the index of a given hypothesis
int CFrameConcept::GetHypIndex(CHyp* pHypIndex) {
	VLOG(FATAL) << "GetHypIndex call on array concept failed (array concepts do not support hypsets)." << _name;
    return -1;
}

// D: return the confidence of a certain hypothesis (specified by index)
float CFrameConcept::GetHypConfidence(int index) {
	VLOG(FATAL) << "GetHypConfidence call on frame concept failed (frame concepts do not support hypsets)." << _name;
    return 0;
}

// D: return the top hypothesis
CHyp* CFrameConcept::GetTopHyp() {
	VLOG(FATAL) << "GetTopHyp call on frame concept failed (frame concepts do not support hypsets)." << _name;
    return NULL;
}

// D: return the top hyp index
int CFrameConcept::GetTopHypIndex() {
	VLOG(FATAL) << "GetTopHypIndex call on frame concept failed (frame concepts do not support hypsets)." << _name;
    return -1;
}

// D: return the confidence score of the top hypothesis
float CFrameConcept::GetTopHypConfidence() {
	VLOG(FATAL) << "GetTopHypConfidence call on frame concept failed (frame concepts do not support hypsets)." << _name;
    return 0;
}

// D: check if a hypothesis is valid (confidence score > 0; valus is not 
// NULL)
bool CFrameConcept::IsValidHyp(int index) {
	VLOG(FATAL) << "IsValidHyp call on frame concept failed (frame concepts do not support hypsets)." << _name;
    return false;
}

// D: return the total number of hypotheses for a concept (including 
// NULLs)
int CFrameConcept::GetNumHyps() {
	VLOG(FATAL) << "GetNumHyps call on frame concept failed (frame concepts do not support hypsets)." << _name;
    return -1;
}

// D: return the number of valid hypotheses for a concept
int CFrameConcept::GetNumValidHyps() {
	VLOG(FATAL) << "GetNumValidHyps call on frame concept failed (frame concepts do not support hypsets)." << _name;
    return -1;
}

// D: clear the current set of hypotheses for the concept
void CFrameConcept::ClearCurrentHypSet() {
	// if the concept is empty, simply return 
	if(!IsUpdated()) return ;
    // clear the current hypsets for all the subsumed concepts
    for(size_t i = 0; i < _items.size(); i++) {
        _item_map[_items[i]]->ClearCurrentHypSet();
    }
    // notify the change
    NotifyChange();
}

// D: copies the current set of hypotheses from another concept
void CFrameConcept::CopyCurrentHypSetFrom(CConcept& concept) {
    // first clear it
    ClearCurrentHypSet();

    // check that concept is the right type
    if(concept.GetConceptType() != ctFrame) {
        VLOG(FATAL) << "CopyCurrentHypSetFrom called with different type  on CFrameConcept." <<  concept.GetConceptType() << "\t" << _name;
        return ;
    }

    // copy the hypsets for the items
    CFrameConcept& rAFrameConcept = (CFrameConcept &)concept;
    for(size_t i = 0; i < _items.size(); i++) {
        if(rAFrameConcept._item_map.find(_items[i]) == 
            rAFrameConcept._item_map.end()) {
            VLOG(FATAL) << "Item not found in concept during CopyCurrentHypSetFrom." << _items[i] << "\t" << rAFrameConcept.GetName();
            return ;
        } else {
            _item_map[_items[i]]->CopyCurrentHypSetFrom(
                *rAFrameConcept._item_map[_items[i]]);
        }
    }

    // notify the change
    NotifyChange();
}

// D: clear the explicitly confirmed hyp std::string
void CFrameConcept::ClearExplicitlyConfirmedHyp() {
    // clear the explicitly confirmed hyp for all subconcepts
    for(size_t i = 0; i < _items.size(); i++) {
        _item_map[_items[i]]->ClearExplicitlyConfirmedHyp();
    }
}

// D: clear the explicitly disconfirmed hyp std::string
void CFrameConcept::ClearExplicitlyDisconfirmedHyp() {
    // clear the explicitly confirmed hyp for all subconcepts
    for(size_t i = 0; i < _items.size(); i++) {
        _item_map[_items[i]]->ClearExplicitlyDisconfirmedHyp();
    }
}

//-----------------------------------------------------------------------------
// Overwritten methods providing access to turn last updated information
//-----------------------------------------------------------------------------

// D: Access to turn in which the concept was last updated information
int CFrameConcept::GetTurnLastUpdated() {
	// go through each item and check when it was last updated
	int iTurnLastUpdated = -1;
	for(size_t i = 0; i < _items.size(); i++) {
		if (iTurnLastUpdated < _item_map[_items[i]]->GetTurnLastUpdated())
			iTurnLastUpdated = _item_map[_items[i]]->GetTurnLastUpdated();
	}

	// finally, return it
    return iTurnLastUpdated;
}

//-----------------------------------------------------------------------------
// Overwritten nethods for concept history manipulation
//-----------------------------------------------------------------------------

// D: reopens the concept (i.e. moves current value into history, and starts
void CFrameConcept::ReOpen() {

	// first check if it's a history concept
	if(_is_history_concept) {
        VLOG(FATAL) << "Cannot perform ReOpen on concept history." <<  _name;
        return ;
    }

    // now, reopen all the items
    for(size_t i = 0; i < _items.size(); i++) {
		_item_map[_items[i]]->ReOpen();
        // and redirect the owner concept accordingly
        _item_map[_items[i]]->operator[](-1).SetOwnerConcept(_prev_concept);
    }

    // clear the current value (this will notify the change)
    ClearCurrentHypSet();

	// create a clone of the current value of the concept (without the history)
	CFrameConcept* pConcept = (CFrameConcept *)Clone(false);
		
	// now destroy its structure, and simply make it its subconcepts point to
	// the main structure subconcepts
	pConcept->DestroyStructure();
	for(size_t i = 0; i < _items.size(); i++) {
	    pConcept->_items.push_back(_items[i]);
	    pConcept->_item_map.insert(std::make_pair(
	        _items[i], &(_item_map[_items[i]]->operator[] (-1))));
	}
	
	// set the flag that it's a history concept
	pConcept->SetHistoryConcept();

    // link it to the current history
	pConcept->_prev_concept = _prev_concept;

	// and push it into the history
    _prev_concept = pConcept;

    // don't log the update since frames don't have grounding models
}

// D: clears the history of the current concept
void CFrameConcept::ClearHistory() {
    if(_is_history_concept) {
        VLOG(FATAL) << "Cannot perform ClearHistory on concept history." << _name;
        return ;
    }

	for(size_t i = 0; i < _items.size(); i++)
		_item_map[_items[i]]->ClearHistory();

    if(_prev_concept != NULL) {
        delete _prev_concept;
        _prev_concept = NULL;
    }
}

// D: merges the history on the concept, and returns a new concept containing 
CConcept* CFrameConcept::CreateMergedHistoryConcept() {

	CFrameConcept *pMergedHistory = (CFrameConcept *)Clone(false);

	for(size_t i = 0; i < pMergedHistory->_items.size(); i++)
        pMergedHistory->_item_map[_items[i]]->MergeHistory();

    pMergedHistory->SetHistoryConcept(false);

    return pMergedHistory;
}

// D: merges the history of the concept into the current value
void CFrameConcept::MergeHistory() {
    for(size_t i = 0; i < _items.size(); i++) { 
        _item_map[_items[i]]->MergeHistory();
    }
    SetHistoryConcept(false);
    ClearHistory();
}


}
