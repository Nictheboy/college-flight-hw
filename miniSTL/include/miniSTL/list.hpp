#pragma once

#include <cstddef>
#include <iterator>
#include <memory>
#include <limits>
#include <stdexcept>
#include <utility>
#include <compare>
#include <initializer_list>

template <class T>
struct ListBaseNode {
    ListBaseNode *m_next;
    ListBaseNode *m_prev;

    inline T &value();
    inline T const &value() const;
};

template <class T>
struct ListValueNode : ListBaseNode<T> {
    union {
        T m_value;
    };
};

template <class T, class Alloc = std::allocator<T>>
struct List {
    using value_type = T;
    using allocator_type = Alloc;
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using pointer = T *;
    using const_pointer = T const *;
    using reference = T &;
    using const_reference = T const &;

private:
    using ListNode = ListBaseNode<T>;
    using AllocNode = std::allocator_traits<Alloc>::template rebind_alloc<ListValueNode<T>>;

    ListNode m_dummy;
    size_t m_size;
    [[no_unique_address]] Alloc m_alloc;

public:
    List();

    List(List &&that);

    List(List const &that);

    List &operator=(List const &that);

    bool empty();

    T &front();

    T &back();

    T const &front();

    T const &back();

    explicit List(size_t n, Alloc const &alloc = Alloc());

    List(size_t n, T const &val, Alloc const &alloc = Alloc());

    template <std::input_iterator InputIt>
    List(InputIt first, InputIt last, Alloc const &alloc = Alloc());

    List(std::initializer_list<T> ilist, Alloc const &alloc = Alloc());

    List &operator=(std::initializer_list<T> ilist);

    size_t size();

    template <std::input_iterator InputIt>
    void assign(InputIt first, InputIt last);

    void assign(std::initializer_list<T> ilist);

    void assign(size_t n, T const &val);

    void push_back(T const &val);

    void push_back(T &&val);

    void push_front(T const &val);

    void push_front(T &&val);

    ~List();

    void clear();

    struct iterator {
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = T;
        using difference_type = ptrdiff_t;
        using pointer = T *;
        using reference = T &;

    private:
        ListNode *m_curr;

        friend List;

        explicit iterator(ListNode *curr);

    public:
        iterator() = default;

        iterator &operator++();

        iterator operator++(int);

        iterator &operator--();

        iterator operator--(int);

        T &operator*();

        bool operator!=(iterator const &that);

        bool operator==(iterator const &that);
    };

    struct const_iterator {
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = T;
        using difference_type = ptrdiff_t;
        using pointer = T const *;
        using reference = T const &;

    private:
        ListNode const *m_curr;

        friend List;

        explicit const_iterator(ListNode const *curr);

    public:
        const_iterator();

        const_iterator(iterator that);

        explicit operator iterator();

        const_iterator &operator++();

        const_iterator operator++(int);

        const_iterator &operator--();

        const_iterator operator--(int);

        T const &operator*();

        bool operator!=(const_iterator const &that);

        bool operator==(const_iterator const &that);
    };

    iterator begin();

    iterator end();

    const_iterator cbegin();

    const_iterator cend();

    const_iterator begin();

    const_iterator end();

    using reverse_iterator = std::reverse_iterator<iterator>;
    using reverse_const_iterator = std::reverse_iterator<const_iterator>;

    reverse_iterator rbegin();

    reverse_iterator rend();

    reverse_const_iterator crbegin();

    reverse_const_iterator crend();

    reverse_const_iterator rbegin();

    reverse_const_iterator rend();

    iterator erase(const_iterator pos);

    iterator erase(const_iterator first, const_iterator last);

    void pop_front();

    void pop_back();

    iterator insert(const_iterator pos, const T &val);

    iterator insert(const_iterator pos, T &&val);

    iterator insert(const_iterator pos, size_t n, T const &val);

    template <std::input_iterator InputIt>
    iterator insert(const_iterator pos, InputIt first, InputIt last);

    iterator insert(const_iterator pos, std::initializer_list<T> ilist);

    bool operator==(List const &that);
};
