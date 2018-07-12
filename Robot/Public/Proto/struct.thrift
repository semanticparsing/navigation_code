namespace cpp robot
namespace java robot
namespace py robot.struct

include "control.thrift"
include "remind.thrift"
include "common.thrift"
include "userProfile.thrift"
include "weatherInfo.thrift"
include "musicInfo.thrift"
include "audioInfo.thrift"
include "takeout.thrift"
include "newsInfo.thrift"


struct WordFilterReq {
    // see definition in Robot/Public/Proto/serviceParams.proto
    1: optional i32 policy_type;
    2: optional list<string> datas;
    3: optional string white_category = "default";
}

struct WordFilterResp {
    // true if filtered
    1: optional list<bool> results;
}

struct NerReq {
    1: optional list<string> querys;
}

struct NerSingleResp {
    1: optional string resp;
}

struct NerResp {
    1: optional map<string, NerSingleResp> resps;
}

struct NewsReply {
    1: optional string title;
    2: optional string content;
    3: optional list<string> images;
    4: optional string url;
}

struct AudioReply {
    1: optional common.Domain domain_type;
    2: optional string audio_file_path;
    3: optional string parent_name;
    4: optional string title;
    5: optional double score;
    6: optional string cover_picture;
}

struct AudioChildReply {
    1: optional string title;
    2: optional string album_title;
    3: optional string album_image;
    4: optional string author;
    5: optional string url;
}

struct Reply {
    1: optional string reply;
    2: optional i16 emotion_id;
    3: optional list<string> reply_tags;
    4: optional list<string> reply_basic_seg;
    5: optional list<string> reply_phrase_seg;
    6: optional i16 emoji_id;
    7: optional i16 emoji_num;
    8: optional double emoji_score;
    9: optional i16 slots;
    10: optional double score;
    11: optional map<string, double> feature_score;
    12: optional map<string, map<string, double> > feat_context_scores;
    // just for some relevance computions between this reply and some sentence.

    20: optional bool is_porn;
    21: optional list<string> porn_score;
    22: optional string url;

    // for reply in suggest
    23: optional string answer;

	// for detail reply related info
	30: optional string detail_info;
    31: optional string post_segment;
    32: optional string is_original;
    33: optional string type;
    34: optional string postid;
    35: optional string original_post;
    36: optional string original_post_seg;
    37: optional list<string> synonyms;

    // for foxconn vertival
    40: optional NewsReply news;
    41: optional AudioReply audio_reply;
    42: optional AudioChildReply audiochild_reply;

    // for TrioR
    43: optional string ui_reply;
}

struct SkillReply {
    1: optional string reply;
    2: optional string ui_reply;
    3: optional i16 emotion;
    //4: optional control.Instruction sportInstraction;
    5: optional musicInfo.music music;
}

struct PostReply {
    1: optional string raw_post;
    3: optional double score;
    4: optional list<Reply> replys;
    5: optional map<i16, i16> post_tags;
    6: optional list<string> nostopwords_seg_post;
    7: optional map<string, double> feature_score;

    11: optional string debug_info;
    12: optional double max_score;
    13: optional list<string> post_basic_seg;
    14: optional list<string> post_phrase_seg;
}

struct ContextPostReply {
    1: optional string raw_curr_post;
    2: optional string raw_last_reply;
    3: optional string raw_last_post;
    4: optional double score;
    5: optional list<Reply> replys;
    6: optional map<i16, i16> post_tags;
    7: optional list<string> nostopwords_seg_post;
    8: optional map<string, double> feature_score;
    9: optional double es_score;

    11: optional string debug_info;
    12: optional double max_score;
    13: optional list<string> curr_post_basic_seg;
    14: optional list<string> curr_post_phrase_seg;
    15: optional list<string> last_reply_basic_seg;
    16: optional list<string> last_reply_phrase_seg;
    17: optional list<string> last_post_basic_seg;
    18: optional list<string> last_post_phrase_seg;
}
struct Location {
    1: optional string country;
    2: optional string province;
    3: optional string city;
    4: optional string district;
    5: optional string street;
    6: optional string address;
}

