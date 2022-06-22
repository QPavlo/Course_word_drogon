#include "UserAttendance.h"

using namespace drogon;
using namespace drogon::orm;

void UserAttendance::updateDateStatisticShort(const SessionPtr &session, char attendanceStatus) {
    session->modify<std::map<std::string, std::string>>("attendanceStatisticShort",
                                                        [&](auto &attendanceStatisticShort) {
                                                            auto itr{attendanceStatisticShort.find(getCurrentDate())};

                                                            if (itr != attendanceStatisticShort.end()) {
                                                                itr->second = std::string(1, attendanceStatus);
                                                            }
                                                        });
}

void UserAttendance::userSettingsPage(const HttpRequestPtr &req,
                                      std::function<void(const drogon::HttpResponsePtr &)> &&callback) {
    const SessionPtr &session{req->getSession()};
    std::string username{session->get<std::string>("username")};
    clientPtr->execSqlAsync("select * from user_general\n"
                            "inner join user_details ud ON user_general.username = ud.username\n"
                            "where ud.username = $1;", [=](const Result &r) {
        if (!r.empty()) {
            HttpViewData data;
            Row row{r.front()};
            data.insert("firstname", row["firstname"].as<std::string>());
            data.insert("surname", row["surname"].as<std::string>());
            data.insert("username", username);
            data.insert("appointment", row["appointment"].as<std::string>());
            data.insert("shiftStart", session->get<std::string>("shiftStart"));
            data.insert("shiftFinish", session->get<std::string>("shiftFinish"));
            data.insert("street", row["street"].as<std::string>());
            data.insert("city", row["city"].as<std::string>());
            data.insert("zip_code", row["zip_code"].as<std::string>());
            data.insert("phone_number", row["phone_number"].as<std::string>());
            data.insert("count_updated_fields", static_cast<size_t>(0));
            session->insert("settingsTempViewData", data);
            auto resp{HttpResponse::newHttpViewResponse("userSettings.csp", data)};
            callback(resp);
        }

    }, [](const DrogonDbException &e) {
        LOG_ERROR << "error:" << e.base().what();
    }, username);
}

void UserAttendance::updateUserSettings(const HttpRequestPtr &req,
                                        std::function<void(const drogon::HttpResponsePtr &)> &&callback) {

    const SessionPtr &session{req->getSession()};
    std::string username{session->get<std::string>("username")};

    std::string firstname{req->getParameter("firstname")};
    std::string surname{req->getParameter("surname")};
    std::string appointment{req->getParameter("appointment")};
    std::string street{req->getParameter("street")};
    std::string city{req->getParameter("city")};
    std::string zip_code{req->getParameter("zip_code")};
    std::string phone_number{req->getParameter("phone_number")};
    try {
        auto affectedRowsUserGeneral{clientPtr->execSqlSync(
                " update user_general \n"
                "set firstname = $1, surname = $2\n"
                "where user_general.username = $3"
                " and (firstname <> $1 or surname <> $2)", firstname, surname, username)};

        auto affectedRowsUserDetails{clientPtr->execSqlSync(
                " update user_details \n"
                "set street = $1,"
                " city = $2 ,"
                " zip_code = $3,"
                " phone_number = $4\n"
                " where username = $5 "
                " and ((street <> $1 or street is null) or (city <> $2 or city is null) or (zip_code <> $3 or zip_code is null) or (phone_number <> $4 or phone_number is null))",
                street, city, zip_code,
                phone_number, username)};

        size_t countAffectedRowsUserGeneral{affectedRowsUserGeneral.affectedRows()};
        size_t countAffectedRowsUserDetails{affectedRowsUserDetails.affectedRows()};

        LOG_INFO << countAffectedRowsUserGeneral << "row updated";
        LOG_INFO << countAffectedRowsUserDetails << "row updated";
        HttpViewData data;

        data.insert("firstname", firstname);
        data.insert("surname", surname);
        data.insert("username", username);
        data.insert("appointment", appointment);
        data.insert("shiftStart", session->get<std::string>("shiftStart"));
        data.insert("shiftFinish", session->get<std::string>("shiftFinish"));
        data.insert("street", street);
        data.insert("city", city);
        data.insert("zip_code", zip_code);
        data.insert("phone_number", phone_number);
        data.insert("count_updated_fields", (countAffectedRowsUserGeneral
                                             + countAffectedRowsUserDetails));

        session->modify<HttpViewData>("settingsTempViewData", [&](HttpViewData &newData) {
            newData = data;
        });
        auto resp{HttpResponse::newHttpViewResponse("userSettings.csp", data)};
        callback(resp);
    } catch (const DrogonDbException &e) {
        LOG_ERROR << "error:" << e.base().what();
    }
}

