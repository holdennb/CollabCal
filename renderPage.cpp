#include "renderPage.h"
#include "user.h"
#include "group.h"
#include "event.h"
#include "serverActions.h"
#include "objectCache.h"
#include <string>
#include <map>
#include <time.h>
#include <sstream>

using namespace std;

/* Construct JSON string of a user's events */
string getEventsJson(const long userID) {
  list<long>* events = getEvents(userID);
  string jsonString;
  stringstream json;
  json << "[";
  for (long eventID: *events) {
    auto event = acquireEvent(eventID);
    json << "{";
    json << "\"id\": \"" << event->getID() << "\",";
    json << "\"name\": \"" << event->getName() << "\",";
    time_t time = event->getTime();
    struct tm * timeinfo = localtime(&time);
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
  int monthNum = timeInfo->tm_mon;
  int today = timeInfo->tm_mday;
  int dayOfWeek = timeInfo->tm_wday;

  // Calculate the day of the week of the 1st
  int startDayOfWeek = (dayOfWeek - (today % 7) + 8) % 7;

  // Calculate the length of the month
  int monthLength = 31;
  if (monthNum == 4 || monthNum == 6 || monthNum == 9 || monthNum == 11) { 
    monthLength = 30;
  } else if (monthNum == 2) {
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

string getFooter() {
  string footerString;
  stringstream footer;
  string editEvent   = "#";
  string createEvent = "#";

  footer << "<div id='edit-event' class='form-div'>";
  footer << "<h3>Edit Event</h3>";
  footer << "<form action='" << editEvent << "' method='POST'>";
  footer << "<div class='modified'>Warning: This event has been modified</div>";
  footer << "<input type='hidden' name='id' class='event-id' />";
  footer << "<input type='hidden' name='uid' class='uid' />";
  footer << "<label>Event Name</label><br />";
  footer << "<input type='text' name='name' class='event-name' /><br />";
  footer << "<label>Event Date &amp; Time <br />(YYYY-MM-DD HH:MM)</label><br />";
  footer << "<input type='text' name='datetime' class='event-datetime' /><br />";
  footer << "<input type='submit' value='Submit' class='submit' />";
  footer << "</form></div>";
  footer << "<div id='create-event' class='form-div'>";
  footer << "<h3>Create New Event</h3>";
  footer << "<form action='" << createEvent << "' method='POST'>";
  footer << "<input type='hidden' name='uid' class='uid' />";
  footer << "<label>Event Name</label><br />";
  footer << "<input type='text' name='name' class='event-name' /><br />";
  footer << "<label>Event Date &amp; Time <br />(YYYY-MM-DD HH:MM)</label><br />";
  footer << "<input type='text' name='datetime' class='event-datetime' /><br />";
  footer << "<label>Group Name (optional)</label><br />";
  footer << "<input type='text' name='group-name' class='group-name' /><br />";
  footer << "<input type='submit' value='Submit' class='submit' />";
  footer << "</form></div></body></html>";

  footer >> footerString;
  return footerString;
}

string getHeader(const string &username) {
  string headerString;
  stringstream header;

  header << "<html><head><title>Collab Cal</title>";
  header << "<script src='http://ajax.googleapis.com/ajax/libs/jquery/2.1.1/jquery.min.js'></script>";
  header << "<script src='scripts.js'></script>";
  header << "<link rel='stylesheet' type='text/css' href='style.css'>";
  header << "</head><body>";

  long userID = userIdByName(username);
  string newGroup   = "#";
  string addToGroup = "#";

  header << "<p id='user'>Logged in as <span class='username'>" << username;
  header << "</span><span class='id'>" << userID << "</span>";
  header << "<span class='groups'>(group actions)</span></p>";
  header << "<div id='groups'><div class='make-group form-div'>";
  header << "<h3>Create New Group</h3>";
  header << "<form action='" << newGroup << "' method='POST'>";
  header << "<input type='hidden' name='uid' class='uid' />";
  header << "<label>Group Name</label><br />";
  header << "<input type='text' name='group-name' class='group-name' /><br />";
  header << "<input type='submit' value='Create' class='submit' />";
  header << "</form></div>";
  header << "<div class='add-to-group form-div'>";
  header << "<h3>Add User to Group</h3>";
  header << "<form action='" << addToGroup << "' method='POST'>";
  header << "<input type='hidden' name='uid' class='uid' />";
  header << "<label>Group Name</label><br />";
  header << "<input type='text' name='group-name' class='group-name' /><br />";
  header << "<label>Username of User to Add</label><br />";
  header << "<input type='text' name='added-name' class='added-name' /><br />";
  header << "<label>Make Admin?</label><br />";
  header << "<input type='checkbox' name='make-admin' class='make-admin' /><br />";
  header << "<input type='submit' value='Add' class='submit' />";
  header << "</form></div></div>";

  header >> headerString;
  return headerString;
}

string getLogin() {
  string loginString;
  stringstream login;

  login << "<html><head><title>Collab Cal</title>";
  login << "<script src='http://ajax.googleapis.com/ajax/libs/jquery/2.1.1/jquery.min.js'></script>";
  login << "<script src='scripts.js'></script>";
  login << "<link rel='stylesheet' type='text/css' href='style.css'>";
  login << "</head><body>";

  string loginURL = "#";
  string newUser  = "#";

  login << "<div id='container'><h1>Collab Cal!</h1><div id='login-div'>";
  login << "<form action='" << loginURL << "' method='POST'>";
  login << "<label>Username</label><br />";
  login << "<input type='text' name='username' class='username' /><br />";
  login << "<label>Password</label><br />";
  login << "<input type='password' name='password' class='password' /><br />";
  login << "<input type='submit' value='Log In' class='login' /></form></div>";
  login << "<p class='register-link'><span>Or Create a New Account</span></p>";
  login << "<div id='register-div'>";
  login << "<form action='" << newUser << "' method='POST'>";
  login << "<label>Username</label><br />";
  login << "<input type='text' name='username' class='username' /><br />";
  login << "<label>Password</label><br />";
  login << "<input type='password' name='password' class='password' /><br />";
  login << "<input type='submit' value='Create' class='login' />";
  login << "</form></div></div></body></html>";

  login >> loginString;
  return loginString;
}

string getResponseHeader(const string &firstline, const map<string, string>* reqHeaders,
   const int contentLength) {
  string headerString;
  stringstream header;

  header << firstline << "\r\n";
  for (auto& pair: reqHeaders) {
    header << pair.first << ": " << pair.second << "\r\n";
  }
  header << "Content-Length: " << contentLength << "\r\n";
  header >> headerString;
  return headerString;
}













