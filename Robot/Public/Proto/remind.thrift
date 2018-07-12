namespace cpp robot
namespace py robot.remind

enum EventType {
    UNKNOWN           = 0;
    ADD               = 1;
    DELETE            = 2;
    LIST              = 3;
    EXIT              = 4;
}

enum RemindType {
    UNKNOWN = 0;
    ALARM_CLOCK = 1;
    MEMORANDUM = 2;
    COUNTDOWN = 3;
}

// expand status according to the functional requirement
enum EventStatus {
  IDLE      = 0;
  INIT      = 1;
  TIME_SET  = 2;
  DEL_ACK   = 3;
  // TODO(foxconn&trio) status trans of delete and show
}

struct TimeInfo {
  1: optional string hour;
  2: optional string minute;
  3: optional string second;
}

struct DateInfo {
  1: optional string year;
  2: optional string month;
  3: optional string day;
}

// struct RelativeDateTime {
//   1: optional i32 year;
//   2: optional i32 month;
//   3: optional i32 day;
//   4: optional i32 hour;
//   5: optional i32 minute;
//   6: optional i32 second;
//   7: optional i32 week;
//   8: optional i32 day_of_week;
// }

enum Frequency {
  NEVER         = 0;
  DAILY         = 1;
  WEEKLY        = 2;
  MONTHLY       = 3;
  YEARLY        = 4;
  CUSTOM        = 5;  // The second Sunday in May every year, or spring festival
}

// ###################CUSTOM Frequency Option####################
struct YearlyOption {
  1: optional i32 month_of_year;
  2: optional i32 day_of_month;
  3: optional i32 week_of_month;
  4: optional i32 week_of_year;
  5: optional i32 day_of_week;
}

enum CalendarType {
  GREGORIAN   = 0;
  LUNAR       = 1;
}

enum Festival {
  MOTHERS_DAY = 1;
  SPRING_FESTIVAL = 2;
}

struct CustomRepeat {
  1: optional Festival    festival;
  2: optional Frequency   frequency;
  3: optional i32         every_n_cycle = 1;  // eg:every 3 month
  4: optional CalendarType  calendar = CalendarType.GREGORIAN;
  5: optional list<YearlyOption>  yearly_option;
}

struct DateOption {
  1: optional Frequency   frequency = Frequency.NEVER;
  2: optional list<string>   days_in_cycle;  // for Frequency=WEEKLY/MONTHLY
  3: optional DateInfo    start_date;  // for Frequency=NEVER/YEARLY
  4: optional DateInfo    end_date;  // for Frequency except NEVER
  5: optional CustomRepeat custom_option; // for Frequency=CUSTOM
}
 
struct RemindEvent {
  1: optional string      event_uid;
  2: optional string      event_content;
  3: optional EventType   event_type;
  4: optional TimeInfo    time;
  5: optional DateOption  date;
  6: optional EventStatus status;
  7: optional bool        is_completed;
  8: optional RemindType  remind_type;
  9: optional string      time_stamp;
  // 10: optional RelativeDateTime relative_time;
  10: optional list<string>  selection;
  11: optional string     timeval_sec;
}

struct RemindContext {
  1: optional EventType   event_type;
  2: optional TimeInfo    time;
  3: optional DateOption  date;
  4: optional EventStatus status = EventStatus.IDLE;
  5: optional RemindType  remind_type;
  6: optional string      event_content;
  7: optional string      am_or_pm;
  // 8: optional RelativeDateTime relative_time;
  9: optional i16         failures = 0;
  10: optional list<string>  selection;
  11: optional string     timeval_sec;
}
