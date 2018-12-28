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

#include "CtxMapValue.hh"
#include <iomanip>

namespace ctx {

#define IF_TYPE_PRINT(TYPE)                                              \
  if (value.type_name_raw() == std::string(typeid(TYPE).name()) ||       \
      value.type_name_raw() == std::string(typeid(const TYPE).name())) { \
    o << std::setw(10) << std::left << value.get<const TYPE>();          \
  }

/** Try to provide a string representation of the CtxMapValue. If this fails, just print
 * the type */
std::ostream& operator<<(std::ostream& o, const CtxMapValue& value) {

  /* clang-format off */
       IF_TYPE_PRINT(bool)
  else IF_TYPE_PRINT(char)
  else IF_TYPE_PRINT(int)
  else IF_TYPE_PRINT(long)
  else IF_TYPE_PRINT(long long)
  else IF_TYPE_PRINT(unsigned char)
  else IF_TYPE_PRINT(unsigned int)
  else IF_TYPE_PRINT(unsigned long)
  else IF_TYPE_PRINT(unsigned long long)
  else IF_TYPE_PRINT(float)
  else IF_TYPE_PRINT(double)
  else IF_TYPE_PRINT(long double)
  else IF_TYPE_PRINT(std::string)
  else {
    o << std::setw(10) << std::left << "???";
  }
  /* clang-format on */

  o << "  (" << value.type_name() << ")";
  return o;
}

}  // namespace ctx
