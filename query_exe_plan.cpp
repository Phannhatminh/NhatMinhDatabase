#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "result_set.cpp"

using namespace std;

template <typename T>
class Expression {
public:
    virtual T evaluate() const = 0;
};

template <typename U>
class Constant : public Expression<U> {
public:
    U evaluate() const override {
        return value;
    }

    void setConstantValue(int const_value) {
        value = const_value;
    }
private:
    U value;
};

template <typename V>
class Variable : public Expression<V> {
public:
    V evaluate() const override {
        return value;
    }
private:
    V value;
};

template <int>
class IntVariable : public Variable<int> {
public:
    int evaluate() const override {
        return value;
    }

    void setValue(int var) {
        value = var;
    }
private:
    int value;
};

template <typename W>
class CompareOperator : public Expression<bool> {
public:
    virtual bool compare(W left, W right) const = 0;
    bool evaluate() {
        W leftValue = getLeftOperand();
        W rightValue = getRightOperand();
        return compare(leftValue, rightValue);
    }

    virtual W getLeftOperand() const = 0;
    virtual W getRightOperand() const = 0;
};

template <typename W>
class IsGreaterThanINT : public Expression<bool> {
public:
    bool evaluate() const override {
        if (int_variable.evaluate() > constant.evaluate()) {
            return true;
        }
        else {
            return false;
        }
    }

    void setVariable(int var) {
        int_variable.setValue(var);
    }

    void setConstant(Constant<int> cons) {
        constant = cons;
    }
private:
    IntVariable<0> int_variable;
    Constant<int> constant;
};

template <typename T>
class Context {
private:
    Row* current_row;
public:
    void SetContext(Row row) {
        current_row = &row;
    }

    T evaluateExpression(Expression<T> * expression) {
        return expression -> evaluate();
    }
};

class QueryExecutionPlan {
private:
    StorageEngine engine;
public:
    ResultSet execute_FROM_clause(string database_name, string table_name) {
        ResultSet rslt_set;
        UserTable table =  engine.getTable(database_name, table_name);
        ColumnDefs col_defs = table.getColumnDefs();
        rslt_set.setName(table_name);
        rslt_set.setColumnDefs(col_defs);
        int row_count = table.getRowCount();
        for (int i = 0; i < row_count; i++) {
            Row current_row = engine.ReadRow(database_name, table_name, i);
            rslt_set.insertRowIntoIndex(current_row, i);
        }
        return rslt_set;
    }

    ResultSet execute_SELECT_clause(ResultSet result_set, string column_list) {
        int size = result_set.getRowCount();
        string column_name;
        vector<string> columnList;
        stringstream ss(column_list);
        string token;
        ResultSet reslt_set;
        reslt_set.setName(result_set.getName());
        //get the columnDefs
        ColumnDefs col_defs = result_set.getColumnDefs();
        // Build a new columnDefs
        ColumnDefs newColumnDefs;
        while (getline(ss, token, ',')) {
            // Remove leading/trailing whitespaces from each token
            size_t startPos = token.find_first_not_of(" ");
            size_t endPos = token.find_last_not_of(" ");
            token = token.substr(startPos, endPos - startPos + 1);
            ColumnDef col_def = result_set.getColumnDefs().getColumnDef(token);
            newColumnDefs.addColumn(col_def);
            columnList.push_back(token);
        }
        //Set the row count
        reslt_set.setRowCount(size);
        //Set new columnDefs into reslt_set (có sẵn function setColumnDefs)
        reslt_set.setColumnDefs(newColumnDefs);
        for (int index = 0; index < size; index++) {
            //Build new row from the new ColumnDefs
            Row current_row;
            current_row.setColumnDefs(newColumnDefs);
            for (int i = 0; i < columnList.size(); i++) {
                current_row.setValueByColumnName(columnList[i], result_set.readRow(index).getValueByColumnName(columnList[i]));
            }
            //Insert new row to new result_set
            reslt_set.insertRowIntoIndex(current_row, index);
        }
        return reslt_set;
    }

    //the challenging part
    ResultSet execute_WHERE_clause(ResultSet result_set, string columnName, Constant<int> constant) {
        IsGreaterThanINT<bool> bool_ex; 
        bool_ex.setConstant(constant);
        ResultSet new_result_set;
        int new_row_count = 0;
        new_result_set.setRowCount(0);
        new_result_set.setColumnDefs(result_set.getColumnDefs());
        new_result_set.setName(result_set.getName());
        int row_count = result_set.getRowCount();
        Context<bool> context;
        for (int i = 0; i < row_count; i++) {
            Row curr_row = result_set.readRow(i);
            bool_ex.setVariable(curr_row.getValueByColumnName(columnName).getIntValue());
            context.SetContext(curr_row);
            if(context.evaluateExpression(&bool_ex) == true) {
                new_result_set.insertRow(curr_row);
                new_row_count += 1;
            }
        }
        new_result_set.setRowCount(new_row_count);
        return new_result_set;
    }

