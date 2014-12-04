#include "user.h"
#include "group.h"
#include "event.h"
#include "calendarServer.h"
#include <string>
#include <list>

/* Construct JSON string of a user's events */
string getEventsJson(const long userID);

/* Returns HTML for the initial calendar, based off the current time */
string getEmptyCalendar();