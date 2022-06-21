#include "AdminAuthentication.h"

using namespace drogon;
using namespace drogon::orm;


void ExecutorAuthentication::signIn(const HttpRequestPtr &req,
                                    std::function<void(const drogon::HttpResponsePtr &)> &&callback) {
    req->session()->clear();
    std::string userName{req->getParameter("userName")};
    std::string password{std::move(encode(req->getParameter("password")))};
    clientPtr->execSqlAsync("select * from user_general where username = $1 and password = $2 and role = $3",
                            [=](const Result &r) {
                                LOG_INFO << r.size() << " rows selected!";
                                if (r.empty()) {
                                    HttpViewData data;
                                    data.insert("signInError", std::string("Incorrect login or password"));
                                    auto resp{HttpResponse::newHttpViewResponse("adminSignIn.csp", data)};
                                    callback(resp);
                                } else {
                                    auto userGeneralAllAddInfo{
                                            [&](std::map<std::string, std::array<std::string, 3>> &usersGeneral,
                                                const Result &result) {
                                                for (const auto &entry: result) {
                                                    usersGeneral.insert({
                                                                                entry["username"].as<std::string>(),
                                                                                {entry["firstname"].as<std::string>(),
                                                                                 entry["surname"].as<std::string>(),
                                                                                 std::to_string(entry["role"].as<int>())
                                                                                }
                                                                        });
                                                }
                                            }
                                    };

                                    try {
                                        auto resultAllUsers{clientPtr->execSqlSync(
                                                "select firstname, surname, username, role from user_general"
                                        )};

                                        std::map<std::string, std::array<std::string, 3>> usersGeneralAll;
                                        userGeneralAllAddInfo(usersGeneralAll, resultAllUsers);

                                        const SessionPtr session{req->getSession()};
                                        session->insert("usersGeneralAll", usersGeneralAll);

                                        HttpViewData data;
                                        data.insert("usersType", static_cast<int>(UsersType::ALL_USERS_NOT_BLOCKED));
                                        data.insert("usersGeneralAll", usersGeneralAll);

                                        auto resp{HttpResponse::newHttpViewResponse(
                                                "adminStatisticPage.csp", data)};
                                        callback(resp);
                                    }
                                    catch (const DrogonDbException &e) {
                                        LOG_ERROR << "error:" << e.base().what();
                                    }
                                }
                            },
                            [](const DrogonDbException &e) {
                                LOG_ERROR << "error:" << e.base().what();
                            }, userName, password, Role::ADMIN);

}

void ExecutorAuthentication::showUsers(const HttpRequestPtr &req,
                                       std::function<void(const drogon::HttpResponsePtr &)> &&callback) {
    const SessionPtr session{req->getSession()};
    session->erase("shiftStart");
    session->erase("shiftFinish");

    HttpViewData data;

    data.insert("usersType", std::stoi(req->getParameter("usersType")));
    data.insert("usersGeneralAll", session->get<std::map<std::string, std::array<std::string, 3>>>("usersGeneralAll"));
    auto resp{HttpResponse::newHttpViewResponse(
            "adminStatisticPage.csp", data)};
    callback(resp);
}

void ExecutorAuthentication::showUsersWithUpdate(const HttpRequestPtr &req,
                                                 std::function<void(const drogon::HttpResponsePtr &)> &&callback) {
    const SessionPtr session{req->getSession()};
    auto isUpdate{session->get<int>("updateFlag")};
    session->erase("shiftStart");
    session->erase("shiftFinish");

    auto userGeneralAllAddInfo{
            [&](std::map<std::string, std::array<std::string, 3>> &usersGeneral,
                const Result &result) {
                for (const auto &entry: result) {
                    usersGeneral.insert({
                                                entry["username"].as<std::string>(),
                                                {entry["firstname"].as<std::string>(),
                                                 entry["surname"].as<std::string>(),
                                                 std::to_string(entry["role"].as<int>())
                                                }
                                        });
                }
            }
    };

    try {
        HttpViewData data;
        if (isUpdate == 1) {
            auto resultAllUsers{clientPtr->execSqlSync(
                    "select firstname, surname, username, role from user_general where role = $1 or role = $2 or role = $3",
                    Role::UNEMPLOYMENT, Role::USER, Role::UNBLOCKED)};

            std::map<std::string, std::array<std::string, 3>> usersGeneralAll;

            userGeneralAllAddInfo(usersGeneralAll, resultAllUsers);

            session->modify<std::map<std::string, std::array<std::string, 3>>>("usersGeneralAll",
                                                                               [&](auto &map) {
                                                                                   map = usersGeneralAll;
                                                                               });

            data.insert("usersGeneralAll", usersGeneralAll);
        } else {
            data.insert("usersGeneralAll",
                        session->get<std::map<std::string, std::array<std::string, 3>>>("usersGeneralAll"));
        }
        data.insert("usersType", static_cast<int>(UsersType::ALL_USERS_NOT_BLOCKED));
        auto resp{HttpResponse::newHttpViewResponse(
                "adminStatisticPage.csp", data)};
        callback(resp);
    }
    catch (const DrogonDbException &e) {
        LOG_ERROR << "error:" << e.base().what();
    }

}

