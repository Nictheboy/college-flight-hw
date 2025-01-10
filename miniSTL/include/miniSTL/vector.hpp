#pragma once

#include <assert.h>
#include <compare>
#include <cstddef>
#include <initializer_list>
#include <iterator>
#include <limits>
#include <memory>
#include <stdexcept>
#include <utility>

template <class T, class Alloc = std::allocator<T>>
struct Vector {
    using value_type = T;
    using allocator_type = Alloc;
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using pointer = T*;
    using const_pointer = T const*;
    using reference = T&;
    using const_reference = T const&;
    using iterator = T*;
    using const_iterator = T const*;
    using reverse_iterator = std::reverse_iterator<T*>;
    using const_reverse_iterator = std::reverse_iterator<T const*>;
    using allocator_traits = std::allocator_traits<Alloc>;

    T* m_data;
    size_t m_size;
    size_t m_cap;
    [[no_unique_address]] Alloc m_alloc;

   private:
    void initialize(size_t n, Alloc const& alloc) {
        m_data = ((Alloc&)alloc).allocate(n);
        m_size = 0;
        m_cap = n;
        m_alloc = alloc;
    }

    void reallocate(int n) {
        assert(n >= m_size);
        T* old = m_data;
        m_data = m_alloc.allocate(n);
        for (int i = 0; i < m_size; i++)
            m_data[i] = std::move(old[i]);
        for (int i = m_size; i < n; i++)
            allocator_traits::construct(m_alloc, m_data + i);
        m_alloc.deallocate(old, m_cap);
        m_cap = n;
    }

    void ensure_space() {
        if (m_size == m_cap)
            reallocate(m_cap == 0 ? 1 : m_cap * 2);
    }

    T* insert_space(T const* it, size_t n) {
        size_t index = it - m_data;
        reallocate(m_size + n);
        T* it2 = m_data + index;
        for (T* i = end() - 1; i != it2; i--)
            *i = std::move(*(i - n));
        m_size += n;
        return it2;
    }

   public:
    Vector()
        : Vector(0, T()) {}

    Vector(std::initializer_list<T> ilist, Alloc const& alloc = Alloc()) {
        initialize(ilist.size(), alloc);
        assign(ilist);
    }

    explicit Vector(size_t n, Alloc const& alloc = Alloc())
        : Vector(n, T(), alloc) {}

    Vector(size_t n, T const& val, Alloc const& alloc = Alloc()) {
        initialize(n, alloc);
        assign(n, val);
    }

    template <std::random_access_iterator InputIt>
    Vector(InputIt first, InputIt last, Alloc const& alloc = Alloc()) {
        initialize(last - first, alloc);
        assign(first, last);
    }

    void clear() {
        while (!empty())
            pop_back();
        reallocate(0);
    }

    void resize(size_t n) {
        resize(n, T());
    }

    void resize(size_t n, T const& val) {
        while (size() > n)
            pop_back();
        reallocate(n);
        while (size() < n)
            push_back(val);
    }

    void shrink_to_fit() { reallocate(m_size); }

    void reserve(size_t n) {
        if (n <= m_cap)
            return;
        reallocate(n);
    }

    size_t capacity() const { return m_cap; }

    size_t size() const { return m_size; }

    bool empty() const { return size() == 0; }

    T const& operator[](size_t i) const { return m_data[i]; }

    T& operator[](size_t i) { return m_data[i]; }

    T const& at(size_t i) const { return m_data[i]; }

    T& at(size_t i) { return m_data[i]; }

    Vector(Vector&& that)
        : Vector(that, that.m_alloc) {}

    Vector(Vector&& that, Alloc const& alloc) {
        initialize(that.m_size, alloc);
        for (auto iterator = that.begin(); iterator != that.end(); iterator++)
            push_back(*iterator);
    }

    Vector& operator=(Vector&& that) {
        ~Vector();
        assign(that.begin(), that.end());
    }

