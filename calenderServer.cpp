#include "calenderServer.h"
#include "serverActions.h"
#include "persistentState.h"
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <thread>
#include <algorithm>
#include <string.h>
#include <unistd.h>

using namespace std;

int main(int argc, char** argv){
  // Grab the port number from the command line, handling bad inputs.
  int portNum;
  if (argc != 2)
    printUsageAndExit();

  try{
    portNum = stoi(argv[1]);
  } catch(invalid_argument e){
    cerr << "Not a valid number!" << endl;
    printUsageAndExit();
  } catch(out_of_range e){
    cerr << "Port must be between 1 and 65535!" << endl;
    printUsageAndExit();
  }
  if (portNum < 1 || portNum > 65535){
    cerr << "Port must be between 1 and 65535!" << endl;
    printUsageAndExit();
  }

  // Load persistent server state.
  init();

  // Listen for and handle incoming connections.
  serverListen(portNum);

  // Save persistent state. This particular call will actually never
  // happen, since we loop until the user forcefully quits us at
  // present, but luckily we also call shutdown() when we get
  // interrupted with a ctrl-C.
  shutdown();
}

void printUsageAndExit(){
  cerr << "Usage: ./calenderServer portNum, where portNum is the number of the port to listen on. " << endl;
  exit(1);
}

void serverListen(int portNum){
  // Lots of structure set up.
  struct addrinfo hints;
  struct addrinfo* serverInfo;
  char portBuf[6];
  int listenSocket;
  
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  // Convert the int port to a string.
  sprintf(portBuf, "%d", portNum);

  // Set up sockets
  if (getaddrinfo(NULL, portBuf, &hints, &serverInfo) != 0){
    cerr << "Problem getting address info!" << endl;
    exit(1);
  }

  if((listenSocket = socket(serverInfo->ai_family, serverInfo->ai_socktype, serverInfo->ai_protocol)) == -1){
    cerr << "Couldn't get a system socket!" << endl;
    exit(1);
  }

  if(bind(listenSocket, serverInfo->ai_addr, serverInfo->ai_addrlen) == -1){
    cerr << "Couldn't bind the socket!" << endl;
    exit(1);
  }

  freeaddrinfo(serverInfo);

  if(listen(listenSocket, 10) == -1){
    cerr << "Couldn't listen on socket!" << endl;
    exit(1);
  }

  // Loop listening for clients until someone stops us.
  listenLoop(listenSocket);
}

void listenLoop(int listenSocket){
  struct sockaddr_storage clientAddr;
  socklen_t cAddrSize = sizeof(clientAddr);
  list<thread> clientThreads;
  int clientSocket;

  while(true){
    if((clientSocket = accept(listenSocket, (struct sockaddr *)&clientAddr, &cAddrSize)) == -1){
      cerr << "Failed to accept client." << endl;
      continue;
    }
    clientThreads.push_front(thread(handleClient, clientSocket));
  }
}
const int BUFFERSIZE = 513;
void handleClient(int clientSocket){
  // Buffer to hold part of the request.
  char requestBuffer[BUFFERSIZE];
  // c++ strings for the request and response.
  string request;
  string response;
  // How many bytes we received this call.
  int bytesReceived;
  // How many bytes we've sent total.
  unsigned bytesSent;
  // How many bytes we've sent this call.
  int sending;

  while(true){
    // Recieve a single request.
    bytesReceived = recv(clientSocket, requestBuffer, BUFFERSIZE, 0);
    while(bytesReceived == BUFFERSIZE){
      request.append(requestBuffer);
      bytesReceived = recv(clientSocket, requestBuffer, BUFFERSIZE, 0);
    }
    if (bytesReceived <= 0) break;

    // Get the response.
    response = handleRequest(request);

    // Reply
    bytesSent = 0;
    while(bytesSent < response.length()){
      sending = send(clientSocket, response.c_str(), response.length(), 0);
      if (sending == -1){
	cerr << "Failed to respond to client. " << endl;
	break;
      }
      bytesSent += sending;
    }

    // If the client didn't close the connection, we'll reply
    // again. Otherwise, we'll just try to receive andf then break.
  }
  
  close(clientSocket);
}

string handleRequest(const string& request){
  return "";
}
