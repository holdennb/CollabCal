#ifndef _EVENT_H
#define _EVENT_H
#include <string>
#include <ctime>
#include <list>
class Event{
 private:
  /* The ID of the next created event. */
  static long int ID_COUNTER = 0;
  /* The public name of the event. */
  std::string name;
}
#endif
