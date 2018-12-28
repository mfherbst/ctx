//
// Copyright 2018 Michael F. Herbst
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

#pragma once
#include "CtxMapAccessor.hh"
#include <iterator>
#include <map>
#include <type_traits>

namespace ctx {

template <bool Const>
class CtxMapIterator
      : std::iterator<std::bidirectional_iterator_tag, CtxMapAccessor<Const>> {
 public:
  typedef CtxMapValue entry_value_type;
  typedef std::map<std::string, entry_value_type> map_type;

  /** The iterator type which is used in this class
   * to iterate over the map contained in CtxMap. */
  typedef typename std::conditional<Const, typename map_type::const_iterator,
                                    typename map_type::iterator>::type iter_type;

  /** Dereference CtxMap iterator */
  CtxMapAccessor<Const>& operator*() const { return *operator->(); }

  /** Obtain pointer to CtxMap accessor */
  CtxMapAccessor<Const>* operator->() const;

  /** Prefix increment to the next key */
  CtxMapIterator& operator++() {
    ++m_iter;
    m_acc_ptr.reset();  // Reset cache
    return *this;
  }

  /** Postfix increment to the next key */
  CtxMapIterator operator++(int) {
    CtxMapIterator copy(*this);
    this->operator++();
    return copy;
  }

  /** Prefix decrement to the next key */
  CtxMapIterator& operator--() {
    --m_iter;
    m_acc_ptr.reset();  // Reset cache
    return *this;
  }

  /** Postfix decrement to the next key */
  CtxMapIterator operator--(int) {
    CtxMapIterator copy(*this);
    this->operator--();
    return copy;
  }

  bool operator==(const CtxMapIterator& other) const { return m_iter == other.m_iter; }
  bool operator!=(const CtxMapIterator& other) const { return m_iter != other.m_iter; }

  /** Explicit conversion to the inner iterator type */
  explicit operator iter_type() { return m_iter; }

  CtxMapIterator(iter_type iter, std::string location)
        : m_acc_ptr(nullptr), m_iter(iter), m_location(std::move(location)) {}

  CtxMapIterator() : m_acc_ptr(nullptr), m_iter(), m_location() {}

 private:
  /** Undo the operation of CtxMap::make_full_key, i.e. strip off the
   * first location part and get a relative path to it*/
  std::string strip_location_prefix(const std::string& key) const;

  /** Cache for the accessor of the current value.
   *  A stored nullptr implies that the accessor needs to rebuild
   *  before using it.*/
  mutable std::shared_ptr<CtxMapAccessor<Const>> m_acc_ptr;

  /** Iterator to the current key,value pair */
  iter_type m_iter;

  /** Subtree location we iterate over */
  std::string m_location;
};

//
// -----------------------------------------------
//

template <bool Const>
CtxMapAccessor<Const>* CtxMapIterator<Const>::operator->() const {
  if (m_acc_ptr == nullptr) {
    // Generate accessor for current state
    const std::string key_stripped = strip_location_prefix(m_iter->first);
    m_acc_ptr = std::make_shared<CtxMapAccessor<Const>>(key_stripped, m_iter->second);
  }

  return m_acc_ptr.get();
}

template <bool Const>
std::string CtxMapIterator<Const>::strip_location_prefix(const std::string& key) const {
  // The first part needs to be exactly the location:
  if (key.size() < m_location.size() ||
      0 != key.compare(0, m_location.size(), m_location)) {
    throw internal_error("key does not start with the expected location prefix.");
  }

  if (key.size() <= m_location.size()) {
    return "/";
  } else {
    std::string res = key.substr(m_location.size());

    if (res.length() > 0) {
      if (res[0] != '/' || res.back() == '/') {
        throw internal_error(
              "strip_location_prefix did something unexpected: The key does not start "
              "with a / or "
              "it ends with a /");
      }  // if
    }    // if
    return res;
  }
}

}  // namespace ctx
