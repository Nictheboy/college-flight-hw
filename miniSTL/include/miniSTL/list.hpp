#pragma once

#include <compare>
#include <cstddef>
#include <initializer_list>
#include <iterator>
#include <limits>
#include <memory>
#include <stdexcept>
#include <utility>

template <class T, class Alloc = std::allocator<T>>
struct List {
    using value_type = T;
    using allocator_type = Alloc;
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using pointer = T*;
    using const_pointer = T const*;
    using reference = T&;
    using const_reference = T const&;

   private:
    struct ListNode {
        ListNode* m_next;
        ListNode* m_prev;
        T m_value;

        ListNode(ListNode* next, ListNode* prev)
            : ListNode(next, prev, T()) {}
        ListNode(ListNode* next, ListNode* prev, T const& value)
            : m_next(next), m_prev(prev), m_value(value) {}
        T& value() { return m_value; }
        T const& value() const { return m_value; }
    };
    using AllocNode = std::allocator_traits<Alloc>::template rebind_alloc<ListNode>;

    ListNode m_dummy;
    size_t m_size;
    [[no_unique_address]] AllocNode m_alloc;

   public:
    List()
        : m_dummy(&m_dummy, &m_dummy), m_size(0), m_alloc() {}

    List(List&& that) {
        m_dummy = that.m_dummy;
        m_size = that.m_size;
        m_alloc = std::move(that.m_alloc);
        that.m_dummy = ListValueNode(&that.m_dummy, &that.m_dummy);
        that.m_size = 0;
    }

    List(List const& that)
        : List() {
        for (auto const& val : that)
            push_back(val);
    }

    List& operator=(List const& that) {
        clear();
        for (auto const& val : that)
            push_back(val);
        return *this;
    }

    bool empty() const { return size() == 0; }

    T& front() { return m_dummy.m_next->value(); }

    T& back() { return m_dummy.m_prev->value(); }

    T const& front() const { return m_dummy.m_next->value(); }

    T const& back() const { return m_dummy.m_prev->value(); }

    explicit List(size_t n, Alloc const& alloc = Alloc())
        : List(n, T(), alloc) {}

    List(size_t n, T const& val, Alloc const& alloc = Alloc())
        : m_dummy(&m_dummy, &m_dummy), m_size(0), m_alloc(alloc) {
        assign(n, val);
    }

    template <std::input_iterator InputIt>
    List(InputIt first, InputIt last, Alloc const& alloc = Alloc())
        : m_dummy(&m_dummy, &m_dummy), m_size(0), m_alloc(alloc) {
        assign(first, last);
    }

    List(std::initializer_list<T> ilist, Alloc const& alloc = Alloc())
        : List(ilist.begin(), ilist.end(), alloc) {}

    List& operator=(std::initializer_list<T> ilist) {
        clear();
        for (auto const& val : ilist)
            push_back(val);
    }

    size_t size() const { return m_size; }

    template <std::input_iterator InputIt>
    void assign(InputIt first, InputIt last) {
        clear();
        for (auto it = first; it != last; ++it)
            push_back(*it);
    }

    void assign(std::initializer_list<T> ilist) {
        assign(ilist.begin(), ilist.end());
    }

    void assign(size_t n, T const& val) {
        clear();
        for (size_t i = 0; i < n; ++i)
            push_back(val);
    }

    void push_back(T const& val) {
        iterator it = insert_node(cend());
        *it = val;
    }

    void push_back(T&& val) {
        iterator it = insert_node(cend());
        *it = std::move(val);
    }

    void push_front(T const& val) {
        iterator it = insert_node(cbegin());
        *it = val;
    }

    void push_front(T&& val) {
        iterator it = insert_node(cbegin());
        *it = std::move(val);
    }

    ~List() {
        clear();
    }

    void clear() {
        while (!empty())
            pop_back();
    }

    struct iterator {
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = T;
        using difference_type = ptrdiff_t;
        using pointer = T*;
        using reference = T&;

       private:
        ListNode* m_curr;

        friend List;

        explicit iterator(ListNode* curr)
            : m_curr(curr) {}

       public:
        iterator() = default;

        iterator& operator++() {
            m_curr = m_curr->m_next;
            return *this;
        }

        iterator operator++(int) {
            iterator copy = *this;
            ++*this;
            return copy;
        }

        iterator& operator--() {
            m_curr = m_curr->m_prev;
            return *this;
        }

        iterator operator--(int) {
            iterator copy = *this;
            --*this;
            return copy;
        }