void ExecutorAuthentication::editUserPage(const HttpRequestPtr &req,
                                          std::function<void(const drogon::HttpResponsePtr &)> &&callback) {
    const SessionPtr &session{req->getSession()};
    std::string username{req->getParameter("username")};
    std::string blockType{req->getParameter("blockType")};
    clientPtr->execSqlAsync("select * from user_general\n"
                            "inner join user_details ud ON user_general.username = ud.username\n"
                            "inner join user_temporary_data utd ON user_general.username = utd.username\n"
                            "where ud.username = $1;", [=](const Result &r) {
        if (!r.empty()) {
            HttpViewData data;
            Row row{r.front()};
            data.insert("firstname", row["firstname"].as<std::string>());
            data.insert("surname", row["surname"].as<std::string>());
            data.insert("username", username);
            data.insert("appointment", row["temp_appointment"].as<std::string>());
            data.insert("shiftStart", removeSeconds(row["temp_shift_start"].as<std::string>()));
            data.insert("shiftFinish", removeSeconds(row["temp_shift_finish"].as<std::string>()));
            data.insert("workingDays", row["temp_working_days"].as<std::string>());
            data.insert("street", row["street"].as<std::string>());
            data.insert("city", row["city"].as<std::string>());
            data.insert("zip_code", row["zip_code"].as<std::string>());
            data.insert("phone_number", row["phone_number"].as<std::string>());
            data.insert("count_updated_fields", static_cast<size_t>(0));
            data.insert("emptyUserFlag", 0);

            if (blockType.empty()) {
                auto resp{HttpResponse::newHttpViewResponse("adminUserSettings.csp", data)};
                callback(resp);
            } else if (blockType == "temp") {
                auto resp{HttpResponse::newHttpViewResponse("adminUserSettingsTempBlockUser.csp", data)};
                callback(resp);
            } else {
                auto resp{HttpResponse::newHttpViewResponse("adminUserSettingsPermanentlyBlock.csp", data)};
                callback(resp);
            }
        }

    }, [](const DrogonDbException &e) {
        LOG_ERROR << "error:" << e.base().what();
    }, username);

}

void ExecutorAuthentication::editUserSettings(const HttpRequestPtr &req,
                                              std::function<void(const drogon::HttpResponsePtr &)> &&callback) {
    const SessionPtr &session{req->getSession()};
    std::string username{req->getParameter("username")};
    std::string appointment{req->getParameter("appointment")};
    std::string shiftStart{req->getParameter("shiftStart")};
    std::string shiftFinish{req->getParameter("shiftFinish")};
    std::string workingDays{req->getParameter("weekdays")};

    try {
        auto affectedRowsUserDetails{clientPtr->execSqlSync(
                " update user_temporary_data \n"
                " set"
                " temp_appointment = $1,"
                " temp_shift_start = $2,"
                " temp_shift_finish = $3,"
                " temp_working_days = $4"
                " where username = $5"
                " and (temp_appointment <> $1 or temp_shift_start <> $2 or temp_shift_finish <> $3 or temp_working_days <> $4)",
                appointment, shiftStart,
                shiftFinish, workingDays, username)};

        size_t countAffectedRowsUserDetails{affectedRowsUserDetails.affectedRows()};
        LOG_INFO << countAffectedRowsUserDetails << "row updated";
        if (countAffectedRowsUserDetails > 0) {

            std::string temp_str{mailMessages::updateWorkScheduleMessage(appointment, shiftStart,
                                                                         shiftFinish, workingDays)};
            std::cout << "\n\n" << temp_str << std::endl;
            smtpmailPtr->sendEmail("smtp.gmail.com",
                                   587,
                                   adminSetting::USER_NAME,
                                   username,
                                   "Schedule updating",
                                   temp_str,
                                   adminSetting::USER_NAME,
                                   adminSetting::PASSWORD,
                                   true);
            session->insert("updateFlag", 1);
        } else {
            session->erase("updateFlag");
        }

        HttpViewData data;

        data.insert("firstname", req->getParameter("firstname"));
        data.insert("surname", req->getParameter("surname"));
        data.insert("username", username);
        data.insert("appointment", appointment);
        data.insert("shiftStart", shiftStart);
        data.insert("shiftFinish", shiftFinish);
        data.insert("workingDays", workingDays);
        data.insert("street", req->getParameter("street"));
        data.insert("city", req->getParameter("city"));
        data.insert("zip_code", req->getParameter("zip_code"));
        data.insert("phone_number", req->getParameter("phone_number"));
        data.insert("count_updated_fields", (//countAffectedRowsUserGeneral+
                countAffectedRowsUserDetails));

//             updateSessionViewData(session, data);
        auto resp{HttpResponse::newHttpViewResponse("adminUserSettings.csp", data)};
        callback(resp);
    } catch (const DrogonDbException &e) {
        LOG_ERROR << "error:" << e.base().what();
    }
}

