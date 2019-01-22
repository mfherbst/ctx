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
#include "CtxExceptionBase.hh"
#include <exception>
#include <stdexcept>

namespace ctx {

// Macro to import an exception from the std library in a way that
// it is both the exception from the std library and as well derived
// off CtxExceptionBase
#define USING_STD_EXCEPTION(NAME)                           \
  struct NAME : public CtxExceptionBase, public std::NAME { \
    using std::NAME::NAME;                                  \
  }

USING_STD_EXCEPTION(invalid_argument);
USING_STD_EXCEPTION(out_of_range);
USING_STD_EXCEPTION(runtime_error);

struct type_mismatch : public invalid_argument {
  using invalid_argument::invalid_argument;
};

struct internal_error : public runtime_error {
  using runtime_error::runtime_error;
};

struct not_implemented_error : public runtime_error {
  using runtime_error::runtime_error;
};

#undef USING_STD_EXCEPTION

}  // namespace ctx