        T& operator*() { return m_curr->value(); }

        bool operator!=(iterator const& that) const { return m_curr != that.m_curr; }

        bool operator==(iterator const& that) const { return m_curr == that.m_curr; }
    };

    struct const_iterator {
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = T;
        using difference_type = ptrdiff_t;
        using pointer = T const*;
        using reference = T const&;

       private:
        ListNode const* m_curr;

        friend List;

        explicit const_iterator(ListNode const* curr)
            : m_curr(curr) {}

       public:
        const_iterator()
            : const_iterator(nullptr) {}

        const_iterator(iterator that)
            : const_iterator(that.m_curr) {}

        explicit operator iterator() { return iterator((ListNode*)m_curr); }

        const_iterator& operator++() {
            m_curr = m_curr->m_next;
            return *this;
        }

        const_iterator operator++(int) {
            const_iterator copy = *this;
            ++*this;
            return copy;
        }

        const_iterator& operator--() {
            m_curr = m_curr->m_prev;
            return *this;
        }

        const_iterator operator--(int) {
            const_iterator copy = *this;
            --*this;
            return copy;
        }

        T const& operator*() { return m_curr->value(); }

        bool operator!=(const_iterator const& that) const { return m_curr != that.m_curr; }

        bool operator==(const_iterator const& that) const { return m_curr == that.m_curr; }
    };

   private:
    iterator insert_node(const_iterator pos) {
        ListNode* node = m_alloc.allocate(1);
        std::allocator_traits<AllocNode>::construct(m_alloc, node, iterator(pos).m_curr, iterator(pos).m_curr->m_prev);
        iterator(pos).m_curr->m_prev->m_next = node;
        iterator(pos).m_curr->m_prev = node;
        ++m_size;
        return iterator(node);
    }

   public:
    iterator begin() { return iterator(m_dummy.m_next); }

    iterator end() { return iterator(&m_dummy); }

    const_iterator cbegin() const { return const_iterator(m_dummy.m_next); }

    const_iterator cend() const { return const_iterator(&m_dummy); }

    const_iterator begin() const { return cbegin(); }

    const_iterator end() const { return cend(); }

    using reverse_iterator = std::reverse_iterator<iterator>;
    using reverse_const_iterator = std::reverse_iterator<const_iterator>;

    reverse_iterator rbegin() { return reverse_iterator(end()); }

    reverse_iterator rend() { return reverse_iterator(begin()); }

    reverse_const_iterator crbegin() const { return reverse_const_iterator(cend()); }

    reverse_const_iterator crend() const { return reverse_const_iterator(cbegin()); }

    reverse_const_iterator rbegin() const { return crbegin(); }

    reverse_const_iterator rend() const { return crend(); }

    iterator erase(const_iterator pos) {
        ListNode* next = pos.m_curr->m_next;
        pos.m_curr->m_prev->m_next = next;
        next->m_prev = pos.m_curr->m_prev;

        std::allocator_traits<AllocNode>::destroy(m_alloc, pos.m_curr);
        m_alloc.deallocate((ListNode*)pos.m_curr, 1);
        --m_size;
        return iterator(next);
    }

    iterator erase(const_iterator first, const_iterator last) {
        while (first != last)
            first = erase(first);
        return iterator(first);
    }

    void pop_front() {
        erase(cbegin());
    }

    void pop_back() {
        erase(--cend());
    }

    iterator insert(const_iterator pos, const T& val) {
        iterator it = insert_node(pos);
        *it = val;
        return it;
    }

    iterator insert(const_iterator pos, T&& val) {
        iterator it = insert_node(pos);
        *it = std::move(val);
        return it;
    }

    iterator insert(const_iterator pos, size_t n, T const& val) {
        for (size_t i = 0; i < n; ++i)
            pos = insert(pos, val);
        return pos;
    }

    template <std::input_iterator InputIt>
    iterator insert(const_iterator pos, InputIt first, InputIt last) {
        for (auto it = first; it != last; ++it)
            pos = ++insert(pos, *it);
        return --pos;
    }

    iterator insert(const_iterator pos, std::initializer_list<T> ilist) {
        return insert(pos, ilist.begin(), ilist.end());
    }

    bool operator==(List const& that) const {
        if (size() != that.size())
            return false;
        for (auto it = begin(), that_it = that.begin(); it != end(); ++it, ++that_it)
            if (*it != *that_it)
                return false;
        return true;
    }
};