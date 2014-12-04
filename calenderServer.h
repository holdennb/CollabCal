#include "user.h"
#include "group.h"
#include "event.h"
#include "objectCache.h"
#include <string>
#include <list>

/* The main point of entry, which spins up a server, and responds to user requests. */
int main(int argc, char** argv);

void* handleClient(void* in);
void init();
void shutdown();

/* Dump the object cache to files. */
void dumpCache();

/* Save the file indices which map object ID's to files. */
void saveFileIndices();

/* Allow us to parse the files containing the index of existing users,
   groups, and events on init, if they exist.*/
void parseUserFile();
void parseGroupFile();
void parseEventFile();
