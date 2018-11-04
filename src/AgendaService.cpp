#include "AgendaService.hpp"
#include "User.hpp"

#include <algorithm>
#include <iostream>
#include <string>
using namespace std;

AgendaService::AgendaService() {
  startAgenda();
}

AgendaService::~AgendaService() {
  quitAgenda();
}

bool AgendaService::userLogIn(const std::string &userName, const std::string &password) {
  auto filter = [userName,password](const User &user) {     // anonymous function
    if (user.getName() == userName && user.getPassword() == password) {
      return true;
    }
  };
  if (m_storage->queryUser(filter).size() > 0) return true;
  return false;
}

bool AgendaService::userRegister(const std::string &userName, const std::string &password,
                    const std::string &email, const std::string &phone) {
  auto filter = [userName, password, email, phone](const User &user) {
    if (user.getName() == userName || user.getEmail() == email || user.getPhone() == phone) {
      return true;
    }
  };
  if (userName == "" || password == "" || email == "" || phone == "") return false;
  if (m_storage->queryUser(filter).size() == 0) {
    User user(userName, password, email, phone);
    m_storage->createUser(user);
    return true;
  }
  return false;
}

bool AgendaService::deleteUser(const std::string &userName, const std::string &password) {
  auto filter = [userName, password](const User &user) {
    if (user.getName() == userName && user.getPassword() == password)
      return true;
  };
  if (m_storage->queryUser(filter).size() > 0) {
    deleteAllMeetings(userName);
    int count = m_storage->deleteUser(filter);
    if (count > 0) return true;
  }
  return false;
}

list<User> AgendaService::listAllUsers(void) const {
  auto filter = [] (const User &user) {
    return true;
  };
  return m_storage->queryUser(filter);
}

bool AgendaService::createMeeting(const std::string &userName, const std::string &title,
                     const std::string &startDate, const std::string &endDate,
                     const std::vector<std::string> &participator) {
  // check date
  if (!Date::isValid(Date::stringToDate(startDate)) || !Date::isValid((Date::stringToDate(endDate))))
    return false;
  if (startDate == endDate) return false;

  // check if the user exist
  auto filter1 = [userName](const User &user) {
    if (user.getName() == userName) return true;
  };
  if (m_storage->queryUser(filter1).size() == 0) return false;

  // check no date coincidence
  auto filter2 = [userName, startDate, endDate](const Meeting &meeting) {
    if (meeting.getStartDate() >= startDate && meeting.getStartDate() <= endDate ||
      (meeting.getEndDate() <= endDate && meeting.getEndDate() >= startDate)) {
      if (meeting.isParticipator(userName)) return true;
    }
    return false;
  };
  if (m_storage->queryMeeting(filter2).size() > 0) return false;

  // check title
  auto filter3 = [title](const Meeting & meeting) {
    if (meeting.getTitle() == title) return true;
  };
  if (m_storage->queryMeeting(filter3).size() > 0) return false;

  Meeting temp(userName, participator, startDate, endDate, title);
  m_storage->createMeeting(temp);
  return true;
}


bool AgendaService::addMeetingParticipator(const std::string &userName,
                              const std::string &title,
                              const std::string &participator) {
  // check the participator is exist
  auto filter1 = [participator](const User &user) {
    if (user.getName() == participator) return true;
  };
  if (m_storage->queryUser(filter1).size() == 0) return false;

  // check sponsor and title
  auto filter2 = [userName,title](const Meeting & meeting) {
    if (meeting.getTitle() == title && meeting.getSponsor() == userName) return true;
  };
  if (m_storage->queryMeeting(filter2).size() == 0) return false;

  m_storage->queryMeeting(filter2).front().addParticipator(participator);
  return true;
}

bool AgendaService::removeMeetingParticipator(const std::string &userName,
                                 const std::string &title,
                                 const std::string &participator) {
  // check the participator is exist
  auto filter1 = [participator](const User &user) {
    if (user.getName() == participator) return true;
  };
  if (m_storage->queryUser(filter1).size() == 0) return false;

  // check sponsor and title
  auto filter2 = [userName,title,participator](const Meeting & meeting) {
    if (meeting.getTitle() == title && meeting.getSponsor() == userName) {
      return meeting.isParticipator(participator);
    }
  };
  if (m_storage->queryMeeting(filter2).size() == 0) return false;

  m_storage->queryMeeting(filter2).front().removeParticipator(participator);
  return true;
}

bool AgendaService::quitMeeting(const std::string &userName, const std::string &title) {
  // check sponsor and title
  auto filter = [userName,title](const Meeting & meeting) {
    if (meeting.getTitle() == title && meeting.isParticipator(userName) && meeting.getSponsor() != userName) {
      return 1;
    }
  };
  if (m_storage->queryMeeting(filter).size() == 0) return false;

  m_storage->queryMeeting(filter).front().removeParticipator(userName);
  return true;
}

list<Meeting> AgendaService::meetingQuery(const std::string &userName,
                                  const std::string &title) const {
  auto filter = [userName, title](const Meeting &meeting) {
    if (meeting.getTitle() == title) {
      return meeting.isParticipator(userName) || meeting.getSponsor() == userName;
    }
  };
  return m_storage->queryMeeting(filter);
}

list<Meeting> AgendaService::meetingQuery(const std::string &userName,
                                  const std::string &startDate,
                                  const std::string &endDate) const {
  auto filter = [userName, startDate, endDate](const Meeting &meeting) {
    if (meeting.isParticipator(userName) or meeting.getSponsor()==userName) {
      if (meeting.getStartDate() == startDate && meeting.getEndDate() == endDate)
        return true;
    }
  };
  return m_storage->queryMeeting(filter);
}

list<Meeting> AgendaService::listAllMeetings(const std::string &userName) const {
  auto filter = [userName](const Meeting & meeting) {
    if (meeting.isParticipator(userName) or meeting.getSponsor()==userName)
      return true;
  };
  return m_storage->queryMeeting(filter);
}

list<Meeting> AgendaService::listAllSponsorMeetings(const std::string &userName) const {
  auto filter = [userName](const Meeting &meeting) {
    if (meeting.getSponsor() == userName)
      return true;
  };
  return m_storage->queryMeeting(filter);
}

list<Meeting> AgendaService::listAllParticipateMeetings(
      const std::string &userName) const {
  auto filter = [userName](const Meeting & meeting) {
    if (meeting.isParticipator(userName) && meeting.getSponsor()!=userName)
      return true;
  };
  return m_storage->queryMeeting(filter);
}

bool AgendaService::deleteMeeting(const std::string &userName, const std::string &title){
  auto filter = [userName, title](const Meeting & meeting) {
    if (meeting.getSponsor() == userName && meeting.getTitle() == title)
      return true;
  };
  if (m_storage->deleteMeeting(filter)>0) return true;
  return false;
}

bool AgendaService::deleteAllMeetings(const std::string &userName){
  auto filter = [](const Meeting & meeting) {
    return true;
  };
  if (m_storage->deleteMeeting(filter)>0) return true;
  return false;
}

void AgendaService::startAgenda(void) {
  m_storage = m_storage->getInstance();
}

void AgendaService::quitAgenda(void) {
  m_storage->sync();
}
