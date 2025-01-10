#pragma once

#include <cstddef>
#include <iterator>
#include <memory>
#include <limits>
#include <stdexcept>
#include <utility>
#include <compare>
#include <initializer_list>

template <class T, class Alloc = std::allocator<T>>
struct Vector {
    using value_type = T;
    using allocator_type = Alloc;
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using pointer = T *;
    using const_pointer = T const *;
    using reference = T &;
    using const_reference = T const &;
    using iterator = T *;
    using const_iterator = T const *;
    using reverse_iterator = std::reverse_iterator<T *>;
    using const_reverse_iterator = std::reverse_iterator<T const *>;

    T *m_data;
    size_t m_size;
    size_t m_cap;
    [[no_unique_address]] Alloc m_alloc;

    Vector();

    Vector(std::initializer_list<T> ilist, Alloc const &alloc = Alloc());

    explicit Vector(size_t n, Alloc const &alloc = Alloc());

    Vector(size_t n, T const &val, Alloc const &alloc = Alloc());

    template <std::random_access_iterator InputIt>
    Vector(InputIt first, InputIt last, Alloc const &alloc = Alloc());

    void clear();

    void resize(size_t n);

    void resize(size_t n, T const &val);

    void shrink_to_fit();

    void reserve(size_t n);

    size_t capacity();

    size_t size();

    bool empty();

    T const &operator[](size_t i);

    T &operator[](size_t i);

    T const &at(size_t i);

    T &at(size_t i);

    Vector(Vector &&that);

    Vector(Vector &&that, Alloc const &alloc);
    Vector &operator=(Vector &&that);

    void swap(Vector &that);

    Vector(Vector const &that);

    Vector(Vector const &that, Alloc const &alloc);

    Vector &operator=(Vector const &that);

    T const &front();

    T &front();

    T const &back();

    T &back();

    void push_back(T const &val);

    void push_back(T &&val);

    T *data();

    T const *data();

    T const *cdata();

    T *begin();

    T *end();

    T const *begin();

    T const *end();

    T const *cbegin();

    T const *cend();

    std::reverse_iterator<T *> rbegin();

    std::reverse_iterator<T *> rend();

    std::reverse_iterator<T const *> crbegin();

    std::reverse_iterator<T const *> crend();

    void pop_back();

    T *erase(T const *it) noexcept(std::is_nothrow_move_assignable_v<T>);

    T *erase(T const *first, T const *last) noexcept(std::is_nothrow_move_assignable_v<T>);

    void assign(size_t n, T const &val);

    template <std::random_access_iterator InputIt>
    void assign(InputIt first, InputIt last);

    void assign(std::initializer_list<T> ilist);

    Vector &operator=(std::initializer_list<T> ilist);

    T *insert(T const *it, T &&val);

    T *insert(T const *it, T const &val);

    T *insert(T const *it, size_t n, T const &val);

    template <std::random_access_iterator InputIt>
    T *insert(T const *it, InputIt first, InputIt last);

    T *insert(T const *it, std::initializer_list<T> ilist);

    ~Vector();

    bool operator==(Vector const &that);
};
