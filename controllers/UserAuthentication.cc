#include "UserAuthentication.h"

using namespace drogon;
using namespace drogon::orm;

// use locally
#define ADMIN_LOGIN userName == adminSetting::ADMIN_NAME and password == encode(adminSetting::ADMIN_PASSWORD)
#define ON_A_BREAK (breakStart != "00:00:00" and breakFinish == "00:00:00")
#define AFTER_A_BREAK (breakStart != "00:00:00" and breakFinish != "00:00:00")
#define FIRST_WORKING_DAY attendanceStatus == attendanceStatus::FIRST_WORK_DAY_ABSENT or attendanceStatus == attendanceStatus::FIRST_WORK_DAY_PRESENT
#define LATE attendanceStatus == attendanceStatus::LATE
#define IN_TIME attendanceStatus == attendanceStatus::IN_TIME
#define PRESENT attendanceStatus != attendanceStatus::ABSENT
#define WEEKEND selectRow["attendance_status"].as<char>() == static_cast<char>(attendanceStatus::WEEKEND)

void UserAuthentication::signIn(const HttpRequestPtr &req,
                                std::function<void(const HttpResponsePtr &)> &&callback) {
    req->session()->clear();
    std::string userName{req->getParameter("userName")};
    std::string password{std::move(encode(req->getParameter("password")))};

    if (ADMIN_LOGIN) {
        auto resp{HttpResponse::newHttpViewResponse("adminSignIn.csp")};
        callback(resp);
    } else {
        clientPtr->execSqlAsync(
                "select * from user_general where username = $1 and password = $2 and (role = $3 or role = $4 or role = $5 or role = $6 or role = $7)",
                [=](const Result &r) {
                    LOG_INFO << r.size() << " rows selected!";

                    if (r.empty()) {
                        HttpViewData data;
                        data.insert("signInError", std::string("Incorrect login or password"));
                        auto resp{HttpResponse::newHttpViewResponse("signIn.csp", data)};
                        callback(resp);

                    } else {
                        auto selectedUserRole{r.front()["Role"].as<int>()};

                        switch (selectedUserRole) {
                            case Role::TEMP_BAN : {
                                auto resp{
                                        HttpResponse::newHttpViewResponse(
                                                "attendanceCheck.csp")
                                };
                                callback(resp);
                                return;
                            }
                            case Role::FOREVER_BAN: {
                                auto resp{
                                        HttpResponse::newHttpViewResponse(
                                                "permanentBlockAccPage.csp")
                                };
                                callback(resp);
                                return;
                            }

                            case Role::UNBLOCKED: {
                                auto resp{
                                        HttpResponse::newHttpViewResponse(
                                                "unBlockAccPage.csp")
                                };
                                callback(resp);
                                return;
                            }
                            default:;
                        }


                        clientPtr->execSqlAsync(
                                "select ud.username, shift_start, shift_finish, break_start, break_finish, attendance_status, date, working_days from user_general\n"
                                "inner join user_details ud ON user_general.username = ud.username\n"
                                "inner join user_intermediate_statistic uis ON user_general.username = uis.username\n"
                                "where ud.username = $1", [=](const Result &r1) {
                                    if (r1.empty()) {
                                        // worker haven't position
                                        auto resp{HttpResponse::newHttpViewResponse("emptyPage.csp")};
                                        callback(resp);
                                    } else {
                                        // worker have position
                                        auto getCurrentDateRow{[&]() {
                                            for (const auto &entry: r1) {
                                                if (entry["date"].as<std::string>() ==
                                                    getCurrentDate()) {
                                                    return entry;
                                                }
                                            }
                                        }};

                                        Row selectRow{getCurrentDateRow()};

                                        LOG_INFO << "user name is "
                                                 << selectRow["userName"].as<std::string>();

                                        std::string shiftStart{
                                                std::move(removeSeconds(
                                                        selectRow["shift_start"].as<std::string>()))};
                                        std::string shiftFinish{
                                                std::move(removeSeconds(
                                                        selectRow["shift_finish"].as<std::string>()))};

                                        std::string breakStart{
                                                selectRow["break_start"].as<std::string>()};

                                        std::string breakFinish{
                                                selectRow["break_finish"].as<std::string>()};

                                        char attendanceStatus{
                                                selectRow["attendance_status"].as<char>()};

                                        std::string workingDays{
                                                selectRow["working_days"].as<std::string>()};

                                        std::string currentTime{std::move(getCurrentTime())};

                                        auto attendanceStatisticsShort{
                                                std::move(getAttendanceStatistics(r1))};


                                        const SessionPtr &session{req->getSession()};

                                        session->insert("username", userName);
                                        session->insert("shiftStart", shiftStart);
                                        session->insert("shiftFinish", shiftFinish);
                                        session->insert("workingDays", workingDays);
                                        session->insert("attendanceStatusCode", attendanceStatus);
                                        session->insert("attendanceStatisticShort",
                                                        attendanceStatisticsShort);

                                        if (WEEKEND) {
                                            LOG_INFO << "Weekend today";
                                            auto resp{HttpResponse::newHttpViewResponse(
                                                    "weekend.csp")};
                                            callback(resp);
                                        }

                                        HttpViewData data;

                                        if (productionSettings::PRODUCTION_START_WORK >
                                            currentTime) { // production hasn't started yet
                                            data.insert("productionStartWork",
                                                        productionSettings::PRODUCTION_START_WORK);
                                            auto resp{HttpResponse::newHttpViewResponse(
                                                    "tooEarly.csp", data)};
                                            callback(resp);
                                        } else if (productionSettings::PRODUCTION_FINISH_WORK <
                                                   currentTime) { //production is closed
                                            data.insert("shiftFinish",
                                                        shiftFinish);
                                            auto resp{HttpResponse::newHttpViewResponse(
                                                    "tooLate.csp", data)};
                                            callback(resp);
                                        } else {

                                            data.insert("shiftStart", shiftStart);
                                            data.insert("shiftFinish", shiftFinish);
                                            data.insert("attendanceStatusCode", attendanceStatus);
                                            data.insert("attendanceStatisticShort", attendanceStatisticsShort);
                                            if (PRESENT) { // worker present

                                                if (ON_A_BREAK) {
                                                    data.insert("breakFinishTime",
                                                                timeArithmetic(currentTime,
                                                                               productionSettings::MAX_BREAK_TIME,
                                                                               '+'));
                                                    data.insert("attendanceStatusText",
                                                                std::string("On break"));
                                                } else {
                                                    if (AFTER_A_BREAK) {
                                                        data.insert("afterBreakFlag", 1);
                                                    }

                                                    if (IN_TIME) {
                                                        data.insert("attendanceStatusText",
                                                                    std::string("In time"));
                                                    } else if (FIRST_WORKING_DAY) {
                                                        data.insert("attendanceStatusText",
                                                                    std::string("First Day"));
                                                    } else if (LATE) {
                                                        data.insert("attendanceStatusText",
                                                                    std::string("Late"));
                                                    } else {
                                                        data.insert("attendanceStatusText",
                                                                    std::string("Ending"));
                                                    }
                                                    session->insert("attendanceStatusText",
                                                                    data.get<std::string>(
                                                                            "attendanceStatusText"));
                                                }
                                            } else { // absent
                                                data.insert("attendanceStatusText",
                                                            std::string("Didn't arrived"));
                                            }
                                            session->insert("tempHttpViewData", data);

                                            auto resp{HttpResponse::newHttpViewResponse(
                                                    "attendanceCheck.csp", data)};
                                            callback(resp);
                                        }
                                    }
                                },
                                [](const DrogonDbException &e) {
                                    LOG_ERROR << "error:" << e.base().what();
                                }, userName);
                    }
                },
                [](const DrogonDbException &e) {
                    LOG_ERROR << "error:" << e.base().what();
                }, userName, password, Role::USER, Role::UNEMPLOYMENT, Role::TEMP_BAN, Role::FOREVER_BAN,
                Role::UNBLOCKED);

    }
}

