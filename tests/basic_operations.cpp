#include "Graph.hpp"
#include <catch.hpp>
#include <utility>
#include <iostream>


template <typename U, typename X>
std::ostream&
operator<<(std::ostream& out, graph<U, X>& g)
{
	for (const auto& v : g._vertices) {
		out << v.first << " ->";
		for (const auto& u : g._edges[v.first]) {
			out << ' ' << u._to->first;
		}
		out << '\n';
	}
	return out;
}

TEST_CASE("Basic Operations", "[basic-operations]")
{
	graph<int, int> g1;

	SECTION("insertion")
	{
		REQUIRE(g1.num_vertices() == 0);
		REQUIRE(g1.num_edges() == 0);

		constexpr int inserted = 5;
		for (int i = 0; i < inserted; ++i) {
			g1.insert(std::make_pair(i, i));
		}

		REQUIRE(g1.num_vertices() == inserted);
		REQUIRE(g1.num_edges() == 0);

		std::cout << g1 << '\n';

		SECTION("find")
		{
			for (int i = 0; i < inserted; ++i) {
				REQUIRE(g1.find(i) != g1.end());
			}

			for (int i = inserted; i < inserted + 5; ++i) {
				REQUIRE(g1.find(i) == g1.end());
			}
		}

	}
}

TEST_CASE("Searches", "[search-breadth],[search-depth],[search-djikstra]")
{
	graph<int, int> g1;

	constexpr int inserted = 8;
	for (int i = 0; i < inserted; ++i) {
		g1.insert(std::make_pair(i, i));
	}

	SECTION("Undirected Edges") {

		SECTION("Complete Graph") {

			g1.add_undirected_edge(0, 1);
			g1.add_undirected_edge(0, 4);
			g1.add_undirected_edge(1, 5);
			g1.add_undirected_edge(2, 3);
			g1.add_undirected_edge(2, 5);
			g1.add_undirected_edge(2, 6);
			g1.add_undirected_edge(3, 6);
			g1.add_undirected_edge(3, 7);
			g1.add_undirected_edge(5, 6);
			g1.add_undirected_edge(6, 7);

			SECTION("BFS") {

				auto bfs = g1.BFS(g1.find(1));

				REQUIRE(bfs.first[0] == g1.find(1));
				REQUIRE(bfs.first[1] == g1.cend());
				REQUIRE(bfs.first[2] == g1.find(5));
				REQUIRE(bfs.first[3] == g1.find(2));
				REQUIRE(bfs.first[4] == g1.find(0));
				REQUIRE(bfs.first[5] == g1.find(1));
				REQUIRE(bfs.first[6] == g1.find(5));
				REQUIRE(bfs.first[7] == g1.find(6));

				REQUIRE(bfs.second[0] == 1);
				REQUIRE(bfs.second[1] == 0);
				REQUIRE(bfs.second[2] == 2);
				REQUIRE(bfs.second[3] == 3);
				REQUIRE(bfs.second[4] == 2);
				REQUIRE(bfs.second[5] == 1);
				REQUIRE(bfs.second[6] == 2);
				REQUIRE(bfs.second[7] == 3);

			}

			SECTION("DFS") {

				auto dfs = g1.DFS(g1.find(1));

				REQUIRE(dfs.first[0] == g1.find(1));
				REQUIRE(dfs.first[1] == g1.cend());
				REQUIRE(dfs.first[2] == g1.find(5));
				REQUIRE(dfs.first[3] == g1.find(2));
				REQUIRE(dfs.first[4] == g1.find(0));
				REQUIRE(dfs.first[5] == g1.find(1));
				REQUIRE(dfs.first[6] == g1.find(3));
				REQUIRE(dfs.first[7] == g1.find(6));

			}
		}

	}

}
