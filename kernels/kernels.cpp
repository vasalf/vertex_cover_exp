// -*- mode: c++; -*-
/*
 * Copyright (c) 2018 Vasily Alferov
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <ascii_table/ascii_table.h>
#include <bits/stdc++.h>

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

    ProblemInstance(int n) {
        vertices.resize(n, { Vertex::State::UNDECIDED });
        graph.resize(n);
        for (int i = 0; i < n; ++i) {
            undecidedVertices.insert(i);
        }
    }

    int size() const {
        return undecidedVertices.size();
    }

    int realSize() const {
        return graph.size();
    }

    int edgesNum() const {
        return edges;
    }

    void addEdge(int u, int v) {
        if (!graph[u].count(v))
            edges++;
        graph[u].insert(v);
        graph[v].insert(u);
    }

    void takeVertex(int v) {
        eraseVertexImpl(v);
        vertices[v].state = Vertex::State::DEFINETELY_IN;
    }

    void removeVertex(int v) {
        eraseVertexImpl(v);
        vertices[v].state = Vertex::State::DEFINETELY_NOT;
    }

    const Set<int>& adjacent(int v) const {
        return graph[v];
    }

    const Set<int>& undecided() const {
        return undecidedVertices;
    }

    std::vector<int> getTook() {
        std::vector<int> result;
        for (int v = 0; v < (int)graph.size(); v++)
            if (vertices[v].state == Vertex::State::DEFINETELY_IN)
                result.push_back(v);
        return result;
    }

private:
    void eraseVertexImpl(int v) {
        undecidedVertices.erase(v);
        vertices[v].state = Vertex::State::DEFINETELY_NOT;
        for (int u : graph[v]) {
            edges--;
            graph[u].erase(v);
        }
    }

    Set<int> undecidedVertices;
    std::vector<Vertex> vertices;
    std::vector<Set<int>> graph;
    int edges = 0;
};

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

class BipartiteGraph {
public:
    enum class Part {
        LEFT,
        RIGHT
    };

    using Vertex = std::pair<int, Part>;
    using VC = std::vector<Vertex>;

public:
    BipartiteGraph(int n, int m) : m(m), adj(n) {}

    BipartiteGraph(const ProblemInstance& graph) {
        adj.resize(graph.realSize());
        m = graph.realSize();

        for (int u : graph.undecided()) {
            for (int v : graph.adjacent(u)) {
                addEdge(u, v);
            }
        }
    }

    void addEdge(int i, int j) {
        adj[i].push_back(j);
    }

    int leftSize() const {
        return adj.size();
    }

    int rightSize() const {
        return m;
    }

    const std::vector<int> adjacent(int v) const {
        return adj[v];
    }

private:
    int m;
    std::vector<std::vector<int> > adj;
};

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

template<class Reducer>
void reduce(ProblemInstance instance) {
    Reducer(instance).reduce();

    std::cout << "Using kernel `" << Reducer::method() << "`" << std::endl;
    std::cout << "Found kernel of size " << instance.size() << std::endl;
    std::cout << "Took vertices: ";
    for (int u : instance.getTook())
        std::cout << u + 1 << " ";
    std::cout << std::endl;
    std::cout << "Undecided vertices: ";
    for (int u : instance.undecided())
        std::cout << u + 1 << " ";
    std::cout << std::endl;
}

struct GeneratedInstance {
    ProblemInstance instance;
    std::string name;
};

template<class Reducer>
void runTestImpl(const GeneratedInstance& test, std::vector<CellPtr>& row) {
    ProblemInstance instance = test.instance;

    Reducer reducer(instance);
    if (reducer.disabled()) {
        row.push_back(make_cell<std::string>("--"));
        row.push_back(make_cell<std::string>("--"));
        row.push_back(make_cell<std::string>("--"));
    } else {
        auto start = std::chrono::high_resolution_clock::now();
        reducer.reduce();
        auto end = std::chrono::high_resolution_clock::now();

        auto execTime = end - start;

        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(execTime);

        char time[20];
        std::sprintf(time, "%.03lf s", 1e-3 * duration.count());

        row.push_back(make_cell<int>(instance.size()));
        row.push_back(make_cell<int>(instance.getTook().size()));
        row.push_back(make_cell<std::string>(time));
    }
}

template<class Reducer>
void makeColumnsImpl(Table& t) {
    t.addColumn(Column(Column::Header(Reducer::method(), { "size", "took", "time" })));
}

template<class T, class... Args>
struct KernelList {
    KernelList<Args...> underlying_;

    void runTest(const GeneratedInstance& test, std::vector<CellPtr>& row) {
        runTestImpl<T>(test, row);
        underlying_.runTest(test, row);
    }

    void makeColumns(Table& t) {
        makeColumnsImpl<T>(t);
        underlying_.makeColumns(t);
    }
};

template<class T>
struct KernelList<T> {
    void runTest(const GeneratedInstance& test, std::vector<CellPtr>& row) {
        runTestImpl<T>(test, row);
    }

    void makeColumns(Table& t) {
        makeColumnsImpl<T>(t);
    }
};

template<class KL>
struct Kernels {
    KL kernels;
    std::vector<GeneratedInstance> tests;

    void run() {
        Table t;
        t.addColumn(Column(Column::Header("Test", {})));
        kernels.makeColumns(t);

        for (const auto& test : tests) {
            std::vector<CellPtr> row = {make_cell<std::string>(test.name)};
            kernels.runTest(test, row);
            t.addRow(row);
        }

        t.print();
    }
};

template<class... Args>
auto makeKernels(std::vector<GeneratedInstance>&& tests) {
    return Kernels<KernelList<Args...> > { KernelList<Args...>(), tests };
}

std::mt19937 rnd(179);

GeneratedInstance randomGraph(int n, int m) {
    ProblemInstance instance(n);
    std::uniform_int_distribution<int> dist(0, n - 1);

    for (int i = 0; i < m; i++) {
        int u, v;
        do {
            u = dist(rnd);
            v = dist(rnd);
        } while (u == v || instance.adjacent(u).count(v));
        instance.addEdge(u, v);
    }

    std::ostringstream ss;
    ss << "randomGraph(n = " << n << ", m = " << m << ")";
    return { instance, ss.str() };
}

GeneratedInstance graphWithPerfectMatching(int n, int m) {
    assert(n % 2 == 0);
    assert(m >= n / 2);

    std::vector<int> vertexMap(n);
    std::iota(vertexMap.begin(), vertexMap.end(), 0);
    std::shuffle(vertexMap.begin(), vertexMap.end(), rnd);

    ProblemInstance instance(n);
    for (int i = 0; i < n; i += 2) {
        instance.addEdge(vertexMap[i], vertexMap[i + 1]);
    }

    std::uniform_int_distribution<int> dist(0, n - 1);
    for (int i = 0; i < m; i++) {
        int u, v;
        do {
            u = dist(rnd);
            v = dist(rnd);
        } while (u == v || instance.adjacent(u).count(v));
        instance.addEdge(u, v);
    }

    std::ostringstream ss;
    ss << "graphWithPerfectMatching(n = " << n << ", m = " << m << ")";
    return { instance, ss.str() };
}

int main() {
    std::vector<GeneratedInstance> tests = {
        randomGraph(100,  100),
        randomGraph(100,  150),
        randomGraph(100,  200),
        randomGraph(100,  300),
        randomGraph(1000, 1000),
        randomGraph(1000, 2000),
        randomGraph(1000, 3000),
        randomGraph(1000, 4000),
        randomGraph(1000, 5000),
        randomGraph(1000, 10000),
        randomGraph(1000, 40000),
        randomGraph(10000, 10000),
        randomGraph(10000, 20000),
        randomGraph(10000, 30000),
        randomGraph(10000, 40000),
        randomGraph(10000, 50000),
        randomGraph(10000, 100000),
        randomGraph(10000, 400000),
        graphWithPerfectMatching(1000, 1000),
        graphWithPerfectMatching(1000, 2000),
        graphWithPerfectMatching(1000, 3000),
        graphWithPerfectMatching(1000, 4000),
        graphWithPerfectMatching(1000, 5000),
        graphWithPerfectMatching(1000, 10000),
        graphWithPerfectMatching(1000, 40000),
        graphWithPerfectMatching(10000, 10000),
        graphWithPerfectMatching(10000, 20000),
        graphWithPerfectMatching(10000, 30000),
        graphWithPerfectMatching(10000, 40000),
        graphWithPerfectMatching(10000, 50000),
        graphWithPerfectMatching(10000, 100000),
        graphWithPerfectMatching(10000, 400000),
    };

    auto kernels = makeKernels<
        CrownKernel,
        ExhaustiveCrownKernel,
        LPKernel,
        ZeroSurplusLPKernel
    >(std::move(tests));

    kernels.run();

    return 0;
}
