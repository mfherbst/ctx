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
#include "IsCheaplyCopyable.hh"
#include "demangle.hh"
#include "exceptions.hh"
#include <memory>
#include <type_traits>

namespace ctx {

// Forward-declare. Proper declaration in CtxMap.hh
class CtxMap;

/** \brief Class to contain an entry value in a CtxMap, i.e. the thing the
 *  key string actually points to.
 *
 * Can be constructed from elementary types by copying their value or from
 * a std::shared_ptr or a SubscriptionPointer to the object we emplace in the
 * map or from an object which is subscribable (which will automatically be
 * subscribed to.
 */
class CtxMapValue {
 public:
  /** \brief Default constructor: Constructs empty object */
  CtxMapValue() : m_object_ptr{nullptr} {}

  /** \brief Make a CtxMapValue out of a type which is cheap to copy.
   *
   * This includes std::string and all relevant numeric types (integers,
   * floating point numbers, complex numbers)
   **/
  template <typename T, typename std::enable_if<!std::is_reference<T>::value &&
                                                      IsCheaplyCopyable<T>::value,
                                                int>::type = 0>
  CtxMapValue(T t) : CtxMapValue{std::make_shared<T>(std::move(t))} {}
  // Note about the enable_if:
  //   - We need to make sure that T is the actual type (and not a
  //     reference)
  //   - T should be cheap to copy

  /** \brief Make an CtxMapValue out of a const char*.
   *
   * This behaves like the equivalent CtxMapValue of a  std::string */
  CtxMapValue(const char* s) : CtxMapValue(std::string(s)) {}

  /** \brief Make a CtxMapValue from a shared pointer */
  template <typename T, typename = typename std::enable_if<!std::is_same<
                              CtxMap, typename std::decay<T>::type>::value>::type>
  CtxMapValue(std::shared_ptr<T> t_ptr)
        : m_object_ptr(t_ptr), m_type_name(typeid(T).name()) {}

  /** \brief Make a CtxMapValue from a shared pointer */
  template <typename T, typename = typename std::enable_if<!std::is_same<
                              CtxMap, typename std::decay<T>::type>::value>::type>
  CtxMapValue(std::shared_ptr<const T> t_ptr)
        : m_object_ptr{std::const_pointer_cast<T>(t_ptr)},
          m_type_name(typeid(const T).name()) {}

  /** Make an CtxMapValue from an rvalue reference */
  template <typename T,
            typename = typename std::enable_if<
                  !std::is_reference<T>::value && !IsCheaplyCopyable<T>::value &&
                  !std::is_same<CtxMap, typename std::decay<T>::type>::value>::type>
  CtxMapValue(T&& t) : CtxMapValue{std::make_shared<T>(std::move(t))} {}
  // Note about the enable_if:
  //   - We need to make sure that T is the actual type (and not a
  //     reference)
  //   - T should not be cheap to copy (else first constructor applies)
  //   - T should not be a CtxMap (we do not want maps in maps)

  /** Obtain a non-const pointer to the internal object */
  template <typename T>
  std::shared_ptr<T> get_ptr();

  /** Obtain a const pointer to the internal object */
  template <typename T>
  std::shared_ptr<const T> get_ptr() const;

  /** Obtain a reference to the internal object */
  template <typename T>
  T& get() {
    return *get_ptr<T>();
  }

  /** Obtain a const reference to the internal object */
  template <typename T>
  const T& get() const {
    return *get_ptr<T>();
  }

  /** Return the demangled typename of the type of the internal object. */
  std::string type_name() const { return demangle(m_type_name); }

  /** Return the raw typename without demangling
   *
   * \note This is most likely not what you want. Try type_name() instead.
   **/
  const std::string& type_name_raw() const { return m_type_name; }

  bool has_value() const { return m_object_ptr != nullptr; }

 private:
  /** Check whether the object pointer stored in m_object_ptr_ptr
   *  can be obtained as a RCPWrapper<T>
   */
  template <typename T>
  bool can_get_value_as() const {
    typedef typename std::remove_const<T>::type nonconstT;

    // Allow if the type is identical to the type originally stored
    // or if a simple addition of const does the trick.
    return m_type_name == std::string(typeid(T).name()) ||
           m_type_name == std::string(typeid(nonconstT).name());
  }

  std::shared_ptr<void> m_object_ptr;
  std::string m_type_name;
};

/** Try to provide a string representation of the CtxMapValue. If this fails, just print
 * the demangled type */
std::ostream& operator<<(std::ostream& o, const CtxMapValue& value);

//
// ----------------------------------------------------------------
//

template <typename T>
std::shared_ptr<T> CtxMapValue::get_ptr() {
  if (m_object_ptr == nullptr) {
    throw runtime_error("CtxMapValue is empty.");
  }
  if (!can_get_value_as<T>()) {
    throw type_mismatch("Requested invalid type '" + demangle(typeid(T)) +
                        "' from CtxMap. The value has type '" + type_name() + "'.");
  }
  return std::static_pointer_cast<T>(m_object_ptr);
}

template <typename T>
std::shared_ptr<const T> CtxMapValue::get_ptr() const {
  if (m_object_ptr == nullptr) {
    throw runtime_error("CtxMapValue is empty.");
  }
  if (!can_get_value_as<T>()) {
    throw type_mismatch("Requested invalid type '" + demangle(typeid(T)) +
                        "' from CtxMap. The value has type '" + type_name() + "'.");
  }
  return std::static_pointer_cast<const T>(m_object_ptr);
}

}  // namespace ctx
