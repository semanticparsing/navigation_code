#include "Common/ThirdParty/gtest/gtest.h"
#include "Common/Util/File/simpleLineReader.h"
#include "Robot/Public/QueryRewriter/queryRewriter.h"
#include "Robot/Public/QueryRewriter/queryRewriterBase.h"
#include "Robot/Public/serviceHead.h"
#include "Robot/Public/QueryRewriter/rewriterPolicy.h"
#include "Common/Util/ThriftClientWrapper/thrift.h"

DEFINE_string(query_rewriter_test_data_dirty, "./test/data/query.txt", "");

TEST(QueryRewriterTestSuite, CheckFunctionTest) {
	
	robot::QueryRewriter* rewriter = robot::SingletonQueryRewriter::get();
	EXPECT_EQ(NULL != rewriter, true);

    robot::RewriterPolicyDefault policy;
	rewriter->Init(&policy);

	{
		file::SimpleLineReader reader(true);
		VLOG(0) << "path: " << FLAGS_query_rewriter_test_data_dirty << endl;
		EXPECT_EQ((reader.Open(FLAGS_query_rewriter_test_data_dirty)), true);

		std::vector<std::string> lines;
		reader.ReadLines(&lines);

        std::vector<std::string> words;
        for (size_t i = 0; i < lines.size(); i++)
		{
            words.clear();
            SplitString(lines[i], '\t', &words);
            EXPECT_EQ(words.size(), 2) << "invalid line " << lines[i] << endl;
			robot::TrioDataBundle bundle;
			robot::ControlParams params;
			params.__set_is_say_hello(false);
			params.__set_anti_vulgar_reply(true);
			params.__set_strict_porn_reply(true);
			robot::QueryPornCondition qc;
			qc.__set_is_gbk(false);
			qc.__set_remove_letter(true);
			qc.__set_use_general_porn_word(true);
			qc.__set_use_porn_words(true);
			qc.__set_use_strict_porn_word(true);
			params.__set_query_porn_con(qc);
			robot::ReplyPornCondition rc;
			rc.__set_is_gbk(false);
			rc.__set_remove_letter(true);
			rc.__set_filter_porn_reply_in_brain(false);
			params.__set_reply_porn_con(rc);
			bundle.__set_params(params);
			robot::UserInfo user_info;
			user_info.__set_uid("yuhui_test");
			user_info.__set_platform(1);
			bundle.__set_user(user_info);
			robot::UserContext user_ctx;
			user_ctx.__set_user_info(user_info);
			user_ctx.__set_user_context(std::vector<robot::UserContextUnit>());
			bundle.__set_user_ctx(user_ctx);
			bundle.__set_query(words[0]);
			rewriter->Apply(bundle);

            EXPECT_EQ(bundle.rewrite_data.prep.sentences.empty(), false) << "sentence empty" << lines[i] << endl;
            EXPECT_EQ(bundle.rewrite_data.prep.sentences[0].senten == words[1], true) << "preprocess error " 
                << lines[i] << FromThriftToUtf8DebugString(&bundle.rewrite_data.prep) << endl;
		}
	}
}
