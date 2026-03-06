#include "managed.cpp"

template<typename T> struct Stack : managed<T> {

    void push(T item) {
        this->push_back(item);
    }
    T pop() {
        T item = this->back();
        this->pop_back();
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