void UserAuthentication::signUp(const HttpRequestPtr &req,
                                std::function<void(const HttpResponsePtr &)> &&callback) {
    std::string firstname{req->getParameter("firstName")};
    std::string surname{req->getParameter("secondName")};
    std::string userName{req->getParameter("userName")};
    std::string password{std::move(encode(req->getParameter("password")))};

    clientPtr->execSqlAsync("select username from user_general where username = $1",
                            [=](const Result &result) {
                                if (result.empty()) {
                                    std::string confirmCode{std::move(randomCode())};
                                    smtpmailPtr->sendEmail(
                                            "smtp.gmail.com",
                                            587,
                                            adminSetting::USER_NAME,
                                            userName,
                                            "Confirm your email",
                                            std::string("Your confirm code: ") + confirmCode,
                                            adminSetting::USER_NAME,
                                            adminSetting::PASSWORD,
                                            false
                                    );
                                    const SessionPtr &session{req->getSession()};
                                    session->insert("firstName", firstname);
                                    session->insert("secondName", surname);
                                    session->insert("userName", userName);
                                    session->insert("password", password);
                                    session->insert("confirmCode", confirmCode);
                                    auto resp{HttpResponse::newHttpViewResponse("confirmEmail.csp")};
                                    callback(resp);
                                } else {
                                    //user already in system
                                    HttpViewData data;
                                    data.insert("signUpError",
                                                std::string("User with such email is already registered"));
                                    auto resp{HttpResponse::newHttpViewResponse("signUp.csp", data)};
                                    callback(resp);
                                }
                            },
                            [](const DrogonDbException &e) {
                                LOG_ERROR << "error:" << e.base().what();
                            },
                            userName);
}

