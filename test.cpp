#include <gtest/gtest.h>
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <cstring>
#include <stdio.h>

#include "database.cpp"

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
    row_.SetValue({value_1, value_2, value_3});

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
    vector<DBValue> val_Vec = row_.getValue();
    EXPECT_EQ(val_Vec[0].getIntValue(), 1);
    EXPECT_EQ(val_Vec[1].getStringValue(), "Phan Nhat Minh");
    EXPECT_EQ(val_Vec[2].getIntValue(), 18);
}

TEST(Read_WriteTest, Test_Write_Read_Cycle) {
    ColumnDef column_1("id", DBType :: INT, sizeof(int));
    ColumnDef column_2("name", DBType :: STRING, 30);
    ColumnDefs colDefs;
    colDefs.addColumn(column_1);
    colDefs.addColumn(column_2);
    DBValue val_1(1);
    DBValue val_2("Phan Nhat Minh");
    vector<DBValue> vec_of_val;
    vec_of_val.push_back(val_1);
    vec_of_val.push_back(val_2);

    string name = "TestingDatabase";
    int result = mkdir(name.c_str(), 0777);
    string file_name_1 = "TestingDatabase/TestingTable1-id.bin";
    string file_name_2 = "TestingDatabase/TestingTable1-name.bin";
    FILE* file_1 = fopen(file_name_1.c_str(), "wb+");
    FILE* file_2 = fopen(file_name_2.c_str(), "wb+");
    UserTable table;
    table.setName("TestingTable");
    table.setDatabaseName("TestingDatabase");
    table.setColumnDefs(colDefs);
    table.createRow(vec_of_val);
    Row* row = table.readRow(0);
    int value = row -> getValue()[0].getIntValue();
    string str_value = row -> getValue()[1].getStringValue();
    EXPECT_EQ(value, 1);
    EXPECT_EQ(str_value, "Phan Nhat Minh");
} // map iterator error confirmed

int main2(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}