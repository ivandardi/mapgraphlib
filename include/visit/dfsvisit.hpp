#pragma once

#include "declarations.hpp"

#include <cstdint>

template <typename N>
struct DfsEvent {

    enum class Kind {
        Discover,
        TreeEdge,
        BackEdge,
        CrossForwardEdge,
        Finish,
    } kind;

    union {
        struct discover {
            N source;
            std::size_t time;
        };
        struct tree_edge {
            N source;
            N target;
        };
        struct back_edge {
            N source;
            N target;
        };
        struct cross_forward_edge {
            N source;
            N target;
        };
        struct finish {
            N source;
            std::size_t time;
        };
    };

};

template <typename T>
struct ControlFlow {
    enum class Kind {
        Continue,
        Break
    } kind;
    T data;

    ControlFlow(Kind kind, T const& data = {})
    : kind(kind)
    , data(data)
    {}
};

template <typename N, typename E, bool directed, typename Ix, typename F>
void depth_first_search(Graph<N, E, directed, Ix>& graph, NodeIndex<Ix> start, F&& visitor)
{
    std::size_t time = 0;
    std::map<Ix, bool> discovered;
    std::map<Ix, bool> finished;
    dfs_visitor(graph, start, visitor, discovered, finished, time);
}

#define TRY_CONTROL(expr) do {                           \
auto TRY_CONTROL_RESULT = expr;                          \
if (TRY_CONTROL_RESULT.kind == ControlFlow::Kind::Break) \
    return TRY_CONTROL_RESULT;                           \
} while (0);

template <typename N, typename E, bool directed, typename Ix, typename F, typename T>
ControlFlow<T> dfs_visitor(Graph<N, E, directed, Ix>& graph, NodeIndex<Ix> u, F&& visitor,
                 std::map<Ix, bool>& discovered, std::map<Ix, bool>& finished, std::size_t& time)
{
    if (!visit(discovered, u)) {
        return ControlFlow(ControlFlow::Kind::Continue);
    }
    auto event = DfsEvent();
    event.kind = DfsEvent::Kind::Discover;
    event.discover.source = u;
    event.discover.time = time++;
    TRY_CONTROL(visitor(event));
}

template <typename Ix>
bool visit(std::map<Ix, bool>& discovered_finished, NodeIndex<Ix> node)
{
    if (discovered_finished.find(node.index()) != discovered_finished.end()) {
        discovered_finished[node.index()] = true;
        return true;
    }
    return false;
}
