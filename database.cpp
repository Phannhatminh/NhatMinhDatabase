#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <cstdlib>
#include "column_defs.cpp"
#include <filesystem>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>

using namespace std;

class Database {
public:
    Database() {
       //Set up Default tables: 
    }

    //Using FILE* to create column's files
    void createTable(string table_name, ColumnDefs colDefs_Of_Table) {
        bool exist = true;
        int size = colDefs_Of_Table.columnsName.size();
        for (int i = 0; i < size; i++) {
            //set up file located in dir
            string filename = nameOfDatabase + "/" + table_name + "-" + colDefs_Of_Table.columnsName[i] + ".bin";

            //Check if the table is already exist
            FILE* file = std::fopen(filename.c_str(), "rb+");
            //create the table's files in database's directory (name: MySchool/hocsinh-id.bin)
            if (file == NULL) {
                FILE* outFile = std::fopen(filename.c_str(), "wb+");

                //Check if file is created successfully
                if (outFile != nullptr) {
                    std::cout << "Column " << filename << " created successfully in directory " << nameOfDatabase << std::endl;
                    exist = exist && true;
                }
                else {
                    std::cerr << "Failed to create column " << filename << " of Table " << table_name << " in database " << nameOfDatabase << std::endl;
                    exist = exist && false;
                }
            }
            else {
                fclose(file);
                cout << "Table " << table_name << " has already exist in database " << nameOfDatabase << endl;
            }
        }

        // Check the exist value
        if (exist) {
            std::cout << "Table " << table_name << " created successfully in database " << nameOfDatabase << std::endl;

            // Update the Sys.Tables
            UpdateSystemTables(table_name, colDefs_Of_Table);
        }  
        else {
            std::cerr << "Database " << nameOfDatabase << " does not exist." << std::endl;
        }
    }

    //method to get a table in a database
    UserTable getTableByName(string table_name) {
        //Scan the sysTableOfTables for name
        vector<string> nameList;
        vector<DBType::Type> typeList;
        vector<int> widthList;
        int row_count;
        int sys_table_row_count = getSysTables().readRow(0).getValueByColumnName("Row_Count").getIntValue();
        int sys_column_row_count = getSysTables().readRow(1).getValueByColumnName("Row_Count").getIntValue();
        for (int i = 0; i < sys_table_row_count; i++) {
            if (getSysTables().readRow(i).getValueByColumnName("Table_Name").getStringValue() == table_name) {
                row_count = getSysTables().readRow(i).getValueByColumnName("Row_Count").getIntValue();
                break;
            }
        }
        //Scan the sysTableOfColumns
        for (int i = 0; i < sys_column_row_count; i++) {
            if (getSysColumns().readRow(i).getValueByColumnName("Column_Table_Name").getStringValue() == table_name) {
                nameList.push_back(getSysColumns().readRow(i).getValueByColumnName("Column_Name").getStringValue());
                if (getSysColumns().readRow(i).getValueByColumnName("Column_Data_Type").getStringValue() == "int") {
                    DBType::Type type = DBType::INT;
                    typeList.push_back(type);
                    widthList.push_back(sizeof(int));
                }
                if (getSysColumns().readRow(i).getValueByColumnName("Column_Data_Type").getStringValue() == "float") {
                    DBType::Type type = DBType::FLOAT;
                    typeList.push_back(type);
                    widthList.push_back(sizeof(float));
                }
                if (getSysColumns().readRow(i).getValueByColumnName("Column_Data_Type").getStringValue() == "string") {
                    DBType::Type type = DBType::STRING;
                    typeList.push_back(type);
                    widthList.push_back(30);
                }
            }      
        }
        //return metadata
        ColumnDefs columnDefs;
        for (int i = 0; i < sys_column_row_count; i++) {
            ColumnDef column_def(nameList[i], typeList[i], widthList[i]);
            columnDefs.addColumn(column_def);
        }

        //build table
        UserTable table(columnDefs);
        //set Database for table
        table.setDatabaseName(nameOfDatabase);
        //Set name for table
        table.setName(table_name);
        //Set ColumnDefs for table
        table.setColumnDefs(columnDefs);
        //get the table
        return table;
    }

    void eraseTableByName(string table_name) {}

