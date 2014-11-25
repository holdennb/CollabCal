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

void makeUser(string username, string password){
  User newUser(password);
  string userFileName = userDir.append(username);
  if(!User.writeToFile(userFileName)){
    cerr << "could not create user " << username << ": problem writing user file!" << endl;
    return;
  }
  userFileMap[newUser.getID()] = userFileName;
}

long long login(string username, string password){
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

bool userChangePassword(long userID, const string &oldPassword, const string &newPassword){
  User* user = lookupUser(userID);
  if (user == nullptr)
    return false;
  return user->setPassword(oldPassword, newPassword)
}

bool deleteUser(long userID){
  auto it = userFileMap.find(userID);
  if (it == map::end)
    return false;
  userFileMap.remove(*it);
  return true;
}

void makeGroup(long userID, string groupName){
  Group newGroup(list<pair<long,bool> >(pair<long,bool>(userID, true)));
  string groupFilename = groupDir.append(groupName);
  if(!Group.writeToFile(groupFilename)){
    cerr << "could not create group " << groupName << ": problem writing group file!" << endl;
    return;
  }
  groupFileMap[newGroup.getID()] = groupFilename;
}
