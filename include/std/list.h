#ifndef __jackos_std_list_H
#define __jackos_std_list_H

namespace jackos {
    namespace std {
        template <class T> struct list_element {
            T data;
            list_element* prev;
            list_element* next;
        };

        template <class T> class list {
            private:
                list_element* head;
                list_element* tail;
                int size;
            public:
                list();
                ~list();

                void push(T t);
                void push(T t, int index);
                void pop();
                void pop(int index);
                T get(int index);
                int get_size();
        };
    }
}

#endif