struct UserInfo {
    1: optional string uid;
    2: optional i32 platform;
    3: optional string bot_name;
    4: optional Location location;
}

enum ReturnStatus {
    UNKOWN = 0;
    SUCCESS = 1;
    FAILED = 2;
    TIMEOUT = 3;
}

enum ServiceName {
    BRAIN = 0;
    CHAT = 1;
    SINGLEROUND = 2;
    QUERYPOST = 3;
    QUERYREPLY = 4;
    VERTICAL = 5;
    JOKE = 6;
    NEWS = 7;
    WEATHER = 8;
    SYSPROFILE = 9;
    QA = 10;
    TCD = 11;
    KB = 12;
    FAQ = 13;
    SNEWS = 14;
    CALCULATOR = 15;
    TIME = 16;
    FAQAPI = 17;
    GENERATIVECHAT = 18;
    WORKFLOW = 19;
    INFO_QUERY = 20;
    BLOCK = 21;
    EXTERNAL_INTERVENTION = 22;
    HOTQUERY = 23;
    QUERYPARSER = 24;
    QUERYUNDERSTANDING = 25;
    H5WavWorkflow = 26;
    FACADE = 27;
    MUSIC = 28;
    GENERATIVEHOT = 29;
    MOVIE_TICKET = 30;
    TAKEOUT_FOOD = 31;
    AUDIO_CHILD = 32;
    AUDIO = 33;
    CONTROL = 34;
    SPORT_CONTROL = 35;
    IOT_DEVICE_CONTROL = 36;
    REMIND =37;
    SKILL_TEST_BRAIN = 38;
    VIDEO = 39;
    INSTRUCTION = 40;
    SQLBUILDER = 41;
}

enum PornType {
  CHAT = 1;
  NEWS = 2;
}

struct QueryPornCondition {
    1: optional bool is_gbk;
    2: optional bool use_porn_words;
    3: optional bool remove_letter;
    4: optional bool use_general_porn_word;
    5: optional bool use_strict_porn_word;
}

struct ReplyPornCondition {
    1: optional bool is_gbk;
    3: optional bool remove_letter;
    4: optional bool filter_porn_reply_in_brain;
}

struct OneRoundQR {
    1: optional string query;
    2: optional string reply;
    3: optional string query_basic_seg;
    4: optional string query_phrase_seg;
    5: optional string reply_basic_seg;
    6: optional string reply_phrase_seg;
}

struct UserProfile {
    1: optional string uid;
    2: optional list<OneRoundQR> context_history;
}


enum ProactiveTrigger {
    EMPTY = 0;
    STAR = 1;
    VIDEO = 2;
    TODAY_IN_HISTORY = 3;
    SAY_HELLO = 4;
}




struct ProactiveReply {
    1: optional string reply;
    2: optional ProactiveTrigger proactive_trigger;
    3: optional bool is_done;
    4: optional bool is_ignore_other_reply;
    5: optional string extra_info;
}

struct Dialog {
    1: optional string user_query;
    2: optional Reply bot_reply;
    3: optional ProactiveReply bot_proactive_reply;
    4: optional i64 request_timestamp;
    5: optional i64 response_timestamp;
}

struct Session {
    1: optional UserInfo user_info;
    2: optional list<Dialog> dialog_vec;
	3: optional string raw_data;
}

struct DebugInfo {
	1: optional bool is_debug;
	2: optional string debug_message;
}


struct DataBundle {
    1: optional string query;
    2: optional Reply reply;
    3: optional bool is_terminal = false;
    4: optional string last_query;
    5: optional string last_reply;
    6: optional string user_id;
    11: optional list<string> query_basic_seg;
    12: optional list<string> query_phrase_seg;
    13: optional UserInfo user_info;
    14: optional ReturnStatus return_status;
    15: optional map<i16, i16> query_tags;
    16: optional string preprocessed_query;
    17: optional list<string> preprocessed_query_basic_seg;
    18: optional list<string> preprocessed_query_phrase_seg;
    19: optional string query_pinyin;

