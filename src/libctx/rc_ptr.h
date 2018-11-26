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
