#pragma once

#include <iostream>
#include <vector>
#include <map>
#include <sstream>
#include <string>
#include <string.h>

namespace orange_tcp {

// Simple on-disk key-value store for ARP cache and routing table.
// Stored type must support a FromString() method.
// Loosely based on:
//   https://gist.github.com/kaityo256/eb6a49cb40f99b97898f5e464c2c208f
template<class Key, class Value>
class serializable_map : public std::map<Key, Value> {
 public:
  std::vector<char> serialize() {
    std::vector<char> buffer;
    std::stringstream ss;
    for (auto &i : (*this)) {
      Key key = i.first;
      Value value = i.second;
      writeline(ss, key, value);
    }

    size_t size = ss.str().size();
    buffer.resize(size);
    ss.read(buffer.data(), size);
    return buffer;
  }

  void deserialize(std::vector<char> const &buffer) {
    offset_ = 0;
    remaining_ = buffer.size();
    while (offset_ < buffer.size()) {
      Key key;
      Value value;

      readuntil(buffer.data() + offset_, &key, ',');
      offset_ += 2;  // for ', '
      remaining_ -= 2;
      readuntil(buffer.data() + offset_, &value, '\n');
      offset_ += 1;  // for '\n'
      remaining_ -= 1;

      (*this)[key] = value;
    }
  }

  void show() {
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

  std::string substr(const char *buffer, size_t size, char delim) {
    std::string out;
    for (size_t i = 0; i < size; i++) {
      if (buffer[i] == delim) {
        out.resize(i);
        memcpy(out.data(), buffer, i);
        break;
      }
    }
    return out;
  }

  template<class T>
  void readuntil(const char *buffer, T *out, char delim) {
    std::string s = substr(buffer, remaining_, delim);
    *out = T::FromString(s);
    offset_ += s.size();
    remaining_ -= s.size();
  }

  size_t offset_;
  size_t remaining_;
};

}  // namespace orange_tcp
