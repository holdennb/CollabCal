#include "calenderServer.h"
#include <iostream>
#include <fstream>
#include <map>
#include <chrono>
#include <random>
#include <cstdio>
#include <algorithm>
#include <stdexcept>
#include <sys/types.h>
#include <sys/stat.h>
#include <sstream>
#include <time.h>
#include <mutex>

using namespace std;

const string userDir = "users/";
const string userIndex = "users.file";
const string eventDir = "events/";
const string eventIndex = "events.file";
const string groupDir = "groups/";
const string groupIndex = "groups.file";
map<long, string> userFileMap;
map<long, string> eventFileMap;
map<long, string> groupFileMap;
map<long long, long> sessionMap;

int main(int argc, char** argv){
  init();

  // Do stuff...

  shutdown();
}

void shutdown(){
  dumpCache();
  saveFileIndices();
}

void saveFileIndices(){
  ofstream usersFile;
  struct stat buffer;

  if(stat(userDir.c_str(), &buffer) != 0)
    if (mkdir(userDir.c_str(), 0777) != 0)
      cerr << "Could not create directory " << userDir << endl;

  usersFile.open(userDir + userIndex);
  if (usersFile.fail() || usersFile.eof()){
    cerr << "Failed to open users file!" << endl;
    return;
  }
  usersFile << User::ID_COUNTER << endl;
  for_each(userFileMap.begin(), userFileMap.end(),
	   [&usersFile](pair<long, string> filemap){
	     usersFile << filemap.first << " " << filemap.second << endl;
	   });
  usersFile.close();

  if(stat(groupDir.c_str(), &buffer) != 0)
    if (mkdir(groupDir.c_str(), 0777) != 0)
      cerr << "Could not create directory " << groupDir << endl;

  ofstream groupFile;
  groupFile.open(groupDir + groupIndex);
  if (groupFile.fail() || groupFile.eof()){
    cerr << "Failed to open group file!" << endl;
    return;
  }
  groupFile << Group::ID_COUNTER << endl;
  for_each(groupFileMap.begin(), groupFileMap.end(),
	   [&groupFile](pair<long, string> filemap){
	     groupFile << filemap.first << " " << filemap.second << endl;
	   });
  groupFile.close();

  if(stat(eventDir.c_str(), &buffer) != 0)
    if(mkdir(eventDir.c_str(), 0777) != 0)
      cerr << "Could not create directory " << eventDir << endl;

  ofstream eventFile;
  eventFile.open(eventDir + eventIndex);
  eventFile << Event::ID_COUNTER << endl;
  if (eventFile.fail() || eventFile.eof()){
    cerr << "Failed to open event file!" << endl;
    return;
  }
  for_each(eventFileMap.begin(), eventFileMap.end(),
	   [&eventFile](pair<long, string> filemap){
	     eventFile << filemap.first << " " << filemap.second << endl;
	   });
  eventFile.close();
}

void init(){
  parseUserFile();
  parseEventFile();
  parseGroupFile();
}

void parseUserFile(){
  struct stat buffer;
  string userIndexFile = userDir + userIndex;
  if (stat(userIndexFile.c_str(), &buffer) == 0){
    ifstream usersFile;
    string nextIdLine;
    string usermapLine;
    long userID;
    string userPath;
    
    usersFile.open(userIndexFile);
    if(usersFile.fail()){
      cerr << "Problem opening existing user file..." << endl;
      return;
    }
    
    if(!getline(usersFile,nextIdLine)){
      cerr << "Problem parsing existing user file: could not get first line." << endl;
      return;
    }
    try{
      User::ID_COUNTER = stol(nextIdLine);
    } catch(invalid_argument e){
      cerr << "Problem parsing existing user file: first line is not a valid integer." << endl;
      return;
    } catch(out_of_range e){
      cerr << "Problem parsing existing user file: first line is too big of a number!" << endl;
      return;
    }

    while(getline(usersFile,usermapLine)){
      stringstream usermapStream(usermapLine);
      usermapStream >> userID;
      usermapStream >> userPath;
      userFileMap[userID] = userPath;
    }

    usersFile.close();
  }
}

