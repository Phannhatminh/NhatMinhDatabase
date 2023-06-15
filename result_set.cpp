#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <cstring>
#include <stdio.h>
#include "database.cpp"

using namespace std;

class ResultSet {
public:
    string getName() {
        return result_set_name;
    }

    void setName(string name) {
        result_set_name = name;
    }

    void setColumnDefs(ColumnDefs columndefs) {
        column_defs = columndefs;
    }

    ColumnDefs getColumnDefs() {
        return column_defs;
    }

    int getRowCount() {
        return rows.size();
    }

    void setRowCount(int count) {
        row_count = count;
    }

    void insertRowIntoIndex(Row row, int index) {
        rows[index] = row;
    }

    void insertRow(Row row) {
        if (row.getColumnDefs().isDifferentColumnDefs(column_defs)) {
            throw std::runtime_error("Row's structure doesn't fit result set " + result_set_name + "'s structure");
        }
        else {
            rows[row_count] = row;
            row_count += 1;
        }
    }

    Row readRow(int index) {
        if (index < 0 || index >= row_count) {
            throw runtime_error("invalid access to result_set");
        }
        else {
            return rows[index];
        }
    }

private:
    int row_count;
    string result_set_name;
    ColumnDefs column_defs;
    map<int, Row> rows;
};