#include "calenderServer.h"
#include <iostream>
#include <map>
#include <chrono>
#include <random>
#include <cstdio>

using namespace std;

string userDir = "users/";
string eventDir = "events/";
string groupDir = "groups/";
map<long, string> userFileMap;
map<long, string> eventFileMap;
map<long, string> groupFileMap;
map<long long, long> sessionMap;

int main(int argc, char** argv){
}

long makeUser(const string &username, const string &password){
  User newUser(password);
  long userID = newUser.getID();
  string userFileName = userDir.append(username);
  if(!User.writeToFile(userFileName)){
    cerr << "could not create user " << username << ": problem writing user file!" << endl;
    return;
  }
  userFileMap[userID] = userFileName;
  return userID;
}

long long login(const string &username, const string &password){
  long userID = userIdByName(username);
  if(userID == -1)
    return -1;
  User* user = lookupUser(userID);
  if (user == nullptr){
    cerr << "was given a bad userID by function userIdByName!" << endl;
    return -1;
  }
  if(!user.hasPassword(password))
    return -1;

  unsigned seed = chrono::system_clock::now().time_since_epoch().count();
  linear_congruential_engine<unsigned long long, 16807, 0, 18446744073709551616> generator(seed);
  long long sessionID;
  while(true){
    sessionID = generator();
    for(auto i = sessionMap.begin(); i != sessionMap.end(); ++i){
      if (i->first == sessionID)
	continue;
    }
    break;
  }

  sessionMap[sessionID] = userID;
  return sessionID;
}

bool userChangePassword(const long userID, const string &oldPassword, const string &newPassword){
  User* user = lookupUser(userID);
  if (user == nullptr)
    return false;
  return user->setPassword(oldPassword, newPassword)
}

bool deleteUser(const long userID){
  auto it = userFileMap.find(userID);
  if (it == map::end)
    return false;
  remove(it->second);
  userFileMap.erase(it);
  return true;
}

long makeGroup(const long userID, const string &groupName){
  Group newGroup(list<pair<long,bool> >(pair<long,bool>(userID, true)));
  long groupID = newGroup.getID();
  string groupFilename = groupDir.append(groupName);
  if(!Group.writeToFile(groupFilename)){
    cerr << "could not create group " << groupName << ": problem writing group file!" << endl;
    return;
  }
  groupFileMap[groupID] = groupFilename;
  return groupID;
}

void renameGroup(const long userID, const long groupID, const string &newName){
  Group* group = lookupGroup(groupID);
  if (group == nullptr || !group->userCanWrite(userID))
    return false;
  group->rename(newName);
}

bool addToGroup(const long adderID, const long addedID, const long groupID, const bool admin){
  Group* group = lookupGroup(groupID);
  if (group == nullptr) return false;
  if (!group->userCanWrite(adderID))
    return false;
  return group->addUser(addedID, admin);
}

bool removeFromGroup(const long removerID, const long removedID, const long groupID){
  Group* group = lookupGroup(groupID);
  if (group == nullptr) return false;
  if (!group->userCanWrite(removerID))
    return false;
  return group.removeUser(removedID);
}

bool deleteGroup(const long userID, const long groupID){
  Group* group = lookupGroup(groupID);
  if (group == nullptr) return false;
  if (!group->userCanWrite(userID))
    return false;
  auto it = groupFileMap.find(userID);
  if (it == map::end) return false;
  remove(it->second);
  groupFileMap.erase(it);
  return true;
}

long makeEvent(const long userID, const string &eventName, const time_t eventTime){
  Event newEvent(eventName, eventTime);
  long eventID = newEvent.getID();
  string eventFilename = eventDir.append(eventName);
  
  User* user = lookupUser(userID);
  if (user == nullptr) return;
  
  if(!newEvent.writeToFile(eventFilename)){
    cerr << "could not create event " << eventName << ": problem writing group file!" << endl;
    return;
  }
  eventFileMap[eventID] = eventFilename;
  user->addEvent(eventID, true);
  return eventID;
}

void renameEvent(const long userID, const long eventID, const std::string &newName){
  Event* event = lookupEvent(eventID);
  User* user = lookupUser(userID);
  if (event == nullptr || user == nullptr || !user->canWrite(eventID))
    return;
  event.rename(newName);
}

void rescheduleEvent(const long userID, const long eventID, const time_t newTime){
  Event* event = lookupEvent(eventID);
  User* user = lookupUser(userID);
  if (event == nullptr || user == nullptr || !user->canWrite(eventID))
    return;
  event.reschedule(newTime);
}

long makeEvent(const long userID, const string &eventName, const time_t eventTime, const long groupID,
	       bool groupWritable){
  Group* group = lookupGroup(groupID);
  if (group == nullptr) return false;
  long eventID = makeEvent(userID, eventName, eventTime);
  group->addEvent(eventID);
  auto userIDs = group->getUserIDs();
  for_each(userIDs.begin(), userIDs.end(),
	   [eventID](long userID){
	     User* user = lookupUser(userID);
	     user.addEvent(eventID,groupWritable);
	   });
  delete userIDs;
  return eventID;
}

bool inviteToEvent(const long inviterID, const long inviteeID, const long eventID, const bool canChange){
  User* inviter = lookupUser(inviterID);
  if (inviter == nullptr || !inviter->canWrite(eventID))
    return false;
  User* invitee = lookupUser(inviteeID);
  if (invitee == nullptr)
    return false;
  invitee->addEvent(eventID, canChange);
  return true;
}

bool deleteEvent(const long userID, const long eventID){
  User* user = lookupUser(userID);
  if (user == nullptr || !user->canWrite(eventID))
    return false;
  Event* event = lookupEvent(eventID);
  if(event == nullptr) return false;
  auto it = eventFileMap.find(eventID);
  if(it == map::end)
    return false;
  remove(it->second);
  eventFileMape.erase(it);
  return true;
}

list<long>* getEvents(const long userID){
  User* user = lookupUser(userID);
  return user->getEventIDs();
}

long userIDByName(const string &username){
  for(auto it = userFileMap.begin(); it != userFileMap.end(); ++it){
    if (it->second.substring(userDir.length()) == username)
      return it->first;
  }
  return -1;
}

long groupIdByName(const string &name){
  Group* checkingGroup;
  for(auto it = groupFileMap.begin(); it != groupFileMap.end(); ++it){
    checkingGroup = lookupGroup(it->first);
    if (checkingGroup.name
  }
}