void UserAuthentication::signInPage(const HttpRequestPtr &req,
                                    std::function<void(const HttpResponsePtr &)> &&callback) {
    auto resp{HttpResponse::newHttpViewResponse("signIn.csp")};
    callback(resp);
}


void UserAuthentication::signUpPage(const HttpRequestPtr &req,
                                    std::function<void(const drogon::HttpResponsePtr &)> &&callback) {
    auto resp{HttpResponse::newHttpViewResponse("signUp.csp")};
    callback(resp);
}

void UserAuthentication::forgotPasswordPage(const drogon::HttpRequestPtr &req,
                                            std::function<void(const drogon::HttpResponsePtr &)> &&callback) {
    auto resp{HttpResponse::newHttpViewResponse("forgotPasswordPage.csp")};
    callback(resp);
}

void UserAuthentication::confirm(const HttpRequestPtr &req,
                                 std::function<void(const drogon::HttpResponsePtr &)> &&callback) {

    const SessionPtr &session{req->getSession()};
    std::string sentConfirmCode{session->get<std::string>("confirmCode")};
    std::string enteredCode{req->getParameter("enteredCode")};

    if (sentConfirmCode == enteredCode) {
        std::string firstname{session->get<std::string>("firstName")};
        std::string surname{session->get<std::string>("secondName")};
        std::string userName{session->get<std::string>("userName")};
        std::string password{session->get<std::string>("password")};

        session->clear();

        try {
            auto resUserGeneral{clientPtr->execSqlSync(
                    "insert into user_general (firstname, surname, username, password)"
                    "values($1,$2,$3,$4);", firstname, surname, userName, password)};

            LOG_INFO << resUserGeneral.affectedRows() << "row updated";
            LOG_INFO << "UserAuthentication " << userName << " added";
        } catch (const DrogonDbException &e) {
            LOG_ERROR << "error:" << e.base().what();
        }
        auto resp{HttpResponse::newHttpViewResponse("successfulRegistration.csp")};
        callback(resp);
    } else {
        auto resp{HttpResponse::newHttpViewResponse("confirmError.csp")};
        callback(resp);
    }
}

void UserAuthentication::forgotPassword(const HttpRequestPtr &req,
                                        std::function<void(const HttpResponsePtr &)> &&callback) {
    req->getSession()->clear();
    std::string userName{req->getParameter("userName")};
    auto result{clientPtr->execSqlSync("select username from user_general where username = $1",
                                       userName)};
    if (!result.empty()) {
        std::string newPassword{std::move(randomPassword())};
        smtpmailPtr->sendEmail(
                "smtp.gmail.com",
                587,
                adminSetting::USER_NAME,
                userName,
                "New Password",
                std::string("Your new password: ") + newPassword,
                adminSetting::USER_NAME,
                adminSetting::PASSWORD,
                false
        );

        try {
            auto r = clientPtr->execSqlSync(
                    "update user_general set password=$1 where username=$2",
                    std::move(encode(newPassword)),
                    userName);
            LOG_INFO << r.affectedRows() << " rows updated!";
        } catch (const DrogonDbException &e) {
            LOG_ERROR << "error:" << e.base().what();
        }
        auto resp{HttpResponse::newHttpViewResponse("passwordSent.csp")};
        callback(resp);
    } else {
        HttpViewData data;
        data.insert("sendError",
                    std::string("User with such email none in system"));
        auto resp{HttpResponse::newHttpViewResponse("forgotPasswordPage.csp", data)};
        callback(resp);
    }
}
