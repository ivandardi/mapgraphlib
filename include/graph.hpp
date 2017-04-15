#pragma once

#include "declarations.hpp"

#include <algorithm>
#include <array>
#include <cassert>
#include <iostream>
#include <list>
#include <numeric>
#include <vector>

namespace Direction {

enum class Direction : std::uint8_t {
    Outgoing = 0,
    Ingoing = 1
};

Direction opposite(Direction direction)
{
    switch (direction) {
    case Direction::Outgoing:
        return Direction::Ingoing;
    case Direction::Ingoing:
        return Direction::Outgoing;
    }
}

constexpr std::size_t index(Direction direction)
{
    return static_cast<std::size_t>(direction);
}
}



template <typename Ix>
struct NodeIndex
{
    using index_t = Ix;

    NodeIndex(Ix index = std::numeric_limits<Ix>::max())
    : _index(index)
    {
    }

    std::size_t index() const
    {
        return _index;
    }

    static NodeIndex end()
    {
        return {std::numeric_limits<Ix>::max()};
    }

    EdgeIndex<Ix> _into_edge() const
    {
        return {_index};
    }

    Ix _index;
};

template <typename Ix>
struct EdgeIndex
{
    EdgeIndex(Ix index = std::numeric_limits<Ix>::max())
    : _index(index)
    {
    }

    std::size_t index() const
    {
        return _index;
    }

    static EdgeIndex end()
    {
        return {std::numeric_limits<Ix>::max()};
    }

    NodeIndex<Ix> _into_edge() const
    {
        return {_index};
    }

    Ix _index;
};

template <typename N, typename Ix>
struct Node
{

    Node(N const& weight = {})
    : weight(weight)
    , next({EdgeIndex<Ix>::end(), EdgeIndex<Ix>::end()})
    {
    }

    EdgeIndex<Ix> next_edge(Direction::Direction dir) const
    {
        return next[Direction::index(dir)];
    }

    N weight;
    std::array<EdgeIndex<Ix>, 2> next;
};

template <typename E, typename Ix>
struct Edge
{

    Edge(std::array<NodeIndex<Ix>, 2> node, E const& weight = {})
    : weight(weight)
    , next({EdgeIndex<Ix>::end(), EdgeIndex<Ix>::end()})
    , node(node)
    {
    }

    EdgeIndex<Ix> next_edge(Direction::Direction dir) const
    {
        return next[Direction::index(dir)];
    }

    EdgeIndex<Ix> source() const
    {
        return node[0];
    }

    EdgeIndex<Ix> target() const
    {
        return node[1];
    }

    E weight;
    std::array<EdgeIndex<Ix>, 2> next;
    std::array<NodeIndex<Ix>, 2> node;
};

template <typename N, typename E, bool directed, typename Ix>
struct Graph
{

    Graph()
    : nodes()
    , edges()
    {
    }

    Graph(std::size_t nodes, std::size_t edges)
    : nodes(nodes)
    , edges(edges)
    {
    }

    void clear()
    {
        nodes.clear();
        edges.clear();
    }

    std::size_t node_count() const
    {
        return nodes.size();
    }

    std::size_t edge_count() const
    {
        return edges.size();
    }

    bool is_directed() const
    {
        return directed;
    }

    NodeIndex<Ix> add_node(N const& weight)
    {
        auto const node_idx = NodeIndex<Ix>(nodes.size());
        nodes.push_back(Node<N, Ix>(weight));
        return node_idx;
    }

    N const& node_weight(NodeIndex<Ix> a) const
    {
        return nodes[a.index()].weight;
    }

    N& node_weight(NodeIndex<Ix> a)
    {
        return nodes[a.index()].weight;
    }

    auto add_edge(NodeIndex<Ix> a, NodeIndex<Ix> b, E const& weight) -> EdgeIndex<Ix>
    {
        auto const edge_idx = EdgeIndex<Ix>(edges.size());
        auto edge = Edge<E, Ix>({{a, b}}, weight);

        if (a.index() == b.index()) {
            auto& an = nodes.at(a.index());
            edge.next = an.next;
            an.next[0] = an.next[1] = edge_idx;
        }
        else {
            auto& an = nodes.at(a.index());
            auto& bn = nodes.at(b.index());
            edge.next = {{an.next[0], bn.next[1]}};
            an.next[0] = edge_idx;
            bn.next[1] = edge_idx;
        }
        edges.push_back(edge);
        return edge_idx;
    }

    E const& edge_weight(EdgeIndex<Ix> e) const
    {
        return edges.at(e.index()).weight;
    }

    E& edge_weight(EdgeIndex<Ix> e)
    {
        return edges.at(e.index()).weight;
    }

    std::pair<NodeIndex<Ix>, NodeIndex<Ix>> edge_endpoints(EdgeIndex<Ix> e) const
    {
        auto const& ed = edges.at(e.index());
        return std::make_pair(ed.source(), ed.target());
    }

    std::vector<Node<N, Ix>> nodes;
    std::vector<Edge<N, Ix>> edges;
};
