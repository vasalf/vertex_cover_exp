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

#include <lib/problem_instance/problem_instance.h>
#include <lib/bipartite_graph/bipartite_graph.h>
#include <lib/bipartite_graph/max_matching_finder.h>
#include <lib/kernels/kernels.h>

#include <ascii_table/ascii_table.h>
#include <bits/stdc++.h>

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
