#ifndef _GROUNDINGUTILS_H__
#define _GROUNDINGUTILS_H__

#include "Robot/TCD/FrameTaskDman/Utils/Common.h"
#include "Robot/TCD/FrameTaskDman/Utils/Utils.h"

#define INVALID_EVENT ((float)-1000000.203040)

namespace TrioTDM {
//-----------------------------------------------------------------------------
// D: CState - models a set of state variables describing the full state of a
//             grounding model
//-----------------------------------------------------------------------------
class CState {

private:
    //---------------------------------------------------------------------
    // private class members
    //---------------------------------------------------------------------

    // the map holding the values for the state variables
    std::map<std::string, std::string> _state_vars;

public: 
    //---------------------------------------------------------------------
    // public class members
    //---------------------------------------------------------------------

    // indexing operator
    std::string& operator[] (std::string index);

    // clear the state
    void Clear();

    void Add(std::map<std::string, std::string> s2s);

    // string conversion function
    std::string ToString();
};

//-----------------------------------------------------------------------------
// D: CBeliefDistribution - models a probability distribution over a set of 
//                          events
//-----------------------------------------------------------------------------
class CBeliefDistribution {

private:
    //---------------------------------------------------------------------
    // private class members
    //---------------------------------------------------------------------

    // the vector of probabilities for each event (action or state, or 
    // anything else
    std::vector<float> _probabilitys;

    // and the lower and upper bounds for the confidence interval on that 
    // vector
    std::vector<float> _probability_low_bounds;
    std::vector<float> _probability_hi_bound;;

public:
    //---------------------------------------------------------------------
    // Constructors and destructors
    //---------------------------------------------------------------------
    //
    CBeliefDistribution(int num_event = 0);
    CBeliefDistribution(CBeliefDistribution& belief_distribution);
    ~CBeliefDistribution();

    // Resize the event space for the distribution 
    //
    void Resize(int num_event);

    //---------------------------------------------------------------------
    // Access 
    //---------------------------------------------------------------------
    
    // Access to probabilities, via the [] operator
    //
    float& operator[] (size_t index);

    // Access to bounds
    // 
    float& LowBound(size_t index);
    float& HiBound(size_t index);

    // Obtain the number of valid events
    //
    int GetValidEventsNumber();

    //---------------------------------------------------------------------
    // Various functions for transforming the distribution
    //---------------------------------------------------------------------
    
    // Normalizes the distribution (so that the probabilities sum up to 1)
    //
    void Normalize();

    // Sharpens the distribution (by raising it to the power fPower and 
    // normalizing)
    //
    void Sharpen(float power);

    //---------------------------------------------------------------------
    // Various functions for choosing a particular event from the 
    // distribution
    //---------------------------------------------------------------------

    // Choose the event with the highest probability/utility
    //
    int GetModeEvent();

    // Choose the highest upper bound event
    // 
    int GetMaxHiBoundEvent();

    // Choose an event according to an epsilon-greedy policy
    //
    int GetEpsilonGreedyEvent(float epsilon);

    // Choose an event according to a soft-max policy
    //
    int GetSoftMaxEvent(float temperature);

    // Randomly draw an event from the given probability/utility 
    // distribution
    //
    int GetRandomlyDrawnEvent();
    
};

}
#endif // __GROUNDINGUTILS_H__