void UserAttendance::updateUserPassword(const HttpRequestPtr &req,
                                        std::function<void(const drogon::HttpResponsePtr &)> &&callback) {
    const SessionPtr &session{req->getSession()};
    std::string username{session->get<std::string>("username")};

    std::string oldPassword{std::move(encode(req->getParameter("password")))};
    std::string newPassword{std::move(encode(req->getParameter("new_password")))};
    std::string confirmNewPassword{std::move(encode(req->getParameter("confirmNewPassword")))};

    try {
        HttpViewData data{session->get<HttpViewData>("settingsTempViewData")};
        auto res{clientPtr->execSqlSync("select password from user_general where username = $1 and password = $2",
                                        username,
                                        oldPassword)};
        if (res.empty()) {
            data.insert("errorPassword", 2);
            auto resp{HttpResponse::newHttpViewResponse("userSettingsPassDefault.csp", data)};
            callback(resp);
        } else if (newPassword != confirmNewPassword) {
            data.insert("errorPassword", 1);
            auto resp{HttpResponse::newHttpViewResponse("userSettingsPassDefault.csp", data)};
            callback(resp);
        } else {
            data.insert("passChangedFlag", 1);
            auto res1{clientPtr->execSqlSync(
                    " update user_general\n"
                    "set password = $1\n"
                    "from user_details ud\n"
                    "where user_general.username = ud.username and user_general.username = $2", newPassword,
                    username)};

            LOG_INFO << res1.affectedRows() << "row updated";
            auto resp{HttpResponse::newHttpViewResponse("userSettingsPassDefault.csp", data)};
            callback(resp);
        }
    } catch (const DrogonDbException &e) {
        LOG_ERROR << "error:" << e.base().what();
    }
}

void UserAttendance::confirmArrival(const HttpRequestPtr &req,
                                    std::function<void(const drogon::HttpResponsePtr &)> &&callback) {

    const SessionPtr &session{req->getSession()};

    std::string username{session->get<std::string>("username")};
    std::string shiftStart{session->get<std::string>("shiftStart")};
    std::string shiftFinish{session->get<std::string>("shiftFinish")};
    char attendanceStatus{session->get<char>("attendanceStatusCode")};

    std::string arrivalTime{req->getParameter("currentTime")};

    HttpViewData data;
    auto updateUserStatisticFunc{
            [&, this](const std::string &attendanceStatusText, char attendanceStatusInput) {
                data.insert("attendanceStatusText", attendanceStatusText);
                data.insert("attendanceStatusCode", attendanceStatusInput);
                session->erase("attendanceStatusCode");
                session->insert("attendanceStatusCode", attendanceStatusInput);
                session->insert("arrivalTime", arrivalTime);
                attendanceStatus = attendanceStatusInput;
                return clientPtr->execSqlSync(
                        "update user_intermediate_statistic set attendance_status = $1, arrival_time = $2 "
                        "where username = $3 and date = CURRENT_DATE;", attendanceStatusInput, arrivalTime, username);
            }
    };
    data.insert("shiftStart", shiftStart);
    data.insert("shiftFinish", shiftFinish);

    try {
        if (attendanceStatus == attendanceStatus::FIRST_WORK_DAY_ABSENT) {
            auto res{updateUserStatisticFunc("First day", attendanceStatus::FIRST_WORK_DAY_PRESENT)};
            LOG_INFO << res.affectedRows() << "row updated";
        } else { // not first day
            if (arrivalTime <= shiftStart) { // in time
                auto res{updateUserStatisticFunc("In time", attendanceStatus::IN_TIME)};
                LOG_INFO << res.affectedRows() << "row updated";
            } else { // late
                auto res{updateUserStatisticFunc("Late", attendanceStatus::LATE)};
                LOG_INFO << res.affectedRows() << "row updated";
            }
        }
        updateDateStatisticShort(session, attendanceStatus);

    } catch (const DrogonDbException &e) {
        LOG_ERROR << "error:" << e.base().what();
    }
    session->insert("attendanceStatusText",
                    data.get<std::string>("attendanceStatusText"));


    data.insert("attendanceStatisticShort",
                session->get<std::map<std::string, std::string>>("attendanceStatisticShort"));
    session->modify<HttpViewData>("tempHttpViewData", [&](HttpViewData &newData) {
        newData = data;
    });
    auto resp{HttpResponse::newHttpViewResponse("attendanceCheck.csp", data)};
    callback(resp);
}

