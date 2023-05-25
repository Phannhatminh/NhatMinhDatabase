#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <cstring>
#include <stdio.h>
#include "storage_engine.cpp"
using namespace std;

class DBType {
public:
    enum Type {INT, FLOAT, STRING};
};

class DBValue {
public:
    DBValue() {}
    
    //Constructor for each type of value:
    DBValue(int value) {
        type_ = DBType :: INT;
        int_value = value;
    }

    DBValue(float value) {
        type_ = DBType :: FLOAT;
        float_value = value;
    }

    DBValue(const string value) {
        type_ = DBType :: STRING;
        str_value = value;
    }

    DBType :: Type getType() {
        return type_;
    }

    int getIntValue() {
        return int_value;
    }

    float getFloatValue() {
        return float_value;
    }

    string getStringValue() {
        return str_value;
    }

    int getWidth() {
        if (type_ == DBType :: INT) {
            return sizeof(int);
        }
        if (type_ == DBType :: FLOAT) {
            return sizeof(float);
        }
        if (type_ == DBType :: STRING) {
            return 30;
        }
        else {
            return 0;
        }
    }

    void setInt(int num) {
        int_value = num;
    }

    void setFloat(float f) {
        float_value = f;
    }

    void setString(string str) {
        str_value = str;
    }

    void setType(DBType :: Type type) {
        type_ = type;
    }


private:
    DBType :: Type type_; //type
    int int_value; // Integer value
    float float_value; // Float value
    string str_value; // String value
};

class ColumnDef {
public:
    ColumnDef() {}
    ColumnDef(string name, DBType::Type type, int width)
        {
            name_ = name;
            type_ = type;
            width_ = width;
        }

    string getName() {
        return name_;
    }

    DBType::Type getType() {
        return type_;
    }

    int getWidth() {
        if (getType() == DBType :: INT) {
            return sizeof(int);
        }
        if (getType() == DBType :: FLOAT) {
            return sizeof(float);
        }
        if (getType() == DBType :: STRING) {
            return 30;
        };
        return 0;
    }

    bool isDifferentColumnDef(ColumnDef other) {
        if (name_ != other.getName() && type_ != other.getType()) {
            return true;
        }
        else {
            return false;
        }
    }

private:
    string name_;
    DBType :: Type type_;
    int width_;
};

class ColumnDefs {
public:
    map<string, ColumnDef> columns_;
    map<int, string> columnsName;
    
    ColumnDefs() {}

    void addColumn(ColumnDef column) {
        int size = columns_.size();
        columnsName.insert({size, column.getName()});
        columns_.insert({column.getName(), column});
    }

    int getColumnCount()  {
        return columns_.size();
    }

    ColumnDef getColumnDef(string ColumnName) {
        if (!columns_.count(ColumnName)) {
            throw out_of_range("Column index out of range");
        }
        return columns_[ColumnName];
    }

    int getRowSize() {
        int rowSize_ = 0;
            for (auto& column : columns_) {
                rowSize_ += column.second.getWidth();
            }
        return rowSize_;
    }

    bool isDifferentColumnDefs(ColumnDefs colDefs) {
        bool different = true;
        for (int i = 0; i < columns_.size(); i++) {
            ColumnDef current_ColumnDef = colDefs.columns_[columnsName[i]];
            if (columns_[columnsName[i]].isDifferentColumnDef(current_ColumnDef)) {
                different = different && true; 
            }
            else {
                different = different && false;
            }
        }
        return different;
    }
};


class Row {
public:
    Row() {}
    Row(ColumnDefs columnDefs)
    {
        columnDefs_ = columnDefs;
    }

    ColumnDefs getColumnDefs() const {
        return columnDefs_;
    }

    void setColumnDefs(ColumnDefs col_defs) {
        columnDefs_ = col_defs;
    }

    void setValueByColumnName(string name, DBValue value) {
        //Check if there exists a column with name name in Table
        if (values_.find(name) != values_.end()) {
            values_[name] = value;
        }
        else {
            int size = values_.size();
            values_name[size] = name;
            values_[name] = value;
        }
    }

    DBValue getValueByColumnName(string name) {
        //Check if there exists a column with name name in the row
        if (values_.find(name) != values_.end()) {
            return values_[name];
        }
        else {
            DBValue ret_val;
            cout << "No such column name " << name << " in row" << endl;
            return ret_val;
        }
    }
    
