#include "StructConcept.h"
#include "Robot/TCD/FrameTaskDman/DMCore/DMCore.h"

namespace TrioTDM {

CStructHyp::CStructHyp(std::map<std::string, CConcept*>* item_map, std::vector<std::string>* items,
        int hyp_index, bool is_complete) {
    _hyp_type = ctStruct;
    _item_map = item_map;
    _items = items;
    _hyp_index = hyp_index;
    _confidence = 0.0;

    for (size_t i = 0;i < _items->size(); ++i ) {
        CHyp* hyp;
        if ( is_complete ) {
            hyp = _item_map->operator[](_items->operator[](i))->GetHyp(_hyp_index);
        } else {
            hyp = _item_map->operator[](_items->operator[](i))->GetPartialHyp(_hyp_index);
        }
        if ( hyp != NULL ) {
            if ( _confidence == 0.0 ) {
                _confidence = hyp->GetConfidence();
            } else if ( _confidence != hyp->GetConfidence() ) {
                VLOG(WARNING) << "Inconsistent confidence id diff, currunt: " << _confidence << "\tnew: " << hyp->GetConfidence();
            }
        }
    }
}

CStructHyp::CStructHyp(CStructHyp& hyp) {
    _hyp_type = ctStruct;
    _item_map = hyp._item_map;
    _items = hyp._items;
    _hyp_index = hyp._hyp_index;
    _confidence = hyp._confidence;
}

void CStructHyp::SetHypIndex(int index) {
    _hyp_index = index;
}

CHyp& CStructHyp::operator = (CHyp& hyp) {
    if ( &hyp != this ) {
        if ( hyp.GetHypType() != ctStruct ) {
            VLOG(WARNING) << "hyp.GetHypType: " << hyp.GetHypType();
            return *this;
        }

        CStructHyp& struct_hyp = (CStructHyp&)hyp;
        for ( size_t i = 0;i < _items->size(); ++i ) {
            std::map<std::string, CConcept*>::iterator f_iter = struct_hyp._item_map->find(_items->operator[](i));
            if ( f_iter == struct_hyp._item_map->end() ) {
                VLOG(WARNING) << "different hyp type.";
                return *this;
            }
            _item_map->operator[](_items->operator[](i))->SetHyp(_hyp_index, f_iter->second->GetHyp(struct_hyp._hyp_index));
        }
        _confidence = struct_hyp._confidence;
    }
    return *this;
}

void CStructHyp::SetConfidence(float confidence) {
    CHyp::SetConfidence(confidence);
    for ( size_t i = 0;i < _items->size(); ++i ) {
        CHyp* item_hyp = _item_map->operator[](_items->operator[](i))->GetHyp(_hyp_index);
        if ( item_hyp != NULL ) {
            item_hyp->SetConfidence(confidence);
        }
    }
}

bool CStructHyp::operator == (CHyp& hyp) {
    if ( hyp.GetHypType() != ctStruct ) {
        VLOG(WARNING) << "hyp.GetHypType(): " << hyp.GetHypType();
        return false;
    }
    CStructHyp& struct_hyp = (CStructHyp&)hyp;
    for ( size_t i = 0;i < _items->size(); ++i ) {
        CHyp* hyp_1 = _item_map->operator[](_items->operator[](i))->GetHyp(_hyp_index);
        std::map<std::string, CConcept*>::iterator f_iter = struct_hyp._item_map->find(_items->operator[](i));
        if ( f_iter != struct_hyp._item_map->end() ) {
            CHyp* hyp_2 = f_iter->second->GetHyp(struct_hyp._hyp_index);
            if ( (hyp_1 == NULL || hyp_2 == NULL) && ( hyp_1 != hyp_2) ) {
                return false;
            } else if ( hyp_1 != NULL && hyp_2 != NULL && !(*hyp_1 == *hyp_2) ) {
                return false;
            }
        } else {
            VLOG(WARNING) << "hyp not item, " << _items->operator[](i);
            return false;
        }
    }
    return false;
}

bool CStructHyp::operator < (CHyp& hyp) {
    VLOG(WARNING) << "Comparison operator < called on CStructHyp.";
    return false;
}

bool CStructHyp::operator > (CHyp& hyp) {
    VLOG(WARNING) << "Comparison operator > called on CStructHyp.";
    return false;
}

bool CStructHyp::operator <= (CHyp& hyp) {
    VLOG(WARNING) << "Comparison operator <= called on CStructHyp.";
    return false;
}

bool CStructHyp::operator >= (CHyp& hyp) {
    VLOG(WARNING) << "Comparison operator >= called on CStructHyp.";
    return false;
}

CHyp* CStructHyp::operator [](const std::string& item) {
    return _item_map->operator[](item)->GetHyp(_hyp_index);
}

std::string CStructHyp::ValueToString() {
    std::string result = "{\n";

    for (size_t i = 0;i < _items->size(); ++i ) {
        CHyp* hyp = _item_map->operator[](_items->operator[](i))->GetHyp(_hyp_index);
        if ( hyp ) {
            result += FormatString("%s\t%s\n", _items->operator[](i).c_str(), hyp->ValueToString().c_str());
        }
    }
    result += "}\n";

    return result;
}

std::string CStructHyp::ToString() {
    std::string result = "{\n";

    for (size_t i = 0;i < _items->size(); ++i ) {
        CHyp* hyp = _item_map->operator[](_items->operator[](i))->GetHyp(_hyp_index);
        if ( hyp ) {
            result += FormatString("%s\t%s\n", _items->operator[](i).c_str(), hyp->ToString().c_str());
        }
    }
    result += "}\n";
    return result;
}

void CStructHyp::FromString(const std::string& strg) {
    VLOG(WARNING) << "FromString called on CStructHyp. call failed.";
}

CStructConcept::CStructConcept(const std::string& name, TConceptSource source) :
    CConcept(name, source, DEFAULT_STRUCT_CARDINALITY) {
    _concept_type = ctStruct;
    CreateStructure();
}

CStructConcept::~CStructConcept() {
    DestroyStructure();
    if ( _prev_concept != NULL ) {
        delete _prev_concept;
        _prev_concept = NULL;
    }
    if ( _grounding_model != NULL ) {
        CGroundingManagerAgent::Instance()->RemoveConceptGroundingRequest(this);
        delete _grounding_model;
        _grounding_model = NULL;
    }
    if ( _is_waiting_conveyance ) {
        ClearWaitingConveyance();
    }
    ClearConceptNotificationPointer();
}

void CStructConcept::CreateStructure() {
}

size_t CStructConcept::GetItemsSize() {
    return _items.size();
}

void CStructConcept::DestroyStructure() {
    for (std::map<std::string, CConcept*>::iterator iter = _item_map.begin();iter != _item_map.end(); ++iter ) {
        delete iter->second;
    }
    _item_map.clear();
    _items.clear();
}

bool CStructConcept::operator < (CConcept& concept) {
    VLOG(WARNING) << "Comparison operator < called on structure concept.";
    return false;
}

bool CStructConcept::operator > (CConcept& concept) {
    VLOG(WARNING) << "Comparison operator > called on structure concept.";
    return false;
}

bool CStructConcept::operator <= (CConcept& concept) {
    VLOG(WARNING) << "Comparison operator <= called on structure concept.";
    return false;
}

bool CStructConcept::operator >= (CConcept& concept) {
    VLOG(WARNING) << "Comparison operator >= called on structure concept.";
    return false;
}

CConcept& CStructConcept::operator[] (const std::string& item) {
    if ( item.empty() ) {
        return *this;
    }
    std::string name;
    std::string follow_up;
    SplitOnFirst(item, ".", name, follow_up);
    int index = atoi(name.c_str());

    if ( index < 0 && _prev_concept != NULL ) {
        if ( index == -1 ) {
            if ( follow_up.empty() ) {
                return *_prev_concept;
            } else {
                return _prev_concept->operator[](follow_up);
            }
        } else {
            return _prev_concept->operator[](FormatString("%d.%s", index+1, follow_up.c_str()));
        }
    } else {
        std::map<std::string, CConcept*>::iterator f_iter = _item_map.find(name);
        if ( f_iter == _item_map.end() ) {
            VLOG(WARNING) << "not find name: " << name << "\ton: " << _name;
            return NULLConcept;
        }
        CConcept* concept = f_iter->second;
        if ( follow_up.empty() ) {
            return *concept;
        } else {
            return concept->operator[](follow_up);
        }
    }
}

CConcept* CStructConcept::Clone(bool is_clone_history) {

	CStructConcept* pConcept = (CStructConcept *)EmptyClone();

	pConcept->DestroyStructure();
	for(size_t i = 0; i < _items.size(); i++) {
		CConcept* pConceptToInsert = _item_map[_items[i]]->Clone();
		pConceptToInsert->SetOwnerConcept(pConcept);
        pConcept->_item_map.insert(std::make_pair(_items[i], pConceptToInsert));
		pConcept->_items.push_back(_items[i]);
	}

    pConcept->SetConceptType(_concept_type);
    pConcept->SetConceptSource(_concept_source);
    pConcept->_name = _name;
    pConcept->_owner_dialog_agent = _owner_dialog_agent;
	pConcept->SetOwnerConcept(_owner_concept);
	pConcept->_grounding_model = NULL;
	pConcept->_is_sealed = _is_sealed;
	pConcept->DisableChangeNotification();
    for(size_t i = 0; i < _current_hyps.size(); i++) {
		if( _current_hyps[i] == NULL) {
			pConcept->_current_hyps.push_back(NULL);
        } else {
			pConcept->_current_hyps.push_back(
				new CStructHyp(&(pConcept->_item_map), &(pConcept->_items), i));
        }
    }
	pConcept->_num_valid_hyps = _num_valid_hyps;
	pConcept->SetGroundedFlag(_is_grounded);
	pConcept->_cardinality = _cardinality;
    pConcept->SetTurnLastUpdated(_turn_last_updated);
    pConcept->_conveyance = _conveyance;
    pConcept->_is_waiting_conveyance = false;
    pConcept->SetHistoryConcept(_is_history_concept);
    if(is_clone_history && (_prev_concept != NULL)) 
		pConcept->_prev_concept = _prev_concept->Clone();
	else
        pConcept->_prev_concept = NULL;

	// finally, return the clone
    return pConcept;
}

void CStructConcept::Update_NPU_AssignFromString(const void* update_data) {
    if ( _is_history_concept ) {
        VLOG(FATAL) << __FUNCTION__ << ", Cannot perform, _is_history_concept is true, Name: " << _name;
        return ;
    }
    std::string strg = *(std::string*)update_data;
    std::string working_strg = TrimLeft(strg);
    ClearCurrentHypSet();
    if ( working_strg[0] != '{' ) {
        VLOG(FATAL) << __FUNCTION__ << ", formt error, not {";
        return ;
    }
    working_strg = TrimLeft(working_strg.substr(1, working_strg.length()-1));
    size_t pos = FindClosingQuoteChar(working_strg, 0, '{', '}');

    while( (pos < working_strg.length() - 1) &&
            ( (working_strg[pos] == '\n') ||
              (working_strg[pos] == '\t') ||
              (working_strg[pos] == ' ')) ) {
        pos += 1;
    }

    std::string confidence;
    if ( pos < working_strg.length() -1 ) {
        confidence = working_strg.substr(pos+1, working_strg.length()-pos-1);
        float f_confidence = (float)atof(confidence.c_str());
        ((void)(f_confidence));
        if ( errno == ERANGE ) {
            VLOG(FATAL) << __FUNCTION__ << ", Cannot perform update, name: " << _name << ", strg: " << strg;
            return ;
        }
    }
    std::string line;
    std::set<std::string> updated;
    int updated_num_hyps = -1;

    while ( ((line = Trim(ExtractFirstLine(working_strg))) != "}") &&
            (line != "") && (line[0] != '}')) {
        std::string item, value;
        if ( SplitOnFirst(line, CONCEPT_EQUALS_SIGN, item, value) ) {
            if ( (value[0] == '{') || (value[0] == ':') ) {
                working_strg = TrimLeft(value + working_strg);
                size_t count = FindClosingQuoteChar(working_strg, working_strg.find('{')+1, '{', '}');
                if ( count >= working_strg.length() ) {
                    VLOG(FATAL) << "conversion error, name: " << _name << "\tworking_strg: " << working_strg;
                    return ;
                }
                value = working_strg.substr(0, count);
                working_strg = TrimLeft(working_strg.substr(count+1, working_strg.length() - count - 1));
            }
            std::map<std::string, CConcept*>::iterator f_iter = _item_map.find(item);
            if ( f_iter == _item_map.end() ) {
                VLOG(FATAL) << __FUNCTION__ << ", not find item, " << item << ", Name: " << _name;
                return ;
            } else {
                if ( confidence != "" ) {
                    value += VAL_CONF_SEPARATOR + confidence;
                }
                f_iter->second->Update(CU_ASSIGN_FROM_STRING, &value);
                updated.insert(item);
                if ( updated_num_hyps == -1 ) {
                    updated_num_hyps = f_iter->second->GetNumHyps();
                } else if ( f_iter->second->GetNumHyps() != updated_num_hyps ) {
                    VLOG(FATAL) << __FUNCTION__ << "variable number, error, item: " << item << ", Name: " << _name;
                    return ;
                }
            }
        } else  {
            VLOG(FATAL) << __FUNCTION__ << ", format error, name: " << _name << ", Strg: " << strg;
        }
    }
    if ( line[0] != '}' ) {
        VLOG(FATAL) << __FUNCTION__ << ", format error, Name: " << _name << ", Strg: " << strg;
    }
    for (size_t i = 0;i < _items.size(); ++i ) {
        if ( updated.find(_items[i]) == updated.end() ) {
            for ( int j = 0;j < updated_num_hyps; ++j ) {
                _item_map[_items[i]]->AddNullHyp();
            }
        }
    }
    CConcept::ClearCurrentHypSet();
    for (int i = 0;i < updated_num_hyps; ++i ) {
        AddHyp(new CStructHyp(&_item_map, &_items, i));
    }
    _num_valid_hyps = updated_num_hyps;
}

void CStructConcept::Update_PartialFromString(const void* update_data) {
    UpdateFromString(update_data, CU_PARTIAL_FROM_STRING);
}

void CStructConcept::UpdateFromString(const void* update_data, const std::string& update_type) {
    if( _is_history_concept) {
        VLOG(WARNING) << __FUNCTION__ << ", Can Not Update, _is_history_concept is true.";
        return ;
    }

    std::string sString = *(string *)update_data;
    std::string sWorkingString = TrimLeft(sString);

    VLOG(DEBUG) << __FUNCTION__ << ", Strg:\n" << sString;

    ClearCurrentHypSet();

	if(sWorkingString[0] != '{') {
		VLOG(FATAL) << FormatString("Cannot perform conversion to "\
            "<CStructConcept> %s from string (dump below).\n%s", 
            _name.c_str(), sString.c_str());
        return ;
	}

	sWorkingString = TrimLeft(sWorkingString.substr(1, 
        sWorkingString.length()-1));

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
		if(errno == ERANGE) {
			// fail the whole process if we can't find a valid number for confidence
			VLOG(FATAL) << FormatString(
                "Cannot perform update to <CStructConcept> "\
				"%s from string (dump below).\n%s", _name.c_str(),
				sString.c_str());
            return ;
		}
	}

