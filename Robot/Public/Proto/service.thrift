namespace cpp robot
namespace py robot.service
namespace java robot

include "struct.thrift"

service NewsServing {
    struct.DataBundle NewsProcess(1: struct.DataBundle bundle);
}

service GfNewsServing {
    struct.TrioDataBundle GfNewsProcess(1: struct.TrioDataBundle bundle);
}

service GfFuturesServing {
    struct.TrioDataBundle GfFuturesProcess(1: struct.TrioDataBundle bundle);
}


service ChatServing {
    struct.DataBundle ChatProcess(1: struct.DataBundle bundle);
}

service QueryPostServing {
    struct.DataBundle QueryPostProcess(1: struct.DataBundle bundle);
}

service ThriftExampleServing {
    struct.DataBundle ThriftExampleProcess(1: struct.DataBundle bundle);
}

service BrainServing {
    struct.DataBundle BrainProcess(1: struct.DataBundle bundle);
}

service VerticalServing {
    struct.DataBundle VerticalProcess(1: struct.DataBundle bundle);
}

service PornFilterServing {
    struct.DataBundle PornFilterProcess(1: struct.DataBundle bundle);
}

service JokeServing {
    struct.DataBundle JokeProcess(1: struct.DataBundle bundle);
}

service SingleRoundServing {
    struct.DataBundle SingleRoundProcess(1: struct.DataBundle bundle);
}

service SysProfileServing {
    struct.DataBundle SysProfileProcess(1: struct.DataBundle bundle);
}

service WeatherServing {
    struct.TrioDataBundle WeatherProcess(1: struct.TrioDataBundle bundle);
}
service TrioWeatherServing {
    struct.TrioDataBundle TrioWeatherProcess(1: struct.TrioDataBundle bundle);
}


service QaServing {
    struct.DataBundle QaProcess(1: struct.DataBundle bundle);
}

service QueryReplyServing {
    struct.DataBundle QueryReplyProcess(1: struct.DataBundle bundle);
}

service TopicServing {
    struct.DataBundle TopicProcess(1: struct.DataBundle bundle);
}

service FaqServing {
    struct.DataBundle FaqProcess(1: struct.DataBundle bundle);
}

service FaqApiServing {
    struct.DataBundle FaqApiProcess(1: struct.DataBundle bundle);
}

service TrioXFaqApiServing{
	struct.TrioDataBundle TrioXFaqApiProcess(1: struct.TrioDataBundle request);
}

service SimilarityServing {
    struct.SimilarityOutput SimilarityProcess(1: struct.SimilarityInput input);
    struct.SimilarityOutput CompareMode(1: struct.SimilarityInput input);
    struct.SimilarityOutput DumpPost(1: struct.SimilarityInput input);
    struct.SimilarityOutput DumpPostList();
    struct.SimilarityOutput DumpList(1: struct.SimilarityInput input);
    struct.SimilarityOutput DumpSynonyms(1: struct.SimilarityInput input);
    struct.SimilarityOutput DumpMustHave(1: struct.SimilarityInput input);
    struct.SimilarityOutput DumpP2QMustHave(1: struct.SimilarityInput input);
    struct.SimilarityOutput DumpExtend(1: struct.SimilarityInput input);
    struct.CustomerData CustomerCreate(1: struct.CustomerData input);
    struct.CustomerData CustomerDelete(1: struct.CustomerData input);
    struct.PostData PostCreateOrUpdate(1: struct.PostData input);
    struct.PostData PostDelete(1: struct.PostData input);
    struct.PostData PostUpdate(1: struct.PostData input);
    struct.ListData ListCreateOrUpdate(1: struct.ListData input);
    struct.ListData ListDelete(1: struct.ListData input);
    struct.TrioDataBundle WordsDistance(1: struct.TrioDataBundle input);
}

service KnowledgeServing {
    struct.TrioDataBundle KnowledgeProcess(1: struct.TrioDataBundle bundle);
}

service CalculatorServing {
    struct.DataBundle CalculatorProcess(1: struct.DataBundle bundle);
}

service AskDatetimeServing {
    struct.DataBundle AskDatetimeProcess(1: struct.DataBundle bundle);
}

service GenerativeChatServing {
    struct.DataBundle GenerativeChatProcess(1: struct.DataBundle bundle);
}

service SNewsServing {
    struct.DataBundle SNewsProcess(1: struct.DataBundle bundle);
}

service WorkflowServing {
    struct.DataBundle WorkflowProcess(1: struct.DataBundle bundle);
}

service MiParserServing {
    struct.DataBundle MiParserProcess(1: struct.DataBundle bundle);
}

service VideoServing {
    struct.DataBundle VideoProcess(1: struct.DataBundle bundle);
}

