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
#include "rc_ptr.h"
#include "root_storage.h"
#include <ctx/exceptions.hh>
#include <iostream>
#include <vector>

namespace libctx {

/** \brief context object which resembles the libctx::context
 *  object by Epifanovsky et. al.
 *
 *  The purpose is to provide a libctx-like interface to our
 *  CtxMap, which is in fact typedefed to ``root_storage``.
 *
 *  We also use C++ 11 smart pointers instead of their rc_ptr objects.
 */
class context {
 public:
  /** \name Constructors */
  ///@{
  /** Create from root_storage, i.e. CtxMap */
  context(root_storage& stor)
        : m_map_ptr{std::make_shared<root_storage>(stor.submap(""))}, m_location{""} {}

  /** Create a subcontext of the referenced context */
  context(const context& ctx, const std::string& base)
        : m_map_ptr{std::make_shared<root_storage>(ctx.m_map_ptr->submap(base))},
          m_location{ctx.m_location + "/" + base} {}
  ///@}

  /** \name Modify elements */
  ///@{
  /** \brief Insert an element into the context.
   * \note The key may not exist yet
   */
  template <typename T>
  void insert(const std::string& key, rc_ptr<T> ptr) {
    if (m_map_ptr->exists(key)) {
      throw ctx::invalid_argument("Key '" + key +
                                  "' already exists. Use update() to update its value.");
    }
    m_map_ptr->update(key, ptr);
  }

  /** \brief Update (replace) an existing element in the context.
   */
  template <typename T>
  void update(const std::string& key, rc_ptr<T> ptr) {
    if (!m_map_ptr->exists(key)) {
      throw ctx::out_of_range("Key '" + key + "' is not known.");
    }
    m_map_ptr->update(key, ptr);
  }

  /** Erase a single element from the context */
  void erase(const std::string& key);

  /** Erase a full path, i.e. all elements located under given path */
  void erase_path(const std::string& path) { m_map_ptr->erase_recursive(path); }
  ///@}

  /** \name Obtain, copy and move elements */
  ///@{

  /** Obtain an element from the context. */
  template <typename T>
  rc_ptr<T> get(const std::string& key) {
    return static_cast<rc_ptr<T>>(m_map_ptr->at_ptr<T>(key));
  }

  /** Make a (shallow) copy of an object inside the same context
   *
   * In other words both ``key_from`` and ``key_to`` now
   * point to the same object in memory
   */
  void copy(const std::string& key_from, const std::string& key_to) {
    copy(key_from, *this, key_to);
  }

  /** Make a (shallow) copy of an object to another context.
   *
   * In other words both ``key_from`` (in *this) and ``key_to``
   * (in ``to``) now point to the same object in memory
   */
  void copy(const std::string& key_from, context& to, const std::string& key_to);

  /** Move an object from one key to another */
  void move(const std::string& key_from, const std::string& key_to);

  /** Move an object from one context to another */
  void move(const std::string& key_from, context& to, const std::string& key_to) {
    to.m_map_ptr->update(key_to, m_map_ptr->at_raw_value(key_from));
    m_map_ptr->erase(key_from);
  }
  ///@}

  /** If the key exists return true, else false */
  bool key_exists(const std::string& key) const { return m_map_ptr->exists(key); }

  /** Return the current location of the context relative to the root storage */
  const std::string& whereami() const { return m_location; }

  /** Returns the vector of all keys that exist in the context */
  void get_keys(std::vector<std::string>& keys) const;

  /** Print the contents of the context to the output stream */
  void print(std::ostream& os = std::cout) const;

  /** Return a reference to the CtxMap used to store the values */
  CtxMap& map() { return *m_map_ptr; }

  /** Return a const reference to the CtxMap used to store the values */
  const CtxMap& map() const { return *m_map_ptr; }

 private:
  std::shared_ptr<root_storage> m_map_ptr;
  std::string m_location;
};

inline std::ostream& operator<<(std::ostream& o, context& c) {
  o << c.map();
  return o;
}

}  // namespace libctx
