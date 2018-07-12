cc_library(
    name = "core_agents",
    srcs = [
        "DMCoreAgent.cpp",
        "GroundingManagerAgent.cpp",
        "DTTManagerAgent.cpp",
        "StateManagerAgent.cpp",
        "OutputManagerAgent.cpp",
        "TrafficManagerAgent.cpp",
        "InteractionEventManagerAgent.cpp",
    ],
    deps = [
        "//Robot/TCD/FrameTaskDman/DMCore/Agents/DialogAgents:dialog_agents",
        "//Robot/TCD/FrameTaskDman/DMCore/Agents/DialogAgents/DiscourseAgents:discourse_agents",
        #"//Robot/TCD/FrameTaskDman/DialogTask/TaskSpecification:task_specification",
        "//Robot/TCD/FrameTaskDman/DMCore/Outputs:outputs",
        '//Common/Util/File:simpleLineReader',
        '//Common/ThirdParty/jsoncpp-0.5.0/src/lib_json:jsoncpp',
        "//Robot/TCD/FrameTaskDman/DMCore/Events:events",
    ],
)
