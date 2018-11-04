#include "Storage.hpp"
#include "Path.hpp"
#include "Meeting.hpp"
#include "Date.hpp"
#include "User.hpp"

#include <iostream>
#include <list>
#include <functional>
#include <memory>
#include <vector>
#include <fstream>
using namespace std;

shared_ptr<Storage> Storage::m_instance = nullptr; // initial

Storage::Storage() {
  m_dirty = false;
  readFromFile();
}

bool Storage::readFromFile(void) {
  // read the information of users
  ifstream infile_users, infile_meetings;
  infile_users.open(Path::userPath);
  infile_meetings.open(Path::meetingPath);

  if(!infile_users.is_open() || !infile_meetings.is_open())  return false;
  string str;

  while (getline(infile_users, str)) {
    string name;
    string password;
    string email;
    string phone;

    int len = str.size();
    vector<int> index;
    for (int i = 0; i < len; ++i ) {
      if (str[i] == '"') {
        index.push_back(i);
      }
    }
    for (int i = index[0]+1; i < index[1]; ++i )
      name.push_back(str[i]);
    for (int i = index[2]+1; i < index[3]; ++i )
      password.push_back(str[i]);
    for (int i = index[4]+1; i < index[5]; ++i )
      email.push_back(str[i]);
    for (int i = index[6]+1; i < index[7]; ++i )
      phone.push_back(str[i]);

    User user(name, password, email, phone);
    m_userList.push_back(user);
  }
  infile_users.close();

  while (getline(infile_meetings, str)) {
    string sponsor;
    vector<string> participators;
    string startDate;
    string endDate;
    string title;

    int len = str.size();
    vector<int> index1;
    vector<int> index2;
    for (int i = 0; i < len; ++i ) {
      if (str[i] == '"') {
        index1.push_back(i);
      }
      if (str[i] == '&') {
        index2.push_back(i);
      }
    }

    for (int i = index1[0]+1; i < index1[1]; ++i ) {
      sponsor.push_back(str[i]);
    }

    int len1 = index2.size();
    if (len1 != 1) {
      for (int i = 0; i < len1; ++i) {
        if (i == 0) {
          string temp;
          for (int j = index1[2]+1; j < index2[0]; ++j ) {
            temp.push_back(str[j]);
          }
          participators.push_back(temp);
        }
        else if (i == len1-1) {
          string temp;
          for (int j = index2[i-1]+1; j < index1[3]; ++j ) {
            temp.push_back(str[j]);
          }
          participators.push_back(temp);
        }
        else {
          string temp;
          for (int j = index2[i-1]+1; j < index2[i]; ++j ) {
            temp.push_back(str[j]);
          }
          participators.push_back(temp);
        }
      }
      for (int i = index1[4]+1; i < index1[5]; ++i ) {
        startDate.push_back(str[i]);
      }
      for (int i = index1[6]+1; i < index1[7]; ++i ) {
        endDate.push_back(str[i]);
      }
      for (int i = index1[8]+1; i < index1[9]; ++i ) {
        title.push_back(str[i]);
      }
    }
    else {  // if len1 == 1
      string temp;
      for (int j = index1[2]+1; j < index1[3]; ++j) {
        temp.push_back(str[j]);
      }
      participators.push_back(temp);
    }

    Date _startDate(startDate);
    Date _endDate(endDate);
    Meeting meeting(sponsor, participators, _startDate, _endDate, title);
    m_meetingList.push_back(meeting);
  }
  infile_meetings.close();

  return true;
}

