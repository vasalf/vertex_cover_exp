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

#include "ascii_table/column.h"

#include <vector>
#include <string>
#include <memory>

Column::Header::Header(const std::string& name, const std::vector<std::string>& subcolumn_names) :
    name_(name), subcolumn_names_(subcolumn_names) {}

bool Column::Header::needSecondRow() const {
    return !subcolumn_names_.empty();
}

std::size_t Column::Header::subcolumnNumber() const {
    return subcolumn_names_.size();
}

std::string Column::Header::Name() const {
    return name_;
}

const std::string& Column::Header::operator[](std::size_t i) const {
    return subcolumn_names_[i];
}

Column::Column(const Header& header) : header_(header) {}

Column::~Column() {}

const Column::Header& Column::getHeader() const {
    return header_;
}
