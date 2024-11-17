#ifndef __jackos_std_H
#define __jackos_std_H

namespace jackos {
    namespace std {

        class string {
            private:
                char* str;
                int len;
            public:
                string();
                string(char* data, int len);
                ~string();
                void append(char c);
                int length();
                char at(int index);
                char* getStr();
                string substring(int start, int end);
                string delimit(char delimeter, int index);
        };

        // bool streq(const char* first, const char* second, int first_len, int second_len) {
        //     if(first_len != second_len) {
        //         return false;
        //     }
        //     for(int i = 0; i < first_len; i++) {
        //         if(first[i] != second[i]) {
        //             return false;
        //         }
        //     }
        //     return true;
        // }
    }
}

#endif