    21: optional list<PostReply> post_replys;
    22: optional i32 select_post_size;
    23: optional i32 select_max_reply_size;
    24: optional double post_threshold_ratio;
    25: optional list<ContextPostReply> context_post_replys;
    31: optional list<Reply> replys;
    32: optional i32 select_reply_size;
    33: optional double reply_threshold_ratio;
    34: optional double reply_resort_threshold;
	35: optional i32 suggest_count;
	36: optional i32 recall_count;
	37: optional i32 min_query_seglen;
	38: optional i32 min_post_seglen;
	39: optional double suggest_threshold_ratio;

	41: optional ServiceName reply_from_service;
    42: optional double intention_score;

    51: optional bool anti_vulgar_reply = true;
    52: optional bool strict_porn_reply = false;

    60: optional bool query_is_porn;
    61: optional list<string> porn_score;
    62: optional QueryPornCondition query_porn_con;
    63: optional ReplyPornCondition reply_porn_con;
    64: optional UserProfile user_profile;
    65: optional i32 query_complete_type = 0; // default UNKNOWN_COMPLETE.

    70: optional ProactiveReply proactive_reply;
    71: optional bool is_say_hello;

    80: optional Session session; // save this field into redis

    90: optional DebugInfo debug_info; // save the debug related information
    91: optional bool workflow_terminal;
    92: optional PornType porn_type;
    93: optional bool gene_reply = false;//default false
    94: optional bool random_reply = false;//default true
    95: optional bool is_intention_only = false;//default false
    96: optional bool vertical_enable = false;//default false
	97: optional string faq_reply_info; // save the faq reply related info
    101: optional list<string> last_query_basic_seg;
    102: optional list<string> last_query_phrase_seg;
    103: optional list<string> last_reply_basic_seg;
    104: optional list<string> last_reply_phrase_seg;

	110: optional string content_type;
	111: optional string intention;
	112: optional bool require_no_session;
}

///////////////////////////////////
// FAQ start

enum FaqContextTypes {
	NONE_CONTEXT = 0;
	COMPLETION_CONTEXT = 1;
	REPLACE_CONTEXT = 2;
}

struct FaqContextFixture {
	1: optional FaqContextTypes type = FaqContextTypes.NONE_CONTEXT;
	2: optional string pre_context_query;
	3: optional string post_context_query;
	4: optional string current_match_key;
	5: optional string previous_match_key;
	6: optional string last_query;
}

struct FaqControlParams {
	1: optional string es_index;
	2: optional bool compare_mode = false;
	3: optional i32 suggest_count = 5;
	4: optional double best_reply_threshold = 0.7;
	5: optional double suggest_reply_threshold = 0.6;
	6: optional i32 recall_count = 30;
	7: optional string default_reply;
	8: optional bool detail_info = false;
	9: optional bool enable_faq_redux = true;
	10: optional i32 min_query_seglen = 1;
	11: optional i32 min_post_seglen = 1;
    12: optional string dynamic_suggest_threshold;
}

struct FaqReplyInfo {
	1: optional double ranking_score;
	2: optional string post;
	3: optional string reply;
	4: optional string is_original;
	5: optional bool is_valid = true;
	6: optional string post_seg;
	7: optional string type;
	8: optional string original_post;
	9: optional string original_post_seg;
	10: optional string post_id;
	11: optional string detail_info;
	12: optional list<string> synonyms;
}

struct FaqDebugFixture {
	1: optional string query;
	2: optional string query_seg;
	3: optional FaqReplyInfo reply_fixture;
	4: optional list<FaqReplyInfo> ranking_fixture;
	5: optional FaqContextFixture context_fixture;
	6: optional list<string> query_synonyms;
}

// FAQ end
///////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//refactor new chat

enum RequestType {
  CHAT      = 0;
  INTENTION = 1;
}

enum IntentionType {
  CHAT = 0;
  // JOKE = 1;
  // NEWS = 2;
  // WEATHER = 3;
  QA = 4;
  // TCD = 5;
  // KB = 6;
  // FAQ = 7;
  // SNEWS = 8;
  // CALCULATOR = 9;
  // TIME = 10;
}

