#pragma once

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <iostream>
#include <iterator>
#include <numeric>
#include <queue>
#include <stack>
#include <string>
#include <unordered_map>
#include <vector>

/// Graph class with a key-value interface implemented using adjacency "lists". It's actually unordered sets, so lookup
/// should be quicker.
///
/// \tparam K Key type. Can be any type accepted as a key on a `std::unordered_map`.
/// \tparam V Value type. Can be any type accepted as a value on a `std::unordered_map`.
/// \tparam EdgeCost Edge cost type. Not used yet, but in the future it will need to be a callable that takes a
///         pair of key-value pairs and returns the cost of the edge between them
template <typename K, typename V, typename EdgeCost>
class graph final
{
private:
    struct Edge;
    using vertex_list = std::unordered_map<K, V>;
    using edge_list   = std::unordered_map<K, std::vector<Edge>>;
    using self        = graph<K, V, EdgeCost>;

public:
    using key_type    = typename vertex_list::key_type;
    using value_type  = typename vertex_list::value_type;
    using mapped_type = typename vertex_list::mapped_type;
    using key_equal   = typename vertex_list::key_equal;

    using pointer         = typename vertex_list::pointer;
    using const_pointer   = typename vertex_list::const_pointer;
    using reference       = typename vertex_list::reference;
    using const_reference = typename vertex_list::const_reference;
    using iterator        = typename vertex_list::iterator;
    using const_iterator  = typename vertex_list::const_iterator;
    using size_type       = typename vertex_list::size_type;
    using difference_type = typename vertex_list::difference_type;

    using distance_map = std::unordered_map<key_type, size_type>;
    using parent_map   = std::unordered_map<key_type, const_iterator>;
    using times_map    = std::unordered_map<key_type, std::pair<size_type, size_type>>;

    static const std::string::size_type npos = std::string::npos;

private:
    /// Edge struct.
    struct Edge {
        Edge(const_iterator to, EdgeCost const& cost = {})
        : _to(to)
        , _cost(cost)
        {
        }

        const_iterator _to; ///< Vertex that the edge points to.
        EdgeCost _cost; ///< Callable that returns the cost of the edge.
    };

public:
    /// Copy-assign constructor. If failure happens when assigning, it is guaranteed that the state won't be altered.
    ///
    /// # Complexity
    /// Linear in the size of *this and other.
    ///
    /// \param other The graph to be copied
    /// \return *this
    graph& operator=(graph const& other)
    {
        if (*this == other) {
            return *this;
        }

        vertex_list tmp_vertices;
        edge_list tmp_edges;

        try {
            tmp_edges = other._vertices;
            tmp_edges = other._edges;
        } catch (...) {
            throw;
        }

        _vertices = std::move(tmp_vertices);
        _edges = std::move(tmp_edges);

        return *this;
    }

    /// Move-assign constructor. If failure happens when assigning, it is guaranteed that the state won't be altered.
    ///
    /// # Complexity
    /// Linear in the size of *this and other.
    ///
    /// \param other The graph to be moved
    /// \return *this
    graph& operator=(graph&& other)
    {
        if (*this == other) {
            return *this;
        }

        using std::swap;

        vertex_list tmp_vertices;
        edge_list tmp_edges;

        try {
            swap(tmp_edges, other._vertices);
            swap(tmp_edges, other._edges);
        } catch (...) {
            throw;
        }

        swap(_vertices, tmp_vertices);
        swap(_edges, tmp_edges);

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
        _edges.clear();
        return *this;
    }

public:

    /// Returns an iterator to the first element of the container. If the container is empty, the returned iterator will
    /// be equal to `end()`.
    ///
    /// # Complexity
    /// Constant.
    ///
    /// \return Iterator to the first element
    iterator begin() noexcept
    {
        return _vertices.begin();
    }


    /// Returns an iterator to the first element of the container. If the container is empty, the returned iterator will
    /// be equal to `end()`.
    ///
    /// # Complexity
    /// Constant.
    ///
    /// \return Iterator to the first element
    const_iterator begin() const noexcept
    {
        return _vertices.begin();
    }


    /// Returns an iterator to the first element of the container. If the container is empty, the returned iterator will
    /// be equal to `end()`.
    ///
    /// # Complexity
    /// Constant.
    ///
    /// \return Iterator to the first element
    const_iterator cbegin() const noexcept
    {
        return _vertices.cbegin();
    }

    /// Returns an iterator to the element following the last element of the container. This element acts as a
    /// placeholder; attempting to access it results in undefined behavior.
    ///
    /// # Complexity
    /// Constant.
    ///
    /// \return Iterator to the element following the last element
    iterator end() noexcept
    {
        return _vertices.end();
    }

