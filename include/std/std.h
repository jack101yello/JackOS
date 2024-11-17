#ifndef __jackos_std_H
#define __jackos_std_H

namespace jackos {
    namespace std {
        bool streq(const char* first, const char* second, int first_len, int second_len) {
            if(first_len != second_len) {
                return false;
            }
            for(int i = 0; i < first_len; i++) {
                if(first[i] != second[i]) {
                    return false;
                }
            }
            return true;
        }
    }
}

#endif