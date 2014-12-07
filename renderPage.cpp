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
#include <iostream>

using namespace std;

/* Construct JSON string of a user's events */
const string getEventsJson(const long userID) {
  list<long>* events = getEvents(userID);
  cout << "events.size() = " << events->size() << endl;
  stringstream json;
  json << "[";
  for (long eventID: *events) {
    auto event = acquireEvent(eventID);
    json << "{";
    json << "\"id\": \"" << event->getID() << "\",";
    json << "\"name\": \"" << event->getName() << "\",";
    time_t time = event->getTime();
    cout << "event time is " << time << endl;
    struct tm * timeinfo = localtime(&time);
    json << "\"year\": \"" << (timeinfo->tm_year + 1900) << "\",";
    json << "\"month\": \"" << (timeinfo->tm_mon + 1) << "\",";
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

  string jsonString = json.str();
  if (events->size() > 0) {
    jsonString = jsonString.substr(0, jsonString.size()-1);
  }
  return jsonString + "]";
}

/* Returns HTML for the initial calendar, based off the current time */
const string getEmptyCalendar() {
  time_t curTime;
  time (&curTime);
  struct tm* timeInfo = localtime(&curTime);
  int year = timeInfo->tm_year + 1900;
  int monthNum = timeInfo->tm_mon;
  int today = timeInfo->tm_mday + 1;
  int dayOfWeek = timeInfo->tm_wday + 1;
  cout << "today is " << year << "-" << monthNum << "-" << today << ", hour " << timeInfo->tm_hour << endl;

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

  stringstream cal;
  cal << "<h1>Collab Cal!</h1>" << "<div id='table-div'>";
  cal << "<h3 id='month' data-value='" << (monthNum + 1) << "'>";
  cal << "<span id='month-name'>" << monthNames[monthNum] << "</span> ";
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
  return cal.str();
}

const string getFooter() {
  stringstream footer;
  string editEvent   = "/editEvent";
  string createEvent = "/createEvent";

  footer << "<div id='edit-event' class='form-div'>";
  footer << "<h3>Edit Event</h3>";
  footer << "<form action='" << editEvent << "' method='POST'>";
  footer << "<div class='modified'>Warning: This event has been modified</div>";
  footer << "<input type='hidden' name='id' class='event-id' />";
  footer << "<label>Event Name</label><br />";
  footer << "<input type='text' name='name' class='event-name' /><br />";
  footer << "<label>Event Date &amp; Time <br />(YYYY-MM-DD HH:MM)</label><br />";
  footer << "<input type='text' class='event-datetime' /><br />";
  footer << "<input type='hidden' name='datetime' class='timestamp'/><br />";
  footer << "<input type='submit' value='Submit' class='submit' /><br />";
  footer << "<span class='message'></span></form></div>";
  footer << "<div id='create-event' class='form-div'>";
  footer << "<h3>Create New Event</h3>";
  footer << "<form action='" << createEvent << "' method='POST'>";
  footer << "<label>Event Name</label><br />";
  footer << "<input type='text' name='name' class='event-name' /><br />";
  footer << "<label>Event Date &amp; Time <br />(YYYY-MM-DD HH:MM)</label><br />";
  footer << "<input type='text' class='event-datetime' /><br />";
  footer << "<input type='hidden' name='datetime' class='timestamp'/><br />";
  footer << "<label>Group Name (optional)</label><br />";
  footer << "<input type='text' name='group-name' class='group-name' /><br />";
  footer << "<input type='submit' value='Submit' class='submit' /><br />";
  footer << "<span class='message'></span>";
  footer << "</form></div></body></html>";

  return footer.str();
}

const string getHeader(const string &username) {
  stringstream header;

  header << "<html>\n<head>\n<title>Collab Cal</title>\n";
  header << "<script src='http://ajax.googleapis.com/ajax/libs/jquery/2.1.1/jquery.min.js'></script>";
  header << getExtras();
  header << "\n</head><body>";

  long userID = userIdByName(username);
  string newGroup   = "/createGroup";
  string addToGroup = "/addToGroup";

  header << "<p id='user'>Logged in as <span class='username'>" << username;
  header << "</span><span class='id'>" << userID << "</span> ";
  header << "<span class='groups'>(group actions)</span> <span class='logout'>(Logout)</span></p>";
  header << "<div id='groups'><div class='make-group form-div'>";
  header << "<h3>Create New Group</h3>";
  header << "<form action='" << newGroup << "' method='POST'>";
  header << "<label>Group Name</label><br />";
  header << "<input type='text' name='group-name' class='group-name' /><br />";
  header << "<input type='submit' value='Create' class='submit' /><br />";
  header << "<span class='message'></span>";
  header << "</form></div>";
  header << "<div class='add-to-group form-div'>";
  header << "<h3>Add User to Group</h3>";
  header << "<form action='" << addToGroup << "' method='POST'>";
  header << "<label>Group Name</label><br />";
  header << "<input type='text' name='group-name' class='group-name' /><br />";
  header << "<label>Username of User to Add</label><br />";
  header << "<input type='text' name='added-name' class='added-name' /><br />";
  header << "<label>Make Admin?</label><br />";
  header << "<input type='checkbox' name='make-admin' class='make-admin' /><br />";
  header << "<input type='submit' value='Add' class='submit' /><br />";
  header << "<span class='message'></span>";
  header << "</form></div></div>";

  return header.str();
}

const string getLogin() {
  stringstream login;

  login << "<html><head><title>Collab Cal</title>";
  login << "<script src='http://ajax.googleapis.com/ajax/libs/jquery/2.1.1/jquery.min.js'></script>";
  login << getExtras();
  login << "</head><body>";

  string loginURL = "/login";
  string newUser  = "/createUser";

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
  login << "<span>(Alphanumeric characters only)</span>\n";
  login << "<label>Username</label><br />";
  login << "<input type='text' name='username' class='username' /><br />";
  login << "<label>Password</label><br />";
  login << "<input type='password' name='password' class='password' /><br />";
  login << "<input type='submit' value='Create' class='login' /><br />";
  login << "<span class='message'></span>";
  login << "</form></div></div></body></html>";

  return login.str();
}

const string getResponseHeader(const string &firstline, const map<string, string>* resHeaders,
   const int contentLength) {
  stringstream header;

  header << firstline << "\r\n";
  for (auto& pair: *resHeaders) {
    header << pair.first << ": " << pair.second << "\r\n";
  }
  header << "Content-Length: " << contentLength << "\r\n";
  return header.str();
}

const string getExtras() {
  stringstream extras;
  extras << "\n<script>\n";
  
  extras << "(function() {\n"
"$(document).ready(function() {\n"
"$('#container .register-link').click(function() {\n"
"$(this).next().toggle();\n"
"});\n"
"$('#user .groups').click(function() {\n"
"$('#groups').toggle();\n"
"});\n"
"$('.form-div input.uid').val($('#user').find('.id').text());\n"
"$(document.body).on('click', '.event', function() {\n"
"console.log('event clicked');\n"
"$('#table-div .event').removeClass('active');\n"
"$(this).addClass('active');\n"
"$('#edit-event .modified').hide();\n"
"$('#edit-event .message').text('');\n"
"$('#edit-event input.event-name').val($(this).find('.name').text());\n"
"var month = $('#month').attr('data-value');\n"
"var day = $(this).parent().find('.day-num').text();\n"
"var time = $(this).find('.time').text();\n"
"var hour = time.substring(0, time.indexOf(':'));\n"
"$('#edit-event input.event-datetime')\n"
".val($('#year').text() + '-' + (month < 10 ? '0' + month : month)\n"
"+ '-' + (day < 10 ? '0' + day : day)\n"
"+ ' ' + (hour < 10 ? '0' + time : time));\n"
"$('#edit-event input.event-id').val($(this).attr('data-value'));\n"
"$('#edit-event').show();\n"
"});\n"
"$('.logout').click(function() {\n"
"document.cookie = 'sessionId=; expires=Thu, 01 Jan 1970 00:00:01 GMT;';\n"
"location.reload();});\n"
"$('#register-div form').submit(function(e) {\n"
"var postData = $(this).serializeArray();\n"
    "var formURL = $(this).attr('action');\n"
    "$.ajax({\n"
        "url : formURL,\n"
        "type: 'POST',\n"
        "data : postData,\n"
        "success: function(data, textStatus, jqXHR) {\n"
            "$('#register-div form').find('.message').text(data);\n"
        "},\n"
        "error: function(jqXHR, textStatus, errorThrown) {\n"
            "$('#register-div form').find('.message').text(textStatus);\n"
        "}\n"
    "});\n"
    "e.preventDefault();\n"
    "e.unbind();});\n"
"$('#login-div form').submit(function(e) {\n"
"var postData = $(this).serializeArray();\n"
    "var formURL = $(this).attr('action');\n"
    "$.ajax({\n"
        "url : formURL,\n"
        "type: 'POST',\n"
        "data : postData,\n"
        "success: function(data, textStatus, jqXHR) {\n"
            "location.reload();\n"
        "}\n"
    "});\n"
    "e.preventDefault();\n"
    "e.unbind();});\n"
"$('#edit-event form').submit(function(e) {\n"
"var datetime = $(this).children('input.event-datetime').val();\n"
    "var dateAndTime = datetime.split(' ');\n"
    "var dateParts = dateAndTime[0].split('-');\n"
    "var timeParts = dateAndTime[1].split(':');\n"
	"var dateObject = new Date(dateParts[0], dateParts[1] - 1,\n"
		"dateParts[2], timeParts[0], timeParts[1]);\n"
	"$(this).children('input.timestamp').val(dateObject.getTime() / 1000);\n"
    "var postData = $(this).serializeArray();\n"
    "var formURL = $(this).attr('action');\n"
    "$.ajax({\n"
        "url : formURL,\n"
        "type: 'POST',\n"
        "data : postData,\n"
        "contentType: 'text/plain',\n"
        "success: function(data, textStatus, jqXHR) {\n"
            "$('#edit-event form').find('.message').text(data);\n"
    "$('.event').removeClass('active');\n"
    "$('#edit-event').hide();\n"
        "},\n"
        "error: function(jqXHR, textStatus, errorThrown) {\n"
            "$('#edit-event form').find('.message').text(textStatus);\n"
        "}\n"
    "});\n"
    "e.preventDefault();\n"
    "e.unbind();});\n"
"$('#create-event form').submit(function(e) {\n"
	"var datetime = $(this).children('input.event-datetime').val();\n"
    "var dateAndTime = datetime.split(' ');\n"
    "var dateParts = dateAndTime[0].split('-');\n"
    "var timeParts = dateAndTime[1].split(':');\n"
	"var dateObject = new Date(dateParts[0], dateParts[1] - 1,\n"
		"dateParts[2], timeParts[0], timeParts[1]);\n"
	"$(this).children('input.timestamp').val(dateObject.getTime() / 1000);\n"
    "var postData = $(this).serialize();\n"
    "console.log(postData);\n"
    "var formURL = $(this).attr('action');\n"
    "$.ajax({\n"
        "url : formURL,\n"
        "type: 'POST',\n"
        "data : postData,\n"
        "contentType: 'text/plain',\n"
        "success: function(data, textStatus, jqXHR) {\n"
            "$('#create-event form').find('.message').text(data);\n"
        "},\n"
        "error: function(jqXHR, textStatus, errorThrown) {\n"
            "$('#create-event form').find('.message').text(textStatus);\n"
        "}\n"
    "});\n"
    "e.preventDefault();\n"
    "e.unbind();\n"
"});\n"
"$('#groups .make-group form').submit(function(e) {\n"
"var postData = $(this).serializeArray();\n"
    "var formURL = $(this).attr('action');\n"
    "$.ajax({\n"
        "url : formURL,\n"
        "type: 'POST',\n"
        "data : postData,\n"
        "contentType: 'text/plain',\n"
        "success: function(data, textStatus, jqXHR) {\n"
            "$('#groups .make-group form').find('.message').text(data);\n"
        "},\n"
        "error: function(jqXHR, textStatus, errorThrown) {\n"
            "$('#groups .make-group form').find('.message').text(textStatus);\n"
        "}\n"
    "});\n"
    "e.preventDefault();\n"
    "e.unbind();});\n"
"$('#groups .add-to-group form').submit(function(e) {\n"
"var postData = $(this).serializeArray();\n"
    "var formURL = $(this).attr('action');\n"
    "$.ajax({\n"
        "url : formURL,\n"
        "type: 'POST',\n"
        "data : postData,\n"
        "contentType: 'text/plain',\n"
        "success: function(data, textStatus, jqXHR) {\n"
            "$('#groups .add-to-group form').find('.message').text(data);\n"
        "},\n"
        "error: function(jqXHR, textStatus, errorThrown) {\n"
            "$('#groups .add-to-group form').find('.message').text(textStatus);\n"
        "}\n"
    "});\n"
    "e.preventDefault();\n"
    "e.unbind();\n"
"});\n"
"if ($('table').length) {\n"
    "poll();\n"
"}\n"
"});\n"
"function poll(){\n"
    "$.ajax({ url: '/getEvents', type: 'GET',\n"
    "dataType: 'text', contentType: 'application/json',\n"
    "complete: function(data) {\n"
      "updateEvents(data);\n"
      "setTimeout(poll, 5000);\n"
    "}});\n"
    "}\n"
"function updateEvents(response) {\n"
"var data = JSON.parse(response.responseText);\n"
"var curYear = parseInt($('#year').text());\n"
"var curMonth = parseInt($('#month').attr('data-value'));\n"
"$('#table-div td.real-day .event:not(.active)').remove();\n"
"var active = $('#table-div td.real-day .event.active');\n"
"for (var i = 0; i < data.length; i++) {\n"
"var event = data[i];\n"
"if (event.year == curYear && event.month == curMonth) {\n"
"if (event.id == active.attr('data-value')\n"
"&& (active.children('.time').text() != event.time\n"
"|| active.children('.name').text() != event.name\n"
"|| active.siblings('.day-num').text() != event.day)) {\n"
"$('#edit-event .modified').show();\n"
"} else if (event.id != active.attr('data-value')) {\n"
"var td = $('#table-div td.real-day.day-' + event.day);\n"
"var newEvent = $('<div class=\\\'event\\\'></div>');\n"
"newEvent.attr('data-value', event.id);\n"
"newEvent.append('<span class=\\\'time\\\'>' + event.time + '</span>');\n"
"newEvent.append(': ');\n"
"newEvent.append('<span class=\\\'name\\\'>' + event.name + '</span>');\n"
"td.append(newEvent);\n"
"}\n"
"}\n"
"}\n"
"$('#table-div td.real-day .event.active').each(function() {\n"
"});\n"
"}\n"
"})();";

  extras << "</script><style>";
  
  extras << "body {"
	"font-family: Helvetica, sans-serif;"
"}"
"h1 {"
	"text-align: center;"
	"clear: both;"
"}"
"#table-div {"
	"width: 90%;"
	"margin: 0 auto;"
"}"
"table {"
	"width: 100%;"
"}"
"table, th, td {"
	"border: 1px solid #ccc;"
	"border-collapse: collapse;"
"}"
"table, td {"
	"min-height: 556px;"
"}"
"th, td {"
	"width: 14%;"
"}"
"th {"
	"height: 32px;"
	"border: 1px solid #000;"
"}"
"td.real-day {"
	"border: 1px solid #999;"
	"vertical-align: top;"
"}"
"td.real-day .day-num {"
	"font-size: 0.8em;"
"}"
"td.today {"
	"background-color: #EBFFEA;"
"}"
"td .event {"
	"background-color: #CCE8F4;"
	"padding: 3px;"
	"cursor: pointer;"
	"margin: 2px;"
"}"
"td .event.active {"
	"background-color: cornflowerblue;"
"}"
"#user {"
	"float: right;"
"}"
"#user .id {"
	"display: none;"
"}"
"/* Form */"
".form-div {"
	"width: 70%;"
	"text-align: center;"
	"margin: 30px auto;"
	"border: 1px solid #333;"
	"padding: 20px;"
	"clear: both;"
"}"
"#edit-event {"
	"display: none;"
"}"
".form-div form {"
	"margin: 0px;"
"}"
".form-div input.submit {"
	"font-size: 1.2em;"
	"margin: 10px;"
"}"
"#groups {"
	"display: none;"
"}"
"#groups ul {"
	"list-style: none;"
	"padding: 0px;"
"}"
"#edit-event .modified {"
	"background-color: #F5FF98;"
	"color: rgb(255, 104, 104);"
	"padding: 4px 9px;"
	"margin: 12px auto;"
	"width: 415px;"
	"display: none;"
"}"
"/* Log in page */"
"#container {"
	"width: 300px;"
	"margin: 0 auto;"
	"text-align: center;"
"}"
"input {"
	"margin: 5px;"
"}"
"#container .register-link, #user .groups, #user .logout {"
	"text-decoration: underline;"
	"color: blue;"
	"cursor: pointer;"
"}"
"#container #register-div {"
	"display: none;"
"}";

  extras << "</style>";
  return extras.str();
}











