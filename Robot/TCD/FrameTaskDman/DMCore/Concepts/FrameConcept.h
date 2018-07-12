// Author : lzy
// Email  : liuzhengyong@trio.ai
// Date   : 2017-09-05 15:09
// Description: 框架类数据 

#ifndef _FRAME_CONCEPT_H__
#define _FRAME_CONCEPT_H__

#include "Robot/TCD/FrameTaskDman/Utils/Common.h"
#include "StructConcept.h"

namespace TrioTDM {

/**
 * @brief 框架类数据
 */
class CFrameConcept : public CStructConcept {

public:
    CFrameConcept(const std::string& name = "NONAME", TConceptSource source = csUser);
    CFrameConcept(CStructConcept& concept);
    ~CFrameConcept();

    virtual bool operator == (CConcept& concept);
    virtual bool operator < (CConcept& concept);
    virtual bool operator > (CConcept& concept);
    virtual bool operator <= (CConcept& concept);
    virtual bool operator >= (CConcept& concept);
	
    virtual CConcept& operator [] (const std::string& index) { return CStructConcept::operator[](index); };
	virtual CConcept& operator [] (int index) { return CStructConcept::operator[](index); };
	virtual CConcept& operator [] (const char* index) { return CStructConcept::operator[](index);};

    virtual CConcept& operator = (CConcept& concept) { return CStructConcept::operator = (concept); };
    virtual CConcept& operator = (int value) { return CStructConcept::operator = (value); };
    virtual CConcept& operator = (bool value){ return CStructConcept::operator = (value); };
    virtual CConcept& operator = (float value){ return CStructConcept::operator = (value); };
    virtual CConcept& operator = (double value){ return CStructConcept::operator = (value); };
    virtual CConcept& operator = (const std::string& value) { return CStructConcept::operator = (value); };
    virtual CConcept& operator = (const char* value) {return CStructConcept::operator = (value); };
    // Clear the contents of the concept
    virtual void Clear();

    // Clear the current contents of the concept
    virtual void ClearCurrentValue();

	//---------------------------------------------------------------------
	// Overwritten methods implementing various types of updates in the 
	// naive probabilistic update scheme
	//---------------------------------------------------------------------

    // assign from another std::string
	virtual void Update_NPU_AssignFromString(const void* update_data);

	// update with another concept
    virtual void Update_NPU_UpdateWithConcept(const void* update_data);

    // update the value of a concept, so that it collapses to the mode
    // with probability 0.95 (this is used mostly on reopens)
    virtual void Update_NPU_CollapseToMode(const void* update_data);

	// update the partial value of a concept (i.e. value obtained through
	// a partial event such as an utterance still in progress)
	virtual void Update_PartialFromString(const void* update_data);

	//---------------------------------------------------------------------
	// Overwritten methods implementing various types of updates in the 
	// calista belief updating scheme
	//---------------------------------------------------------------------

	// update with another concept
    virtual void Update_Calista_UpdateWithConcept(const void* update_data);

    // update the value of a concept, so that it collapses to the mode
    virtual void Update_Calista_CollapseToMode(const void* update_data);

	//---------------------------------------------------------------------
	// Overwritten methods implementing various flags (state information) 
    // on the concept
	//---------------------------------------------------------------------
    
    // check if the concept is updated (there is at least a current value)
    virtual bool IsUpdated();

    // check if the concept is updated and grounded
    virtual bool IsUpdatedAndGrounded();

	// check if the concept is available (we have a current value or a 
    // history value)
	virtual bool IsAvailable();

    // check that the concept is available and grounded
	virtual bool IsAvailableAndGrounded();

	// check if the concept is currently undergoing grounding
    virtual bool IsUndergoingGrounding();
    

	//---------------------------------------------------------------------
	// Ovewritten methods implementing conversions to std::string format
	//---------------------------------------------------------------------
    
	// Generate a std::string representation of the grounded hypothesis (the top
	// one, if the concept is grounded)
	virtual std::string GroundedHypToString();

	// Generate a std::string representation of the top hypothesis
	virtual std::string TopHypToString();

    // Generate a std::string representation of the set of hypotheses
	virtual std::string HypSetToString();

	//---------------------------------------------------------------------
    // Overwritten methods related to the grounding model
	//---------------------------------------------------------------------
    
    // create a grounding model for this concept
    virtual void CreateGroundingModel(const std::string& sGroundingModelSpec);

    //---------------------------------------------------------------------
    // Methods related to invalidating a concept
	//---------------------------------------------------------------------

	// set the invalidated flag
	virtual void SetInvalidatedFlag(bool is_invalidated = true);

