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
#include "rc_ptr.h"
#include "root_storage.h"
#include <krims/RCPWrapper.hh>

namespace ctx {

/** \brief context object which resembles the libctx::context
 *  object by Epifanovsky et. al.
 *
 *  The purpose is to provide a libctx-like interface to our
 *  krims::ParameterMap, which is in fact typedefed to ``root_storage``.
 *
 *  We also use C++ 11 smart pointers instead of their rc_ptr objects.
 */
class context {
 public:
  /** Exception thrown if a key already exists */
  DefException1(ExcExistingKey, std::string,
                << "The key " << arg1
                << " already exists in the context. Use update() to update the key.");
  typedef krims::ParameterMap::ExcUnknownKey ExcUnknownKey;

  /** \name Constructors */
  ///@{
  /** Create from root_storage, i.e. ParameterMap */
  context(root_storage& stor) : m_map_ptr{krims::make_subscription(stor, "context")} {}

  /** Create a subcontext of the referenced context */
  context(const context& ctx, const std::string& base)
        : m_map_ptr{std::make_shared<root_storage>(ctx.m_map_ptr->submap(base))} {}
  ///@}

  /** \name Modify elements */
  ///@{
  /** \brief Insert an element into the context.
   * \note The key may not exist yet
   */
  template <typename T>
  void insert(const std::string& key, rc_ptr<T> ptr) {
    assert_throw(!m_map_ptr->exists(key), ExcExistingKey(key));
    m_map_ptr->update(key, ptr);
  }

  /** \brief Update (replace) an existing element in the context.
   */
  template <typename T>
  void update(const std::string& key, rc_ptr<T> ptr) {
    assert_throw(m_map_ptr->exists(key), ExcUnknownKey(key));
    m_map_ptr->update(key, ptr);
  }

  /** Erase a single element from the context */
  void erase(const std::string& key) {
    size_t count = m_map_ptr->erase(key);
    assert_throw(count == 1, ExcUnknownKey(key));
  }

  /** Erase a full path, i.e. all elements located under given path */
  void erase_path(const std::string& path) { m_map_ptr->erase_recursive(path); }
  ///@}

  /** \name Obtain and copy elements */
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
  void copy(const std::string& key_from, context& to, const std::string& key_to) {
    assert_throw(!to.m_map_ptr->exists(key_to), ExcExistingKey(key_to));
    to.m_map_ptr->update(key_to, m_map_ptr->at_raw_value(key_from));
  }
  ///@}

  /** If the key exists return true, else false */
  bool key_exists(const std::string& key) const { return m_map_ptr->exists(key); }

  /** Return a reference to the krims::ParameterMap used to store the values */
  krims::ParameterMap& map() { return *m_map_ptr; }

  /** Return a const reference to the krims::ParameterMap used to store the values */
  const krims::ParameterMap& map() const { return *m_map_ptr; }

 private:
  krims::RCPWrapper<root_storage> m_map_ptr;
};

}  // namespace ctx
