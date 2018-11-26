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
#include "CheaplyCopyable_i.hh"
#include "IsComplexNumber.hh"
#include <string>

namespace ctx {
//@{
/** \brief struct representing a type (std::true_type, std::false_type) which
 *  indicates whether it is cheap to copy the type T
 *
 *  All arithmetic types (float, double, long double, all integers), all enums,
 *  std::string and std::complex types are currently accepted as cheaply
 *  copyable.
 **/
template <typename T>
struct IsCheaplyCopyable
      : public std::integral_constant<
              bool,
              // The class is explicitly marked as cheaply copyable
              std::is_base_of<CheaplyCopyable_i, T>::value
                    //
                    || std::is_arithmetic<T>::value
                    //
                    || std::is_enum<T>::value
                    //
                    || std::is_same<std::string, typename std::remove_cv<T>::type>::value
                    //
                    || IsComplexNumber<T>::value> {};
//@}

}  // namespace ctx