void parseGroupFile(){
  struct stat buffer;
  string groupIndexFile = groupDir + groupIndex;
  if (stat(groupIndexFile.c_str(), &buffer) == 0){
    ifstream groupFile;
    string nextIdLine;
    string groupmapLine;
    long groupID;
    string groupPath;
    
    groupFile.open(groupIndexFile);
    if(groupFile.fail()){
      cerr << "Problem opening existing group file..." << endl;
      return;
    }
    
    if(!getline(groupFile,nextIdLine)){
      cerr << "Problem parsing existing group file: could not get first line." << endl;
      return;
    }
    try{
      Group::ID_COUNTER = stol(nextIdLine);
    } catch(invalid_argument e){
      cerr << "Problem parsing existing group file: first line is not a valid integer." << endl;
      return;
    } catch(out_of_range e){
      cerr << "Problem parsing existing group file: first line is too big of a number!" << endl;
      return;
    }

    while(getline(groupFile,groupmapLine)){
      stringstream groupmapStream(groupmapLine);
      groupmapStream >> groupID;
      groupmapStream >> groupPath;
      groupFileMap[groupID] = groupPath;
    }

    groupFile.close();
  }
}

void parseEventFile(){
  struct stat buffer;
  string eventIndexFile = eventDir + eventIndex;
  if (stat(eventIndexFile.c_str(), &buffer) == 0){
    ifstream eventsFile;
    string nextIdLine;
    string eventmapLine;
    long eventID;
    string eventPath;
    
    eventsFile.open(eventIndexFile);
    if(eventsFile.fail()){
      cerr << "Problem opening existing event file..." << endl;
      return;
    }
    
    if(!getline(eventsFile,nextIdLine)){
      cerr << "Problem parsing existing event file: could not get first line." << endl;
      return;
    }
    try{
      Event::ID_COUNTER = stol(nextIdLine);
    } catch(invalid_argument e){
      cerr << "Problem parsing existing event file: first line is not a valid integer." << endl;
      return;
    } catch(out_of_range e){
      cerr << "Problem parsing existing event file: first line is too big of a number!" << endl;
      return;
    }

    while(getline(eventsFile,eventmapLine)){
      stringstream eventmapStream(eventmapLine);
      eventmapStream >> eventID;
      eventmapStream >> eventPath;
      eventFileMap[eventID] = eventPath;
    }

    eventsFile.close();
  }
}

long makeUser(const string &username, const string &password){
  User newUser(password);
  long userID = newUser.getID();
  string userFileName = userDir + username;
  if(!newUser.writeToFile(userFileName)){
    cerr << "could not create user " << username << ": problem writing user file!" << endl;
    return -1;
  }
  userFileMap[userID] = userFileName;
  return userID;
}

long long login(const string &username, const string &password){
  long userID = userIdByName(username);
  if(userID == -1)
    return -1;
  auto user = acquireUser(userID);
  if (user.isNull()){
    cerr << "was given a bad userID by function userIdByName!" << endl;
    return -1;
  }
  if(!user->hasPassword(password))
    return -1;

  unsigned seed = chrono::system_clock::now().time_since_epoch().count();
  linear_congruential_engine<unsigned long long, 16807, 0, 18446744073709551616ull> generator(seed);
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
  auto user = acquireUser(userID);
  if (user.isNull())
    return false;
  bool result = user->setPassword(oldPassword, newPassword);
  return result;
}

bool deleteUser(const long userID){
  // We acquire the user to make sure no one else is using it.
  auto user = acquireUser(userID);
  auto it = userFileMap.find(userID);
  if (it == userFileMap.end())
    return false;
  remove(it->second.c_str());
  userFileMap.erase(it);
  return true;
}

long makeGroup(const long userID, const string &groupName){
  Group newGroup(list<pair<long,bool> >(1,pair<long,bool>(userID, true)));
  long groupID = newGroup.getID();
  string groupFilename = groupDir + groupName;
  if(!newGroup.writeToFile(groupFilename)){
    cerr << "could not create group " << groupName << ": problem writing group file!" << endl;
    return -1;
  }
  groupFileMap[groupID] = groupFilename;
  return groupID;
}

