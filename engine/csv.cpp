// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

// The MIT License (MIT)
//
// Copyright (c) 2018 - 2020 Huldra
// Copyright (c) 2017 Romain Sylvian
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.

#include <fstream>
#include <sstream>
#include <iomanip>
#include <cstring>

#include "engine/csv.h"
#include "engine/arctic_types.h"

namespace arctic {

CsvTable::CsvTable() {
}

bool CsvTable::LoadFile(const std::string &filename, char sep) {
  type_ = kCsvSourceFile;
  sep_ = sep;
  file_ = filename;
  std::string line;
  std::ifstream ifile(file_.c_str());
  if (ifile.is_open()) {
    while (ifile.good()) {
      getline(ifile, line);
      if (!line.empty()) {
        original_file_.push_back(line);
      }
    }
    ifile.close();

    if (original_file_.empty()) {
      error_description = std::string("No Data in ").append(file_);
      return false;
    }
    // Remove the BOM (Byte Order Mark) for UTF-8
    if (original_file_[0].length() >= 3 &&
        Ui8(original_file_[0][0]) == 0xEF &&
        Ui8(original_file_[0][1]) == 0xBB &&
        Ui8(original_file_[0][2]) == 0xBF) {
      original_file_[0] = original_file_[0].erase(0, 3);
    }
    bool is_ok = true;
    is_ok = is_ok && ParseHeader();
    is_ok = is_ok && ParseContent();
    return is_ok;
  } else {
    error_description = std::string("Failed to open ").append(file_);
    return false;
  }
}

bool CsvTable::LoadString(const std::string &data, char sep) {
  type_ = kCsvSourcePure;
  sep_ = sep;
  std::string line;
  std::istringstream stream(data);
  while (std::getline(stream, line)) {
    if (line.empty()) {
      original_file_.push_back(line);
    }
  }
  if (original_file_.empty()) {
    error_description = std::string("No Data in pure content");
    return false;
  }

  bool is_ok = true;
  is_ok = is_ok && ParseHeader();
  is_ok = is_ok && ParseContent();
  return is_ok;
}

CsvTable::~CsvTable() {
  std::vector<CsvRow *>::iterator it;
  for (it = content_.begin(); it != content_.end(); ++it) {
    delete *it;
  }
}

bool CsvTable::ParseHeader() {
  std::stringstream ss(original_file_[0]);
  std::string item;
  while (std::getline(ss, item, sep_)) {
    while (!item.empty() && item[item.size()-1] == '\r') {
      item = item.substr(0, item.size() - 1);
    }
    header_.push_back(item);
  }
  return true;
}

bool CsvTable::ParseContent() {
  std::deque<std::string>::iterator it = original_file_.begin();
  ++it;  // skip header
  Ui64 line_idx = 1;
  for (; it != original_file_.end(); ++it) {
    bool quoted = false;
    size_t token_start = 0;
    size_t i = 0;

    CsvRow *row = new CsvRow(header_);

    for (; i != it->length(); ++i) {
      if (it->at(i) == '"') {
        quoted = !quoted;
      } else if (it->at(i) == ',' && !quoted) {
        row->Push(it->substr(token_start, i - token_start));
        token_start = i + 1;
      }
    }
    // end
    std::string item = it->substr(token_start, it->length() - token_start);
    while (!item.empty() && item[item.size()-1] == '\r') {
      item = item.substr(0, item.size() - 1);
    }
    row->Push(item);

    // if value(s) missing
    if (row->Size() != header_.size()) {
      delete row;
      std::stringstream str;
      str << "corrupted data at line " << line_idx;
      error_description = str.str();
      return false;
    }
    content_.push_back(row);

    line_idx++;
  }
  return true;
}

CsvRow *CsvTable::GetRow(Ui64 row_position) const {
  if (row_position < content_.size()) {
    return content_[static_cast<size_t>(row_position)];
  }
  return nullptr;
}

CsvRow &CsvTable::operator[](Ui64 row_position) const {
  CsvRow *row = CsvTable::GetRow(row_position);
  // Check(row, "row_position out of bounds in CvsTable");
  return *row;
}

std::string CsvTable::GetErrorDescription() const {
  return error_description;
}

Ui64 CsvTable::RowCount() const {
  return static_cast<Ui64>(content_.size());
}

Ui64 CsvTable::ColumnCount() const {
  return static_cast<Ui64>(header_.size());
}

std::vector<std::string> CsvTable::GetHeader() const {
  return header_;
}

const std::string CsvTable::GetHeaderElement(Ui64 pos) const {
  if (pos >= header_.size()) {
    return std::string();
  }
  return header_[static_cast<size_t>(pos)];
}

bool CsvTable::DeleteRow(Ui64 pos) {
  if (static_cast<size_t>(pos) < content_.size()) {
    delete *(content_.begin() + static_cast<int>(pos));
    content_.erase(content_.begin() + static_cast<int>(pos));
    return true;
  }
  return false;
}

bool CsvTable::AddRow(Ui64 pos, const std::vector<std::string> &r) {
  CsvRow *row = new CsvRow(header_);

  for (auto it = r.begin(); it != r.end(); ++it) {
    row->Push(*it);
  }

  if (pos <= content_.size()) {
    content_.insert(content_.begin() + static_cast<int>(pos), row);
    return true;
  }
  delete row;
  return false;
}

void CsvTable::SaveFile() const {
  if (type_ == CsvSourceType::kCsvSourceFile) {
    std::ofstream f;
    f.open(file_, std::ios::out | std::ios::trunc);

    // header
    Ui64 i = 0;
    for (auto it = header_.begin(); it != header_.end(); ++it) {
      f << *it;
      if (i < header_.size() - 1) {
        f << ",";
      } else {
        f << std::endl;
      }
      i++;
    }

    for (auto it = content_.begin(); it != content_.end(); ++it) {
      f << **it << std::endl;
    }
    f.close();
  }
}

const std::string &CsvTable::GetFileName() const {
  return file_;
}

// ROW
CsvRow::CsvRow(const std::vector<std::string> &header)
    : header_(header)
{}

CsvRow::~CsvRow() {}

Ui64 CsvRow::Size() const {
  return static_cast<Ui64>(values_.size());
}

void CsvRow::Push(const std::string &value) {
  values_.push_back(value);
}

bool CsvRow::Set(const std::string &key, const std::string &value) {
  std::vector<std::string>::const_iterator it;
  size_t pos = 0;
  for (it = header_.begin(); it != header_.end(); ++it) {
    if (key == *it) {
      values_[pos] = value;
      return true;
    }
    pos++;
  }
  return false;
}

const std::string CsvRow::operator[](Ui64 value_position) const {
  if (value_position < values_.size()) {
    return values_[static_cast<size_t>(value_position)];
  }
  return std::string();
}

const std::string CsvRow::operator[](const std::string &key) const {
  std::vector<std::string>::const_iterator it;
  size_t pos = 0;
  for (it = header_.begin(); it != header_.end(); ++it) {
    if (key == *it) {
      return values_[pos];
    }
    pos++;
  }
  return std::string();
}

std::ostream &operator<<(std::ostream &os, const CsvRow &row) {
  for (size_t i = 0; i != row.values_.size(); ++i) {
    os << row.values_[i] << " | ";
  }
  return os;
}

std::ofstream &operator<<(std::ofstream &os, const CsvRow &row) {
  for (size_t i = 0; i != row.values_.size(); ++i) {
    os << row.values_[i];
    if (i + 1 < row.values_.size()) {
      os << ",";
    }
  }
  return os;
}

}  // namespace arctic

