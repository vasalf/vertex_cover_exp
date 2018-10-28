#pragma once

#include "bipartite_graph.h"

#include <cassert>

// TODO: optimize
struct KuhnMaxMatchingFinder {
    const BipartiteGraph& graph;
    int n, m;
    std::vector<bool> vis;
    std::vector<int> pair;
    std::vector<bool> hasPair;
    int size = 0;

    bool dfs(int v) {
        vis[v] = true;
        for (int u : graph.adjacent(v)) {
            if (pair[u] == -1 || (!vis[pair[u]] && dfs(pair[u]))) {
                pair[u] = v;
                hasPair[v] = true;
                return true;
            }
        }
        return false;
    }

    KuhnMaxMatchingFinder(const BipartiteGraph& graph) : graph(graph) {
        n = graph.leftSize();
        m = graph.rightSize();
    }

    void find() {
        hasPair.resize(n);
        vis.resize(n);
        pair.resize(m, -1);
        for (int v = 0; v < n; ++v) {
            vis.assign(n, false);
            if (dfs(v))
                size++;
        }

    }
};

template<class MaxMatchingFinder>
struct VCFinder {
    const BipartiteGraph& graph;
    MaxMatchingFinder maxm;
    int n, m;

    std::vector<bool> vis;
    std::vector<bool> rvis;
    void vcdfs(int v) {
        vis[v] = true;
        for (int u : graph.adjacent(v)) {
            if (rvis[u])
                continue;
            rvis[u] = true;
            if (maxm.pair[u] != -1 && !vis[maxm.pair[u]])
                vcdfs(maxm.pair[u]);
        }
    }

    VCFinder(const BipartiteGraph& graph) : graph(graph), maxm(graph) {
        n = graph.leftSize();
        m = graph.rightSize();
    }

    BipartiteGraph::VC find() {
        maxm.find();

        vis.assign(n, false);
        rvis.assign(m, false);
        bool launched = false;
        for (int v = 0; v < n; ++v) {
            if (!maxm.hasPair[v] && !vis[v]) {
                launched = true;
                vcdfs(v);
            }
        }

        if (!launched) {
            BipartiteGraph::VC ans;
            BipartiteGraph::Part part;
            if (n <= m) {
                part = BipartiteGraph::Part::LEFT;
            } else {
                part = BipartiteGraph::Part::RIGHT;
            }
            for (int i = 0; i < std::min(n, m); i++) {
                ans.push_back(std::make_pair(i, part));
            }

            assert(ans.size() == maxm.size);
            return ans;
        }

        BipartiteGraph::VC ans;
        for (int i = 0; i < n; i++) {
            if (!vis[i])
                ans.push_back(std::make_pair(i, BipartiteGraph::Part::LEFT));
        }
        for (int i = 0; i < m; i++) {
            if (rvis[i])
                ans.push_back(std::make_pair(i, BipartiteGraph::Part::RIGHT));
        }

        assert(ans.size() == maxm.size);
        return ans;
    }
};

using MaxMatchingFinder = KuhnMaxMatchingFinder;
