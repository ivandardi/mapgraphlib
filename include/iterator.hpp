#include <cstddef>
#include <iterator>
#include <list>

template <typename T, bool is_directed>
class Graph;

template <typename T, bool is_directed>
struct graph_iterator
{
private:
    using self = graph_iterator<T, is_directed>;
    using iterator = typename std::list<T>::iterator;

    friend class Graph<typename T::value_type, is_directed>;

public:
    using difference_type = std::ptrdiff_t;
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = typename T::value_type;
    using pointer = value_type*;
    using reference = value_type&;

public:
    graph_iterator() noexcept
    : _list_iterator()
    {
    }

    explicit graph_iterator(iterator other) noexcept
    : _list_iterator(other)
    {
    }

public:
    reference operator*() const noexcept
    {
        return _list_iterator->value;
    }

    pointer operator->() const noexcept
    {
        return &(operator*());
    }

    self& operator++() noexcept
    {
        ++_list_iterator;
        return *this;
    }

    self operator++(int)noexcept
    {
        self temp = *this;
        ++_list_iterator;
        return temp;
    }

    self& operator--() noexcept
    {
        --_list_iterator;
        return *this;
    }

    self operator--(int)noexcept
    {
        self temp = *this;
        --_list_iterator;
        return temp;
    }

    bool operator==(const self& rhs) const noexcept
    {
        return _list_iterator == rhs._list_iterator;
    }

    bool operator!=(const self& rhs) const noexcept
    {
        return _list_iterator != rhs._list_iterator;
    }

private:
    iterator _list_iterator;
};