void renameGroup(const long userID, const long groupID, const string &newName){
  auto group = acquireGroup(groupID);
  if (group.isNull() || !group->userCanWrite(userID)){
    return;
  }
  group->rename(newName);
}

bool addToGroup(const long adderID, const long addedID, const long groupID, const bool admin){
  auto group = acquireGroup(groupID);
  if (group.isNull()) return false;
  if (!group->userCanWrite(adderID)){
    return false;
  }
  auto added = acquireUser(addedID);
  if (added.isNull())
    return false;
  if (!group->addUser(addedID, admin))
    return false;
  list<long>* groupEvents = group->getEventIDs();
  for_each(groupEvents->begin(), groupEvents->end(),
	   [&added](long eventID){
	     added->addEvent(eventID, false);
	   });
  delete groupEvents;
  return true;
}

bool removeFromGroup(const long removerID, const long removedID, const long groupID){
  auto group = acquireGroup(groupID);
  if (group.isNull()) return false;
  if (!group->userCanWrite(removerID))
    return false;
  return group->removeUser(removedID);
}

bool deleteGroup(const long userID, const long groupID){
  auto group = acquireGroup(groupID);
  if (group.isNull()) return false;
  if (!group->userCanWrite(userID))
    return false;
  auto it = groupFileMap.find(userID);
  if (it == groupFileMap.end()) return false;
  remove(it->second.c_str());
  groupFileMap.erase(it);
  return true;
}

long makeEvent(const long userID, const string &eventName, const time_t eventTime){
  Event newEvent(eventName, eventTime);
  long eventID = newEvent.getID();
  string eventFilename = eventDir + eventName;
  
  auto user = acquireUser(userID);
  if (user.isNull()) return -1;
  
  if(!newEvent.writeToFile(eventFilename)){
    cerr << "could not create event " << eventName << ": problem writing group file!" << endl;
    return -1;
  }
  eventFileMap[eventID] = eventFilename;
  user->addEvent(eventID, true);
  return eventID;
}

void renameEvent(const long userID, const long eventID, const std::string &newName){
  auto event = acquireEvent(eventID);
  auto user = acquireUser(userID);
  if (event.isNull() || user.isNull() || !user->canWrite(eventID))
    return;
  event->rename(newName);
}

void rescheduleEvent(const long userID, const long eventID, const time_t newTime){
  auto event = acquireEvent(eventID);
  auto user = acquireUser(userID);
  if (event.isNull() || user.isNull() || !user->canWrite(eventID))
    return;
  event->reschedule(newTime);
}

long makeEvent(const long userID, const string &eventName, const time_t eventTime, const long groupID,
	       bool groupWritable){
  auto group = acquireGroup(groupID);
  if (group.isNull()) return -1;
  long eventID = makeEvent(userID, eventName, eventTime);
  group->addEvent(eventID);
  auto userIDs = group->getUserIDs();
  for_each(userIDs->begin(), userIDs->end(),
	   [eventID, groupWritable](long userID){
	     auto user = acquireUser(userID);
	     user->addEvent(eventID,groupWritable);
	   });
  delete userIDs;
  return eventID;
}

bool inviteToEvent(const long inviterID, const long inviteeID, const long eventID, const bool canChange){
  // We do this to prevent deadlocks if one thread has A trying to
  // invite B while another has B trying to invite A.
  if (inviterID < inviteeID){
    auto inviter = acquireUser(inviterID);
    auto invitee = acquireUser(inviteeID);

    if (inviter.isNull() || !inviter->canWrite(eventID))
      return false;
    if (invitee.isNull())
      return false;
    invitee->addEvent(eventID, canChange);
    return true;

  } else {
    auto invitee = acquireUser(inviteeID);
    auto inviter = acquireUser(inviterID);

    if (inviter.isNull() || !inviter->canWrite(eventID))
      return false;
    if (invitee.isNull())
      return false;
    invitee->addEvent(eventID, canChange);
    return true;

  }

}

bool deleteEvent(const long userID, const long eventID){
  auto event = acquireEvent(eventID);
  if(event.isNull()) return false;
  auto user = acquireUser(userID);
  if (user.isNull() || !user->canWrite(eventID))
    return false;
  auto it = eventFileMap.find(eventID);
  if(it == eventFileMap.end())
    return false;
  remove(it->second.c_str());
  eventFileMap.erase(it);
  return true;
}

