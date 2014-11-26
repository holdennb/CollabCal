#include "group.h"
#include <fstream>
#include <stdexcept>
#include <algorithm>

using namespace std;

long Group::ID_COUNTER = 0;

Group::Group(const std::list<pair<long,bool>> &initialUsers) :
  id(ID_COUNTER++), userInfos(initialUsers){}

Group::Group(const long id,
	     const std::list<pair<long,bool>> &initialUsers) :
  id(id), userInfos(initialUsers){}

const long Group::getID(){ return id; }

const string getName(){ return name;}

const list<long>* Group::getUserIDs(){
  auto result = new list<long>();
  transform(userInfos.begin(), userInfos.end(), result->begin(),
	    [](pair<long,bool> userInfo){ return userInfo.first; });
  return result;
}

const bool Group::userCanWrite(const long userID){
  for(auto i = userInfos.begin(); i != userInfos.end(); ++i){
    if (i->first == userID)
      return i->second;
  }
  return false;
}

const list<long>* Group::getEventIDs() { return new list<long>(events); }

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

const bool Group::writeToFile(const string &filename){
  ofstream file;
  file.open(filename);
  if(file.fail())
    return false;
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

  long id;
  list<pair<long,bool>> userInfos;

  // Open the file
  file.open(filename);
  if(file.fail())
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
  return new Group(id, userInfos);
}
