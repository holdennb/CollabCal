#ifndef _OBJECT_CACHE_H
#define _OBJECT_CACHE_H
#include "event.h"
#include "user.h"
#include "group.h"
#include "locking_pointer.h"
#include <mutex>

const unsigned int USER_CACHESIZE = 10;
const unsigned int GROUP_CACHESIZE = 10;
const unsigned int EVENT_CACHESIZE = 10;

template <typename T>
struct cacheRecord{
  T* item;
  unsigned ttl;
  std::mutex m;
};

/* Gets the object cooresponding to a user, group, or event ID.  The
   pointer returned is owned by the internals of this module, so don't
   try and free it, and it may become invalid upon the next call to
   the cooresponding function, so don't try to store it. If you need
   to store something, just store the ID, and look it up again
   later. Remember to release the object when you're done, so others
   can use it.*/
lockptr<User> acquireUser(const long userID);
lockptr<Group> acquireGroup(const long groupID);
lockptr<Event> acquireEvent(const long eventID);
void dumpCache();

/*===============================================
  Helper Functions.
  ===============================================*/

/* Given the name of a user, group, or event, lets you get it's ID.
   Returns -1 if it couldn't be found. These are linear in the number
   we have, so avoid using them if possible.*/
long userIdByName(const std::string &username);
long groupIdByName(const std::string &name);
long eventIdByName(const std::string &name);

/* This version scales better, since it only searches through the
   events that the user has access to. */
long eventIdByName(const long userID, const std::string &name);

#endif