service InfoQueryServing {
    struct.DataBundle InfoQueryProcess(1: struct.DataBundle bundle);
}

service TrioTopicServing {
    struct.TrioDataBundle TrioTopicProcess(1: struct.TrioDataBundle bundle);
}

service TrioChatServing {
    struct.TrioDataBundle ChatProcess(1: struct.TrioDataBundle bundle);
}

service TrioQueryPostServing {
    struct.TrioDataBundle QueryPostProcess(1: struct.TrioDataBundle bundle);
}

service TrioQueryReplyServing {
    struct.TrioDataBundle QueryReplyProcess(1: struct.TrioDataBundle bundle);
}

service TrioBrainServing {
    struct.TrioDataBundle TrioBrainProcess(1: struct.TrioDataBundle bundle);
}

service TrioSysProfileServing {
    struct.TrioDataBundle SysProfileProcess(1: struct.TrioDataBundle bundle);
}

service TrioNewsServing {
    struct.TrioDataBundle NewsProcess(1: struct.TrioDataBundle bundle);
}

service ContextQueryPostServing {
    struct.TrioDataBundle ContextQueryPostProcess(1: struct.TrioDataBundle bundle);
}

service ContextQueryReplyServing {
    struct.TrioDataBundle ContextQueryReplyProcess(1: struct.TrioDataBundle bundle);
}

service TrioGenerativeChatServing {
    struct.TrioDataBundle TrioGenerativeChatProcess(1: struct.TrioDataBundle bundle);
}

service ChinaMobileCRSServing {
    struct.TrioDataBundle ChinaMobileCRSProcess(1: struct.TrioDataBundle bundle);
}

service TrioWordFilterServing {
    struct.WordFilterResp WordFilterProcess(1: struct.WordFilterReq request);
}

service QueryParserServing {
    struct.DataBundle QueryParserProcess(1: struct.DataBundle bundle);
}

service QueryUnderstandingServing {
    struct.DataBundle QueryUnderstandingProcess(1: struct.DataBundle bundle);
}

service XuhuiH5WavWorkflowServing{
    struct.TrioDataBundle XuhuiH5WavWorkflowProcess(1: struct.TrioDataBundle bundle);
}

service FacadeServing {
    struct.TrioDataBundle FacadeProcess(1: struct.TrioDataBundle bundle);
}

service MusicServing {

        struct.TrioDataBundle MusicProcess(1: struct.TrioDataBundle bundle);
}

service TrioJokeServing {

    struct.TrioDataBundle TrioJokeProcess(1: struct.TrioDataBundle bundle);
}

service GuangfaBrainServing {
    struct.TrioDataBundle GuangfaBrainProcess(1: struct.TrioDataBundle bundle);
}

service TrioAudioServing {
    struct.TrioDataBundle TrioAudioProcess(1: struct.TrioDataBundle bundle);
}

service TrioTakeOutFoodServing {

    struct.TrioDataBundle TrioTakeOutFoodProcess(1: struct.TrioDataBundle bundle);
}

service TrioMovieTicketServing {

    struct.TrioDataBundle TrioMovieTicketProcess(1: struct.TrioDataBundle bundle);
}

service TrioAudioChildServing {
    struct.TrioDataBundle AudioChildProcess(1: struct.TrioDataBundle bundle);
}

service VideoControllerServing {
    struct.DataBundle VideoControllerProcess(1: struct.DataBundle bundle);
}

service TrioSysControlServing {
    struct.TrioDataBundle TrioSysControlProcess(1: struct.TrioDataBundle bundle);
}

service TrioIotDeviceControlServing {
    struct.TrioDataBundle TrioIotDeviceControlProcess(1: struct.TrioDataBundle bundle);
}

service TrioSkillServing {
    struct.TrioDataBundle TrioSkillProcess(1: struct.TrioDataBundle bundle);
    bool UpdateSkill(1:string skillId);
}

service TrioRemindServing {
    struct.TrioDataBundle TrioRemindProcess(1: struct.TrioDataBundle bundle);
}

service SkillTestBrainServing {
    struct.TrioDataBundle SkillTestBrainProcess(1: struct.TrioDataBundle
                                                bundle,2:string skillName);
}

service ShoppingServing {

    struct.TrioDataBundle ShoppingProcess(1: struct.TrioDataBundle bundle);
}

service NerServing {
    struct.NerResp NerProcess(1: struct.NerReq req);
}

service ContentAggregationServing {
    struct.TrioDataBundle ContentAggregationProcess(1: struct.TrioDataBundle bundle);
}

service TimeDateServing {

    struct.TrioDataBundle TimeDateProcess(1: struct.TrioDataBundle bundle);
}
