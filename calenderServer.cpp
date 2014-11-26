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
    if (checkingGroup.getName() == name)
      return checkingGroup.getID();
  }
  return -1;
}

long eventIdByName(const string &name){
  Event* checkingEvent;
  for(auto it = eventFileMap.begin(); it != groupFileMap.end(); ++it){
    checkingEvent = lookupEvent(it->first);
    if (checkingEvent.getName() == name)
      return checkingGroup.getID();
  }
  return -1;
}

long eventIdByName(const long userID, const string &name){
  Event* checkingEvent;
  User* user = lookupUser(userID);
  list<long> eventIDs = user->getEventIDs();
  for(auto it = eventIDs.begin(); it != eventIDs.end(); ++it){
    checkingEvent = lookupEvent(*it);
    if (checkingEvent.getName() == name)
      return checkingEvent.getID();
  }
  return -1;
}

const unsigned int USER_CACHESIZE = 10;
const unsigned int GROUP_CACHESIZE = 10;
const unsigned int EVENT_CACHESIZE = 10;

map<long, pair<User*, long>> userCache;
map<long, pair<Group*, long>> groupCache;
map<long, pair<Event*, long>> eventCache;

/* Okay, I haven't put that many comments in the implementation, but
   for sanitys sake I'm going to comment this one, since it's a
   doozy. The other two lookup functions are basically the same thing,
   so I'm only going to comment this one, and those comments also
   apply to the others.*/
User* lookupUser(const long userID){
  // First thing, we check if we already have this user object cached.
  auto cacheIt = userCache.find(userID);
  // If we do, we can just return the cached version, and we're done.
  if (cacheIt != map::end)
    return cacheIt->second.first;

  // If not, we want to first make room in the cache for a new object,
  // by kicking out the oldest one.
  for(auto it = userCache.begin(); it != userCache.end(); ++it){
    // The second member of the pair that ID's map to is sort of like
    // a TTL (time to live) counter, in that it starts at cachesize,
    // and is decremented every time a new object is added, so that
    // after ten objects are added, the oldest will have TTL 1, and
    // then get kicked out when it's next decremented.
    if(--it->second.second < 1){
      // Now that we've decided to kick out this element, we need to
      // figure out if we still have a file mapping for it.  If not,
      // that means that that object was deleted already, so there's
      // no need to try to save it's state to a file.
      auto mapIt = userFileMap.find(it->first);
      if (mapIt != map::end)
	// If we do have a file mapping, we want to write out it's
	// current state, since any actions taken on it since it was
	// last pulled are in memory, but not in the file.
	it->second.first->writeToFile(mapIt->second);
      // Since we, the cache, own the pointers to these objects, we
      // now need to clean it up, and finally remove the item from the
      // cache.
      delete it->second.first;
      userCache.erase(it);
    }
  }
  // Next, we need to try to find the mapping for the file of the new
  // object we want to pull in. This might not succeed, since the
  // client could have an old ID for a user that has since been
  // deleted, so wee need to check for that, and return nullptr if so.
  auto mapIt = userFileMap.find(userID);
  if (mapIt == map::end)
    return nullptr;
  // Here, we finally load in the new User, taking ownership of the
  // pointer to it. Then, we update the cache with this new object,
  // and return the new object. We're done!
  User* loadedUser = User::readFromFile(mapIt->second);
  userCache[userID] = pair<User*, long>(loadedUser, USER_CACHESIZE);
  return loadedUser;
}

Group* lookupGroup(const long groupID){
  auto cacheIt = groupCache.find(groupID);
  if (cacheIt != map::end)
    return cacheIt->second.first;
  for(auto it = groupCache.begin(); it != groupCache.end(); ++it){
    if(--it->second.second < 1){
      auto mapIt = groupFileMap.find(it->first);
      if (mapIt != map::end)
	it->second.first->writeToFile(mapIt->second);
      delete it->second.first;
      groupCache.erase(it);
    }
  }
  auto mapIt = groupFileMap.find(groupID);
  if (mapIt == map::end)
    return nullptr;
  Group* loadedGroup = Group::readFromFile(mapIt->second);
  groupCache[groupID] = pair<Group*, long>(loadedGroup, GROUP_CACHESIZE);
  return loadedGroup;
}

Event* lookupEvent(const eventID){
  auto cacheIt = eventCache.find(eventID);
  if (cacheIt != map::end)
    return cacheIt->second.first;
  for(auto it = groupCache.begin(); it != groupCache.end(); ++it){
    if(--it->second.second < 1){
      auto mapIt = eventFileMap.find(it->first);
      if (mapIt != map::end)
	it->second.first->writeToFile(mapIt);
      delete it->second.first;
      eventCache.erase(it);
    }
  }
  auto mapIt = eventFileMap.find(eventID);
  if (mapIt == map::end)
    return nullptr;
  Event* loadedEvent = Event::readFromFile(mapIt->second);
  eventCache[eventID] = pair<Event*, long>(loadedEvent, EVENT_CACHESIZE);
  return loadedEvent;
}
