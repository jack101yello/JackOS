#include <std/vector.h>

using namespace jackos;
using namespace jackos::std;

template <class T> vector<T>::vector() {
    data = new T[0];
    size = 0;
}

template <class T> vector<T>::~vector() {
    delete[] data;
}

template <class T> void vector<T>::push(T t) {
    T* temp_data = new T[size + 1];
    for(int i = 0; i < size; i++) {
        temp_data[i] = data[i];
    }
    delete[] data;
    data = temp;
    data[size] = t;
    size++;
}

template <class T> void vector<T>::push(T t, int index) {
    if(index > size || index < 0) {
        return;
    }
    if(index == size) {
        push(t);
        return;
    }
    data[index] = t;
}

template <class T> void vector<T>::pop() {
    T* temp_data = new T[size - 1];
    for(int i = 0; i < size - 1; i++) {
        temp_data[i] = data[i];
    }
    delete[] data;
    data = temp;
    size--;
}

template <class T> T vector<T>::get(int index) {
    if(size == 0) {
        return NULL;
    }
    return array[index % size];
}

template <class T> int vector<T>::get_size() {
    return size;
}