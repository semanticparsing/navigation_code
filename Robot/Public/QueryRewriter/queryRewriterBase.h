#ifndef PUBLIC_QUERYREWRITER_QUERYREWRITERBASE_H
#define PUBLIC_QUERYREWRITER_QUERYREWRITERBASE_H

#include <string>
#include "Common/Base/trio.h"
#include "Common/Util/Registerer/registerer.h"
#include "Robot/Public/serviceHead.h"
#include "Robot/Public/Utils/trioLog.h"
#include "Common/ThirdParty/glog/logging.h"

namespace robot {

class TrioDataBundle;

class QueryRewriter;
class QueryRewriterBase {
public:
    enum Type
    {
        PRE_COMPLETE,       //��ȫǰ����������ѯ����
        POST_COMPETE,       //��ȫ�󣬷־�ǰ����ÿ��MULTI�Ĳ�ѯ��������
        PRE_SEGMENT,        //�־�󣬷ִ�ǰ����ÿ���־䴦��
        POST_SEGMENT,       //�ִʺ󣬶�ÿ���ִʺ�ķ־������ִʴ���
    };
public:
    QueryRewriterBase() {}
    virtual ~QueryRewriterBase(){}

    virtual bool Init(QueryRewriter* p) { 
		VLOG(TRIO_LOG_ERR) << "query rewriter not implement interface of init" << endl; 
		return false;
	}
    virtual bool InitVoid() { 
		VLOG(TRIO_LOG_ERR) << "query rewriter not implement interface of initvoid" << endl; 
		return false;
	}
    virtual Type GetType() = 0;
    virtual void Apply(std::string& data) {
        VLOG(TRIO_LOG_ERR) << "query rewriter not implement interface of string" << endl;
    }
    virtual void ApplySentence(RewriteSentence& data) {
        VLOG(TRIO_LOG_ERR) << "query rewriter not implement interface of rewrite sentence" << endl;
    }

private:
    DISALLOW_COPY_AND_ASSIGN(QueryRewriterBase);

};

REGISTER_REGISTERER(QueryRewriterBase);
#define REGISTER_REWRITER_PROCESSHANDLER(name) REGISTER_CLASS(QueryRewriterBase, name)

} // namespace preprocess

#endif // ROBOT_INTERFACE_DOC_PROCESS_HANDLER_H
