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