    void execute_INSERT_INTO_clause(string database_name, string table_name, string column_list, string value_list) {
        //declare essential objects for the function
        stringstream ss(column_list);
        stringstream ssv(value_list);
        vector<string> columnList;
        vector<string> valueList;
        string token;
        string token_1;
        Row row;
        UserTable table = engine.getTable(database_name, table_name);
        ColumnDefs column_defs_for_table = table.getColumnDefs();

        //process string parameters
        //process the column_list
        while (getline(ss, token, ',')) {
            // Remove leading/trailing whitespaces from each token
            size_t startPos = token.find_first_not_of(" ");
            size_t endPos = token.find_last_not_of(" ");
            token = token.substr(startPos, endPos - startPos + 1);

            columnList.push_back(token);
        }

        //process the value_list
        while (getline(ssv, token_1, ',')) {
            // Remove leading/trailing whitespaces from each token
            size_t startPos = token_1.find_first_not_of(" ");
            size_t endPos = token_1.find_last_not_of(" ");
            token_1 = token_1.substr(startPos, endPos - startPos + 1);

            valueList.push_back(token_1);
        }

        //Initialise the row by two string list
        for (int i = 0; i < columnList.size(); i++) {
            DBValue value;
            //Handle different type of value
            if (column_defs_for_table.getColumnDef(columnList[i]).getType() == DBType :: INT) {
                //Convert string to int
                int val = stoi(valueList[i]);
                //Set type for value
                value.setType(DBType::INT);
                //set int value for value
                value.setInt(val);
            }
            if (column_defs_for_table.getColumnDef(columnList[i]).getType() == DBType :: FLOAT) {
                //convert string to float
                float val = stof(valueList[i]);
                //set type float for value
                value.setType(DBType::FLOAT);
                //set float value to value
                value.setFloat(val);
            }
            if (column_defs_for_table.getColumnDef(columnList[i]).getType() == DBType :: STRING) {
                string val = valueList[i];
                //set type
                value.setType(DBType::STRING);
                //set string value
                value.setString(val);
            }
            //set the value into the row
            row.setValueByColumnName(columnList[i], value);
        }

        //insert row into table
        engine.InsertRow(database_name, table_name, row);
    }

    void execute_CREATE_TABLE_clause(string database_name, string table_name, string column_list, string type_list) {
        string column_name;
        stringstream ss(column_list);
        stringstream ss_1(type_list);
        vector<string> columnList;
        vector<string> typeList;
        string token;
        ColumnDefs newColumnDefs;

        //Build the columndefs
        while (getline(ss, token, ',')) {
            // Remove leading/trailing whitespaces from each token
            size_t startPos = token.find_first_not_of(" ");
            size_t endPos = token.find_last_not_of(" ");
            token = token.substr(startPos, endPos - startPos + 1);
            columnList.push_back(token);
        }

        //Build a list of type name
        while (getline(ss_1, token, ',')) {
            // Remove leading/trailing whitespaces from each token
            size_t startPos = token.find_first_not_of(" ");
            size_t endPos = token.find_last_not_of(" ");
            token = token.substr(startPos, endPos - startPos + 1);
            typeList.push_back(token);
        }

        //set up the columnDefs
        for (int i = 0; i < columnList.size(); i++) {
            ColumnDef col_def;
            if (typeList[i] == "INT") {
                col_def.setName(columnList[i]);
                col_def.setType(DBType::INT);
                col_def.setWidth(sizeof(int));
            }
            if (typeList[i] == "FLOAT") {
                col_def.setName(columnList[i]);
                col_def.setType(DBType::FLOAT);
                col_def.setWidth(sizeof(float));
            }
            if (typeList[i] == "STRING") {
                col_def.setName(columnList[i]);
                col_def.setType(DBType::STRING);
                col_def.setWidth(30);
            }
            else {
                throw runtime_error("invalid column definitions");
            }
            newColumnDefs.addColumn(col_def);
        }

        //Create Table
        engine.createTable(database_name, table_name, newColumnDefs);
    }

    void execute_CREATE_DATABASE_clause(string database_name) {
        engine.createDatabase(database_name);
    }
};