#include <stdio.h>
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <unordered_map>
#include "query_exe_plan.cpp"

using namespace std;

class Setting {
    public:
    static Setting& getInstance() {
        static Setting instance;
        return instance;
    }

    std::map<std::string, std::string> settings;

    void setValue(const std::string& key, const std::string& value) {
        settings[key] = value;
    }

    std::string getValue(const std::string& key) const {
        return settings.at(key);
    }


    private:
    Setting() {}
    Setting(const Setting&) = delete;
    Setting& operator=(const Setting&) = delete;

};

class Application {
public:
    static Application& getInstance() {
        static Application instance;
        return instance;
    }

    void initialize(int argc, char* argv[]) {
        Setting::getInstance().setValue("query", argv[2]);
        Setting::getInstance().setValue("data", argv[4]);
    }

    void run() {
        //Implementation of the application
    }

private:
    Application() {
        //Constructor logic
    };

    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;
};

int main(int argc, char **argv) {
    /*StorageEngine strengine;
    strengine.getInstance().createDatabase("TestingDatabase");
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();*/
    return 0;
}



