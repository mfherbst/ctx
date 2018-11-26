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
#include <complex>
#include <type_traits>

namespace ctx {

/* Using statement we need for some SFINAE
 * (substitution failure is not an error)
 * */
template <typename... Ts>
using VoidType = void;

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
