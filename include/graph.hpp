#pragma once

#include "iterator.hpp"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <list>
#include <numeric>
#include <string>
#include <vector>

template <typename T, bool is_directed>
class Graph
{

private:
    struct Vertex;
    using vertex_list = std::list<Vertex>;
    using graph = Graph<T, is_directed>;

public:
    using value_type = T;
    using size_type = typename vertex_list::size_type;
    using difference_type = typename vertex_list::difference_type;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    using reference = value_type&;
    using const_reference = const value_type&;

    using iterator = graph_iterator<Vertex, is_directed>;

    static const std::string::size_type npos = std::string::npos;

public:
    struct Edge
    {
        using vertex_pointer = typename vertex_list::iterator;

        Edge(vertex_pointer const& incident_vertex)
        : incident_vertex(incident_vertex)
        {
        }

        vertex_pointer incident_vertex;
    };

private:
    struct Vertex
    {

        using value_type = T;

        Vertex(value_type const& value)
        : value(value)
        , adjacent_edges()
        {
        }

        value_type value;
        std::vector<Edge> adjacent_edges;
    };

public:
    /// Copy-assign constructor. If failure happens when assigning, it is
    /// guaranteed that the state won't be altered.
    ///
    /// # Complexity
    /// Linear in the size of *this and other.
    ///
    /// \param other The graph to be copied
    /// \return *this
    graph& operator=(graph const& other)
    {
        _vertices = other._vertices;
        return *this;
    }

    /// Move-assign constructor. If failure happens when assigning, it is
    /// guaranteed that the state won't be altered.
    ///
    /// # Complexity
    /// Linear in the size of *this and other.
    ///
    /// \param other The graph to be moved
    /// \return *this
    graph& operator=(graph&& other)
    {
        _vertices = std::move(other)._vertices;
        return *this;
    }

    /// Initializer-list constructor. Used to initialize the set of vertices.
    ///
    /// # Complexity
    /// Linear in the size of *this and ilist.
    ///
    /// \param ilist `std::initializer_list` of vertices
    /// \return *this
    graph& operator=(std::initializer_list<value_type> ilist)
    {
        _vertices = ilist;
        return *this;
    }

public:
    /// Returns an iterator to the first element of the container. If the
    /// container is empty, the returned iterator will
    /// be equal to `end()`.
    ///
    /// # Complexity
    /// Constant.
    ///
    /// \return Iterator to the first element
    iterator begin() noexcept
    {
        return iterator(_vertices.begin());
    }

    /// Returns an iterator to the element following the last element of the
    /// container. This element acts as a
    /// placeholder; attempting to access it results in undefined behavior.
    ///
    /// # Complexity
    /// Constant.
    ///
    /// \return Iterator to the element following the last element
    iterator end() noexcept
    {
        return iterator(_vertices.end());
    }

public:
    std::vector<iterator> adjacent_vertices_of(iterator const& vertex)
    {
        using std::begin;
        using std::end;

        std::vector<iterator> adj;
        for (Edge const& edge : vertex._list_iterator->adjacent_edges) {
            adj.emplace_back(edge.incident_vertex);
        }
        return adj;
    }

    /// Checks if the container has no elements, i.e. whether `begin() ==
    /// end()`.
    ///
    /// # Complexity
    /// Constant.
    ///
    /// \return `true` if the container is empty, false otherwise
    bool empty() const noexcept
    {
        return _vertices.empty();
    }

    /// Returns the number of vertices in the graph.
    ///
    /// # Complexity
    /// Constant.
    ///
    /// \return the number of vertices in the graph
    size_type num_vertices() const noexcept
    {
        return _vertices.size();
    }

    /// Returns the number of edges in the graph.
    ///
    /// # Complexity
    /// Linear in the amount of vertices.
    ///
    /// \return the number of edges in the graph
    size_type num_edges() const noexcept
    {
        using std::accumulate;
        using std::cbegin;
        using std::cend;

        auto const amount = accumulate(cbegin(_vertices), cend(_vertices), static_cast<size_type>(0),
                                       [](size_type s, Vertex const& vertex) {
                                           std::cout << "SIZE = " << vertex.adjacent_edges.size() << '\n';
                                           return s + vertex.adjacent_edges.size();
                                       });

        return is_directed ? amount : amount / 2;
    }

