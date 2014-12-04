#include "renderPage.h"
#include <string>
#include <time.h>

/* Construct JSON string of a user's events */
string getEventsJson(const long userID) {
  list<long>* events = getEvents(userID);
  string jsonString;
  stringstream json;
  json << "[";
  for (long eventID: *events) {
    Event* event = lookupEvent(eventID);
    json << "{";
    json << "\"id\": \"" << event->getID() << "\",";
    json << "\"name\": \"" << event->getName() << "\",";
    time_t time = event->getTime();
    struct tm * timeInfo = localtime(&time);
    json << "\"year\": \"" << (timeinfo->tm_year + 1900) << "\",";
    json << "\"day\": \"" << timeinfo->tm_mday << "\",";
    // 24 hour time
    json << "\"time\": \"" << timeinfo->tm_hour << ":";
    if (timeinfo->tm_min < 10) {
      json << "0" << timeinfo->tm_min << "\"";
    } else {
      json << timeinfo->tm_min << "\"";
    }
    json << "},";
  }
  json << "]";
  json >> jsonString;
  return jsonString;
}

/* Returns HTML for the initial calendar, based off the current time */
string getEmptyCalendar() {
  time_t curTime;
  time (&curTime);
  struct tm* timeInfo = localtime(&curTime);
  int year = timeInfo->tm_year + 1900;
  int monthNum = timeInfo->tm_month;
  int today = timeinfo->tm_mday;
  int dayOfWeek = timeInfo->tm_wday;

  // Calculate the day of the week of the 1st
  int startDayOfWeek = (dayOfWeek - (today % 7) + 8) % 7;

  // Calculate the length of the month
  int monthLength = 31;
  if (month == 4 || month == 6 || month == 9 || month == 11) { 
    monthLength = 30;
  } else if (month == 2) {
    bool isLeapYear = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
    monthLength = (isLeapYear ? 29 : 28);
  }

  string monthNames[12] = {"January", "February", "March", "April",
                          "May", "June", "July", "August",
                          "September", "October", "November", "December"};

  string calString;
  stringstream cal;
  cal << "<h1>Collab Cal!</h1>" << "<div id='table-div'>";
  cal << "<h3 id='month' data-value='" << monthNum << "'>";
  cal << "<span id='month-name'>" << monthNames[monthNum - 1] << "</span>";
  cal << "<span id='year'>" << year << "</span></h3>";
  cal << "<table><tr><th>Sun</th><th>Mon</th><th>Tue</th>";
  cal << "<th>Wed</th><th>Thu</th><th>Fri</th><th>Sat</th></tr>";

  int dayOfMonth = 1;
  for (int week = 0; week < 6; week++) {
    cal << "<tr>";

    for (int dayNum = 0; dayNum < 7; dayNum++) {
      if ((week == 0 && dayNum < startDayOfWeek) || (dayOfMonth > monthLength)) {
        // Not a valid day
        cal << "<td><span class='day-num'></span></td>";
      } else {
        // valid day
        cal << "<td class='real-day day-" << dayOfMonth;
        if (dayOfMonth == today) {
          cal << " today";
        }
        cal << "'><span class='day-num'>" << dayOfMonth << "</span></td>";
        dayOfMonth++;
      }
    }

    cal << "</tr>";
  }

  cal << "</tr></table></div>";
  cal >> calString;
  return calString;
}













