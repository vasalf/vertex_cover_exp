#pragma once

#include <iostream>
#include <unordered_set>
#include <vector>

class ProblemInstance {
    // TODO: replace with lists
    template<class T>
    using Set = std::unordered_set<T>;

    struct Vertex {
        enum class State {
            DEFINETELY_IN,
            DEFINETELY_NOT,
            UNDECIDED
        };

        State state;
    };
public:
    ProblemInstance() = default;
    ~ProblemInstance() = default;

    ProblemInstance(int n);

    int size() const;
    int realSize() const;

    int edgesNum() const;

    void addEdge(int u, int v);
    const Set<int>& adjacent(int v) const;

    void takeVertex(int v);
    void removeVertex(int v);

    const Set<int>& undecided() const;
    std::vector<int> getTook();

private:
    void eraseVertexImpl(int v);

    Set<int> undecidedVertices;
    std::vector<Vertex> vertices;
    std::vector<Set<int>> graph;
    int edges = 0;
};

std::istream& operator>>(std::istream& is, ProblemInstance& instance);