	// now go through all the lines and convert the items 
    std::string sLine;

	// while keeping track of the ones that were updated and how many hypotheses they
	// had
    std::set<std::string> ssUpdated;
	int iUpdatedNumHyps = -1;

	while(((sLine = Trim(ExtractFirstLine(sWorkingString))) != "}") && 
		(sLine != "") && (sLine[0] != '}')) {

		// split at the equals sign
		string sItem, sValue;
		if(SplitOnFirst(sLine, CONCEPT_EQUALS_SIGN, sItem, sValue)) {
			// if we successfully made the split
			// check if sValue starts with {. 
			if((sValue[0] == '{') || (sValue[0] == ':')) {
				// in this case, we are dealing with a nested structure 
				// (or an array), so identify where it ends, and correct 
                // the sValue;

				// basically go forward counting the matching {}s and
				// terminate when the matching one is found
				sWorkingString = TrimLeft(sValue + sWorkingString);
				size_t iCount = FindClosingQuoteChar(sWorkingString, 
					sWorkingString.find('{') + 1, '{', '}');

				// if we ran out of the string, signal an error
				if(iCount >= sWorkingString.length()) {
					VLOG(FATAL) << FormatString(
                        "Cannot perform conversion to <CStructConcept> "\
                        "%s from string (dump below).\n%s",
                        _name.c_str(), sWorkingString.c_str());
                    return ;
				}

				// set the value to the enclosed string
				sValue = sWorkingString.substr(0, iCount);
				// and the working string to whatever was left
				sWorkingString = TrimLeft(sWorkingString.substr(iCount + 1, 
					sWorkingString.length() - iCount - 1));
			}

			// look for that item in the structure
            std::map<std::string, CConcept*>::iterator iPtr;
			iPtr = _item_map.find(sItem);
			if(iPtr == _item_map.end()) {
				VLOG(FATAL) << FormatString(
                    "Item %s not found in structured concept %s"\
					" while converting from string. ",
					sItem.c_str(), _name.c_str());
			} else {
				// and if found, set its value accordingly
                if(sConfidence != "") { 
					sValue += VAL_CONF_SEPARATOR + sConfidence;
                }
				iPtr->second->Update(update_type, &sValue);
				// and mark it as updated
				ssUpdated.insert(sItem);
				if (update_type == CU_PARTIAL_FROM_STRING) {
					// set the number of hypotheses
					if(iUpdatedNumHyps == -1) 
						iUpdatedNumHyps = iPtr->second->GetNumPartialHyps();
					else if(iPtr->second->GetNumPartialHyps() != iUpdatedNumHyps) {
						VLOG(FATAL) << FormatString(
							"Variable number of partial hypotheses in item %s while"\
							" converting concept %s from string.", 
							sItem.c_str(), _name.c_str());
                        return ;
					}
				} else {
					// set the number of hypotheses
					if(iUpdatedNumHyps == -1) 
						iUpdatedNumHyps = iPtr->second->GetNumHyps();
					else if(iPtr->second->GetNumHyps() != iUpdatedNumHyps) {
						VLOG(FATAL) << FormatString(
							"Variable number of hypotheses in item %s while"\
							" converting concept %s from string.", 
							sItem.c_str(), _name.c_str());
					}
				}
			}
		} else {
			// if no equals sign (split unsuccessful), fail the whole process
			VLOG(FATAL) << FormatString(
                "Cannot perform conversion to <CStructConcept> "\
				"%s from string (dump below).%s",
				_name.c_str(), sString.c_str());
            return ;
		} 
	}

