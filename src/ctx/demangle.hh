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
