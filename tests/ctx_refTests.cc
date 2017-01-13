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

#include <catch.hpp>

// TEST_QCHEM_LIBCTX enables the sections for testing Q-Chem's libctx with this test suite
// DISABLE_LIBCTX_COMPATIBILITY disables all compatibility and namespace remapping
//                              from ctx to libctx.
#if defined DISABLE_LIBCTX_COMPATIBILITY and not defined TEST_QCHEM_LIBCTX
#include <ctx/ctx_ref.h>
#else
#include <libctx/ctx_ref.h>
#endif

namespace ctx {
#ifndef DISABLE_LIBCTX_COMPATIBILITY
using namespace libctx;
#endif
namespace tests {

/** Dummy function to enforce conversion to a reference from ctx_ref classes */
template <typename T>
T& toref(ctx_ref<T>& in) {
  return in;
}

TEST_CASE("Test ctx_ref", "[ctx_ref]") {
  root_storage stor;
  context ctx(stor);

  SECTION("Test storing integers") {
#ifdef TEST_QCHEM_LIBCTX
    stor.insert("/six", rc_ptr<int>(new int(6)));
#else
    stor.update("six", 6);
#endif
    ctx.insert("four", rc_ptr<int>(new int(4)));
    ctx.insert("five", rc_ptr<int>(new int(5)));

    ctx_ref<int> six(ctx, "six");
    ctx_ref<int> four(ctx, std::string("four"));
    ctx_ref<int> five(ctx, "five");

    REQUIRE(toref(six) == 6);
    REQUIRE(toref(four) == 4);
    REQUIRE(toref(five) == 5);
  }  // integers

  SECTION("Test storing strings") {
#ifdef TEST_QCHEM_LIBCTX
    stor.insert("/six", rc_ptr<std::string>(new std::string("6")));
#else
    stor.update("six", "6");
#endif
    ctx.insert("four", rc_ptr<std::string>(new std::string("4")));
    ctx.insert("five", rc_ptr<std::string>(new std::string("5")));

    ctx_ref<std::string> six(ctx, "six");
    ctx_ref<std::string> four(ctx, std::string("four"));
    ctx_ref<std::string> five(ctx, "five");

    REQUIRE(toref(six) == "6");
    REQUIRE(toref(four) == "4");
    REQUIRE(toref(five) == "5");
  }  // strings

  SECTION("Test storing doubles") {
#ifdef TEST_QCHEM_LIBCTX
    stor.insert("/six", rc_ptr<double>(new double(6.0)));
#else
    stor.update("six", 6.0);
#endif
    ctx.insert("four", rc_ptr<double>(new double(4.0)));
    ctx.insert("five", rc_ptr<double>(new double(5.0)));

    ctx_ref<double> six(ctx, "six");
    ctx_ref<double> four(ctx, std::string("four"));
    ctx_ref<double> five(ctx, "five");

    REQUIRE(toref(six) == 6.);
    REQUIRE(toref(four) == 4.);
    REQUIRE(toref(five) == 5.);
  }  // doubles

}  // ctx_ref

}  // namespace tests
}  // namespace ctx