struct QueryIntention {
  1: optional IntentionType type;
  2: optional double score;
}

//read only, don't modify, mostly from client
struct ControlParams
{
    1: optional bool    is_say_hello = false;
    2: optional i32     qp_max_post_num = 20;
    3: optional i32     qp_max_reply_num = 200;
    4: optional double  qp_threshold = 0.7;
    5: optional i32     qr_max_reply_num = 10;
    6: optional double  qr_threshold = 0.89;
    7: optional double  qr_threshold_resort = 0.9;
    8: optional bool anti_vulgar_reply = true;
    9: optional bool strict_porn_reply = false;
    10: optional QueryPornCondition query_porn_con;
    11: optional ReplyPornCondition reply_porn_con;
    12: map<ServiceName, i32> service_priority,
    13: optional bool is_debug = false;
    14: optional i32 reply_num;
    15: optional string trace_id;
    16: optional RequestType request_type = RequestType.CHAT;
    17: optional string service_params;
    18: optional bool is_eva = false;
	19: optional FaqControlParams faq_params;
    20: optional string req_type = "brain";
}

struct RewriteWord
{
    1: optional i32 weight;   //0-100, larger important
    2: optional string word;
}

struct RewriteSentence
{
    1: optional i32 weight; //0-100, larger important, 0 stop  1 key sentence
    2: optional string senten;
    3: optional string pinyin;
    4: optional list<RewriteWord> basic_segs;
    5: optional list<RewriteWord> phrase_segs;
}

struct RewriteQuery
{
    1: optional list<RewriteSentence> sentences;
}

//generated by query preprocess module, read only for other module
struct MultiRewrite
{
    1: optional RewriteQuery raw;
    2: optional RewriteQuery prep;
    3: optional RewriteQuery auto_complete_raw;
    4: optional RewriteQuery auto_complete_prep;
    5: optional i32 auto_complete_type = 0; // default UNKNOWN_COMPLETE.
    6: optional string query_iot;

}

//one user context unit records one round dialog
struct UserContextUnit
{
    1: optional string query;
    2: optional MultiRewrite rewrite;
    3: optional i64 request_timestamp;
    4: optional i64 response_timestamp;
    5: optional list<Reply> replys;
    6: optional ProactiveReply proactive_reply;
    7: optional ServiceName reply_from_service;
}

struct Intent {
    1: optional common.Domain type;
    2: optional double decay_factor = 0.8;
    3: optional double score = 1.0;
    4: optional double threshold = 0.6;
}

struct Intents {
    1: optional map<i32, Intent> intents;
}

//redis cache user context
struct UserContext
{
    1: optional UserInfo user_info;
    2: optional list<UserContextUnit> user_context;
    3: optional FaqContextFixture faq_context_fixture;
    4: optional remind.RemindContext remind_context;
    5: optional Intents intents;
}

struct DomainFeatures {
  // slu features
  1: optional list<string>  keywords;
  2: optional i16           hit_slots;
  3: optional double        hit_score;
  4: optional double        hit_ratio;
  // result features
  10: optional bool         reply_with_rsrc;  // 是否返回资源
  11: optional bool         exact_match;      // 资源是否是完全匹配
  12: optional bool         fuzzy_match;      // 资源是否是模糊匹配
  13: optional double       fuzzy_score;      // 模糊匹配分数
  14: optional bool       rec_match;      // 资源是否是来自推荐
}


struct SlotValue {
    1: optional string name;
    2: optional string value;
    3: optional string value_normalized;
    4: optional double prob;
}


struct ActionItem {
    1: optional map<string, string> fields;
    2: optional string prompts;
}

struct Action {
    1: optional string name;
    2: optional list<ActionItem> items;

}

struct NLUResult {
    1: optional string domain_name;
    2: optional double domain_prob;
    3: optional string intent_name;
    4: optional double intent_prob;
    5: optional list<SlotValue> svps;
    6: optional list<Action> actions;

}

//////////////////////////////////////////////
// api struct start

