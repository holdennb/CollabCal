#include "group.h"
#include <fstream>
#include <stdexcept>
#include <algorithm>

using namespace std;

long Group::ID_COUNTER = 0;

Group::Group(const string &name, const std::list<pair<long,bool>> &initialUsers) :
  id(ID_COUNTER++), name(name), userInfos(initialUsers){}

Group::Group(const long id,
	     const string &name,
	     const std::list<pair<long,bool>> &initialUsers) :
  id(id), name(name), userInfos(initialUsers){}

long Group::getID() const { return id; }

string Group::getName() const { return name;}

list<long>* Group::getUserIDs() const {
  auto result = new list<long>();
  for (auto userInfo: userInfos) {
    result->push_back(userInfo.first);
  }
  return result;
}

bool Group::userCanWrite(const long userID) const {
  for(auto i = userInfos.begin(); i != userInfos.end(); ++i){
    if (i->first == userID)
      return i->second;
  }
  return false;
}

list<long>* Group::getEventIDs() const { return new list<long>(events); }

void Group::rename(const string &newName){ name = newName; }

bool Group::addUser(const long userID, const bool canWrite){
  for(auto i = userInfos.begin(); i != userInfos.end(); ++i){
    if(i->first == userID)
      return false;
  }
  userInfos.push_front(pair<long,bool>(userID, canWrite));
  return true;
}

bool Group::removeUser(const long userID){
  for(auto i = userInfos.begin(); i != userInfos.end(); ++i){
    if(i->first == userID){
      userInfos.remove(*i);
      return true;
    }
  }
  return false;
}

void Group::addEvent(const long eventID){
  events.push_front(eventID);
}

void Group::removeEvent(const long eventID){
  events.remove(eventID);
}

bool Group::writeToFile (const string &filename) const {
  ofstream file;
  file.open(filename);
  if(file.fail())
    return false;
  file << name << endl;
  file << id << endl;
  for_each(userInfos.begin(), userInfos.end(),
	   [&file](pair<long,bool> userInfo){
	     file << userInfo.first;
	     if (userInfo.second)
	       file << " w";
	     file << endl;
	   });
  file.close();
  return true;
}

Group* Group::readFromFile(const string &filename){
  ifstream file;

  string groupIdLine;
  string userInfoLine;
  string name;

  long id;
  list<pair<long,bool>> userInfos;

  // Open the file
  file.open(filename);
  if(file.fail())
    return nullptr;

  // Read the groupName line.
  if(!getline(file,name))
    return nullptr;

  // Read the group ID
  if(!getline(file,groupIdLine))
    return nullptr;
  try{
    id = stol(groupIdLine);
  } catch(invalid_argument e){
    return nullptr;
  } catch(out_of_range e){
    return nullptr;
  }

  // Read the users IDs from the group file.
  userInfos = list<pair<long,bool>>();
  while(getline(file, userInfoLine)){
    size_t endOfID = userInfoLine.find(' ');
    long userID;
    string userIDstring;
    bool canWrite;
    if(endOfID == string::npos){
      userIDstring = userInfoLine;
      canWrite = false;
    } else {
      userIDstring = userInfoLine.substr(0,endOfID);
      canWrite = true;
    }
    try{
      userID = stol(userIDstring);
    } catch(invalid_argument e){
      return nullptr;
    } catch(out_of_range e){
      return nullptr;
    }
    userInfos.push_front(pair<long,bool>(userID, canWrite));
  }
  file.close();
  return new Group(id, name, userInfos);
}
