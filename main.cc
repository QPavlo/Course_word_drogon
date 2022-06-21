#include <drogon/drogon.h>

using namespace drogon::orm;

int main() {
    drogon::app().loadConfigFile("../config.json");
    drogon::app().run();
    return 0;
}
