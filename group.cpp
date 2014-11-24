#include "group.h"

using namespace std;

long Group::ID_COUNTER = 0;

Group::Group(const std::list<long> &initialUsers = std::list<long>()) :
  users(initialUsers), id(ID_COUNTER++){}

Group::Group(const std::list<long> &initialUsers = std::list<long>(),
      const long id) :
  users(intiailUsers), id(id) {}

long getID(){ return id; }

list<long> getUserIDs(){ return users; }

bool addUser(const long userID){
  if users.users.push_front(userID);}