	// at this point, we should get out of the loop 
	if(sLine[0] != '}') {
		// fail the whole process
		VLOG(FATAL) << FormatString(
            "Cannot perform conversion to <CStructConcept> from %s.", 
            sString.c_str());
	}

	// finally, go through all the items again and update all the ones that were 
	//  not updated
	for(size_t i = 0; i < _items.size(); i++) {
		if(ssUpdated.find(_items[i]) == ssUpdated.end()) {
			// then update this concept with empty hypotheses
			for(int h = 0; h < iUpdatedNumHyps; h++) {
				if (update_type == CU_PARTIAL_FROM_STRING) {
					_item_map[_items[i]]->AddNullPartialHyp();
				} else {
					// this will notify the update
					_item_map[_items[i]]->AddNullHyp();
				}
			}
		}
	}

	// finally, update the valconf set
    // first clear it (but without deleting the hypotheses from the 
    //  concepts
	if (update_type == CU_PARTIAL_FROM_STRING) {
		CConcept::ClearPartialHypSet();
		for(int h = 0; h < iUpdatedNumHyps; h++) {
			// this will notify the change
			AddPartialHyp(new CStructHyp(&_item_map, &_items, h, false));
		}
        _num_valid_partial_hyps = iUpdatedNumHyps;
	} else {
		CConcept::ClearCurrentHypSet();
		for(int h = 0; h < iUpdatedNumHyps; h++) {
			// this will notify the change
			AddHyp(new CStructHyp(&_item_map, &_items, h));
		}
	    _num_valid_hyps = iUpdatedNumHyps;
    }
}

