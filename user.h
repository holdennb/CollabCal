#ifndef _USER_H
#define _USER_H
#include <string>
#include <list>

class User{
 private:
  /* The next user ID to use. */
  static long ID_COUNTER;
  /* The id of this user. */
  const long id;
  /*
    The users password. There is no way to get
    this value out of the class, for security reasons.
   */
  std::string password;
  /*
    The info of the events that this user has access to.
    The first element in the pair is the id of the event,
    and the second element is whether or not the user
    has permission to change the event.
   */
  std::list<std::pair<long, bool> > eventsInfo;
  /*
     Create a user that can not view any events,
     and has the given password,
     using the existing given id.
  */
  User(const std::string &password, const long id);

 public:
  /* Create a user that can not view any events,
     and has the given password.*/
  User(const std::string &password);
  /* Get the unique identifier of the user. */
  const long getID();
  /*
    Returns true if the given string matches the users password,
    false otherwise.
   */
  const bool hasPassword(const std::string &password);
  /*
    Set the users password. The first argument is the users
    previous password, the second is the desired new password.
    If the first argument does not match the users old
    password, false is returned, and the password is not changed.
    Otherwise, the password is set to the second argument,
    and true is returned.
   */
  bool setPassword(const std::string &oldPassword,
		   const std::string &newPassword);
  /*
    Returns a pointer to a list of event ids, where each id cooresponds to
    an event that this user can view. The client is responsible for
    freeing the memory used by this list.
   */
  const std::list<long>* getEventIDs();
  /* Mark an existing event as being viewable by this user. */
  void addEvent(const long eventID, const bool canWrite);
  /*
    Mark an event currently viewable by the user as no longer
    viewable by the user.
   */
  void removeEvent(const long eventID);
  /*
    Set whether or not the user can modify the event cooresponding
    to the given event ID.
  */
  void setCanWrite(const long eventID, const bool canWrite);
  /*
    Returns whether or not the user can modify the event
    cooresponding to the given event ID.
   */
  const bool canWrite(const long eventID);
  /*
    Write the event information a file with the given filename,
    overwriting any previous data. Returns true on success,
    false otherwise.
    The format of a user file is as follows:
    The first line is the users ID,
    The second line is the users password,
    Every subsequent line has an event id optionally followed by the 'w'
    character, if the user has write permissions for that event.
   */
  const bool writeToFile(const std::string &filename);
  /*
    Read the data from the file with the given filename,
    parsing it into a User object, and returning a pointer to the User.
    Returns nullptr if the file could not be found, or
    could not be parsed.
    The client is responsible for freeing the result.
   */
  static User* readFromFile(const std::string &filename);
};
#endif
