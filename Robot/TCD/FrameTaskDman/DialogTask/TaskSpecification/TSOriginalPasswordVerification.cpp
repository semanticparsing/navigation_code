#include "TSOriginalPasswordVerification.h"
#include "Robot/TCD/FrameTaskDman/DMCore/Core.h"
#include "Robot/TCD/FrameTaskDman/DMCore/Agents/AllAgents.h"
#include "Robot/TCD/FrameTaskDman/DialogTask/DialogTask.h"
#include "Projects/ChinaMobile/CRS/Src/Session/GainConceptApi.h"

DEFINE_int32(PASSWORD_LENGTH, 7, "密码长度限制");

namespace TrioTDM {

/* 原密码验证节点 */
DEFINE_AGENCY( COriginalPasswordVerificationAgency,
    IS_MAIN_TOPIC()

    DEFINE_CONCEPTS(
        BOOL_SYSTEM_CONCEPT( is_lock )
    )

    DEFINE_SUBAGENTS(
        /* 密码是否锁定 */
        SUBAGENT( GainIsPasswordLock, CGainIsPasswordLock, "none")
        /* 处理是否锁定不同分支 */
        SUBAGENT( OriginalPasswordVerificationIsLockAgency, COriginalPasswordVerificationIsLockAgency, "none")
    )
)

/* 获取密码是否锁定 */
DEFINE_GAIN_CONCEPT_AGENT( CGainIsPasswordLock,
    CONCEPT_NAME(is_lock)
    GAIN_CONCEPT_FUNC(&CGainConceptApi::IsPasswordLock)
)

/* 处理是否锁定不同分支 */
DEFINE_AGENCY( COriginalPasswordVerificationIsLockAgency,
    SUCCESS_POLICY_IS_SELECT()

    DEFINE_SUBAGENTS(
        /* 密码锁定 */
        SUBAGENT( OriginalPasswordVerificationLockAgency, COriginalPasswordVerificationLockAgency, "none")
        /* 密码未锁定 */
        SUBAGENT( OriginalPasswordVerificationNoLockAgency, COriginalPasswordVerificationNoLockAgency, "none")
    )
)

/* 密码锁定 */
DEFINE_AGENCY( COriginalPasswordVerificationLockAgency,
    PRECONDITION(
        C("is_lock").IsUpdated() && (bool)C("is_lock") == true
    )

    DEFINE_SUBAGENTS(
        /* jl0060102 */
        SUBAGENT( ErrorTimeTransfiniteInform, CTaskPromptInform, "none")
        /* 跳转到nt_main6 */
        SUBAGENT(PrimaryMenu, CNtMain6PrimaryMenuAgency, "none")
    )
)

/* 密码未锁定 */
DEFINE_AGENCY( COriginalPasswordVerificationNoLockAgency,
    PRECONDITION(
        C("is_lock").IsUpdated() && (bool)C("is_lock") == false
    )

    DEFINE_SUBAGENTS(
        /* 语音播报：jl0060101 */
        SUBAGENT( OriginalPasswordVerificationReqUser, CRequestPasswordInput, "none")
        /* 处理原始密码输入 */
        SUBAGENT( CircleDealOriginalPasswordInputAgency, CCircleDealOriginalPasswordInputAgency, "none")
    )
)

/* 语音播报：jl0060101 */
DEFINE_REQUEST_AGENT( CRequestPasswordInput,
    MODEL_TYPE_OPERATOR( BROADCAST | DTMF )
    MODEL_TYPE_DTMF_LENGTH( FLAGS_PASSWORD_LENGTH )
    MODEL_TYPE_END( END_POUND_SIGN | END_ASTERISK)
)

/* 处理原始密码输入 */
DEFINE_AGENCY( CCircleDealOriginalPasswordInputAgency,
    EXECUTION_POLICY( START_AGAIN_LEFT_TO_RIGHT )
    SUCCEEDS_WHEN(
        C("is_right_password").IsUpdated() && (bool)C("is_right_password")
    )
    /*
     * 变量定义：
     * is_right_password：密码是否正确
    */
    DEFINE_CONCEPTS(
        BOOL_SYSTEM_CONCEPT( is_right_password )
    )

    DEFINE_SUBAGENTS(
        /* 原始密码输入验证 */
        SUBAGENT( OriginalPasswordVerificationUserInputAgency, COriginalPasswordVerificationUserInputAgency, "none")
        /* 识别结果判断 */
        SUBAGENT( OriginalPasswordInputRecognitionResultsAgency, COriginalPasswordInputRecognitionResultsAgency, "none")
    )
)

/* 原始密码输入验证 */
DEFINE_AGENCY( COriginalPasswordVerificationUserInputAgency,
    SUCCESS_POLICY_IS_SELECT()

    ON_EXECUTE_FAIL(
        TRIGGER_NO_UNDERSTAND_AND_CLEAR_UPDATE_CONCEPT
    )
    DEFINE_SUBAGENTS(
        /* 获取输入的原始密码 */
        SUBAGENT( RequestOriginalPassword, CRequestOriginalPassword, "none")
        /* 错误流程验证 */
        SUBAGENT( SpecificBusinessAgency, CSpecificBusinessAgency, "none")
    )
)

/* 获取输入的原始密码 */
DEFINE_REQUEST_AGENT( CRequestOriginalPassword,
    PRECONDITION( false )
    REQUEST_CONCEPT( original_password )
    GRAMMAR_MAPPING( "[dtmf]" )
)

/* 识别结果判断 */
DEFINE_AGENCY( COriginalPasswordInputRecognitionResultsAgency,
    SUCCESS_POLICY_IS_SELECT()

    DEFINE_SUBAGENTS(
        /* 返回主菜单 */
        SUBAGENT( ReturnPrimaryMenuAgency, COriginalPasswordReturnPrimaryMenuAgency, "none")
        /* 原始密码判定流程继续 */
        SUBAGENT( OriginalPasswordContinueAgency, COriginalPasswordContinueAgency, "none")
    )
)

/* 返回主菜单 */
DEFINE_AGENCY( COriginalPasswordReturnPrimaryMenuAgency,
    PRECONDITION(
        C("original_password").IsUpdated() && (std::string)C("original_password") == "*"
    )
    DEFINE_SUBAGENTS(
        /* 跳转到nt_main2 */
        SUBAGENT( SkipNtMain_2, CSkipNtMain_2, "none")
    )
)

/* 原始密码判定流程继续 */
DEFINE_AGENCY( COriginalPasswordContinueAgency,
    PRECONDITION(
        C("original_password").IsUpdated() && (std::string)C("original_password") != "*" && (std::string)C("original_password") != "1"
    )
    DEFINE_SUBAGENTS(
        /* 获取原始密码状态：[正确：不正确] */
        SUBAGENT( GainIsRightOriginalPassword, CGainIsRightOriginalPassword, "none")
        /* 处理原始密码不同状态 */
        SUBAGENT( DealIsRightOriginalPasswordAgency, CDealIsRightOriginalPasswordAgency, "none")
    )
)

/* 获取原始密码状态：[正确：不正确] */
DEFINE_GAIN_CONCEPT_AGENT( CGainIsRightOriginalPassword,
    /* 输出参数 */
    CONCEPT_NAME( is_right_password )
    /* 输入参数 */
    PARAM_NAME( original_password )
    GAIN_CONCEPT_PARAM_FUNC( &CGainConceptApi::IsMatchedPassword )
)

/* 处理原始密码不同状态 */
DEFINE_AGENCY( CDealIsRightOriginalPasswordAgency,
    SUCCESS_POLICY_IS_SELECT()

    DEFINE_SUBAGENTS(
        /* 原始密码正确 */
        SUBAGENT( ExecuteRightOriginalPassword, CExecuteRightOriginalPassword, "none")
        /* 原始密码错误 */
        SUBAGENT( ErrorOriginalPasswordAgency, CErrorOriginalPasswordAgency, "none")
    )
)

/* 原始密码正确，继续上一个流程 */
DEFINE_EXECUTE_AGENT( CExecuteRightOriginalPassword,
    PRECONDITION( false )
    SUCCEEDS_WHEN(
        C("is_right_password").IsUpdated() && (bool)C("is_right_password")
    )
)

/* 原始密码错误 */
DEFINE_AGENCY( CErrorOriginalPasswordAgency,
    PRECONDITION(
        C("is_right_password").IsUpdated() && (bool)C("is_right_password") == false
    )
    DEFINE_CONCEPTS(
        BOOL_SYSTEM_CONCEPT( is_lock_again )
    )

    DEFINE_SUBAGENTS(
        /* 密码是否锁定 */
        SUBAGENT( GainIsPasswordLockAgain, CGainIsPasswordAgainLock, "none")
        /* 处理是否锁定不同分支 */
        SUBAGENT( OriginalPasswordErrorIsLockAgency, CErrorOriginalPasswordIsLockAgency, "none")
    )
)

/* 获取密码是否锁定 */
DEFINE_GAIN_CONCEPT_AGENT( CGainIsPasswordAgainLock,
    CONCEPT_NAME(is_lock_again)
    GAIN_CONCEPT_FUNC(&CGainConceptApi::IsPasswordLock)
)

/* 原始密码错误--处理是否锁定不同分支 */
DEFINE_AGENCY( CErrorOriginalPasswordIsLockAgency,
    SUCCESS_POLICY_IS_SELECT()

    DEFINE_SUBAGENTS(
        /* 错误次数在没有超过5次(没有锁定) */
        SUBAGENT( ErrorTimeInLimitAgency, CErrorTimeInLimitAgency, "none")
        /* 错误次数等于5次（锁定） */
        SUBAGENT( ErrorTimeTransfiniteAgency, CErrorTimeTransfiniteAgency, "none")
    )
)

/* 错误次数没有超过5次 */
DEFINE_AGENCY( CErrorTimeInLimitAgency,
    PRECONDITION(
        C("is_lock_again").IsUpdated() && (bool)C("is_lock_again") == false
    )

    DEFINE_SUBAGENTS(
        /* 语音播报： jl0060103 */
        SUBAGENT( ErrorOriginalPasswordReqUser, CErrorOriginalPasswordReqUser, "none")
    )
)
/* 语音播报： jl0060103 */
DEFINE_REQUEST_AGENT( CErrorOriginalPasswordReqUser,
    MODEL_TYPE_OPERATOR( BROADCAST | DTMF )
    MODEL_TYPE_DTMF_LENGTH( FLAGS_PASSWORD_LENGTH )
    MODEL_TYPE_END( END_POUND_SIGN | END_ASTERISK)
)

/* 错误次数等于5次 */
DEFINE_AGENCY( CErrorTimeTransfiniteAgency,
    PRECONDITION(
        C("is_lock_again").IsUpdated() && (bool)C("is_lock_again") == true
    )
    DEFINE_SUBAGENTS(
        /* jl0060102 */
        SUBAGENT( ErrorTimeTransfiniteInform, CTaskPromptInform, "none")
        /* 跳转到nt_main6 */
        SUBAGENT(PrimaryMenu, CNtMain6PrimaryMenuAgency, "none")
    )
)

/* 声明节点 */
void RegisterOriginalPasswordVerification() {
    DECLARE_AGENT( COriginalPasswordVerificationAgency)
    DECLARE_AGENT( CGainIsPasswordLock)
    DECLARE_AGENT( CGainIsPasswordAgainLock)
    DECLARE_AGENT( COriginalPasswordVerificationIsLockAgency)
    DECLARE_AGENT( COriginalPasswordVerificationLockAgency)
    DECLARE_AGENT( COriginalPasswordVerificationNoLockAgency)
    DECLARE_AGENT( CRequestPasswordInput)
    DECLARE_AGENT( CCircleDealOriginalPasswordInputAgency)
    DECLARE_AGENT( COriginalPasswordVerificationUserInputAgency)
    DECLARE_AGENT( CRequestOriginalPassword)
    DECLARE_AGENT( COriginalPasswordInputRecognitionResultsAgency)
    DECLARE_AGENT( COriginalPasswordReturnPrimaryMenuAgency)
    DECLARE_AGENT( COriginalPasswordContinueAgency)
    DECLARE_AGENT( CGainIsRightOriginalPassword)
    DECLARE_AGENT( CDealIsRightOriginalPasswordAgency)
    DECLARE_AGENT( CExecuteRightOriginalPassword)
    DECLARE_AGENT( CErrorOriginalPasswordAgency)
    DECLARE_AGENT( CErrorOriginalPasswordIsLockAgency)
    DECLARE_AGENT( CErrorTimeInLimitAgency)
    DECLARE_AGENT( CErrorOriginalPasswordReqUser)
    DECLARE_AGENT( CErrorTimeTransfiniteAgency)
}

}
