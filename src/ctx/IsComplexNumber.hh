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
#include <complex>
#include <type_traits>

namespace ctx {

namespace detail {
/** Workaround make_void class required in some versions of gcc
 *  (most notably gcc <5) to get the ``using VoidType = void`` idiom
 *  to work.
 *  For gcc 5 and most clang versions this could be avoided and else
 *  we could use the simpler version:
 *  ```
 *  template <typename... Ts>
 *  using VoidType = void.
 *  ```
 *  See https://stackoverflow.com/questions/35753920/
 *  why-does-the-void-t-detection-idiom-not-work-with-gcc-4-9
 *  for details.
 *
 *  The basic reason is that the simpler code above does not actually
 *  make use of the Ts, so their substitution is not a failure, hence
 *  SFINAE is not triggered.
 */
template <typename... Ts>
struct make_void {
  using type = void;
};
}  // namespace detail

/* Using statement needed for SFINAE patterns
 *
 * See also the documentation of \t detail::make_void, why it is
 * so complicated.
 */
template <typename... Ts>
using VoidType = typename detail::make_void<Ts...>::type;

//@{
/** \brief struct representing a type (std::true_type, std::false_type) which
 *  indicates whether T is a complex number type
 *
 * The definition is done using SFINAE, such that even for types not having a
 * typedef value_type this expression is valid.
 *  */
template <typename T, typename = void>
struct IsComplexNumber : public std::false_type {};

template <typename T>
struct IsComplexNumber<T, VoidType<typename T::value_type>>
      : public std::integral_constant<
              bool, std::is_arithmetic<typename T::value_type>::value &&
                          std::is_same<std::complex<typename T::value_type>, T>::value> {
};
//@}
}  // namespace ctx
