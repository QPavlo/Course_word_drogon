#pragma once

#include "utils/coder.h"
#include "utils/stringGenerator.h"
#include "utils/applicationSettings.h"
#include "utils/timeDateHandler.h"
#include "../plugins/SMTPMail.h"
#include <drogon/HttpController.h>
#include <drogon/utils/Utilities.h>

using namespace drogon;

class ExecutorAuthentication : public drogon::HttpController<ExecutorAuthentication> {
public:
    METHOD_LIST_BEGIN
        ADD_METHOD_TO(ExecutorAuthentication::signIn, "/Admin/signIn", drogon::Post);
        ADD_METHOD_TO(ExecutorAuthentication::showUsers, "/Admin/showUsers", drogon::Post);
        ADD_METHOD_TO(ExecutorAuthentication::editUserPage, "/Admin/editUserPage", drogon::Post);
        ADD_METHOD_TO(ExecutorAuthentication::editEmptyUserPage, "/Admin/editEmptyUserPage", drogon::Post);
        ADD_METHOD_TO(ExecutorAuthentication::showUsersWithUpdate, "/Admin/updateUsersViewData", drogon::Post);
        ADD_METHOD_TO(ExecutorAuthentication::editUserSettings, "/Admin/editUserSettings", drogon::Post);
        ADD_METHOD_TO(ExecutorAuthentication::editEmptyUserSettings, "/Admin/editEmptyUserSettings", drogon::Post);
        ADD_METHOD_TO(ExecutorAuthentication::showUserStatistics, "/Admin/ShowUserStatistics", drogon::Post);
        ADD_METHOD_TO(ExecutorAuthentication::banUser, "/Admin/BanUser", drogon::Post);
        ADD_METHOD_TO(ExecutorAuthentication::unBlockUser, "/Admin/UnBlockUser", drogon::Post);
    METHOD_LIST_END

    void signIn(const drogon::HttpRequestPtr &req,
                std::function<void(const drogon::HttpResponsePtr &)> &&callback);

    void showUsers(const drogon::HttpRequestPtr &req,
                   std::function<void(const drogon::HttpResponsePtr &)> &&callback);

    void editUserPage(const drogon::HttpRequestPtr &req,
                      std::function<void(const drogon::HttpResponsePtr &)> &&callback);

    void editEmptyUserPage(const drogon::HttpRequestPtr &req,
                           std::function<void(const drogon::HttpResponsePtr &)> &&callback);

    void editUserSettings(const drogon::HttpRequestPtr &req,
                          std::function<void(const drogon::HttpResponsePtr &)> &&callback);

    void editEmptyUserSettings(const drogon::HttpRequestPtr &req,
                               std::function<void(const drogon::HttpResponsePtr &)> &&callback);

    void showUserStatistics(const drogon::HttpRequestPtr &req,
                            std::function<void(const drogon::HttpResponsePtr &)> &&callback);

    void banUser(const drogon::HttpRequestPtr &req,
                            std::function<void(const drogon::HttpResponsePtr &)> &&callback);

    void showUsersWithUpdate(const HttpRequestPtr &req,
                             std::function<void(const drogon::HttpResponsePtr &)> &&callback);

    void unBlockUser(const HttpRequestPtr &req,
                             std::function<void(const drogon::HttpResponsePtr &)> &&callback);

private:
    drogon::orm::DbClientPtr clientPtr{drogon::app().getDbClient("postgresql_client")};
    SMTPMail *smtpmailPtr{(drogon::app().getPlugin<SMTPMail>())};

};