    void joinTable(UserTable table1, UserTable table2) {
         // can primary key va forein key 
    }

    //method to build system tables
    void BuildSystemTables() {
        BuildSysTableOfTables();
        BuildSysTableOfColumns();
    }

    void setName(string name) {
        nameOfDatabase = name;
    }

    string getName() {
        return nameOfDatabase;
    }

private:
    string nameOfDatabase;
    ColumnDefs columnDefsTables;
    ColumnDefs columnDefsColumns;

    void BuildSysTableOfTables() {
        //Set up ColumnDefs objects
        ColumnDef column_1("Table_Name", DBType :: STRING, 30);
        ColumnDef column_2("Row_Count", DBType :: INT, sizeof(int));
        ColumnDefs column_defs;
        column_defs.addColumn(column_1);
        column_defs.addColumn(column_2);

        // Set up bool value of existence
        bool exist = true;
        int size = column_defs.columnsName.size();
        for (int i = 0; i < size; i++) {
            //set up file for SysTables's columns value
            string filename = nameOfDatabase + "/Sys_Tables-" + column_defs.columnsName[i] + ".bin";

            //Check if the table is already exist
            FILE* file = std::fopen(filename.c_str(), "rb+");
            if (file == NULL) {
                //create the SysTable's files in database's directory (name: MySchool/SysTables-TableName.bin)
                FILE* outFile = std::fopen(filename.c_str(), "wb+");

                //Check if file is created successfully
                if (outFile != nullptr) {
                    std::cout << "Column " << filename << " created successfully in directory " << nameOfDatabase << std::endl;
                    exist = exist && true;
                }
                else {
                    std::cerr << "Failed to create column " << column_defs.columnsName[i] << " of SysTables in database " << nameOfDatabase << std::endl;
                    exist = exist && false;
                }
                fclose(outFile);
            }
            else {
                cout << "Sys_Tables " << " has already exist in database " << nameOfDatabase << endl;
            }
        }
        //Check if the SysTables is created successfully
        if (exist == true) {
            //Initialise the SystemTables
            SystemTable sys_Tables(column_defs);
            sys_Tables.setDatabaseName(nameOfDatabase);
            sys_Tables.setName("Sys_Tables");
            sys_Tables.setColumnDefs(column_defs);
            sys_Tables.setRowCount(2);

            DBValue table_name_value_1("Sys_Tables");
            DBValue row_count_value_1(2);
            Row row_1;
            row_1.setColumnDefs(sys_Tables.getColumnDefs());
            row_1.setValueByColumnName("Table_Name", table_name_value_1);
            row_1.setValueByColumnName("Row_Count", row_count_value_1);
            sys_Tables.updateRow(row_1, 0);
        
            DBValue table_name_value_2("Sys_Columns");
            DBValue row_count_value_2(6);
            Row row_2;
            row_2.setColumnDefs(sys_Tables.getColumnDefs());
            row_2.setValueByColumnName("Table_Name", table_name_value_2);
            row_2.setValueByColumnName("Row_Count", row_count_value_2);
            sys_Tables.updateRow(row_2, 1);
        }

        //print out success message
        cout << "Sys_Tables has been created successfully" << endl;
    }

