#include <stdio.h>
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <unordered_map>
#include "database.cpp"

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




int main() {
    //    /usr/bin/clang++ -std=c++17 -pthread -fcolor-diagnostics -fansi-escape-codes -g /Users/nhatminh/NhatMinhDatabase/main.cpp -o /Users/nhatminh/NhatMinhDatabase/main
    int i = 0;
    DatabaseEngine dbEngine;
    dbEngine.getInstance().createDatabase("MySchool16");
    Database db = dbEngine.getInstance().openDatabase("MySchool16");
    ColumnDef column_1("id", DBType :: INT, sizeof(int));
    ColumnDef column_2("name", DBType :: STRING, 30);
    ColumnDefs colDefs;
    colDefs.addColumn(column_1);
    colDefs.addColumn(column_2);
    DBValue val_1(1);
    DBValue val_2("Phan Nhat Minh");
    Row row(colDefs);
    row.setColumnDefs(colDefs);
    row.setValueByColumnName("id", val_1);
    row.setValueByColumnName("name", val_2);
    db.createTable("hocsinh", colDefs);
    UserTable tbl = db.getTableByName("hocsinh");
    tbl.createRow(row);
}



