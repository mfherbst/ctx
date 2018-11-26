//
// Copyright (C) 2018 by Michael F. Herbst
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
#include "IsCheaplyCopyable.hh"
#include "demangle.hh"
#include "exceptions.hh"
#include <memory>
#include <type_traits>

namespace ctx {

// Forward-declare. Proper declaration in PamMap.hh
class PamMap;

/** \brief Class to contain an entry value in a PamMap, i.e. the thing the
 *  key string actually points to.
 *
 * Can be constructed from elementary types by copying their value or from
 * a std::shared_ptr or a SubscriptionPointer to the object we emplace in the
 * map or from an object which is subscribable (which will automatically be
 * subscribed to.
 */
class PamMapValue {
 public:
  /** \brief Default constructor: Constructs empty object */
  PamMapValue() : m_object_ptr{nullptr} {}

  /** \brief Make a PamMapValue out of a type which is cheap to copy.
   *
   * This includes std::string and all relevant numeric types (integers,
   * floating point numbers, complex numbers)
   **/
  template <typename T, typename std::enable_if<!std::is_reference<T>::value &&
                                                      IsCheaplyCopyable<T>::value,
                                                int>::type = 0>
  PamMapValue(T t) : PamMapValue{std::make_shared<T>(std::move(t))} {}
  // Note about the enable_if:
  //   - We need to make sure that T is the actual type (and not a
  //     reference)
  //   - T should be cheap to copy

  /** \brief Make an PamMapValue out of a const char*.
   *
   * This behaves like the equivalent PamMapValue of a  std::string */
  PamMapValue(const char* s) : PamMapValue(std::string(s)) {}

  /** \brief Make a PamMapValue from a shared pointer */
  template <typename T, typename = typename std::enable_if<!std::is_same<
                              PamMap, typename std::decay<T>::type>::value>::type>
  PamMapValue(std::shared_ptr<T> t_ptr)
        : m_object_ptr(t_ptr), m_type_name(typeid(T).name()) {}

  /** \brief Make a PamMapValue from a shared pointer */
  template <typename T, typename = typename std::enable_if<!std::is_same<
                              PamMap, typename std::decay<T>::type>::value>::type>
  PamMapValue(std::shared_ptr<const T> t_ptr)
        : m_object_ptr{std::const_pointer_cast<T>(t_ptr)},
          m_type_name(typeid(const T).name()) {}

  /** Make an PamMapValue from an rvalue reference */
  template <typename T,
            typename = typename std::enable_if<
                  !std::is_reference<T>::value && !IsCheaplyCopyable<T>::value &&
                  !std::is_same<PamMap, typename std::decay<T>::type>::value>::type>
  PamMapValue(T&& t) : PamMapValue{std::make_shared<T>(std::move(t))} {}
  // Note about the enable_if:
  //   - We need to make sure that T is the actual type (and not a
  //     reference)
  //   - T should not be cheap to copy (else first constructor applies)
  //   - T should not be a PamMap (we do not want maps in maps)

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

/** Try to provide a string representation of the PamMapValue. If this fails, just print
 * the demangled type */
std::ostream& operator<<(std::ostream& o, const PamMapValue& value);

//
// ----------------------------------------------------------------
//

template <typename T>
std::shared_ptr<T> PamMapValue::get_ptr() {
  if (m_object_ptr == nullptr) {
    throw runtime_error("PamMapValue is empty.");
  }
  if (!can_get_value_as<T>()) {
    throw type_mismatch("Requested invalid type '" + demangle(typeid(T)) +
                        "' from PamMap. The value has type '" + type_name() + "'.");
  }
  return std::static_pointer_cast<T>(m_object_ptr);
}

template <typename T>
std::shared_ptr<const T> PamMapValue::get_ptr() const {
  if (m_object_ptr == nullptr) {
    throw runtime_error("PamMapValue is empty.");
  }
  if (!can_get_value_as<T>()) {
    throw type_mismatch("Requested invalid type '" + demangle(typeid(T)) +
                        "' from PamMap. The value has type '" + type_name() + "'.");
  }
  return std::static_pointer_cast<const T>(m_object_ptr);
}

}  // namespace ctx
