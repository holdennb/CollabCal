#include <string>
#include <list>
#include <map>

/* Construct JSON string of a user's events */
std::string getEventsJson(const long userID);

/* Returns HTML for the initial calendar, based off the current time */
std::string getEmptyCalendar();

/* Returns HTML for the footer of the page */
std::string getFooter();

/* Returns HTML for the header of the page, based off the current user */
std::string getHeader(const std::string &username);

/* Returns HTML for the login page */
std::string getLogin();

std::string getResponseHeader(const std::string &firstline, const std::map<std::string, std::string>* reqHeaders,
   const int contentLength);