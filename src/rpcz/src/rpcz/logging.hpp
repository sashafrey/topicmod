// Copyright 2011 Google Inc. All Rights Reserved.
// 
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// 
//     http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// Author: nadavs@google.com <Nadav Samet>

#ifndef RPCZ_LOGGING_H
#define RPCZ_LOGGING_H

#define GLOG_NO_ABBREVIATED_SEVERITIES
#include "glog/logging.h"

#include <string>

namespace rpcz {

class Log {
 public:
  static std::string ptr_to_hex(const void* ptr) {
    std::stringstream stream;
    stream << "0x" << ptr;
    std::string result = stream.str();
    return result;
  }

  static std::string string_to_hex(const std::string& value) {
    static const char char_map[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
    std::stringstream stream;
    stream << "0x";
    for (size_t i = 0; i < value.size(); ++i) {
      unsigned char x = value[i];
      stream << char_map[(x / 16) % 16];
      stream << char_map[x % 16];
    }

    std::string result = stream.str();
    return result;
  }
};

class LogModule {
 public:
  LogModule(std::string name) {
    id_ = name;
  }

  LogModule(std::string name, std::string id) {
    std::stringstream stream;
    stream << name << "(" << id << ") ";
    id_ = stream.str();
  }

  LogModule(std::string name, const void* id) {
    std::stringstream stream;
    stream << name << "(" << Log::ptr_to_hex(id) << ") ";
    id_ = stream.str();
  }

  void append_id(std::string name, std::string id) {
    std::stringstream stream;
    stream << id_ << "(" << name << "=" << id << ") ";
    id_ = stream.str();
  }

  const std::string& operator()() const {
    return id_;
  }

 private:
  std::string id_;
};

}  // namespace rpcz

#endif