struct RespApiCustom {
    1: required string key;
    2: required string value;
    3: optional string info;
    4: optional string value_json;
}

struct RespApiReply {
  1: required string content;
  2: optional double score;
  3: required string tts_text;
  4: optional string emotion;
  5: optional double emotion_score;
  6: optional string audio;
  7: optional string video;
  8: optional string image;
  9: optional string url;
  10: optional string action;
  // insert here
  99: optional list<RespApiCustom> custom_info;
}

struct RespApiResult {
  1: optional RespApiReply reply;
  2: optional RespApiReply proactive_reply;
}

struct RespApiData {
  1: required string intent;
  2: required double intent_score;
  3: optional i32 terminal = 0;
  4: required list<RespApiResult> results;
  99: optional list<RespApiCustom> custom_info;
}

struct RespApiParams {
  1: required i32 status; // 0 成功  -1 错误
  2: optional string msg;
  3: optional string callback_msg;
  4: required list<RespApiData> data;
}

// api struct end
//////////////////////////////////////////

//response to client
struct ResponseParams
{
    1: optional ReturnStatus return_status;
    2: optional bool is_terminal = false;
    3: optional list<Reply> replys;
    4: optional ProactiveReply proactive_reply;
    5: optional map<string, string> debug_msgs;
    6: optional list<QueryIntention> intentions;
    7: optional control.ControlInstruction control_instruction;
    8: optional remind.RemindEvent remind_event;
    9: optional list<weatherInfo.weather> weather_info;
    10: optional list<musicInfo.music> music_content;
    11: optional list<audioInfo.audio> audio_content;
    12: optional takeout.TakeoutInfo takeout_info;
    13: optional list<newsInfo.news> news_info;
    100: optional ServiceName reply_from_service;
    101: optional DomainFeatures domain_features;
    102: optional bool inent_hijack = false;
	200: optional FaqDebugFixture faq_debug_fixture;
	201: optional FaqReplyInfo faq_reply_info;
	202: optional string faq_query_seg;
	203: optional list<string> faq_query_synonyms;

    301: optional list<NLUResult> nlu_results;

}

//ContextService input
struct ContextServiceInput {
    1: optional string last_query;
    2: optional string last_reply;

    21: optional list<string> last_query_basic_seg;
    22: optional list<string> last_query_phrase_seg;
    23: optional list<string> last_reply_basic_seg;
    24: optional list<string> last_reply_phrase_seg;
}

//ChatService params
struct ChatServiceContext
{
    1: optional list<PostReply> post_replys;
    2: optional list<ContextPostReply> context_post_replys;
    3: optional ContextServiceInput context_input;
}

//Skill command
enum SkillCmd {
    Open = 0;
    Close = 1;
    Intent = 2;
}

//Skill custom data
struct SkillData {
    1: optional bool is_open;
    2: optional map<string, string> values;
}

//SkillService params
struct SkillServiceContext
{
    1: optional string skill_name;
    2: optional SkillCmd skill_cmd;
    3: optional SkillData skill_data;
    4: optional SkillReply skill_reply;
    5: optional string skill_query;
}

//service params
struct ServiceContext
{
    1: optional ServiceName reply_from_service;
    2: optional ChatServiceContext chat;
    3: optional SkillServiceContext skill;
}

struct TerminalState {
  1: optional map<common.Domain, string> state;
  2: optional common.Domain front_domain;
  3: optional string token;
  4: optional string version;
  5: optional string user_openid;
  6: optional string client_ip;
  7: optional string location;
  8: optional string os_type;
  9: optional string app_ver;
  10: optional string mac;
  11: optional string cbmsg;
  12: optional list<common.Domain> front_domains;
}

struct TrioDataBundle
{
    1: optional string query;
    2: optional UserInfo user;
    3: optional ControlParams params;
    4: optional MultiRewrite rewrite_data;
    5: optional UserContext user_ctx;
    6: optional ServiceContext ctx;
    7: optional ResponseParams resp;
    8: optional string KBtv_state;
    9: optional string priority;
    10: optional TerminalState terminal;
    11: optional string query_ner;
    12: optional RespApiParams api_resp;
}

