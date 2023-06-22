#include <gtest/gtest.h>
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <cstring>
#include <stdio.h>

#include "query_exe_plan.cpp"

TEST(RowTest, TestEncodeDecode) {
    // Create a ColumnDefs object, a Table object, a Row object to test the row buffer:
    ColumnDefs columnDefs;
    columnDefs.addColumn(ColumnDef("id", DBType :: INT, sizeof(int)));
    columnDefs.addColumn(ColumnDef("name", DBType :: STRING, 30));
    columnDefs.addColumn(ColumnDef("age", DBType :: INT, sizeof(int)));
    Row row_(columnDefs);
    DBValue value_1(1);
    DBValue value_2("Phan Nhat Minh");
    DBValue value_3(18);
    row_.setValueByColumnName("id", value_1);
    row_.setValueByColumnName("name", value_2);
    row_.setValueByColumnName("age", value_3);

    //Test encode and decode functions
    char* buffer = row_.encode();
    EXPECT_EQ(buffer[0], '\x1');
    EXPECT_EQ(buffer[1], '\0');
    EXPECT_EQ(buffer[2], '\0');
    EXPECT_EQ(buffer[3], '\0');
    EXPECT_EQ(buffer[4], 'P');
    EXPECT_EQ(buffer[5], 'h');
    EXPECT_EQ(buffer[6], 'a');
    EXPECT_EQ(buffer[7], 'n');
    EXPECT_EQ(buffer[8], ' ');
    EXPECT_EQ(buffer[9], 'N');
    EXPECT_EQ(buffer[10], 'h');
    EXPECT_EQ(buffer[11], 'a');
    EXPECT_EQ(buffer[12], 't');
    EXPECT_EQ(buffer[13], ' ');
    EXPECT_EQ(buffer[14], 'M');
    EXPECT_EQ(buffer[15], 'i');
    EXPECT_EQ(buffer[16], 'n');
    EXPECT_EQ(buffer[17], 'h');
    EXPECT_EQ(buffer[18], '\0');
    EXPECT_EQ(buffer[19], '\0');
    EXPECT_EQ(buffer[20], '\0');
    EXPECT_EQ(buffer[33], '\0');
    EXPECT_EQ(buffer[34], '\x12');
    EXPECT_EQ(buffer[35], '\0');
    EXPECT_EQ(buffer[36], '\0');
    EXPECT_EQ(buffer[37], '\0');
    row_.decode(buffer);
    EXPECT_EQ(row_.getValueByColumnName("id").getIntValue(), 1);
    EXPECT_EQ(row_.getValueByColumnName("name").getStringValue(), "Phan Nhat Minh");
    EXPECT_EQ(row_.getValueByColumnName("age").getIntValue(), 18);
}

class StorageEngineTest : public ::testing::Test {
protected:
    void SetUp() override {
        //create a database
        strengine.getInstance().createDatabase("TestingDatabase");

        // Add column definitions to colDefs object
        ColumnDef col_1("Name", DBType :: STRING, 30);
        ColumnDef col_2("Age", DBType :: INT, sizeof(int));
        colDefs.addColumn(col_1);
        colDefs.addColumn(col_2);

        // Set up any necessary test fixtures
        Database db = strengine.openDatabase("TestingDatabase");
        db.createTable("TestingTable", colDefs);

        // initialise the table
        table.setDatabaseName("TestingDatabase");
        table.setName("TestingTable");
        table.setColumnDefs(colDefs);

        //Initialise the row
        row.setColumnDefs(colDefs);
        DBValue value_1("Phan Nhat Minh");
        DBValue value_2(18);
        row.setValueByColumnName("Name", value_1);
        row.setValueByColumnName("Age", value_2);

        //add the row to the table
        table.insertRow(row);
        //table.insertRow(empty_row);
    }

    // Declare any necessary variables and objects
    StorageEngine strengine;
    ColumnDefs colDefs;
    UserTable table = strengine.getTable("TestingDatabase", "TestingTable");
    Row row;
    Row incorrect_row;
    Row retrievedRow = table.readRow(0);
};

//Test case for open database that already exist in the storage engine
TEST_F(StorageEngineTest, openDatabase) {
    Database db = strengine.openDatabase("TestingDatabase");
    EXPECT_EQ(db.getName(), "TestingDatabase");
}

//Test case for opening database that haven't exist in the storage engine
TEST_F(StorageEngineTest, openNotExistingDatabase) {
    EXPECT_THROW(strengine.openDatabase("NotExistingDatabase"), std::runtime_error);
}

//Test case for opening an existing table
TEST_F(StorageEngineTest, getTable) {
    EXPECT_EQ(table.getName(), "TestingTable");
    EXPECT_EQ(table.getRowCount(), 1);
    EXPECT_EQ(table.getColumnDefs().getColumnCount(), 2);
}

