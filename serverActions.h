#ifndef _SERVER_ACTIONS_H
#define _SERVER_ACTIONS_H

#include "group.h"
#include "user.h"
#include "event.h"
#include <list>
#include <map>

extern std::map<long long, long> sessionMap;
extern std::map<long long, int> sessionTTLs;

/*==================================
  Visitor Actions.
  Any visitor to the site can do these things.
  ==================================*/

/* Create a new user, with a specified username and password.
   Returns the ID of the user.*/
long makeUser(const std::string &username, const std::string &password);

/* Log in with a particular username and password. Returns a sessionID,
   a random long long int which does not match any currently logged in user.
   returns -1 if there is not a user with that username and password. */
long long login(const std::string &username, const std::string &password);

/*=======================================
  User Actions.
  Only currently existing users can do these things.
  The first argument of each function is the user
  doing the thing.
  =======================================*/

/* Change a users password. Returns true on success, false if
   the given oldPassword didn't match, or the user couldn't be found. */
bool userChangePassword(const long userID, const std::string &oldPassword,
			const std::string &newPassword);

/* Remove a user from the server. Returns true upon sucess,
   false if we couldn't find the user. */
bool deleteUser(const long userID);

/* Create a new user group, with the given name.
   Initially, the only member of the group is the user who created it.
   Returns the group ID.*/
long makeGroup(const long userID, const std::string &groupName);

/* Change the name of a group. */
void renameGroup(const long userID, const long groupID, const std::string &newName);

/* Add a user to a group. This might do nothing and return false
   if the adder does not have write permissions on the group.
   Setting admin to true allows the added user to add and remove users,
   but any user in a group can create events for that group. */
bool addToGroup(const long adderID, const long addedID, const long groupID, const bool admin=false);

/* Remove a user from a group. This might do nothing and return false
   if the remover does not have write permissions on the group. */
bool removeFromGroup(const long removerID, const long removedID, const long groupID);

/* Remove a group from the server. This might do nothing and return false
   if the user does not have write permissions on the group. */
bool deleteGroup(const long userID, const long groupID);

/* Create a new event, with a name and time. Initially, only the user
   who created it will be invited to the event.
   Returns the event ID.*/
long makeEvent(const long userID, const std::string &eventName, const time_t eventTime);

/* Create a new event, with a name, time, and a group from which all
   users are invited. Returns the event ID on success. Might do
   nothing and return -1 if the user who is trying to create the event
   is not part of the group they are trying to create it for. */
long makeEvent(const long userID, const std::string &eventName, const time_t eventTime, const long groupID,
	       bool groupWritable=false);

/* Change the name of an event. */
void renameEvent(const long userID, const long eventID, const std::string &newName);

/* Change the time of an event. */
void rescheduleEvent(const long userID, const long eventID, const time_t newTime);

/* Invite another user to an event. Might do nothing and return false if
   the inviting user is not invited to the event, does not have write permissions,
   or the event does not exist.
   The canChange parameter specifies whether the invitee should be able to invite to
   and modify the event.*/
bool inviteToEvent(const long inviterID, const long inviteeID, const long eventID,
		   const bool canChange=false);

/* Delete a previously created event. Might do nothing and return false
   if the user trying to delete the event does not have write permissions,
   or was not invited to the event. */
bool deleteEvent(const long userID, const long eventID);

/* Get the events which the user is invited to. Might return a nullptr
   if the user could not be found. The client is responsible for
   freeing the result.*/
std::list<long>* getEvents(const long userID);
#endif
