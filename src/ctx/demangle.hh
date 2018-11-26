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
#include <string>
#include <typeinfo>

namespace ctx {
/** Return the demangled name of the C++ symbol represented by the argument
 * as a std::string */
std::string demangle(const char* mangled);

/** Return the demangled name of the C++ symbol represented by the argument
 * as a std::string */
inline std::string demangle(const std::string& mangled_name) {
  return demangle(mangled_name.c_str());
}

/** Return the demangled typename of the type_info */
inline std::string demangle(const std::type_info& type) { return demangle(type.name()); }

}  // namespace ctx