void ExecutorAuthentication::editEmptyUserPage(const HttpRequestPtr &req,
                                               std::function<void(const drogon::HttpResponsePtr &)> &&callback) {
    const SessionPtr &session{req->getSession()};
    std::string firstname{req->getParameter("firstname")};
    std::string surname{req->getParameter("surname")};
    std::string username{req->getParameter("username")};

    HttpViewData data;
    data.insert("firstname", firstname);
    data.insert("surname", surname);
    data.insert("username", username);
    data.insert("appointment", "");
    data.insert("shiftStart", "");
    data.insert("shiftFinish", "");
    data.insert("street", "");
    data.insert("city", "");
    data.insert("zip_code", "");
    data.insert("phone_number", "");
    data.insert("count_updated_fields", static_cast<size_t>(0));
    data.insert("emptyUserFlag", 1);
    auto resp{HttpResponse::newHttpViewResponse("adminUserSettings.csp", data)};
    callback(resp);
}

void ExecutorAuthentication::editEmptyUserSettings(const HttpRequestPtr &req,
                                                   std::function<void(const drogon::HttpResponsePtr &)> &&callback) {
    const SessionPtr &session{req->getSession()};

    std::string firstname{req->getParameter("firstname")};
    std::string surname{req->getParameter("surname")};
    std::string username{req->getParameter("username")};
    std::string appointment{req->getParameter("appointment")};
    std::string shiftStart{req->getParameter("shiftStart")};
    std::string shiftFinish{req->getParameter("shiftFinish")};
    std::string workingDays{req->getParameter("weekdays")};

    try {
        auto affectedRowsUserGeneral{
                clientPtr->execSqlSync("update user_general set role = $1 where username = $2",
                                       static_cast<int>(Role::USER), username)
        };

        auto affectedRowsUserDetails{clientPtr->execSqlSync(
                " insert into user_details \n"
                " (username,"
                " appointment,"
                " shift_start,"
                " shift_finish,"
                " working_days) "
                " values ($1, $2, $3, $4, $5)", username, appointment, shiftStart, shiftFinish, workingDays)};

        clientPtr->execSqlSync("insert into user_temporary_data \n"
                               "(username, "
                               " temp_appointment,"
                               " temp_shift_start,"
                               " temp_shift_finish,"
                               " temp_working_days) "
                               " values ($1, $2, $3, $4, $5)", username,
                               appointment, shiftStart, shiftFinish, workingDays);

        auto currentDate{
                std::chrono::floor<std::chrono::days>(std::chrono::system_clock::now())};

        std::chrono::year_month_day temp_ymd{currentDate};

        std::string temp_date{std::move(std::string(std::to_string(static_cast<int>(temp_ymd.year())) + "-" +
                                                    std::to_string(static_cast<unsigned>(temp_ymd.month())) + "-" +
                                                    std::to_string(static_cast<unsigned int>(temp_ymd.day()))
        ))};
        if (!(workingDays.find(std::to_string(std::chrono::weekday(currentDate).
                c_encoding())) + 1)) {
            do {
                currentDate += std::chrono::days{1};

                temp_ymd = currentDate;
                temp_date = std::move(std::string(std::to_string(static_cast<int>(temp_ymd.year())) + "-" +
                                                  std::to_string(static_cast<unsigned>(temp_ymd.month())) + "-" +
                                                  std::to_string(static_cast<unsigned int>(temp_ymd.day()))
                ));

                auto tempCurrDateBD{clientPtr->execSqlSync("select * from date_statistic where date = $1", temp_date)};
                if (temp_date.empty()) {
                    clientPtr->execSqlSync("insert into date_statistic values($1)", temp_date);
                }
                clientPtr->execSqlSync("insert into user_intermediate_statistic \n"
                                       " (username,"
                                       " attendance_status, "
                                       " date)"
                                       " values ($1, $2, $3)", username, static_cast<char>(attendanceStatus::WEEKEND),
                                       temp_date
                );
            } while (!(workingDays.find(std::to_string(std::chrono::weekday(currentDate).
                    c_encoding())) + 1));
        }

        clientPtr->execSqlSync(
                " insert into user_intermediate_statistic \n"
                " (username,"
                " attendance_status,"
                " date)"
                " values ($1, $2, $3)", username, static_cast<char>(attendanceStatus::FIRST_WORK_DAY_ABSENT),
                temp_date);

        //  size_t countAffectedRowsUserGeneral{affectedRowsUserGeneral.affectedRows()};
        size_t countAffectedRowsUserDetails{affectedRowsUserDetails.affectedRows()};

        LOG_INFO << countAffectedRowsUserDetails << "row updated";
        HttpViewData data;

        data.insert("firstname", firstname);
        data.insert("surname", surname);
        data.insert("username", username);
        data.insert("appointment", appointment);
        data.insert("shiftStart", shiftStart);
        data.insert("shiftFinish", shiftFinish);
        data.insert("street", "");
        data.insert("city", "");
        data.insert("zip_code", "");
        data.insert("phone_number", "");
        data.insert("count_updated_fields", countAffectedRowsUserDetails);

        data.insert("emptyUserFlag", 0);
        if ((countAffectedRowsUserDetails
            ) > 0) {
            session->insert("updateFlag", 1);
        } else {
            session->erase("updateFlag");
        }
        auto resp{HttpResponse::newHttpViewResponse("adminUserSettings.csp", data)};
        callback(resp);
    } catch (const DrogonDbException &e) {
        LOG_ERROR << "error:" << e.base().what();
    }
}

