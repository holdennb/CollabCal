#include "calender.h"

using namespace std;

User::getID(){
  return id;
}

User::hasPassword(string password){
  return this.password.equals(password);
}

User::setPassword(string oldPassword, string newPassword){
  if (!hasPassword(oldPassword))
    return false;
  password = newPassword;
  return true;
}

User::getEventIDs(){
  var result = new list<long>();
  transform(eventsInfo.begin(), eventsInfo.end(), result.begin(),
	    [](pair<long,char> eventInfo){ return eventInfo.first()});
  return result;
}

User::addEvent(long eventID, bool canRead, bool canWrite){
  
}
