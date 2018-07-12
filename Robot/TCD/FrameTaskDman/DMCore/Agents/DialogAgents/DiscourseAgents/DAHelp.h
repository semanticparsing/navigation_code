// Author : lzy
// Email  : liuzhengyong@trio.ai
// Date   : 2017-08-26 15:36
// Description: 

#ifndef _DA_HELP_H__
#define _DA_HELP_H__

#include "Robot/TCD/FrameTaskDman/DMCore/Core.h"
#include "Robot/TCD/FrameTaskDman/DMCore/Agents/AllAgents.h"

namespace TrioTDM {

void RegisterHelpAgent();
void RegisterHelpGetTipsAgent();
void RegisterHelpEstablishContextAgent();
void RegisterHelpWhatCanISayAgent();
void RegisterHelpSystemCapabilitiesAgent();

}

#endif
