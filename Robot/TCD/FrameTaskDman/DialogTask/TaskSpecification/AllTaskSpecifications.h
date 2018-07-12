// Author : lzy
// Email  : liuzhengyong@trio.ai
// Date   : 2017-09-01 10:37
// Description: 

#ifndef _ALL_TASK_SPECIFICATIONS_H__
#define _ALL_TASK_SPECIFICATIONS_H__

#include "TSCommonAgent.h"
#include "TSWelcome.h"
#include "TSPrimaryMenu.h"
#include "TaskFlowRoot.h"
#include "TSErrorHandle.h"
#include "TSHelp.h"
#include "TSRepeat.h"
#include "TSTrasferArtificial.h"
#include "TSTrasferArtificialConfirm.h"
#include "TSTrasferIVR.h"
#include "TSOutOfServiceJudge.h"
#include "TSSendMsg.h"
#include "TSOtherMean.h"
#include "TSSystemException.h"
#include "TSNonOnlineBusiness.h"
#include "TSBalanceInquiry.h"
#include "TSRemainingTrafficInquiry.h"
#include "TSRemainingTalkMinutesInquiry.h"
#include "TSModifyPassword.h"
#include "TSModifyPasswordOfWideBand.h"
#include "TSOpenedBusinessInquiry.h"
#include "TSRechargeBill.h"
#include "TSRecuperate.h"
#include "TSCertificateVerification.h"
#include "TSNewPasswordInput.h"
#include "TSOriginalPasswordVerification.h"
#include "TSPasswordDoubleConfirm.h"
#include "TSTelephonePasswordModify.h"
#include "TSTelephonePasswordReset.h"
#include "Oppugn/TSOppugnCallCharge.h"
#include "Oppugn/TSOppugnExpenses.h"
#include "Oppugn/TSOppugnFixedFee.h"
#include "Oppugn/TSOppugnLowestSpend.h"
#include "Oppugn/TSOppugnMonthlyRent.h"
#include "Oppugn/TSOppugnNetCharge.h"
#include "Oppugn/TSOppugnPackage.h"
#include "Oppugn/TSOppugnTrafficFee.h"

namespace TrioTDM {

void RegisterAllTaskSpecificationAgents();

}

#endif
