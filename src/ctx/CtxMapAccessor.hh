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
#include "CtxMapValue.hh"
#include <memory>
#include <string>

namespace ctx {

/** Accessor to a CtxMap object. Can be used to retrieve the key or the value
 *  or the typename of the value */
template <bool Const>
class CtxMapAccessor {};

template <>
class CtxMapAccessor<true> {
 public:
  /** Return the key of the key/value pair the accessor holds */
  const std::string& key() const { return m_key; }

  /** Return the type name of the value object referred to by the key, which
   * is held in this accessor.
   */
  std::string type_name() const { return m_value.type_name(); }

  /** Return the value of the key/value pair the accessor holds (Const version).
   *
   * Raises an exception if the value object referred to by this key
   * cannot be converted to this value type.
   **/
  template <typename T>
  const T& value() const {
    return m_value.get<T>();
  }

  /** Return the value of the key/value pair the accessor holds
   * as a pointer (Const version).
   *
   * Raises an exception if the value object referred to by this key
   * cannot be converted to this value type.
   **/
  template <typename T>
  std::shared_ptr<const T> value_ptr() const {
    return m_value.get_ptr<T>();
  }

  /** Return a reference to the raw value object the accessor holds. (Const version)
   *
   * \note This is an advanced method. Use only if you know what you are doing.
   **/
  const CtxMapValue& value_raw() const { return m_value; }

  /** Construct an accessor */
  CtxMapAccessor(const std::string key, const CtxMapValue& value)
        : m_key(key), m_value(value) {}

 private:
  const std::string m_key;
  const CtxMapValue& m_value;
};

template <>
class CtxMapAccessor<false> : public CtxMapAccessor<true> {
  // Use the const version as the base type:
  typedef CtxMapAccessor<true> base_type;

 public:
  /** Return the value of the key/value pair the accessor holds.
   *
   * Raises an exception if the value object referred to by this key
   * cannot be converted to this value type.
   **/
  template <typename T>
  T& value() {
    return m_value.get<T>();
  }

  /** Return the value of the key/value pair the accessor holds
   * as a pointer.
   *
   * Raises an exception if the value object referred to by this key
   * cannot be converted to this value type.
   **/
  template <typename T>
  std::shared_ptr<T> value_ptr() {
    return m_value.get_ptr<T>();
  }

  /** Return the value of the key/value pair the accessor holds (Const version).
   *
   * Raises an exception if the value object referred to by this key
   * cannot be converted to this value type.
   **/
  template <typename T>
  const T& value() const {
    return m_value.get<T>();
  }

  /** Return the value of the key/value pair the accessor holds
   * as a pointer (Const version).
   *
   * Raises an exception if the value object referred to by this key
   * cannot be converted to this value type.
   **/
  template <typename T>
  std::shared_ptr<const T> value_ptr() const {
    return m_value.get_ptr<T>();
  }

  /** Return a reference to the raw value object the accessor holds. ( Const version)
   *
   * \note This is an advanced method. Use only if you know what you are doing.
   **/
  const CtxMapValue& value_raw() const { return m_value; }

  /** Return a reference to the raw value object the accessor holds.
   *
   * \note This is an advanced method. Use only if you know what you are doing.
   **/
  CtxMapValue& value_raw() { return m_value; }

  /** Construct an accessor */
  CtxMapAccessor(const std::string key, CtxMapValue& value)
        : base_type(key, value), m_value(value) {}

 private:
  CtxMapValue& m_value;
};

}  // namespace ctx
