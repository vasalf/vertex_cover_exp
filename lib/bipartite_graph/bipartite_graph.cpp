#include "bipartite_graph.h"

BipartiteGraph::BipartiteGraph(int n, int m) : m(m), adj(n) {}

BipartiteGraph::BipartiteGraph(const ProblemInstance& graph) {
    adj.resize(graph.realSize());
    m = graph.realSize();

    for (int u : graph.undecided()) {
        for (int v : graph.adjacent(u)) {
            addEdge(u, v);
        }
    }
}

void BipartiteGraph::addEdge(int i, int j) {
    adj[i].push_back(j);
}

const std::vector<int> BipartiteGraph::adjacent(int v) const {
    return adj[v];
}

int BipartiteGraph::leftSize() const {
    return adj.size();
}

int BipartiteGraph::rightSize() const {
    return m;
}
