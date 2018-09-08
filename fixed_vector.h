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

    struct c_iterator : std::iterator<std::bidirectional_iterator_tag, const T> {
        fixed_vector<T, N> const *current_vector;

        c_iterator() = default;

        c_iterator operator++() {
            ++pos;
            return *this;
        }

        c_iterator operator--() {
            if (pos == 0) {
                pos = current_vector->size() - 1;
            } else {
                --pos;
            }
            return *this;
        }

        const c_iterator operator++(int) {
            c_iterator result(*this);
            operator++();
            return result;
        }

        const c_iterator operator--(int) {
            c_iterator result(*this);
            operator--();
            return result;
        }

        T const &operator*() const {
            return (*current_vector)[pos];
        }

        T const *operator->() const {
            return &((*current_vector)[pos]);
        }

        c_iterator(c_iterator const &rhs) {
            current_vector = rhs.current_vector;
            pos = rhs.pos;
        }

        c_iterator &operator=(c_iterator const &rhs) {
            current_vector = rhs.current_vector;
            pos = rhs.pos;
            return *this;
        }

    private:
        size_t pos;

        c_iterator(fixed_vector const &vc, size_t pos) : current_vector(&vc), pos(pos) {}

        c_iterator(fixed_vector const *vc, size_t pos) : current_vector(vc), pos(pos) {}

        friend class fixed_vector<T, N>;

        friend bool operator==(c_iterator const &a, c_iterator const &b) {
            return a.current_vector == b.current_vector && a.pos == b.pos;
        }

        friend bool operator!=(c_iterator const &a, c_iterator const &b) {
            return a.current_vector != b.current_vector || a.pos != b.pos;
        }

        friend bool operator<(c_iterator const &a, c_iterator const &b) {
            return a.pos < b.pos;
        }

        friend bool operator>(c_iterator const &a, c_iterator const &b) {
            return a.pos > b.pos;
        }

        friend bool operator<=(c_iterator const &a, c_iterator const &b) {
            return a.pos <= b.pos;
        }

        friend bool operator>=(c_iterator const &a, c_iterator const &b) {
            return a.pos >= b.pos;
        }
    };

public:
    typedef c_iterator const_iterator;
    typedef const_iterator iterator;
    typedef std::reverse_iterator<iterator> reverse_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;


    const_iterator begin() const {
        return c_iterator(this, 0);
    }

    const_iterator end() const {
        return c_iterator(this, _size);
    }

    const_iterator cbegin() const {
        return begin();
    }

    const_iterator cend() const {
        return end();
    }

    reverse_iterator rbegin() {
        return reverse_iterator(end());
    }

    const_reverse_iterator rbegin() const {
        return reverse_iterator(end());
    }


    const_reverse_iterator rend() const {
        return reverse_iterator(begin());
    }

    const_reverse_iterator crbegin() const {
        return const_reverse_iterator(cend());
    }

    const_reverse_iterator crend() const {
        return const_reverse_iterator(cbegin());
    }

    const_iterator inseart(size_t const &pos, size_t const &val) {
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
