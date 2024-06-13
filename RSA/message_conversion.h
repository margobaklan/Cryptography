#ifndef MESSAGE_CONVERSION_H
#define MESSAGE_CONVERSION_H

#include <string>
#include <vector>
#include <iostream>
#include <random>

using namespace std;

vector<int64_t> getNumMessage(const string &m) {
    vector<int64_t> message;
    for (char c : m) {
        message.push_back(static_cast<int>(c));
    }
    return message;
}

string getStrMessage(const vector<int64_t> &m) {
    string message;
    for (int64_t part : m) {
        for (int i = 0; i < 8; ++i) {
            char symbol = static_cast<char>((part >> (8 * (7 - i))) & 0xFF);
            if (symbol != '\0') {  
                message += symbol;
            }
        }
    }
    return message;
}

#endif