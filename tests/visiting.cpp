#include "graph.hpp"
#include <catch.hpp>
#include <map>

SCENARIO("Visiting", "[visiting]")
{

    GIVEN("An undirected graph")
    {

        UnGraph<int, int> graph;
        std::map<int, NodeIndex<DefaultIx>> vertice_tags;

        const int num_vertices = 6;

        for (int i = 0; i < num_vertices; ++i) {
            vertice_tags[i] = graph.add_node(i);
        }

        WHEN("The graph is complete")
        {

            for (int i = 0; i < num_vertices - 1; ++i) {
                for (int j = i + 1; j < num_vertices; ++j) {
                    graph.add_edge(vertice_tags[i], vertice_tags[j], 0);
                }
            }

            THEN("The number of edges must be (n*(n-1))/2")
            {
                REQUIRE((num_vertices * (num_vertices - 1)) / 2 == graph.edge_count());
            }

        }

        WHEN("The graph is emptied")
        {

            graph.clear();

            THEN("The number of vertices and edges should be 0")
            {
                REQUIRE(0 == graph.node_count());
                REQUIRE(0 == graph.edge_count());
            }
        }
    }

    GIVEN("A directed graph with 8 vertices")
    {

        DiGraph<int, int> graph;
        std::map<int, NodeIndex<DefaultIx>> vertice_tags;

        const int num_vertices = 8;

        for (int i = 0; i < num_vertices; ++i) {
            vertice_tags[i] = graph.add_node(i);
        }

        WHEN("The graph is emptied")
        {

            graph.clear();

            THEN("The number of vertices and edges should be 0")
            {
                REQUIRE(0 == graph.node_count());
                REQUIRE(0 == graph.edge_count());
            }
        }

    }
}
