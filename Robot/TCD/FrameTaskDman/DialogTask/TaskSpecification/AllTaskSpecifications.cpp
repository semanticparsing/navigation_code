#include "AllTaskSpecifications.h"

namespace TrioTDM {

void RegisterAllTaskSpecificationAgents() {
    RegisterCommonAgent();
    RegisterSendMsg();
    RegisterWelcome();
    RegisterPrimaryMenu();
    RegisterTaskFlowRoot();
    RegisterTrasferIVR();
    RegisterOutOfServiceJudge();
    RegisterTrasferArtificialConfirm();
    RegisterTrasferArtificial();
    RegisterNonOnlineBusiness();
    RegisterBalanceInquiry();
    RegisterOppugnCallCharge();
    RegisterOppugnExpenses();
    RegisterOppugnFixedFee();
    RegisterOppugnLowestSpend();
    RegisterOppugnMonthlyRent();
    RegisterOppugnNetCharge();
    RegisterOppugnPackage();
    RegisterOppugnTrafficFee();
    RegisterRemainingTrafficInquiry();
    RegisterRemainingTalkMinutesInquiry();
    RegisterModifyPassword();
    RegisterModifyPasswordOfWideBand();
    RegisterOpenedBusinessInquiry();
    RegisterRechargeBill();
    RegisterRecuperate();
    RegisterTelephonePasswordReset();
    RegisterCertificateVerification();
    RegisterTelephonePasswordModify();
    RegisterOriginalPasswordVerification();
    RegisterNewPasswordInput();
    RegisterPasswordDoubleConfirm();

}

}
