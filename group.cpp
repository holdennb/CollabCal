#include "group.h"
#include <fstream>
#include <stdexcept>
#include <algorithm>

using namespace std;

long Group::ID_COUNTER = 0;

Group::Group(const std::list<long> &initialUsers) :
  id(ID_COUNTER++), users(initialUsers){}

Group::Group(const long id,
	     const std::list<long> &initialUsers) :
  id(id), users(initialUsers){}

const long Group::getID(){ return id; }

const list<long> Group::getUserIDs(){ return users; }

template <typename T> bool listContains(const list<T> &haystack, const T &needle){
  for(auto i = haystack.begin(); i != haystack.end(); ++i){
    if (*i == needle)
      return true;
  }
  return false;
}

bool Group::addUser(const long userID){
  if (listContains<long>(users, userID))
    return false;
  users.push_front(userID);
  return true;
}

bool Group::removeUser(const long userID){
  if (!listContains<long>(users, userID))
    return false;
  users.remove(userID);
  return true;
}

const bool Group::writeToFile(const string &filename){
  ofstream file;
  file.open(filename);
  if(file.fail())
    return false;
  file << id << endl;
  for_each(users.begin(), users.end(),
	   [&file](long userID){
	     file << userID << endl;
	   });
  file.close();
  return true;
}

Group* Group::readFromFile(const string &filename){
  ifstream file;

  string groupIdLine;
  string userIdLine;

  long id;
  list<long> userIDs;
  Group* result;

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

  userIDs = list<long>();
  while(getline(file, userIdLine)){
    try{
      userIDs.push_front(stol(userIdLine));
    } catch(invalid_argument e){
      return nullptr;
    } catch(out_of_range e){
      return nullptr;
    }
  }
  return new Group(id, userIDs);
}
