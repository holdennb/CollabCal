#include "user.h"
#include <fstream>
#include <algorithm>
#include <stdexcept>

using namespace std;

long User::ID_COUNTER = 0;

User::User(const string &initialPassword) :
  User(initialPassword,ID_COUNTER++){
}

User::User(const string &initialPassword, const long existingID) :
  id(existingID),
  password(initialPassword),
  eventsInfo(list<pair<long,bool>>()){
}

const long User::getID(){
  return id;
}

const bool User::hasPassword(const string &password){
  return this->password == password;
}

bool User::setPassword(const string &oldPassword, const string &newPassword){
  if (!hasPassword(oldPassword))
    return false;
  password = newPassword;
  return true;
}

std::list<long>* const User::getEventIDs(){
  auto result = new list<long>();
  transform(eventsInfo.begin(), eventsInfo.end(), result->begin(),
	    [](pair<long,bool> eventInfo){ return eventInfo.first;});
  return result;
}

void User::addEvent(const long eventID, const bool canWrite){
  eventsInfo.push_front(pair<long, bool>(eventID, canWrite));
}

void User::removeEvent(const long eventID){
  eventsInfo.remove_if([eventID](pair<long,bool> eventInfo){ return eventInfo.second == eventID; });
}

void User::setCanWrite(const long eventID, const bool canWrite){
  for_each(eventsInfo.begin(), eventsInfo.end(),
	   [eventID, canWrite](pair<long,bool> eventInfo){
	     if (eventInfo.first == eventID)
	       eventInfo.second = canWrite;
	   });
}

const bool User::canWrite(const long eventID){
  for(auto i = eventsInfo.begin(); i != eventsInfo.end(); i++){
    if (i->first == eventID)
      return i->second;
  }
  return false;
}

const bool User::writeToFile(const string &filename){
  ofstream file;
  file.open(filename);
  if(file.fail())
    return false;
  file << id << endl;
  file << password << endl;
  for_each(eventsInfo.begin(), eventsInfo.end(),
	   [&file](pair<long,bool> eventInfo){
	     file << eventInfo.first;
	     if (eventInfo.second)
	       file << " w";
	     file << endl;
	   });
  file.close();
  return true;
}

User* User::readFromFile(const string &filename){
  ifstream file;
  
  string idLine;
  string eventLine;
  
  string password;
  long id;
  User* result;

  // Open the file
  file.open(filename);
  if(file.fail())
    return nullptr;

  // Read the user ID
  if(!getline(file,idLine))
    return nullptr;
  try{
    id = stol(idLine);
  } catch(invalid_argument e){
    return nullptr;
  } catch(out_of_range e){
    return nullptr;
  }

  // Read the password
  if(!getline(file,password))
    return nullptr;

  // Allocate a user object on the heap
  result = new User(password,id);

  // Read the events list.
  while(getline(file,eventLine)){
    size_t endOfID = eventLine.find(' ');
    long eventID;
    if(endOfID == string::npos){
      try{
	eventID = stol(eventLine);
      } catch(invalid_argument e){
	return nullptr;
      } catch(out_of_range e){
	return nullptr;
      }
      result->addEvent(eventID,false);
      continue;
    }
    try{
      eventID = stol(eventLine.substr(0,endOfID));
    } catch(invalid_argument e){
      return nullptr;
    } catch(out_of_range e){
      return nullptr;
    }
    result->addEvent(eventID,true);
  }
  file.close();
  
  return result;
}
