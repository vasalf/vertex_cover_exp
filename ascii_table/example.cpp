// -*- mode: c++; -*-
/*
 * Copyright (c) 2017 Vasily Alferov
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
#include <string>
#include <vector>
#include <memory>
#include <sstream>

using namespace std;

template<>
class TypedCell<double> : public Cell {
public:
    TypedCell(double value) : value_(value) {}
    virtual ~TypedCell() {}

    virtual string show(size_t maxlen) const {
        ostringstream ss;
        ss.precision(3);
        ss << fixed << value_;
        if (ss.str().size() > maxlen)
            throw CellLengthException(ss.str(), maxlen);
        return ss.str();
    }
private:
    double value_;
};

int main() {
    Table t;

    t.addColumn(Column(Column::Header("String", {})));
    t.addColumn(Column(Column::Header("Fx string", {})), 20);
    t.addColumn(Column(Column::Header("int", {})));
    t.addColumn(Column(Column::Header("double", {})));
    t.addColumn(Column(Column::Header("Double column", {"a", "b"})));
    
    t.addRow({make_cell<string>("1, 1"),
                make_cell<string>("1, 2"),
                make_cell<int>(3),
                make_cell<double>(0.5),
                make_cell<int>(179),
                make_cell<int>(181)});
    t.addRow({make_cell<string>("2, 1"),
                make_cell<string>("2, 2"),
                make_cell<int>(3),
                make_cell<double>(123),
                make_cell<int>(183),
                make_cell<int>(185)});

    t.print();

    return 0;
}
