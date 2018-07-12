namespace cpp robot
namespace py robot.userProfile
include "common.thrift"

struct InterestInfo {
  1: optional string      name;
  2: optional double      weight;
  3: optional common.Domain      domain;
}

struct DomainInterestInfo {
  1: InterestInfo mian_interest;
  2: optional list<InterestInfo> sub_interests;
  3: optional common.Domain      domain;
}

struct TrioUserProfile {
  1: optional string uid;
  2: optional list<DomainInterestInfo> interests;
}
