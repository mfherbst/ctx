//
// Copyright (C) 2017 by the ctx authors
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

// If DISABLE_LIBCTX_COMPATIBILITY is set, this disables
// the generation of the libctx namespace and hence makes the
// code independent of libctx. Then they could theoretically be
// used side-by-side
#ifndef DISABLE_LIBCTX_COMPATIBILITY

namespace ctx {
// Dummy: We need this for the using statement below.
}  // namespace ctx

namespace libctx {
using namespace ctx;
}  // namespace libctx

#endif  // DISABLE_LIBCTX_NAMESPACE
