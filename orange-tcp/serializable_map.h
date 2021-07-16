#pragma once

#include <iostream>
#include <vector>
#include <map>
#include <sstream>
#include <string>

namespace orange_tcp {

/*
# Copyright H. Watanabe 2017
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)
*/

// Simple on-disk key-value store.
template<class Key, class Value>
class serializable_map : public std::map<Key, Value> {
 public:
  std::vector<char> serialize() {
    std::vector<char> buffer;
    std::stringstream ss;
    for (auto &i : (*this)) {
      Key key = i.first;
      Value value = i.second;

      printf("Writing %s, %s\n",
      	key.str().c_str(), value.str().c_str());

      writeline(ss, key, value);
    }

    size_t size = ss.str().size();
    buffer.resize(size);
    ss.read(buffer.data(), size);
    return buffer;
  }

  void deserialize(std::vector<char> &buffer) {
    offset_ = 0;
    while (offset_ < buffer.size()) {
      Key key;
      Value value;
      // TODO(jmecom) Read until delimeter and then cast FromString
      // to the actual objects.
      read(buffer, &key);
//       read(buffer, " ");
      read(buffer, &value);
      (*this)[key] = value;
    }
  }

  void show(void) {
    for (auto &i : (*this)) {
      printf("%s, %s\n", i.first.str().c_str(),
        i.second.str().c_str());
    }
  }

 private:
  void writeline(std::stringstream &ss, std::string const &key,
                 std::string const &value) {
    write(ss, key);
    write(ss, ", ");
    write(ss, value);
    write(ss, "\n");
  }

  void write(std::stringstream &ss, std::string const &str) {
    ss.write(reinterpret_cast<const char *>(str.data()), str.length());
  }

  // template<class Key, class Value>
  // void read(std::vector<char> const &buffer, Key *key, Value *value) {
  //   size_t pos = 0;
  //   std::string token;
  //   while ((pos = s.find(",")) != std::string::npos) {
  //       token = s.substr(0, pos);
  //       std::cout << token << std::endl;
  //       s.erase(0, pos + ",".length());
  //   }
  // }

//   void read(std::vector<char> &buffer, std::string &str) {
//     size_t size = (int)(*(buffer.data() + offset_));
//     offset_ += sizeof(size_t);
//     std::string str2(buffer.data() + offset_, buffer.data() + offset_ + size);
//     str = str2;
//     offset_ += size;
//   }

  template<class T>
  void read(std::vector<char> &buffer, T *t) {
    // t = (T)(*(buffer.data() + offset));
    *t = *(reinterpret_cast<T *>(buffer.data() + offset_));
    offset_ += sizeof(T);
  }

  size_t offset_;
};

}  // namespace orange_tcp
