#include "InteractionEvent.h"

namespace TrioTDM {

CInteractionEvent::CInteractionEvent() :
    _confidence(1.0) {
    ;
}

CInteractionEvent::CInteractionEvent(const std::string& type) :
    _type(type),
    _confidence(1.0) {
    ;
}

CInteractionEvent::~CInteractionEvent() {
    ;
}

std::string CInteractionEvent::GetType() const {
    return _type;
}

int CInteractionEvent::GetID() const {
    return _id;
}

void CInteractionEvent::SetComplete(bool flag) {
    _is_complete = flag;
}

bool CInteractionEvent::IsComplete() const {
    return _is_complete;
}

float CInteractionEvent::GetConfidence() const {
    return _confidence;
}

std::string CInteractionEvent::GetStringProperty(const std::string& slot) const {
    VLOG(DEBUG) << __FUNCTION__ << ", slot: " << slot;
    std::string a_slot = StringToLowerASCII(slot);
    std::map<std::string, std::string>::const_iterator f_iter = _properties.find(a_slot);
    if ( f_iter != _properties.end() ) {
        return f_iter->second;
    } else {
        VLOG(WARNING) << "Can not find slot, " << slot;
        return "";
    }
}

std::string CInteractionEvent::GetStringProperty(const std::string& dman, const std::string& slot) const {
    VLOG(DEBUG) << __FUNCTION__ << ", Dman: " << dman << ", Slot: " << slot;
    std::string l_dman = StringToLowerASCII( dman );
    std::string l_slot = StringToLowerASCII( slot );

    std::map<std::string, std::map<std::string, std::string> >::const_iterator f_iter = _dman2properties.find( l_dman);
    if ( f_iter != _dman2properties.end() ) {
        std::map<std::string, std::string>::const_iterator ff_iter = f_iter->second.find( l_slot );
        if ( ff_iter != f_iter->second.end() ) {
            return ff_iter->second;
        } else {
            VLOG(WARNING) << __FUNCTION__ << ", Not Find Slot, Dman: " << dman << ", Slot: " << slot;
            return "";
        }
    } else {
        VLOG(WARNING) << __FUNCTION__ << ", Not Find Dman, Dman: " << dman << ", Slot: " << slot;
        return GetStringProperty( slot );
    }
}

int CInteractionEvent::GetIntProperty(const std::string& slot) const {
    VLOG(DEBUG) << __FUNCTION__ << ", slot: " << slot;
    std::string a_slot = StringToLowerASCII(slot);
    std::map<std::string, std::string>::const_iterator f_iter = _properties.find(a_slot);
    if ( f_iter != _properties.end() ) {
        return atoi(f_iter->second.c_str());
    } else {
        VLOG(WARNING) << "Can not find slot, " << slot;
        return 0;
    }
}
    
int CInteractionEvent::GetIntProperty(const std::string& dman, const std::string& slot) const {
    VLOG(DEBUG) << __FUNCTION__ << ", Dman: " << dman << ", Slot: " << slot;
    std::string l_dman = StringToLowerASCII( dman );
    std::string l_slot = StringToLowerASCII( slot );

    std::map<std::string, std::map<std::string, std::string> >::const_iterator f_iter = _dman2properties.find( l_dman);
    if ( f_iter != _dman2properties.end() ) {
        std::map<std::string, std::string>::const_iterator ff_iter = f_iter->second.find( l_slot );
        if ( ff_iter != f_iter->second.end() ) {
            return atoi(ff_iter->second.c_str());
        } else {
            VLOG(WARNING) << __FUNCTION__ << ", Not Find Slot, Dman: " << dman << ", Slot: " << slot;
            return 0;
        }
    } else {
        VLOG(WARNING) << __FUNCTION__ << ", Not Find Dman, Dman: " << dman << ", Slot: " << slot;
        return GetIntProperty( slot );
    }
}

float CInteractionEvent::GetFloatProperty(const std::string& slot) const {
    VLOG(DEBUG) << __FUNCTION__ << ", slot: " << slot;
    std::string a_slot = StringToLowerASCII(slot);
    std::map<std::string, std::string>::const_iterator f_iter = _properties.find(a_slot);
    if ( f_iter != _properties.end() ) {
        return atof(f_iter->second.c_str());
    } else {
        VLOG(WARNING) << "Can not find slot, " << slot;
        return 0.0;
    }
}

float CInteractionEvent::GetFloatProperty(const std::string& dman, const std::string& slot) const {
    VLOG(DEBUG) << __FUNCTION__ << ", Dman: " << dman << ", Slot: " << slot;
    std::string l_dman = StringToLowerASCII( dman );
    std::string l_slot = StringToLowerASCII( slot );

    std::map<std::string, std::map<std::string, std::string> >::const_iterator f_iter = _dman2properties.find( l_dman);
    if ( f_iter != _dman2properties.end() ) {
        std::map<std::string, std::string>::const_iterator ff_iter = f_iter->second.find( l_slot );
        if ( ff_iter != f_iter->second.end() ) {
            return atof(ff_iter->second.c_str());
        } else {
            VLOG(WARNING) << __FUNCTION__ << ", Not Find Slot, Dman: " << dman << ", Slot: " << slot;
            return 0.0;
        }
    } else {
        VLOG(WARNING) << __FUNCTION__ << ", Not Find Dman, Dman: " << dman << ", Slot: " << slot;
        return GetFloatProperty( slot );
    }
}

bool CInteractionEvent::HasProperty(const std::string& slot) const {
    VLOG(DEBUG) << __FUNCTION__ << ", slot: " << slot;
    std::string a_slot = StringToLowerASCII(slot);
    return _properties.find(a_slot) != _properties.end();
}

bool CInteractionEvent::HasProperty(const std::string& dman, const std::string& slot) const {
    VLOG(DEBUG) << __FUNCTION__ << ", Dman: " << dman << ", Slot: " << slot;
    std::string l_dman = StringToLowerASCII( dman );
    std::string l_slot = StringToLowerASCII( slot );

    std::map<std::string, std::map<std::string, std::string> >::const_iterator f_iter = _dman2properties.find( l_dman);
    if ( f_iter != _dman2properties.end() ) {
        return f_iter->second.find(l_slot) != f_iter->second.end();
    } else {
        return HasProperty( slot );
    }
}

void CInteractionEvent::SetProperty(const std::string& slot, const std::string& value) {
    VLOG(DEBUG) << __FUNCTION__ << ", slot: " << slot << "\tvalue: " << value;
    std::string l_slot = StringToLowerASCII(slot);
    _properties[l_slot] = value;
}

void CInteractionEvent::SetProperty(const std::string& dman, const std::string& slot, const std::string& value) {
    VLOG(DEBUG) << __FUNCTION__ << ", Dman: " << dman << ", slot: " << slot << ", value: " << value;
    std::string l_dman = StringToLowerASCII( dman );
    std::string l_slot = StringToLowerASCII(slot);
    if ( l_dman.empty() ) {
        _properties[l_slot] = value;
    } else {
        _dman2properties[l_dman][l_slot] = value;
    }
}

std::map<std::string, std::string>& CInteractionEvent::GetProperties() {
    return _properties;
}

std::string CInteractionEvent::ToString() const {
    std::string event = FormatString("Type\t%s\n", _type.c_str());
    event += FormatString("Complete\t%d\n", _is_complete);

    if ( _properties.size() > 0 ) {
        for ( std::map<std::string, std::string>::const_iterator c_iter = _properties.begin();c_iter != _properties.end(); ++c_iter) {
            event += FormatString(" %s = %s\n", c_iter->first.c_str(), c_iter->second.c_str());
        }
    }
    return event;
}

bool CInteractionEvent::Matches(const std::string& grammar_expectation) const {
    VLOG(DEBUG) << __FUNCTION__ << ", grammar_expectation: " << grammar_expectation;
    return Matches(grammar_expectation, _properties);
}

bool CInteractionEvent::Matches(const std::string& dman, const std::string& grammar_expectation) const {
    VLOG(DEBUG) << __FUNCTION__ << ", Dman: " << dman << ", GrammarExpectation: " << grammar_expectation;
    std::string l_dman = StringToLowerASCII( dman );
    std::map<std::string, std::map<std::string, std::string> >::const_iterator f_iter = _dman2properties.find( l_dman);
    if ( f_iter != _dman2properties.end() ) {
        return Matches(grammar_expectation, f_iter->second);
    } else {
        return Matches( grammar_expectation );
    }
}
    
bool CInteractionEvent::Matches(const std::string& grammar_expectation, const std::map<std::string, std::string>& properties) const {
    std::string tmp = grammar_expectation.substr(1, grammar_expectation.size()-2);
    std::string channel, expectation;
    SplitOnFirst(tmp, ":", channel, expectation);

    if ( expectation == "" ) {
        expectation = channel;
        channel = "";
    }
    if ( channel != "" && channel != _type ) {
        return false;
    }

    for (std::map<std::string, std::string>::const_iterator c_iter = properties.begin();c_iter != properties.end(); ++c_iter ) {
        if ( MatchesSlot(expectation, c_iter->first) ) {
            return true;
        }
    }
    return false;
}

std::string CInteractionEvent::GetValueForExpectation(const std::string& grammar_expectation) const {
    VLOG(DEBUG) << __FUNCTION__ << ", grammar_expectation: " << grammar_expectation;
    return GetValueForExpectation( grammar_expectation, _properties);
}

std::string CInteractionEvent::GetValueForExpectation(const std::string& dman, const std::string& grammar_expectation) const {
    VLOG(DEBUG) << __FUNCTION__ << ", Dman: " << dman << ", GrammarExpectation: " << grammar_expectation;
    std::string l_dman = StringToLowerASCII( dman );
    std::map<std::string, std::map<std::string, std::string> >::const_iterator f_iter = _dman2properties.find( l_dman);
    if ( f_iter != _dman2properties.end() ) {
        return GetValueForExpectation(grammar_expectation, f_iter->second);
    } else {
        return GetValueForExpectation( grammar_expectation );
    }
}

std::string CInteractionEvent::GetValueForExpectation(const std::string& grammar_expectation, const std::map<std::string, std::string>& properties) const {
    std::string tmp = grammar_expectation.substr(1, grammar_expectation.size()-2);
    std::string channel, expectation;
    SplitOnFirst(tmp, ":", channel, expectation);

    if ( expectation == "" ) {
        expectation = channel;
        channel = "";
    }
    if ( channel != "" && channel != _type ) {
        VLOG(WARNING) << __FUNCTION__ << ", type diff, _type: " << _type << "\tchannel: " << channel;
        return "";
    }
    
    for (std::map<std::string, std::string>::const_iterator c_iter = properties.begin();c_iter != properties.end(); ++c_iter ) {
        if ( MatchesSlot(expectation, c_iter->first) ) {
            return c_iter->second;
        }
    }
    VLOG(WARNING) << __FUNCTION__ << ", Can not find grammar_expectation, " << grammar_expectation;
    return "";
}

bool CInteractionEvent::MatchesSlot(const std::string& pattern, const std::string& slot) const {
    VLOG(DEBUG) << __FUNCTION__ << ", pattern: " << pattern << ", slot: " << slot;
    size_t pos_slot = 0;
	size_t len_slot = 0;
	size_t pos_patt = 0;
	size_t len_patt = 0;
    std::string sub_slot, sub_patt;

	// removes the "[" and "]" around the slot
    std::string t_slot = slot.substr( 1, slot.size()-2);

	// gets the first level in the pattern
	if ((int)pattern.find_first_of( '.', pos_patt) > (int)pos_patt) {
		len_patt = pattern.find_first_of( '.', pos_patt) - pos_patt;
	} else {
		len_patt = pattern.size() - pos_patt;
	}
	sub_patt = pattern.substr( pos_patt, len_patt);
	pos_patt += len_patt + 1;

	// gets the first level in the slot name
	if ((int)t_slot.find_first_of( '.', pos_slot) > (int)pos_slot) {
		len_slot = t_slot.find_first_of( '.', pos_slot) - pos_slot;
	} else {
		len_slot = t_slot.size() - pos_slot;
	}
	sub_slot = t_slot.substr( pos_slot, len_slot);
	pos_slot += len_slot + 1;

	// traverses the expectation pattern and the slot name
	while (true) {
		if ( sub_slot ==  sub_patt) {
			// the final subslot of both the pattern and the slot matched:
			// we won!
			if ((pos_patt >= pattern.size()) && (pos_slot >= t_slot.size())) {
				return true;
			}
			// we reached the end of the pattern but not that of the slo => fail
			if (pos_patt >= pattern.size()) {
				return false;
			}

			if ((int)pattern.find_first_of( '.', pos_patt) > (int)pos_patt) {
				len_patt = pattern.find_first_of( '.', pos_patt) - pos_patt;
			} else {
				len_patt = pattern.size() - pos_patt;
			}
			sub_patt = pattern.substr( pos_patt, len_patt);

			// move the position index for the pattern
			pos_patt += len_patt + 1;
		}

		// we reached the end of the slot but not that of the pattern => fail
		if (pos_slot >= t_slot.size()) {
			return false;
		}

		// gets the next level in the slot name
		if ((int)t_slot.find_first_of( '.', pos_slot) > (int)pos_slot) {
			len_slot = t_slot.find_first_of( '.', pos_slot) - pos_slot;
		} else {
			len_slot = t_slot.size() - pos_slot;
		}
		sub_slot = t_slot.substr( pos_slot, len_slot);
		pos_slot += len_slot + 1;
	}
}

}
