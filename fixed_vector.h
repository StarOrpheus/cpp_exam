//
// Created by Vladislav Kalugin on 08/09/2018.
//

#ifndef FIXED_VECTOR_FIXED_VECTOR_H
#define FIXED_VECTOR_FIXED_VECTOR_H

#include <cstdio>
#include <type_traits>
#include <new>

template<typename T, size_t N>
class fixed_vector {
private:
    size_t _size;
    typename std::aligned_storage<sizeof(T), alignof(T)>::type data[N]; //?
public:
    fixed_vector() : _size(0) {
    }

    fixed_vector(fixed_vector const &other) {
        if (this == &other) return;
        for (size_t i = 0; i < _size; ++i) {
            reinterpret_cast<const T *>(data + i)->~T();
        }
        _size = other.size();
        for (size_t i = 0; i < _size; ++i) {
            new(data + _size) T(other[i]);
        }
    }

    fixed_vector& operator=(fixed_vector const & other){
        if (this == &other) return *this;
        for (size_t i = 0; i < _size; ++i) {
            reinterpret_cast<const T *>(data + i)->~T();
        }
        _size = other.size();
        for (size_t i = 0; i < _size; ++i) {
            new(data + _size) T(other[i]);
        }
        return *this;
    }

    ~fixed_vector() {
        for (std::size_t i = 0; i < _size; ++i) {
            reinterpret_cast<const T *>(data + i)->~T();
        }
    }

    void push_back(T const &args) {
        if (_size >= N) {
            throw std::bad_alloc{};
        }
        new(data + _size) T(args);
        ++_size;
    }

    void pop_back() {
        reinterpret_cast<const T *>(data + _size - 1)->~T();
        --_size;
    }

    const T &operator[](std::size_t const &pos) const {
        return *reinterpret_cast<const T *>(data + pos);
    }

    T &operator[](std::size_t const &pos) {
        return *reinterpret_cast<const T *>(data + pos);
    }

    const T &back() const {
        return *reinterpret_cast<const T *>(data + _size - 1);
    }

    T &back() {
        return *reinterpret_cast<const T *>(data + _size - 1);
    }

    const T &front() const {
        return *reinterpret_cast<const T *>(data);
    }

    T &front() {
        return *reinterpret_cast<const T *>(data);
    }

    size_t size() const {
        return size;
    }

    bool empty() const {
        return size() == 0;
    }

    size_t capacity() {
        return N;
    }

    size_t max_size() {
        return N;
    }
};


#endif //FIXED_VECTOR_FIXED_VECTOR_H
