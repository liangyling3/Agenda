#include "Date.hpp"

#include <iostream>
#include <string>
#include <cstdlib>
#include <cstdio>
using namespace std;

Date::Date() {
  m_year = 0;
  m_month = 0;
  m_day = 0;
  m_hour = 0;
  m_minute = 0;
}

Date::Date(int t_year, int t_month, int t_day, int t_hour, int t_minute) {
  this->m_year = t_year;
  this->m_month = t_month;
  this->m_day = t_day;
  this->m_hour = t_hour;
  this->m_minute = t_minute;
}

Date::Date (const std::string &dateString) {
    (*this) = stringToDate(dateString);
}

int Date::getYear(void) const {
  return this->m_year;
}

void Date::setYear(const int t_year) {
  this->m_year = t_year;
}

int Date::getMonth(void) const {
  return this->m_month;
}

void Date::setMonth(const int t_month) {
  this->m_month = t_month;
}

int Date::getDay(void) const {
  return this->m_day;
}

void Date::setDay(const int t_day) {
  this->m_day = t_day;
}

int Date::getHour(void) const {
  return this->m_hour;
}

void Date::setHour(const int t_hour) {
  this->m_hour = t_hour;
}

int Date::getMinute(void) const {
  return this->m_minute;
}

void Date::setMinute(const int t_minute) {
  this->m_minute = t_minute;
}

bool Date::isValid(const Date &t_date) {
    int day[13] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
  if(t_date.m_year >= 1000 && t_date.m_year <= 9999) {
    if((t_date.m_year % 4 == 0 && t_date.m_year % 100 != 0) || t_date.m_year % 400 == 0) {
      day[2]++;
    }
    if(t_date.m_month >= 1 && t_date.m_month <= 12) {
      if(t_date.m_day > 0 && t_date.m_day <= day[t_date.m_month]) {
        if(t_date.m_hour >= 0 && t_date.m_hour < 24) {
          if(t_date.m_minute >= 0 && t_date.m_minute < 60) {
              return 1;
          }
        }
      }
    }
  }
  return 0;
}

// return false if the format is not matching
bool checkFormat(const string &str) {
  if(str.size() != 16) {
    return 0;
  }
  for(int i = 0; i < 16; i++) {
    if(i == 4 || i == 7) {
      if(str[i] != '-') {
          return 0;
      }
    }
    else if(i == 10) {
      if(str[i] != '/') {
          return 0;
      }
    }
    else if(i == 13) {
      if(str[i] != ':') {
          return 0;
      }
    }
    else {
      if(str[i] - '0' < 0 || str[i] - '9' > 0) {
          return 0;
      }
    }
  }
  return 1;
}

// convert string to date
Date Date::stringToDate(const string &t_dateString) {
  Date date;
  if (!checkFormat(t_dateString)) return date;

  int i = 0;
  int year = 0, month = 0, day = 0, hour = 0, minute = 0;
  for(i = 0; i < 4; i++) {
      year = year * 10 + t_dateString[i] - '0';
  }
  for(i = 5; i < 7; i++) {
      month = month * 10 + t_dateString[i] - '0';
  }
  for(i = 8; i < 10; i++) {
      day = day * 10 + t_dateString[i] - '0';
  }
  for(i = 11; i < 13; i++) {
      hour = hour * 10 + t_dateString[i] - '0';
  }
  for(i = 14; i < 16; i++) {
      minute = minute * 10 + t_dateString[i] - '0';
  }
  date.m_year = year;
  date.m_month = month;
  date.m_day = day;
  date.m_hour = hour;
  date.m_minute = minute;
  return date;
}

string numToString(int num) {
  char line[5];
  if(num < 10) {
    sprintf(line,"0%d", num);
  }
  else {
    sprintf(line,"%d", num);
  }
  string str(line);
  return str;
}

string Date::dateToString(const Date &t_date) {
    string date;
    if(isValid(t_date) == 0) {
        date = "0000-00-00/00:00";
    }
    else {
        date = numToString(t_date.m_year) + "-" + numToString(t_date.m_month) + "-" + numToString(t_date.m_day) + "/" + numToString(t_date.m_hour) + ":" + numToString(t_date.m_minute);
    }
    return date;
}

Date & Date::operator = (const Date &t_date) {
  this->m_year = t_date.m_year;
  this->m_month = t_date.m_month;
  this->m_day = t_date.m_day;
  this->m_hour = t_date.m_hour;
  this->m_minute = t_date.m_minute;
  return *this;
}

bool Date::operator == (const Date &t_date) const {
  if (this->m_year != t_date.m_year) return 0;
  if (this->m_month != t_date.m_month) return 0;
  if (this->m_day != t_date.m_day) return 0;
  if (this->m_hour != t_date.m_hour) return 0;
  if (this->m_minute != t_date.m_minute) return 0;
  return 1;
}

bool Date::operator > (const Date &t_date) const {
  if (this->m_year > t_date.m_year) return 1;
  if (this->m_year < t_date.m_year) return 0;
  if (this->m_month > t_date.m_month) return 1;
  if (this->m_month < t_date.m_month) return 0;
  if (this->m_day > t_date.m_day) return 1;
  if (this->m_day < t_date.m_day) return 0;
  if (this->m_hour > t_date.m_hour) return 1;
  if (this->m_hour < t_date.m_hour) return 0;
  if (this->m_minute > t_date.m_minute) return 1;
  if (this->m_minute <= t_date.m_minute) return 0;
  return 1;
}

bool Date::operator < (const Date &t_date) const {
  if ((*this)>t_date) return 0;
  if ((*this)==t_date) return 0;
  return 1;
}

bool Date::operator >=(const Date &t_date) const {
  if ((*this)>t_date) return 1;
  if ((*this)==t_date) return 1;
  return 0;
}

bool Date::operator <=(const Date &t_date) const {
  if ((*this)<t_date) return 1;
  if ((*this)==t_date) return 1;
  return 0;
}