std::string CStructConcept::HypSetToString() {
    std::string result;
    if ( IsUpdated() ) {
        result = "{\n";
        for ( size_t i = 0;i < _items.size(); ++i ) {
            if ( _item_map[_items[i]]->IsUpdated() ||
                    _item_map[_items[i]]->HasPartialHyp() ) {
                result += FormatString("%s\t%s", _items[i].c_str(), _item_map[_items[i]]->HypSetToString().c_str());
            }
        }
        result += "}\n";
        return result;
    } else {
        return IsInvalidated() ? INVALIDATED_CONCEPT : UNDEFINED_CONCEPT;
    }
}

void CStructConcept::SetName(const std::string& name) {
    CConcept::SetName(name);
    for ( size_t i = 0;i < _items.size(); ++i ) {
        _item_map[_items[i]]->SetName(name + "." + _items[i]);
    }
}

void CStructConcept::SetOwnerDialogAgent(CDialogAgent* agent) {
    CConcept::SetOwnerDialogAgent(agent);
    for ( size_t i = 0;i < _items.size(); ++i ) {
        _item_map[_items[i]]->SetOwnerDialogAgent(agent);
    }
}

void CStructConcept::CreateGroundingModel(const std::string& spec) {
    std::string sDefaultGroundingModelSpec = "";
    std::string sThisSpec = "";

    std::vector<std::string> vsGMSpecs;
    SplitString(spec, ',', &vsGMSpecs);
	for(size_t i = 0; i < vsGMSpecs.size(); i++) {
		vsGMSpecs[i] = TrimLeft(vsGMSpecs[i]);
		if((sThisSpec == "") && (vsGMSpecs[i].find('=') == -1)) {
			sThisSpec = vsGMSpecs[i];
		}
		else {
			string sHead, sTail;
			SplitOnFirst(vsGMSpecs[i], "=", sHead, sTail);
			if (sHead == "@default") {
				sDefaultGroundingModelSpec = sTail;
			}
			else {
			    VLOG(FATAL) << FormatString(
					"Could not create a grounding model for struct"
					" %s. Ill formed grounding model specification: %s.",
					GetAgentQualifiedName().c_str(), vsGMSpecs[i].c_str());
			}
		}
	}
	
	if (sThisSpec == "") {
		sThisSpec = sDefaultGroundingModelSpec;
	}

    if(!(CGroundingManagerAgent::Instance()->GetConfiguration().bGroundConcepts) ||
		(sThisSpec == "none") || (sThisSpec == "")) {
        _grounding_model = NULL;
    } else {
        _grounding_model = new CGMConcept(sThisSpec);
        _grounding_model->Initialize();
        _grounding_model->SetConcept(this);
    }

	for(size_t in = 0; in < _items.size(); in++) {
		string sItemName = _items[in];
		string sItemGroundingModelSpec = "";
		for(size_t i = 0; i < vsGMSpecs.size(); i++) {
			string sHead, sRest;
			SplitOnFirst(vsGMSpecs[i], " =.", sHead, sRest);
			if(sHead == sItemName) {
				sRest = TrimLeft(sRest);
				if(sRest.length() > 1) { 
					sItemGroundingModelSpec += sRest + ", ";
				}
			}
		}
		// now create the grounding model on the item
	    _item_map[_items[in]]->CreateGroundingModel(TrimRight(sItemGroundingModelSpec, ", "));
	}
}

