#pragma once

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <iostream>
#include <iterator>
#include <numeric>
#include <queue>
#include <string>
#include <stack>
#include <unordered_map>
#include <vector>

template <typename K, typename V, typename EdgeCost = int>
class graph
{
	struct Edge;
	using vertex_list = std::unordered_map<K, V>;
	using edge_list = std::unordered_map<K, std::vector<Edge>>;
	using self = graph<K, V, EdgeCost>;

public:
	using key_type = typename vertex_list::key_type;
	using value_type = typename vertex_list::value_type;
	using mapped_type = typename vertex_list::mapped_type;
	using key_equal = typename vertex_list::key_equal;

	using pointer = typename vertex_list::pointer;
	using const_pointer = typename vertex_list::const_pointer;
	using reference = typename vertex_list::reference;
	using const_reference = typename vertex_list::const_reference;
	using iterator = typename vertex_list::iterator;
	using const_iterator = typename vertex_list::const_iterator;
	using size_type = typename vertex_list::size_type;
	using difference_type = typename vertex_list::difference_type;

	using distance_map = std::unordered_map<key_type, size_type>;
	using parent_map = std::unordered_map<key_type, const_iterator>;
	using times_map = std::unordered_map<key_type, std::pair<size_type, size_type>>;

	static const std::string::size_type npos = std::string::npos;

private:
	struct Edge {
		Edge(const_iterator to, EdgeCost const& cost = {})
		: _to(to)
		, _cost(cost)
		{
		}

		const_iterator _to;
		EdgeCost _cost;
	};

public:
	graph& operator=(graph const& other)
	{
		_vertices = other._vertices;
		_edges = other._edges;
		return *this;
	}

	graph& operator=(graph&& other)
	{
		_vertices = std::move(other._vertices);
		_edges = std::move(other._edges);
		return *this;
	}

	graph& operator=(std::initializer_list<value_type> ilist)
	{
		_vertices = ilist;
		_edges.clear();
		return *this;
	}

	iterator begin() noexcept
	{
		return _vertices.begin();
	}

	iterator end() noexcept
	{
		return _vertices.end();
	}

	const_iterator cbegin() const noexcept
	{
		return _vertices.cbegin();
	}

	const_iterator cend() const noexcept
	{
		return _vertices.cend();
	}

	bool empty() const noexcept
	{
		return _vertices.empty() && _edges.empty();
	}

	/**
	 *
	 * @return (V, E) pair where V is the amount of vertices and E is the amount
	 * of edges
	 */
	std::pair<size_type, size_type> size() const noexcept
	{
		using std::make_pair;
		return make_pair(num_vertices(), num_edges());
	}

	size_type num_vertices() const noexcept
	{
		return _vertices.size();
	}

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

	void clear() noexcept
	{
		_vertices.clear();
		_edges.clear();
	}

	void insert(value_type const& value) noexcept
	{
		_vertices.insert(value);
	}

	void insert(value_type&& value) noexcept
	{
		_vertices.insert(std::move(value));
	}

	template <typename... Args>
	std::pair<iterator, bool> emplace(Args&&... args)
	{
		return _vertices.emplace(std::forward<Args>(args)...);
	};

	void erase(key_type const& key) noexcept
	{
		erase(const_iterator(_vertices.find(key)));
	}

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

	void swap(graph& other) noexcept
	{
		_vertices.swap(other._vertices);
		_edges.swap(other._edges);
	}

	mapped_type& at(key_type const& key)
	{
		return _vertices.at(key);
	}

	const mapped_type& at(key_type const& key) const
	{
		return _vertices.at(key);
	}

	mapped_type& operator[](const key_type& k) noexcept
	{
		return _vertices[k];
	}

	mapped_type& operator[](key_type&& k) noexcept
	{
		return _vertices[std::move(k)];
	}

	size_type count(key_type const& key) const
	{
		return _vertices.count(key);
	}

	iterator find(key_type const& key) noexcept
	{
		return _vertices.find(key);
	}

	const_iterator find(key_type const& key) const noexcept
	{
		return _vertices.find(key);
	}

	std::vector<Edge> edges(key_type const& key) const noexcept
	{
		return edges(find(key));
	}

	std::vector<Edge> const& edges(const_iterator vertex) const noexcept
	{
		return _edges[vertex->first];
	}

	vertex_list const& vertices() const noexcept
	{
		return _vertices;
	}

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

	auto DFS(const_iterator source) const
	{
		parent_map parent;
		times_map times;
		size_type time = 0;

		parent[source->first] = cend();
		DFS(source, parent, times, time);

		return std::make_pair(parent, times);
	}

	template <typename U, typename X>
	friend std::ostream& operator<<(std::ostream& out, graph<U, X>& g);

private:

	void DFS(const_iterator src, parent_map& parent, times_map& times, size_type& time) const
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
	vertex_list _vertices; ///< graph itself
	edge_list _edges;
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