    //Same for SysTableOfColumns
    void BuildSysTableOfColumns() {
        bool exist = true;
        ColumnDef column_1("Column_Name", DBType :: STRING, 30);
        ColumnDef column_2("Column_Table_Name", DBType :: STRING, 30);
        ColumnDef column_3("Column_Data_Type", DBType :: STRING, 30);
        ColumnDef column_4("Order", DBType :: INT, sizeof(int));
        ColumnDefs column_defs;
        column_defs.addColumn(column_1);
        column_defs.addColumn(column_2);
        column_defs.addColumn(column_3);
        column_defs.addColumn(column_4);
        int size = column_defs.columnsName.size();
        for (int i = 0; i < size; i++) {
            //set up file for SysTables's columns value
            string filename = nameOfDatabase + "/Sys_Columns-" + column_defs.columnsName[i] + ".bin";

            //Check if the table is already exist
            FILE* file = std::fopen(filename.c_str(), "rb+");
            if (file == NULL) {
                //create the SysTable's files in database's directory (name: MySchool/SysTables-TableName.bin)
                FILE* outFile = std::fopen(filename.c_str(), "wb+");

                //Check if file is created successfully
                if (outFile != nullptr) {
                    std::cout << "Column " << filename << " created successfully in directory " << nameOfDatabase << std::endl;
                    exist = exist && true;
                }
                else {
                    std::cerr << "Failed to create column " << column_defs.columnsName[i] << " of SysColumns in database " << nameOfDatabase << std::endl;
                    exist = exist && false;
                }
            }
            else {
                cout << "Sys_Columns " << " has already exist in database " << nameOfDatabase << endl;
            }
        }

        //Check if the SysTables is created successfully
        if (exist == true) {
            //Initialise the SystemTables
            SystemTable sys_Columns(column_defs);

            sys_Columns.setDatabaseName(nameOfDatabase);
            sys_Columns.setName("Sys_Columns");
            sys_Columns.setColumnDefs(column_defs);
            sys_Columns.setRowCount(6);

            DBValue table_name_value_1("Table_Name");
            DBValue col_Tbl_name_1("Sys_Tables");
            DBValue col_Type_1("string");
            DBValue order_1(0);
            Row row_1;
            row_1.setColumnDefs(sys_Columns.getColumnDefs());
            row_1.setValueByColumnName("Column_Name", table_name_value_1);
            row_1.setValueByColumnName("Column_Table_Name", col_Tbl_name_1);
            row_1.setValueByColumnName("Column_Data_Type", col_Type_1);
            row_1.setValueByColumnName("Order", order_1);
            sys_Columns.updateRow(row_1, 0);
        
            DBValue table_name_value_2("Row_Count");
            DBValue col_Tbl_name_2("Sys_Tables");
            DBValue col_Type_2("int");
            DBValue order_2(1);
            Row row_2;
            row_2.setColumnDefs(sys_Columns.getColumnDefs());
            row_2.setValueByColumnName("Column_Name", table_name_value_2);
            row_2.setValueByColumnName("Column_Table_Name", col_Tbl_name_2);
            row_2.setValueByColumnName("Column_Data_Type", col_Type_2);
            row_2.setValueByColumnName("Order", order_2);
            sys_Columns.updateRow(row_2, 1);

            DBValue table_name_value_3("Column_Name");
            DBValue col_Tbl_name_3("Sys_Columns");
            DBValue col_Type_3("string");
            DBValue order_3(0);
            Row row_3;
            row_3.setColumnDefs(sys_Columns.getColumnDefs());
            row_3.setValueByColumnName("Column_Name", table_name_value_3);
            row_3.setValueByColumnName("Column_Table_Name", col_Tbl_name_3);
            row_3.setValueByColumnName("Column_Data_Type", col_Type_3);
            row_3.setValueByColumnName("Order", order_3);
            sys_Columns.updateRow(row_3, 2);

            DBValue table_name_value_4("Column_Table_Name");
            DBValue col_Tbl_name_4("Sys_Columns");
            DBValue col_Type_4("string");
            DBValue order_4(1);
            Row row_4;
            row_4.setColumnDefs(sys_Columns.getColumnDefs());
            row_4.setValueByColumnName("Column_Name", table_name_value_4);
            row_4.setValueByColumnName("Column_Table_Name", col_Tbl_name_4);
            row_4.setValueByColumnName("Column_Data_Type", col_Type_4);
            row_4.setValueByColumnName("Order", order_4);
            sys_Columns.updateRow(row_4, 3);

            DBValue table_name_value_5("Column_Data_Type");
            DBValue col_Tbl_name_5("Sys_Columns");
            DBValue col_Type_5("string");
            DBValue order_5(2);
            Row row_5;
            row_5.setColumnDefs(sys_Columns.getColumnDefs());
            row_5.setValueByColumnName("Column_Name", table_name_value_5);
            row_5.setValueByColumnName("Column_Table_Name", col_Tbl_name_5);
            row_5.setValueByColumnName("Column_Data_Type", col_Type_5);
            row_5.setValueByColumnName("Order", order_5);
            sys_Columns.updateRow(row_5, 4);

            DBValue table_name_value_6("Order");
            DBValue col_Tbl_name_6("Sys_Columns");
            DBValue col_Type_6("string");
            DBValue order_6(3);
            Row row_6;
            row_6.setColumnDefs(sys_Columns.getColumnDefs());
            row_6.setValueByColumnName("Column_Name", table_name_value_6);
            row_6.setValueByColumnName("Column_Table_Name", col_Tbl_name_6);
            row_6.setValueByColumnName("Column_Data_Type", col_Type_6);
            row_6.setValueByColumnName("Order", order_1);
            sys_Columns.updateRow(row_6, 5);

            //Print out the success message
            cout << "SysColumns has been created successfully" << endl;
        }
    }

