#include <string>
#include <list>

/* The main point of entry, which spins up a server, and responds to user requests. */
int main(int argc, char** argv);
void printUsageAndExit();

void serverListen(int portNum);

void listenLoop(int listenSocket);

void handleClient(int clientSocket);

/* Take in a string representing an HTTP request, and return a string
   representing it's response, based on the actions taken. */
std::string handleRequest(const std::string& request);
