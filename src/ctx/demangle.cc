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

#include "demangle.hh"
#include <cstdlib>
#include <cstring>

#ifdef CTX_ENABLE_TYPE_DEMANGLING
#include <cxxabi.h>

namespace ctx {
std::string demangle(const char* mangled) {
  int status;
  char* demangled = abi::__cxa_demangle(mangled, nullptr, nullptr, &status);

  if (status == 0) {
    std::string ret(demangled);
    free(demangled);  // NOLINT
    return ret;
  }
  return std::string(mangled);
}
}  // namespace ctx

#else  // CTX_ENABLE_TYPE_DEMANGLING

namespace ctx {
std::string demangle(const char* mangled) { return std::string(mangled); }
}  // namespace ctx

#endif  // CTX_ENABLE_TYPE_DEMANGLING
