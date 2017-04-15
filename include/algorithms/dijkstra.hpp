#pragma once

#include "graph.hpp"

#include <map>
#include <set>
#include <utility>

/// F must return something convertible to double
template <typename T, bool is_directed, typename F>
std::pair<std::map<typename Graph<T, is_directed>::iterator, double>, std::map<typename Graph<T, is_directed>::iterator, typename Graph<T, is_directed>::iterator>>
dijkstra(Graph<T, is_directed>& g, Graph<T, is_directed>::iterator source, F&& weight)
{
    using weight_t = double;
    using graph_t = Graph<T, is_directed>;
    using iterator_t = typename graph_t::iterator;
    using value = std::pair<iterator_t, weight_t>;

    auto compare = [](value const& lhs, value const& rhs){
        return lhs.second < rhs.second;
    };

    std::multiset<value, compare> q;

    std::map<iterator_t, weight_t> distance{{source, 0}};
    std::map<iterator_t, iterator_t> parent{{source, g.end()}};

    q.push(std::make_pair(source, 0));
    while (!q.empty()) {
        auto current = q.front();
        q.pop();
        for (auto&& it : g.adjacent_vertices_of(current.first)) {
            weight_t new_cost = current.second + F(current, it);
            if (new_cost < distance[it]) {
                distance[it] = new_cost;
                parent[it] = current;
                q.push(it);
            }
        }
    }

    return std::make_pair(distance, parent);
}
