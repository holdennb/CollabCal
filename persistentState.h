#include <list>

/* Load the persistent state. */
void init();
/* Save the persistent state. */
void shutdown();
/* Close open sockets. */
void closeSockets();

/* Dump the object cache to files. */
void dumpCache();

/* Save the file indices which map object ID's to files. */
void saveFileIndices();

/* Allow us to parse the files containing the index of existing users,
   groups, and events on init, if they exist.*/
void parseUserFile();
void parseGroupFile();
void parseEventFile();

extern std::list<int> openSockets;
