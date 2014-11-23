#ifndef _CALENDER_H_
#define _CALENDER_H_
#include <string>
#include <ctime>
#include <list>

class User{
 private:
  static long int ID_COUNTER = 0;
  long int id;
  std::string password;
  std::list<pair<long,char>> eventsInfo;
 public:
  long int getID();
  bool hasPassword(std::string password);
  bool setPassword(std::string oldPassword, std::string newPassword);
  std::list<long> getEventIDs();
  void addEvent(long eventID, bool canWrite);
  void removeEvent(long eventID);
  bool setCanWrite(long eventID, bool canWrite);
  bool canWrite(long eventID);
  bool writeToFile(std::string filename);
  static User* readFromFile(std::string filename);
}

class Group{
 private:
  static long int ID_COUNTER = 0;
  long int id;
  std::list<User> users;
 public:
  Group();
  Group(std::list<User> initialUsers);
  long int getID();
  std::list<User> getUsers();
  bool addUser(User usr);
  bool removeUser(User usr);
  bool writeToFile(std::string filename);
}

class Event{
 private:
  static long int ID_COUNTER = 0;
  std::string name;
  time_t time;
  long int id;
  std::list<Group> groups;
 public:
  Event(std::string name, time_t time, std::list<Group> groups);
  std::string getName();
  time_t getTime();
  long int getID();
  std::list<Group> getGroups();
  bool addToGroup(Group grp);
  bool removeFromGroup(Group grp);
  void reschedule(time_t newTime);
  void rename(std::string newName);
  bool writeToFile(std::string filename);
}

#endif
