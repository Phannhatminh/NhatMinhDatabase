#include <iostream>
#include <vector>

using namespace std;



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