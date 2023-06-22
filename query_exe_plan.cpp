#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "result_set.cpp"

using namespace std;

class Expression {
public:
    virtual Expression* evaluate() = 0;

    virtual string getType() = 0;

    virtual void setType(string type_name) {
        Type = type_name;
    }
private:
    string Type;
    Expression* expr;
};

class AlgebraicExpression : public Expression {
public:
private:

};

class Constant : public Expression {
public:
    Expression* evaluate() override {
        return this;
    }
};

class IntConstant : public Constant {
public:
    void setConstantValue(int value) {
        constant = value;
    }

    int getConstantValue() {
        return constant;
    }
private:
    int constant;
};

class FloatConstant : public Constant {
public:
    void setConstantValue(float value) {
        constant = value;
    }

    float getConstantValue() {
        return constant;
    }
private:
    float constant;
};

class StringConstant : public Constant {
public:
    void setConstantValue(string value) {
        constant = value;
    }

    string getConstantValue() {
        return constant;
    }
private:
    string constant;
};

class LogicalConstant : public Constant {
public:
    void setConstantValue(bool value) {
        constant = value;
    }

    bool getConstantValue() {
        return constant;
    }
private:
    bool constant;
};

class Variable : public Expression {
public:
    Expression* evaluate() override {
        return const_value;
    }
    string getType() override {
        return "variable";
    }
private:
    Constant* const_value;
};

class IntVariable : public Variable {
public:
    IntConstant* evaluate() {
        return int_const_value;
    }

    void setVariableValue(int value) {
        int_const_value -> setConstantValue(value);
    }
    string getType() override {
        return "int";
    }
private:
    IntConstant* int_const_value;
};

class FloatVariable : public Variable {
public:
    Expression* evaluate() {
        return float_const_value;
    }

    void setVariableValue(float value) {
        float_const_value -> setConstantValue(value);
    }
private:
    FloatConstant* float_const_value;
};

class StringVariable : public Variable {
public:
    Expression* evaluate() {
        return string_const_value;
    }

    void setVariableValue(string value) {
        string_const_value -> setConstantValue(value);
    }
private:
    StringConstant* string_const_value;
};

class LogicalVariable : public Variable {
public:
    Expression* evaluate() {
        return logic_const_value;
    }

    void setVariableValue(bool value) {
        logic_const_value -> setConstantValue(value);
    }
private:
    LogicalConstant* logic_const_value;
};

class BinaryOperator : public Expression {
public:
    Expression* left_hand_side;
    Expression* right_hand_side;
    void set_up(Expression* left, Expression* right) {
        left_hand_side = left;
        right_hand_side = right;
    }
private:
};

class IsGreaterThanINT : public Expression {
public:
    LogicalConstant* evaluate() override {
        if (int_var -> evaluate() -> getConstantValue() > int_constant -> getConstantValue()) {
            logic_constant -> setConstantValue(true);
        }
        else {
            logic_constant -> setConstantValue(false);
        }
        return logic_constant;
    }

    string getType() override {
        return "logic";
    }

    void setConstant(IntConstant* constant) {
        int_constant = constant;
    }

    void setVariable(IntVariable* var) {
        int_var = var;
    }
private:
    IntVariable* int_var;
    IntConstant* int_constant;
    LogicalConstant* logic_constant;
};

class Context {
private:
    Row* current_row;
public:
    void SetContext(Row row) {
        current_row = &row;
    }

    Expression* evaluateExpression(Expression * expression) {
        return expression -> evaluate();
    }
};

class QueryExecutionPlan {
private:
protected:
    StorageEngine engine;
    ResultSet rslt_set;
public:
    virtual bool execute() = 0;

    ResultSet getResultSet() {
        return rslt_set;
    }
};

class FROM_Execution : public QueryExecutionPlan {
private:
    string database_name; 
    string table_name;
public:
    bool execute() override {
        UserTable table =  engine.getTable(database_name, table_name);
        ColumnDefs col_defs = table.getColumnDefs();
        rslt_set.setName(table_name);
        rslt_set.setColumnDefs(col_defs);
        int row_count = table.getRowCount();
        for (int i = 0; i < row_count; i++) {
            Row current_row = engine.ReadRow(database_name, table_name, i);
            rslt_set.insertRowIntoIndex(current_row, i);
        }
        return true;
    }
};

class WHERE_Execution : public QueryExecutionPlan {
private:
    ResultSet result_set;
    string columnName;
    IntConstant* constant;
public:
    bool execute() override {
        IntVariable variable;
        IsGreaterThanINT bool_ex; 
        bool_ex.setConstant(constant);
        int new_row_count = 0;
        rslt_set.setRowCount(0);
        rslt_set.setColumnDefs(result_set.getColumnDefs());
        rslt_set.setName(result_set.getName());
        int row_count = result_set.getRowCount();
        Context context;
        for (int i = 0; i < row_count; i++) {
            Row curr_row = result_set.readRow(i);
            variable.setVariableValue(curr_row.getValueByColumnName(columnName).getIntValue());
            bool_ex.setVariable(&variable);
            context.SetContext(curr_row);
            if(bool_ex.evaluate() -> getConstantValue() == true) {
                rslt_set.insertRow(curr_row);
                new_row_count += 1;
            }
        }
        rslt_set.setRowCount(new_row_count);
        return true;
    }
};

class SELECT_Execution : public QueryExecutionPlan {
private:
    ResultSet result_set;
    string column_list;
public:
    bool execute() override {
        int size = result_set.getRowCount();
        string column_name;
        vector<string> columnList;
        stringstream ss(column_list);
        string token;
        rslt_set.setName(result_set.getName());
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
        rslt_set.setRowCount(size);
        //Set new columnDefs into reslt_set (có sẵn function setColumnDefs)
        rslt_set.setColumnDefs(newColumnDefs);
        for (int index = 0; index < size; index++) {
            //Build new row from the new ColumnDefs
            Row current_row;
            current_row.setColumnDefs(newColumnDefs);
            for (int i = 0; i < columnList.size(); i++) {
                current_row.setValueByColumnName(columnList[i], result_set.readRow(index).getValueByColumnName(columnList[i]));
            }
            //Insert new row to new result_set
            rslt_set.insertRowIntoIndex(current_row, index);
        }
        return true;
    }
};

class INSERT_INTO_Execution : public QueryExecutionPlan {
private:
    string database_name; 
    string table_name; 
    string column_list; 
    string value_list;
public:
    bool execute() override {
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
        return true;
    }
};

class CREATE_TABLE_Execution : public QueryExecutionPlan {
private:
    string database_name; 
    string table_name; 
    string column_list; 
    string type_list;
public:
    bool execute() override {
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
        return true;
    }
};

class CREATE_DATABASE_Execution : public QueryExecutionPlan {
private:
    string database_name;
public:
    bool execute() override {
        engine.createDatabase(database_name);
        return true;
    }
};

class ORDER_BY_Execution : public QueryExecutionPlan {
public:
    bool execute() override {}
};