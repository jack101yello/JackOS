#include <std/string.h>

using namespace jackos;
using namespace jackos::std;

extern void printf(const char* msg);
extern void printf(jackos::std::string str);

string::string() {
    str = new char[1];
    str[0] = '\0';
    this -> len = 0;
}

string::string(char* data, int len) {
    str = new char[len+1];
    for(int i = 0; i < len; i++) {
        str[i] = data[i];
    }
    str[len] = '\0';
    this -> len = len;
}

string::~string() {
    delete[] str;
}

void string::append(char c) {
    char* temp = new char[len + 2];
    for(int i = 0; i < len; i++) {
        temp[i] = str[i];
    }
    delete[] str;
    str = temp;
    str[len] = c;
    str[len + 1] = '\0';
    len++;
}

int string::length() {
    return len;
}

char string::at(int index) {
    if(index < 0 || index >= len) {
        return '\0';
    }
    return str[index];
}

char* string::getStr() {
    return str;
}

string string::substring(int start, int end) {
    string output;
    if(start < 0 || start >= len || end < 0 || end >= len || end < start) {
        return output;
    }
    for(int i = start; i < end; i++) {
        if(str[i] == 'a') {
            printf("HELLO");
        }
        output.append(str[i]);
    }
    return output;
}

string string::delimit(char delimeter, int index) {
    string output;
    int count = 0;
    for(int i = 0; i < len; i++) {
        if(count == index) {
            for(int j = i; j < len; j++) {
                if(str[j] == delimeter) {
                    return output;
                }
                output.append(str[j]);
            }
        }
        if(str[i] == delimeter) {
            count++;
        }
    }
    return output;
}