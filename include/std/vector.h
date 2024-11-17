#ifndef __jackos_std_vector_H
#define __jackos_std_vector_H

namespace jackos {
    namespace std {
        template <class T> class vector {
            private:
                T* data;
                int size;

            public:
                vector();
                ~vector();

                void push(T t);
                void push(T t, int index);
                void pop();
                T get(int index);
                int get_size();
        };
    }
}

#endif