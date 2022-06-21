#pragma once

#include <chrono>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <string>
#include <map>
#include <drogon/orm/Result.h>
#include <drogon/orm/Row.h>
#include <drogon/orm/ResultIterator.h>
#include <drogon/orm/Field.h>
#include <drogon/Session.h>
#include <drogon/HttpViewData.h>

enum attendanceStatus {
    ABSENT = 'a',
    LATE = 'b',
    SHORTCOMINGS = 'c',
    IN_TIME = 'd',
    OVERWORK = 'e',
    LATE_CLOSED = 'f',
    IN_TIME_CLOSED = 'g',
    FIRST_WORK_DAY_ABSENT = 'h',
    FIRST_WORK_DAY_PRESENT = 'i',
    FIRST_DAY_CLOSED = 'j',
    LATE_NOT_CLOSED = 'k',
    IN_TIME_NOT_CLOSED = 'l',
    FIRST_WORK_DAY_NOT_CLOSED = 'm',
    WEEKEND = 'n'
};

enum dayOfWeek {
    SUNDAY = 0,
    MONDAY = 1,
    TUESDAY = 2,
    WEDNESDAY = 3,
    THURSDAY = 4,
    FRIDAY = 5,
    SATURDAY = 6
};

std::string getCurrentTime();

std::string getCurrentDate();

std::string removeSeconds(const std::string &time);

std::string addSeconds(const std::string &time);

std::string timeArithmetic(const std::string &time1, const std::string &time2, char operation);

std::map<std::string, std::string> getAttendanceStatistics(const drogon::orm::Result &result);

void updateDateStatisticInSession(const drogon::SessionPtr &session, char attendanceStatus);

std::string statusParser(const std::string &status);