    //encode the Row into the buffer
    char* encode() {
        ColumnDefs colDefs = getColumnDefs();
        int rowBufferSize = getColumnDefs().getRowSize();
        int rowLength = values_.size();
        int offset = 0;

        //allocate memory for rowBuffer_
        rowBuffer_ = new char[columnDefs_.getRowSize()];

        //clean the buffer
        memset(rowBuffer_, 0, rowBufferSize);

        //encoding process
        for (int i = 0; i < rowLength; i++) {
            if (values_[values_name[i]].getType() == DBType :: INT) {
                //Convert integer value to byte array and copy it to the rơBuffer
                int int_value = values_[values_name[i]].getIntValue();
                memcpy(rowBuffer_ + offset, &int_value, sizeof(int));
                offset += sizeof(int);
            }
            if (values_[values_name[i]].getType() == DBType :: FLOAT) {
                float float_value = values_[values_name[i]].getFloatValue();
                memcpy(rowBuffer_ + offset, &float_value, sizeof(float));
                offset += sizeof(float);
            }

            if (values_[values_name[i]].getType() == DBType :: STRING) {
                string str_value = values_[values_name[i]].getStringValue();
                int str_length = str_value.size();
                char* char_str_value = &str_value[0];
                memcpy(rowBuffer_ + offset, char_str_value, str_length);
                offset += str_length;
                if (str_length < values_[values_name[i]].getWidth()) {
                    memset(rowBuffer_ + offset + str_length, 0, values_[values_name[i]].getWidth() - str_length);
                    offset += values_[values_name[i]].getWidth() - str_length;
                }
            }
        }
        return rowBuffer_;
    }

    //decode from byte array to field value in Row
    //Deckde build row xong return ra row với mấy cái map trống trơn.
    void decode(char* buffer) {
        DBValue value;
        int row_length = columnDefs_.columns_.size();
        int offset = 0;
        for (int i = 0; i < row_length; i++) {
            if (columnDefs_.columns_[columnDefs_.columnsName[i]].getType() == DBType :: INT) {
                int num;
                memcpy(&num, buffer + offset, sizeof(int)); 
                value.setInt(num);
                this -> setValueByColumnName(columnDefs_.columnsName[i], value);
            }
            if (columnDefs_.columns_[columnDefs_.columnsName[i]].getType() == DBType :: FLOAT) {
                float f;
                memcpy(&f, buffer + offset, sizeof(float));
                value.setFloat(f);
                this -> setValueByColumnName(columnDefs_.columnsName[i], value);
            }
            if (columnDefs_.columns_[columnDefs_.columnsName[i]].getType() == DBType :: STRING) {
                string str(buffer, 30);
                value.setString(str);
                this -> setValueByColumnName(columnDefs_.columnsName[i], value);  
            }  
            offset += columnDefs_.columns_[columnDefs_.columnsName[i]].getWidth();
        }
    }

private:
    ColumnDefs columnDefs_;
    map<string, DBValue> values_;
    map<int, string> values_name;
    map<string, ColumnDef> columns = columnDefs_.columns_;
    char* rowBuffer_;

    //Set a vector of values into row
    void SetValue(vector<DBValue> val_Vec) {
        //check if wrong input that mismatch the expected structure
        if (val_Vec.size() != columnDefs_.columns_.size()) {    
            cerr << "Error: Input vector has different size than value_ vector" << endl;
            return;
        }
        else {
            int size = columnDefs_.columnsName.size();
            for (int i = 0; i < size; i++) {
                values_name.insert({i, columnDefs_.columnsName[i]});
                values_.insert({columnDefs_.columnsName[i], val_Vec[i]});
            }
        }
    }

    // get Value
    vector<DBValue> getValue() {
        vector<DBValue> val_Vec;
        for (int i = 0; i < values_.size(); i++) {
            val_Vec.push_back(values_[values_name[i]]);
        }
        return val_Vec;
    }
};

class Table {
public:
    virtual void createRow(Row row) = 0;
    virtual Row readRow(int index) = 0;
    virtual int getRowCount() = 0;
    virtual void setRowCount(int n) = 0;
};

class UserTable : public Table {
public:
    UserTable()
    {}
    
    UserTable(ColumnDefs col_Defs) {
        columnDefs_ = col_Defs;
    }

    void setName(string name) {
        table_name = name;
    }

    void setColumnDefs(ColumnDefs columndefs) {
        columnDefs_ = columndefs;
    }

    ColumnDefs getColumnDefs() {
        return columnDefs_;
    }

