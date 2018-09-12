//
// Created by Vladislav Kalugin on 08/09/2018.
//

#ifndef FIXED_VECTOR_FIXED_VECTOR_H
#define FIXED_VECTOR_FIXED_VECTOR_H

#include <cstdio>
#include <type_traits>
#include <new>
#include <iterator>

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
            new(data + i) T(other[i]);
        }
    }

    fixed_vector &operator=(fixed_vector const &other) {
        if (this == &other) return *this;
        for (size_t i = 0; i < _size; ++i) {
            reinterpret_cast<const T *>(data + i)->~T();
        }
        _size = other.size();
        for (size_t i = 0; i < _size; ++i) {
            new(data + i) T(other[i]);
        }
        return *this;
    }

    ~fixed_vector() {
        for (std::size_t i = 0; i < _size; ++i) {
            reinterpret_cast<const T *>(data + i)->~T();
        }
    }

    void push_back(T const &arg) {
        if (_size >= N) {
            throw std::bad_alloc{};
        }
        new(data + _size) T(arg);
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
        return *reinterpret_cast<T *>(data + pos);
    }

    const T &back() const {
        return *reinterpret_cast<const T *>(data + _size - 1);
    }

    T &back() {
        return *reinterpret_cast<T *>(data + _size - 1);
    }

    const T &front() const {
        return *reinterpret_cast<const T *>(data);
    }

    T &front() {
        return *reinterpret_cast<T *>(data);
    }

    size_t size() const {
        return _size;
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

    using iterator = T*;
    using const_iterator = T const*;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    iterator begin() {
        return reinterpret_cast<T*>(data);
    }

    const_iterator begin() const {
        return const_cast<T const*>(reinterpret_cast<T*>(data));
    }

    reverse_iterator rbegin() {
        return reverse_iterator(reinterpret_cast<T*>(data) + N);
    }

    const_reverse_iterator rbegin() const {
        return const_reverse_iterator(const_cast<T const*>(reinterpret_cast<T*>(data) + N));
    }

    iterator end() {
        return reinterpret_cast<T*>(data) + N;
    }

    const_iterator end() const {
        return const_cast<T const*>(reinterpret_cast<T*>(data) + N);
    }

    reverse_iterator rend() {
        return reverse_iterator(reinterpret_cast<T*>(data));
    }

    const_reverse_iterator rend() const {
        return const_reverse_iterator(const_cast<T const*>(reinterpret_cast<T*>(data)));
    }

    const_iterator insert(size_t const &pos, size_t const &val) {
        if (_size >= N) {
            throw std::bad_alloc{};
        }
        for (size_t i = _size; i > pos; --i) {
            reinterpret_cast<const T *>(data + i)->~T();
            new(data + i) T(data[i - 1]);
        }
        reinterpret_cast<const T *>(data + pos)->~T();
        new(data + pos) T(val);
        ++_size;
        return c_iterator(this, pos);
    }

    const_iterator erase(size_t const &pos) {
        --_size;
        for (size_t i = pos; i < _size; ++i) {
            reinterpret_cast<const T *>(data + i)->~T();
            new(data + i) T(data[i + 1]);
        }
        reinterpret_cast<const T *>(data + _size)->~T();
        return c_iterator(this, pos);
    }
};


#endif //FIXED_VECTOR_FIXED_VECTOR_H
