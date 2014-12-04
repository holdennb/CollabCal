#include <string>
#include <list>

/* The main point of entry, which spins up a server, and responds to user requests. */
int main(int argc, char** argv);

void* handleClient(void* in);
