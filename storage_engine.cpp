#include <vector>
#include <iostream>
#include <stdio.h>
#include <cstdint>
#include <filesystem>
#include <fstream>

using namespace std;

class Extent {
public:
    Extent() {}
    void setExtentFileName(string file_name) {
        extent_file_name = file_name;
    }

    void setWidth(int value) {
        width = value;
    }

    int getExtentWidth() {
        return width;
    }

    void writeData(int offset, char* data) {
        //open the file  
        FILE* file;
        file = fopen(extent_file_name.c_str(), "rb+");

        //seek the position
        int seek = fseek(file, offset, SEEK_SET);

        //write data
        size_t wrData = fwrite(data, width, 1, file);

        //close the file
        fclose(file);
    }

    char* readData(int offset) {
        //init the data buffer
        char* data = new char[width];

        //open the file  
        FILE* file;
        file = fopen(extent_file_name.c_str(), "rb+");

        //seek the offset
        int seek = fseek(file, offset, SEEK_SET);

        //read data from file and copy it to the data buffer
        size_t rdData = fread(data, width, 1, file);

        //close file
        fclose(file);
        
        //return buffer
        return data;
    }
    
private:
    int width;
    string extent_file_name;
};

class Segment {
public:
    Segment() {}

    void setExtentsFile(int i, string file_name) {
        extent_list[i].setExtentFileName(file_name);
    }

    void setSegmentSize(int size) {
        segmentSize = size;
    }

    void createExtent() {
        Extent extent;
        extent_list.insert({extent_list.size(), extent});
    }

    void initialise_Extent(map<int, int> columns_width) {
        for (int i = 0; i < columns_width.size(); i++) {
            Extent extent;
            extent_list[i] = extent;
        }
        for (int i = 0; i < extent_list.size(); i++) {
            extent_list[i].setWidth(columns_width[i]);
        }
    }

    void write(int index, char* buffer) {
        int offset = 0;
        int extent_list_size = extent_list.size();
        for (int current_extent = 0; current_extent < extent_list_size; current_extent++) {
            //create a new pointer to the beginning offset
            char* sub_buffer = buffer + offset;

            //Compute Width
            int width = extent_list[current_extent].getExtentWidth();

            //Create a new buffer
            char* current_extent_buffer = new char[width];
            memset(current_extent_buffer, 0, width);

            //copy value to the new offset
            memcpy(current_extent_buffer, sub_buffer, width);

            //write
            extent_list[current_extent].writeData(index * width, current_extent_buffer);
            offset += width;

            //delete the buffer
            delete[] current_extent_buffer;
        }
    }

    char* read(int index) {
        //initialise some objects...
        int offset = 0;
        int extent_list_size = extent_list.size();
        char* buffer = new char[segmentSize];

        for (int current_extent = 0; current_extent < extent_list_size; current_extent ++) {
            //read data from the current extent and copy it to the current extent buffer
            int width = extent_list[current_extent].getExtentWidth();
            char* current_extent_buffer = extent_list[current_extent].readData(index * width);

            //copy the current extent buffer to the right offset of the buffer
            memcpy(buffer + offset, current_extent_buffer, width);
            offset += width;
        }
        return buffer;
    }

private:
    int segmentSize;
    map<int, Extent> extent_list;
};

class StorageEngine {};