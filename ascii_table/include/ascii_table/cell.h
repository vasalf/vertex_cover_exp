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

#include <sstream>
#include <string>
#include <memory>

class CellLengthException {
public:
    CellLengthException(std::string show, int length);
    std::string message() const;
private:
    std::string show_;
    int length_;
};

class Cell {
public:
    Cell();
    virtual ~Cell();

    virtual std::string show(std::size_t maxlen) const = 0;
};
typedef std::shared_ptr<Cell> CellPtr;

template<class T>
class TypedCell : public Cell {
public:
    TypedCell(T value) : value_(value) {}
    virtual ~TypedCell() {}

    virtual std::string show(std::size_t maxlen) const {
        std::ostringstream ss;
        ss << value_;
        if (ss.str().length() > maxlen)
            throw CellLengthException(ss.str(), maxlen);
        return ss.str();
    }
private:
    T value_;
};

template<class T, class... Args>
CellPtr make_cell(Args... args) {
    return std::make_shared<TypedCell<T>>(args...);
}