	// get the invalidated flag
	virtual bool GetInvalidatedFlag();

    //---------------------------------------------------------------------
    // Methods related to restoring a concept for grounding
	//---------------------------------------------------------------------

	// set the restored for grounding flag
	virtual void SetRestoredForGroundingFlag(
	    bool is_restored_for_grounding = true);
	
	// get the restored for grounding flag
	virtual bool GetRestoredForGroundingFlag();

	//---------------------------------------------------------------------
    // Overwritten methods related to the current hypotheses set and belief 
    // manipulation
	//---------------------------------------------------------------------

    // factory method for hypotheses
    virtual CHyp* HypFactory();

    // add a hypothesis to the current set (return the index)
    virtual int AddHyp(CHyp* hyp);

	// add a new hypothesis to the current set (return the index)
    virtual int AddNewHyp();

    // add a NULL hypothesis to the current set (return the index)
    virtual int AddNullHyp();

    // sets a hypothesis into a location (location has to contain a 
    // null-hyp and the operation actually copies)
    virtual void SetHyp(int index, CHyp* hyp);

    // sets a hypothesis into a location
    virtual void SetNullHyp(int index);

	// deletes a hypothesis
	virtual void DeleteHyp(int index);

	// return the hypothesis at a given index
	virtual CHyp* GetHyp(int index);

    // finds the index of a given hypothesis
    virtual int GetHypIndex(CHyp* hyp);

    // return the confidence of a certain hypothesis (specified by index)
	virtual float GetHypConfidence(int index);

    // return the top hypothesis
    virtual CHyp* GetTopHyp();

    // return the top hyp index
    virtual int GetTopHypIndex();

    // return the confidence score of the top hypothesis
    virtual float GetTopHypConfidence();

    // check if a hypothesis is valid (confidence score > 0; valus is not 
	// NULL)
	virtual bool IsValidHyp(int index);

    // return the total number of hypotheses for a concept (including 
    // NULLs)
	virtual int GetNumHyps();

    // return the number of valid hypotheses for a concept
	virtual int GetNumValidHyps();

    // clear the current set of hypotheses for the concept
    virtual void ClearCurrentHypSet();

    // copies the current hypset from another concept
    virtual void CopyCurrentHypSetFrom(CConcept& concept);
    
    // clears the explicitly confirmed and disconfirmed hyps
    // 
    virtual void ClearExplicitlyConfirmedHyp();    
    virtual void ClearExplicitlyDisconfirmedHyp();    

	//---------------------------------------------------------------------
    // Overwritten methods providing access to turn last updated 
    // information
	//---------------------------------------------------------------------

    // return the turn when the concept was last updated
    virtual int GetTurnLastUpdated();

    //---------------------------------------------------------------------
	// Overwritten methods for concept history manipulation
	//---------------------------------------------------------------------

    // reopen the concept (i.e. push the current value into the history)
    virtual void ReOpen();

    // clear the history of a concept
    virtual void ClearHistory();

    // construct a new concept by merging over the history of this concept
    virtual CConcept* CreateMergedHistoryConcept();

    // merge history of the concept back into the current value
    virtual void MergeHistory();

protected:

	//---------------------------------------------------------------------
	// Protected virtual methods implementing various types of updates
	//---------------------------------------------------------------------

	void UpdateFromString(const void* update_data, const std::string& update_type);

};

#define DEFINE_FRAME_CONCEPT_TYPE(FrameConceptTypeName, OTHER_CONTENTS) \
	class FrameConceptTypeName: public CFrameConcept {\
	public:\
		FrameConceptTypeName(string name = "NONAME",\
							 TConceptSource source = csUser):\
			CFrameConcept(name, source) {\
			CreateStructure();\
		};\
		virtual CConcept* EmptyClone() {\
			return new FrameConceptTypeName;\
		};\
        virtual CConcept& operator = (CConcept& concept) { return CFrameConcept::operator = (concept); };\
        virtual CConcept& operator = (int value) { return CFrameConcept::operator = (value); };\
        virtual CConcept& operator = (bool value){ return CFrameConcept::operator = (value); };\
        virtual CConcept& operator = (float value){ return CFrameConcept::operator = (value); };\
        virtual CConcept& operator = (double value){ return CFrameConcept::operator = (value); };\
        virtual CConcept& operator = (const std::string& value) { return CFrameConcept::operator = (value); };\
        virtual CConcept& operator = (const char* value) {return CFrameConcept::operator = (value); };\
		OTHER_CONTENTS\
	};\

}

#endif