void UserAttendance::confirmEnding(const HttpRequestPtr &req,
                                   std::function<void(const drogon::HttpResponsePtr &)> &&callback) {

    const SessionPtr &session{req->getSession()};

    std::string username{session->get<std::string>("username")};
    std::string shiftStart{session->get<std::string>("shiftStart")};
    std::string shiftFinish{session->get<std::string>("shiftFinish")};
    std::string arrivalTime{session->get<std::string>("arrivalTime")};
    char attendanceStatus{session->get<char>("attendanceStatusCode")};

    std::string endingTime{req->getParameter("currentTime")};

    std::string countPausedHourAndMinutes{session->get<std::string>("countPausedHourAndMinutes")};
    std::string countWorkedHourAndMinutes;

    HttpViewData data;

    data.insert("shiftStart", shiftStart);
    data.insert("attendanceStatusText", std::string("Ending"));

    auto updateUserStatisticFunc{
            [&, this](char attendanceStatusInput) {
                attendanceStatus = attendanceStatusInput;
                data.insert("attendanceStatusCode", attendanceStatusInput);
                return clientPtr->execSqlSync(
                        "update user_intermediate_statistic set attendance_status = $1, finish_time = $2, count_worked_hour_and_minutes = $3 "
                        "where username = $4 and date = CURRENT_DATE;", attendanceStatusInput, endingTime,
                        countWorkedHourAndMinutes,
                        username);
            }
    };

    std::string countWorkedHourAndMinutesWithoutPause{timeArithmetic(endingTime, arrivalTime, '-')};

    if (countPausedHourAndMinutes < productionSettings::MAX_BREAK_TIME) {
        countWorkedHourAndMinutes = std::move(countWorkedHourAndMinutesWithoutPause);
    } else {
        std::string overPause = timeArithmetic(countPausedHourAndMinutes, productionSettings::MAX_BREAK_TIME, '-');
        countWorkedHourAndMinutes = timeArithmetic(countWorkedHourAndMinutesWithoutPause, overPause, '-');
    }

    try {
        if (attendanceStatus == attendanceStatus::FIRST_WORK_DAY_PRESENT) {
            updateUserStatisticFunc(attendanceStatus::FIRST_DAY_CLOSED);
        } else {
            if (endingTime < shiftFinish or
                countPausedHourAndMinutes < timeArithmetic(shiftFinish, shiftStart, '-')) {
                updateUserStatisticFunc(attendanceStatus::SHORTCOMINGS);
            } else if (attendanceStatus == attendanceStatus::IN_TIME and
                       endingTime < timeArithmetic(shiftFinish, "00:15", '+')) {
                updateUserStatisticFunc(attendanceStatus::IN_TIME_CLOSED);
            } else if (attendanceStatus == attendanceStatus::LATE and
                       endingTime < timeArithmetic(shiftFinish, "00:15", '+')) {
                updateUserStatisticFunc(attendanceStatus::LATE_CLOSED);
            } else {
                updateUserStatisticFunc(attendanceStatus::OVERWORK);
            }
        }
        updateDateStatisticShort(session, attendanceStatus);

    } catch (const DrogonDbException &e) {
        LOG_ERROR << "error:" << e.base().what();
    }
    data.insert("attendanceStatisticShort",
                session->get<std::map<std::string, std::string>>("attendanceStatisticShort"));

    session->modify<HttpViewData>("tempHttpViewData", [&](HttpViewData &newData) {
        newData = data;
    });

    auto resp{HttpResponse::newHttpViewResponse("attendanceCheck.csp", data)};
    callback(resp);
}

