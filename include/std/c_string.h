#ifndef __jackos_std_c_string_H
#define __jackos_std_c_string_H

extern void printf(const char* msg);
extern void printfhex(int hex);

namespace jackos {
    namespace std {
        int char_array_size(const char* a) {
            int size = 0;
            while(a[size] != '\0') {
                if(size > 1024) { // Insurance against a non-terminated string
                    return -1;
                }
                size++;
            }
            return size;
        }

        bool cmp(const char* a, const char* b) {
            int a_len = char_array_size(a);
            int b_len = char_array_size(b);

            if(a_len != b_len) return false;
            for(int i = 0; i < a_len; i++) {
                if(a[i] != b[i]) {
                    return false;
                }
            }
            return true;
        }

        const char* substr(const char* a, int start, int end) {
            if(start < 0 || end > char_array_size(a) || end < start) {
                return a;
            }
            int size = end - start + 2;
            char* substring = new char[size];
            for(int i = 0; i < size - 1; i++) {
                substring[i] = a[start + i];
            }
            substring[size - 1] = '\0';
            return substring;
        }

        bool subcmp(const char* a, const char* b) {
            if(char_array_size(b) > char_array_size(a)) {
                return subcmp(b, a);
            }
            const char* temp = substr(a, 0, char_array_size(b));
            return cmp(a, temp);
        }

        int num_occurrances(const char* a, char delimeter) {
            int count = 0;
            for(int i = 0; i < char_array_size(a); i++) {
                if(a[i] == delimeter) {
                    count++;
                }
            }
            return count;
        }

        char* concat(const char* a, const char* b) {
            char* output = new char[char_array_size(a) + char_array_size(b) + 1];
            for(int i = 0; i < char_array_size(a); i++) {
                output[i] = a[i];
            }
            for(int i = 0; i < char_array_size(b); i++) {
                output[char_array_size(a) + i] = b[i];
            }
            output[char_array_size(a) + char_array_size(b)] = '\0';
            return output;
        }

        void strcpy(char* dest, const char* src)  {
            do {
                *dest++ = *src++;
            }
            while(*src != '\0');
        }
    }
}

#endif