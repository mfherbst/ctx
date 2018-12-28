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