// for reply decision, online/offline feature extract
struct TrioFeatureBundle {
    // query related
    1: required   string                query;
    2: optional   map<common.Domain, double>  intentions;
    3: optional   i64 request_timestamp;

    // context and profile
    50: optional   UserContext           user_ctx;
    51: optional   userProfile.TrioUserProfile       user_profile;

    // result record
    100: optional ServiceName           reply_from_service;
    101: optional list<ResponseParams>  resps;
}

//end refactor new chat
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
//for FAQ similarity

struct SimilarityInput
{
    1: optional string indexName;
    2: optional string query;
    3: optional string rawQuery;
    4: optional i32 cnt;
    5: optional bool log;
    6: optional string rawQuery2;
    7: optional string type;
    8: optional i32 min_query_seglen;
    9: optional i32 min_post_seglen;
}

struct SimilarityReply
{
    1: optional double score;
    2: optional string post;
    3: optional string reply;
    4: optional string detail;
    5: optional list<string> post_segment;
    6: optional bool is_original;
    7: optional string subType;
    8: optional string postid;
    9: optional string original_post;
    10:optional list<string> original_post_seg;
    11:optional list<string> synonyms_replace;
    12:optional string must_have;
    13:optional string p2q_must_have;
    14:optional string extend_post;
}

struct SimilarityCompareModeReply
{
    1: optional double score;
    2: optional string query1;
    3: optional string query2;
    4: optional string detail;
    5: optional list<string> query_1_rawSeg;
    6: optional list<string> query_1_segment;
    7: optional list<string> query_1_extend;
    8: optional string query_1_extendInfo;
    9: optional list<string> query_1_synonyms;
    10:optional list<string> query_1_synonymsInfo;
    11:optional list<string> query_2_rawSeg;
    12:optional list<string> query_2_segment;
    13:optional list<string> query_2_extend;
    14:optional string query_2_extendInfo;
    15:optional list<string> query_2_synonyms;
    16:optional list<string> query_2_synonymsInfo;
}

struct SimilarityPost
{
    1: optional string post;
    2: optional i32 segLen;
    3: optional list<string> postSeg;
    4: optional string reply;
    5: optional bool f;
    6: optional i64 st;
    7: optional i64 et;
    8: optional bool isOriginal;
    9: optional string subType;
    10:optional string postId;
    11:optional string originalPost;
    12:optional i32 rawLen;
    13:optional list<string> rawSeg;
    14:optional i32 posLen;
    15:optional list<string> posSeg;
    16:optional i32 postSegCharLen;
    17:optional list<string> postSegChar;
    18:optional i32 postCharLen;
    19:optional list<string> postChar;
}

struct SimilarityListItem
{
    1: optional string key;
    2: optional string value;
    3: optional string id;
}

struct SimilarityOutput
{
    1: optional list<SimilarityReply> replies;
    2: optional list<SimilarityPost> posts;
    3: optional list<SimilarityListItem> items;
    4: optional string rawQuery;
    5: optional list<string> raw_seg;
    6: optional list<string> query_seg;
    7: optional list<string> extend;
    8: optional list<string> extendQuery;
    9: optional list<string> synonym;
    10:optional list<string> synonymQuery;
    11:optional list<string> mustHave;
    12:optional SimilarityCompareModeReply compareReply;
    13:optional list<string> synonyms_replace;
}

struct CustomerData
{
    1: optional string id;
    2: optional string name;
    3: optional string log;
}

struct PostData
{
    1: optional string id;
    2: optional string index;
    3: optional string enabled;
    4: optional string effectTime;
    5: optional string expireTime;
    6: optional string post;
    7: optional string reply;
    8: optional string isOriginal;
    9: optional string subType;
    10:optional string postId;
    11:optional string log;
}

struct ListData
{
    1: optional string id;
    2: optional string index;
    3: optional string type;
    4: optional string key;
    5: optional string value;
    6: optional string log;
}

//end FAQ similarity
////////////////////////////////////////////////////////////////////////////////

