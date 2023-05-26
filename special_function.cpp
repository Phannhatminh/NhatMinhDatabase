#include <iostream>
#include <vector>
#include <string>

using namespace std;

//function for comparing vector
bool are_vectors_equal(vector<int>& v1, vector<int>& v2) {
    if (v1.size() != v2.size()) { // check if the vectors are of the same size
        return false;
    }

    for (int i = 0; i < v1.size(); i++) { // iterate over each element of the vectors
        if (v1[i] != v2[i]) { // check if the corresponding elements of the vectors are not equal
            return false;
        }
    }

    return true; // if all elements are equal, the vectors are equal
}

//trailing zero function
std::string removeTrailingZeros(std::string byteString) {
    size_t lastNonZeroIndex = byteString.size() - 1;
    while (lastNonZeroIndex > 0 && byteString[lastNonZeroIndex] == '\x00') {
        lastNonZeroIndex--;
    }

    return byteString.substr(0, lastNonZeroIndex + 1);
}
