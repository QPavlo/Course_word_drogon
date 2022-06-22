#include "timeDateHandler.h"

std::string getCurrentTime() {
    auto now{std::chrono::system_clock::now()};
    auto in_time_t{std::chrono::system_clock::to_time_t(now)};
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%X");
    return removeSeconds(ss.str());
}

std::string removeSeconds(const std::string &time) {
    return time.substr(0, 5);
}

std::string addSeconds(const std::string &time) {
    return time + ":00";
}

std::string timeArithmetic(const std::string &time1, const std::string &time2, const char operation) {
    std::stringstream ssTime1{time1};
    std::stringstream ssTime2{time2};

    std::tm t{};

    ssTime1 >> std::get_time(&t, "%H:%M");
    auto duration1{std::chrono::hours(t.tm_hour) + std::chrono::minutes(t.tm_min)};

    ssTime2 >> std::get_time(&t, "%H:%M");
    auto duration2{std::chrono::hours(t.tm_hour) + std::chrono::minutes(t.tm_min)};

    auto totalDuration{std::chrono::hours() + std::chrono::minutes()};

    auto hours{std::chrono::duration_cast<std::chrono::hours>(std::chrono::hours())};
    auto minutes{std::chrono::duration_cast<std::chrono::minutes>(std::chrono::minutes())};
    switch (operation) {
        case '+':
            totalDuration = duration1 + duration2;
            hours = std::chrono::duration_cast<std::chrono::hours>(totalDuration);
            minutes = std::chrono::duration_cast<std::chrono::minutes>(totalDuration - hours);
            break;
        case '-':
            totalDuration = duration1 - duration2;
            hours = std::chrono::duration_cast<std::chrono::hours>(totalDuration);
            minutes = std::chrono::duration_cast<std::chrono::minutes>(totalDuration - hours);
            break;
        default:
            throw std::invalid_argument("Only addition or subtraction");
    }

    t.tm_hour = hours.count();
    t.tm_min = minutes.count();;

    std::stringstream resultTme;
    resultTme << std::put_time(&t, "%H:%M");

    return resultTme.str();
}

std::string getCurrentDate() {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d");
    return ss.str();
}

std::map<std::string, std::string> getAttendanceStatistics(const drogon::orm::Result &result) {
    std::map<std::string, std::string> attendanceStatisticsAll;
    for (const auto &entry: result) {
        attendanceStatisticsAll.insert({entry["date"].as<std::string>(),
                                        entry["attendance_status"].as<std::string>()});
    }
    return attendanceStatisticsAll;
}

std::string statusParser(const std::string &status) {
    switch (status[0]) {
        case attendanceStatus::ABSENT:
            return "absent";
        case attendanceStatus::LATE:
            return "late";
        case attendanceStatus::SHORTCOMINGS:
            return "underwork";
        case attendanceStatus::IN_TIME:
            return "in time";
        case attendanceStatus::OVERWORK:
            return "overwork";
        case attendanceStatus::LATE_CLOSED:
            return "late";
        case attendanceStatus::IN_TIME_CLOSED:
            return "in time";
        case attendanceStatus::FIRST_WORK_DAY_ABSENT:
            return "absent at first day";
        case attendanceStatus::FIRST_WORK_DAY_PRESENT:
            return "present at first day";
        case attendanceStatus::FIRST_DAY_CLOSED :
            return "first day";
        case attendanceStatus::LATE_NOT_CLOSED:
            return "not closed statement";
        case attendanceStatus::IN_TIME_NOT_CLOSED:
            return "not closed statement(in time)";
        case attendanceStatus::FIRST_WORK_DAY_NOT_CLOSED :
            return "not closed statement(first day)";
        case attendanceStatus::WEEKEND:
            return "weekend";
        default:
            return "-";
    }
}