list<long>* getEvents(const long userID){
  auto user = acquireUser(userID);
  return user->getEventIDs();
}

long userIdByName(const string &username){
  for(auto it = userFileMap.begin(); it != userFileMap.end(); ++it){
    if (it->second.substr(userDir.length()) == username)
      return it->first;
  }
  return -1;
}

long groupIdByName(const string &name){
  for(auto it = groupFileMap.begin(); it != groupFileMap.end(); ++it){
    auto checkingGroup = acquireGroup(it->first);
    if (checkingGroup->getName() == name)
      return checkingGroup->getID();
  }
  return -1;
}

long eventIdByName(const string &name){
  for(auto it = eventFileMap.begin(); it != groupFileMap.end(); ++it){
    auto checkingEvent = acquireEvent(it->first);
    if (checkingEvent->getName() == name)
      return checkingEvent->getID();
  }
  return -1;
}

long eventIdByName(const long userID, const string &name){
  auto user = acquireUser(userID);
  list<long>* eventIDs = user->getEventIDs();
  for(auto it = eventIDs->begin(); it != eventIDs->end(); ++it){
    auto checkingEvent = acquireEvent(*it);
    if (checkingEvent->getName() == name)
      return checkingEvent->getID();
  }
  return -1;
}

const unsigned int USER_CACHESIZE = 10;
const unsigned int GROUP_CACHESIZE = 10;
const unsigned int EVENT_CACHESIZE = 10;

template <typename T>
struct cacheRecord{
  T* item;
  unsigned ttl;
  mutex m;
};

map<long, cacheRecord<User>* > userCache;
mutex userCacheManager;
map<long, cacheRecord<Group>* > groupCache;
mutex groupCacheManager;
map<long, cacheRecord<Event>* > eventCache;
mutex eventCacheManager;

/* Okay, I haven't put that many comments in the implementation, but
   for sanitys sake I'm going to comment this one, since it's a
   doozy. The other two lookup functions are basically the same thing,
   so I'm only going to comment this one, and those comments also
   apply to the others.*/
lockptr<User> acquireUser(const long userID){
  userCacheManager.lock();
  // First thing, we check if we already have this user object cached.
  auto cacheIt = userCache.find(userID);
  // If we do, we can just lock the cached version, return it, and
  // we're done.
  if (cacheIt != userCache.end()){
    auto entry = cacheIt->second;
    userCacheManager.unlock();
    return lockptr<User>(entry->item, &(entry->m));
  }

  // If not, we want to first make room in the cache for a new object,
  // by kicking out the oldest one.
  for(auto it = userCache.begin(); it != userCache.end(); ++it){
    // Each cache entry has a ttl, which is initialized at the size of
    // the cache, and decremented every time we want to add something
    // new to the cache, so that if always we kick out objects with
    // non-positive TTL, we restrict our cache to the proper
    // size. Unfortunately, we might need to keep things in our cache
    // even if they have run out of life, because another thread might
    // still be holding on to them. This means that our cache might
    // grow above cachesize, but will never grow above
    // cachesize+number_of_threads.
    auto entry = it->second;
    if(--entry->ttl < 1 && entry->m.try_lock()){
      long entryID = it->first;
      // Now that we've decided to kick out this element, we need to
      // figure out if we still have a file mapping for it.  If not,
      // that means that that object was deleted already, so there's
      // no need to try to save it's state to a file.
      auto mapIt = userFileMap.find(entryID);
      if (mapIt != userFileMap.end())
	// If we do have a file mapping, we want to write out it's
	// current state, since any actions taken on it since it was
	// last pulled are in memory, but not in the file.
	entry->item->writeToFile(mapIt->second);
      // Since we, the cache, own the pointers to these objects, we
      // now need to clean it up, and finally remove the item from the
      // cache.
      delete entry->item;
      entry->m.unlock();
      delete entry;
      userCache.erase(it);
    }
  }
  // Next, we need to try to find the mapping for the file of the new
  // object we want to pull in. This might not succeed, since the
  // client could have an old ID for a user that has since been
  // deleted, so wee need to check for that, and return nullptr if so.
  auto mapIt = userFileMap.find(userID);
  if (mapIt == userFileMap.end()){
    userCacheManager.unlock();
    return lockptr<User>(nullptr, nullptr);
  }
  // Here, we finally load in the new User, taking ownership of the
  // pointer to it. Then, we update the cache with this new object,
  // and return the new object. We're done!
  User* loadedUser = User::readFromFile(mapIt->second);
  cacheRecord<User>* newEntry = new cacheRecord<User>();
  newEntry->item = loadedUser;
  newEntry->ttl = USER_CACHESIZE;
  userCache[userID] = newEntry;
  userCacheManager.unlock();
  return lockptr<User>(loadedUser, &(newEntry->m));
}