//Test case for opening a nonexisting table
TEST_F(StorageEngineTest, getNonExistingTable) {
    EXPECT_THROW(strengine.getTable("NotExistingDatabase", "NotExistingTable"), std::runtime_error);
    EXPECT_THROW(strengine.getTable("TestingDatabase", "NotExistingTable"), std::runtime_error);
    EXPECT_THROW(strengine.getTable("NotExistingDatabase", "TestingTable"), std::runtime_error);
}

// Test case to check if row insertion and reading work correctly
TEST_F(StorageEngineTest, InsertAndReadRow) {
    EXPECT_EQ(table.getRowCount(), 1); // Verify that the row count is incremented
    DBValue retrieved_value_2 = retrievedRow.getValueByColumnName("Age");
    DBValue retrieved_value_1 = retrievedRow.getValueByColumnName("Name");
    EXPECT_EQ(retrieved_value_1.getStringValue(), "Phan Nhat Minh");
    EXPECT_EQ(retrieved_value_2.getIntValue(), 18);
}

//Test case for inserting row without the correct structure
TEST_F(StorageEngineTest, IncorrectInsertion) {
    EXPECT_THROW(table.insertRow(incorrect_row), runtime_error);
}

// Test case to check if the data segment is set correctly
TEST_F(StorageEngineTest, SetDataSegment) {
    Segment segment = table.setDataSegment();
    // Verify that the segment is set up correctly
    // ...
    // Add more assertions if needed
}

TEST_F(StorageEngineTest, mainFunction) {
  //    /usr/bin/clang++ -std=c++17 -pthread -fcolor-diagnostics -fansi-escape-codes -g /Users/nhatminh/NhatMinhDatabase/main.cpp -o /Users/nhatminh/NhatMinhDatabase/main
    int i = 0;
    StorageEngine Engine;
    Engine.getInstance().createDatabase("MySchool");
    Database db = Engine.getInstance().openDatabase("MySchool");
    ColumnDef column_1("id", DBType :: INT, sizeof(int));
    ColumnDef column_2("name", DBType :: STRING, 30);
    ColumnDefs colDefs;
    colDefs.addColumn(column_1);
    colDefs.addColumn(column_2);
    DBValue val_1(1);
    DBValue val_2("Phan Nhat Minh");
    Row row(colDefs);
    DBValue val_3(2);
    DBValue val_4("Pham Duy Long");
    Row row_1(colDefs);
    row_1.setColumnDefs(colDefs);
    row_1.setValueByColumnName("id", val_3);
    row_1.setValueByColumnName("name", val_4);
    row.setColumnDefs(colDefs);
    row.setValueByColumnName("id", val_1);
    row.setValueByColumnName("name", val_2);
    db.createTable("hocsinh", colDefs);
    UserTable tbl = db.getTableByName("hocsinh");
    Engine.InsertRow("MySchool", "hocsinh", row);
    Engine.InsertRow("MySchool", "hocsinh", row_1); 
    //tbl.insertRow(row); //Update Row_count by table
    //tbl.insertRow(row_1);
}

class Query_ExecutionPlanTest : public ::testing::Test {
public:
    QEPComponent plan;
    StorageEngine Engine;
};

TEST_F(Query_ExecutionPlanTest, FROM_Test) {
    ResultSet result = plan.execute_FROM_clause("MySchool", "hocsinh");
    UserTable table = Engine.getTable("MySchool", "hocsinh");
    EXPECT_EQ(result.getName(), "hocsinh");
    EXPECT_EQ(result.getRowCount(), table.getRowCount());
    EXPECT_EQ(result.readRow(0).getValueByColumnName("id").getIntValue(), 1);
    EXPECT_EQ(result.readRow(1).getValueByColumnName("id").getIntValue(), 2);
    EXPECT_EQ(result.readRow(0).getValueByColumnName("name").getStringValue(), "Phan Nhat Minh");
    EXPECT_EQ(result.readRow(1).getValueByColumnName("name").getStringValue(), "Pham Duy Long");
}

TEST_F(Query_ExecutionPlanTest, WHERE_Test) {
    Constant<int> c;
    c.setConstantValue(1);
    ResultSet result = plan.execute_WHERE_clause(plan.execute_FROM_clause("MySchool", "hocsinh"), "id", c);
    EXPECT_EQ(result.readRow(0).getValueByColumnName("name").getStringValue(), "Pham Duy Long");
}

TEST_F(Query_ExecutionPlanTest, SELECT_Test) {
    ResultSet result = plan.execute_SELECT_clause(plan.execute_FROM_clause("MySchool", "hocsinh"), "name, id");
}
