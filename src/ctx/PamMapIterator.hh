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

#pragma once
#include "PamMapAccessor.hh"
#include <iterator>
#include <map>
#include <type_traits>

namespace ctx {

template <bool Const>
class PamMapIterator
      : std::iterator<std::bidirectional_iterator_tag, PamMapAccessor<Const>> {
 public:
  typedef PamMapValue entry_value_type;
  typedef std::map<std::string, entry_value_type> map_type;

  /** The iterator type which is used in this class
   * to iterate over the map contained in PamMap. */
  typedef typename std::conditional<Const, typename map_type::const_iterator,
                                    typename map_type::iterator>::type iter_type;

  /** Dereference PamMap iterator */
  PamMapAccessor<Const>& operator*() const { return *operator->(); }

  /** Obtain pointer to PamMap accessor */
  PamMapAccessor<Const>* operator->() const;

  /** Prefix increment to the next key */
  PamMapIterator& operator++() {
    ++m_iter;
    m_acc_ptr.reset();  // Reset cache
    return *this;
  }

  /** Postfix increment to the next key */
  PamMapIterator operator++(int) {
    PamMapIterator copy(*this);
    this->operator++();
    return copy;
  }

  /** Prefix decrement to the next key */
  PamMapIterator& operator--() {
    --m_iter;
    m_acc_ptr.reset();  // Reset cache
    return *this;
  }

  /** Postfix decrement to the next key */
  PamMapIterator operator--(int) {
    PamMapIterator copy(*this);
    this->operator--();
    return copy;
  }

  bool operator==(const PamMapIterator& other) const { return m_iter == other.m_iter; }
  bool operator!=(const PamMapIterator& other) const { return m_iter != other.m_iter; }

  /** Explicit conversion to the inner iterator type */
  explicit operator iter_type() { return m_iter; }

  PamMapIterator(iter_type iter, std::string location)
        : m_acc_ptr(nullptr), m_iter(iter), m_location(std::move(location)) {}

  PamMapIterator() : m_acc_ptr(nullptr), m_iter(), m_location() {}

 private:
  /** Undo the operation of PamMap::make_full_key, i.e. strip off the
   * first location part and get a relative path to it*/
  std::string strip_location_prefix(const std::string& key) const;

  /** Cache for the accessor of the current value.
   *  A stored nullptr implies that the accessor needs to rebuild
   *  before using it.*/
  mutable std::shared_ptr<PamMapAccessor<Const>> m_acc_ptr;

  /** Iterator to the current key,value pair */
  iter_type m_iter;

  /** Subtree location we iterate over */
  std::string m_location;
};

//
// -----------------------------------------------
//

template <bool Const>
PamMapAccessor<Const>* PamMapIterator<Const>::operator->() const {
  if (m_acc_ptr == nullptr) {
    // Generate accessor for current state
    const std::string key_stripped = strip_location_prefix(m_iter->first);
    m_acc_ptr = std::make_shared<PamMapAccessor<Const>>(key_stripped, m_iter->second);
  }

  return m_acc_ptr.get();
}

template <bool Const>
std::string PamMapIterator<Const>::strip_location_prefix(const std::string& key) const {
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
