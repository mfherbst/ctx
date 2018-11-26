//
// Copyright (C) 2018 by the ctx authors
//
// This file is part of ctx.
//
// ctx is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// ctx is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with ctx. If not, see <http://www.gnu.org/licenses/>.
//

#include "PamMap.hh"
#include <iomanip>
#include <vector>

namespace ctx {
PamMap& PamMap::operator=(PamMap other) {
  m_location      = std::move(other.m_location);
  m_container_ptr = std::move(other.m_container_ptr);
  return *this;
}

PamMap::PamMap(const PamMap& other) : PamMap() {
  if (other.m_location == std::string("")) {
    // We are root, copy everything
    m_container_ptr = std::make_shared<map_type>(*other.m_container_ptr);
  } else {
    update(other);
  }
}

void PamMap::update(std::initializer_list<entry_type> il) {
  // Make each key a full path key and append/modify entry in map
  for (entry_type t : il) {
    (*m_container_ptr)[make_full_key(t.first)] = std::move(t.second);
  }
}

void PamMap::clear() {
  if (m_location == std::string("")) {
    // We are root, clear everything
    m_container_ptr->clear();
  } else {
    // Clear only our stuff
    erase(begin(), end());
  }
}

void PamMap::update(const std::string& key, const PamMap& other) {
  for (auto it = other.begin(); it != other.end(); ++it) {
    // The iterator truncates the other key relative to the builtin
    // location of other for us. We then make it full for our location
    // and update.
    (*m_container_ptr)[make_full_key(key + "/" + it->key())] = it->value_raw();
  }
}

void PamMap::update(const std::string& key, PamMap&& other) {
  for (auto it = other.begin(); it != other.end(); ++it) {
    // The iterator truncates the other key relative to the builtin
    // location of other for us. We then make it full for our location
    // and update.
    (*m_container_ptr)[make_full_key(key + "/" + it->key())] = std::move(it->value_raw());
  }
}

std::string PamMap::make_full_key(const std::string& key) const {
  if (m_location.length() > 0) {
    if (m_location[0] != '/' || m_location.back() == '/') {
      throw runtime_error(
            "Encountered unexpected key format: Keys passed to make_full_key should "
            "start with a /, but not end with a /.");
    }
  }

  // Make a stack out of the key:
  std::vector<std::string> pathparts;

  // start gives the location after the last '/',
  // ie where the current part of the key path begins and end gives
  // the location of the current '/', i.e. the past-the-end index
  // of the current path part.
  for (size_t start = 0; start < key.size(); ++start) {
    // Past-the-end of the current path part:
    const size_t end = key.find('/', start);

    // Empty path part (i.e. something like '//' is encountered:
    if (start == end) continue;

    // Extract the part we deal with in this iteration:
    std::string part = key.substr(start, end - start);

    // Update start for next iteration:
    start += part.length();

    if (part == ".") {
      // Ignore "." path part (does nothing)
      continue;
    } else if (part == "..") {
      // If ".." path part, then pop the most recently added path part if any.
      if (!pathparts.empty()) pathparts.pop_back();
    } else {
      pathparts.push_back(std::move(part));
    }
  }

  std::string res{m_location};
  for (const auto& part : pathparts) {
    res += "/" + part;
  }

  if (res.length() > 0) {
    if (res[0] != '/' || res.back() == '/') {
      throw runtime_error(
            "make_full_key did something unexpected: The key does not start with a / or "
            "it ends with a /");
    }
  }

  return res;
}

typename PamMap::iterator PamMap::begin(const std::string& path) {
  // Obtain iterator to the first key-value pair, which has a
  // key starting with the full path.
  //
  // (since the keys are sorted alphabetically in the map
  //  the ones which follow next must all be below our current
  //  location or already well past it.)
  const std::string path_full = make_full_key(path);
  return iterator(starting_keys_begin(*m_container_ptr, path_full), path_full);
}

typename PamMap::const_iterator PamMap::cbegin(const std::string& path) const {
  const std::string path_full = make_full_key(path);
  return const_iterator(starting_keys_begin(*m_container_ptr, path_full), path_full);
}

typename PamMap::iterator PamMap::end(const std::string& path) {
  // Obtain the first key which does no longer start with the pull path,
  // i.e. where we are done processing the subpath.
  const std::string path_full = make_full_key(path);
  return iterator(starting_keys_end(*m_container_ptr, path_full), path_full);
}

typename PamMap::const_iterator PamMap::cend(const std::string& path) const {
  const std::string path_full = make_full_key(path);
  return const_iterator(starting_keys_end(*m_container_ptr, path_full), path_full);
}

std::ostream& operator<<(std::ostream& o, const PamMap& map) {
  size_t maxlen = 0;
  for (auto& kv : map) {
    maxlen = std::max(maxlen, kv.key().size());
  }

  for (auto& kv : map) {
    o << std::setw(maxlen) << std::left << kv.key() << "  :  " << kv.value_raw() << "\n";
  }
  return o;
}

}  // namespace ctx
