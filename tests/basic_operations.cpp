#include "graph.hpp"
#include <catch.hpp>
#include <iostream>
#include <unordered_map>
#include <utility>

template <typename T, bool is_directed>
std::ostream& operator<<(std::ostream& out, Graph<T, is_directed>& g)
{
    for (UndirectedGraph<int>::iterator it = g.begin(); it != g.end(); ++it) {
        out << *it << " ->";
        auto adj = g.adjacent_vertices_of(it);
        for (auto u : adj) {
            out << ' ' << *u;
        }
        out << '\n';
    }
    return out;
}

TEST_CASE("Basic Operations", "[basic-operations]")
{

    SECTION("edge_insertion")
    {
        UndirectedGraph<int> graph;
        std::unordered_map<int, UndirectedGraph<int>::iterator> vertice_tags;

        REQUIRE(graph.num_vertices() == 0);
        REQUIRE(graph.num_edges() == 0);

        constexpr int inserted = 5;
        for (int i = 0; i < inserted; ++i) {
            vertice_tags[i] = graph.insert(i);
        }

        REQUIRE(graph.num_vertices() == inserted);
        REQUIRE(graph.num_edges() == 0);

        std::cout << graph << '\n';
    }

    SECTION("edge_insertion")
    {
        UndirectedGraph<int> graph;

        REQUIRE(graph.num_vertices() == 0);
        REQUIRE(graph.num_edges() == 0);

        auto u = graph.insert(10);
        auto v = graph.insert(20);

        REQUIRE(graph.num_vertices() == 2);
        REQUIRE(graph.num_edges() == 0);

        graph.add_edge(u, v);

        REQUIRE(graph.num_vertices() == 2);
        REQUIRE(graph.num_edges() == 1);

        std::cout << graph << '\n';
    }
}
