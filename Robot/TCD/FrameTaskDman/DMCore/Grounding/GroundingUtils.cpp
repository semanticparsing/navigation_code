#include "GroundingUtils.h"
#include <math.h>

namespace TrioTDM {

std::string& CState::operator[] (std::string index) {
    return _state_vars[index];
}

// D: clear the state
void CState::Clear() {
    _state_vars.clear();
}

// D: insert from another hash
void CState::Add(std::map<std::string, std::string> s2s) {
    _state_vars.insert(s2s.begin()++, s2s.end()--);
}

// D: string conversion function
string CState::ToString() {
    return S2SHashToString(_state_vars, "\n") + "\n";
}

// D: constructor
CBeliefDistribution::CBeliefDistribution(int num_event) {
    // resize the probabilities vector
    _probabilitys.resize(num_event, 0.0);
    _probability_low_bounds.resize(num_event, INVALID_EVENT);
    _probability_hi_bound.resize(num_event, INVALID_EVENT);    
}

// D: constructor from reference
CBeliefDistribution::CBeliefDistribution(
    CBeliefDistribution& belief_distribution) {
    // copy the probabilities vector
    _probabilitys = belief_distribution._probabilitys;
    _probability_low_bounds = belief_distribution._probability_low_bounds;
    _probability_hi_bound = belief_distribution._probability_hi_bound;
}

// D: destructor - does nothing
CBeliefDistribution::~CBeliefDistribution() {
}

// D: resize the event space for the distribution
void CBeliefDistribution::Resize(int num_event) {
    // resize the probabilities vector
    _probabilitys.resize(num_event, 0.0);
    _probability_low_bounds.resize(num_event, INVALID_EVENT);
    _probability_hi_bound.resize(num_event, INVALID_EVENT);
}

//-----------------------------------------------------------------------------
// D: Access to distribution
//-----------------------------------------------------------------------------
// D: Access to probabilities, via the [] operator
float& CBeliefDistribution::operator[] (size_t index) {
    // resize the vector if we're falling out of it
    if(index >= _probabilitys.size()) {
        _probabilitys.resize(index + 1, 0.0);
        _probability_low_bounds.resize(index + 1, INVALID_EVENT);
        _probability_hi_bound.resize(index + 1, INVALID_EVENT);
    } 
    return _probabilitys[index];
}

// D: Access to low bounds
float& CBeliefDistribution::LowBound(size_t index) {
    // resize the vector if we're falling out of it
    if(index >= _probability_low_bounds.size()) {
        _probabilitys.resize(index + 1, 0.0);
        _probability_low_bounds.resize(index + 1, INVALID_EVENT);
        _probability_hi_bound.resize(index + 1, INVALID_EVENT);
    }
    return _probability_low_bounds[index];
}

// D: Access to hi bounds
float& CBeliefDistribution::HiBound(size_t index) {
    // resize the vector if we're falling out of it
    if(index >= _probability_hi_bound.size()) {
        _probabilitys.resize(index + 1, 0.0);
        _probability_low_bounds.resize(index + 1, INVALID_EVENT);
        _probability_hi_bound.resize(index + 1, INVALID_EVENT);
    }
    return _probability_hi_bound[index];
}

// D: Access to the number of valid events
int CBeliefDistribution::GetValidEventsNumber() {
    int event_num = 0; 
    for(size_t i = 0; i < _probabilitys.size(); ++i )
        if(_probabilitys[i] != INVALID_EVENT)
            event_num++;
    return event_num;
}

//-----------------------------------------------------------------------------
// D: Functions for transforming the distribution
//-----------------------------------------------------------------------------

// D: Normalize the distribution
void CBeliefDistribution::Normalize() {
    // compute the normalization constant
    float normalizer = 0;
    for(size_t i = 0; i < _probabilitys.size(); ++i ) {
        if(_probabilitys[i] != INVALID_EVENT) {
            normalizer += _probabilitys[i];
        }
    }
    // normalize
    if(normalizer != 0) {
        for(size_t i = 0; i < _probabilitys.size(); i++) 
            if(_probabilitys[i] != INVALID_EVENT) {
                _probabilitys[i] = _probabilitys[i] / normalizer;
            }
    }
}

//-----------------------------------------------------------------------------
// Functions for choosing a particular action from the distribution
//-----------------------------------------------------------------------------

// D: return the action with the highest probability/utility
int CBeliefDistribution::GetModeEvent() {
    int max_index = -1;
    float max_probability = 0;
    for(size_t i = 0; i < _probabilitys.size(); ++i ) {
        if(_probabilitys[i] > max_probability) {
            max_probability = _probabilitys[i];
            max_index = i;
        }
    }
    return max_index;
}

// D: return the action with the highest upper bound on the probability/utility
//    if multiple events in the distribution have the same highest upper bound, 
//    choose randomly between them
int CBeliefDistribution::GetMaxHiBoundEvent() {

    // store a vector with the indices that had the max highest bound
    std::vector<int> max_indexs;
    max_indexs.push_back(-1);
    float max_probability = 0;
    for(size_t i = 0; i < _probability_hi_bound.size(); i++) {
        if(_probability_hi_bound[i] > max_probability) {
            max_probability = _probability_hi_bound[i];
            max_indexs.clear();
            max_indexs.push_back(i);
        } else if(_probability_hi_bound[i] == max_probability) {
            // if we have another maximum store it in the vector
            max_indexs.push_back(i);
        }
    }

    // now return randomly from that vector
    if(max_indexs.size() == 1)
        return max_indexs[0];
    else
        return max_indexs[rand() % max_indexs.size()];
}

// D: return the event according to an epsilon-greedy policy
int CBeliefDistribution::GetEpsilonGreedyEvent(float epsilon) {
    float random = rand() / (float)(RAND_MAX - 1);
    if(random > epsilon) {
        // if outside of epsilon, then choose the mode
        return GetModeEvent();
    } else {
        // if within epsilon, then randomly choose between the
        // non-mode events
        int mode = GetModeEvent();
        
        // check that there are other actions
        int valid_event_num = GetValidEventsNumber();
        if( valid_event_num <= 1)
            return mode;

        // if we have other actions, choose one of them
        int temp = rand() % (GetValidEventsNumber() - 1);
        // find the first valid non-mode event
        int choose = 0;
        while((choose == mode) || (_probabilitys[choose] == INVALID_EVENT))
            choose++;

        // now count down from iTemp and for each count, find the next 
        // valid event 
        while(temp) {
            temp--;
            choose++;
            while((choose == mode) || (_probabilitys[choose] == INVALID_EVENT))
                choose++;
        }

        // finally, return the chosen event
        return choose;
    }
}

// D: return the event according to a soft-max policy
int CBeliefDistribution::GetSoftMaxEvent(float temperature) {
    // compute the exponentiated values in a temporary belief distribution
    CBeliefDistribution bd_temp(_probabilitys.size());
    for(size_t i = 0; i < _probabilitys.size(); i++) 
        if(_probabilitys[i] == INVALID_EVENT) {
            bd_temp[i] = INVALID_EVENT;
        } else {
            bd_temp[i] = (float)(exp(_probabilitys[i]/temperature));
        }
    // normalize
    bd_temp.Normalize();
    // then get a randomly drawn event
    return bd_temp.GetRandomlyDrawnEvent();
}

// D: Randomly draw an event from the probability/utility distribution
int CBeliefDistribution::GetRandomlyDrawnEvent() {
    float accumulator = 0;
    float random = rand() / (float)(RAND_MAX - 1);    
    Normalize();
    int last = -1;
    for(size_t i = 0; i < _probabilitys.size(); i++) {
        if(_probabilitys[i] != INVALID_EVENT) {
            accumulator += _probabilitys[i];
            last = i;
        }
        if(accumulator > random) return i;
    }
    // o/w return the last event
    return last;
}

}