bool Storage::writeToFile(void) {
  ofstream outfile_users;
  outfile_users.open(Path::userPath);
  if (!outfile_users.is_open())
    return false;
  auto pUser = m_userList.begin();
  for (int i = 0; i < (int)m_userList.size() - 1; i++) {
    outfile_users << '"' << (*pUser).getName() << '"' << ',' ;
    outfile_users << '"' << (*pUser).getPassword() << '"' << ',' ;
    outfile_users << '"' << (*pUser).getEmail() << '"' << ',' ;
    outfile_users << '"' << (*pUser).getPhone() << '"' << endl;
    pUser++;
  }
  if ((int)m_userList.size() > 0) {
    outfile_users << '"' << (*pUser).getName() << '"' << ',' ;
    outfile_users << '"' << (*pUser).getPassword() << '"' << ',' ;
    outfile_users << '"' << (*pUser).getEmail() << '"' << ',' ;
    outfile_users << '"' << (*pUser).getPhone() << '"';
  }
  outfile_users.close();

  ofstream outfile_Meetings;
  outfile_Meetings.open(Path::meetingPath);
  if (!outfile_Meetings.is_open())
    return false;

  list<Meeting>::iterator pMeeting = m_meetingList.begin();
  for (int i = 0; i < (int)m_meetingList.size() - 1; i++) {
    outfile_Meetings << '"' << (*pMeeting).getSponsor() << '"' << ',';
    vector<string> temp = (*pMeeting).getParticipator();
    outfile_Meetings << '"';
    int j = 0;
    for (; j < (int)temp.size() - 1; j++) 
      outfile_Meetings << temp[j] << '&';
    outfile_Meetings << temp[j] << '"' << ',';
    outfile_Meetings << '"' << Date::dateToString((*pMeeting).getStartDate()) << '"' << ','; 
    outfile_Meetings << '"' << Date::dateToString((*pMeeting).getEndDate()) << '"' << ',';
    outfile_Meetings << '"' << (*pMeeting).getTitle() << '"' << endl;
    pMeeting++;
  }
  if ((int)m_meetingList.size() > 0) {
    outfile_Meetings << '"' << (*pMeeting).getSponsor() << '"' << ',';
    vector<string> temp = (*pMeeting).getParticipator();
    outfile_Meetings << '"';
    int j;
    for (j = 0; j < (int)temp.size() - 1; j++)
     outfile_Meetings << temp[j] << '&';
    outfile_Meetings << temp[j] << '"' << ',';
    outfile_Meetings << '"' << Date::dateToString((*pMeeting).getStartDate()) << '"' << ',';
    outfile_Meetings << '"' << Date::dateToString((*pMeeting).getEndDate()) << '"' << ',';
    outfile_Meetings << '"' << (*pMeeting).getTitle() << '"';
  }
  outfile_Meetings.close();
  return true;
}

shared_ptr<Storage> Storage::getInstance(void) {
  if (m_instance == nullptr) {
    m_instance = shared_ptr<Storage> (new Storage());
  }
  return m_instance;
}

Storage::~Storage() {
  sync();
}

void Storage::createUser(const User &t_user) {
    m_userList.push_back(t_user);
    m_dirty = true;
}

std::list<User> Storage::queryUser(std::function<bool(const User &)> filter) const {
    list<User> filter_user;
    auto iter = m_userList.begin();
    for (; iter != m_userList.end(); ++iter) {
        if(filter(*iter))
            filter_user.push_back(*iter);
    }
    return filter_user;
}

int Storage::updateUser(std::function<bool(const User &)> filter,
                 std::function<void(User &)> switcher) {
    list<User> filter_user;
    list<User>::iterator iter;
    int count = 0;
    for (iter = m_userList.begin(); iter != m_userList.end(); ++iter) {
        if(filter(*iter)) {
            switcher(*iter);
            count ++;
        }
    }
    if (count != 0)
      m_dirty = true;
    return count;
}

int Storage::deleteUser(std::function<bool(const User &)> filter) {
    list<User> filter_user;
    list<User>::iterator iter;
    int count = 0;
    for (iter = m_userList.begin(); iter != m_userList.end(); ) {
        if(filter(*iter)) {
            auto it = iter;
            iter = m_userList.erase(it);
            count ++;
            continue;
        }
        iter ++;
    }
    if (count != 0)
      m_dirty = true;
    return count;
}

void Storage::createMeeting(const Meeting &t_meeting) {
    m_meetingList.push_back(t_meeting);
}

std::list<Meeting> Storage::queryMeeting(
      std::function<bool(const Meeting &)> filter) const {
    list<Meeting> filter_meeting;
    auto iter = m_meetingList.begin();
    for (; iter != m_meetingList.end(); ++iter) {
        if(filter(*iter))
            filter_meeting.push_back(*iter);
    }
    return filter_meeting;
}

int Storage::updateMeeting(std::function<bool(const Meeting &)> filter,
                    std::function<void(Meeting &)> switcher) {
    int count = 0;
    list<Meeting> filter_meeting;
    list<Meeting>::iterator iter;
    for (iter = m_meetingList.begin(); iter != m_meetingList.end(); ++iter) {
        if(filter(*iter)) {
            switcher(*iter);
            count ++;
        }
    }
    if (count != 0)
      m_dirty = true;
    return count;
}

int Storage::deleteMeeting(std::function<bool(const Meeting &)> filter) {
    int count = 0;
    list<Meeting> filter_meeting;
    auto iter = m_meetingList.begin();
    for (; iter != m_meetingList.end();) {
        if(filter(*iter)) {
            count ++;
            auto it = iter;
            iter = m_meetingList.erase(it);
            continue;
        }
        iter++;
    }
    if (count != 0)
      m_dirty = true;
    return count;
}

bool Storage::sync(void) {
  if (m_dirty == false) return false;
  if (m_dirty == true) {
    bool flag = writeToFile();
    if (flag == false)
      return false;
  }
  return true;
}
