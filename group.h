#ifndef _GROUP_H
#define _GROUP_H
#include <string>
#include <list>
/*
  A group of users. Events that are created are accessable by users
  of a particular group or groups.
 */
class Group{
 private:
  /* The next group ID to use. */
  static long int ID_COUNTER = 0;
  /* The id of this group. */
  const long int id;
  /* The users that belong to this group. */
  std::list<long int> users;
 public:
  /* Create an empty group. */
  Group();
  /* Create a group with some initial users in it. */
  Group(const std::list<long int> &initialUsers);
  /* Get the unique identifier of this group. */
  long int getID();
  /*
     Get a list of user IDs cooresponding to the users
     that belong to this group.
  */
  std::list<long int> getUsers();
  /*
     Add the existing user with the given ID to the group.
     Returns false if the user is already in the group,
     true otherwise.
  */
  bool addUser(const long int userID);
  /* 
     Remove the user with the given ID from the group.
     returns false if the user is not already in the group,
     true otherwise.
  */
  bool removeUser(const long int userID);
  /*
     Write the group to a file with the given filename,
     overwriting any data that was previously in the file.
  */
  bool writeToFile(const std::string &filename);
  /*
     Read out a Group object from the file with the given
     filename, returning a pointer to that object.
     Returns nullptr if the file does not exist, or could not be read.
  */
  static Group* readFromFile(const std::string &filename);
}
#endif
