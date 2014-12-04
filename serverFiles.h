#include <map>
#include <string>

const std::string userDir = "users/";
const std::string userIndex = "users.file";
const std::string eventDir = "events/";
const std::string eventIndex = "events.file";
const std::string groupDir = "groups/";
const std::string groupIndex = "groups.file";
extern std::map<long, std::string> userFileMap;
extern std::map<long, std::string> eventFileMap;
extern std::map<long, std::string> groupFileMap;
