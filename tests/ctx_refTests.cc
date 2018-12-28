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

#include <catch2/catch.hpp>
#include <libctx/ctx_ref.h>

namespace ctx {
using namespace libctx;
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
