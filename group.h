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
  static long ID_COUNTER;
  /* The id of this group. */
  const long id;
  /* The name of this group. */
  std::string name;
  /* The users that belong to this group, along with whether or not they can modify the group. */
  std::list<std::pair<long,bool> > userInfos;
  /* The events that are in this group. */
  std::list<long> events;
  /* Recreate an existing group with a specific ID. */
  Group(const long id,
	const std::list<std::pair<long,bool> > &initialUsers = std::list<std::pair<long,bool> >());
 public:
  /* Create a group with some initial users in it. */
  Group(const std::list<std::pair<long,bool> > &initialUsers = std::list<std::pair<long,bool> >());

  /* Get the unique identifier of this group. */
  long getID() const;
  /* Get the current name of this group. */
  std::string getName() const;
  /*
     Get a list of user IDs cooresponding to the users that belong to
     this group. The client is responsible for freeing the result.
  */
  std::list<long>* getUserIDs() const;
  /* Whether or not the user can modify this group. */
  bool userCanWrite(const long userID) const;
  /* Get a list of event IDs cooresponding to the events that are part
     of this group. */
  std::list<long>* getEventIDs() const;
  /* Change the name of the group. */
  void rename(const std::string &newName);
  /*
     Add the existing user with the given ID to the group.
     Returns false if the user is already in the group,
     true otherwise.
  */
  bool addUser(const long userID, const bool canWrite=false);
  /* 
     Remove the user with the given ID from the group.
     returns false if the user is not already in the group,
     true otherwise.
  */
  bool removeUser(const long userID);
  /* Add an event to this group. This does NOT invite the users,
     you have to do that seperately. */
  void addEvent(const long eventID);
  /* Remove an event from this group. This does NOT uninvite the users,
     you have to do that separately. */
  void removeEvent(const long eventID);
  /*
     Write the group to a file with the given filename,
     overwriting any data that was previously in the file.
     A group file consists of a line containing the group id,
     followed by a line for each user in the group containing
     the users id.
  */
  bool writeToFile(const std::string &filename) const;
  /*
     Read out a Group object from the file with the given
     filename, returning a pointer to that object.
     Returns nullptr if the file does not exist, or could not be read.
     The client is responsible for deleting the result.
  */
  static Group* readFromFile(const std::string &filename);
};
#endif
