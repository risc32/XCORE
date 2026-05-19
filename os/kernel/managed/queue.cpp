#include "managed.cpp"

template<typename T> struct Queue : managed<T> {

    void push(T item) {
        this->push_back(item);
    }
    T pop() {
        T item = this->operator[](0);
        this->remove(0);
        return item;
    }
    T& peek() { return *(this->end() - 1); }

private:
    using managed<T>::begin;
    using managed<T>::end;
    using managed<T>::back;
    using managed<T>::push_back;
    using managed<T>::pop_back;
};