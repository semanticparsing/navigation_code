
#ifndef ROBOT_PUBLIC_UTILS_SINGLETON_WORDSEGTAG_H
#define ROBOT_PUBLIC_UTILS_SINGLETON_WORDSEGTAG_H

#include "Common/Util/Registerer/singleton.h"
#include "NLP/WordSegTag/src/wordsegtag.h"

namespace robot {

class WordSegPuncTagging
{
public:
	WordSegPuncTagging();
	~WordSegPuncTagging();

    bool Init();
	bool InitWithUserDict();

    bool Segment(const string& query, string &terms);
    bool Segment(const string& query, vector<string> &terms);
	bool SegmentWithTag(const string& query, string &terms);

private:
	trio::WordSegTag* wordseg_tag_;
	bool inited_;
	bool split_sentence_;
};

typedef Singleton<WordSegPuncTagging> SingletonWordSegTag;

} // namespace robot

#endif // ROBOT_PUBLIC_UTILS_SINGLETON_WORDSEGTAG_H
