#include "serverActions.h"
#include "objectCache.h"
#include "serverFiles.h"
#include "renderPage.h"
#include <iostream>
#include <chrono>
#include <random>
#include <cstdio>
#include <algorithm>
#include <stdexcept>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>

using namespace std;

map<long long, long> sessionMap;

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
  Group newGroup(groupName, list<pair<long,bool> >(1,pair<long,bool>(userID, true)));
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
  cout << "Successfully added user " << addedID << " to group." << endl;
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
  cout << "Creating group event for " << group->getName() << ", group has " << userIDs->size() << " users" << endl;
  for_each(userIDs->begin(), userIDs->end(),
	   [userID, eventName, eventID, groupWritable](long userIdInGroup){
	     if (userID != userIdInGroup) {
	       cout << "Inviting group member " << userIdInGroup << " to event " << eventName << endl;
	       auto user = acquireUser(userIdInGroup);
	       user->addEvent(eventID,groupWritable);
	     }
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