void UserAttendance::takeBreak(const HttpRequestPtr &req,
                               std::function<void(const drogon::HttpResponsePtr &)> &&callback) {
    const SessionPtr &session{req->getSession()};


    std::string breakStart{req->getParameter("currentTime")};

    session->insert("breakStart", breakStart);

    HttpViewData data;
    try {
        auto res{clientPtr->execSqlSync(
                "update user_intermediate_statistic\n"
                "set break_start = $1\n"
                "where username = $2 and date = CURRENT_DATE", breakStart, session->get<std::string>("username"))};

        LOG_INFO << res.affectedRows() << "row updated";
    } catch (const DrogonDbException &e) {
        LOG_ERROR << "error:" << e.base().what();
    }
    data.insert("breakFinishTime", timeArithmetic(breakStart, productionSettings::MAX_BREAK_TIME, '+'));
    data.insert("attendanceStatusText", std::string("On break"));
    data.insert("attendanceStatisticShort",
                session->get<std::map<std::string, std::string>>("attendanceStatisticShort"));

    session->modify<HttpViewData>("tempHttpViewData", [&](HttpViewData &newData) {
        newData = data;
    });

    auto resp{HttpResponse::newHttpViewResponse("attendanceCheck.csp", data)};
    callback(resp);
}

void UserAttendance::endBreak(const HttpRequestPtr &req,
                              std::function<void(const drogon::HttpResponsePtr &)> &&callback) {

    const SessionPtr &session{req->getSession()};

    std::string breakFinish{req->getParameter("currentTime")};

    session->insert("countPausedHourAndMinutes",
                    timeArithmetic(breakFinish, session->get<std::string>("breakStart"), '-'));

    HttpViewData data;
    try {
        auto res{clientPtr->execSqlSync(
                " update user_intermediate_statistic\n"
                "set break_finish = $1\n"
                "where username = $2 and date = CURRENT_DATE", breakFinish, session->get<std::string>("username"))};

        LOG_INFO << res.affectedRows() << "row updated";
    } catch (const DrogonDbException &e) {
        LOG_ERROR << "error:" << e.base().what();
    }
    data.insert("attendanceStatusCode", session->get<char>("attendanceStatusCode"));
    data.insert("shiftFinish", session->get<std::string>("shiftFinish"));
    data.insert("attendanceStatisticShort",
                session->get<std::map<std::string, std::string>>("attendanceStatisticShort"));

    data.insert("attendanceStatusText", session->get<std::string>("attendanceStatusText"));
    data.insert("afterBreakFlag", 1);

    session->modify<HttpViewData>("tempHttpViewData", [&](HttpViewData &newData) {
        newData = data;
    });

    auto resp{HttpResponse::newHttpViewResponse("attendanceCheck.csp", data)};
    callback(resp);
}

void UserAttendance::tooEarlyPage(const HttpRequestPtr &req,
                                  std::function<void(const drogon::HttpResponsePtr &)> &&callback) {


    const SessionPtr &session{req->getSession()};

    if (req->getParameter("tooEarlyKey") == "1") {
        session->insert("tooEarlyKey", 1);
    }

    auto attendanceStatisticShort{
            session->get<std::map<std::string, std::string>>("attendanceStatisticShort")};

    HttpViewData data;
    data.insert("attendanceStatisticShort", attendanceStatisticShort);
    auto resp{HttpResponse::newHttpViewResponse("tooEarlyPage.csp", data)};
    callback(resp);
}

void UserAttendance::tooLatePage(const HttpRequestPtr &req,
                                 std::function<void(const drogon::HttpResponsePtr &)> &&callback) {
    auto attendanceStatisticShort{
            req->getSession()->get<std::map<std::string, std::string>>("attendanceStatisticShort")};

    HttpViewData data;
    data.insert("attendanceStatisticShort", attendanceStatisticShort);
    auto resp{HttpResponse::newHttpViewResponse("tooLatePage.csp", data)};
    callback(resp);
}

void UserAttendance::weekendPage(const HttpRequestPtr &req,
                                 std::function<void(const drogon::HttpResponsePtr &)> &&callback) {
    auto attendanceStatisticShort{
            req->getSession()->get<std::map<std::string, std::string>>("attendanceStatisticShort")};

    HttpViewData data;
    data.insert("attendanceStatisticShort", attendanceStatisticShort);
    auto resp{HttpResponse::newHttpViewResponse("weekendPage.csp", data)};
    callback(resp);
}


