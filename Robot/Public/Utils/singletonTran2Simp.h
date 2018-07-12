
#ifndef ROBOT_PUBLIC_UTILS_SINGLETON_TRAN2SIMP_H
#define ROBOT_PUBLIC_UTILS_SINGLETON_TRAN2SIMP_H

#include "Common/Util/Registerer/singleton.h"
#include "Common/Base/hashTables.h"
#include <string>
#include <map>

namespace robot {

class Tran2Simp
{
public:
	Tran2Simp();
	~Tran2Simp();

    bool Init(const std::string& t2s_filename);

    void Convert(const string& tran, string& simp);

private:
	bool inited_;
    //std::map<std::string, std::string> t2s_;
    common::hash_map<std::string, std::string> t2s_;
};

typedef Singleton<Tran2Simp> SingletonTran2Simp;

} // namespace robot

#endif // ROBOT_PUBLIC_UTILS_SINGLETON_TRAN2SIMP_H