void CStructConcept::DeclareGroundingModels(
        std::vector<CGroundingModel*>& models,
        std::set<CGroundingModel*>& exclued) {
    CConcept::DeclareGroundingModels(models, exclued);
    for ( size_t i = 0;i < _items.size(); ++i ) {
        _item_map[_items[i]]->DeclareGroundingModels(models, exclued);
    }
}

void CStructConcept::DeclareConcepts(
        std::vector<CConcept*>& concepts,
        std::set<CConcept*>& exclued) {
    CConcept::DeclareConcepts(concepts, exclued);
    for ( size_t i = 0;i < _items.size(); ++i ) {
        _item_map[_items[i]]->DeclareConcepts(concepts, exclued);
    }
}

void CStructConcept::SetChangeNotification(bool is_notification) {
    _is_change_notification = is_notification;
    for (size_t i = 0;i < _items.size(); ++i ) {
        _item_map[_items[i]]->SetChangeNotification(is_notification);
    }
}

CHyp* CStructConcept::HypFactory() {
    VLOG(WARNING) << "cannot call HypFactory on CStructConcept.";
    return NULL;
}

int CStructConcept::AddNewHyp() {
    for ( size_t i = 0;i < _items.size(); ++i ) {
        _item_map[_items[i]]->AddNewHyp();
    }
    return AddHyp(new CStructHyp(&_item_map, &_items, _current_hyps.size()));
}