void ExecutorAuthentication::showUserStatistics(const HttpRequestPtr &req,
                                                std::function<void(const drogon::HttpResponsePtr &)> &&callback) {
    std::string username{req->getParameter("username")};
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
                                totalAmountHoursWorked = timeArithmetic(totalAmountHoursWorked,
                                                                        removeSeconds(
                                                                                entry["count_worked_hour_and_minutes"]
                                                                                        .as<std::string>()),
                                                                        '+');
                            }

                        }
                };


                std::map<std::string, std::array<std::string, 8>> usersStatisticInfo;
                userStatisticAddInfo(usersStatisticInfo, r);

                const SessionPtr session{req->getSession()};
                session->insert("currentDate_YY_MM", currentDate);
                session->insert("usersStatisticAll", usersStatisticInfo);

                if (!session->find("shiftStart") and !session->find("shiftFinish")) {
                    auto shiftInfo{
                            clientPtr->execSqlSync(
                                    "select temp_shift_start, temp_shift_finish from user_temporary_data where username=$1",
                                    username).front()};
                    session->insert("shiftStart", removeSeconds(shiftInfo["temp_shift_start"].as<std::string>()));
                    session->insert("shiftFinish", removeSeconds(shiftInfo["temp_shift_finish"].as<std::string>()));
                }


                HttpViewData data;
                data.insert("username", username);
                data.insert("shiftStart", session->get<std::string>("shiftStart"));
                data.insert("shiftFinish", session->get<std::string>("shiftFinish"));
                data.insert("currentDate_YY_MM", currentDate);
                data.insert("usersStatisticAll", usersStatisticInfo);
                data.insert("totalAmountHoursWorked", totalAmountHoursWorked);

                auto resp{HttpResponse::newHttpViewResponse("adminStatisticDateTablePage.csp", data)};
                callback(resp);

            },
            [](const DrogonDbException &e) {
                LOG_ERROR << "error:" << e.base().what();
            }, username, currentYearStr, currentMonthStr);

}

