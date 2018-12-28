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
#include "context.h"

namespace libctx {

/** Class representing a reference to an object in the context */
template <typename T>
class ctx_ref {
 public:
  ctx_ref(context& ctx, const std::string& key) : m_ptr(ctx.get<T>(key)) {}

  /** Implicit conversion to const T& */
  operator const T&() const { return *m_ptr; }

  /** Implicit conversion to T& */
  operator T&() { return *m_ptr; }

 private:
  rc_ptr<T> m_ptr;
};

}  // namespace libctx
