#include "TSCertificateVerification.h"

#include "Robot/TCD/FrameTaskDman/DMCore/Core.h"
#include "Robot/TCD/FrameTaskDman/DMCore/Agents/AllAgents.h"
#include "Robot/TCD/FrameTaskDman/DialogTask/DialogTask.h"
#include "Projects/ChinaMobile/CRS/Src/Session/GainConceptApi.h"

DEFINE_int32(certificate_error_time_limit, 4, "");

#define CERTIFICATE_LENGTH 19

namespace TrioTDM {
/* 证件验证节点 */
DEFINE_AGENCY( CCertificateVerificationAgency,
    IS_MAIN_TOPIC()

    ERROR_HANDLE(FIFTH_SKIP_NT_MAIN_2)
    
    DEFINE_SUBAGENTS(
        /* 语音播报：jl0040101*/
        SUBAGENT( CertificateVerficationInform, CRequestCertificateInput, "none")
        /* 处理证件输入 */
        SUBAGENT( CircleDealCertificateInputAgency, CCircleDealCertificateInputAgency, "none")
    )
)

/* 语音播报：jl0040101*/
DEFINE_REQUEST_AGENT( CRequestCertificateInput,
    MODEL_TYPE_OPERATOR( BROADCAST | DTMF )
    MODEL_TYPE_DTMF_LENGTH( CERTIFICATE_LENGTH )
    MODEL_TYPE_END( END_POUND_SIGN | END_ASTERISK )
)

/* 处理证件输入：用户输入 */
DEFINE_AGENCY( CCircleDealCertificateInputAgency,
    EXECUTION_POLICY( START_AGAIN_LEFT_TO_RIGHT )
    SUCCEEDS_WHEN(
        C("is_right_certificate").IsUpdated() && (bool)C("is_right_certificate")
    )

    /*
     变量定义：
    is_legal_input：输入是否合法
    is_right_certificate：证件号码是否正确 
    */
    DEFINE_CONCEPTS(
        BOOL_SYSTEM_CONCEPT( is_legal_input )
        BOOL_SYSTEM_CONCEPT( is_right_certificate )
    )

    DEFINE_SUBAGENTS(
        /* 证件号码输入 */
        SUBAGENT( CertificateVerificationUserInputAgency, CCertificateVerificationUserInputAgency, "none")
        /* 获取证件的状态:[合法：不合法] */
        SUBAGENT( GainIsLegalInput, CCertificateVerificationGainIsLegalInput, "none")
        /* 处理证件的不同状态 */
        SUBAGENT( DealIsLegalInputAgency, CCertificateVerificatioDealIsLegalInputAgency, "none")
    )
)

/* 证件号码输入 */
DEFINE_AGENCY( CCertificateVerificationUserInputAgency,
    SUCCESS_POLICY_IS_SELECT()

    ON_EXECUTE_FAIL(
        TRIGGER_NO_UNDERSTAND_AND_CLEAR_UPDATE_CONCEPT
    )
    DEFINE_SUBAGENTS(
        /* 获取输入的证件号码 */
        SUBAGENT( RequestCertificate, CRequestCertificate, "none")
        /* 特殊业务处理 */
        SUBAGENT( SpecificBusinessAgency, CSpecificBusinessAgency, "none")
    )
)

/* 获取输入的证件号码 */
DEFINE_REQUEST_AGENT( CRequestCertificate,
    PRECONDITION( false )
    REQUEST_CONCEPT( certificate )
    GRAMMAR_MAPPING( "[dtmf]" )
)

/* 获取证件的状态:[合法：不合法] */
DEFINE_GAIN_CONCEPT_AGENT( CCertificateVerificationGainIsLegalInput,
    /* 输出参数 */
    CONCEPT_NAME( is_legal_input )
    /* 输入参数 */
    PARAM_NAME( certificate )
    /* 判断输入的证件号码是否合法 */
    GAIN_CONCEPT_PARAM_FUNC( &CGainConceptApi::IsLegalCertificateInput )
)

/* 处理证件的不同状态 */
DEFINE_AGENCY( CCertificateVerificatioDealIsLegalInputAgency,
    SUCCESS_POLICY_IS_SELECT()
    DEFINE_SUBAGENTS(
        /* 证件不合法 */
        SUBAGENT( NotLegalInputAgency, CCertificateVerificatioNotLegalInputAgency, "none")
        /* 证件合法 */
        SUBAGENT( LegalInputAgency, CCertificateVerificationLegalInputAgency, "none")
    )
)

/* 证件不合法 */
DEFINE_AGENCY( CCertificateVerificatioNotLegalInputAgency,
    PRECONDITION(
        C("is_legal_input").IsUpdated() && (bool)C("is_legal_input") == false
    )
    /* 如果不合法，返回到用户输入处，重新输入 */
    ON_COMPLETION(
        SetCompleted( ctStartAgain);
    )
    DEFINE_SUBAGENTS(
        /* 输入不合法，错误处理，小于5时跳转到用户输入，如果等于5跳转到nt_main2返回主菜单 需要修改，现有为3次*/
        SUBAGENT( TriggerNoUnderstand, CTriggerNoUnderstand, "none")
    )
)

/* 证件合法(合法之后进行识别结果的判断,判断依据：是否返回主菜单) */
DEFINE_AGENCY( CCertificateVerificationLegalInputAgency,
    PRECONDITION(
        C("is_legal_input").IsUpdated() && (bool)C("is_legal_input")
    )
    /*
    * 变量定义：
    * is_return_primary_menu：是否返回主菜单
    */
    DEFINE_CONCEPTS(
        //是否返回上级菜单
        BOOL_SYSTEM_CONCEPT( is_return_primary_menu )
    )
    DEFINE_SUBAGENTS(
        /* 返回主菜单状态:[返回主菜单：不返回主菜单]*/
        SUBAGENT( GainIsReturnPrimaryMenu, CCertificateVerificationGainIsReturnPrimaryMenu, "none")
        /* 处理返回主菜单的不同状态 */
        SUBAGENT( DealIsReturnPrimaryMenuAgency, CCertificateVerificationDealIsReturnPrimaryMenuAgency, "none")
    )
)

/* 返回主菜单状态:[返回主菜单：不返回主菜单]*/ 
DEFINE_GAIN_CONCEPT_AGENT( CCertificateVerificationGainIsReturnPrimaryMenu,
    CONCEPT_NAME( is_return_primary_menu )
    PARAM_NAME( certificate )
    GAIN_CONCEPT_PARAM_FUNC( &CGainConceptApi::IsInputReturnPrimaryMenu)
)

/* 处理返回主菜单的不同状态 */
DEFINE_AGENCY( CCertificateVerificationDealIsReturnPrimaryMenuAgency,
    SUCCESS_POLICY_IS_SELECT()
    DEFINE_SUBAGENTS(
        /* 返回主菜单 */
        SUBAGENT( ReturnPrimaryMenuAgency, CCertificateReturnPrimaryMenuAgency, "none")
        /* 不返回主菜单 */
        SUBAGENT( NotReturnPrimeryMenuAgency, CCertificateNotReturnPrimeryMenuAgency, "none")
    )
)

/* 返回主菜单 */
DEFINE_AGENCY( CCertificateReturnPrimaryMenuAgency,
    PRECONDITION(
        C("is_return_primary_menu").IsUpdated() && (bool)C("is_return_primary_menu")
    )
    DEFINE_SUBAGENTS(
        /* 跳转到nt_main3 */
        SUBAGENT( SkipNtMain_2, CSkipNtMain_2, "none")
    )
)

/* 不返回主菜单 */
DEFINE_AGENCY( CCertificateNotReturnPrimeryMenuAgency,
    PRECONDITION(
        C("is_return_primary_menu").IsUpdated() && (bool)C("is_return_primary_menu") == false
    )
    DEFINE_SUBAGENTS(
        /* 获取证件的状态：[正确：错误] */
        SUBAGENT( GainIsRightCertificate, CGainIsRightCertificate, "none")
        /* 处理证件的不同状态 */
        SUBAGENT( DealIsRightCertificateAgency, CDealIsRightCertificateAgency, "none")
    )
)

/* 获取证件的状态：[正确：错误] */
DEFINE_GAIN_CONCEPT_AGENT( CGainIsRightCertificate,
    CONCEPT_NAME( is_right_certificate )
    PARAM_NAME( certificate )
    GAIN_CONCEPT_PARAM_FUNC( &CGainConceptApi::IsLegalCertificateInputServer)
)

/* 处理证件的不同状态 */
DEFINE_AGENCY( CDealIsRightCertificateAgency,
    SUCCESS_POLICY_IS_SELECT()
    
    DEFINE_SUBAGENTS(
         /* 证件号码正确 */
        SUBAGENT( ExecuteRightCertificate, CExecuteRightCertificate, "none")
        /* 证件号码错误 */
        SUBAGENT( ErrorCertificateAgency, CErrorCertificateAgency, "none")
    )
)

/* 证件号码正确 */
DEFINE_EXECUTE_AGENT( CExecuteRightCertificate,
    PRECONDITION( false )
    SUCCEEDS_WHEN(
        C("is_right_certificate").IsUpdated() && (bool)C("is_right_certificate")
    )
)

/* 证件号码错误 */
DEFINE_AGENCY( CErrorCertificateAgency,
    PRECONDITION(
        C("is_right_certificate").IsUpdated() && (bool)C("is_right_certificate") == false
    )

    /* 用户输入错误后返回继续输入 */
    ON_COMPLETION(
        SetCompleted( ctStartAgain);
    )

    DEFINE_SUBAGENTS(
        /* 语音播报：jl0040102 */
        SUBAGENT( IDIncorrect, CTaskPromptReqUser, "none" )
    )
)

/* 声明节点 */
void RegisterCertificateVerification() {
    DECLARE_AGENT( CCertificateVerificationAgency)
    DECLARE_AGENT( CCertificateVerificationUserInputAgency)
    DECLARE_AGENT( CRequestCertificateInput)
    DECLARE_AGENT( CCircleDealCertificateInputAgency)
    DECLARE_AGENT( CRequestCertificate)
    DECLARE_AGENT( CCertificateVerificationGainIsLegalInput)
    DECLARE_AGENT( CCertificateVerificatioDealIsLegalInputAgency)
    DECLARE_AGENT( CCertificateVerificatioNotLegalInputAgency)
    DECLARE_AGENT( CCertificateVerificationLegalInputAgency)
    DECLARE_AGENT( CCertificateVerificationGainIsReturnPrimaryMenu)
    DECLARE_AGENT( CCertificateVerificationDealIsReturnPrimaryMenuAgency)
    DECLARE_AGENT( CCertificateReturnPrimaryMenuAgency)
    DECLARE_AGENT( CCertificateNotReturnPrimeryMenuAgency)
    DECLARE_AGENT( CGainIsRightCertificate)
    DECLARE_AGENT( CDealIsRightCertificateAgency)
    DECLARE_AGENT( CExecuteRightCertificate)
    DECLARE_AGENT( CErrorCertificateAgency)
}

}