    void createRow(Row row) override {
        //handle the case where row has a different columndefs than Table
        if (row.getColumnDefs().isDifferentColumnDefs(columnDefs_)) {
            cout << "Row's structure doesn't fit Table's structure" << endl;
        }
        else {
            //encode Row into Byte Buffer
            char* buffer = row.encode();
            //write the byteBuffer to files
            setDataSegment().write(row_count, buffer);
            //Update row_count
            row_count += 1;
        }
    }

    Row readRow(int index) override {
        Row row;
        row.setColumnDefs(columnDefs_);
        char* buffer = setDataSegment().read(index);
        row.decode(buffer);
        return row;
    }

    void setRowCount(int n) override {
        row_count = n;
    }

    int getRowCount() override {
        return row_count;
    }

    void setDatabaseName(string name) {
        DatabaseName = name;
    }

    //set and get the corresponding data segment
    Segment setDataSegment() {
        Segment segment;
        map<int, int> columns_width;
        int i = 0;
        int current_extent = 0;
        int size = columnDefs_.columns_.size();
        int table_size = columnDefs_.getRowSize();
        for (int i = 0; i < size; i++) {
            columns_width[i] = columnDefs_.columns_[columnDefs_.columnsName[i]].getWidth();
        }
        segment.setSegmentSize(table_size);
        segment.initialise_Extent(columns_width);
        for(int i = 0; i < size; i++) {
            segment.setExtentsFile(current_extent, DatabaseName + "/" + table_name + "-" + columnDefs_.columnsName[i] + ".bin");
            current_extent += 1;
        }
        return segment;
    }

private:
    int row_count;
    string table_name;
    ColumnDefs columnDefs_;
    Segment DataSegment;
    Segment Index;
    map<string, Extent> table_extent_list;
    string DatabaseName;
};

class SystemTable : public Table {
public:
    SystemTable() {}
    SystemTable(ColumnDefs col_Defs) {
        columnDefs_ = col_Defs;
    }

    void setName(string name) {
        table_name = name;
    }

    void setColumnDefs(ColumnDefs columndefs) {
        columnDefs_ = columndefs;
    }

    ColumnDefs getColumnDefs() {
        return columnDefs_;
    }

    void createRow(Row row) override {
        //handle the case where row has a different columndefs than Table
        if (row.getColumnDefs().isDifferentColumnDefs(columnDefs_)) {
            cout << "Row's structure doesn't fit Table's structure" << endl;
        }
        else {
            //encode Row into Byte Buffer
            char* buffer = row.encode();
            //write the byteBuffer to files
            setDataSegment().write(row_count, buffer);
            //Update row_count
            row_count += 1;
        }
    }

    void updateRow(Row row, int index) {
        //handle the case where row has a different columndefs than Table
        if (row.getColumnDefs().isDifferentColumnDefs(columnDefs_)) {
            cout << "Row's structure doesn't fit Table's structure" << endl;
        }
        else {
            //encode Row into Byte Buffer
            char* buffer = row.encode();
            //write the byteBuffer to files
            setDataSegment().write(index, buffer);
        }
    }

    Row readRow(int index) override {
        Row row;
        row.setColumnDefs(columnDefs_);
        char* buffer = setDataSegment().read(index);
        row.decode(buffer);
        return row;
    }

    void setRowCount(int n) override {
        row_count = n;
    }

    int getRowCount() override {
        return row_count;
    }

    void setDatabaseName(string name) {
        DatabaseName = name;
    }

    //get the corresponding data segment
    Segment setDataSegment() {
        Segment segment;
        map<int, int> columns_width;
        int current_extent = 0;
        int size = columnDefs_.columns_.size();
        int table_size = columnDefs_.getRowSize();
        for (int i = 0; i < size; i++) {
            string column_name = columnDefs_.columnsName[i];
            columns_width[i] = columnDefs_.columns_[columnDefs_.columnsName[i]].getWidth();
        }
        segment.setSegmentSize(table_size);
        segment.initialise_Extent(columns_width);
        for(int i = 0; i < size; i++) {
            segment.setExtentsFile(current_extent, DatabaseName + "/" + table_name + "-" + columnDefs_.columnsName[i] + ".bin");
            current_extent += 1;
        }
        return segment;
    }

private:
    int row_count;
    string table_name;
    ColumnDefs columnDefs_;
    Segment DataSegment;
    Segment Index;
    map<string, Extent> table_extent_list;
    string DatabaseName;
};