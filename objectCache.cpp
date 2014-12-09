#include "objectCache.h"
#include "serverFiles.h"
#include <map>
#include <algorithm>
#include <iostream>

using namespace std;

map<long, cacheRecord<User>* > userCache;
mutex userCacheManager;
map<long, cacheRecord<Group>* > groupCache;
mutex groupCacheManager;
map<long, cacheRecord<Event>* > eventCache;
mutex eventCacheManager;

/* For sanitys sake I'm going to comment this one, since it's a
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
  if (loadedEvent == nullptr){
    cerr << "Could not load event " << mapIt->second << endl;
    return lockptr<Event>(nullptr, nullptr);
  }
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
	     delete entry.second->item;
	     delete entry.second;
	   });
  for_each(groupCache.begin(), groupCache.end(),
	   [](pair<long, cacheRecord<Group>* > entry){
	     entry.second->item->writeToFile(groupFileMap[entry.first]);
	     delete entry.second->item;
	     delete entry.second;
	   });
  for_each(eventCache.begin(), eventCache.end(),
	   [](pair<long, cacheRecord<Event>* > entry){
	     entry.second->item->writeToFile(eventFileMap[entry.first]);
	     delete entry.second->item;
	     delete entry.second;
	   });
}

/* These lookup functions allow you to get object fields. */

string userNameById(const long userID){
  for(auto it = userFileMap.begin(); it != userFileMap.end(); ++it){
    if (it->first == userID)
      return it->second.substr(userDir.length());
  }
  return "";
}

long userIdByName(const string &username){
  for(auto it = userFileMap.begin(); it != userFileMap.end(); ++it){
    if (it->second.substr(userDir.length()) == username)
      return it->first;
  }
  cout << "Didn't find a user with name " << username << endl;
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
