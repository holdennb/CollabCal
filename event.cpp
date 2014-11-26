#include "event.h"
#include <fstream>
#include <stdexcept>
#include <algorithm>

using namespace std;

long Event::ID_COUNTER = 0;

Event::Event(const string &name, const time_t time) :
  Event(name,time,ID_COUNTER++) {}

Event::Event(const string &name, const time_t time, const long id) :
  id(id), name(name), time(time) {}

string Event::getName() const { return name; }

time_t Event::getTime() const { return time; }

long Event::getID() const { return id; }

void Event::reschedule(const time_t newTime){ time = newTime; }

void Event::rename(const string &newName){ name = newName; }

bool Event::writeToFile(const string &filename) const {
  ofstream file;

  file.open(filename);
  if(file.fail())
    return false;

  file << id << endl;
  file << name << endl;
  file << (long long int)time << endl;

  file.close();
  return true;
}

Event* Event::readFromFile(const string &filename){
  ifstream file;

  string idLine;
  string name;
  string timeLine;

  long id;
  time_t time;

  file.open(filename);
  if(file.fail())
    return nullptr;

  if(!getline(file,idLine))
    return nullptr;
  try{
    id = stol(idLine);
  } catch(invalid_argument e){
    return nullptr;
  } catch(out_of_range e){
    return nullptr;
  }

  if(!getline(file,name))
    return nullptr;

  if(!getline(file,timeLine))
    return nullptr;
  try{
    time = (time_t)stoll(timeLine);
  } catch(invalid_argument e){
    return nullptr;
  } catch(out_of_range e){
    return nullptr;
  }
  file.close();
  return new Event(name,time,id);
}
