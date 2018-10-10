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

    void addEdge(int u, int v) {
        graph[u].insert(v);
        graph[v].insert(u);
    }

    void takeVertex(int v) {
        undecidedVertices.erase(v);
        vertices[v].state = Vertex::State::DEFINETELY_IN;
    }

    void removeVertex(int v) {
        undecidedVertices.erase(v);
        vertices[v].state = Vertex::State::DEFINETELY_NOT;
        for (int u : graph[v]) {
            graph[u].erase(v);
        }
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
    Set<int> undecidedVertices;
    std::vector<Vertex> vertices;
    std::vector<Set<int>> graph;
};

class BipartiteGraph {
public:
    enum class Part {
        LEFT,
        RIGHT
    };

    using VC = std::vector<std::pair<int, Part>>;

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
struct KuhnVCFinder {
    const BipartiteGraph& graph;
    int n, m;
    std::vector<bool> vis;
    std::vector<int> pair;
    std::vector<bool> hasPair;

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

    std::vector<bool> rvis;
    void vcdfs(int v) {
        vis[v] = true;
        for (int u : graph.adjacent(v)) {
            if (rvis[u])
                continue;
            rvis[u] = true;
            if (pair[u] != -1 && !vis[pair[u]])
                vcdfs(pair[u]);
        }
    }

    KuhnVCFinder(const BipartiteGraph& graph) : graph(graph) {
        n = graph.leftSize();
        m = graph.rightSize();
    }

    BipartiteGraph::VC find() {
        hasPair.resize(n);
        vis.resize(n);
        pair.resize(m, -1);
        for (int v = 0; v < n; ++v) {
            vis.assign(n, false);
            dfs(v);
        }

        int start = -1;
        for (int v = 0; v < n && start == -1; ++v) {
            if (!hasPair[v])
                start = v;
        }

        if (start == -1) {
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
            return ans;
        }

        vis.assign(n, false);
        rvis.assign(m, false);
        vcdfs(start);

        BipartiteGraph::VC ans;
        for (int i = 0; i < n; i++) {
            if (!vis[i])
                ans.push_back(std::make_pair(i, BipartiteGraph::Part::LEFT));
        }
        for (int i = 0; i < m; i++) {
            if (rvis[i])
                ans.push_back(std::make_pair(i, BipartiteGraph::Part::RIGHT));
        }
        return ans;
    }
};

using VCFinder = KuhnVCFinder;

struct LPReducer {
    ProblemInstance& graph;

    LPReducer(ProblemInstance& graph) : graph(graph) {}

    void reduce() {
        BipartiteGraph bigraph(graph);
        auto lpSolution = VCFinder(bigraph).find();

        std::vector<int> count(graph.realSize());
        for (auto p : lpSolution) {
            count[p.first]++;
        }

        for (int i = 0; i < graph.realSize(); i++) {
            if (count[i] == 0)
                graph.removeVertex(i);
            else if (count[i] == 2)
                graph.takeVertex(i);
        }
    }
};

int main() {
    int n, m;
    std::cin >> n >> m;

    ProblemInstance instance(n);
    for (int i = 0; i < m; i++) {
        int u, v;
        std::cin >> u >> v;
        u--; v--;

        instance.addEdge(u, v);
    }

    LPReducer(instance).reduce();

    std::cout << "Found kernel of size " << instance.size() << std::endl;
    std::cout << "Took vertices: ";
    for (int u : instance.getTook())
        std::cout << u + 1 << " ";
    std::cout << std::endl;
    std::cout << "Undecided vertices: ";
    for (int u : instance.undecided())
        std::cout << u + 1 << " ";
    std::cout << std::endl;

    return 0;
}
