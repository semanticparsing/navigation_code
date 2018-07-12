
#ifndef ROBOT_PUBLIC_UTILS_PUNC_OPER_H
#define ROBOT_PUBLIC_UTILS_PUNC_OPER_H

#include <map>
#include <string>
#include "Common/Base/hashTables.h"
#include "Common/Base/basictypes.h"
#include "Common/Util/Registerer/singleton.h"

namespace robot {

class PuncOper {
public:
    PuncOper() { inited_ = false; }
	~PuncOper() {}
public:
    bool Init();

	bool IsPunc(const std::string& key);
    void ReplacePunc(const std::string& replacer, std::string* data);

private:
    bool InitPuncSet(const std::string& punc_file);

private:
    common::hash_set<std::string> punc_set_;
    bool inited_;

    DISALLOW_COPY_AND_ASSIGN(PuncOper);
};

typedef Singleton<PuncOper> SingletonPuncOper;

} // namespace robot

#endif // ROBOT_PUBLIC_UTILS_PUNC_OPER_H
