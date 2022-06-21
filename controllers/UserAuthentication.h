#pragma once

#include "utils/coder.h"
#include "utils/stringGenerator.h"
#include "utils/applicationSettings.h"
#include "utils/timeDateHandler.h"
#include "../plugins/SMTPMail.h"
#include <drogon/HttpController.h>
#include <drogon/utils/Utilities.h>

class UserAuthentication : public drogon::HttpController<UserAuthentication> {

public:
    METHOD_LIST_BEGIN
        ADD_METHOD_TO(UserAuthentication::signIn, "/signIn", drogon::Post);
        ADD_METHOD_TO(UserAuthentication::signInPage, "/start", drogon::Get);
        ADD_METHOD_TO(UserAuthentication::signUp, "/signUp", drogon::Post);
        ADD_METHOD_TO(UserAuthentication::signUpPage, "/signUpPage", drogon::Get);
        ADD_METHOD_TO(UserAuthentication::confirm, "/confirm", drogon::Post);
        ADD_METHOD_TO(UserAuthentication::forgotPassword, "/forgot", drogon::Post);
        ADD_METHOD_TO(UserAuthentication::forgotPasswordPage, "/forgotPage", drogon::Get, drogon::Post);
    METHOD_LIST_END

    void signIn(const drogon::HttpRequestPtr &req,
                std::function<void(const drogon::HttpResponsePtr &)> &&callback);

    void signUp(const drogon::HttpRequestPtr &req,
                std::function<void(const drogon::HttpResponsePtr &)> &&callback);

    void signInPage(const drogon::HttpRequestPtr &req,
                    std::function<void(const drogon::HttpResponsePtr &)> &&callback);

    void signUpPage(const drogon::HttpRequestPtr &req,
                    std::function<void(const drogon::HttpResponsePtr &)> &&callback);

    void confirm(const drogon::HttpRequestPtr &req,
                 std::function<void(const drogon::HttpResponsePtr &)> &&callback);

    void forgotPassword(const drogon::HttpRequestPtr &req,
                        std::function<void(const drogon::HttpResponsePtr &)> &&callback);

    void forgotPasswordPage(const drogon::HttpRequestPtr &req,
                            std::function<void(const drogon::HttpResponsePtr &)> &&callback);


private:
    drogon::orm::DbClientPtr clientPtr{drogon::app().getDbClient("postgresql_client")};
    SMTPMail *smtpmailPtr{(drogon::app().getPlugin<SMTPMail>())};
};
