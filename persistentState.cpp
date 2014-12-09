#include "persistentState.h"
#include "objectCache.h"
#include "serverFiles.h"
#include "group.h"
#include "event.h"
#include "user.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <signal.h>
#include <unistd.h>

using namespace std;

list<int> openSockets;

void shutdown(){
  dumpCache();
  saveFileIndices();
  closeSockets();
}

void closeSockets(){
  for(int openSocket : openSockets){
    close(openSocket);
  }
}

void saveFileIndices(){
  ofstream usersFile;
  struct stat buffer;

  if(stat(userDir.c_str(), &buffer) != 0)
    if (mkdir(userDir.c_str(), 0777) != 0)
      cerr << "Could not create directory " << userDir << endl;

  usersFile.open(userDir + userIndex);
  if (usersFile.fail() || usersFile.eof()){
    cerr << "Failed to open users file!" << endl;
    return;
  }
  usersFile << User::ID_COUNTER << endl;
  for_each(userFileMap.begin(), userFileMap.end(),
	   [&usersFile](pair<long, string> filemap){
	     usersFile << filemap.first << " " << filemap.second << endl;
	   });
  usersFile.close();

  if(stat(groupDir.c_str(), &buffer) != 0)
    if (mkdir(groupDir.c_str(), 0777) != 0)
      cerr << "Could not create directory " << groupDir << endl;

  ofstream groupFile;
  groupFile.open(groupDir + groupIndex);
  if (groupFile.fail() || groupFile.eof()){
    cerr << "Failed to open group file!" << endl;
    return;
  }
  groupFile << Group::ID_COUNTER << endl;
  for_each(groupFileMap.begin(), groupFileMap.end(),
	   [&groupFile](pair<long, string> filemap){
	     groupFile << filemap.first << " " << filemap.second << endl;
	   });
  groupFile.close();

  if(stat(eventDir.c_str(), &buffer) != 0)
    if(mkdir(eventDir.c_str(), 0777) != 0)
      cerr << "Could not create directory " << eventDir << endl;

  ofstream eventFile;
  eventFile.open(eventDir + eventIndex);
  eventFile << Event::ID_COUNTER << endl;
  if (eventFile.fail() || eventFile.eof()){
    cerr << "Failed to open event file!" << endl;
    return;
  }
  for_each(eventFileMap.begin(), eventFileMap.end(),
	   [&eventFile](pair<long, string> filemap){
	     eventFile << filemap.first << " " << filemap.second << endl;
	   });
  eventFile.close();
}

void interruptHandler(int s){ shutdown(); exit(0); }

void init(){
  // Catch interrupts and save state properly.
  struct sigaction sigIntHandler;
  sigIntHandler.sa_handler = interruptHandler;
  sigemptyset(&sigIntHandler.sa_mask);
  sigIntHandler.sa_flags = 0;
  sigaction(SIGINT, &sigIntHandler, NULL);

  parseUserFile();
  parseEventFile();
  parseGroupFile();
}

void parseUserFile(){
  struct stat buffer;
  string userIndexFile = userDir + userIndex;
  if (stat(userIndexFile.c_str(), &buffer) == 0){
    ifstream usersFile;
    string nextIdLine;
    string usermapLine;
    long userID;
    string userPath;
    
    usersFile.open(userIndexFile);
    if(usersFile.fail()){
      cerr << "Problem opening existing user file..." << endl;
      return;
    }
    
    if(!getline(usersFile,nextIdLine)){
      cerr << "Problem parsing existing user file: could not get first line." << endl;
      return;
    }
    try{
      User::ID_COUNTER = stol(nextIdLine);
    } catch(invalid_argument e){
      cerr << "Problem parsing existing user file: first line is not a valid integer." << endl;
      return;
    } catch(out_of_range e){
      cerr << "Problem parsing existing user file: first line is too big of a number!" << endl;
      return;
    }

    while(getline(usersFile,usermapLine)){
      stringstream usermapStream(usermapLine);
      usermapStream >> userID;
      usermapStream >> userPath;
      userFileMap[userID] = userPath;
    }

    usersFile.close();
  }
}

void parseGroupFile(){
  struct stat buffer;
  string groupIndexFile = groupDir + groupIndex;
  if (stat(groupIndexFile.c_str(), &buffer) == 0){
    ifstream groupFile;
    string nextIdLine;
    string groupmapLine;
    long groupID;
    string groupPath;
    
    groupFile.open(groupIndexFile);
    if(groupFile.fail()){
      cerr << "Problem opening existing group file..." << endl;
      return;
    }
    
    if(!getline(groupFile,nextIdLine)){
      cerr << "Problem parsing existing group file: could not get first line." << endl;
      return;
    }
    try{
      Group::ID_COUNTER = stol(nextIdLine);
    } catch(invalid_argument e){
      cerr << "Problem parsing existing group file: first line is not a valid integer." << endl;
      return;
    } catch(out_of_range e){
      cerr << "Problem parsing existing group file: first line is too big of a number!" << endl;
      return;
    }

    while(getline(groupFile,groupmapLine)){
      stringstream groupmapStream(groupmapLine);
      groupmapStream >> groupID;
      groupmapStream >> groupPath;
      groupFileMap[groupID] = groupPath;
    }

    groupFile.close();
  }
}

void parseEventFile(){
  struct stat buffer;
  string eventIndexFile = eventDir + eventIndex;
  if (stat(eventIndexFile.c_str(), &buffer) == 0){
    ifstream eventsFile;
    string nextIdLine;
    string eventmapLine;
    long eventID;
    string eventPath;
    
    eventsFile.open(eventIndexFile);
    if(eventsFile.fail()){
      cerr << "Problem opening existing event file..." << endl;
      return;
    }
    
    if(!getline(eventsFile,nextIdLine)){
      cerr << "Problem parsing existing event file: could not get first line." << endl;
      return;
    }
    try{
      Event::ID_COUNTER = stol(nextIdLine);
    } catch(invalid_argument e){
      cerr << "Problem parsing existing event file: first line is not a valid integer." << endl;
      return;
    } catch(out_of_range e){
      cerr << "Problem parsing existing event file: first line is too big of a number!" << endl;
      return;
    }

    while(getline(eventsFile,eventmapLine)){
      stringstream eventmapStream(eventmapLine);
      eventmapStream >> eventID;
      eventmapStream >> eventPath;
      eventFileMap[eventID] = eventPath;
    }

    eventsFile.close();
  }
}