void UserAttendance::closeSettingsPage(const HttpRequestPtr &req,
                                       std::function<void(const drogon::HttpResponsePtr &)> &&callback) {
    const SessionPtr &session{req->getSession()};
    HttpViewData data;

    if (session->find("tooEarlyKey")) {
        data.insert("attendanceStatisticShort",
                    session->get<std::map<std::string, std::string>>("attendanceStatisticShort"));
        auto resp{HttpResponse::newHttpViewResponse("tooEarlyPage.csp", data)};
        callback(resp);
    } else {
        data = session->get<HttpViewData>("tempHttpViewData");
        auto resp{HttpResponse::newHttpViewResponse("attendanceCheck.csp", data)};
        callback(resp);
    }
}

void UserAttendance::openTableViewPage(const HttpRequestPtr &req,
                                       std::function<void(const drogon::HttpResponsePtr &)> &&callback) {
    const SessionPtr session{req->getSession()};
    std::string username{session->get<std::string>("username")};
    std::string currentDateString{req->getParameter("currentDate")};

    int needToInc{0};
    if (currentDateString.empty()) {
        currentDateString = getCurrentDate();
    } else {
        std::string lastPartOfDate{currentDateString.substr(8, 3)};
        if (lastPartOfDate == "inc") {
            needToInc = 1;
        } else {
            needToInc = -1;
        }
    }

    std::chrono::year_month currentDate{std::chrono::year(std::stoi(currentDateString.substr(0, 4))),
                                        std::chrono::month(std::stoi(currentDateString.substr(5, 2)))};

    if (needToInc == 1) {
        currentDate += std::chrono::months{1};
    } else if (needToInc == -1) {
        currentDate -= std::chrono::months{1};
    }

    std::string currentYearStr{std::to_string(static_cast<int>(currentDate.year()))};
    std::string currentMonthStr{std::to_string(static_cast<unsigned>(currentDate.month()))};

    clientPtr->execSqlAsync(
            "select * from user_intermediate_statistic where username = $1 and DATE_PART('year', date) = $2 and DATE_PART('month', date) = $3",
            [=](const Result &r) {
                std::string totalAmountHoursWorked{"00:00"};

                auto userStatisticAddInfo{
                        [&](std::map<std::string, std::array<std::string, 8>> &usersStatistic,
                            const Result &result) {
                            for (const auto &entry: result) {
                                LOG_INFO <<
                                         entry["count_worked_hour_and_minutes"]
                                                 .as<std::string>();
                                totalAmountHoursWorked = timeArithmetic(totalAmountHoursWorked,
                                                                        removeSeconds(
                                                                                entry["count_worked_hour_and_minutes"]
                                                                                        .as<std::string>()),
                                                                        '+');
                                usersStatistic.insert({
                                                              entry["date"].as<std::string>(),
                                                              {
                                                                      entry["arrival_time"].as<std::string>(),
                                                                      entry["finish_time"].as<std::string>(),
                                                                      entry["attendance_status"].as<std::string>(),
                                                                      entry["break_start"].as<std::string>(),
                                                                      entry["break_finish"].as<std::string>(),
                                                                      entry["count_worked_hour_and_minutes"].as<std::string>(),
                                                              }
                                                      });

                            }
                        }
                };

                std::map<std::string, std::array<std::string, 8>> usersStatisticInfo;
                userStatisticAddInfo(usersStatisticInfo, r);

                session->insert("currentDate_YY_MM", currentDate);
                session->insert("usersStatisticAll", usersStatisticInfo);

                HttpViewData data;
                data.insert("username", username);
                data.insert("shiftStart", session->get<std::string>("shiftStart"));
                data.insert("shiftFinish", session->get<std::string>("shiftFinish"));
                data.insert("currentDate_YY_MM", currentDate);
                data.insert("usersStatisticAll", usersStatisticInfo);
                data.insert("totalAmountHoursWorked", totalAmountHoursWorked);

                auto resp{HttpResponse::newHttpViewResponse("attendanceCheckTableView.csp", data)};
                callback(resp);

            },
            [](const DrogonDbException &e) {
                LOG_ERROR << "error:" << e.base().what();
            }, username, currentYearStr, currentMonthStr);

}

void UserAttendance::closeTableViewPage(const HttpRequestPtr &req,
                                        std::function<void(const drogon::HttpResponsePtr &)> &&callback) {
    const SessionPtr &session{req->getSession()};
    HttpViewData data{session->get<HttpViewData>("tempHttpViewData")};
    data.insert("showCalendarFlag", std::stoi(req->getParameter("showCalendarFlag")));
    auto resp{HttpResponse::newHttpViewResponse("attendanceCheck.csp", data)};
    callback(resp);
}