    //method to access to system tables
    SystemTable getSysTables() {
        //Init Sys_Tables
        ColumnDef column_1("Table_Name", DBType :: STRING, 30);
        ColumnDef column_2("Row_Count", DBType :: INT, sizeof(int));
        ColumnDefs column_defs;
        column_defs.addColumn(column_1);
        column_defs.addColumn(column_2);
        SystemTable sys_Tables(column_defs);

        //Set up Sys_Tables
        sys_Tables.setDatabaseName(nameOfDatabase);
        sys_Tables.setName("Sys_Tables");
        sys_Tables.setColumnDefs(column_defs);
        Row row = sys_Tables.readRow(0);
        int row_count = row.getValueByColumnName("Row_Count").getIntValue();
        sys_Tables.setRowCount(row_count);
        sys_Tables.setDataSegment();
        return sys_Tables;
    }

    //method to access to system columns
    SystemTable getSysColumns() {
        //Init Sys_Columns
        ColumnDef column_1("Column_Name", DBType :: STRING, 30);
        ColumnDef column_2("Column_Table_Name", DBType :: STRING, 30);
        ColumnDef column_3("Column_Data_Type", DBType :: STRING, 30);
        ColumnDef column_4("Order", DBType :: INT, sizeof(int));
        ColumnDefs column_defs;
        column_defs.addColumn(column_1);
        column_defs.addColumn(column_2);
        column_defs.addColumn(column_3);
        column_defs.addColumn(column_4);
        SystemTable sys_Columns(column_defs);

        //Set up Sys_Columns
        sys_Columns.setDatabaseName(nameOfDatabase);
        sys_Columns.setName("Sys_Columns");
        sys_Columns.setColumnDefs(column_defs);
        int row_count = getSysTables().readRow(1).getValueByColumnName("Row_Count").getIntValue();
        sys_Columns.setRowCount(row_count);
        sys_Columns.setDataSegment();
        return sys_Columns;
    }

