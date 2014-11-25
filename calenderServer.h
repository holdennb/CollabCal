#include "user.h"
#include "group.h"
#include "event.h"
#include <string>
#include <list>

/* The main point of entry, which spins up a server, and responds to user requests. */
int main(int argc, char** argv);

/*==================================
  Visitor Actions.
  Any visitor to the site can do these things.
  ==================================*/

/* Create a new user, with a specified username and password. */
void makeUser(std::string username, std::string password);

/* Log in with a particular username and password. Returns a sessionID,
   a random long long int which does not match any currently logged in user.
   returns -1 if there is not a user with that username and password. */
long long login(std::string username, std::string password);

/*=======================================
  User Actions.
  Only currently existing users can do these things.
  The first argument of each function is the user
  doing the thing.
  =======================================*/

/* Change a users password. Returns true on success, false if
   the given oldPassword didn't match, or the user couldn't be found. */
bool userChangePassword(long userID, const std::string &oldPassword,
			const std::string &newPassword);

/* Remove a user from the server. Returns true upon sucess,
   false if we couldn't find the user, or there was a weird
   file permission thing.*/
bool deleteUser(long userID);

/* Create a new user group, with the given name.
   Initially, the only member of the group is the user who created it. */
void makeGroup(long userID, std::string groupName);

/* Add a user to a group. This might do nothing and return false
   if the adder does not have write permissions on the group.
   Setting admin to true allows the added user to add and remove users,
   but any user in a group can create events for that group. */
bool addToGroup(long adderID, long addedID, long groupID, bool admin=false);

/* Remove a user from a group. This might do nothing and return false
   if the remover does not have write permissions on the group. */
bool removeFromGroup(long removerID, long removedID, long groupID);

/* Remove a group from the server. This might do nothing and return false
   if the user does not have write permissions on the group. */
bool deleteGroup(long userID, long groupID);

/* Create a new event, with a name and time. Initially, only the user
   who created it will be invited to the event. */
void makeEvent(long userID, std::string eventName, time_t eventTime);

/* Create a new event, with a name, time, and a group from which all users
   are invited. Might do nothing and return false if the user who is trying
   to create the event is not part of the group they are trying to create it for. */
bool makeEvent(long userID, std::string eventName, time_t eventTime, long groupID);

/* Invite another user to an event. Might do nothing and return false if
   the inviting user is not invited to the event, does not have write permissions,
   or the event does not exist.*/
bool inviteToEvent(long inviterID, long inviteeID, long eventID);

/* Delete a previously created event. Might do nothing and return false
   if the user trying to delete the event does not have write permissions,
   or was not invited to the event. */
bool deleteEvent(long userID, long eventID);

/* Get the events which the user is invited to. Might return a nullptr
   if the user could not be found. */
std::list<long>* getEvents(long userID);

/*===============================================
  Helper Functions.
  ===============================================*/

/* Given the name of a user, group, or event, lets you get it's ID.
   Returns -1 if it couldn't be found.*/
long userIdByName(std::string username);
long groupIdByName(std::string name);
long eventIdByName(std::string name);

/* Gets the object cooresponding to a user, group, or event ID.
   The pointer returned is owned by the internals of this module,
   so don't try and free it, and it may become invalid upon
   the next call to the cooresponding function,
   so don't try to store it. If you need to store something,
   just store the ID, and look it up again later. */
User* lookupUser(long userID);
Group* lookupGroup(long groupID);
Event* lookupEvent(long eventID);
