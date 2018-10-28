#include "problem_instance.h"

ProblemInstance::ProblemInstance(int n) {
    vertices.resize(n, { Vertex::State::UNDECIDED });
    graph.resize(n);
    for (int i = 0; i < n; ++i) {
        undecidedVertices.insert(i);
    }
}

int ProblemInstance::size() const {
    return undecidedVertices.size();
}

int ProblemInstance::realSize() const {
    return graph.size();
}

int ProblemInstance::edgesNum() const {
    return edges;
}

void ProblemInstance::addEdge(int u, int v) {
    if (!graph[u].count(v))
        edges++;
    graph[u].insert(v);
    graph[v].insert(u);
}

void ProblemInstance::takeVertex(int v) {
    eraseVertexImpl(v);
    vertices[v].state = Vertex::State::DEFINETELY_IN;
}

void ProblemInstance::removeVertex(int v) {
    eraseVertexImpl(v);
    vertices[v].state = Vertex::State::DEFINETELY_NOT;
}

const ProblemInstance::Set<int>& ProblemInstance::adjacent(int v) const {
    return graph[v];
}

const ProblemInstance::Set<int>& ProblemInstance::undecided() const {
    return undecidedVertices;
}

std::vector<int> ProblemInstance::getTook() {
    std::vector<int> result;
    for (int v = 0; v < (int)graph.size(); v++)
        if (vertices[v].state == Vertex::State::DEFINETELY_IN)
            result.push_back(v);
    return result;
}

void ProblemInstance::eraseVertexImpl(int v) {
    undecidedVertices.erase(v);
    vertices[v].state = Vertex::State::DEFINETELY_NOT;
    for (int u : graph[v]) {
        edges--;
        graph[u].erase(v);
    }
}

std::istream& operator>>(std::istream& is, ProblemInstance& instance) {
    int n, m;
    is >> n >> m;
    instance = ProblemInstance(n);
    for (int i = 0; i < m; i++) {
        int u, v;
        is >> u >> v;
        u--; v--;
        instance.addEdge(u, v);
    }
    return is;
}
