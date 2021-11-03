// The MIT License (MIT)
//
// Copyright (c) 2018 - 2021 Huldra
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

#ifndef ENGINE_CSV_H_
#define ENGINE_CSV_H_

#include <deque>
#include <stdexcept>
#include <string>
#include <vector>
#include <list>
#include <sstream>

#include "engine/arctic_types.h"

namespace arctic {

/// @addtogroup global_utility
/// @{
class CsvRow {
 private:
  const std::vector<std::string> header_;
  std::vector<std::string> values_;

 public:
  explicit CsvRow(const std::vector<std::string> &);
  ~CsvRow();

  Ui64 Size() const;
  void Push(const std::string &);
  bool Set(const std::string &, const std::string &);

  template<typename T>
  const T GetValue(Ui64 pos, T default_value) const {
    if (pos < values_.size()) {
      T res;
      std::stringstream ss;
      ss << values_[static_cast<size_t>(pos)];
      ss >> res;
      if (ss.fail()) {
        return default_value;
      }
      if (ss.peek() == std::iostream::traits_type::eof()) {
        return res;
      }
    }
    return default_value;
  }
  template<typename T>
  const T GetValue(const std::string &value_name, T default_value) const {
    std::stringstream ss((*this)[value_name]);
    T res;
    ss >> res;
    if (ss.fail()) {
      return default_value;
    }
    if (ss.peek() == std::iostream::traits_type::eof()) {
      return res;
    }
    return default_value;
  }
  const std::string operator[](Ui64) const;
  const std::string operator[](const std::string &value_name) const;
  friend std::ostream& operator<<(std::ostream& os, const CsvRow &row);
  friend std::ofstream& operator<<(std::ofstream& os, const CsvRow &row);
};

enum CsvSourceType {
  kCsvSourceFile = 0,
  kCsvSourcePure = 1
};

class CsvTable {
 public:
  CsvTable();
  bool LoadFile(const std::string &filename, char sep = ',');
  bool LoadString(const std::string &input, char sep = ',');
  ~CsvTable();
  CsvRow *GetRow(Ui64 row) const;
  Ui64 RowCount() const;
  Ui64 ColumnCount() const;
  std::vector<std::string> GetHeader() const;
  const std::string GetHeaderElement(Ui64 pos) const;
  const std::string &GetFileName() const;
  bool DeleteRow(Ui64 row);
  bool AddRow(Ui64 pos, const std::vector<std::string> &);
  void SaveFile() const;
  CsvRow &operator[](Ui64 row) const;
  std::string GetErrorDescription() const;

 protected:
  bool ParseHeader();
  bool ParseContent();

 private:
  std::string file_;
  CsvSourceType type_ = kCsvSourcePure;
  char sep_ = ',';
  std::deque<std::string> original_file_;
  std::vector<std::string> header_;
  std::vector<CsvRow *> content_;
  std::string error_description;
};
/// @}

}  // namespace arctic

#endif  // ENGINE_CSV_H_
