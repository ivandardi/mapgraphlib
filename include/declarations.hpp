#pragma once

#include <cstdint>

using DefaultIx = std::uint32_t;



// graph.hpp

template <typename Ix = DefaultIx>
struct EdgeIndex;

template <typename Ix = DefaultIx>
struct NodeIndex;

template <typename N, typename Ix = DefaultIx>
struct Node;

template <typename E, typename Ix = DefaultIx>
struct Edge;

template <typename N, typename E = char, bool directed = true, typename Ix = DefaultIx>
struct Graph;

template <typename N, typename E, typename Ix = DefaultIx>
using DiGraph = Graph<N, E, true, Ix>;

template <typename N, typename E, typename Ix = DefaultIx>
using UnGraph = Graph<N, E, false, Ix>;
