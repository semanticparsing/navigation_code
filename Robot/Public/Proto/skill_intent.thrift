namespace cpp skill_intent
namespace py skill_intent

struct Entity {
1:optional string key;
2:optional string phrase;
3:optional string value;
}

struct SkillIntentRequest {
1: string query;
2: optional string user_id;
3: optional string app_name;
}

struct SkillIntentResponse {
1: optional i32 error_code;
2: optional string error_msg;
3: optional string intent;
4: optional list<Entity> entities;
5: optional string order;
}

service SkillIntentServing {
  bool SkillIntentReload();
  SkillIntentResponse SkillIntentProcess(1: SkillIntentRequest request);
}

