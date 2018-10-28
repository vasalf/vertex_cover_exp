#pragma once

#include <lib/bipartite_graph/bipartite_graph.h>
#include <lib/bipartite_graph/max_matching_finder.h>
#include <lib/problem_instance/problem_instance.h>

namespace LPPrivate {
    void reduceImpl(ProblemInstance& graph, const BipartiteGraph::VC& lpSolution) {
        std::vector<int> count(graph.realSize());
        for (auto p : lpSolution) {
            count[p.first]++;
        }

        auto undecided = graph.undecided();
        for (int i : undecided) {
            if (count[i] == 0) {
                graph.removeVertex(i);
            } else if (count[i] == 2) {
                graph.takeVertex(i);
            }
        }
    }

    void reduceImpl(ProblemInstance& graph, const BipartiteGraph& bigraph) {
        auto lpSolution = VCFinder<MaxMatchingFinder>(bigraph).find();
        reduceImpl(graph, lpSolution);
    }
}

struct LPKernel {
    ProblemInstance& graph;

    LPKernel(ProblemInstance& graph) : graph(graph) {}

    void reduce() {
        BipartiteGraph bigraph(graph);
        LPPrivate::reduceImpl(graph, bigraph);
    }

    static std::string method() {
        return "LP";
    }

    bool disabled() {
        return false;
    }
};

struct ZeroSurplusLPKernel {
    ProblemInstance& graph;

    BipartiteGraph buildBpartiteGraphSkippingVertex(int u) {
        BipartiteGraph result(graph.realSize(), graph.realSize());
        for (int i : graph.undecided()) {
            if (i == u)
                continue;
            for (int v : graph.adjacent(i)) {
                if (u != v)
                    result.addEdge(i, v);
            }
        }
        return result;
    }

    ZeroSurplusLPKernel(ProblemInstance& graph) : graph(graph) {}

    void reduce() {
        LPKernel(graph).reduce();

        while (true) {
            bool found = false;
            for (int u : graph.undecided()) {
                auto bigraph = buildBpartiteGraphSkippingVertex(u);
                auto lpSolution = VCFinder<MaxMatchingFinder>(bigraph).find();
                if ((int)lpSolution.size() + 2 == graph.size()) {
                    lpSolution.push_back({u, BipartiteGraph::Part::LEFT});
                    lpSolution.push_back({u, BipartiteGraph::Part::RIGHT});
                    LPPrivate::reduceImpl(graph, lpSolution);
                    found = true;
                    break;
                }
            }
            if (!found)
                break;
        }
    }

    static std::string method() {
        return "ZeroSurplusLP";
    }

    bool disabled() {
        return 1ll * graph.size() * graph.size() * graph.edgesNum() > 2e9;
    }
};

struct IsolatedVerticesReducer {
    ProblemInstance& graph;

    IsolatedVerticesReducer(ProblemInstance& graph) : graph(graph) {}

    void reduce() {
        auto oldUndecided = graph.undecided();
        for (int u : oldUndecided)
            if (graph.adjacent(u).empty())
                graph.removeVertex(u);
    }
};

struct CrownKernel {
    ProblemInstance& graph;

    CrownKernel(ProblemInstance& graph) : graph(graph) {}

    void reduce() {
        IsolatedVerticesReducer(graph).reduce();

        std::vector<bool> undecided(graph.realSize());
        for (int u : graph.undecided())
            undecided[u] = true;

        std::vector<bool> covered(graph.realSize());
        for (int u : graph.undecided())
            for (int v : graph.adjacent(u)) {
                if (!covered[u] && !covered[v]) {
                    covered[u] = true;
                    covered[v] = true;
                }
            }

        std::vector<int> leftId, rightId;
        std::vector<int> idInPart(graph.realSize());
        for (int u : graph.undecided()) {
            if (covered[u]) {
                idInPart[u] = leftId.size();
                leftId.push_back(u);
            } else {
                idInPart[u] = rightId.size();
                rightId.push_back(u);
            }
        }

        BipartiteGraph bigraph(leftId.size(), rightId.size());
        for (int i = 0; i < (int)rightId.size(); i++) {
            for (int u : graph.adjacent(rightId[i]))
                bigraph.addEdge(idInPart[u], i);
        }

        VCFinder<MaxMatchingFinder> vcf(bigraph);
        auto vc = vcf.find();

        std::vector<bool> inVC(graph.realSize());
        for (auto v : vc) {
            auto& inPart = v.second == BipartiteGraph::Part::LEFT ? leftId : rightId;
            inVC[inPart[v.first]] = true;
        }

        bool took = false;
        for (int i = 0; i < (int)leftId.size(); i++) {
            if (inVC[leftId[i]]) {
                graph.takeVertex(leftId[i]);
                took = true;
            }
        }
        if (!took)
            return;

        for (int i = 0; i < (int)rightId.size(); i++) {
            if (vcf.maxm.pair[i] != -1 && inVC[leftId[vcf.maxm.pair[i]]])
                graph.removeVertex(rightId[i]);
        }

        auto iter = graph.undecided();
        for (int u : iter)
            if (graph.adjacent(u).empty())
                graph.removeVertex(u);
    }

    static std::string method() {
        return "Crown";
    }

    bool disabled() {
        return false;
    }
};

struct ExhaustiveCrownKernel {
    ProblemInstance& graph;

    ExhaustiveCrownKernel(ProblemInstance& graph) : graph(graph) {}

    void reduce() {
        int size;
        do {
            size = graph.size();
            CrownKernel(graph).reduce();
        } while (size > graph.size());
    }

    static std::string method() {
        return "CrownEx";
    }

    bool disabled() {
        return false;
    }
};