lockptr<Group> acquireGroup(const long groupID){
  groupCacheManager.lock();
  auto cacheIt = groupCache.find(groupID);
  if (cacheIt != groupCache.end()){
    auto entry = cacheIt->second;
    groupCacheManager.unlock();
    return lockptr<Group>(entry->item, &(entry->m));
  }
  for(auto it = groupCache.begin(); it != groupCache.end(); ++it){
    auto entry = it->second;
    if(--entry->ttl < 1 && entry->m.try_lock()){
      long entryID = it->first;
      auto mapIt = groupFileMap.find(entryID);
      if (mapIt != groupFileMap.end())
	entry->item->writeToFile(mapIt->second);
      delete entry->item;
      entry->m.unlock();
      delete entry;
      groupCache.erase(it);
    }
  }
  auto mapIt = groupFileMap.find(groupID);
  if (mapIt == groupFileMap.end()){
    groupCacheManager.unlock();
    return lockptr<Group>(nullptr, nullptr);
  }
  Group* loadedGroup = Group::readFromFile(mapIt->second);
  cacheRecord<Group>* newEntry = new cacheRecord<Group>();
  newEntry->item = loadedGroup;
  newEntry->ttl = GROUP_CACHESIZE;
  groupCache[groupID] = newEntry;
  groupCacheManager.unlock();
  return lockptr<Group>(loadedGroup, &(newEntry->m));
}

lockptr<Event> acquireEvent(const long eventID){
  eventCacheManager.lock();
  auto cacheIt = eventCache.find(eventID);
  if (cacheIt != eventCache.end()){
    auto entry = cacheIt->second;
    eventCacheManager.unlock();
    return lockptr<Event>(entry->item, &(entry->m));
  }

  for(auto it = eventCache.begin(); it != eventCache.end(); ++it){
    auto entry = it->second;
    if(entry->ttl < 1 && entry->m.try_lock()){
      long entryID = it->first;
      auto mapIt = eventFileMap.find(entryID);
      if (mapIt != eventFileMap.end())
	entry->item->writeToFile(mapIt->second);
      delete entry->item;
      entry->m.unlock();
      delete entry;
      eventCache.erase(it);
    }
  }
  auto mapIt = eventFileMap.find(eventID);
  if (mapIt == eventFileMap.end()){
    eventCacheManager.unlock();
    return lockptr<Event>(nullptr, nullptr);
  }
  Event* loadedEvent = Event::readFromFile(mapIt->second);
  cacheRecord<Event>* newEntry = new cacheRecord<Event>();
  newEntry->item = loadedEvent;
  newEntry->ttl = EVENT_CACHESIZE;
  eventCache[eventID] = newEntry;
  eventCacheManager.unlock();
  return lockptr<Event>(loadedEvent, &(newEntry->m));
}

void dumpCache(){
  for_each(userCache.begin(), userCache.end(),
	   [](pair<long, cacheRecord<User>* > entry){
	     entry.second->item->writeToFile(userFileMap[entry.first]);
	   });
  for_each(groupCache.begin(), groupCache.end(),
	   [](pair<long, cacheRecord<Group>* > entry){
	     entry.second->item->writeToFile(groupFileMap[entry.first]);
	   });
  for_each(eventCache.begin(), eventCache.end(),
	   [](pair<long, cacheRecord<Event>* > entry){
	     entry.second->item->writeToFile(eventFileMap[entry.first]);
	   });
}
