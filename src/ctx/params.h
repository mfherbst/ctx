//
// Copyright (C) 2017 by the ctx authors
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

#include "libctx_namespace.hh"
#include <iostream>
#include <krims/ParameterMap.hh>
#include <krims/make_unique.hh>
#include <map>
#include <sstream>
#include <vector>

namespace ctx {

/** \brief Routine parameters
 *
 *   Internally, this class keeps all the parameters as strings. The
 *   conversion to the target type is done by using string streams.
 */
class params {
 public:
  /** \name Exception definitions */
  //@{
  typedef krims::ParameterMap::ExcUnknownKey ExcUnknownKey;
  DefException2(ExcConversionFailed, std::string, std::string,
                << "Could not convert the value \"" << arg2 << "\" for key \"" << arg1
                << "\" to the requested type");
  DefExceptionMsg(ExcEmptyKey, "An empty key is not allowed.");
  //@}

  typedef krims::ParameterMap::KeyIterator const_iterator;

  /**  \name Constructors */
  ///@{
  /**	\brief Create an empty parameter tree */
  params() : m_map_ptr{krims::make_unique<krims::ParameterMap>()}, m_subtree_cache{} {}

  /** Create a deep copy of a krims::ParameterMap.
   *
   * We copy all values, so the input ParameterMap and
   * the constructed object have no internal relationship
   * and operate on different memory.
   *
   * \note The ParameterMap may only contain string values,
   *       i.e. all keys should map to a value type of
   *       std::string
   */
  explicit params(const krims::ParameterMap& map) : params() {
    for (auto itkey = map.begin_keys(); itkey != map.end_keys(); ++itkey) {
      m_map_ptr->update(*itkey, map.at<std::string>(*itkey));
    }
  }

  /**	\brief Create a deep copy of a parameter tree */
  params(const params& other) : params(*other.m_map_ptr) {}

  params(params&& other) = default;

  /** Assignment operator */
  params& operator=(params p) {
    m_map_ptr = std::move(p.m_map_ptr);
    m_subtree_cache.clear();
    return *this;
  }
  ///@}

  /** \name Existence checks */
  ///@{
  /** Return true if the key identifies a subtree, else false */
  bool subtree_exists(const std::string& key) const;

  /**	\brief Returns true if a value with a given key exists,
          false otherwise.
   **/
  bool key_exists(const std::string& key) const {
    // The key indentifies a value if it plainly exists and returns a value:
    return m_map_ptr->exists(key);
  }
  ///@}

  /** \name Get data */
  ///@{
  //  /** \brief Returns iterator to start of the key range */
  //  const_iterator begin() const { return m_map_ptr->begin_keys(); }
  //
  //  /** \brief Returns iterator to end */
  //  const_iterator end() const { return m_map_ptr->end_keys(); }

  /** \brief Return a subtree (const version)
   *
   * Throws a ExcUnknownKey if the key does not refer
   * to a subtree
   **/
  const params& get_subtree(const std::string& key) const {
    assert_throw(subtree_exists(key), ExcUnknownKey(key));
    auto it = m_subtree_cache.find(normalise_key(key));
    assert_throw(it != std::end(m_subtree_cache), krims::ExcInternalError());
    return it->second;
  }

  /** \brief Return a subtree.
   *
   * Creates one if required.
   **/
  params& get_subtree(const std::string& key);

  /** Return the value indentified by a key as a plain string. */
  const std::string& get(const std::string& key) const { return get_str(key); }

  /** Return the value indentified by a key as a plain string. */
  const std::string& get_str(const std::string& key) const {
    return m_map_ptr->at<std::string>(key);
  }

  /**  Convert the string value referenced by key to the requestet type ``T``
   *   and return the result.
   *
   *   If the conversion fails an ExcInvalidConversion is thrown.
   */
  template <typename T>
  T get(const std::string& key) const;

  ///@}

  /** Set an entry. The value is converted to a string before setting it.
   *
   * If the conversion fails an ExcInvalidConversion is thrown. */
  template <typename T>
  void set(const std::string& key, const T& value);

  /** Set an entry. */
  void set(const std::string& key, const std::string& value) {
    m_map_ptr->update(key, value);
  }

  /** Erase a single key if it exists. Nothing happens if the key is absent */
  void erase_value(const std::string& key) { m_map_ptr->erase(key); }

  /** Get the underlying parameter map.
   *
   * Note that all data is stored
   * as strings, so you will only able to get<std::string> from
   * the map.
   */
  krims::ParameterMap& map() { return *m_map_ptr; }

  /** Get the underlying parameter map. (Const version)
   *
   * See non-const version above for details.
   */
  const krims::ParameterMap& map() const { return *m_map_ptr; }

  /** Parse the value referred to by the key to a vector of
   *  arbitrary type and return it.
   *
   *  The expectation is that the different elements are separated by
   *  spaces, tab or similar, which can be parsed by the default formatted
   *  stream extraction operators.
   *
   *  Missing keys are skipped, i.e. they do *not* throw an exception,
   *  but return an empty vector.
   *
   *  If the conversion fails an ExcConversionFailed exception is thrown.
   */
  template <typename T>
  std::vector<T> get_vec(const std::string& key) const;

  /** Parse the value referred to by the key to a vector of arbitrary type.
   *  The vector provided by reference is modified and the number of
   *  elements in the vector is returned.
   *
   *  If the conversion fails an ExcConversionFailed exception is thrown.
   */
  template <typename T>
  size_t get_vec(const std::string& key, std::vector<T>& vec) const {
    vec = get_vec<T>(key);
    return vec.size();
  }

  /** Merge another parameter tree under the given key by updating
   *  all values. Return a reference to the subtree.
   **/
  params& merge_subtree(const std::string& key, const params& from);

  // Make printing data a friend.
  friend std::ostream& operator<<(std::ostream& os, const params& p);

 private:
  /** Internal constructor to make a params object from an already existent
   *  pointer to a  ParameterMap */
  params(std::unique_ptr<krims::ParameterMap> map_ptr)
        : m_map_ptr(std::move(map_ptr)), m_subtree_cache() {}

  std::string normalise_key(const std::string& raw_key) const {
    return raw_key[0] == '/' ? raw_key : "/" + raw_key;
  }

  //! The ParameterMap representing this tree. Contains only strings.
  std::unique_ptr<krims::ParameterMap> m_map_ptr;

  //! Cache for subtree objects.
  mutable std::map<std::string, params> m_subtree_cache;
};

//
// ------------------------------------------------------------------
//

template <typename T>
T params::get(const std::string& key) const {
  T t;
  const bool success = (std::istringstream(get_str(key)) >> t);
  assert_throw(success, ExcConversionFailed(key, get_str(key)));
  return t;
}

template <typename T>
void params::set(const std::string& key, const T& val) {
  std::ostringstream ss;
  assert_throw(ss << val, ExcConversionFailed(key, "<conversion_error>"));
  set(key, ss.str());
}

template <typename T>
std::vector<T> params::get_vec(const std::string& key) const {
  if (!key_exists(key)) return std::vector<T>();

  std::vector<T> ret;
  std::istringstream ss(get_str(key));
  ss >> std::ws;  // Skip leading whitespace.

  while (!ss.eof()) {
    T val;
    ss >> val >> std::ws;
    assert_throw(ss, ExcConversionFailed(key, get_str(key)));
    ret.push_back(std::move(val));
  }
  return ret;
}
}  // namespace ctx
