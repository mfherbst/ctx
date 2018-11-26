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
#include "context.h"

namespace ctx {

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

}  // namespace ctx