    /// Removes all elements from the container. Invalidates any references,
    /// pointers, or iterators referring to
    /// contained elements. May also invalidate past-the-end iterators.
    ///
    /// # Complexity
    /// Linear in the size of the graph.
    ///
    void clear() noexcept
    {
        _vertices.clear();
    }

    /// Inserts element(s) into the container, if the container doesn't already
    /// contain an element with an equivalent
    /// key.
    ///
    /// # Complexity
    /// Average case: O(1), worst case linear in the amount of vertices
    ///
    /// \param value element value to insert
    iterator insert(value_type const& value) noexcept
    {
        _vertices.push_back(value);
        return end();
    }
    /// Inserts element(s) into the container, if the container doesn't already
    /// contain an element with an equivalent
    /// key.
    ///
    /// # Complexity
    /// Average case: O(1), worst case linear in the amount of vertices
    ///
    /// \param value element value to insert
    iterator insert(value_type&& value) noexcept
    {
        _vertices.push_back(std::move(value));
        return end();
    }

    /// Inserts a new element into the container constructed in-place with the
    /// given args if there is no element with
    /// the key in the container.
    ///
    /// # Complexity
    /// Amortized constant on average, worst case linear in the amount of
    /// vertices.
    ///
    /// # Exceptions
    /// If an exception is thrown by any operation, this function has no effect.
    ///
    /// \param args arguments to forward to the constructor of the element
    template <typename... Args>
    iterator emplace(Args&&... args)
    {
        _vertices.emplace_back(std::forward<Args>(args)...);
        return end();
    };

    /// Removes the element at pos. References and iterators to the erased
    /// elements are invalidated. Other iterators and
    /// references are not invalidated. The iterator pos must be valid and
    /// dereferenceable. Thus the `end()` iterator
    /// (which is valid, but is not dereferencable) cannot be used as a value
    /// for pos.
    ///
    /// # Complexity
    /// Average case constant, worst case linear in the amount of edges
    ///
    /// \param pos iterator to the element to remove
    void erase(iterator pos) noexcept
    {
        using std::remove_if;
        using std::begin;
        using std::end;

        for (auto& vertex : _vertices) {
            vertex.adjacent_edges.erase(remove_if(begin(vertex.adjacent_edges), end(vertex.adjacent_edges),
                                                  [pos](Edge const& edge) {
                                                      return edge.incident_vertex == pos._list_iterator;
                                                  }),
                                        end(vertex.adjacent_edges));
        }

        return iterator(_vertices.erase(pos._list_iterator));
    }

    /// Exchanges the contents of the container with those of other. Does not
    /// invoke any move, copy, or swap operations
    /// on individual elements. All iterators and references remain valid. The
    /// past-the-end iterator is invalidated.
    ///
    /// # Complexity
    /// Constant.
    ///
    /// \param other container to exchange the contents with
    void swap(graph& other)
    {
        _vertices.swap(other._vertices);
    }

    /// Adds a directed edge (a, b) from the vertex with key a to the vertex
    /// with key b.
    ///
    /// # Complexity
    /// Constant.
    ///
    /// \param a key of the first vertex
    /// \param b key of the second vertex
    void add_edge(iterator& a, iterator& b) noexcept
    {
        a._list_iterator->adjacent_edges.push_back(Edge(b._list_iterator));
        std::cout << "adj edges size is: " << a._list_iterator->adjacent_edges.size() << '\n';
        if (!is_directed) {
            b._list_iterator->adjacent_edges.push_back(Edge(a._list_iterator));
        }
    }

private:
    vertex_list _vertices; ///< The vertices of the graph
};

template <typename T, bool is_directed>
inline bool operator==(Graph<T, is_directed> const& lhs, Graph<T, is_directed> const& rhs)
{
    return lhs._vertices == rhs._vertices;
}

template <typename T, bool is_directed>
inline bool operator!=(Graph<T, is_directed> const& lhs, Graph<T, is_directed> const& rhs)
{
    return !(lhs == rhs);
}

template <typename T>
using DirectedGraph = Graph<T, true>;

template <typename T>
using UndirectedGraph = Graph<T, false>;