    void swap(Vector& that) {
        T* data = m_data;
        size_t size = m_size;
        size_t cap = m_cap;
        Alloc alloc = m_alloc;
        m_data = that.m_data;
        m_size = that.m_size;
        m_cap = that.m_cap;
        m_alloc = that.m_alloc;
        that.m_data = data;
        that.m_size = size;
        that.m_cap = cap;
        that.m_alloc = alloc;
    }

    Vector(Vector const& that)
        : Vector(that) {}

    Vector(Vector const& that, Alloc const& alloc)
        : Vector(that, alloc) {}

    Vector& operator=(Vector const& that) {
        ~Vector();
        assign(that.begin(), that.end());
    }

    T const& front() const { return m_data[0]; }

    T& front() { return m_data[0]; }

    T const& back() const { return m_data[m_size - 1]; }

    T& back() { return m_data[m_size - 1]; }

    void push_back(T const& val) {
        ensure_space();
        m_data[m_size++] = val;
    }

    void push_back(T&& val) {
        ensure_space();
        m_data[m_size++] = std::move(val);
    }

    T* data() { return m_data; }

    T const* data() const { return m_data; }

    T const* cdata() const { return m_data; }

    T* begin() { return m_data; }

    T* end() { return m_data + m_size; }

    T const* begin() const { return m_data; }

    T const* end() const { return m_data + m_size; }

    T const* cbegin() const { return m_data; }

    T const* cend() const { return m_data + m_size; }

    std::reverse_iterator<T*> rbegin() { return std::reverse_iterator<T*>(end()); }

    std::reverse_iterator<T*> rend() { return std::reverse_iterator<T*>(begin()); }

    std::reverse_iterator<T const*> crbegin() { return std::reverse_iterator<T const*>(cend()); }

    std::reverse_iterator<T const*> crend() { return std::reverse_iterator<T const*>(cbegin()); }

    void pop_back() {
        assert(!empty());
        m_size--;
        allocator_traits::destroy(m_alloc, m_data + m_size);
    }

    T* erase(T const* it) noexcept(std::is_nothrow_move_assignable_v<T>) {
        return erase(it, it + 1);
    }

    T* erase(T const* first, T const* last) noexcept(std::is_nothrow_move_assignable_v<T>) {
        assert(first <= last);
        assert(last <= end());
        size_t n = last - first;
        for (T* it = first - m_data + m_data; it != last; it++)
            it->~T();
        for (T* it = first - m_data + m_data; it != end() - n; it++)
            *it = std::move(*(it + n));
        m_size -= n;
        return first - m_data + m_data;
    }

    void assign(size_t n, T const& val) {
        clear();
        resize(n, val);
    }

    template <std::random_access_iterator InputIt>
    void assign(InputIt first, InputIt last) {
        clear();
        for (InputIt it = first; it != last; it++)
            push_back(*it);
    }

    void assign(std::initializer_list<T> ilist) {
        clear();
        for (T const& val : ilist)
            push_back(val);
    }

    Vector& operator=(std::initializer_list<T> ilist) {
        assign(ilist);
    }

    T* insert(T const* it, T&& val) {
        T* it2 = insert_space(it, 1);
        *it2 = std::move(val);
        return it2;
    }

    T* insert(T const* it, T const& val) {
        T* it2 = insert_space(it, 1);
        *it2 = val;
        return it2;
    }

    T* insert(T const* it, size_t n, T const& val) {
        T* it2 = insert_space(it, n);
        for (size_t i = 0; i < n; i++)
            it2[i] = val;
        return it2;
    }

    template <std::random_access_iterator InputIt>
    T* insert(T const* it, InputIt first, InputIt last) {
        size_t n = last - first;
        T* it2 = insert_space(it, n);
        for (size_t i = 0; i < n; i++)
            it2[i] = *first++;
        return it2;
    }

    T* insert(T const* it, std::initializer_list<T> ilist) {
        return insert(it, ilist.begin(), ilist.end());
    }

    ~Vector() {
        clear();
        m_alloc.deallocate(m_data, m_cap);
    }

    bool operator==(Vector const& that) const {
        if (size() != that.size())
            return false;
        for (size_t i = 0; i < size(); i++)
            if (!(m_data[i] == that.m_data[i]))
                return false;
        return true;
    }
};