    //Update the system Tables after create or update an User Table
    void UpdateSystemTables(string table_name, ColumnDefs colDefs_Of_Table) {
        //Specify the column's size
        int column_size = colDefs_Of_Table.columnsName.size();

        //Set up and prepare system tables instances:
        //Set up sysTables:
        SystemTable sys_table = getSysTables();
        sys_table.setDataSegment();
        Row row_for_sys_table = sys_table.readRow(0);
        int sys_table_row_count = row_for_sys_table.getValueByColumnName("Row_Count").getIntValue();
        sys_table.setRowCount(sys_table_row_count);
        
        //Set up sysColumns:
        SystemTable sys_columns = getSysColumns();
        Row row_for_sys_columns = getSysTables().readRow(1);
        int sys_columns_row_count = row_for_sys_columns.getValueByColumnName("Row_Count").getIntValue();

        //Update the Sys_Tables
        DBValue val_1(table_name);
        val_1.setString(table_name);
        DBValue val_2(0);
        val_2.setInt(0);
        Row row_1;
        row_1.setColumnDefs(sys_table.getColumnDefs());
        row_1.setValueByColumnName("Table_Name", val_1);
        row_1.setValueByColumnName("Row_Count", val_2);
        sys_table.createRow(row_1);

        //Update the row_count of Sys_Tables
        DBValue init_values_ = row_for_sys_table.getValueByColumnName("Row_Count");
        DBValue value_1("Sys_Tables");
        DBValue value_2(init_values_.getIntValue() + 1);
        Row changed_values_;
        changed_values_.setColumnDefs(getSysTables().getColumnDefs());
        changed_values_.setValueByColumnName("Table_Name", value_1);
        changed_values_.setValueByColumnName("Row_Count", value_2);
        getSysTables().updateRow(changed_values_, 0);

        // Update the Sys_Columns
        //get all the column name and put them into a vector
        vector<string> columnNameList;
        for (int i = 0; i < column_size; i++) {
            columnNameList.push_back(colDefs_Of_Table.columnsName[i]);
        }
        //create a vector of string that represents fields' types
        vector<string> columnTypeList;
        for (int i = 0; i < column_size; i++) {
            if (colDefs_Of_Table.columns_[colDefs_Of_Table.columnsName[i]].getType() == DBType :: INT) {
                columnTypeList.push_back("int");
            }
            if (colDefs_Of_Table.columns_[colDefs_Of_Table.columnsName[i]].getType() == DBType :: FLOAT) {
                columnTypeList.push_back("float");
            }
            if (colDefs_Of_Table.columns_[colDefs_Of_Table.columnsName[i]].getType() == DBType :: STRING) {
                columnTypeList.push_back("string");
            }
        }

        //Update the row_count of Sys_Columns
        Row row_2 = getSysTables().readRow(1);
        DBValue init_values_1 = row_for_sys_columns.getValueByColumnName("Row_Count");
        DBValue value_3("Sys_Columns");
        DBValue value_4(init_values_1.getIntValue() + colDefs_Of_Table.getColumnCount());
        Row changed_values_1;
        changed_values_1.setValueByColumnName("Table_Name", value_3);
        changed_values_1.setValueByColumnName("Row_Count", value_4);
        getSysColumns().updateRow(changed_values_1, 1);

        //create DBValues and write records into Sys_Columns
        int columnCount = colDefs_Of_Table.getColumnCount();
        for (int i = 0; i < columnCount; i++) {
            sys_columns.setRowCount(sys_columns_row_count);
            DBValue val_5(columnNameList[i]);
            DBValue val_6(table_name);
            DBValue val_7(columnTypeList[i]);
            DBValue val_8(i);
            val_5.setString(columnNameList[i]);
            val_6.setString(table_name);
            val_7.setString(columnTypeList[i]);
            val_8.setInt(i);
            Row row;
            row.setColumnDefs(getSysColumns().getColumnDefs());
            row.setValueByColumnName("Columns_Name", val_5);
            row.setValueByColumnName("Columns_Table_Name", val_6);
            row.setValueByColumnName("Columns_Data_Type", val_7);
            row.setValueByColumnName("Order", val_8);
            sys_columns.createRow(row);
            sys_columns_row_count += 1;
        }
    }
};

class DatabaseEngine {
public:
    DatabaseEngine() {}
    ~DatabaseEngine() {
        // Cleanup the database engine...
    }
    static DatabaseEngine& getInstance() {
        static DatabaseEngine instance;
        return instance;
    }

    //create Database by its name using mkdir
    void createDatabase(string name) {
        //Check the existence of the database of same name in the system
        if (DatabaseExist(name)) {
            cout << "Database " << name << " has already existed." << endl;
        }
        else {
            //Create a directory for the database
            int result = mkdir(name.c_str(), 0777);

            if (result == 0) {
                //Initialise the database object of class Database to build the sysTables
                MyCurrentDatabase.setName(name);
                MyCurrentDatabase.BuildSystemTables();

                //Print the message to the console
                cout << "Database " << name << " is created successfully" << endl;
            }
            else {
                cout << "Failed to create database " << name << endl;
            }
        }
    }

    Database openDatabase(string name) {
        if (DatabaseExist(name)) {
            //Assign the path to the directory to the database's path
            MyCurrentDatabase.setName(name);

            //return the database
            return MyCurrentDatabase;
        }
        else {
            cout << "Database " << name << " does not exist" << endl;
            return MyCurrentDatabase;
        }
    }
private:
    
    Database MyCurrentDatabase;
    // Disable copy and move constructors
    DatabaseEngine(const DatabaseEngine&) = delete;
    DatabaseEngine& operator=(const DatabaseEngine&) = delete;
    DatabaseEngine(DatabaseEngine&&) = delete;
    DatabaseEngine& operator=(DatabaseEngine&&) = delete;

    //function to check the existence of database in the system
    bool DatabaseExist(string database_name) {
        DIR* dir = opendir(database_name.c_str());
        if (dir) {
            return true;
        }
        else {
            return false;
        }
    }
};