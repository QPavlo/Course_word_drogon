#pragma once
#include <string>

namespace adminSetting {
    constexpr char USER_NAME[]{"pavlo.andriiv.knm.2020@lpnu.ua"};
    constexpr char PASSWORD[]{"28.06.2003"};
    constexpr char ADMIN_NAME[]{"admin"};
    constexpr char ADMIN_PASSWORD[]{"admin_first_password"};
    //  "Long_And_Strong_Password_For_Admin_Account_1234"
}

namespace productionSettings {
    constexpr char MAX_BREAK_TIME[]{"00:45"};
    constexpr char PRODUCTION_START_WORK[]{"00:00"};
    constexpr char PRODUCTION_FINISH_WORK[]{"23:00"};
}

namespace mailMessages {
    constexpr char PERMANENT_BLOCK_EMAIL_MESSAGE[]{
            "<!DOCTYPE html>\n"
            "<html lang=\"en\">\n"
            "\n"
            "\n"
            "<head>\n"
            "    <link href=\"https://fonts.cdnfonts.com/css/abyssinica-sil\" rel=\"stylesheet\">\n"
            "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"/>\n"
            "\n"
            "</head>\n"
            "\n"
            "<body style=\"background: linear-gradient(to right, #7a8fff, #60e7ff);\n"
            "                          font-family: 'Abyssinica SIL', sans-serif; padding: 100px;\">\n"
            "<div class=\"main\"  style=\"background-color: #dcdcdc;\n"
            "            width: 400px;\n"
            "            height: 320px;\n"
            "            margin: 7em auto;\n"
            "            border-radius: 0.5em;\n"
            "            box-shadow: 0 11px 35px 2px rgba(0, 0, 0, 0.14);\">\n"
            "    <p class=\"sign\" align=\"center\" style=\"\n"
            "            padding-top: 40px;\n"
            "            color: #e12424;\n"
            "            font-family: 'Abyssinica SIL', sans-serif;\n"
            "            font-weight: bold;\n"
            "            font-size: 29px;\">Permanent block :(</p>\n"
            "    <p class=\"sign1\" align=\"center\" style=\"padding-top: 20px;\n"
            "            color: rgba(32, 32, 32, 0.66);\n"
            "            font-family: 'Abyssinica SIL', sans-serif;\n"
            "    font-weight: bold;\n"
            "    font-size: 23px;\n"
            "    margin: 30px;\">Unfortunately you have been banned permanently, access to the site is blocked forever\n"
            "        use the site again</p>\n"
            "    <form class=\"singInForm\">\n"
            "\n"
            "\n"
            "    </form>\n"
            "</div>\n"
            "\n"
            "</body>\n"
            "</html>\n"
    };

    constexpr char TEMPORARY_BLOCK_EMAIL_MESSAGE[]{
            "<!DOCTYPE html>\n"
            "<html lang=\"en\">\n"
            "\n"
            "\n"
            "<head>\n"
            "    <link href=\"https://fonts.cdnfonts.com/css/abyssinica-sil\" rel=\"stylesheet\">\n"
            "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"/>\n"
            "\n"
            "</head>\n"
            "\n"
            "<body style=\"background: linear-gradient(to right, #7a8fff, #60e7ff);\n"
            "                          font-family: 'Abyssinica SIL', sans-serif; padding: 100px;\">\n"
            "<div class=\"main\"  style=\"background-color: #dcdcdc;\n"
            "            width: 400px;\n"
            "            height: 320px;\n"
            "            margin: 7em auto;\n"
            "            border-radius: 0.5em;\n"
            "            box-shadow: 0 11px 35px 2px rgba(0, 0, 0, 0.14);\">\n"
            "    <p class=\"sign\" align=\"center\" style=\"\n"
            "            padding-top: 40px;\n"
            "            color: #e1aa24;\n"
            "            font-family: 'Abyssinica SIL', sans-serif;\n"
            "            font-weight: bold;\n"
            "            font-size: 29px;\">Temporary block :(</p>\n"
            "    <p class=\"sign1\" align=\"center\" style=\"padding-top: 20px;\n"
            "            color: rgba(32, 32, 32, 0.66);\n"
            "            font-family: 'Abyssinica SIL', sans-serif;\n"
            "    font-weight: bold;\n"
            "    font-size: 23px;\n"
            "    margin: 30px;\">Unfortunately, you have received a temporary ban, if you are unlocked then you can\n"
            "        use the site again</p>\n"
            "    <form class=\"singInForm\">\n"
            "\n"
            "\n"
            "    </form>\n"
            "</div>\n"
            "\n"
            "</body>\n"
            "</html>\n"
    };

    constexpr char UNBLOCK_EMAIL_MESSAGE[]{
            "<!DOCTYPE html>\n"
            "<html lang=\"en\">\n"
            "\n"
            "\n"
            "<head>\n"
            "    <link href=\"https://fonts.cdnfonts.com/css/abyssinica-sil\" rel=\"stylesheet\">\n"
            "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"/>\n"
            "\n"
            "</head>\n"
            "\n"
            "<body style=\"background: linear-gradient(to right, #7a8fff, #60e7ff);\n"
            "                          font-family: 'Abyssinica SIL', sans-serif; padding: 100px;\">\n"
            "<div class=\"main\"  style=\"background-color: #dcdcdc;\n"
            "            width: 400px;\n"
            "            height: 320px;\n"
            "            margin: 7em auto;\n"
            "            border-radius: 0.5em;\n"
            "            box-shadow: 0 11px 35px 2px rgba(0, 0, 0, 0.14);\">\n"
            "    <p class=\"sign\" align=\"center\" style=\"\n"
            "            padding-top: 40px;\n"
            "            color: green;\n"
            "            font-family: 'Abyssinica SIL', sans-serif;\n"
            "            font-weight: bold;\n"
            "            font-size: 29px;\">your account is unlocked</p>\n"
            "    <p class=\"sign1\" align=\"center\" style=\"padding-top: 20px;\n"
            "            color: rgba(32, 32, 32, 0.66);\n"
            "            font-family: 'Abyssinica SIL', sans-serif;\n"
            "    font-weight: bold;\n"
            "    font-size: 23px;\n"
            "    margin: 30px;\">Fortunately, your ban has been removed, you can use the site again\n"
            "       </p>\n"
            "    <form class=\"singInForm\">\n"
            "\n"
            "\n"
            "    </form>\n"
            "</div>\n"
            "\n"
            "</body>\n"
            "</html>\n"
    };

    std::string updateWorkScheduleMessage(const std::string &newPosition, const std::string &newShiftStart,
                                   const std::string &newShiftFinish, const std::string &newWorkingDays);
}

enum Role {
    UNEMPLOYMENT = 0,
    USER = 1,
    ADMIN = 2,
    TEMP_BAN = 3,
    FOREVER_BAN = 4,
    UNBLOCKED = 5
};

enum UsersType {
    ALL_USERS_NOT_BLOCKED = 0,
    EMPLOYMENT_USERS = 1,
    UNEMPLOYMENT_USERS = 2,
    TEMPORARY_BAN_USERS = 3,
    FOREVER_BAN_USERS = 4
};