    /// Returns an iterator to the element following the last element of the container. This element acts as a
    /// placeholder; attempting to access it results in undefined behavior.
    ///
    /// # Complexity
    /// Constant.
    ///
    /// \return Iterator to the element following the last element
    const_iterator end() const noexcept
    {
        return _vertices.end();
    }

    /// Returns an iterator to the element following the last element of the container. This element acts as a
    /// placeholder; attempting to access it results in undefined behavior.
    ///
    /// # Complexity
    /// Constant.
    ///
    /// \return Iterator to the element following the last element
    const_iterator cend() const noexcept
    {
        return _vertices.cend();
    }

    /// Checks if the container has no elements, i.e. whether `begin() == end()`.
    ///
    /// # Complexity
    /// Constant.
    ///
    /// \return `true` if the container is empty, false otherwise
    bool empty() const noexcept
    {
        return _vertices.empty() && _edges.empty();
    }

    /// Returns the number of vertices and edges in the graph.
    ///
    /// # Complexity
    /// Linear in the amount of edges.
    ///
    /// \return (V, E) pair where V is the number of vertices and E is the number of edges
    std::pair<size_type, size_type> size() const noexcept
    {
        using std::make_pair;
        return make_pair(num_vertices(), num_edges());
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
    /// Linear in the amount of edges.
    ///
    /// \return the number of edges in the graph
    size_type num_edges() const noexcept
    {
        using std::accumulate;
        using std::cbegin;
        using std::cend;

        return accumulate(cbegin(_edges), cend(_edges),
                          static_cast<size_type>(0),
                          [](size_type s, auto const& v) {
                              return s + v.second.size();
                          });
    }

    /// Removes all elements from the container. Invalidates any references, pointers, or iterators referring to
    /// contained elements. May also invalidate past-the-end iterators.
    ///
    /// # Complexity
    /// Linear in the size of the graph.
    ///
    void clear() noexcept
    {
        _vertices.clear();
        _edges.clear();
    }

    /// Inserts element(s) into the container, if the container doesn't already contain an element with an equivalent
    /// key.
    ///
    /// # Complexity
    /// Average case: O(1), worst case linear in the amount of vertices
    ///
    /// \param value element value to insert
    void insert(value_type const& value) noexcept
    {
        _vertices.insert(value);
    }
    /// Inserts element(s) into the container, if the container doesn't already contain an element with an equivalent
    /// key.
    ///
    /// # Complexity
    /// Average case: O(1), worst case linear in the amount of vertices
    ///
    /// \param value element value to insert
    void insert(value_type&& value) noexcept
    {
        _vertices.insert(std::move(value));
    }

    /// Inserts a new element into the container constructed in-place with the given args if there is no element with
    /// the key in the container.
    ///
    /// # Complexity
    /// Amortized constant on average, worst case linear in the amount of vertices.
    ///
    /// # Exceptions
    /// If an exception is thrown by any operation, this function has no effect.
    ///
    /// \param args arguments to forward to the constructor of the element
    /// \return A pair consisting of an iterator to the inserted element, or the already-existing element if no
    ///         insertion happened, and a bool denoting whether the insertion took place. True for Insertion, False for
    ///         No Insertion.
    template <typename... Args>
    std::pair<iterator, bool> emplace(Args&&... args)
    {
        return _vertices.emplace(std::forward<Args>(args)...);
    };

    /// Removes the element at pos. References and iterators to the erased elements are invalidated. Other iterators and
    /// references are not invalidated. The iterator pos must be valid and dereferenceable. Thus the `end()` iterator
    /// (which is valid, but is not dereferencable) cannot be used as a value for pos.
    ///
    /// # Complexity
    /// Average case constant, worst case linear in the amount of edges
    ///
    /// \param pos iterator to the element to remove
    void erase(const_iterator pos) noexcept
    {
        using std::remove_if;
        using std::begin;
        using std::end;

        auto edge_ptr = _edges.find(pos->first);

        for (auto& v : _edges) {
            v.second.erase(remove_if(begin(v.second), end(v.second),
                                     [edge_ptr](Edge const& edge) {
                                         return edge._to == edge_ptr;
                                     }),
                           end(v.second));
        }

        _edges.erase(pos->first);
        _vertices.erase(pos);
    }


    /// Removes the element (if one exists) with the key equivalent to key. References and iterators to the erased
    /// elements are invalidated. Other iterators and references are not invalidated. The iterator pos must be valid and
    /// dereferenceable. Thus the `end()` iterator (which is valid, but is not dereferencable) cannot be used as a value
    /// for pos.
    ///
    /// # Complexity
    /// Average case constant, worst case linear in the amount of edges
    ///
    /// \param key key value of the element to remove
    void erase(key_type const& key) noexcept
    {
        erase(const_iterator(_vertices.find(key)));
    }

    /// Exchanges the contents of the container with those of other. Does not invoke any move, copy, or swap operations
    /// on individual elements. All iterators and references remain valid. The past-the-end iterator is invalidated.
    ///
    /// # Complexity
    /// Constant.
    ///
    /// # Exceptions
    /// Any exception thrown by the swap Hash or KeyEqual objects.
    ///
    /// \param other container to exchange the contents with
    void swap(graph& other)
    {
        _vertices.swap(other._vertices);
        _edges.swap(other._edges);
    }

    /// Returns a reference to the mapped value of the element with key equivalent to key. If no such element exists, an
    /// exception of type std::out_of_range is thrown.
    ///
    /// # Complexity
    /// Average case constant, worst case linear in the amount of vertices
    ///
    /// # Exceptions
    /// `std::out_of_range` if the container does not have an element with the specified key
    ///
    /// \param key the key of the element to find
    /// \return Reference to the mapped value of the requested element
    mapped_type& at(key_type const& key)
    {
        return _vertices.at(key);
    }

    /// Returns a reference to the mapped value of the element with key equivalent to key. If no such element exists, an
    /// exception of type std::out_of_range is thrown.
    ///
    /// # Complexity
    /// Average case constant, worst case linear in the amount of vertices
    ///
    /// # Exceptions
    /// `std::out_of_range` if the container does not have an element with the specified key
    ///
    /// \param key the key of the element to find
    /// \return Reference to the mapped value of the requested element
    const mapped_type& at(key_type const& key) const
    {
        return _vertices.at(key);
    }

    /// Returns a reference to the value that is mapped to a key equivalent to key, performing an insertion if such key
    /// does not already exist.
    ///
    /// # Complexity
    /// Average case constant, worst case linear in the amount of vertices
    ///
    /// # Exceptions
    /// If an exception is thrown by any operation, the insertion has no effect
    ///
    /// \param k the key of the element to find
    /// \return Reference to the mapped value of the new element if no element with key key existed. Otherwise a
    ///         reference to the mapped value of the existing element whose key is equivalent to key.
    mapped_type& operator[](const key_type& k) noexcept
    {
        return _vertices[k];
    }

    /// Returns a reference to the value that is mapped to a key equivalent to key, performing an insertion if such key
    /// does not already exist.
    ///
    /// # Complexity
    /// Average case constant, worst case linear in the amount of vertices
    ///
    /// # Exceptions
    /// If an exception is thrown by any operation, the insertion has no effect
    ///
    /// \param k the key of the element to find
    /// \return Reference to the mapped value of the new element if no element with key key existed. Otherwise a
    ///         reference to the mapped value of the existing element whose key is equivalent to key.
    mapped_type& operator[](key_type&& k) noexcept
    {
        return _vertices[std::move(k)];
    }

    /// Returns the number of elements with key that compares equal to the specified argument key, which is either 1 or
    /// 0 since this container does not allow duplicates.
    ///
    /// # Complexity
    /// Average case constant, worst case linear in the amount of vertices
    ///
    /// \param key key value of the elements to count
    /// \return Number of elements with key key, that is either 1 or 0.
    size_type count(key_type const& key) const
    {
        return _vertices.count(key);
    }

    /// Finds an element with key equivalent to key.
    ///
    /// # Complexity
    /// Average case constant, worst case linear in the amount of vertices
    ///
    /// \param key key value of the element to search for
    /// \return Iterator to an element with key equivalent to key. If no such element is found, past-the-end (see
    ///         `end()`) iterator is returned.
    iterator find(key_type const& key) noexcept
    {
        return _vertices.find(key);
    }

    /// Finds an element with key equivalent to key.
    ///
    /// # Complexity
    /// Average case constant, worst case linear in the amount of vertices
    ///
    /// \param key key value of the element to search for
    /// \return Iterator to an element with key equivalent to key. If no such element is found, past-the-end (see
    ///         `end()`) iterator is returned.
    const_iterator find(key_type const& key) const noexcept
    {
        return _vertices.find(key);
    }

    /// Returns a vector of `Edges` of a vertex with key key.
    ///
    /// # Complexity
    /// Average case constant, worst case linear in the amount of vertices
    ///
    /// \param key key value of the element to search for
    /// \return a vector of `Edges` of a vertex with key key
    std::vector<Edge> edges(key_type const& key) const noexcept
    {
        return edges(find(key));
    }

    /// Returns a vector of `Edges` of a vertex.
    ///
    /// # Complexity
    /// Average case constant, worst case linear in the amount of vertices
    ///
    /// \param vertex the vertex to get the list of edges from
    /// \return a vector of `Edges` of a vertex with key key
    std::vector<Edge> const& edges(const_iterator vertex) const noexcept
    {
        return _edges[vertex->first];
    }

    /// Returns a list of vertices of the graph.
    ///
    /// # Complexity
    /// Linear in the amount of vertices
    ///
    /// \return the list of vertices of the graph
    vertex_list const& vertices() const noexcept
    {
        return _vertices;
    }

    /// Adds an undirected edge (a, b) from the vertex with key a to the vertex with key b.
    ///
    /// # Complexity
    /// Linear in the amount of vertices
    ///
    /// \param a key of the first vertex
    /// \param b key of the second vertex
    /// \param cost callable object that returns a value which describes the cost of the edge
    void add_undirected_edge(key_type const& a,
                             key_type const& b,
                             EdgeCost const& cost = {}) noexcept
    {
        auto u = _vertices.find(a);
        auto v = _vertices.find(b);
        if (u != _vertices.end() && v != _vertices.end()) {
            _edges[u->first].emplace_back(v, cost);
            _edges[v->first].emplace_back(u, cost);
        }
    }

    /// Adds a directed edge (a, b) from the vertex with key a to the vertex with key b.
    ///
    /// # Complexity
    /// Linear in the amount of vertices
    ///
    /// \param a key of the first vertex
    /// \param b key of the second vertex
    /// \param cost callable object that returns a value which describes the cost of the edge
    void add_directed_edge(key_type const& a,
                           key_type const& b,
                           EdgeCost const& cost = {}) noexcept
    {
        auto u = _vertices.find(a);
        auto v = _vertices.find(b);
        if (u != _vertices.end() && v != _vertices.end()) {
            _edges[u->first].emplace_back(v, cost);
        }
    }

    /// Performs a breadth-first search on the graph from the source vertex.
    ///
    /// # Complexity
    /// Linear in the size of the graph (number of edges + number of vertices)
    ///
    /// \param source source vertex to start the search
    /// \return an `std::pair<parent_map, distance_map>`, where the parent map denotes from which vertex the vertex was
    ///         reached, and the distance map denotes the number of edges from the source. If the vertex is unreachable,
    ///         the value is `npos`.
    auto BFS(const_iterator source) const
    {
        parent_map parent;
        distance_map distance;
        std::queue<const_iterator> q;

        parent[source->first] = cend();
        distance[source->first] = 0;
        q.push(source);

        while (!q.empty()) {
            auto curr = q.front();
            q.pop();
            for (auto const& edge : _edges.at(curr->first)) {
                auto adj_key = edge._to->first;
                if (!distance.count(adj_key)) {
                    distance[adj_key] = distance[curr->first] + 1;
                    parent[adj_key] = curr;
                    q.push(edge._to);
                }
            }
        }

        for (auto const& it : _vertices) {
            if (!distance.count(it.first)) {
                distance[it.first] = npos;
            }
        }

        return std::make_pair(parent, distance);
    }

    /// Performs a depth-first search on the graph from the source vertex.
    ///
    /// # Complexity
    /// Linear in the size of the graph (number of edges + number of vertices)
    ///
    /// \param source source vertex to start the search
    /// \return an `std::pair<parent_map, times_map>`, where the parent map denotes from which vertex the vertex was
    ///         reached, and the times map denotes the time in the traversal when the edge was reached and the time when
	///         it finished being explored.  If the vertex is unreachable, the value is a pair of `npos`.
    auto DFS(const_iterator source) const
    {
        parent_map parent;
        times_map times;
        size_type time = 0;

        parent[source->first] = cend();
        DFS(source, parent, times, time);

        for (auto const& it : _vertices) {
            if (!times.count(it.first)) {
                times[it.first] = std::make_pair(npos, npos);
            }
        }

        return std::make_pair(parent, times);
    }

	/// For debug purposes
    template <typename U, typename X>
    friend std::ostream& operator<<(std::ostream& out, graph<U, X>& g);

private:
    void DFS(const_iterator src,
             parent_map& parent,
             times_map& times,
             size_type& time) const
    {
        times[src->first].first = time++;
        for (auto const& edge : _edges.at(src->first)) {
            auto adj_key = edge._to->first;
            if (!parent.count(adj_key)) {
                parent[adj_key] = src;
                DFS(find(adj_key), parent, times, time);
            }
        }
        times[src->first].second = time++;
    }

private:
    vertex_list _vertices; ///< The vertices of the graph
    edge_list _edges; ///< The edges of the graph, and what actually stores the graph itself
};

template <typename K, typename V, typename EdgeCost>
inline bool
operator==(graph<K, V, EdgeCost> const& lhs, graph<K, V, EdgeCost> const& rhs)
{
    return lhs._vertices == rhs._vertices && lhs._edges == rhs._edges;
}

template <typename K, typename V, typename EdgeCost>
inline bool
operator!=(graph<K, V, EdgeCost> const& lhs, graph<K, V, EdgeCost> const& rhs)
{
    return !(lhs == rhs);
}
