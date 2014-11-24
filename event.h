#ifndef _EVENT_H
#define _EVENT_H
#include <string>
#include <ctime>
#include <list>
class Event{
 private:
  /* The ID of the next created event. */
  static long ID_COUNTER = 0;
  /* The unique identifier for this event. */
  long id;
  /* The public name of the event. */
  std::string name;
  /* The time at which the event occurs. */
  time_t time;
 public:
  /* Create an event with the given name, occurring at the given time. */
  Event(std::string name, time_t time);
  /* Get the name of the event. */
  std::string getName();
  /* Get the time at which the event is occurring. */
  time_t getTime();
  /* Get the uniqe identifier for this event. */
  long getID();
  /* Change the time at which this event is occurring. */
  void reschedule(time_t newTime);
  /* Change the name of the event. */
  void rename(std::string newName);
  /*
     Serialize this event and write it to a file with the given filename,
     overwriting any data that was previously there.
     Returns true if the write succeeded, false otherwise.
  */
  bool writeToFile(std::string filename);
  /*
     Read an event from the file with the given filename.
     Returns a pointer to the generated event.
     Returns nullptr if the file could not be found,
     or was not formatted properly.
  */
  static Event* readFromFile(std::string filename);
}
#endif
