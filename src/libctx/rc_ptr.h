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
#include <memory>

namespace libctx {
template <typename T>
class rc_ptr : public std::shared_ptr<T> {
 public:
  typedef std::shared_ptr<T> base_type;

  // Import base constructors:
  using base_type::shared_ptr;

  /** Construct rc_ptr implicitly from pointer */
  rc_ptr(T* ptr = nullptr) : base_type(ptr) {}

  /** Implictly construct from shared pointer */
  rc_ptr(std::shared_ptr<T> p) : base_type(std::move(p)) {}

// We know this is deprecated, but since we need the interface,
// it has to stick for know, unfortunately.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
  template <typename U>
  explicit rc_ptr(std::auto_ptr<U>& p) : base_type(p.release()) {}
#pragma GCC diagnostic pop

  /** A pointer equivalent to nullptr */
  static const std::shared_ptr<T> null_ptr;
};

template <typename T>
const std::shared_ptr<T> rc_ptr<T>::null_ptr{};

template <typename T, typename... Args>
rc_ptr<T> make_rcptr(Args&&... args) {
  return std::make_shared<T>(std::forward<Args>(args)...);
}

}  // namespace libctx
