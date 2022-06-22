#pragma once

#include "utils/applicationSettings.h"
#include "utils/timeDateHandler.h"
#include "utils/coder.h"
#include "../plugins/SMTPMail.h"
#include <drogon/HttpController.h>

using namespace drogon;

class UserAttendance : public drogon::HttpController<UserAttendance> {
public:
    METHOD_LIST_BEGIN
        ADD_METHOD_TO(UserAttendance::userSettingsPage, "/userSettingsPage", drogon::Get, drogon::Post);
        ADD_METHOD_TO(UserAttendance::openTableViewPage, "/openTableViewPage", drogon::Post);
        ADD_METHOD_TO(UserAttendance::closeSettingsPage, "/closeSettingsPage", drogon::Post);
        ADD_METHOD_TO(UserAttendance::closeTableViewPage, "/closeTableViewPage", drogon::Post);
        ADD_METHOD_TO(UserAttendance::tooEarlyPage, "/tooEarlyPage", drogon::Post);
        ADD_METHOD_TO(UserAttendance::weekendPage, "/weekendPage", drogon::Post);
        ADD_METHOD_TO(UserAttendance::tooLatePage, "/tooLatePage", drogon::Post);
        ADD_METHOD_TO(UserAttendance::updateUserSettings, "/updateUserSettings", drogon::Post);
        ADD_METHOD_TO(UserAttendance::updateUserPassword, "/updateUserPassword", drogon::Post);
        ADD_METHOD_TO(UserAttendance::confirmArrival, "/confirmArrival", drogon::Post);
        ADD_METHOD_TO(UserAttendance::confirmEnding, "/confirmEnding", drogon::Post);
        ADD_METHOD_TO(UserAttendance::takeBreak, "/takeBreak", drogon::Post);
        ADD_METHOD_TO(UserAttendance::endBreak, "/endBreak", drogon::Post);
    METHOD_LIST_END

    void userSettingsPage(const drogon::HttpRequestPtr &req,
                          std::function<void(const drogon::HttpResponsePtr &)> &&callback);

    void tooEarlyPage(const drogon::HttpRequestPtr &req,
                      std::function<void(const drogon::HttpResponsePtr &)> &&callback);

    void weekendPage(const drogon::HttpRequestPtr &req,
                      std::function<void(const drogon::HttpResponsePtr &)> &&callback);

    void tooLatePage(const drogon::HttpRequestPtr &req,
                     std::function<void(const drogon::HttpResponsePtr &)> &&callback);

    void updateUserSettings(const drogon::HttpRequestPtr &req,
                            std::function<void(const drogon::HttpResponsePtr &)> &&callback);


    void updateUserPassword(const drogon::HttpRequestPtr &req,
                            std::function<void(const drogon::HttpResponsePtr &)> &&callback);

    void confirmArrival(const drogon::HttpRequestPtr &req,
                        std::function<void(const drogon::HttpResponsePtr &)> &&attendanceStatusInput);

    void confirmEnding(const drogon::HttpRequestPtr &attendanceStatusInput,
                       std::function<void(const drogon::HttpResponsePtr &)> &&callback);

    void takeBreak(const drogon::HttpRequestPtr &req,
                   std::function<void(const drogon::HttpResponsePtr &)> &&callback);

    void endBreak(const drogon::HttpRequestPtr &req,
                  std::function<void(const drogon::HttpResponsePtr &)> &&callback);

//    void attendancePage(const HttpRequestPtr &req,
//                        std::function<void(const drogon::HttpResponsePtr &)> &&callback);

    void openTableViewPage(const HttpRequestPtr &req,
                      std::function<void(const drogon::HttpResponsePtr &)> &&callback);

    void closeTableViewPage(const HttpRequestPtr &req,
                            std::function<void(const drogon::HttpResponsePtr &)> &&callback);

    void closeSettingsPage(const HttpRequestPtr &req,
                           std::function<void(const drogon::HttpResponsePtr &)> &&callback);

private:
    void updateDateStatisticShort(const SessionPtr &session, char attendanceStatus);

    drogon::orm::DbClientPtr clientPtr{drogon::app().getDbClient("postgresql_client")};
    SMTPMail *smtpmailPtr{(drogon::app().getPlugin<SMTPMail>())};

    const std::string updateUserStatistic{"update user_intermediate_statistic set attendance_status = $1"
                                          "where username = $2 and date = CURRENT_DATE;"};

};
