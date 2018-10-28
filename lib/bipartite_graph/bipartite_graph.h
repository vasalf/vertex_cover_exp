#pragma once

#include <lib/problem_instance/problem_instance.h>

#include <utility>
#include <vector>

class BipartiteGraph {
public:
    enum class Part {
        LEFT,
        RIGHT
    };

    using Vertex = std::pair<int, Part>;
    using VC = std::vector<Vertex>;

public:
    BipartiteGraph(int n, int m);

    BipartiteGraph(const ProblemInstance& graph);

    void addEdge(int i, int j);
    const std::vector<int> adjacent(int v) const;

    int leftSize() const;
    int rightSize() const;

private:
    int m;
    std::vector<std::vector<int> > adj;
};
