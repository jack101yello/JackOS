#include <std/list.h>

using namespace jackos;
using namespace jackos::std;

template <class T> list<T>::list() {
    head = 0;
    tail = 0;
}

template <class T> list<T>::~list() {
    for(int i = 0; i < size; i++) {
        pop();
    }
}

template <class T> void list<T>::push(T t) {
    if(head == 0) {
        head = new list_element;
        head -> data = t;
        head -> prev = 0;
        head -> next = 0;
        tail = head;
        return;
    }
    list_element* iterator = head;
    while(iterator -> next != 0) {
        iterator = iterator -> next;
    }
    iterator -> next = new list_element;
    tail = iterator -> next;
    tail -> data = t;
    tail -> prev = iterator;
    tail -> next = 0;
}

template <class T> void list<T>::push(T t, int index) {
    if(index < 0 || index > size) {
        return;
    }
    if(index == size) {
        push(t);
        return;
    }
    list_element* iterator = head;
    for(int i = 0; i < index; i++) {
        iterator = iterator -> next;
    }
    iterator -> data = t;
}

template <class T> void list<T>::pop() {
    if(head == 0) {
        return;
    }
    tail = tail -> prev;
    delete tail -> next;
    tail -> next = 0;
}

template <class T> void list<T>::pop(int index) {
    if(index < 0 || index > size || head == 0) {
        return;
    }
    if(index == size) {
        pop();
    }
    list_element* iterator = head;
    for(int i = 0; i < index; i++) {
        iterator = iterator -> next;
    }
    iterator -> prev -> next = iterator -> next;
    iterator -> next -> prev = iterator -> prev;
    delete iterator;
}

template <class T> T list<T>::get(int index) {
    if(head == 0) {
        return NULL;
    }
    list_element* iterator = head;
    for(int i = 0; i < index % size; i++) {
        iterator = iterator -> next;
    }
    return iterator -> data;
}

template <class T> int list<T>::get_size() {
    return size;
}