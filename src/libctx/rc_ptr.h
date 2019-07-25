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

  //@{
  /** Default constructor */
  constexpr rc_ptr() noexcept : base_type() {}
  constexpr rc_ptr(std::nullptr_t) : base_type() {}
  //@}

  // Default copy and move constructors
  rc_ptr(const rc_ptr& x) = default;
  rc_ptr(rc_ptr&& x)      = default;

  /** Initialise from unique pointer */
  template <class U, class D>
  rc_ptr(std::unique_ptr<U, D>&& p) : base_type(std::forward<std::unique_ptr<U, D>>(p)) {}

  // TODO The next constructor should be made explicit
  //      to avoid capturing a pointer twice, but for
  //      compatibility it is unfortunately needed like so.
  /** Construct rc_ptr implicitly from pointer */
  rc_ptr(T* ptr) : base_type(ptr) {}

  /** Implictly construct from shared pointer */
  rc_ptr(std::shared_ptr<T> p) : base_type(p) {}

  // Default assignment operators
  rc_ptr& operator=(const rc_ptr& in) = default;
  rc_ptr& operator=(rc_ptr&&) = default;

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
