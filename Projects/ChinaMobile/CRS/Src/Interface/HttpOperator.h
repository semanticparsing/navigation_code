#ifndef _HTTP_OPERATOR_H_
#define _HTTP_OPERATOR_H_

#include <string>

using namespace std;

namespace ChinaMobileProvince {

    void InitializeHttpPost2Java(const string &urlRoot);

    int SendHttpPost2Java(const string &url, const string &post_content, string &out);


} // namespace ChinaMobileProvince
#endif //_HTTP_OPERATOR_H_