int CStructConcept::AddNullHyp() {
    for (size_t i = 0;i < _items.size(); ++i ) {
        _item_map[_items[i]]->AddNullHyp();
    }
    return CConcept::AddNullHyp();
}

void CStructConcept::SetHyp(int index, CHyp* hyp) {
    SetNullHyp(index);
    if ( !hyp ) {
        return ;
    }
    _current_hyps[index] = new CStructHyp(&_item_map, &_items, index);
    *(_current_hyps[index]) = *hyp;
    _num_valid_hyps += 1;
    NotifyChange();
}

void CStructConcept::DeleteHyp(int index) {
    for ( size_t i = 0;i < _items.size(); ++i ) {
        _item_map[_items[i]]->DeleteHyp(index);
    }
    if ( _current_hyps[index] != NULL ) {
        delete _current_hyps[index];
        _num_valid_hyps -= 1;
    }
    _current_hyps.erase(_current_hyps.begin() + index);

    for(size_t i = 0;i < _current_hyps.size(); ++i ) {
        if ( _current_hyps[i] != NULL ) {
            ((CStructHyp*)_current_hyps[i])->SetHypIndex(i);
        }
    }
    NotifyChange();
}

void CStructConcept::SetNullHyp(int index) {
    if ( _current_hyps[index] == NULL ) {
        return ;
    }
    delete _current_hyps[index];
    _current_hyps[index] = NULL;

    for ( size_t i = 0;i < _items.size(); ++i ) {
        _item_map[_items[i]]->SetNullHyp(index);
    }
    _num_valid_hyps -= 1;
    NotifyChange();
}

void CStructConcept::ClearCurrentHypSet() {
    CConcept::ClearCurrentHypSet();
    for ( size_t i = 0;i < _items.size(); ++i ) {
        _item_map[_items[i]]->ClearCurrentHypSet();
    }
}

int CStructConcept::AddNewPartialHyp() {
    for ( size_t i = 0;i < _items.size(); ++i ) {
        _item_map[_items[i]]->AddNewPartialHyp();
    }
    return AddPartialHyp(new CStructHyp(&_item_map, &_items, _partial_hyps.size(), false));
}

int CStructConcept::AddNullPartialHyp() {
    for ( size_t i = 0;i < _items.size(); ++i ) {
        _item_map[_items[i]]->AddNullPartialHyp();
    }
    return CConcept::AddNullPartialHyp();
}

void CStructConcept::SetHistoryConcept(bool is_history) {
    _is_history_concept = is_history;
    for (size_t i = 0;i < _items.size(); ++i ) {
        _item_map[_items[i]]->SetHistoryConcept(is_history);
    }
}

}