void ExecutorAuthentication::banUser(const HttpRequestPtr &req,
                                     std::function<void(const drogon::HttpResponsePtr &)> &&callback) {
    const SessionPtr session{req->getSession()};

    std::string banStatus{req->getParameter("banStatus")};
    std::string username{req->getParameter("username")};

    auto updateUserRoleDataSession{[&](const std::string &banType) {
        session->modify<std::map<std::string, std::array<std::string, 3>>>("usersGeneralAll",
                                                                           [&](auto &usersGeneralAll) {
                                                                               auto itr{usersGeneralAll.find(
                                                                                       username)};

                                                                               if (itr != usersGeneralAll.end()) {
                                                                                   itr->second[2] = banType;
                                                                               }
                                                                           });
    }};

    HttpViewData data;
    if (banStatus == "Temp") {
//        updateDateStatisticInSession()
        clientPtr->execSqlSync("update user_general set role = $1 where username = $2",
                               static_cast<int>(Role::TEMP_BAN), username);
        updateUserRoleDataSession(std::to_string(Role::TEMP_BAN));
        smtpmailPtr->sendEmail("smtp.gmail.com",
                               587,
                               adminSetting::USER_NAME,
                               username,
                               "Temporary ban",
                               mailMessages::TEMPORARY_BLOCK_EMAIL_MESSAGE,
                               adminSetting::USER_NAME,
                               adminSetting::PASSWORD,
                               true);
        data.insert("blockStatus", "temporarily blocked");
    } else {
        clientPtr->execSqlSync("update user_general set role = $1 where username = $2",
                               static_cast<int>(Role::FOREVER_BAN), username);
        updateUserRoleDataSession(std::to_string(Role::FOREVER_BAN));
        smtpmailPtr->sendEmail("smtp.gmail.com",
                               587,
                               adminSetting::USER_NAME,
                               username,
                               "Eternal ban",
                               mailMessages::PERMANENT_BLOCK_EMAIL_MESSAGE,
                               adminSetting::USER_NAME,
                               adminSetting::PASSWORD,
                               true);
        data.insert("blockStatus", "permanently blocked");
    }

    data.insert("username", username);
    data.insert("userBlockFlag", 1);

//    session->erase("shiftStart");
//    session->erase("shiftFinish");

    try {

        data.insert("usersGeneralAll",
                    session->get<std::map<std::string, std::array<std::string, 3>>>("usersGeneralAll"));
        data.insert("usersType", static_cast<int>(UsersType::ALL_USERS_NOT_BLOCKED));
    } catch (const DrogonDbException &e) {
        LOG_ERROR << "error:" << e.base().what();
    }
    auto resp{HttpResponse::newHttpViewResponse("adminStatisticPage.csp", data)};
    callback(resp);
}

void ExecutorAuthentication::unBlockUser(const HttpRequestPtr &req,
                                         std::function<void(const drogon::HttpResponsePtr &)> &&callback) {
    const SessionPtr session{req->getSession()};

    std::string username{req->getParameter("username")};

    auto updateUserRoleDataSession{[&](const std::string &banType) {
        session->modify<std::map<std::string, std::array<std::string, 3>>>("usersGeneralAll",
                                                                           [&](auto &usersGeneralAll) {
                                                                               auto itr{usersGeneralAll.find(
                                                                                       username)};

                                                                               if (itr != usersGeneralAll.end()) {
                                                                                   itr->second[2] = banType;
                                                                               }
                                                                           });
    }};

    HttpViewData data;

    clientPtr->execSqlSync("update user_general set role = $1 where username = $2",
                           static_cast<int>(Role::UNBLOCKED), username);

    updateUserRoleDataSession(std::to_string(Role::USER));
    smtpmailPtr->sendEmail("smtp.gmail.com",
                           587,
                           adminSetting::USER_NAME,
                           username,
                           "Unblock account",
                           mailMessages::UNBLOCK_EMAIL_MESSAGE,
                           adminSetting::USER_NAME,
                           adminSetting::PASSWORD,
                           true);

    data.insert("username", username);
    data.insert("blockStatus", std::string("unblocked"));

//    session->erase("shiftStart");
//    session->erase("shiftFinish");
    data.insert("userUnblockFlag", 1);
    try {
        data.insert("usersGeneralAll",
                    session->get<std::map<std::string, std::array<std::string, 3>>>("usersGeneralAll"));
        data.insert("usersType", static_cast<int>(UsersType::ALL_USERS_NOT_BLOCKED));
    } catch (const DrogonDbException &e) {
        LOG_ERROR << "error:" << e.base().what();
    }
    auto resp{HttpResponse::newHttpViewResponse("adminStatisticPage.csp", data)};
    callback(resp);
}
