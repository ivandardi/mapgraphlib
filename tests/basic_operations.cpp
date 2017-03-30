#include "graph.hpp"
#include <algorithm>
#include <catch.hpp>
#include <cstddef>
#include <iostream>
#include <numeric>
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

SCENARIO("Basic Operations", "[basic-operations]")
{

    GIVEN("An undirected graph with 5 vertices")
    {

        UndirectedGraph<int> graph;
        std::unordered_map<int, UndirectedGraph<int>::iterator> vertice_tags;

        const int num_vertices = 5;

        for (int i = 0; i < num_vertices; ++i) {
            vertice_tags[i] = graph.insert(i);
        }

        WHEN("The graph is complete")
        {

            for (int i = 0; i < num_vertices - 1; ++i) {
                for (int j = i + 1; j < num_vertices; ++j) {
                    graph.add_edge(vertice_tags[i], vertice_tags[j]);
                }
            }

            THEN("The number of edges must be (n*(n-1))/2")
            {
                REQUIRE((num_vertices * (num_vertices - 1)) / 2 == graph.num_edges());
            }

            THEN("All vertices must have degree n-1")
            {
                for (auto&& elem : vertice_tags) {
                    REQUIRE((num_vertices - 1) == graph.degree(elem.second));
                }
            }
        }

        WHEN("The graph is emptied")
        {

            graph.clear();

            THEN("The number of vertices and edges should be 0")
            {
                REQUIRE(0 == graph.num_vertices());
                REQUIRE(0 == graph.num_edges());
            }
        }
    }

    GIVEN("A directed graph with 5 vertices")
    {

        DirectedGraph<int> graph;
        std::unordered_map<int, DirectedGraph<int>::iterator> vertice_tags;

        const int num_vertices = 5;

        for (int i = 0; i < num_vertices; ++i) {
            vertice_tags[i] = graph.insert(i);
        }

        WHEN("The graph is complete")
        {

            for (int i = 0; i < num_vertices - 1; ++i) {
                for (int j = i + 1; j < num_vertices; ++j) {
                    graph.add_edge(vertice_tags[i], vertice_tags[j]);
                }
            }

            size_t out_degree = 0;
            size_t in_degree = 0;
            for (DirectedGraph<int>::iterator it = graph.begin(); it != graph.end(); ++it) {
                out_degree += graph.out_degree(it);
                in_degree += graph.in_degree(it);
            }

            THEN("The sum of indegrees must be equal to the sum of outdegrees")
            {
                REQUIRE(in_degree == out_degree);
            }

            THEN("The sum of the indegrees must be equal to the number of edges")
            {
                REQUIRE(in_degree == 0);
            }

            THEN("The sum of the outdegrees must be equal to the number of edges")
            {
                REQUIRE(out_degree == 0);
            }
        }

        WHEN("The graph is emptied")
        {

            graph.clear();

            THEN("The number of vertices and edges should be 0")
            {
                REQUIRE(0 == graph.num_vertices());
                REQUIRE(0 == graph.num_edges());
            }
        }
    }
}
