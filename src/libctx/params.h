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

#include <ctx/demangle.hh>
#include <iostream>
#include <map>
#include <sstream>
#include <vector>

// Note: Instead of using smart std::unique_ptr<CtxMap>, this implementation
//       deliberately uses raw CtxMap pointers to make sure that C++11 is not
//       needed to compile this file.

namespace ctx {
/** Forward declaration to avoid using ctx/CtxMap header (and thus C++11) */
class CtxMap;
}  // namespace ctx

namespace libctx {

/** \brief Routine parameters
 *
 *   Internally, this class keeps all the parameters as strings. The
 *   conversion to the target type is done by using string streams.
 */
class params {
 public:
  /**  \name Constructors */
  ///@{
  /** \brief Create an empty parameter tree */
  params();

  /** Create a deep copy of a CtxMap.
   *
   * We copy all values, so the input CtxMap and
   * the constructed object have no internal relationship
   * and operate on different memory.
   *
   * \note The CtxMap may only contain string values,
   *       i.e. all keys should map to a value type of
   *       std::string
   */
  explicit params(const ctx::CtxMap& map);

  /** \brief Create a deep copy of a parameter tree */
  params(const params& other);

  /** Assignment operator */
  params& operator=(params p);

  /** Destructor */
  ~params();
  ///@}

  /** \name Existence checks */
  ///@{
  /** Return true if the key identifies a subtree, else false */
  bool subtree_exists(const std::string& key) const;

  /**	\brief Returns true if a value with a given key exists,
          false otherwise.
   **/
  bool key_exists(const std::string& key) const;
  ///@}

  /** \brief Return a subtree (const version)
   *
   * Throws a out_of_range exception if the key does not refer
   * to a subtree
   **/
  const params& get_subtree(const std::string& key) const;

  /** \brief Return a subtree.
   *
   * Creates one if required.
   **/
  params& get_subtree(const std::string& key);

  /** Return the value indentified by a key as a plain string. */
  const std::string& get(const std::string& key) const { return get_str(key); }

  /** Return the value indentified by a key as a plain string. */
  const std::string& get_str(const std::string& key) const;

  /**  Convert the string value referenced by key to the requestet type ``T``
   *   and return the result.
   */
  template <typename T>
  T get(const std::string& key) const;

  /** Set an entry. The value is converted to a string before setting it. */
  template <typename T>
  void set(const std::string& key, const T& value);

  /** Set an entry. */
  void set(const std::string& key, const std::string& value);

  /** Erase a single key if it exists. Nothing happens if the key is absent */
  void erase_value(const std::string& key);

  /** Get the underlying parameter map.
   *
   * Note that all data is stored
   * as strings, so you will only be able to get strings from the
   * map, i.e. you need to access the data via the at<std::string>
   * function.
   */
  ctx::CtxMap& map() { return *m_map_ptr; }

  /** Get the underlying parameter map. (Const version)
   *
   * See non-const version above for details.
   */
  const ctx::CtxMap& map() const { return *m_map_ptr; }

  /** Parse the value referred to by the key to a vector of
   *  arbitrary type and return it.
   *
   *  The expectation is that the different elements are separated by
   *  spaces, tab or similar, which can be parsed by the default formatted
   *  stream extraction operators.
   *
   *  Missing keys are skipped, i.e. they do *not* throw an exception,
   *  but return an empty vector.
   */
  template <typename T>
  std::vector<T> get_vec(const std::string& key) const;

  /** Parse the value referred to by the key to a vector of arbitrary type.
   *  The vector provided by reference is modified and the number of
   *  elements in the vector is returned.
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
  std::string normalise_key(const std::string& raw_key) const;

  /** Obtain a reference to a subtree from the subtree cache.
   *  Make a new one in case of a cache miss or if force_renew is true. */
  params& get_cached_subtree(const std::string& normalised,
                             bool force_renew = false) const;

  //! The CtxMap representing this tree. Contains only strings.
  ctx::CtxMap* m_map_ptr;

  //! Cache for subtree objects.
  mutable std::map<std::string, params> m_subtree_cache;
};

/** Helper function to throw a ctx::type_mismatch exception
 *
 * Needed to hide the ctx interface from this header and make it
 * compile smoothly without C++11.
 **/
void throw_type_mismatch(const std::string& str);

//
// ------------------------------------------------------------------
//

template <typename T>
T params::get(const std::string& key) const {
  // Get the key value and add a terminating white space
  std::istringstream ss(get_str(key) + " ");
  T t;
  if (!(ss >> t)) {
    throw_type_mismatch("Could not convert the value \"" + get_str(key) +
                        "\" for key \"" + key + "\" to the requested type '" +
                        ctx::demangle(typeid(T)) + "'");
  }
  return t;
}

template <typename T>
void params::set(const std::string& key, const T& val) {
  std::ostringstream ss;
  if (!(ss << val)) {
    throw_type_mismatch("Could not convert the value for key \"" + key + "\" to string.");
  }
  set(key, ss.str());
}

template <typename T>
std::vector<T> params::get_vec(const std::string& key) const {
  if (!key_exists(key)) return std::vector<T>();

  // Get the key value and add a terminating white space
  std::vector<T> ret;
  std::istringstream ss(get_str(key) + " ");
  ss >> std::ws;  // Skip leading whitespace.

  while (!ss.eof()) {
    T val;
    ss >> val >> std::ws;
    if (!ss) {
      std::ostringstream oss;
      oss << val;

      throw_type_mismatch("Could not convert the value \"" + get_str(key) +
                          "\" for key \"" + key + "\" to the requested type vector<" +
                          ctx::demangle(typeid(T)) +
                          ">. Error occurred for parsing value '" + oss.str() + "'.");
    }
    ret.push_back(val);
  }
  return ret;
}
}  // namespace libctx
